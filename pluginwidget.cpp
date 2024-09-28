#include "pluginwidget.h"
#include "ui_pluginwidget.h"
#include <QStringListModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

PluginWidget::PluginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginWidget)
{
    ui->setupUi(this);

    baseDir = QDir(QCoreApplication::applicationDirPath());
    baseDir.cdUp();
    baseDir.cdUp();

    initializePython();
    loadPlugins("cleaning");
    loadPlugins("visualization");

    connect(ui->cleaningButton, &QPushButton::clicked, this, &PluginWidget::uploadCleaningPlugin);
    connect(ui->visualizationButton, &QPushButton::clicked, this, &PluginWidget::uploadVisualizationPlugin);

    emitVisualizationPluginsUpdated();
}

PluginWidget::~PluginWidget()
{
    unloadPlugins();
    finalizePython();
    delete ui;
}

void PluginWidget::initializePython()
{
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('.')");

    PyObject* sysPath = PySys_GetObject("path");
    PyObject* sitePackagesPath = PyUnicode_DecodeFSDefault(PYTHON_SITE_PACKAGES_PATH);
    PyList_Append(sysPath, sitePackagesPath);
    Py_DECREF(sitePackagesPath);
}

void PluginWidget::finalizePython()
{
    Py_Finalize();
}

void PluginWidget::loadPlugins(const QString& pluginType)
{
    QDir pluginsDir(baseDir.absolutePath() + "/plugins/" + pluginType);
    QStringList pluginList;

    pluginNameToFile.clear();  // Clear existing entries for this plugin type

    foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
        if(fileName.endsWith(".py")) {
            QString filePath = pluginsDir.absoluteFilePath(fileName);
            QString pluginName = getPluginName(filePath, pluginType);
            if (!pluginName.isEmpty()) {
                pluginList.append(pluginName);
                pluginNameToFile[pluginName] = fileName;  // Add to our map
            } else {
                qDebug() << "Invalid plugin found in" << pluginType << "folder:" << fileName;
            }
        }
    }

    QStringListModel *model = new QStringListModel(this);
    model->setStringList(pluginList);
    if (pluginType == "cleaning") {
        ui->cleaningView->setModel(model);
    } else if (pluginType == "visualization") {
        ui->visualizationView->setModel(model);
        emitVisualizationPluginsUpdated();
    }

    // Print the contents of the map for testing
    // qDebug() << "Plugin map for" << pluginType << ":";
    // for (auto it = pluginNameToFile.constBegin(); it != pluginNameToFile.constEnd(); ++it) {
    //     qDebug() << "Plugin Name:" << it.key() << "File Name:" << it.value();
    // }
    // qDebug() << getPluginInputs("Pie Chart", "visualization");
}

QString PluginWidget::getPluginName(const QString& filePath, const QString& pluginType)
{
    QFileInfo fileInfo(filePath);
    QString moduleName = fileInfo.baseName();
    QString dirPath = fileInfo.absolutePath();
    QString functionName = (pluginType == "cleaning") ? "get_technique_name" : "get_visualization_name";

    PyObject* sysPath = PySys_GetObject("path");
    PyObject* pythonDirPath = PyUnicode_FromString(dirPath.toUtf8().constData());
    PyList_Append(sysPath, pythonDirPath);
    Py_DECREF(pythonDirPath);

    PyObject *name = PyUnicode_FromString(moduleName.toUtf8().constData());
    PyObject *module = PyImport_Import(name);
    Py_DECREF(name);

    if (!module) {
        PyErr_Print();
        return QString();
    }

    PyObject *func = PyObject_GetAttrString(module, functionName.toUtf8().constData());
    if (!func || !PyCallable_Check(func)) {
        Py_XDECREF(func);
        Py_DECREF(module);
        return QString();
    }

    PyObject *result = PyObject_CallObject(func, NULL);
    Py_DECREF(func);

    if (!result) {
        PyErr_Print();
        Py_DECREF(module);
        return QString();
    }

    QString pluginName = QString::fromUtf8(PyUnicode_AsUTF8(result));
    Py_DECREF(result);
    Py_DECREF(module);

    return pluginName;
}

void PluginWidget::uploadPlugin(const QString& pluginType)
{
    QDir pluginsDir(baseDir.absolutePath() + "/plugins/" + pluginType);

    QString capitalizedPluginType = pluginType;
    if (!capitalizedPluginType.isEmpty()) {
        capitalizedPluginType[0] = capitalizedPluginType[0].toUpper();
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select %1 Plugin").arg(capitalizedPluginType),
                                                    QDir::homePath(),
                                                    tr("Python Files (*.py)"));

    if (fileName.isEmpty())
        return;

    QFileInfo fileInfo(fileName);
    QString destPath = pluginsDir.absoluteFilePath(fileInfo.fileName());

    // Check if the plugin is of the correct type
    QString tempPluginName = getPluginName(fileName, pluginType);
    if (tempPluginName.isEmpty()) {
        QMessageBox::warning(this, "Error", "The selected file is not a valid " + pluginType + " plugin.");
        return;
    }

    bool fileExists = QFile::exists(destPath);
    if (fileExists) {
        // Check if the files are identical
        if (compareFiles(fileName, destPath)) {
            QMessageBox::information(this, "Info", "This plugin is already installed and up to date.");
            return;
        }

        QMessageBox::StandardButton reply = QMessageBox::question(this, "File Exists",
                                                                  "A file with this name already exists. Do you want to replace it?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
            return;
    }

    if (QFile::copy(fileName, destPath)) {
        QMessageBox::information(this, "Success", "Plugin uploaded successfully.");
        loadPlugins(pluginType);
    } else {
        QMessageBox::warning(this, "Error", "Failed to upload the plugin. Please check file permissions.");
    }
}

// Add this function to your PluginWidget class
bool PluginWidget::compareFiles(const QString &file1, const QString &file2)
{
    QFile f1(file1);
    QFile f2(file2);

    if (!f1.open(QFile::ReadOnly) || !f2.open(QFile::ReadOnly))
        return false;

    while (!f1.atEnd() && !f2.atEnd()) {
        if (f1.read(1024) != f2.read(1024))
            return false;
    }

    return f1.atEnd() && f2.atEnd();
}

void PluginWidget::unloadPlugins()
{
    for(auto it = loadedPlugins.begin(); it != loadedPlugins.end(); ++it) {
        Py_DECREF(it.value());
    }
    loadedPlugins.clear();
}

QAbstractItemModel* PluginWidget::getVisualizationModel() const
{
    return ui->visualizationView->model();
}

void PluginWidget::triggerInitialVisualizationUpdate()
{
    qDebug() << "triggerInitialVisualizationUpdate called";
    emitVisualizationPluginsUpdated();
    qDebug() << "emitVisualizationPluginsUpdated completed";
}

void PluginWidget::emitVisualizationPluginsUpdated()
{
    QStringListModel *model = qobject_cast<QStringListModel*>(ui->visualizationView->model());
    if (model) {
        QStringList pluginList = model->stringList();
        qDebug() << "Emitting visualizationPluginsUpdated with" << pluginList.size() << "plugins";
        emit visualizationPluginsUpdated(pluginList);
    } else {
        qWarning() << "Failed to get model from visualizationView";
    }
}

PluginInputs PluginWidget::getPluginInputs(const QString& pluginName, const QString& pluginType)
{
    PluginInputs inputs;

    if (pluginNameToFile.contains(pluginName))
    {
        QString fileName = pluginNameToFile[pluginName];

        QDir pluginsDir(baseDir.absolutePath() + "/plugins/" + pluginType);
        QString filePath = pluginsDir.absoluteFilePath(fileName);

        if (QFile::exists(filePath))
        {
            QFileInfo fileInfo(filePath);
            QString moduleName = fileInfo.baseName();
            QString dirPath = fileInfo.absolutePath();

            PyObject* sysPath = PySys_GetObject("path");
            PyObject* pythonDirPath = PyUnicode_FromString(dirPath.toUtf8().constData());
            PyList_Append(sysPath, pythonDirPath);
            Py_DECREF(pythonDirPath);

            PyObject *pName = PyUnicode_FromString(moduleName.toUtf8().constData());
            PyObject *pModule = PyImport_Import(pName);
            Py_DECREF(pName);

            if (pModule)
            {
                // Get required_data
                PyObject *pFunc = PyObject_GetAttrString(pModule, "get_required_data");
                if (pFunc && PyCallable_Check(pFunc))
                {
                    PyObject *pValue = PyObject_CallObject(pFunc, NULL);
                    if (pValue && PyList_Check(pValue))
                    {
                        Py_ssize_t size = PyList_Size(pValue);
                        for (Py_ssize_t i = 0; i < size; i++)
                        {
                            PyObject *item = PyList_GetItem(pValue, i);
                            inputs.requiredData << QString::fromUtf8(PyUnicode_AsUTF8(item));
                        }
                    }
                    Py_XDECREF(pValue);
                }
                Py_XDECREF(pFunc);

                // Get parameters
                pFunc = PyObject_GetAttrString(pModule, "get_parameters");
                if (pFunc && PyCallable_Check(pFunc))
                {
                    PyObject *pValue = PyObject_CallObject(pFunc, NULL);
                    if (pValue && PyList_Check(pValue))
                    {
                        Py_ssize_t size = PyList_Size(pValue);
                        for (Py_ssize_t i = 0; i < size; i++)
                        {
                            PyObject *item = PyList_GetItem(pValue, i);
                            inputs.parameters << QString::fromUtf8(PyUnicode_AsUTF8(item));
                        }
                    }
                    Py_XDECREF(pValue);
                }
                Py_XDECREF(pFunc);

                Py_DECREF(pModule);
            }
            else
            {
                PyErr_Print();
                qWarning() << "Failed to load Python module:" << moduleName;
            }
        }
        else
        {
            qWarning() << "Plugin file does not exist:" << filePath;
        }
    }
    else
    {
        qWarning() << "Plugin name not found in map:" << pluginName;
    }

    return inputs;
}

QByteArray PluginWidget::generateVisualization(const QString& pluginName, const QMap<QString, QVector<QVariant>>& data, const QVector<QString>& parameters)
{
    QByteArray imageData;

    if (pluginNameToFile.contains(pluginName))
    {
        QString fileName = pluginNameToFile[pluginName];
        QDir pluginsDir(baseDir.absolutePath() + "/plugins/visualization");
        QString filePath = pluginsDir.absoluteFilePath(fileName);

        if (QFile::exists(filePath))
        {
            QFileInfo fileInfo(filePath);
            QString moduleName = fileInfo.baseName();
            QString dirPath = fileInfo.absolutePath();

            PyObject* sysPath = PySys_GetObject("path");
            PyObject* pythonDirPath = PyUnicode_FromString(dirPath.toUtf8().constData());
            PyList_Append(sysPath, pythonDirPath);
            Py_DECREF(pythonDirPath);

            PyObject *pName = PyUnicode_FromString(moduleName.toUtf8().constData());
            PyObject *pModule = PyImport_Import(pName);
            Py_DECREF(pName);

            if (pModule)
            {
                PyObject *pFunc = PyObject_GetAttrString(pModule, "create_visualization");
                if (pFunc && PyCallable_Check(pFunc))
                {
                    // Convert data to Python dictionary
                    PyObject *pyData = PyDict_New();
                    for (auto it = data.begin(); it != data.end(); ++it) {
                        PyObject *pyList = PyList_New(it.value().size());
                        for (int i = 0; i < it.value().size(); ++i) {
                            PyList_SetItem(pyList, i, toPyObject(it.value()[i]));
                        }
                        PyDict_SetItemString(pyData, it.key().toUtf8().constData(), pyList);
                    }

                    // Convert parameters to Python list
                    PyObject *pyParams = PyList_New(parameters.size());
                    for (int i = 0; i < parameters.size(); ++i) {
                        PyList_SetItem(pyParams, i, PyUnicode_FromString(parameters[i].toUtf8().constData()));
                    }

                    // Call the create_visualization function
                    PyObject *pValue = PyObject_CallFunctionObjArgs(pFunc, pyData, pyParams, NULL);
                    Py_DECREF(pyData);
                    Py_DECREF(pyParams);

                    if (pValue)
                    {
                        // Assume the function returns the image as bytes
                        char *buffer;
                        Py_ssize_t length;
                        if (PyBytes_AsStringAndSize(pValue, &buffer, &length) != -1)
                        {
                            imageData = QByteArray(buffer, length);
                        }
                        Py_DECREF(pValue);
                    }
                    else
                    {
                        PyErr_Print();
                        qWarning() << "Call to create_visualization failed";
                    }
                }
                else
                {
                    if (PyErr_Occurred())
                        PyErr_Print();
                    qWarning() << "Cannot find function create_visualization";
                }
                Py_XDECREF(pFunc);
                Py_DECREF(pModule);
            }
            else
            {
                PyErr_Print();
                qWarning() << "Failed to load Python module:" << moduleName;
            }
        }
        else
        {
            qWarning() << "Plugin file does not exist:" << filePath;
        }
    }
    else
    {
        qWarning() << "Plugin name not found in map:" << pluginName;
    }

    return imageData;
}

PyObject* PluginWidget::toPyObject(const QVariant& var)
{
    switch (var.type()) {
    case QVariant::Int:
        return PyLong_FromLong(var.toInt());
    case QVariant::Double:
        return PyFloat_FromDouble(var.toDouble());
    case QVariant::String:
        return PyUnicode_FromString(var.toString().toUtf8().constData());
    default:
        Py_RETURN_NONE;
    }
}

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

    foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
        if(fileName.endsWith(".py")) {
            QString filePath = pluginsDir.absoluteFilePath(fileName);
            QString pluginName = getPluginName(filePath, pluginType);
            if (!pluginName.isEmpty()) {
                pluginList.append(pluginName);
            } else {
                qDebug() << "Invalid plugin found in" << pluginType << "folder:" << fileName;
                // Optionally, you could move or delete invalid plugins here
            }
        }
    }

    QStringListModel *model = new QStringListModel(this);
    model->setStringList(pluginList);
    if (pluginType == "cleaning") {
        ui->cleaningView->setModel(model);
    } else if (pluginType == "visualization") {
        ui->visualizationView->setModel(model);
    }
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

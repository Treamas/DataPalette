#ifndef PLUGINWIDGET_H
#define PLUGINWIDGET_H

#include <QWidget>
#include <QDir>
#include <QMap>
#include <QAbstractItemModel>

#ifdef slots
#undef slots
#include <Python.h>
#define slots Q_SLOTS
#else
#include <Python.h>
#endif

namespace Ui {
class PluginWidget;
}

struct PluginInputs {
    QStringList requiredData;
    QStringList parameters;
};

class PluginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PluginWidget(QWidget *parent = nullptr);
    ~PluginWidget();

    QAbstractItemModel* getVisualizationModel() const;
    void triggerInitialVisualizationUpdate();
    QMap<QString, QString> getPluginMap() const { return pluginNameToFile; }
    QMap<QString, QString> pluginNameToFile;
    PluginInputs getPluginInputs(const QString& pluginName, const QString& pluginType);
    QByteArray generateVisualization(const QString& pluginName, const QMap<QString, QVector<QVariant>>& data, const QVector<QString>& parameters);

private slots:
    void uploadCleaningPlugin() { uploadPlugin("cleaning"); }
    void uploadVisualizationPlugin() { uploadPlugin("visualization"); }

signals:
    void visualizationPluginsUpdated(const QStringList &pluginNames);

private:
    void initializePython();
    void finalizePython();
    void loadPlugins(const QString& pluginType);
    QString getPluginName(const QString& filePath, const QString& pluginType);
    void uploadPlugin(const QString& pluginType);
    bool compareFiles(const QString &file1, const QString &file2);
    void unloadPlugins();
    void emitVisualizationPluginsUpdated();

    Ui::PluginWidget *ui;
    QDir baseDir;
    QMap<QString, PyObject*> loadedPlugins;

    void clearOptionsBox();
    void populateRequiredDataUI(const QStringList& requiredData);
    void populateParametersUI(const QStringList& parameters);

    PyObject* toPyObject(const QVariant& var);
};

#endif // PLUGINWIDGET_H

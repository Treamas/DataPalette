#ifndef PLUGINWIDGET_H
#define PLUGINWIDGET_H

#include <QWidget>
#include <QDir>
#include <QMap>

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

class PluginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PluginWidget(QWidget *parent = nullptr);
    ~PluginWidget();

private:
    Ui::PluginWidget *ui;
    QMap<QString, PyObject*> loadedPlugins;
    QDir baseDir;

    void initializePython();
    void finalizePython();
    void loadPlugins(const QString& pluginType);
    void uploadPlugin(const QString& pluginType);
    void unloadPlugins();
    QString getPluginName(const QString& filePath, const QString& pluginType);
    bool compareFiles(const QString &file1, const QString &file2);

private slots:
    void uploadCleaningPlugin() { uploadPlugin("cleaning"); }
    void uploadVisualizationPlugin() { uploadPlugin("visualization"); }
};

#endif // PLUGINWIDGET_H

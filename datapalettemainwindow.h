#ifndef DATAPALETTEMAINWINDOW_H
#define DATAPALETTEMAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include "homewidget.h"
#include "datawidget.h"
#include "cleaningwidget.h"
#include "visualizationwidget.h"
#include "pluginwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class DataPaletteMainWindow;
}
QT_END_NAMESPACE

class DataPaletteMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    DataPaletteMainWindow(QWidget *parent = nullptr);
    ~DataPaletteMainWindow();

private slots:
    void on_homeButton_clicked();
    void on_dataButton_clicked();
    void on_cleaningButton_clicked();
    void on_visualizationButton_clicked();
    void on_pluginButton_clicked();

private:
    Ui::DataPaletteMainWindow *ui;
    HomeWidget *homeWidget;
    DataWidget *dataWidget;
    CleaningWidget *cleaningWidget;
    VisualizationWidget *visualizationWidget;
    PluginWidget *pluginWidget;
};
#endif // DATAPALETTEMAINWINDOW_H

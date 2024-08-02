#ifndef DATAPALETTEMAINWINDOW_H
#define DATAPALETTEMAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>

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
    void on_dbConnectionButton_clicked();

private:
    Ui::DataPaletteMainWindow *ui;
    QSqlDatabase db;
};
#endif // DATAPALETTEMAINWINDOW_H

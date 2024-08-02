#ifndef DATAPALETTEMAINWINDOW_H
#define DATAPALETTEMAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::DataPaletteMainWindow *ui;
};
#endif // DATAPALETTEMAINWINDOW_H

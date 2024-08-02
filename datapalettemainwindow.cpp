#include "datapalettemainwindow.h"
#include "./ui_datapalettemainwindow.h"

DataPaletteMainWindow::DataPaletteMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DataPaletteMainWindow)
{
    ui->setupUi(this);
}

DataPaletteMainWindow::~DataPaletteMainWindow()
{
    delete ui;
}

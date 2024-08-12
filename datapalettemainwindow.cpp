#include "datapalettemainwindow.h"
#include "./ui_datapalettemainwindow.h"

DataPaletteMainWindow::DataPaletteMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DataPaletteMainWindow)
    , homeWidget(new HomeWidget)
    , dataWidget(new DataWidget)
    , cleaningWidget(new CleaningWidget)
    , visualizationWidget(new VisualizationWidget)
    , pluginWidget(new PluginWidget)
{
    ui->setupUi(this);
    ui->stackedWidget->addWidget(homeWidget);
    ui->stackedWidget->addWidget(dataWidget);
    ui->stackedWidget->addWidget(cleaningWidget);
    ui->stackedWidget->addWidget(visualizationWidget);
    ui->stackedWidget->addWidget(pluginWidget);

    connect(homeWidget, &HomeWidget::databaseConnectionChanged, dataWidget, &DataWidget::populateTableList);
}

DataPaletteMainWindow::~DataPaletteMainWindow()
{
    delete ui;
}

void DataPaletteMainWindow::on_homeButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(homeWidget);
}

void DataPaletteMainWindow::on_dataButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(dataWidget);
}

void DataPaletteMainWindow::on_cleaningButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(cleaningWidget);
}

void DataPaletteMainWindow::on_visualizationButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(visualizationWidget);
}

void DataPaletteMainWindow::on_pluginButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(pluginWidget);
}


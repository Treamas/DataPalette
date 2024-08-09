#include "cleaningwidget.h"
#include "ui_cleaningwidget.h"

CleaningWidget::CleaningWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CleaningWidget)
{
    ui->setupUi(this);
}

CleaningWidget::~CleaningWidget()
{
    delete ui;
}

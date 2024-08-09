#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include <QtSql/QSqlDatabase>

namespace Ui {
class HomeWidget;
}

class HomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);
    ~HomeWidget();

private slots:
    void on_dbConnectionButton_clicked();

private:
    Ui::HomeWidget *ui;
    QSqlDatabase db;
};

#endif // HOMEWIDGET_H

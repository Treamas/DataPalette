#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

namespace Ui {
class HomeWidget;
}

class HomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);
    ~HomeWidget();

signals:
    void databaseConnectionChanged(bool successful);

private slots:
    void on_dbConnectionButton_clicked();

private:
    Ui::HomeWidget *ui;
    bool isConnecting;
    QLabel *statusLabel;
    QTimer *statusTimer;

    void showNotification(const QString &message, bool success);
};

#endif // HOMEWIDGET_H

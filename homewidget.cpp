#include "homewidget.h"
#include "ui_homewidget.h"
#include "databasemanager.h"

#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

HomeWidget::HomeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomeWidget)
    , isConnecting(false)
    , statusTimer(new QTimer(this))
{
    ui->setupUi(this);

    // Set up the notification timer
    statusTimer->setInterval(2000); // 2 seconds
    connect(statusTimer, &QTimer::timeout, [this]() {
        // Fade out the notification
        QGraphicsOpacityEffect *fadeEffect = new QGraphicsOpacityEffect(this);
        ui->statusLabel->setGraphicsEffect(fadeEffect);
        QPropertyAnimation *animation = new QPropertyAnimation(fadeEffect, "opacity");
        animation->setDuration(1000);
        animation->setStartValue(1.0);
        animation->setEndValue(0.0);
        connect(animation, &QPropertyAnimation::finished, ui->statusLabel, &QLabel::hide);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
    });
}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::on_dbConnectionButton_clicked()
{
    if (isConnecting) {
        return;
    }

    isConnecting = true;

    QString server = ui->serverLinedEdit->text();
    QString database = ui->dbLineEdit->text();
    QString username = ui->userLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    DatabaseManager& dbManager = DatabaseManager::getInstance();

    if(dbManager.isConnected() == true) dbManager.closeConnection(); //Close existing connection

    bool success = dbManager.connectToDatabase(server, database, username, password);

    if (success) {
        showNotification("Database connected successfully", true);
    } else {
        showNotification("Database connection failed", false);
    }

    emit databaseConnectionChanged(success);

    isConnecting = false;
}

void HomeWidget::showNotification(const QString &message, bool success)
{
    ui->statusLabel->setText(message);
    ui->statusLabel->setStyleSheet(success ?
                                         "QLabel { background-color : green; color : white; padding: 5px; border-radius: 5px; }" :
                                         "QLabel { background-color : red; color : white; padding: 5px; border-radius: 5px; }");

    ui->statusLabel->show();
    statusTimer->start();
}


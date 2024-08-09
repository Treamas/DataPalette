#include "homewidget.h"
#include "ui_homewidget.h"
#include <QMessageBox>
#include <QtSql/QSqlError>

HomeWidget::HomeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomeWidget)
{
    ui->setupUi(this);
    connect(ui->dbConnectionButton, &QPushButton::clicked, this, &HomeWidget::on_dbConnectionButton_clicked);
}

HomeWidget::~HomeWidget()
{
    if (db.isOpen()) {
        db.close();
    }
    delete ui;
}

void HomeWidget::on_dbConnectionButton_clicked()
{
    // Close any existing connection
    QString connectionName;
    {
        QSqlDatabase existingDb = QSqlDatabase::database();
        if (existingDb.isValid()) {
            connectionName = existingDb.connectionName();
            existingDb.close();
        }
    }
    QSqlDatabase::removeDatabase(connectionName);

    // Create a new connection
    QString server = ui->serverLinedEdit->text();
    QString database = ui->dbLineEdit->text();
    QString username = ui->userLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(QString("Driver={ODBC Driver 17 for SQL Server};Server=%1;Database=%2;Uid=%3;Pwd=%4;")
                           .arg(server, database, username, password));

    if (db.open()) {
        QMessageBox::information(this, "Connection", "Database connected successfully");
    } else {
        QMessageBox::critical(this, "Connection", "Database connection failed: " + db.lastError().text());
    }
}

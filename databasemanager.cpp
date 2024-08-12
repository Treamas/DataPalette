#include "databasemanager.h"

#include <QSqlError>
#include <QUuid>
#include <QSqlQuery>

DatabaseManager& DatabaseManager::getInstance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() : connectionName(QUuid::createUuid().toString()), connected(false) {}

DatabaseManager::~DatabaseManager()
{
    closeConnection();
}

bool DatabaseManager::connectToDatabase(const QString &server, const QString &database, const QString &username, const QString &password)
{
    if (connected) {
        closeConnection();
    }

    db = QSqlDatabase::addDatabase("QODBC", connectionName);
    db.setDatabaseName(QString("Driver={ODBC Driver 17 for SQL Server};Server=%1;Database=%2;Uid=%3;Pwd=%4;")
                           .arg(server, database, username, password));

    connected = db.open();
    if (connected) {
        qDebug() << "Database connected successfully";
    } else {
        qDebug() << "Database connection failed:" << db.lastError().text();
    }
    return connected;
}

QSqlDatabase DatabaseManager::getDatabase()
{
    return QSqlDatabase::database(connectionName);
}

bool DatabaseManager::isConnected() const
{
    return connected;
}

void DatabaseManager::closeConnection()
{
    if (connected) {
        db.close();
    }
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }
    connected = false;
}

QStringList DatabaseManager::getUserTables()
{
    QStringList tables;

    if (connected) {
        QSqlQuery query(db);
        query.exec("SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_TYPE = 'BASE TABLE' AND TABLE_SCHEMA = 'dbo'");

        while (query.next()) {
            tables << query.value(0).toString();
        }
    }

    return tables;
}

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager
{
public:
    static DatabaseManager& getInstance();
    bool connectToDatabase(const QString &server, const QString &database, const QString &username, const QString &password);
    QSqlDatabase getDatabase();
    bool isConnected() const;
    void closeConnection();
    QStringList getUserTables();

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase db;
    QString connectionName;
    bool connected;
};

#endif // DATABASEMANAGER_H

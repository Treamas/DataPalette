#include "datapalettemainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "DataPalette_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // Create a QSqlDatabase object and set the driver to "QODBC"
    // QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    // // Set the database connection parameters
    // db.setDatabaseName("Driver={ODBC Driver 17 for SQL Server};Server=DESKTOP-PSOLF65;Database=ArtefactTestDB;Uid=read;Pwd=cread;");

    // // Open the database connection
    // if (!db.open()) {
    //     qDebug() << "Error: Unable to connect to the database.";
    //     qDebug() << "Details:" << db.lastError().text();
    //     return -1;
    // }

    // // Perform database operations
    // QSqlQuery query;
    // if (!query.exec("SELECT * FROM Technology")) {
    //     qDebug() << "Query Error:" << query.lastError().text();
    // } else {
    //     while (query.next()) {
    //         qDebug() << query.value(0).toString();  // Replace with your own processing
    //     }
    // }



    DataPaletteMainWindow w;
    w.show();
    return a.exec();
}

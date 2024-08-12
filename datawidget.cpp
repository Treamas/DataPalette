#include "datawidget.h"
#include "ui_datawidget.h"
#include "databasemanager.h"

#include <QStringListModel>
#include <QSqlQuery>
#include <QSqlQueryModel>

DataWidget::DataWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataWidget)
{
    ui->setupUi(this);
}

DataWidget::~DataWidget()
{
    delete ui;
}

void DataWidget::populateTableList()
{
    DatabaseManager& dbManager = DatabaseManager::getInstance();
    if (dbManager.isConnected()) {
        QStringList tables = dbManager.getUserTables();
        QStringListModel *model = new QStringListModel(this);
        model->setStringList(tables);
        ui->tablesList->setModel(model);

        connect(ui->tablesList->selectionModel(), &QItemSelectionModel::currentChanged, this, &DataWidget::onTableSelected);
    }
}

void DataWidget::onTableSelected(const QModelIndex &index)
{
    if (!index.isValid()) return;

    QString tableName = index.data().toString();
    DatabaseManager& dbManager = DatabaseManager::getInstance();

    if (dbManager.isConnected()) {
        QSqlQuery query(dbManager.getDatabase());
        if (query.exec("SELECT * FROM " + tableName)) {
            QSqlQueryModel *model = new QSqlQueryModel(this);
            model->setQuery(std::move(query));
            ui->dataView->setModel(model);
            ui->dataView->resizeColumnsToContents();
        }
    }
}

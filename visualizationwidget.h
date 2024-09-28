#ifndef VISUALIZATIONWIDGET_H
#define VISUALIZATIONWIDGET_H

#include "databasemanager.h"
#include <QWidget>
#include <QListView>
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>

// Forward declaration
class PluginWidget;

namespace Ui {
class VisualizationWidget;
}

class VisualizationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VisualizationWidget(QWidget *parent = nullptr);
    ~VisualizationWidget();
    void setPluginModel(QAbstractItemModel* model);
    void setPluginWidget(PluginWidget* pluginWidget);

    void setAvailableTables(const QStringList& tables);
    void setColumnsForTable(const QString& table, const QStringList& columns);

    void updateAvailableTables();

public slots:
    void updateVisualizationPlugins(const QStringList &pluginNames);
    void onPluginSelected(const QModelIndex &index);

private:
    Ui::VisualizationWidget *ui;
    QStringListModel *pluginModel;
    PluginWidget *pluginWidget;

    void clearOptionsBox();
    void populateRequiredDataUI(const QStringList& requiredData);
    void populateParametersUI(const QStringList& parameters);

    QGroupBox* optionsBox;
    QMap<QString, QComboBox*> tableComboBoxes;
    QMap<QString, QComboBox*> columnComboBoxes;
    QMap<QString, QLineEdit*> parameterLineEdits;

    QStringList availableTables;
    QMap<QString, QStringList> tableColumns;

    void updateColumnsForTable(const QString& tableName);

    DatabaseManager& dbManager;

    void onGenerateButtonClicked();
};

#endif // VISUALIZATIONWIDGET_H

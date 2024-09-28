#include "visualizationwidget.h"
#include "ui_visualizationwidget.h"
#include "pluginwidget.h"
#include "databasemanager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>

VisualizationWidget::VisualizationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VisualizationWidget)
    , pluginModel(new QStringListModel(this))
    , dbManager(DatabaseManager::getInstance())
{
    ui->setupUi(this);

    ui->pluginView->setModel(pluginModel);

    connect(ui->pluginView, &QListView::clicked, this, &VisualizationWidget::onPluginSelected);

    optionsBox = ui->optionsBox;
}

VisualizationWidget::~VisualizationWidget()
{
    delete ui;
}

void VisualizationWidget::updateVisualizationPlugins(const QStringList &pluginNames)
{
    if (!pluginModel) {
        qCritical() << "pluginModel is null in updateVisualizationPlugins";
        return;
    }

    try {
        pluginModel->setStringList(pluginNames);
        qDebug() << "Successfully updated plugin list with" << pluginNames.size() << "plugins";
    } catch (const std::exception& e) {
        qCritical() << "Exception caught while updating visualization plugins:" << e.what();
        // You might want to show a message box to the user here
        QMessageBox::warning(this, "Warning", "Failed to update visualization plugins: " + QString(e.what()));
    } catch (...) {
        qCritical() << "Unknown exception caught while updating visualization plugins";
        QMessageBox::warning(this, "Warning", "An unknown error occurred while updating visualization plugins");
    }
}

void VisualizationWidget::setPluginModel(QAbstractItemModel* model)
{
    if (QStringListModel* stringListModel = qobject_cast<QStringListModel*>(model)) {
        delete pluginModel;
        pluginModel = stringListModel;
        ui->pluginView->setModel(pluginModel);
    } else {
        qWarning() << "Attempted to set non-QStringListModel as plugin model";
    }
}

void VisualizationWidget::setPluginWidget(PluginWidget* widget)
{
    pluginWidget = widget;
}

void VisualizationWidget::onPluginSelected(const QModelIndex &index)
{
    if (!pluginWidget) {
        qWarning() << "PluginWidget is not set";
        return;
    }

    QString pluginName = index.data().toString();
    qDebug() << "Selected plugin:" << pluginName;

    PluginInputs inputs = pluginWidget->getPluginInputs(pluginName, "visualization");

    if (!inputs.requiredData.isEmpty() || !inputs.parameters.isEmpty()) {
        clearOptionsBox();
        populateRequiredDataUI(inputs.requiredData);
        populateParametersUI(inputs.parameters);

        // Create the "Apply" button
        QPushButton* generateButton = new QPushButton("Generate", optionsBox);
        optionsBox->layout()->addWidget(generateButton);
        connect(generateButton, &QPushButton::clicked, this, &VisualizationWidget::onGenerateButtonClicked);
    } else {
        qWarning() << "No inputs found for plugin:" << pluginName;
        QMessageBox::warning(this, "Plugin Inputs", "No inputs found for the selected plugin.");
    }
}

void VisualizationWidget::onGenerateButtonClicked()
{
    if (!pluginWidget) {
        QMessageBox::warning(this, "Error", "Plugin widget is not set.");
        return;
    }

    QString pluginName = ui->pluginView->currentIndex().data().toString();
    QMap<QString, QVector<QVariant>> data;
    QVector<QString> parameters;

    // Collect data from selected columns
    for (const auto& item : tableComboBoxes.keys()) {
        QString tableName = tableComboBoxes[item]->currentText();
        QString columnName = columnComboBoxes[item]->currentText();

        QVector<QVariant> columnData = dbManager.getColumnData(tableName, columnName);
        if (columnData.isEmpty()) {
            QMessageBox::warning(this, "Error", QString("Failed to fetch data for %1 from %2").arg(columnName, tableName));
            return;
        }

        data[item] = columnData;
    }

    // Collect parameters
    for (const auto& param : parameterLineEdits.keys()) {
        parameters.append(parameterLineEdits[param]->text());
    }

    // Generate the visualization
    QByteArray imageData = pluginWidget->generateVisualization(pluginName, data, parameters);

    // Error Checking
    // if (imageData.isEmpty()) {
    //     QMessageBox::warning(this, "Error", "Failed to generate visualization.");
    //     return;
    // }
}

void VisualizationWidget::clearOptionsBox()
{
    if (!optionsBox) {
        qDebug() << "optionsBox is null!";
        return;
    }

    // Remove and delete all child widgets
    QList<QWidget*> childWidgets = optionsBox->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget* widget : childWidgets) {
        optionsBox->layout()->removeWidget(widget);
        widget->setParent(nullptr);
        widget->deleteLater();
    }

    // Clear the internal collections
    tableComboBoxes.clear();
    columnComboBoxes.clear();
    parameterLineEdits.clear();

    // Reset the layout
    delete optionsBox->layout();
    optionsBox->setLayout(new QVBoxLayout(optionsBox));

    // Force update
    optionsBox->updateGeometry();
    optionsBox->update();
}

void VisualizationWidget::updateAvailableTables()
{
    availableTables = dbManager.getUserTables();
    qDebug() << "Available Tables: " << availableTables;
}

void VisualizationWidget::updateColumnsForTable(const QString& tableName)
{
    tableColumns[tableName] = dbManager.getTableColumns(tableName);
}

void VisualizationWidget::populateRequiredDataUI(const QStringList& requiredData)
{
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(optionsBox->layout());
    if (!mainLayout) {
        mainLayout = new QVBoxLayout(optionsBox);
    }

    for (const QString& item : requiredData) {
        availableTables = dbManager.getUserTables();

        QHBoxLayout* rowLayout = new QHBoxLayout();

        QLabel* label = new QLabel(item, optionsBox);
        rowLayout->addWidget(label);

        QComboBox* tableCombo = new QComboBox(optionsBox);
        tableCombo->addItems(availableTables);
        rowLayout->addWidget(tableCombo);
        tableComboBoxes[item] = tableCombo;

        QComboBox* columnCombo = new QComboBox(optionsBox);
        rowLayout->addWidget(columnCombo);
        columnComboBoxes[item] = columnCombo;

        mainLayout->addLayout(rowLayout);

        connect(tableCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [this, item](int index) {
                    QString selectedTable = tableComboBoxes[item]->itemText(index);
                    updateColumnsForTable(selectedTable);
                    columnComboBoxes[item]->clear();
                    columnComboBoxes[item]->addItems(tableColumns[selectedTable]);
                });
    }
}

void VisualizationWidget::populateParametersUI(const QStringList& parameters)
{
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(optionsBox->layout());
    if (!mainLayout) {
        mainLayout = new QVBoxLayout(optionsBox);
    }

    for (const QString& param : parameters) {
        QHBoxLayout* rowLayout = new QHBoxLayout();

        QLabel* label = new QLabel(param, optionsBox);
        rowLayout->addWidget(label);

        QLineEdit* lineEdit = new QLineEdit(optionsBox);
        rowLayout->addWidget(lineEdit);
        parameterLineEdits[param] = lineEdit;

        mainLayout->addLayout(rowLayout);
    }
}

void VisualizationWidget::setAvailableTables(const QStringList& tables)
{
    availableTables = tables;
}

void VisualizationWidget::setColumnsForTable(const QString& table, const QStringList& columns)
{
    tableColumns[table] = columns;
}


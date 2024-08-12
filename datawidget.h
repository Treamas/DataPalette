#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>

namespace Ui {
class DataWidget;
}

class DataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataWidget(QWidget *parent = nullptr);
    ~DataWidget();

public slots:
    void populateTableList();
    void onTableSelected(const QModelIndex &index);

private:
    Ui::DataWidget *ui;
};

#endif // DATAWIDGET_H

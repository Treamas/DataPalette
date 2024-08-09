#ifndef CLEANINGWIDGET_H
#define CLEANINGWIDGET_H

#include <QWidget>

namespace Ui {
class CleaningWidget;
}

class CleaningWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CleaningWidget(QWidget *parent = nullptr);
    ~CleaningWidget();

private:
    Ui::CleaningWidget *ui;
};

#endif // CLEANINGWIDGET_H

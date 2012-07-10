#ifndef TABLETABWIDGET_H
#define TABLETABWIDGET_H

#include <QTabWidget>

namespace Ui {
class TableTabWidget;
}

class TableTabWidget : public QTabWidget
{
    Q_OBJECT
    
public:
    explicit TableTabWidget(QWidget *parent = 0);
    ~TableTabWidget();
    
private:
    Ui::TableTabWidget *ui;
};

#endif // TABLETABWIDGET_H

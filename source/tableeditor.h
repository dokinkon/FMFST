#ifndef TABLEEDITOR_H
#define TABLEEDITOR_H

#include <QDockWidget>

namespace Ui {
class TableEditor;
}

class TableEditor : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit TableEditor(QWidget *parent = 0);
    ~TableEditor();
    
private:
    Ui::TableEditor *ui;
    struct Private;
    Private* mPrivate;
};

#endif // TABLEEDITOR_H

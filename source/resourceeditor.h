#ifndef RESOURCEEDITOR_H
#define RESOURCEEDITOR_H

#include <QWidget>

namespace Ui {
class ResourceEditor;
}

class ResourceEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit ResourceEditor(QWidget *parent = 0);
    ~ResourceEditor();
    
private:
    Ui::ResourceEditor *ui;
    struct Private;
    Private* mPrivate;

private slots:
    void addButtonClicked();
    void editButtonClicked();
    void deleteButtonClicked();
};

#endif // RESOURCEEDITOR_H

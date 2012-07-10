#ifndef NODEEDITOR_H
#define NODEEDITOR_H

#include <QWidget>
#include "node.h"

namespace Ui {
class NodeEditor;
}

class NodeEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit NodeEditor(QWidget *parent = 0);
    ~NodeEditor();
    QVector<Node> nodes() const;
    bool hasProgram(const QString&) const;
    bool hasFile(const QString&) const;

protected:
    void closeEvent(QCloseEvent*);
    
private:
    Ui::NodeEditor *ui;
    struct Private;
    Private* mPrivate;
private slots:
    void updateButtonClicked();
    void slotCellChanged(int row, int col);
};

NodeEditor& GetNodeEditor();
NodeEditor* GetNodeEditorPtr();

#endif // NODEEDITOR_H

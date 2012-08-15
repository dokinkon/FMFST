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
    void initialize();
    Node createNode(const QString& name, const QString& fa, const QString& pa);
    void destroyNodes(const QVector<Node>& nodes);
    QVector<Node> nodes() const;
    bool hasProgram(const QString&) const;
    bool hasProgram(const QSet<QString>&) const;
    bool hasFile(const QString&) const;
    Node getNode(int nodeIndex) const;

    QSet<int> getNecessaryNodes() const;

protected:
    void closeEvent(QCloseEvent*);
    
private:
    Ui::NodeEditor *ui;
    struct Private;
    Private* mPrivate;
private slots:
    void updateButtonClicked();
};

NodeEditor& GetNodeEditor();
NodeEditor* GetNodeEditorPtr();

#endif // NODEEDITOR_H

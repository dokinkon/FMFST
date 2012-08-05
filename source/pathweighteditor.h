#ifndef PATHWEIGHTEDITOR_H
#define PATHWEIGHTEDITOR_H

#include <QWidget>
#include <QVector>

namespace Ui {
class PathWeightEditor;
}

class Edge;

/*!
 * TODO: rename to edge manager
 */
class PathWeightEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit PathWeightEditor(QWidget *parent = 0);
    ~PathWeightEditor();
    void initialize();
    void serialize();
    void updateNodeData();
    /*!
     *
     */
    bool hasEdge(int, int) const;
    /*!
     *
     */
    void createEdge(int, int);
    /*!
     *
     */
    void destroyConnection(int nodeId1, int nodeId2);
    /*!
     *
     */
    void destroyConnectionRefToNode(int);

    QVector<float> pathWeights() const;
    /*!
     *
     */
    QVector<Edge> getEdges() const;

protected:
    void closeEvent(QCloseEvent* e);
    
private:
    Ui::PathWeightEditor *ui;
    struct Private;
    Private* mPrivate;

private slots:
    void updateButtonClicked();
};

PathWeightEditor& GetPathWeightEditor();

#endif // PATHWEIGHTEDITOR_H

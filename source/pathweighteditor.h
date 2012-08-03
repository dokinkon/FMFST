#ifndef PATHWEIGHTEDITOR_H
#define PATHWEIGHTEDITOR_H

#include <QWidget>
#include <QVector>

namespace Ui {
class PathWeightEditor;
}

/*!
 * TODO: rename to edge manager
 */
class PathWeightEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit PathWeightEditor(QWidget *parent = 0);
    ~PathWeightEditor();
    /*!
     *
     */
    bool hasEdge(int, int) const;

    /*!
     *
     */
    void createEdge(int, int);

    QVector<float> pathWeights() const;
    
private:
    Ui::PathWeightEditor *ui;
    struct Private;
    Private* mPrivate;

private slots:
    void updateButtonClicked();
    void slotCellChanged(int, int);
};

PathWeightEditor& GetPathWeightEditor();

#endif // PATHWEIGHTEDITOR_H

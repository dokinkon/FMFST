#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QGraphicsView>

class CanvasWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit CanvasWidget(QWidget *parent = 0);
    ~CanvasWidget();
    
protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void keyReleaseEvent(QKeyEvent*);
signals:
    
private slots:
    void slotCreateNode();
    void slotDestroyNodes();
    void slotCreateConnection();
    void slotDestroyConnections();

private:
    struct Private;
    Private* mPrivate;
    
};

#endif // CANVASWIDGET_H

#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWebView>

class MapWidget : public QWebView
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = 0);
    void setSize(const QSize&);
    
signals:
    
public slots:
private:
    QSize mLastSize;

    
};

#endif // MAPWIDGET_H

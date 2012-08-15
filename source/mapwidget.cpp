#include "mapwidget.h"
#include <QtCore>

MapWidget::MapWidget(QWidget *parent) :
    QWebView(parent)
{
    QFile f(":defaults/resource/mapview.html");
    if (f.open(QIODevice::ReadOnly)) {
        setHtml(f.readAll());
    }
    resize(1024, 768);
}

void MapWidget::setSize(const QSize& size)
{
    if (mLastSize==size)
        return;

    QString embeddedMap = QString("<iframe src=\"http://www.dr2ooo.com/tools/maps/maps.php?zoom=13&width=%1&height=%2&ll=24.220667,120.684299&ctrl=true&\" width=\"%1\" height=\"%2\"></iframe>").arg(size.width()).arg(size.height());
    //QString embeddedMap = QString("<div id=\"map_canvas_custom_13869\" style=\"width:%1px; height:%2px\" ></div><script type=\"text/javascript\">(function(d, t) {var g = d.createElement(t),s = d.getElementsByTagName(t)[0];g.src = \"http://www.map-generator.net/en/maps/13869.js\";s.parentNode.insertBefore(g, s);}(document, \"script\"));</script><a class=\"mapgen-link\" style=\"font:8px Arial;text-decoration:none;color:#5C5C5C;text-align: right; display: block; width: %1px;\" href=\"http://www.map-generator.net/en\">www.map-generator.net</a>").arg(size.width()).arg(size.height());
    QString htmlContent = QString("<!DOCTYPE html><html><body>%1</body></html>").arg(embeddedMap);
    setHtml(htmlContent);
    resize(size.width(), size.height());
    mLastSize = size;
}


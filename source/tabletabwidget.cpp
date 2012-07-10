#include "tabletabwidget.h"
//#include "nodeeditwidget.h"
#include "resourceeditor.h"
#include "pathweighteditor.h"
#include "ui_tabletabwidget.h"

TableTabWidget::TableTabWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TableTabWidget)
{
    ui->setupUi(this);
    //addTab(new NodeEditWidget(this), "Node Name");
    addTab(new ResourceEditor(this), "Resources");
    addTab(new PathWeightEditor(this), "Path Weight");
}

TableTabWidget::~TableTabWidget()
{
    delete ui;
}

#include "tableeditor.h"
#include "nodeeditor.h"
#include "resourceeditor.h"
#include "pathweighteditor.h"
//#include "tabletabwidget.h"
#include "ui_tableeditor.h"


struct TableEditor::Private
{
    void initContents(QDockWidget*);
};

void TableEditor::Private::initContents(QDockWidget* w)
{
    if (!w)
        return;

    QTabWidget* tab = new QTabWidget(w);
    tab->addTab(new NodeEditor, "Nodes");
    tab->addTab(new ResourceEditor, "Resources");
    tab->addTab(new PathWeightEditor, "Path Weight");
    w->setWidget(tab);
}

TableEditor::TableEditor(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::TableEditor),
    mPrivate(new Private)
{
    ui->setupUi(this);
    mPrivate->initContents(this);
    //TableTabWidget* w = new TableTabWidget(this);
    //setWidget(w);
}

TableEditor::~TableEditor()
{
    delete ui;
}

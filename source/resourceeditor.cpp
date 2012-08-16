#include "resourceeditor.h"
#include "ui_resourceeditor.h"
#include <QtGui>
#include <cassert>

namespace {

const static int ColResourceNo      = 0;
const static int ColResourceName    = 1;

}

struct ResourceEditor::Private
{
    bool serialize();
    bool deserialize();
    bool serializeToText();
    bool deserializeFromText();
    void commitResourceData(QTableWidget*);
    void updateResourceData(QTableWidget*);
    QStringList mResources;
};

// ui to data model
void ResourceEditor::Private::commitResourceData(QTableWidget* w)
{
    if (!w)
        return;

    mResources.clear();
    for (int i=0;i<w->rowCount();++i) {
        QTableWidgetItem* item = w->item(i, 1);
        assert(item);
        mResources << item->text();
    }
    serialize();
}

// from data model to ui
void ResourceEditor::Private::updateResourceData(QTableWidget* w)
{
    if (!w)
        return;

    if (!deserialize())
        return;

    for (int i=0;i<mResources.size();++i) {
        w->insertRow(i);
        QTableWidgetItem* item = NULL;

        item = new QTableWidgetItem(QString("F%1").arg(i+1));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        w->setItem(i, ColResourceNo, item);

        item = new QTableWidgetItem(mResources[i]);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        w->setItem(i, ColResourceName, item);
    }
}

bool ResourceEditor::Private::serializeToText()
{
    QFile f("resource.dat");
    if (!f.open(QIODevice::WriteOnly)) {
        qDebug() << "[RESOURCE-EDITOR]: FAILED TO OPEN resource.dat";
        return false;
    }

    QTextStream ts(&f);
    foreach (const QString& r, mResources) {
        ts << r << "\n";
    }

    f.close();
    qDebug() << "[RESOURCE-EDITOR]: serialize ok!";
    return true;
}
bool ResourceEditor::Private::deserializeFromText()
{
    QFile f("resource.dat");
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "[RESOURCE-EDITOR]: FAILED TO OPEN resource.dat";
        return false;
    }

    QTextStream ts(&f);
    mResources.clear();
    while (!ts.atEnd()) {
        QString line = ts.readLine();
        mResources << line;
    }

    f.close();
    qDebug() << "[RESOURCE-EDITOR]: deserialize ok!";
    return true;
}
bool ResourceEditor::Private::serialize()
{
    QFile f("resource.dat");
    if (!f.open(QIODevice::WriteOnly)) {
        qDebug() << "[RESOURCE-EDITOR]: FAILED TO OPEN resource.dat";
        return false;
    }

    QDataStream ds(&f);
    foreach (const QString& r, mResources) {
        ds << r;
    }

    f.close();
    qDebug() << "[RESOURCE-EDITOR]: serialize ok!";
    return true;
}

bool ResourceEditor::Private::deserialize()
{
    QFile f("resource.dat");
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "[RESOURCE-EDITOR]: FAILED TO OPEN resource.dat";
        return false;
    }

    QDataStream ds(&f);
    mResources.clear();
    while (!ds.atEnd()) {
        QString s;
        ds >> s;
        mResources << s;
    }

    f.close();
    qDebug() << "[RESOURCE-EDITOR]: deserialize ok!";
    return true;
}

ResourceEditor::ResourceEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResourceEditor),
    mPrivate(new Private)
{
    ui->setupUi(this);
    connect(ui->pushButtonAdd,    SIGNAL(clicked()), this, SLOT(addButtonClicked()));
    connect(ui->pushButtonEdit,   SIGNAL(clicked()), this, SLOT(editButtonClicked()));
    connect(ui->pushButtonDelete, SIGNAL(clicked()), this, SLOT(deleteButtonClicked()));

    ui->tableWidgetContent->setColumnCount(2);
    mPrivate->updateResourceData(ui->tableWidgetContent);
}

ResourceEditor::~ResourceEditor()
{
    delete ui;
    delete mPrivate;
}

void ResourceEditor::addButtonClicked()
{
    QTableWidget* w = ui->tableWidgetContent;
    const int row = w->rowCount();
    QString value = QInputDialog::getText(this, "Add Resource", QString("F%1").arg(row+1));
    if (value.isEmpty()||value.isNull())
        return;

    w->insertRow(row);
    QTableWidgetItem* item = new QTableWidgetItem(QString("F%1").arg(row+1));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    w->setItem(row, ColResourceNo, item);

    item = new QTableWidgetItem(value);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    w->setItem(row, ColResourceName, item);

    mPrivate->commitResourceData(w);
}

void ResourceEditor::editButtonClicked()
{
    QTableWidget* w = ui->tableWidgetContent;
    const int row = w->currentRow();

    QTableWidgetItem* item = w->item(row, 1);
    if (!item)
        return;

    QString value = item->text();
    value = QInputDialog::getText(this, "Edit Resource", QString("F%1").arg(row+1), QLineEdit::Normal, value);
    if (value.isEmpty()||value.isNull())
        return;

    item = w->item(row, ColResourceNo);
    item->setText(QString("F%1").arg(row+1));

    item = w->item(row, ColResourceName);
    item->setText(value);
    mPrivate->commitResourceData(w);
}

void ResourceEditor::deleteButtonClicked()
{
    const int row = ui->tableWidgetContent->currentRow();
    ui->tableWidgetContent->removeRow(row);
    mPrivate->commitResourceData(ui->tableWidgetContent);
}















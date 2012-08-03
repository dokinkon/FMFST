#include "createnodedialog.h"
#include "ui_createnodedialog.h"


struct CreateNodeDialog::Private
{

};

CreateNodeDialog::CreateNodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateNodeDialog),
    mPrivate(new Private)
{
    ui->setupUi(this);
}

CreateNodeDialog::~CreateNodeDialog()
{
    delete ui;
    delete mPrivate;
}

QString CreateNodeDialog::getNodeName() const
{
    return ui->lineEditNodeName->text();
}

QString CreateNodeDialog::getFA() const
{
    return ui->lineEditFA->text();
}

QString CreateNodeDialog::getPA() const
{
    return ui->lineEditPA->text();
}

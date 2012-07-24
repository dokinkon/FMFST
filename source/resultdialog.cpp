#include "resultdialog.h"

ResultDialog::ResultDialog(QWidget *parent) :
    QDialog(parent)
{
    ui = new Ui::ResultDialog;
    ui->setupUi(this);
}

ResultDialog::~ResultDialog()
{
    delete ui;
}

void ResultDialog::setResult(const QString& result)
{
    ui->plainTextEdit->setPlainText(result);
}

#include "fmfstdialog.h"
#include "ui_fmfstdialog.h"

FMFSTDialog::FMFSTDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FMFSTDialog)
{
    ui->setupUi(this);
}

FMFSTDialog::~FMFSTDialog()
{
    delete ui;
}

QString FMFSTDialog::getProgram() const
{
    return ui->lineEditProgram->text();

}

QSet<QString> FMFSTDialog::getFN() const
{
    QSet<QString> fn;
    QStringList ss = ui->lineEditFN->text().split(",");
    foreach (const QString& s, ss) {
        s.trimmed();
        if (s.isEmpty()||s.isNull())
            continue;
        fn << s;
    }
    return fn;
}

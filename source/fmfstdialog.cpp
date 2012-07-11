#include "fmfstdialog.h"
#include "ui_fmfstdialog.h"
#include <QtCore>

#define JOBKEY "JOBKEY"
#define FILEKEY "FILEKEY"

FMFSTDialog::FMFSTDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FMFSTDialog)
{
    ui->setupUi(this);
	QSettings settings;
	ui->lineEditProgram->setText(settings.value(JOBKEY).toString());
	ui->lineEditFN->setText(settings.value(FILEKEY).toString());
}

FMFSTDialog::~FMFSTDialog()
{
    delete ui;
}

void FMFSTDialog::commit()
{
	QSettings settings;
	settings.setValue(JOBKEY, getProgram());
	settings.setValue(FILEKEY, ui->lineEditFN->text());
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

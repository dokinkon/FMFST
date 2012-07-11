#ifndef FMFSTDIALOG_H
#define FMFSTDIALOG_H

#include <QDialog>

namespace Ui {
class FMFSTDialog;
}

class FMFSTDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FMFSTDialog(QWidget *parent = 0);
    ~FMFSTDialog();

    QString getProgram() const;
    QSet<QString> getFN() const;
	void commit();
    
private:
    Ui::FMFSTDialog *ui;
};

#endif // FMFSTDIALOG_H

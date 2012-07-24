#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>
#include "ui_resultdialog.h"

class ResultDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResultDialog(QWidget *parent = 0);
    ~ResultDialog();
    
signals:
    
public slots:
    void setResult(const QString&);

private:
    Ui::ResultDialog* ui;
    
};

#endif // RESULTDIALOG_H

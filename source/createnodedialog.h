#ifndef CREATENODEDIALOG_H
#define CREATENODEDIALOG_H

#include <QDialog>

namespace Ui {
class CreateNodeDialog;
}

class CreateNodeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CreateNodeDialog(QWidget *parent = 0);
    ~CreateNodeDialog();
    QString getNodeName() const;
    QString getFA() const;
    QString getPA() const;
    
private:
    Ui::CreateNodeDialog *ui;

    struct Private;
    Private* mPrivate;
};

#endif // CREATENODEDIALOG_H

#include "mainwindow.h"
#include "tableeditor.h"
#include "graphicsscene.h"
#include "pathweighteditor.h"
#include "nodeeditor.h"
#include "fmfst.h"
#include "fmfstdialog.h"
#include "canvaswidget.h"
#include "ui_mainwindow.h"
#include "resultdialog.h"
#include "mapwidget.h"

#include <QtCore>
#include <QtGui>




struct MainWindow::Private
{

};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    TableEditor* editor = new TableEditor(this);
    addDockWidget(Qt::LeftDockWidgetArea, editor);
    ui->menuView->addAction(editor->toggleViewAction());

    CanvasWidget* canvasWidget = new CanvasWidget(this);
    setCentralWidget(canvasWidget);
    GetGraphicsScene().update();

    // connection actions
    connect(ui->actionCalculatePath, SIGNAL(triggered()), SLOT(slotFindMSPT()));

    // Restore settings
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    GetNodeEditor().initialize();
    GetPathWeightEditor().initialize();
    GetGraphicsScene().initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    GetGraphicsScene().serialize();
    GetPathWeightEditor().serialize();

    // save window geometry
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(e);
}

void MainWindow::slotFindMSPT()
{
    FMFSTDialog dialog(this);
    if (QDialog::Accepted!=dialog.exec())
        return;

    // check data correctness
    //QString program = dialog.getProgram();
    QSet<QString> programs = dialog.getPrograms();
    if (!GetNodeEditor().hasProgram(programs)) {
        QMessageBox::critical(this, "ERROR", QString("No such node contains required programs"));
        return;
    }

    QSet<QString> fn = dialog.getFN();
    foreach (const QString& file, fn) {
        if (!GetNodeEditor().hasFile(file)) {
            QMessageBox::critical(this, "ERROR", QString("No such file:%1").arg(file));
            return;
        }
    }

	dialog.commit();

    FMFST fmfst;
    fmfst.init(GetNodeEditor().nodes());


    QSet<int> necessaryNodes = GetNodeEditor().getNecessaryNodes();

    QSet<int>::Iterator it = necessaryNodes.begin();
    while (it!=necessaryNodes.end()) {
        int nodeId = *it;

        Node node = GetNodeEditor().getNode(nodeId);
        bool isValid = false;
        foreach (const QString& f, node.getFA().keys()) {
            if (fn.contains(f)) {
                isValid = true;
                break;
            }
        }

        if (!isValid) {
            it = necessaryNodes.erase(it);
        } else {
            ++it;
        }
    }

    foreach (int nodeId, necessaryNodes) {
        Node node = GetNodeEditor().getNode(nodeId);

        foreach (const QString& f, node.getFA().keys()) {
            if (fn.contains(f)) {
                fn.remove(f);
            }
        }
    }


    QString result = fmfst.run(programs, fn, necessaryNodes);
    ResultDialog resultDialog(this);
    resultDialog.setResult(result);
    resultDialog.exec();
}
























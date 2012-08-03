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

    // setup graphicview and graphicsscene...
    //QGraphicsView* view = new QGraphicsView(this);
    //view->setScene(new GraphicsScene);

    CanvasWidget* canvasWidget = new CanvasWidget(this);
    setCentralWidget(canvasWidget);
    GetGraphicsScene().update();

    // connection actions
    connect(ui->actionCalculatePath, SIGNAL(triggered()), SLOT(slotFindMSPT()));

    // Restore settings
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    GetGraphicsScene().serialize();

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
    QString program = dialog.getProgram();
    if (!GetNodeEditor().hasProgram(program)) {
        QMessageBox::critical(this, "ERROR", QString("No such program:%1").arg(program));
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
    QVector<Node> nodes = GetNodeEditor().nodes();

    QSet<int>::Iterator it = necessaryNodes.begin();
    while (it!=necessaryNodes.end()) {
        int nodeIndex = *it;
        Node& node = nodes[nodeIndex];
        bool isValid = false;
        foreach (const QString& f, node.getFA()) {
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



    foreach (int nodeIndex, necessaryNodes) {
        Node& node = nodes[nodeIndex];

        foreach (const QString& f, node.getFA()) {
            if (fn.contains(f)) {
                fn.remove(f);
            }
        }
    }


    QString result = fmfst.run(program, fn, necessaryNodes);
    ResultDialog resultDialog(this);
    resultDialog.setResult(result);
    resultDialog.exec();
}
























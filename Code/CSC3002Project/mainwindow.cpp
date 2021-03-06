#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mips_win.h"
#include "debugger.h"
#include "compiler.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->splitter);    // set the splitter the central widget
    setupEditor();
    checkMipsFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{ //ask if exit
   QMessageBox::StandardButton result=QMessageBox::question(this, "QUIT", "Do you want to quit?",
                      QMessageBox::Yes|QMessageBox::No |QMessageBox::Cancel,
                      QMessageBox::No);

    if (result==QMessageBox::Yes) {
        remove("testfile.txt");
        remove("mips.asm");
        remove("midCode.txt");
        remove("output.txt");
        event->accept();
    }   else  event->ignore();
}


void MainWindow::checkMipsFile()
{
    QFile file("mips.asm");
    if(file.open(QFile::ReadOnly | QFile::Text)){
        remove("testfile.txt");
        remove("mips.asm");
        remove("midCode.txt");
        remove("output.txt");
    }

}




void MainWindow::on_actionNew_triggered()
{
    file_path = "";
    ui->Editor->setText("");

}


void MainWindow::on_actionOpen_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this,"Open this file");
    QFile file(file_name);
    file_path = file_name;
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","file not open");
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    ui->Editor->setText(text);
    file.close();
    string s = file_name.toStdString();
    copyFile(s,"testfile.txt");
}

void MainWindow::on_actionSave_triggered()
{
    QFile file("testfile.txt");
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","file not open");
        return;
    }
    QTextStream out(&file);
    QString text = ui->Editor->toPlainText();
    out << text;
    file.flush();
    file.close();
}

void MainWindow::on_actionSave_as_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this,"Open this file");
    QFile file(file_name);
    file_path = file_name;
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning.","file not open");
        return;
    }
    QTextStream out(&file);
    QString text = ui->Editor->toPlainText();
    out << text;
    file.flush();
    file.close();
    string s = file_name.toStdString();
    copyFile(s, "testfile.txt");
}

void MainWindow::on_actionCut_triggered()
{
    ui->Editor->cut();
}

void MainWindow::on_actionCopy_triggered()
{
    ui->Editor->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->Editor->paste();
}

void MainWindow::on_actionRedo_triggered()
{
    ui->Editor->redo();
}

void MainWindow::on_actionUndo_triggered()
{
    ui->Editor->undo();
}

void MainWindow::setupEditor()
{
    QFont font;                  // set default font
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    ui->Editor->setFont(font);

    highlighter = new Highlighter(ui->Editor->document());  // initialize and install the Highlighter object

}

void MainWindow::on_actionInformation_triggered()
{
    QMessageBox::information(this,"Information","This project is based on the final assignment of CSC3002,\n"
                                                "Topic 2: The System Binary Tools.\n"
                                                "Team members:\n"
                                                "Yuzhe Jin 118010128, Hanshen Jing 117010112,\n"
                                                "Wentao Guo 117010078, Yuan Gao 118010075,\n"
                                                "Yuhao Du 118010053, Jiaqi Liu 117010165.");
}

void MainWindow::on_actionExit_triggered()
{
    remove("testfile.txt");
    remove("mips.asm");
    remove("midCode.txt");
    remove("output.txt");
    QApplication::quit();
}


void MainWindow::on_actionRun_in_terminal_triggered()
{
    QString codetext = ui->Editor->toPlainText();
    if(codetext.trimmed().isEmpty()) {
        QMessageBox::warning(this,"Warning","You should open a file or write programs to run first.");
        return;
    }

    QString terminaltext = ui->Terminal->toPlainText();
//    if(!terminaltext.trimmed().isEmpty()) {
//        QMessageBox::warning(this,"Warning","Please refresh this mainwindow first.");
//        return;
//    }


    compiler();
    QFile file("output.txt");
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","file not open");
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    ui->Terminal->setText(text);
    file.close();

}


void MainWindow::on_actionCompile_to_other_codes_triggered()
{
    QFile file("mips.asm");
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","file not open");
        return;
    } else {
        MIPS_win mips_win;
        mips_win.setModal(true);
        mips_win.exec();
    }
}

void MainWindow::on_actionDebugger_triggered()
{   QFile file("mips.asm");
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","file not open");
        return;
    } else {
        Debugger debugger;
        debugger.setModal(true);
        debugger.exec();
    }
}



void MainWindow::on_actionRefresh_triggered()
{
    remove("testfile.txt");
    remove("mips.asm");
    remove("midCode.txt");
    remove("output.txt");
    QString program = QApplication::applicationFilePath();
    QStringList arguments = QApplication::arguments();
    QString workingDirectory = QDir::currentPath();
    QProcess::startDetached(program,arguments,workingDirectory);
    QApplication::exit();
}

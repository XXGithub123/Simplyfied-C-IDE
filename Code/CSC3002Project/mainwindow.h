#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "highlighter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);
    void checkMipsFile();


private slots:                       //Actions on menubar
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSave_as_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionRedo_triggered();

    void on_actionUndo_triggered();

    void on_actionInformation_triggered();

    void on_actionExit_triggered();

    void on_actionRun_in_terminal_triggered();

    void on_actionCompile_to_other_codes_triggered();

    void on_actionDebugger_triggered();

    void on_actionRefresh_triggered();

private:
    void setupEditor();

    Ui::MainWindow *ui;
    QString file_path;
    Highlighter *highlighter;
};
#endif // MAINWINDOW_H

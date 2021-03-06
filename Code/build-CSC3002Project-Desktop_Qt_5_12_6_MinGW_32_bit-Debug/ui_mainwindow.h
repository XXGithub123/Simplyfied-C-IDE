/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSave_as;
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionRedo;
    QAction *actionUndo;
    QAction *actionRun_in_terminal;
    QAction *actionInformation;
    QAction *actionExit;
    QAction *actionCompile_to_other_codes;
    QAction *actionDebugger;
    QAction *actionRefresh;
    QWidget *centralwidget;
    QSplitter *splitter;
    QTextEdit *Editor;
    QTextBrowser *Terminal;
    QMenuBar *menubar;
    QMenu *menuCode_Editor;
    QMenu *menuEdit;
    QMenu *menuCompile;
    QMenu *menuRun;
    QMenu *menuAbout;
    QMenu *menuDebug;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(789, 611);
        actionNew = new QAction(MainWindow);
        actionNew->setObjectName(QString::fromUtf8("actionNew"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/rec/win/filenew.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew->setIcon(icon);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/rec/win/fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon1);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/rec/win/filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon2);
        actionSave_as = new QAction(MainWindow);
        actionSave_as->setObjectName(QString::fromUtf8("actionSave_as"));
        actionSave_as->setIcon(icon2);
        actionCut = new QAction(MainWindow);
        actionCut->setObjectName(QString::fromUtf8("actionCut"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/rec/win/editcut.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCut->setIcon(icon3);
        actionCopy = new QAction(MainWindow);
        actionCopy->setObjectName(QString::fromUtf8("actionCopy"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/rec/win/editcopy.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCopy->setIcon(icon4);
        actionPaste = new QAction(MainWindow);
        actionPaste->setObjectName(QString::fromUtf8("actionPaste"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/rec/win/editpaste.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPaste->setIcon(icon5);
        actionRedo = new QAction(MainWindow);
        actionRedo->setObjectName(QString::fromUtf8("actionRedo"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/rec/win/editredo.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRedo->setIcon(icon6);
        actionUndo = new QAction(MainWindow);
        actionUndo->setObjectName(QString::fromUtf8("actionUndo"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/rec/win/editundo.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUndo->setIcon(icon7);
        actionRun_in_terminal = new QAction(MainWindow);
        actionRun_in_terminal->setObjectName(QString::fromUtf8("actionRun_in_terminal"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/rec/image/Knob Play Green.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionRun_in_terminal->setIcon(icon8);
        actionInformation = new QAction(MainWindow);
        actionInformation->setObjectName(QString::fromUtf8("actionInformation"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/rec/image/About.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionInformation->setIcon(icon9);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/rec/image/Knob Cancel.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon10);
        actionCompile_to_other_codes = new QAction(MainWindow);
        actionCompile_to_other_codes->setObjectName(QString::fromUtf8("actionCompile_to_other_codes"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/rec/image/Copy.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionCompile_to_other_codes->setIcon(icon11);
        actionDebugger = new QAction(MainWindow);
        actionDebugger->setObjectName(QString::fromUtf8("actionDebugger"));
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/rec/image/Check.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionDebugger->setIcon(icon12);
        actionRefresh = new QAction(MainWindow);
        actionRefresh->setObjectName(QString::fromUtf8("actionRefresh"));
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/rec/image/Knob Loop On.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionRefresh->setIcon(icon13);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setGeometry(QRect(10, 0, 771, 531));
        splitter->setOrientation(Qt::Vertical);
        Editor = new QTextEdit(splitter);
        Editor->setObjectName(QString::fromUtf8("Editor"));
        splitter->addWidget(Editor);
        Terminal = new QTextBrowser(splitter);
        Terminal->setObjectName(QString::fromUtf8("Terminal"));
        Terminal->setReadOnly(true);
        Terminal->setOverwriteMode(false);
        splitter->addWidget(Terminal);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 789, 26));
        menuCode_Editor = new QMenu(menubar);
        menuCode_Editor->setObjectName(QString::fromUtf8("menuCode_Editor"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuCompile = new QMenu(menubar);
        menuCompile->setObjectName(QString::fromUtf8("menuCompile"));
        menuRun = new QMenu(menubar);
        menuRun->setObjectName(QString::fromUtf8("menuRun"));
        menuAbout = new QMenu(menubar);
        menuAbout->setObjectName(QString::fromUtf8("menuAbout"));
        menuDebug = new QMenu(menubar);
        menuDebug->setObjectName(QString::fromUtf8("menuDebug"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setMinimumSize(QSize(2, 10));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menuCode_Editor->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuRun->menuAction());
        menubar->addAction(menuCompile->menuAction());
        menubar->addAction(menuDebug->menuAction());
        menubar->addAction(menuAbout->menuAction());
        menuCode_Editor->addAction(actionNew);
        menuCode_Editor->addAction(actionOpen);
        menuCode_Editor->addSeparator();
        menuCode_Editor->addAction(actionSave);
        menuCode_Editor->addAction(actionSave_as);
        menuCode_Editor->addSeparator();
        menuCode_Editor->addAction(actionExit);
        menuCode_Editor->addAction(actionRefresh);
        menuEdit->addAction(actionCut);
        menuEdit->addAction(actionCopy);
        menuEdit->addAction(actionPaste);
        menuEdit->addSeparator();
        menuEdit->addAction(actionRedo);
        menuEdit->addAction(actionUndo);
        menuCompile->addAction(actionCompile_to_other_codes);
        menuRun->addAction(actionRun_in_terminal);
        menuAbout->addAction(actionInformation);
        menuDebug->addAction(actionDebugger);
        toolBar->addAction(actionNew);
        toolBar->addAction(actionOpen);
        toolBar->addAction(actionSave);
        toolBar->addAction(actionUndo);
        toolBar->addAction(actionRedo);
        toolBar->addAction(actionRun_in_terminal);
        toolBar->addAction(actionExit);
        toolBar->addAction(actionRefresh);
        toolBar->addAction(actionInformation);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Code Editor", nullptr));
        actionNew->setText(QApplication::translate("MainWindow", "New", nullptr));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", nullptr));
        actionSave->setText(QApplication::translate("MainWindow", "Save", nullptr));
        actionSave_as->setText(QApplication::translate("MainWindow", "Save as", nullptr));
        actionCut->setText(QApplication::translate("MainWindow", "Cut", nullptr));
        actionCopy->setText(QApplication::translate("MainWindow", "Copy", nullptr));
        actionPaste->setText(QApplication::translate("MainWindow", "Paste", nullptr));
        actionRedo->setText(QApplication::translate("MainWindow", "Redo", nullptr));
        actionUndo->setText(QApplication::translate("MainWindow", "Undo", nullptr));
        actionRun_in_terminal->setText(QApplication::translate("MainWindow", "Run in terminal", nullptr));
        actionInformation->setText(QApplication::translate("MainWindow", "Information", nullptr));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", nullptr));
        actionCompile_to_other_codes->setText(QApplication::translate("MainWindow", "Compile to other codes", nullptr));
        actionDebugger->setText(QApplication::translate("MainWindow", "Debugger", nullptr));
        actionRefresh->setText(QApplication::translate("MainWindow", "Refresh", nullptr));
        menuCode_Editor->setTitle(QApplication::translate("MainWindow", "File", nullptr));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", nullptr));
        menuCompile->setTitle(QApplication::translate("MainWindow", "Compile", nullptr));
        menuRun->setTitle(QApplication::translate("MainWindow", "Run", nullptr));
        menuAbout->setTitle(QApplication::translate("MainWindow", "About", nullptr));
        menuDebug->setTitle(QApplication::translate("MainWindow", "Debug", nullptr));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

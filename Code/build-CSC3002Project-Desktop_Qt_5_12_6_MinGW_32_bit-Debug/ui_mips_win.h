/********************************************************************************
** Form generated from reading UI file 'mips_win.ui'
**
** Created by: Qt User Interface Compiler version 5.12.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MIPS_WIN_H
#define UI_MIPS_WIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_MIPS_win
{
public:
    QTextBrowser *textBrowser;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *MIPS_win)
    {
        if (MIPS_win->objectName().isEmpty())
            MIPS_win->setObjectName(QString::fromUtf8("MIPS_win"));
        MIPS_win->resize(575, 554);
        textBrowser = new QTextBrowser(MIPS_win);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(20, 110, 531, 421));
        pushButton = new QPushButton(MIPS_win);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(70, 40, 151, 51));
        pushButton_2 = new QPushButton(MIPS_win);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(340, 40, 151, 51));

        retranslateUi(MIPS_win);

        QMetaObject::connectSlotsByName(MIPS_win);
    } // setupUi

    void retranslateUi(QDialog *MIPS_win)
    {
        MIPS_win->setWindowTitle(QApplication::translate("MIPS_win", "Dialog", nullptr));
        pushButton->setText(QApplication::translate("MIPS_win", "MIPS", nullptr));
        pushButton_2->setText(QApplication::translate("MIPS_win", "Mid Code", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MIPS_win: public Ui_MIPS_win {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MIPS_WIN_H

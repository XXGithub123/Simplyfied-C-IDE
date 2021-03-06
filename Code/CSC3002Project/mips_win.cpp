#include "mips_win.h"
#include "ui_mips_win.h"
#include "compiler.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

MIPS_win::MIPS_win(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MIPS_win)
{
    ui->setupUi(this);
    init_MIPS_win();
}

MIPS_win::~MIPS_win()
{
    delete ui;
}

void MIPS_win::init_MIPS_win()
{
    on_pushButton_clicked();
}

void MIPS_win::on_pushButton_clicked()
{
    QFile file("mips.asm");
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","file not open");
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    ui->textBrowser->setText(text);
    file.close();
}

void MIPS_win::on_pushButton_2_clicked()
{
    QFile file("midCode.txt");
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","file not open");
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    ui->textBrowser->setText(text);
    file.close();
}

#ifndef MIPS_WIN_H
#define MIPS_WIN_H

#include <QDialog>

namespace Ui {
class MIPS_win;
}

class MIPS_win : public QDialog
{
    Q_OBJECT

public:
    explicit MIPS_win(QWidget *parent = nullptr);
    ~MIPS_win();

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MIPS_win *ui;
    void init_MIPS_win();
};

#endif // MIPS_WIN_H

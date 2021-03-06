#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QDialog>
#include "simulator.h"
#include <vector>
#include <QTextBrowser>

using namespace std;


int init_debugger();

vector<int> debugging(int line_idx);


namespace Ui {
class Debugger;
}

class Debugger : public QDialog
{
    Q_OBJECT

public:
    explicit Debugger(QWidget *parent = nullptr);
    ~Debugger();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    map<int, QTextBrowser*> textbrowser;
    void init_map();
    Ui::Debugger *ui;
};

#endif // DEBUGGER_H

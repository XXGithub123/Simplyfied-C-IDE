#include "debugger.h"
#include "ui_debugger.h"
#include "simulator.h"
#include "instructions.h"
#include "assembler.h"
#include "str.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTextBlock>
#include <QTextBrowser>
#include <map>
#include <vector>

using namespace std;

map<int, int> address2line;
map<int, string> line2instruction;
int * end_position;

extern ifstream is;
extern int registers[34];

int find_first_line();
void init_map(int);

int init_debugger(){
    memary_initialize();
    registers_initialize();
    open_file();
    end_position = assemble_machine_code();
    load_data();

    int first_line = find_first_line();
    init_map(first_line);

    return first_line;
}

int find_first_line(){
    string str;
    int line_idx = 0;
    while(getline(is,str)){
        if (!is_Empty(str) && trim(str) == ".text") break;
        line_idx += 1;
    }
    return line_idx+1;
}


void init_map(int first_line){
    string str;
    int line_idx = first_line;
    int address = (int&) PC;
    while(getline(is,str)){
        line2instruction[line_idx] = str;
        if(split(str)[0] == "") {
            line_idx++;
            continue;
        }
        address2line[address] = line_idx;
//        cout << "address is: " << hex << address << endl;
//        cout << "line idx is: " << dec << line_idx << endl;
        address += 4;
        line_idx++;
    }
    is.clear();
    is.seekg(0, ios::beg);

}

vector<int> debugging(int line_idx){
    string instruction = line2instruction[line_idx];

    run_instruction(*PC);
    PC++;
    int input_register1, input_register2, output_register;
    int output_value;
    line_idx = address2line[(int&)PC];   // line index of next instruction.


    input_register1 = input_register2 = output_register = output_value = -1;    // defalut value.
    vector<string> s = split(instruction);

    if (s.size() == 3){ // I type instruction.
        if(s[2].find('(') == string::npos){     // for lw and sw
            input_register1 = input_register2 = find_register_number(s[2]); // rs
        }
        output_register = find_register_number(s[1]);   // rt
        output_value = registers[output_register];
    }
    else if (s.size() == 4) {   // R type instruction.
        input_register1 = find_register_number(s[2]);
        input_register2 = find_register_number(s[3]);
        output_register = find_register_number(s[1]);
        output_value = registers[output_register];
    }
    vector<int> return_vector =  {input_register1, input_register2, output_register, output_value, line_idx};
    return return_vector;
}







Debugger::Debugger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Debugger)
{
    ui->setupUi(this);
    init_map();
}

Debugger::~Debugger()
{
    is.close();
    delete ui;
}

int line_idx; //define a global variable to record the line number.

void Debugger::init_map(){
    vector<QTextBrowser*> txVector = {ui->textBrowser_2, ui->textBrowser_3, ui->textBrowser_4, ui->textBrowser_5,
                                      ui->textBrowser_6, ui->textBrowser_7, ui->textBrowser_8, ui->textBrowser_9,
                                      ui->textBrowser_10, ui->textBrowser_11, ui->textBrowser_12, ui->textBrowser_13,
                                      ui->textBrowser_14, ui->textBrowser_15, ui->textBrowser_16, ui->textBrowser_17,
                                      ui->textBrowser_18, ui->textBrowser_19, ui->textBrowser_20, ui->textBrowser_21,
                                      ui->textBrowser_22, ui->textBrowser_23, ui->textBrowser_24, ui->textBrowser_25,
                                      ui->textBrowser_26, ui->textBrowser_27, ui->textBrowser_28, ui->textBrowser_29,
                                      ui->textBrowser_30, ui->textBrowser_31, ui->textBrowser_32, ui->textBrowser_33,
                                      ui->textBrowser_34, ui->textBrowser_35, ui->textBrowser_36, ui->textBrowser_37,
                                      ui->textBrowser_38, ui->textBrowser_39, ui->textBrowser_40, ui->textBrowser_41,
                                      ui->textBrowser_42, ui->textBrowser_43, ui->textBrowser_44, ui->textBrowser_45,
                                      ui->textBrowser_46, ui->textBrowser_47, ui->textBrowser_48, ui->textBrowser_49,
                                      ui->textBrowser_50, ui->textBrowser_51, ui->textBrowser_52, ui->textBrowser_53,
                                      ui->textBrowser_54, ui->textBrowser_55, ui->textBrowser_56, ui->textBrowser_57,
                                      ui->textBrowser_58, ui->textBrowser_59, ui->textBrowser_60, ui->textBrowser_61,
                                      ui->textBrowser_62, ui->textBrowser_63, ui->textBrowser_64, ui->textBrowser_65};

    for(int i = 0; i < 64; i++) {
        textbrowser[i] = txVector[i];
    }

}


void Debugger::on_pushButton_clicked()
{
    QString mipstext = ui->plainTextEdit->toPlainText();
    if(!mipstext.trimmed().isEmpty()) {
        QMessageBox::warning(this,"Result","You have already started.");
        return;
    }

    QFile file("mips.asm");
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","file not open");
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    ui->plainTextEdit->setPlainText(text);
    file.close();


    line_idx = init_debugger(); //The first line number to highlight.
    for (int i = 0; i<32;i++){  // Initialize the registers value.
        textbrowser[i+32]->setText(QString::number(registers[i]));
    }


    QList<QTextEdit::ExtraSelection> extraSelection;
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(Qt::yellow);
    selection.format.setProperty(QTextFormat::FullWidthSelection,true);

    QTextBlock block = ui->plainTextEdit->document()->findBlockByNumber(line_idx);
    ui->plainTextEdit->setTextCursor(QTextCursor(block));
    selection.cursor = ui->plainTextEdit->textCursor();
    selection.cursor.clearSelection();
    //push back the selection set just now to the list
    extraSelection.append(selection);
    ui->plainTextEdit->setExtraSelections(extraSelection);
}

void Debugger::on_pushButton_2_clicked()
{
        QString text = ui->plainTextEdit->toPlainText();
        if(text.trimmed().isEmpty()) {
            QMessageBox::information(this,"Result","There are no MIPS codes displayed.");
            return;
        } else {

            for(int i = 0; i < 32; i++) {
                textbrowser[i] -> setStyleSheet("background-color:white");
            }

            vector<int> debugging_result = debugging(line_idx);
            line_idx = debugging_result[4];

            QList<QTextEdit::ExtraSelection> extraSelection;
            QTextEdit::ExtraSelection selection;
            selection.format.setBackground(Qt::yellow);
            selection.format.setProperty(QTextFormat::FullWidthSelection,true);

            QTextBlock block = ui->plainTextEdit->document()->findBlockByNumber(line_idx);
            ui->plainTextEdit->setTextCursor(QTextCursor(block));
            selection.cursor = ui->plainTextEdit->textCursor();
            selection.cursor.clearSelection();
            //push back the selection set just now to the list
            extraSelection.append(selection);
            ui->plainTextEdit->setExtraSelections(extraSelection);

            QString output_value = QString::number(debugging_result[3]);

            if (debugging_result[0] != -1)
                textbrowser[debugging_result[0]]->setStyleSheet("background-color:yellow");
            if (debugging_result[1] != -1)
                textbrowser[debugging_result[1]]->setStyleSheet("background-color:yellow");
            if (debugging_result[2] != -1){
                textbrowser[debugging_result[2]]->setStyleSheet("background-color:orange");
                textbrowser[debugging_result[2]+32]->setText(output_value);
            }

            if(output_content.to_print){

                ui->textBrowser->append(QString::fromStdString(output_content.content));
                output_content.to_print = false;
            }

        }

}

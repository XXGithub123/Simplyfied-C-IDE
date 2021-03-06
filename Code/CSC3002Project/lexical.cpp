#include "lexical.h"
#include "function.h"

char ch;
char token[100000];                     // Store the token
int tokenIndex = 0;                     // The end index of the token
int num;                                // Record integer constant
char con_ch;                            // Record character constant
char s[100000];                         // Record string constant
enum typeId symbol;

int len_reservedWord = 12;
char reservedWord[20][10] = {
    "const", "int", "char", "void", "main", "if",
    "else", "do", "while", "for", "printf", "return",
};

int indexs = 0;                         // Pointer to the char that is being read
int oldIndex;                           // Store the privious pointer position when needed
int line = 1;                           // line number
string filecontent;                     // Store the whole file content in one string

// extern ofstream errorfile;

bool isSpace()
{
    return (ch == ' ');
}

bool isNewline()
{
    return (ch == '\n');
}

bool isBlank()
{
    return (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r' || ch == '\f' || ch == '\v');
}

bool isNoDigit()
{
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == '_'));
}

bool isDigit()
{
    return (ch >= '0' && ch <= '9');
}

bool isPlus()
{
    return (ch == '+');
}

bool isMinu()
{
    return (ch == '-');
}

bool isMult()
{
    return (ch == '*');
}

bool isDiv()
{
    return (ch == '/');
}

bool isLss()
{
    return (ch == '<');
}

bool isGre()
{
    return (ch == '>');
}

bool isExcla()
{
    return (ch == '!');
}

bool isAssign()
{
    return (ch == '=');
}

bool isSemicn()
{
    return (ch == ';');
}

bool isComma()
{
    return (ch == ',');
}

bool isLparent()
{
    return (ch == '(');
}

bool isRparent()
{
    return (ch == ')');
}

bool isLbrack()
{
    return (ch == '[');
}

bool isRbrack()
{
    return (ch == ']');
}

bool isLbrace()
{
    return (ch == '{');
}

bool isRbrace()
{
    return (ch == '}');
}

bool isSquo()
{
    return (ch == '\'');
}

bool isDquo()
{
    return (ch == '\"');
}

bool isBackSlash()
{
    return (ch == '\\');
}

bool isEOF()
{
    int fileLen = filecontent.size();
    return indexs >= fileLen;
}

bool isStringChar()
{
    return ((ch >= 35 && ch <= 126) || ch == 32 || ch == 33);
}

bool isOperator()
{
    return (isLss() || isGre() || isAssign() || isExcla()
        || isPlus() || isMinu() || isMult() || isDiv()
        || isRparent() || isRbrack() || isComma() || isSemicn());
}

/*
 * Implementation notes: clearToken
 * --------------------------------
 * Set the end index of the token to the start to clear the token
 */
void clearToken()
{
//    int i;
//    for (i = 0; i < tokenIndex; i++)
//        token[i] = '\0';
    tokenIndex = 0;
}

/*
 * Implementation notes: form_token
 * --------------------------------
 * Add one char to the end of the token to form the completed token
 */
void formToken()
{
    token[tokenIndex++] = ch;
}

/*
 * Implementation notes: get_ch
 * ----------------------------
 * Read one character from the file content
 */
void get_ch()
{
    ch = filecontent[indexs++];
    if (ch == '\n') {
        line++;
    }
}

/*
 * Implementation notes: retract
 * -----------------------------
 * Retract one char and reduce one line number if the char is '\n'
 */
void retract()
{
    if (ch == '\n') {
        line--;
    }
    indexs--;
}

/*
 * Implementation notes: retractString
 * -----------------------------------
 * Retract one string by setting the index to the start of the string
 * and reduce one line number if there is a '\n'
 */
void retractString(int oldIndex) {
    for (int i = oldIndex; i < indexs; i++) {
        if (filecontent[i] == '\n') {
            line--;
        }
    }
    indexs = oldIndex;
}

/*
 * Implementation notes: reserver
 * ------------------------------
 * Return the index of the reserved word if the token is a reserved word,
 * otherwise return -1, which means the token is an identifier
 */
int reserver()
{
    for (int i = 0; i < len_reservedWord; i++) {
        if (!strcmp(reservedWord[i], token)) {
            return i;
        }
    }
    return -1;
}

/*
 * Implementation notes: transNum
 * ------------------------------
 * Use ASCii to transfer the string into decimal integer
 */
int transNum()
{
    int res = 0;
    for (int i = 0; i < tokenIndex; i++) {
        res = res * 10 + (token[i] - '0');
    }
    return res;
}

/*
 * Implementation notes: parseWord
 * ------------------------------
 * Parse reserved word or identifier
 */
void parseWord()
{
    while (isNoDigit() || isDigit()) {  // Get the token
        formToken();
        get_ch();
    }
    retract();                      // Keep the pointer at the end of the last token
    token[tokenIndex] = '\0';
}

/*
 * Implementation notes: parseNum
 * ------------------------------
 * Parse int constants
 */
void parseNum()
{
    while (isDigit()) {
        formToken();
        get_ch();
    }
    retract();
    token[tokenIndex] = '\0';
    num = transNum();
    symbol = INTCON;
}

/*
 * Implementation notes: parseChar
 * -------------------------------
 * Parse char constants
 */
void parseChar(int out)
{
    get_ch();
    char tmp = ch;                      // Record the ch temporary
    if (isBackSlash())
    {
        get_ch();
        switch(ch)                      // Parse escape characters
        {
        case '0':
            tmp = '\0';
            break;
        case 'a':
            tmp = '\a';
            break;
        case 'b':
            tmp = '\b';
            break;
        case 't':
            tmp = '\t';
            break;
        case 'n':
            tmp = '\n';
            break;
        case 'v':
            tmp = '\v';
            break;
        case 'f':
            tmp = '\f';
            break;
        case 'r':
            tmp = '\r';
            break;
        case '\"':
            tmp = '\"';
            break;
        case '\'':
            tmp = '\'';
            break;
        case '\\':
            tmp = '\\';
            break;
        }
    }
    get_ch();
    if (isSquo()) {
        con_ch = tmp;
        symbol = CHARCON;
    }
    else {                              // Lacking the close quote
        if (out) {
            raiseError("Lacking the close quote.", line);
            // errorfile << line << " Lacking the close quote\n";
            throw -1;
        }
        int old = indexs;
        while (1) {
            if (isSquo()) {
                break;
            }
            if (isNewline()) {
                retractString(old-1);   // Change the index back to where the close quote should be
                break;
            }
            if (isOperator()) {         //, ; + - * / > < ! =
                indexs--;
                break;
            }
            get_ch();
        }
        con_ch = tmp;                   // Take the first char of the string to be the char constant in default
        symbol = CHARCON;
    }
}

/*
 * Implementation notes: parseString
 * ---------------------------------
 * Parse string constants
 */
void parseString(int out)
{
    get_ch();
    while (isStringChar()) {
        formToken();
        get_ch();
    }
    if (isDquo()) {
        symbol = STRCON;
        token[tokenIndex] = '\0';
        strcpy(s, token);
    }
    else {                              // Lacking the close quote
        if (isNewline()) {
            line--;
            if (out) {
                raiseError("Lacking the close quote.", line);
                // errorfile << line << " Lacking the close quote\n";
                throw -1;
            }
        while (1) {
            indexs--;
            if (filecontent[indexs] == ')') {
                break;
            }
            tokenIndex--;
        }
        symbol = STRCON;
        token[tokenIndex] = '\0';
        strcpy(s, token);
        }
    }
}

/*
 * Implementation notes: preprocess
 * --------------------------------
 * Clear all kinds of blank space and clear comments
 */
void preprocess(int out)
{
    while(1)
    {
        if(isBlank())
          get_ch();
        else if(isDiv())
        {
            get_ch();               // Read one char ahead to see if there are comments following
            if(isMult())
            {
                parseComment(out);
            }
            else if(isDiv())
            {
                parseComment(out);
            }
            else
            {
                retract();       // Return '/' back to the string
                break;
            }
        }
        else
            break;
    }
}

/*
 * Implementation notes: parseComment
 * ----------------------------------
 * Ignore the comments (// and /\**\/)
 */
void parseComment(int out)
{
    if (isDiv())                            // Parse "//"
    {
        while (!isNewline()) get_ch();
        get_ch();
        line++;
        return;
    }
    get_ch();                               // Parse "/\**\/"
    do
    {
        do
        {
            if(isNewline() || isMult() || isEOF())
                break;
            else
                get_ch();
        }while(1);
        if(isNewline())
        {
            line++;
            get_ch();
        }
        else if(isMult())
        {
            get_ch();
            if(isDiv())
            {
                get_ch();
                return;
            }
        }
        else
        {
            if (out) {
                raiseError("No corresponding end mark till EOF", line);
                throw -1;
                return;
            }
        }
    }while(1);
}

/*
 * Implementation notes: getsym
 * ----------------------------
 * Get next token and parse its symbol
 */
int getsym(int out)
{
    oldIndex = indexs;                  // Record the end index of the privious token in case that retraction is needed
    clearToken();
    get_ch();
    preprocess(out);
    if (isEOF()) {                      // Exit when reaching the end of the file
        return -1;
    }
    if (isNoDigit()) {                  // The token is a reserved word or an identifier
        parseWord();
        if (reserver()== -1) {          // Identify whether the token is an identifier
            symbol = IDENFR;
        }
        else {
            symbol = (typeId)reserver();
        }
        return 1;
    }
    else if (isDigit()) {               // The token is a int constant
        parseNum();
        return 1;
    }
    else if (isSquo()) {                // The token is a char constant
        parseChar(out);
        return 1;
    }
    else if (isDquo()) {                // The token is a string constant
        parseString(out);
        return 1;
    }
    else if (isPlus()) {                // +
        symbol = PLUS;
        return 1;
    }
    else if (isMinu()) {                // -
        symbol = MINU;
        return 1;
    }
    else if (isMult()) {                // *
        symbol = MULT;
        return 1;
    }
    else if (isDiv()) {                 // /     
        symbol = DIV;
        return 1;
    }
    else if (isLss()) {                 // <
        get_ch();
        if (isAssign()) {               // <=
            symbol = LEQ;
        }
        else {
            symbol = LSS;               // <
            retract();
        }
        return 1;
    }
    else if (isGre()) {                 // >
        get_ch();
        if (isAssign()) {               // >=
            symbol = GEQ;
        }
        else {
            symbol = GRE;               // >
            retract();
        }
        return 1;
    }
    else if (isExcla()) {               // !
        get_ch();
        if (isAssign()) {               // !=
            symbol = NEQ;
        }
        else {                          // Not suppport operator '!'
            retract();
            if (out) {
                raiseError("Lacking the close quote.", line);
                // errorfile << line << " Illegal operator '!'\n";
                throw -1;
            }
            symbol = NEQ;
        }
        return 1;
    }
    else if (isAssign()) {              // =
        get_ch();
        if (isAssign()) {               //==
            symbol = EQL;
        }
        else {
            symbol = ASSIGN;
            retract();
        }
        return 1;
    }
    else if (isSemicn()) {              // ;
        symbol = SEMICN;
        return 1;
    }
    else if (isComma()) {               // ,
        symbol = COMMA;
        return 1;
    }
    else if (isLparent()) {             // (
        symbol = LPARENT;
        return 1;
    }
    else if (isRparent()) {             // )
        symbol = RPARENT;
        return 1;
    }
    else if (isLbrack()) {              // [
        symbol = LBRACK;
        return 1;
    }
    else if (isRbrack()) {              // ]
        symbol = RBRACK;
        return 1;
    }
    else if (isLbrace()) {              // {
        symbol = LBRACE;
        return 1;
    }
    else if (isRbrace()) {              // }
        symbol = RBRACE;
        return 1;
    }
    else {
        //wrong! retract();
        if (out) {
            raiseError("Unrecognizable symbol", line);
            // errorfile << line << " Unrecognizable symbol\n";
            throw -1;
        }
        return getsym(out);
    }
}


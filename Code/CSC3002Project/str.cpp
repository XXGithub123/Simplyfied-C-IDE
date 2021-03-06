/*
 * File: str_utils.cpp
 * ---------------------
 * This file implement some function to deal with the string in the input file.
 */
#include <string>
#include <vector>
#include <sstream>
#include "str.h"

using namespace std;

/*
 * Judge a string is empty or all whitespace.
 */
bool is_Empty(string str){
    trim(str);
    for(int i =0; i < (int)str.length(); i++){
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
            return false;
    }
    return true;
}


int string_to_integer(string str){
    int i;
    stringstream ss;
    ss << str;
    ss >> i;
    return i;
}

/*
 * Delete the left and right whitespace of the string.
 */
string& trim(string &s) {
    if (s.empty()) {
        return s;
    }
    if ( (int)s.find_first_of("#") != -1)
        s.erase(s.find_first_of("#"));
    s.erase(0,s.find_first_not_of(" "));
    s.erase(0,s.find_first_not_of("\t"));
    s.erase(s.find_last_not_of(" ") + 1);
    s.erase(s.find_last_not_of("\t") + 1);
    return s;
}

/*
 * Split the string and store the instruction and registers in the vector.
 */
vector<string> split(string s){
    // cout << "The string in split is " << s << endl;
    string word;
    vector<string> vs;
    trim(s);
    int start = s.find(':');
    if (start > 0)
        s.erase(0,start+1);
    trim(s);
    int end = s.find_first_of(' ');
    word = s.substr(0 ,end);
    vs.push_back(trim(word)); // store the name of instruction;
    s.erase(0, end+1);
    stringstream iss(s);
    while(getline(iss,word,',')){ // split the reset string by ','
        vs.push_back(trim(word));
    }
    return vs;
}

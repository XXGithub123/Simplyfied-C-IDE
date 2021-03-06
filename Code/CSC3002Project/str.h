/*
 * File: str_utils.h
 * -------------------
 * This file implement some function to deal with the string in the input file.
 */

#ifndef STRING_H
#define STRING_H
#include <string>
#include <vector>
#include <sstream>

using namespace std;

/*
 * Judge a string is empty or all whitespace.
 */
bool is_Empty(string str);

/*
 * Convert a string to integer.
 */
int string_to_integer(string str);

/*
 * Delete the left and right whitespace of the string.
 */
string& trim(string &s);

/*
 * Split the string and store the instruction and registers in the vector.
 */
vector<string> split(string s);


#endif

/*
 * File: preprocess.cpp
 * ------------------------
 * Output a txt file with standard C++ code after preprocess.
 */

#include <iostream>
#include <fstream>
//#include "console.h"
#include "string.h"
#include <map>

using namespace std;

/* Function prototypes */

void Preprocess(istream & is, ostream & os);
bool CheckFuncDef(string line);
bool CheckVarDef(string line);
bool CheckLoop(string line);
bool CheckIf(string line);
bool CheckCin(string line);
bool CheckCout(string line);

/* Main program */
/*
int main() {
   std::ifstream infile("..\\article.txt");
   std::ofstream outfile("..\\codeProcessed.txt", ios::trunc);

   Preprocess(infile, outfile);
   infile.close();
   outfile.close();
   cout << "\nThis is a new run9." <<endl;
   return 0;
}*/

/*
 * Function: Preprocess
 * Usage: Preproess(is, os);
 * ------------------------------
 * Copies characters from the input stream is to the output stream os,
 * removing any comments it finds.  This program eliminates both the
 * "/*" and "//" comment forms and check to see if those characters
 * are embedded within strings.
 * Also, this function delete extra whitespaces and add necessary whitespaces
 * to fit the format of compilation next to come.
 */

void Preprocess(istream & is, ostream & os) {
    int ch = '\n';
    int next_ch;
    streampos cursor = 0;                 //start from the begining of the file
    string currentLine;                   //NOTICE: This 'currentLine' would not go through all lines, only those that matter to the CheckFunc ,CheckVar or CheckLoop
    string demoLine;

    while (true) {

        if (ch == EOF) break;              //preprocess ends right after the last digit of the file is processed

        if (ch == '\n') {                  //Get the current line for later to recognize the type of code line before we do any put(),
            cursor = is.tellg();
            getline(is,currentLine);
            is.seekg(cursor);
        }

        if ((next_ch = is.get()) == ' ') {      //detect and delete indent/ws in the front of the line
            while(next_ch == ' ') {
                next_ch = is.get();
            }
            //cout << char(next_ch)<<endl;      //only for test
        }
        is.unget();                         //place outside the if statement because even if the condition is not satisfied, unget() is needed


        while ((ch = is.get()) != '\n') {       //This is a while loop in line unit
            next_ch = is.get();                 //check the next character
            is.unget();                         //move back the current character

            if (ch == EOF) break;               //and will break again immediately from a larger while loop

            //For General line:
            if (ch == '{'){                      //delete all ws right after '{'
                os<<(char(ch));                  //put '{' into os
                if (next_ch == ' '){
                    while (next_ch==' '){
                        next_ch = is.get();
                    }
                    is.unget();
                }
                /*ch = is.get();
                next_ch = is.get();
                is.unget();*/
                continue;
            }
            if (ch == ' ' && next_ch == ';'){
                continue;
            }
            if (ch == ' ' && next_ch == ' '){
                continue;
            }
            if (ch == ')' && next_ch == '{'){
                os<<(char(ch));
                os<<(' ');
                continue;
            }
            if (ch == '\t'){        //delete all non-C++ indent
                continue;
            }

            //For Cin & Cout line:
            if (CheckCin(currentLine)) {
                //add ws
                if (next_ch == '>' && (ch != ' ' && ch != '>')) {
                    os<<(char(ch));
                    os<<(' ');
                    continue;
                }
                if (ch == '>' && (next_ch != ' ' && next_ch != '>')) {
                    os<<(char(ch));
                    os<<(' ');
                    continue;
                }
            }
            if (CheckCout(currentLine)) {
                //add ws
                if (next_ch == '<' && (ch != ' ' && ch != '<')) {
                    os<<(char(ch));
                    os<<(' ');
                    continue;
                }
                if (ch == '<' && (next_ch != ' ' && next_ch != '<')) {
                    os<<(char(ch));
                    os<<(' ');
                    continue;
                }
            }

            //For Function def line:
            if (CheckFuncDef(currentLine) ) {      //delete all extra ws
                //belowed: delete ws
                if (ch == ' '){
                    if (next_ch == ' ' || next_ch == '(' || next_ch == ')' || next_ch == '{'){
                        continue;
                    }
                 }

                if (ch == '(' && next_ch == ' '){
                    while ((next_ch = is.get()) == ' '){
                        //do nothing
                    }
                    is.unget();
                    continue;
                }

                //belowed: add ws
                if (next_ch == ',' && ch != ' ') {
                    os<<(char(ch));
                    os<<(' ');
                    continue;
                }
                if (ch == ',' && next_ch != ' ') {
                    os<<(char(ch));
                    os<<(' ');
                    continue;
                }
            }

            //For declaration line of Variable:
            if (CheckVarDef(currentLine)) {
                if (ch == ' '){
                    if (next_ch == ' '){
                        continue;
                    }
                }
                if (next_ch == '=' && ch != ' '){
                    os<<(char(ch));
                    os<<(' ');
                    continue;
                }
                if (ch == '=' && next_ch != ' ') {
                    os<<(char(ch));
                    os<<(' ');
                    continue;
                }
            }

            //For Conditional Statement line:
            if (CheckLoop(currentLine) || CheckIf(currentLine)) {       //notice that we can only deal with one condition in one statement
                // delete ws
                if (ch == ' '){
                    if (next_ch == ' ' || next_ch == ')' || next_ch == '{'){
                        continue;
                    }
                }
                if (ch == '(' && next_ch == ' '){
                    while ((next_ch = is.get()) == ' '){
                        //do nothing
                    }
                    is.unget();
                    continue;
                }
                // add ws
                if (ch != ' ' && next_ch == '('){
                    os<<(char(ch));
                    os<<(' ');
                    continue;
                }
                if (ch != ' ' && ch != '=' && ch != '!'){
                    if (next_ch == '<' || next_ch == '>' || next_ch == '=' || next_ch == '!'){
                        os<<((char(ch)));
                        os<<(' ');
                        continue;
                    }
                }
                if (ch== '<' || ch == '>'){
                    if(next_ch != ' ' && next_ch != '='){
                        os<<(char(ch));
                        os<<(' ');
                        continue;
                    }
                }
                if (ch == '='){
                    if (next_ch != ' '){
                        os<<(char(ch));
                        os<<(' ');
                        continue;
                    }
                }
            }

            /*Any content inside quotational marks would be put() in these two smaller loops*/
            if (ch == '\'') {            //get everything inside quotation without other disturbance
                while(true) {
                    os<<(char(ch));
                    ch = is.get();
                    if (ch == '\'') break;
                }
                os<<(char(ch));
            }
            else if (ch == '\"') {      //get everything inside quotation
                while(true) {
                    os<<(char(ch));
                    ch = is.get();
                    if (ch == '\"') break;
                }
                os<<(char(ch));
            }

            else if (!((ch == '/' && next_ch == '/') || (ch == '/' && next_ch == '*'))) {       // if not in comments, put the character into the output stream
                os<<(char(ch));
            }
            else if (ch == '/' && next_ch == '*') {                                             // overlook the comments from "/*" until it comes to "*/"
                is.get();               //because after next_ch=is.get() we unget() the inputstream for once, so we need to get one more time to pass '*'
                while (true) {
                    ch = is.get();
                    next_ch = is.get();
                    is.unget();         //move back to the current character
                    if (ch == '*' && next_ch == '/') {
                        ch = is.get();   //skip a next_ch=='/'
                        break;
                    }
                    if (ch == EOF) break;
                }
            }
            else if (ch == '/' && next_ch == '/') {     // overlook the '//' comments until the end of the sentence
                while (true) {
                    ch = is.get();
                    if (ch == EOF || ch == '\n') {
                        is.unget();
                        break;
                    }
                }
            }
        }
        if (ch != EOF || currentLine != "\n"){        //Without this if statement, EOF will be put() too.
            os<<(char(ch));                           //put '\n into output stream; char(ch)=='\n'
        }
     }
}

/*
 * Function: CheckIf
 * Usage: CheckIf(line);
 * ------------------------------
 * Check the key line whether it is a If statement line.
 * Return true if it is, false otherwise.
 */

bool CheckIf(string line){
    if ((line.find("if") != -1) && (line.find('"') == -1)) {
        return true;
    }
    else {
        return false;
    }
}

/*
 * Function: CheckCin
 * Usage: CheckCin(line);
 * ------------------------------
 * Check the key line whether it is a cin line.
 * Return true if it is, false otherwise.
 */

bool CheckCin(string line){
    if ((line.find("cin") != -1) && (line.find(">>") != -1)) {
        return true;
    }
    else {
        return false;
    }
}

/*
 * Function: CheckCout
 * Usage: CheckCout(line);
 * ------------------------------
 * Check the key line whether it is a cout line.
 * Return true if it is, false otherwise.
 */

bool CheckCout(string line){
    if ((line.find("cout") != -1) && (line.find("<<") != -1)) {
        return true;
    }
    else {
        return false;
    }
}

// Belowed are 3 methods this Preprocess uses but defined in translater.cpp
/*  
bool CheckFuncDef(string line){
    if (((line.substr(0,3) == "int") || (line.substr(0,4) == "void") || (line.substr(0,6) == "string")) && line.find('=') == -1) {

        return true;
    }
    else{

        return false;
    }
}

bool CheckVarDef(string line){
    if (((line.find("int") != -1 && line.find('"') == -1) && (line.find('=') != -1)) ||
            ((line.find("string") != -1 && line.find('=') != -1)
             && line.find('"') > line.find("string"))){
        return true;
    }
    else{
        return false;
    }
}

bool CheckLoop(string line){

    if ((line.find("for") != -1 && line.find('"') == -1) ||
         ((line.find("while") != -1 && (line.find('"') == -1 || line.find('"') > line.find("while")))
         && line.find(';') == -1)){

        return true;
    }
    else{
        return  false;
    }
}
*/

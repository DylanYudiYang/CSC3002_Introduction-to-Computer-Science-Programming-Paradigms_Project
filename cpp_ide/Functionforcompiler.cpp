#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <map>
#include <vector>
#include "Functionforcompiler.h"

using namespace std;


void FillRegisterStatus(map<int, string> & type);
//Check the status of Registers
bool CheckRegisters(map<string, bool> RegisterStatus, vector<string> & VacantRegisters);
//Check if this line defines a function
bool CheckFuncDef(string line);
//Check if this line defines a variable
bool CheckVarDef(string line);
//Check if this line starts a logic tree
bool Checklogic(string line);
//Check if this line starts a loop
bool CheckLoop(string line);
//Check the data is variable name or an integer, true for int, false for var
bool CheckType(string datapiece);
//Convert string into an integer
bool CheckFunctionCall(vector<string> Funcs, string line);
//Check  if this line call a funciton
bool is_element_in_vector(vector<string> v,string element);
//Check if this element(string type) is in the vector
int strtoint(string num);
//Caluculate POwer
int power(int x, unsigned y);
//Find Variable via Registers{
int countsubstr(string line, char substring);
//Return the number of the substring in the string
string LocateRegs(string RegName, vector<string> Variablegroups,  map<string, string> Registers);
//Find variable names via Register names
void VariableClassify(string variable, string Register, vector<string> & v1, vector<string> & v2, vector<string> & v3);
//Classify the currently using register
////////////////////////////////////////////////////START OF FUNCTION DEFINITION /////////////////////////////////////////
bool CheckFuncDef(string line){
    if (((line.substr(0,3) == "int") || (line.substr(0,4) == "void") ||
          (line.substr(0,6) == "string")) && line.find('=') == -1){

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

bool Checklogic(string line){

    if(line.find("if") != -1 || line.find("else") != -1){

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


//add empty registers into VancantRegisters and return false if all are occupied
bool CheckRegisters(map<string, bool> RegisterStatus, vector<string> & VacantRegisters){
    /*The vector VacantRegisters refreshes every time before the translation of this line,
     * to offer the information of register currently available
     */
    VacantRegisters.clear();
    map<string, bool> ::iterator iter;

    for(iter=RegisterStatus.begin(); iter!=RegisterStatus.end(); iter++){
        if (bool vacant = iter->second == true){
            VacantRegisters.push_back(iter->first);
        }
    }

    return !(VacantRegisters.empty());
}

//Return true if tthe calculation ia about an iteger, false if it is about variable
bool CheckType(string datapiece){

    vector<char> nums = {'-', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};

    for (int i = 0; i < datapiece.length(); i++){

        if (count(nums.begin(), nums.end(), datapiece[i]) == 0){

            return false;
        }
        else{

            return true;
        }
    }
}

bool CheckFunctionCall(vector<string> Funcs, string line){

    // Basically to types of funtion call
    //CALL of int function
    //CALL of void funtion
    string funcname;

    if((line.find('=') != -1 && line.find('+') == -1  && line.find('-') == -1
            && line.find('/') == -1  && line.find('%') == -1  && line.find('*') == -1)
       ||(line.find('=') == -1 && is_element_in_vector(Funcs, (line.substr(line.find_first_not_of(' '), line.find('(') - line.find_first_not_of(' ')))))){

        if (line.find('=') != -1){

            int funchead = line.find('=') + 2;
            int functail = line.find('(');
            funcname = line.substr(funchead, functail - funchead);
        }
        else{

            int funchead = line.find_first_not_of(' ');
            int functail = line.find('(');
            funcname = line.substr(funchead, functail - funchead);

        }


        if (is_element_in_vector(Funcs, funcname)){

            return true;
        }
        else{
            return false;
        }

    }
    else{
        return false;
    }

}


bool is_element_in_vector(vector<string> v,string element){
    vector<string>::iterator it;
    it=find(v.begin(),v.end(),element);
    if (it!=v.end()){
        return true;
    }
    else{
        return false;
    }
}


int strtoint(string num){
    unsigned len(num.length());
    int temp(0);
    if (num[0] == '-'){

        for (unsigned i = 1; i < len; i++){
            if (num[i] == '1'){
                temp += 1*power(10,len-1-i);
            }
            if (num[i] == '2'){
                temp += 2*power(10,len-1-i);
            }
            if (num[i] == '3'){
                temp += 3*power(10,len-1-i);
            }
            if (num[i] == '4'){
                temp += 4*power(10,len-1-i);
            }
            if (num[i] == '5'){
                temp += 5*power(10,len-1-i);
            }
            if (num[i] == '6'){
                temp += 6*power(10,len-1-i);
            }
            if (num[i] == '7'){
                temp += 7*power(10,len-1-i);
            }
            if (num[i] == '8'){
                temp += 8*power(10,len-1-i);
            }
            if (num[i] == '9'){
                temp += 9*power(10,len-1-i);
            }
            if (num[i] == '0'){
                temp += 0;
            }
        }

        return temp* -1;

    }
    else {

        for (unsigned i = 0; i < len; i++){
            if (num[i] == '1'){
                temp += 1*power(10,len-1-i);
            }
            if (num[i] == '2'){
                temp += 2*power(10,len-1-i);
            }
            if (num[i] == '3'){
                temp += 3*power(10,len-1-i);
            }
            if (num[i] == '4'){
                temp += 4*power(10,len-1-i);
            }
            if (num[i] == '5'){
                temp += 5*power(10,len-1-i);
            }
            if (num[i] == '6'){
                temp += 6*power(10,len-1-i);
            }
            if (num[i] == '7'){
                temp += 7*power(10,len-1-i);
            }
            if (num[i] == '8'){
                temp += 8*power(10,len-1-i);
            }
            if (num[i] == '9'){
                temp += 9*power(10,len-1-i);
            }
            if (num[i] == '0'){
                temp += 0;
            }
        }
        return temp;
    }

}

int power(int x, unsigned y){
    int temp(1);
    for (unsigned i = 0; i < y; i ++){
        temp = temp*x;
    }
    return temp;
}

int countsubstr(string line, char substring){

    int counter = 0;

    for(int i = 0; i < line.length(); i++){

        if(line[i] == substring){

            counter++;
        }
    }

    return counter;

}

//Find variable name via register Name
string LocateRegs(string RegName, vector<string> Variablegroups,  map<string, string> Registers){

    for(int i = 0; i < Variablegroups.size(); i++){

        if (Registers[Variablegroups[i]] == RegName){

            return Variablegroups[i];
        }

    }


}

void VariableClassify(string variable, string Register, vector<string> & v1, vector<string> & v2, vector<string> & v3){

    if (Register.find("$a") != -1){

        v1.push_back(variable);

    }
    else if(Register.find("$t") != -1){

        v2.push_back(variable);
    }
    else if(Register.find("$s") != -1){

         v3.push_back(variable);
    }


}

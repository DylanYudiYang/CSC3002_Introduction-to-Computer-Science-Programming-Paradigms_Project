#ifndef FUNCTIONFORCOMPILER_H
#define FUNCTIONFORCOMPILER_H

#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <map>
#include <vector>

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

bool is_element_in_vector(vector<string> v,string element);

int strtoint(string num);
//Caluculate POwer
int power(int x, unsigned y);
//Find Variable via Registers{
int countsubstr(string line, char substring);
//Return the number of the substring in the string

string LocateRegs(string RegName, vector<string> Variablegroups,  map<string, string> Registers);

void VariableClassify(string variable, string Register, vector<string> & v1, vector<string> & v2, vector<string> & v3);
#endif // FUNCTIONFORCOMPILER_H

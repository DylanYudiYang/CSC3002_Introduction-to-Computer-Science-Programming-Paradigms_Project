#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <iostream>
#include <fstream>
//#include "console.h"
#include "string.h"
#include <map>

using namespace std;

//The integrated preprocess function
void Preprocess(istream & is, ostream & os);

//Check whether it's an 'If' statement line
bool CheckIf(string line);

//Check whether it's a 'Cin' line
bool CheckCin(string line);

//Check whether it's a 'Cout' line
bool CheckCout(string line);

#endif

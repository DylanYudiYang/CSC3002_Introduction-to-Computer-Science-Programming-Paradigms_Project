#ifndef BUGCHECK_H
#define BUGCHECK_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "Translater.h"

using namespace std;

void Fillmap(map<int, string> & type);

bool Debugger(ifstream & Program, map<int, string> & BugType, map<int, vector<int> > & BugPosition, vector<int> & bugline );

void addtovector(vector<int> ints, int newint);

#endif // BUGCHECK_H

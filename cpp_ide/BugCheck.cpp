#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "Functionforcompiler.h"

using namespace std;

void Fillmap(map<int, string> & type);

bool Debugger(ifstream & Program, map<int, string> & BugType, map<int, vector<int> > & BugPosition, vector<int> & bugline );

void addtovector(vector<int> ints, int newint);


void Fillmap(map<int, string> & type){

    type.insert(pair<int, string>(1, "expect { before expression"));
    type.insert(pair<int, string>(2, "expect } after expression"));
    type.insert(pair<int, string>(3, "expect ; after expression"));
    type.insert(pair<int, string>(4, "expect statement"));
    type.insert(pair<int, string>(5, "expect ( before statement"));
    type.insert(pair<int, string>(6, "expect ) after statement"));
    type.insert(pair<int, string>(7, "syntax error"));
    type.insert(pair<int, string>(8, "call for undefined function"));

}

bool Debugger(ifstream & Program, map<int, string> & BugType, map<int, vector<int> > & BugPosition, vector<int> & bugline ){


    Fillmap(BugType);

    string line;
    int PotentialBugSign = 0;
    int FunctionHeadSign = 0;
    int PositionHolder = 0;

    int linecounter = 1;
    vector <int> upperBraces;
    vector <int> lowerBraces;
    vector <int> Funclines;
    bool Initializationsection(true);
    vector <string> Functions;
    map <string, int> ParaofFuncs;



    while(getline(Program, line)){
        if (line.find("#") != -1){

            continue;
        }
        if (line.size() == 0){
            continue;
        }
        //Check ';'

        if (line.find(';') == -1){

            if(line.find('{') == -1 && line.find('}') == -1 && line.find('(') && line.find(')')){

                BugPosition[linecounter].push_back(3);
                addtovector(bugline, linecounter);
            }
        }

        //Check Brace
        if (line.find('{')){
            upperBraces.push_back(linecounter);

        }
        if (line.find('}')){
            lowerBraces.push_back(linecounter);

        }
        if (CheckFuncDef(line)){

            Funclines.push_back(linecounter);
        }


        //Check Statement
        if ((line.find("if") != -1) || (line.find("if") != -1 && line.find("else") != -1) ||
                ((line.find("int") != -1 || line.find("string") != -1 || line.find("void") != -1) && line.find('=') == -1)){

            if (line.find("main") == line.find('(') -4) {

                continue;
            }

            if (line.find('(') != -1 && line.find(')') == -1){

                BugPosition[linecounter].push_back(6);
                addtovector(bugline, linecounter);
            }
            else if (line.find(')') != -1 && line.find('(') == -1){

                BugPosition[linecounter].push_back(5);
                addtovector(bugline, linecounter);
            }
            else if (line.find('(') == -1 && line.find(')') == -1){

                BugPosition[linecounter].push_back(4);
                addtovector(bugline, linecounter);
            }
            else{

                if (line.find('(') == line.find(')') -1){

                    BugPosition[linecounter].push_back(4);
                    addtovector(bugline, linecounter);
                }
            }
        }

        //Check Assignment

        if ((line.find("int") != -1 || line.find("string") != -1) && line.find('=') != -1){

            if (line.find("int") != -1){

                if (line.find('\'') != -1){

                    BugPosition[linecounter].push_back(7);
                    addtovector(bugline, linecounter);
                }
                else {

                    string valuetemp = line.substr(line.find('=')+2, line.find(';')-line.find('=')-2);
                    if(!(CheckType(valuetemp))){

                        BugPosition[linecounter].push_back(7);
                        addtovector(bugline, linecounter);

                    }

                }
            }
            else{

                if (line.find('"') == -1){

                    BugPosition[linecounter].push_back(7);
                    addtovector(bugline, linecounter);

                }
            }

        }

        //Check call of function
        if (CheckFuncDef(line)){

            if (Initializationsection){

                if (line.substr(4,4) == "main"){
                    Initializationsection = false;
                }
                else{

                    int Nametail;
                    string fName;
                    char ps = ',';
                    int paraCounter = countsubstr(line, ps) + 1;

                    if (line.substr(0,3) == "int"){

                        Nametail = line.find('(');
                        fName = line.substr(4, Nametail-4);
                        Functions.push_back(fName);
                        //cout << "PRCTER" << paraCounter;


                    }
                    else if (line.substr(0,4) == "void"){

                        Nametail = line.find('(');
                        fName = line.substr(5, Nametail-5);
                        Functions.push_back(fName);
                        ParaofFuncs[fName] = paraCounter;
                        cout << "PRCTER" << fName;



                    }
                    else if (line.substr(0,6) == "string"){

                        Nametail = line.find('(');
                        fName = line.substr(7, Nametail-7);
                        Functions.push_back(fName);
                        ParaofFuncs[fName] = paraCounter ;


                    }

                }

            }


            if((line.find('=') != -1 && line.find('+') == -1  && line.find('-') == -1
                    && line.find('/') == -1  && line.find('%') == -1  && line.find('*') == -1)
               ||(line.find('=') == -1 && line.find("int") && line.find(">>") == -1 && line.find("<<") == -1)){

                string funcname;

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
                cout << funcname << "GGS" << endl;

                if (is_element_in_vector(Functions, funcname)){

                    char ps = ',';
                    int paraCounter = countsubstr(line, ps) + 1;

                    if (ParaofFuncs[funcname] == countsubstr(line, ps) + 1){

                        continue;
                    }
                    else{

                        BugPosition[linecounter].push_back(8);
                    }
                }
                else{
                   BugPosition[linecounter].push_back(8);
                }

            }

        }







        linecounter ++;
    }
/*
    for(int i = 0; i < Funclines.size(); i++){

        if( i == Funclines.size()-1){
            vector<int> upper;
            vector<int> lower;

            for (int q = 0; q < upperBraces.size(); i++){

                if(upperBraces[q] >= Funclines[i]){

                    upper.push_back(upperBraces[q]);
                }


            }
            for (int q = 0; q < lowerBraces.size(); i++){

                if(lowerBraces[q] >= Funclines[i]){

                    lower.push_back(lowerBraces[q]);
                }

            }

            if (upper.size() > lower.size()){

                BugPosition[lowerBraces[lowerBraces.size()-1]].push_back(2);
            }
            else if (upper.size() < lower.size()){

                BugPosition[Funclines[i]].push_back(1);
            }

        }
        else{

            vector<int> upper;
            vector<int> lower;

            for (int q = 0; q < upperBraces.size(); i++){

                if(upperBraces[q] >= Funclines[i] && upperBraces[q] < Funclines[i+1]){

                    upper.push_back(upperBraces[q]);
                }


            }
            for (int q = 0; q < lowerBraces.size(); i++){

                if(lowerBraces[q] >= Funclines[i] && lowerBraces[q] < Funclines[i+1]){

                    lower.push_back(lowerBraces[q]);
                }

            }
            if (upper.size() > lower.size()){

                BugPosition[Funclines[i+1]-1].push_back(2);
            }
            else if (upper.size() < lower.size()){

                BugPosition[Funclines[i]].push_back(1);
            }


        }
    }
*/

    if (BugPosition.size() == 0){
        return true;
    }
    else{
        return false;
    }
}

void addtovector(vector<int> ints, int newint){

    int sign = 0;

    for(int i = 0; i < ints.size(); i++){

        if (ints[i] == newint){
            sign = 1;
        }
    }

    if(sign == 0){

        ints.push_back(newint);
    }


}



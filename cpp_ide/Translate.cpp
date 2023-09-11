#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <map>
#include <vector>
#include "Functionforcompiler.h"


using namespace std;


void Translator(string instream, ofstream & MIPSProgram, map<string, bool> & RegisterStatus);

//This Function Initialize the status of the Registers
void FillRegisterStatus(map<string, bool> & Regs){

    Regs.insert(pair<string, bool>("$a0", true));
    Regs.insert(pair<string, bool>("$a1", true));
    Regs.insert(pair<string, bool>("$a2", true));
    Regs.insert(pair<string, bool>("$a3", true));
    Regs.insert(pair<string, bool>("$t0", true));
    Regs.insert(pair<string, bool>("$t1", true));
    Regs.insert(pair<string, bool>("$t2", true));
    Regs.insert(pair<string, bool>("$t3", true));
    Regs.insert(pair<string, bool>("$t4", true));
    Regs.insert(pair<string, bool>("$t5", true));
    Regs.insert(pair<string, bool>("$t6", true));
    Regs.insert(pair<string, bool>("$t7", true));
    Regs.insert(pair<string, bool>("$s1", true));
    Regs.insert(pair<string, bool>("$s2", true));
    Regs.insert(pair<string, bool>("$s3", true));
    Regs.insert(pair<string, bool>("$s4", true));
    Regs.insert(pair<string, bool>("$s5", true));
    Regs.insert(pair<string, bool>("$s6", true));
    Regs.insert(pair<string, bool>("$s7", true));
    Regs.insert(pair<string, bool>("$t8", true));
    Regs.insert(pair<string, bool>("$t9", true));
}

//The function translate C++ into MIPS asm
void Translator(string instream, ofstream & MIPSProgram, map<string, bool> & RegisterStatus){

    //A vector use to record the register names
    vector<string> RegisterNames = {"$a0","$a1","$a2", "$a3", "$t0",
                                   "$t1", "$t2", "$t3", "$t4", "$t5",
                                   "$t6", "$t7", "$s1", "$s2", "$s3",
                                   "$s4", "$s5", "$s6", "$s7", "$t8", "$t9",};

    //Initialize the status of Registers
    FillRegisterStatus(RegisterStatus);

    //Open C++ File
    ifstream CProgram1(instream);

    string line;
    int StringCount(0);

    map<int, int> SizeofString;
    map<string, int> StringIcon;

    MIPSProgram << ".data" << endl;

    //ADD DATA SECTION
    while(getline(CProgram1, line)){

        if(line.find("cout") != -1 && line.find("<<") != -1 && line.find('"') != -1){

            //Find the Output String
            int PosStart = line.find('"') + 1;
            int StrLength = line.find_last_of('"') - PosStart + 1;

            StringCount ++;
            SizeofString.insert(pair<int, int>(StringCount, StrLength + 1 + ((StrLength + 1) % 4 != 0)));
            StringIcon.insert(pair<string, int>(line.substr(PosStart, StrLength), StringCount));

            MIPSProgram <<"str"<< StringCount << ": .asciiz " << '"' << line.substr(PosStart, StrLength) << '"' <<endl;

        }
    }
    CProgram1.close();
    ifstream CProgram(instream);


    //ADD TEXT SECTION
    /* THE BASIC LOGIC
     * FOR A LINE OF CODE
     * CHECK THE VARIABLE -> NEW VARIABLE ->CHECK IF THERE ARE REIGISTERS AVAILABLE -> YES -> LOAD DATA-----
     *                 |                                                  ^      ^      |                  |
     *                 |                                                  |      |     NO---> SAVE DATA    |
     *                 ->  OLD VARIABLE -> CHECK IF IT IS IN REGISTERS ->NO      |                |        |
     *                                              |                            -----------------         |
     *                                              |                                                      |
     *                                              v                                                      |
     *                                             YES --------> TRANSLATE <-------------------------------
     *
     *
     * FOR CALL OF FUNCTION
     *
     * SAVE ALL THE PROGRESS -> LOAD DATA REQUIRED BY PARAMETERS -> JUMP TO FUNCTION BODY -> RETURN RESULT -> RESTORE PROGRESS
     *
     *
     *
     */

     MIPSProgram << ".text" << endl;

     vector<string> Variables;   //Store the name of variables
     vector<string> activeVariable; // Store variables in $as
     vector<string> Variableinst;   // Store variables in $ts
     vector<string> Variableinss;   // Store variables in $ss
     map<string, string> VariablesinRegs; // Store variables currently in which register variable name-> REG
     map<string, int> VariableinMem;      // Store the memory positions of variables     variable name-> MEM position
     map<string, int> ParaofFuncs;

     int LogicTreeCounter(0);
     int LogicBranchCounter(0);
     int memposition(0);

     bool inLoop;
     bool inLogicTree;
     bool inLogicBranch;
     bool inFunction;
     bool Initializationsection(true);
     bool mainfunc(true);


     vector<string> recentlyusedRegs; //Registers used in last line of code
     vector<string> VacantRegisters;  //Currently Available Registers
     vector<string> Functions;



     while(getline(CProgram, line)){

         //IGNORE EMPTY LINES
         if (line.size() == 0){

             continue;
         }
         //IF THIS LINE IS DEFINITION OF A FUNC
         else if (CheckFuncDef(line)){

             //IF THIS LINE IS INITIALIZATION OF A FUNCITON
             if (Initializationsection){

                 if (line.substr(4,4) == "main"){

                     MIPSProgram << "main:  ";
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
                         ParaofFuncs[fName] = paraCounter;
                         //cout << "PRCTER" << paraCounter;


                     }
                     else if (line.substr(0,4) == "void"){

                         Nametail = line.find('(');
                         fName = line.substr(5, Nametail-5);
                         Functions.push_back(fName);
                         ParaofFuncs[fName] = paraCounter;
                        // cout << "PRCTER" << fName;



                     }
                     else if (line.substr(0,6) == "string"){

                         Nametail = line.find('(');
                         fName = line.substr(7, Nametail-7);
                         Functions.push_back(fName);
                         ParaofFuncs[fName] = paraCounter ;


                     }

                 }

             }

             else{

                 //END OF MAIN FUNCITON, ADD EXIT INSTRUCTION
                 if(mainfunc == true){
                     MIPSProgram << "    addi $v0, $zero, 10" << endl;
                     MIPSProgram << "    syscall" << endl;
                     mainfunc = false;

                 }

                 //TRANSLATION OF FUNCTION BODY
                 /* The translation of a fucntion is basically the same as other codes
                  * The differences is that before the start of execution of a function
                  * The progress in the main program will be saved
                  * Therefore, apart from the registers that are used to load parameters,
                  * ALl registers are available
                  *
                  * HOWEVER
                  * Since the translation of function body is usually at the end of a program,
                  * Every time finish the translation of a function, ALL information of registers
                  * and variables will be cleared
                  */

                 int Nametail;
                 string fName;


                 if (line.substr(0,3) == "int"){

                     Nametail = line.find('(');
                     fName = line.substr(4, Nametail-4);
                     MIPSProgram << fName << ":";


                 }
                 else if (line.substr(0,4) == "void"){

                     Nametail = line.find('(');
                     fName = line.substr(5, Nametail-5);
                     MIPSProgram << fName << ":";



                 }
                 //THOUGH THIS PROGRAM DOES NOT SUPPORT STRING TYPE VARIABLE, IT IS LISTED AS A CASE
                 else if (line.substr(0,6) == "string"){

                     Nametail = line.find('(');
                     fName = line.substr(7, Nametail-7);
                     MIPSProgram << fName << ":";


                 }



                 RegisterStatus.clear();
                 FillRegisterStatus(RegisterStatus);
                 activeVariable.clear();
                 Variableinss.clear();
                 Variableinst.clear();
                 VariablesinRegs.clear();

                 CheckRegisters(RegisterStatus, VacantRegisters);

                 vector<string> varnames;


                 int newstart = 0;

                 //CHECK PARAMETERS
                 // Pair parameters and register

                 for(int q = 0; q < ParaofFuncs[fName]; q++){



                     if (q == 0){

                         varnames.push_back(line.substr(line.find('(')+5, line.find(',') - line.find('(') - 5 ));
                         newstart = line.find(',')+6;

                     }
                     else if (q == ParaofFuncs[fName] -1){

                         varnames.push_back(line.substr(line.find_last_of(',')+6, line.find(')') - line.find_last_of(',')-6));

                     }
                     else {

                         string newline = line.substr(newstart, line.length()-newstart);

                         varnames.push_back(newline.substr(0, newline.find(',')));
                         newstart = newline.find(',')+6;
                     }
                 }

                 for(int q = 0; q < varnames.size(); q ++){

                     VariablesinRegs[varnames[q]] = VacantRegisters[q];
                     RegisterStatus[VacantRegisters[q]] = false;
                     VariableClassify(varnames[q], VacantRegisters[q], activeVariable, Variableinst, Variableinss);


                 }

                 for(int q = 0; q < varnames.size(); q++){


                 }





             }
         }
         //IF THIS LINE IS DEFINITION OF A VAR
         /////////////////////////////////////////START OF INITIALIZING A NEW DATA///////////////////////
         else if (CheckVarDef(line)){
             //cout << "usee" << endl;

            int namehead = line.find("int") + 4;
            int nametail = line.find('=') -1;
            string varname = line.substr(namehead, nametail - namehead);
            Variables.push_back(varname);
            //cout << varname <<endl;

            int valuehead = line.find_last_of('=') + 2;
            int valuetail = line.find(';');
            int newvalue = strtoint(line.substr(valuehead, valuetail - valuehead));
            //IF there are available registers

            if (CheckRegisters(RegisterStatus, VacantRegisters)){
                // DIRECTLY PUT IN $a
                if(VacantRegisters[0].find("$a") != -1){

                    string reg = VacantRegisters[0];

                    MIPSProgram << "    addi " << VacantRegisters[0] <<", $zero, " << line.substr(valuehead, valuetail - valuehead) << endl;
                    VariablesinRegs.insert(pair<string,string>(varname, VacantRegisters[0]));
                    RegisterStatus[reg] = false;
                    activeVariable.push_back(varname);
                    recentlyusedRegs = {reg};

                }
                // DIRECTLY PUT IN $t
                else if(VacantRegisters[0].find("$t") != -1){

                    string reg = VacantRegisters[0];

                    MIPSProgram << "    addi " << VacantRegisters[0] <<", $zero, " << line.substr(valuehead, valuetail - valuehead) << endl;
                    VariablesinRegs.insert(pair<string,string>(varname, VacantRegisters[0]));
                    RegisterStatus[reg] = false;
                    Variableinst.push_back(varname);
                    recentlyusedRegs = {reg};


                }
                // DIRECTLY PUT IN $s
                else if(VacantRegisters[0].find("$s") != -1){

                    string reg = VacantRegisters[0];

                    MIPSProgram << "    addi " << VacantRegisters[0] << "," << line.substr(valuehead, valuetail - valuehead) << endl;
                    VariablesinRegs.insert(pair<string,string>(varname, VacantRegisters[0]));
                    RegisterStatus[reg] = false;
                    Variableinss.push_back(varname);
                    recentlyusedRegs = {reg};


                }
            }

            // IF ALL REGS ARE OCCUPIED
            else {

                //SAVE ALL THE DATA IN $S to Memory

                for(int v = 7; v >= 0; v --){

                    string reg = VariablesinRegs[Variableinss[v]];
                    MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                    VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                    VariablesinRegs.erase(Variableinss[v]);
                    RegisterStatus[reg] = true;
                    Variableinss.pop_back();

                    memposition = memposition + 4;

                }


                MIPSProgram << "    addi " << "$s0" <<", $zero, " << line.substr(valuehead, valuetail - valuehead) << endl;
                VariablesinRegs.insert(pair<string,string>(varname, "$s0"));
                RegisterStatus["$s0"] = false;
                Variableinss.push_back(varname);
                recentlyusedRegs = {"$s0"};

                }


         /////////////////////////////////////////END OF INITIALIZING A NEW DATA///////////////////////


         }

         //OTHER NORMAL OPERATIONS
         else if (CheckLoop(line)){
             //LEFT EMPTY
         }
         else if (Checklogic(line)){
             //LEFT EMPTY
         }
         else{
             //PROGRAM CALL FOR PRINT DATA
             if (line.find("cout") != -1 && line.find("<<") != -1){
                 //PRINT STRINGS
                 /////////////////////////////////START OF PRINTING STRINGS////////////////////////////////
                 if (line.find('"') != -1){

                     int PosStart = line.find('"') + 1;
                     int StrLength = line.find_last_of('"') - PosStart + 1;
                     int stringposition = 0;

                     string output = line.substr(PosStart, StrLength);
                     for(int i = 1; i < StringIcon[output]; i++){

                         stringposition = stringposition + SizeofString[StringIcon[output]];

                     }

                     //IF $a0 is available
                     if(RegisterStatus["$a0"]){

                         MIPSProgram << "    lui $a0, 80" <<endl;
                         MIPSProgram << "    ori $a0, $a0, "<< stringposition << endl;
                     }
                     //IF $a0 is occupied but there is empty reg
                     else{
                         string variable = LocateRegs("$a0", activeVariable, VariablesinRegs);
                         vector<string>::iterator iter = find(activeVariable.begin(), activeVariable.end(), variable);
                         activeVariable.erase(iter);

                         if(CheckRegisters(RegisterStatus, VacantRegisters)){

                             MIPSProgram << "    addi "<< VacantRegisters[0] << " ," << "$a0, 0" << endl;
                             RegisterStatus[VacantRegisters[0]] = false;
                             recentlyusedRegs = {"$a0", VacantRegisters[0]};
                             VariablesinRegs[variable] = VacantRegisters[0];

                             if (VacantRegisters[0].find("$s") != -1){

                                 Variableinss.push_back(variable);

                             }
                             else if(VacantRegisters[0].find("$t") != -1){

                                  Variableinst.push_back(variable);

                             }
                         }
                         //If $a0 is occupied but all registers is occupied
                         else{

                             //SAVE ALL THE DATA IN $S to Memory

                             for(int v = 7; v >= 0; v --){

                                 string reg = VariablesinRegs[Variableinss[v]];
                                 MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                 VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                 VariablesinRegs.erase(Variableinss[v]);
                                 RegisterStatus[reg] = true;
                                 Variableinss.pop_back();

                                 memposition = memposition + 4;

                             }

                             MIPSProgram << "    addi $s0, $a0, 0" << endl;
                             VariablesinRegs.insert(pair<string,string>(variable, "$s0"));
                             RegisterStatus["$s0"] = false;
                             Variableinst.push_back(variable);
                             recentlyusedRegs = {"$s0", "$a0"};

                         }

                         //LOAD THE BOTTOM OF STACK DATA

                         MIPSProgram << "    lui $a0, 80" <<endl;
                         MIPSProgram << "    ori $a0, $a0, "<< stringposition << endl;


                     }
                     //CALL FOR PRINT DATA

                     MIPSProgram << "    addi $v0, $zero, 4" << endl;
                     MIPSProgram << "    syscall" << endl;

                 }
             /////////////////////////////////END OF PRINTING STRINGS////////////////////////////////

                 //PRINT INT
                 /////////////////////////////////START OF PRINTING INTS/////////////////////////////////
                 else if (line.find('"') == -1){

                     int varhead = line.find("<<") + 3;
                     int vartail = line.find_last_of("<<") - 2;
                     string variable = line.substr(varhead, vartail-varhead);
                     /*
                     cout << variable <<endl;
                     cout <<'@' <<endl;
                     cout << activeVariable[1] << endl;
                     cout <<'#' <<endl;
                     cout <<is_element_in_vector(activeVariable, variable)<< endl;
                     */

                     if (is_element_in_vector(activeVariable, variable)){

                         //IF the data to print is already in $a

                         if (RegisterStatus["$a0"]){

                             MIPSProgram << "    addi $a0, " << VariablesinRegs[variable] <<", 0"<<endl;
                         }
                         else{
                             //Temperarily use $a0, load back data after use

                             MIPSProgram << "    sw $a0, "<<memposition << "($sp)" << endl;
                             MIPSProgram << "    addi $a0, " << VariablesinRegs[variable] <<", 0"<<endl;

                         }


                     }
                     //IF the data to print is already in $t
                     else if(is_element_in_vector(Variableinst, variable)){

                         if (RegisterStatus["$a0"]){

                             MIPSProgram << "    addi $a0, " << VariablesinRegs[variable] <<", 0"<<endl;
                         }
                         else{

                             //Temperarily use $a0, load back data after use
                             MIPSProgram << "    sw $a0, "<<memposition << "($sp)" << endl;
                             MIPSProgram << "    addi $a0, " << VariablesinRegs[variable] <<", 0"<<endl;

                         }

                     }
                     //IF the data to print is already in $s
                     else if(is_element_in_vector(Variableinss, variable)){

                         if (RegisterStatus["$a0"]){

                             MIPSProgram << "    addi $a0, " << VariablesinRegs[variable] <<", 0"<<endl;
                         }
                         else{

                             //Temperarily use $a0, load back data after use
                             MIPSProgram << "    sw $a0, "<<memposition << "($sp)" << endl;
                             MIPSProgram << "    addi $a0, " << VariablesinRegs[variable] <<", 0"<<endl;

                         }

                     }
                     else{

                         //Temperarily use $a0, load back data after use
                         MIPSProgram << "    sw $a0, "<<memposition << "($sp)" << endl;
                         MIPSProgram << "    lw $a0, "<<VariableinMem[variable] << "($sp)" << endl;

                     }

                     MIPSProgram << "    addi $v0, $zero, 1" << endl;
                     MIPSProgram << "    syscall" << endl;
                     MIPSProgram << "    lw $a0, " << memposition << "($sp)" << endl;
                     //LOAD BACK DATA TO $a0 AFTER USE
                 }

                 /////////////////////////////////END OF PRINTING INTS/////////////////////////////////
            }
             /////////////////////////////////END OF PRINTING DATA/////////////////////////////////////
             /////////////////////////////////START OF READING DATA/////////////////////////////////////
             else if (line.find("cin") != -1 && line.find(">>") != -1){

                 int variablehead = line.find(">>") + 3;
                 int variabletail = line.find(';');

                 string variable = line.substr(variablehead, variablehead - variabletail);
                 string reg;

                 if ((is_element_in_vector(activeVariable, variable))
                 || (is_element_in_vector(Variableinst, variable))
                 || (is_element_in_vector(Variableinss, variable)) ){

                     reg = VariablesinRegs[variable];

                 }
                 else{
                     //IF THERE ARE AVAILABLE REGISTERS
                     if(CheckRegisters(RegisterStatus, VacantRegisters)){

                         reg = VacantRegisters[0];

                         MIPSProgram << "    lw"<<reg << ", "<< VariableinMem[variable] <<"($sp)" <<endl;
                         RegisterStatus[reg] = false;
                         recentlyusedRegs = {reg};
                         VariablesinRegs[variable] = reg;
                         VariableClassify(variable, reg, activeVariable, Variableinst, Variableinss);

                     }
                     else{
                         //IF REGISTERS ARE FULLY OCCUPIED

                         //SAVE ALL THE DATA IN $S to Memory
                         for(int v = 7; v >= 0; v --){

                             string reg = VariablesinRegs[Variableinss[v]];
                             MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                             VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                             VariablesinRegs.erase(Variableinss[v]);
                             RegisterStatus[reg] = true;
                             Variableinss.pop_back();

                             memposition = memposition + 4;

                         }


                         //TEMPERARILY USE $a0
                         MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                         VariablesinRegs.insert(pair<string,string>(variable, "$s0"));
                         RegisterStatus["$s0"] = false;
                         Variableinss.push_back(variable);
                         recentlyusedRegs = {"$s0"};
                         reg = "$s0";

                     }

                     MIPSProgram << "    addi $v0, $zero, 5" << endl;
                     MIPSProgram << "    addi "<< reg <<", $v0, 0"<< endl;

                 }







             }
             //BASIC MATH CALCULATION
             //TOTALLY FIVE KINDS OF CALCULATION IS SUPPORTED
             //THEY BASICALLY FOLLOW THE SAME PROCESS
             else if(line.find('=') != -1 &&
                     (line.find('+') != -1 || line.find('-') != -1 || line.find('*') != -1 ||
                      line.find('/') != -1 || line.find('%') != -1)){

                 if (line.find('+') != -1 && line.find('+') > line.find('=')){

                     string variable1 = line.substr(line.find_first_not_of(' '), line.find('=')-1-line.find_first_not_of(' '));
                     string variable2 = line.substr(line.find('=')+2, line.find('+')-3-line.find('='));
                     string variable3 = line.substr(line.find('+')+2, line.find(';')-2-line.find('+'));

                     string reg1;
                     string reg2;
                     string reg3;

                     //First Check the variable on the left side of the '='

                     //If this variable is in registers
                     if ((is_element_in_vector(activeVariable, variable1))
                             || (is_element_in_vector(Variableinst, variable1))
                             || (is_element_in_vector(Variableinss, variable1))){

                         reg1 = VariablesinRegs[variable1];


                     }
                     //If it is in meory
                     else{
                         //Check if there are available registers

                         if(CheckRegisters(RegisterStatus, VacantRegisters)){

                             reg1 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg1 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg1] = false;
                             recentlyusedRegs = {reg1};
                             VariablesinRegs[variable1] = reg1;
                             VariableClassify(variable1, reg1, activeVariable, Variableinst, Variableinss);

                         }

                         //If registers are fully occupied, store all the data in $ss registers into memory
                         else{

                             for(int v = 7; v >= 0; v --){

                                 string reg = VariablesinRegs[Variableinss[v]];
                                 MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                 VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                 VariablesinRegs.erase(Variableinss[v]);
                                 RegisterStatus[reg] = true;
                                 Variableinss.pop_back();

                                 memposition = memposition + 4;

                             }


                             MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                             VariablesinRegs.insert(pair<string,string>(variable1, "$s0"));
                             RegisterStatus["$s0"] = false;
                             Variableinss.push_back(variable1);
                             recentlyusedRegs = {"$s0"};
                             reg1 = "$s0";

                         }

                     }

                     //Then Check the right side of '='



                     if(CheckType(variable2) && CheckType(variable3)){
                         //IF the two data are all integers

                         MIPSProgram << "    addi " << reg1 << ", $zero, " << strtoint(variable2) + strtoint(variable3) << endl;

                     }
                     //If both of them are variables
                     else if(!(CheckType(variable2)) && !(CheckType(variable3))){

                         //If Both of the variasbles are in registers

                         if(((is_element_in_vector(activeVariable, variable2))
                             || (is_element_in_vector(Variableinst, variable2))
                             || (is_element_in_vector(Variableinss, variable2)) )
                                 &&((is_element_in_vector(activeVariable, variable3))
                                    || (is_element_in_vector(Variableinst, variable3))
                                    || (is_element_in_vector(Variableinss, variable3)))){
                             reg2 = VariablesinRegs[variable2];
                             reg3 = VariablesinRegs[variable3];

                         }
                         //If one of the variable is in memory

                         else if(((is_element_in_vector(activeVariable, variable2))
                                  || (is_element_in_vector(Variableinst, variable2))
                                  || (is_element_in_vector(Variableinss, variable2)) )
                                      &&!((is_element_in_vector(activeVariable, variable3))
                                         || (is_element_in_vector(Variableinst, variable3))
                                         || (is_element_in_vector(Variableinss, variable3)))){

                             reg2 = VariablesinRegs[variable2];
                             //CHECK REGISTERS
                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg3 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg3 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg3] = false;
                                 recentlyusedRegs.push_back(reg3);
                                 VariablesinRegs[variable3] = reg3;
                                 VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);

                             }
                             else{
                                 //STORE DATA TO MEMORY

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable3, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable3);
                                 recentlyusedRegs.push_back( "$s0");
                                 reg3 = "$s0";

                                 }



                             }

                         // IF one of the variabel is in memory

                         else if(!((is_element_in_vector(activeVariable, variable2))
                                  || (is_element_in_vector(Variableinst, variable2))
                                  || (is_element_in_vector(Variableinss, variable2)) )
                                      &&((is_element_in_vector(activeVariable, variable3))
                                         || (is_element_in_vector(Variableinst, variable3))
                                         || (is_element_in_vector(Variableinss, variable3)))){



                             reg3 = VariablesinRegs[variable3];
                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg2] = false;
                                 recentlyusedRegs.push_back(reg2);
                                 VariablesinRegs[variable2] = reg2;
                                 VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);
                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable2, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable2);
                                 recentlyusedRegs.push_back( "$s0");
                                 reg2 = "$s0";

                                 }



                         }


                         //If both variables are in memory
                         else{

                             CheckRegisters(RegisterStatus, VacantRegisters);

                             if (VacantRegisters.size() <= 1){

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }
                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg1] = false;
                                 recentlyusedRegs.push_back(reg2);
                                 VariablesinRegs[variable2] = reg2;




                             }

                             reg2 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg2] = false;
                             recentlyusedRegs.push_back(reg2);
                             VariablesinRegs[variable2] = reg2;
                             VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);

                             CheckRegisters(RegisterStatus, VacantRegisters);

                             reg3 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg3 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg3] = false;
                             recentlyusedRegs.push_back(reg3);
                             VariablesinRegs[variable3] = reg3;
                             VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);


                         }

                         MIPSProgram << "    add " << reg1 <<", " << reg2 <<", " << reg3 <<endl;

                     }
                     //IF one of them is a variabel and the other is a integer

                     else if(!(CheckType(variable2)) && CheckType(variable3)){

                         if ((is_element_in_vector(activeVariable, variable2))
                                 || (is_element_in_vector(Variableinst, variable2))
                                 || (is_element_in_vector(Variableinss, variable2))){

                             reg2 = VariablesinRegs[variable2];


                         }
                         else{

                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable2] <<"($sp)" <<endl;
                                 RegisterStatus[reg2] = false;
                                 recentlyusedRegs = {reg2};
                                 VariablesinRegs[variable2] = reg2;
                                 VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable2, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable2);
                                 recentlyusedRegs = {"$s0"};
                                 reg1 = "$s0";

                                 }

                             }

                         MIPSProgram << "    addi " << reg1 << ", " << reg2 <<", "<< variable3 <<endl;


                     }
                     else{

                         if((is_element_in_vector(activeVariable, variable3))
                                 || (is_element_in_vector(Variableinst, variable3))
                                 || (is_element_in_vector(Variableinss, variable3))){

                             reg3 = VariablesinRegs[variable3];


                         }
                         else{

                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg3 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<< reg3 << ", "<< VariableinMem[variable3] <<"($sp)" <<endl;
                                 RegisterStatus[reg3] = false;
                                 recentlyusedRegs = {reg3};
                                 VariablesinRegs[variable2] = reg3;
                                 VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable3, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable3);
                                 recentlyusedRegs = {"$s0"};
                                 reg1 = "$s0";

                                 }

                             }
                         MIPSProgram << "    addi " << reg1 << ", " << reg3 <<", "<< variable2 <<endl;

                     }


                 }
                 else if(line.find('-') != -1 && line.find('-') > line.find('=')){

                     string variable1 = line.substr(line.find_first_not_of(' '), line.find('=')-1-line.find_first_not_of(' '));
                     string variable2 = line.substr(line.find('=')+2, line.find('-')-3-line.find('='));
                     string variable3 = line.substr(line.find('-')+2, line.find(';')-2-line.find('-'));
                     string reg1;
                     string reg2;
                     string reg3;

                     //The special part of subtraction is that subtraction does not have I-type instruction,
                     //Meaning that, even if the calculation is between an integer and a variable,
                     //The integer needs storing into register to continue to next step of operation


                     //First Check the variable on the left side of the '='
                     if ((is_element_in_vector(activeVariable, variable1))
                             || (is_element_in_vector(Variableinst, variable1))
                             || (is_element_in_vector(Variableinss, variable1))){

                         reg1 = VariablesinRegs[variable1];


                     }
                     else{

                         if(CheckRegisters(RegisterStatus, VacantRegisters)){

                             reg1 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg1 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg1] = false;
                             recentlyusedRegs = {reg1};
                             VariablesinRegs[variable1] = reg1;
                             VariableClassify(variable1, reg1, activeVariable, Variableinst, Variableinss);

                         }
                         else{

                             for(int v = 7; v >= 0; v --){

                                 string reg = VariablesinRegs[Variableinss[v]];
                                 MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                 VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                 VariablesinRegs.erase(Variableinss[v]);
                                 RegisterStatus[reg] = true;
                                 Variableinss.pop_back();

                                 memposition = memposition + 4;

                             }


                             MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                             VariablesinRegs.insert(pair<string,string>(variable1, "$s0"));
                             RegisterStatus["$s0"] = false;
                             Variableinss.push_back(variable1);
                             recentlyusedRegs = {"$s0"};
                             reg1 = "$s0";

                             }

                         }

                     //Then check the right side

                     if(CheckType(variable2) && CheckType(variable3)){

                         MIPSProgram << "    addi " << reg1 << ", $zero, " << strtoint(variable2) - strtoint(variable3) << endl;

                     }
                     //If both data are variables
                     else if(!(CheckType(variable2)) && !(CheckType(variable3))){

                         //If both variables are in registers
                         if(((is_element_in_vector(activeVariable, variable2))
                             || (is_element_in_vector(Variableinst, variable2))
                             || (is_element_in_vector(Variableinss, variable2)) )
                                 &&((is_element_in_vector(activeVariable, variable3))
                                    || (is_element_in_vector(Variableinst, variable3))
                                    || (is_element_in_vector(Variableinss, variable3)))){
                             reg2 = VariablesinRegs[variable2];
                             reg3 = VariablesinRegs[variable3];

                         }


                         else if(((is_element_in_vector(activeVariable, variable2))
                                  || (is_element_in_vector(Variableinst, variable2))
                                  || (is_element_in_vector(Variableinss, variable2)) )
                                      &&!((is_element_in_vector(activeVariable, variable3))
                                         || (is_element_in_vector(Variableinst, variable3))
                                         || (is_element_in_vector(Variableinss, variable3)))){

                             reg2 = VariablesinRegs[variable2];
                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg3 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg3 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg3] = false;
                                 recentlyusedRegs.push_back(reg3);
                                 VariablesinRegs[variable3] = reg3;
                                 VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable3, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable3);
                                 recentlyusedRegs.push_back( "$s0");
                                 reg3 = "$s0";

                                 }



                             }


                         else if(!((is_element_in_vector(activeVariable, variable2))
                                   || (is_element_in_vector(Variableinst, variable2))
                                   || (is_element_in_vector(Variableinss, variable2)) )
                                       &&((is_element_in_vector(activeVariable, variable3))
                                          || (is_element_in_vector(Variableinst, variable3))
                                          || (is_element_in_vector(Variableinss, variable3)))){



                             reg3 = VariablesinRegs[variable3];
                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg2] = false;
                                 recentlyusedRegs.push_back(reg2);
                                 VariablesinRegs[variable2] = reg2;
                                 VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);
                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable2, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable2);
                                 recentlyusedRegs.push_back( "$s0");
                                 reg2 = "$s0";

                                 }



                         }


                         else{
                             CheckRegisters(RegisterStatus, VacantRegisters);

                             if (VacantRegisters.size() <= 1){

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }
                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg1] = false;
                                 recentlyusedRegs.push_back(reg2);
                                 VariablesinRegs[variable2] = reg2;




                             }

                             reg2 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg2] = false;
                             recentlyusedRegs.push_back(reg2);
                             VariablesinRegs[variable2] = reg2;
                             VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);

                             CheckRegisters(RegisterStatus, VacantRegisters);

                             reg3 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg3 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg3] = false;
                             recentlyusedRegs.push_back(reg3);
                             VariablesinRegs[variable3] = reg3;
                             VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);


                         }

                         MIPSProgram << "    sub " << reg1 <<", " << reg2 <<", " << reg3;

                     }
                     else if(!(CheckType(variable2)) && CheckType(variable3)){

                         if ((is_element_in_vector(activeVariable, variable2))
                                 || (is_element_in_vector(Variableinst, variable2))
                                 || (is_element_in_vector(Variableinss, variable2))){

                             reg2 = VariablesinRegs[variable2];


                         }
                         else{

                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable2] <<"($sp)" <<endl;
                                 RegisterStatus[reg2] = false;
                                 recentlyusedRegs = {reg2};
                                 VariablesinRegs[variable2] = reg2;
                                 VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable2, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable2);
                                 recentlyusedRegs = {"$s0"};
                                 reg2 = "$s0";

                                 }

                             }

                         //Load integer to register

                         if (reg1 != reg2){

                             MIPSProgram<< "    addi " << reg1 <<", $zero, " << variable3 <<endl;
                             MIPSProgram<< "    sub " << reg1 << ", " << reg2 <<", "<< reg1 <<endl;
                         }
                         else{
                             //Find an empty register to store the integer

                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg3 = VacantRegisters[0];
                                 MIPSProgram<< "    addi " << reg3 <<", $zero, " << variable3 <<endl;
                                 MIPSProgram<< "    sub " << reg1 << ", " << reg2 <<", "<< reg3 <<endl;
                             }
                             else{
                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;
                                 }

                                 MIPSProgram << "    addi " << "$s0" <<", $zero, " << variable3 <<endl;
                                 reg3 = "$s0";
                                 MIPSProgram<< "    sub " << reg1 << ", " << reg2 <<", "<< reg3 <<endl;

                             }

                         }


                     }
                     else{

                         if ((is_element_in_vector(activeVariable, variable3))
                                 || (is_element_in_vector(Variableinst, variable3))
                                 || (is_element_in_vector(Variableinss, variable3))){

                             reg3 = VariablesinRegs[variable3];


                         }
                         else{

                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg3 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<< reg3 << ", "<< VariableinMem[variable3] <<"($sp)" <<endl;
                                 RegisterStatus[reg3] = false;
                                 recentlyusedRegs = {reg3};
                                 VariablesinRegs[variable2] = reg3;
                                 VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable3, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable3);
                                 recentlyusedRegs = {"$s0"};
                                 reg1 = "$s0";

                                 }

                             }
                         MIPSProgram << "    addi " << reg1 <<", $zero, " << variable2 <<endl;
                         MIPSProgram << "    sub " << reg1 << ", " << reg1 <<", "<< reg3 <<endl;

                     }

                 }

                 else if(line.find('/')!= -1 && line.find('/') > line.find('=')){
                     //Division calculation automatically stores the data in the lo register
                     // Similar to subtraction, all data involved in calculation are required to be stored in
                     // registers.

                     string variable1 = line.substr(line.find_first_not_of(' '), line.find('=')-1-line.find_first_not_of(' '));
                     string variable2 = line.substr(line.find('=')+2, line.find('/')-3-line.find('='));
                     string variable3 = line.substr(line.find('/')+2, line.find(';')-2-line.find('/'));
                     string reg1;
                     string reg2;
                     string reg3;


                     if ((is_element_in_vector(activeVariable, variable1))
                             || (is_element_in_vector(Variableinst, variable1))
                             || (is_element_in_vector(Variableinss, variable1))){

                         reg1 = VariablesinRegs[variable1];



                     }
                     else{

                         if(CheckRegisters(RegisterStatus, VacantRegisters)){

                             reg1 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg1 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg1] = false;
                             recentlyusedRegs = {reg1};
                             VariablesinRegs[variable1] = reg1;
                             VariableClassify(variable1, reg1, activeVariable, Variableinst, Variableinss);

                         }
                         else{

                             for(int v = 7; v >= 0; v --){

                                 string reg = VariablesinRegs[Variableinss[v]];
                                 MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                 VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                 VariablesinRegs.erase(Variableinss[v]);
                                 RegisterStatus[reg] = true;
                                 Variableinss.pop_back();

                                 memposition = memposition + 4;

                             }


                             MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                             VariablesinRegs.insert(pair<string,string>(variable1, "$s0"));
                             RegisterStatus["$s0"] = false;
                             Variableinss.push_back(variable1);
                             recentlyusedRegs = {"$s0"};
                             reg1 = "$s0";

                             }

                         }


                     if(CheckType(variable2) && CheckType(variable3)){

                         MIPSProgram << "    addi " << reg1 << ", $zero, " << strtoint(variable2) / strtoint(variable3) << endl;

                     }
                     else if(!(CheckType(variable2)) && !(CheckType(variable3))){

                         if(((is_element_in_vector(activeVariable, variable2))
                             || (is_element_in_vector(Variableinst, variable2))
                             || (is_element_in_vector(Variableinss, variable2)) )
                                 &&((is_element_in_vector(activeVariable, variable3))
                                    || (is_element_in_vector(Variableinst, variable3))
                                    || (is_element_in_vector(Variableinss, variable3)))){
                             reg2 = VariablesinRegs[variable2];
                             reg3 = VariablesinRegs[variable3];

                         }


                         else if(((is_element_in_vector(activeVariable, variable2))
                                  || (is_element_in_vector(Variableinst, variable2))
                                  || (is_element_in_vector(Variableinss, variable2)) )
                                      &&!((is_element_in_vector(activeVariable, variable3))
                                         || (is_element_in_vector(Variableinst, variable3))
                                         || (is_element_in_vector(Variableinss, variable3)))){

                             reg2 = VariablesinRegs[variable2];
                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg3 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg3 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg3] = false;
                                 recentlyusedRegs.push_back(reg3);
                                 VariablesinRegs[variable3] = reg3;
                                 VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable3, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable3);
                                 recentlyusedRegs.push_back( "$s0");
                                 reg3 = "$s0";

                                 }



                             }


                         else if(!((is_element_in_vector(activeVariable, variable2))
                                   || (is_element_in_vector(Variableinst, variable2))
                                   || (is_element_in_vector(Variableinss, variable2)) )
                                       &&((is_element_in_vector(activeVariable, variable3))
                                          || (is_element_in_vector(Variableinst, variable3))
                                          || (is_element_in_vector(Variableinss, variable3)))){



                             reg3 = VariablesinRegs[variable3];
                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg2] = false;
                                 recentlyusedRegs.push_back(reg2);
                                 VariablesinRegs[variable2] = reg2;
                                 VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);
                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable2, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable2);
                                 recentlyusedRegs.push_back( "$s0");
                                 reg2 = "$s0";

                                 }



                         }


                         else{
                             CheckRegisters(RegisterStatus, VacantRegisters);

                             if (VacantRegisters.size() <= 1){

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }
                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg1] = false;
                                 recentlyusedRegs.push_back(reg2);
                                 VariablesinRegs[variable2] = reg2;




                             }

                             reg2 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg2] = false;
                             recentlyusedRegs.push_back(reg2);
                             VariablesinRegs[variable2] = reg2;
                             VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);

                             CheckRegisters(RegisterStatus, VacantRegisters);

                             reg3 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg3 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg3] = false;
                             recentlyusedRegs.push_back(reg3);
                             VariablesinRegs[variable3] = reg3;
                             VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);


                         }

                         MIPSProgram << "    div " << reg2 <<", " << reg3 << endl;
                         MIPSProgram << "    mflo " << reg1 << endl;


                     }
                     else if(!(CheckType(variable2)) && CheckType(variable3)){

                         if ((is_element_in_vector(activeVariable, variable2))
                                 || (is_element_in_vector(Variableinst, variable2))
                                 || (is_element_in_vector(Variableinss, variable2))){

                             reg2 = VariablesinRegs[variable2];


                         }
                         else{

                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable2] <<"($sp)" <<endl;
                                 RegisterStatus[reg2] = false;
                                 recentlyusedRegs = {reg2};
                                 VariablesinRegs[variable2] = reg2;
                                 VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable2, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable2);
                                 recentlyusedRegs = {"$s0"};
                                 reg1 = "$s0";

                                 }

                             }
                         //Get result from lo register

                         MIPSProgram << "    addi " << reg1 <<", $zero, " << variable3 <<endl;
                         MIPSProgram << "    div " << reg2 <<", " << reg1 << endl;
                         MIPSProgram << "    mflo " << reg1 << endl;


                     }
                     else{

                         if ((is_element_in_vector(activeVariable, variable3))
                                 || (is_element_in_vector(Variableinst, variable3))
                                 || (is_element_in_vector(Variableinss, variable3))){

                             reg3 = VariablesinRegs[variable3];


                         }
                         else{

                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg3 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<< reg3 << ", "<< VariableinMem[variable3] <<"($sp)" <<endl;
                                 RegisterStatus[reg3] = false;
                                 recentlyusedRegs = {reg3};
                                 VariablesinRegs[variable2] = reg3;
                                 VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable3, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable3);
                                 recentlyusedRegs = {"$s0"};
                                 reg1 = "$s0";

                                 }

                             }
                         MIPSProgram << "    addi " << reg1 <<", $zero, " << variable2 <<endl;
                         MIPSProgram << "    div " << reg1 <<", " << reg3 << endl;
                         MIPSProgram << "    mflo " << reg1 << endl;

                     }

                 }
                 else if(line.find('*') != -1 && line.find('*') > line.find('=')){

                     //Multiplication calculation stores data in both hi and lo registers
                     //Since this program only supports 32-bit integers, we only consider the data in the lo register
                     //All data involved in calculation is required to be stored in reigisters

                     string variable1 = line.substr(line.find_first_not_of(' '), line.find('=')-1-line.find_first_not_of(' '));
                     string variable2 = line.substr(line.find('=')+2, line.find('*')-3-line.find('='));
                     string variable3 = line.substr(line.find('*')+2, line.find(';')-2-line.find('*'));
                     string reg1;
                     string reg2;
                     string reg3;


                     if ((is_element_in_vector(activeVariable, variable1))
                             || (is_element_in_vector(Variableinst, variable1))
                             || (is_element_in_vector(Variableinss, variable1))){

                         reg1 = VariablesinRegs[variable1];


                     }
                     else{

                         if(CheckRegisters(RegisterStatus, VacantRegisters)){

                             reg1 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg1 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg1] = false;
                             recentlyusedRegs = {reg1};
                             VariablesinRegs[variable1] = reg1;
                             VariableClassify(variable1, reg1, activeVariable, Variableinst, Variableinss);

                         }
                         else{

                             for(int v = 7; v >= 0; v --){

                                 string reg = VariablesinRegs[Variableinss[v]];
                                 MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                 VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                 VariablesinRegs.erase(Variableinss[v]);
                                 RegisterStatus[reg] = true;
                                 Variableinss.pop_back();

                                 memposition = memposition + 4;

                             }


                             MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                             VariablesinRegs.insert(pair<string,string>(variable1, "$s0"));
                             RegisterStatus["$s0"] = false;
                             Variableinss.push_back(variable1);
                             recentlyusedRegs = {"$s0"};
                             reg1 = "$s0";

                             }

                         }


                     if(CheckType(variable2) && CheckType(variable3)){

                         MIPSProgram << "    addi " << reg1 << ", $zero, " << strtoint(variable2) * strtoint(variable3) << endl;

                     }
                     else if(!(CheckType(variable2)) && !(CheckType(variable3))){

                         if(((is_element_in_vector(activeVariable, variable2))
                             || (is_element_in_vector(Variableinst, variable2))
                             || (is_element_in_vector(Variableinss, variable2)) )
                                 &&((is_element_in_vector(activeVariable, variable3))
                                    || (is_element_in_vector(Variableinst, variable3))
                                    || (is_element_in_vector(Variableinss, variable3)))){
                             reg2 = VariablesinRegs[variable2];
                             reg3 = VariablesinRegs[variable3];

                         }


                         else if(((is_element_in_vector(activeVariable, variable2))
                                  || (is_element_in_vector(Variableinst, variable2))
                                  || (is_element_in_vector(Variableinss, variable2)) )
                                      &&!((is_element_in_vector(activeVariable, variable3))
                                         || (is_element_in_vector(Variableinst, variable3))
                                         || (is_element_in_vector(Variableinss, variable3)))){

                             reg2 = VariablesinRegs[variable2];
                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg3 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg3 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg3] = false;
                                 recentlyusedRegs.push_back(reg3);
                                 VariablesinRegs[variable3] = reg3;
                                 VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable3, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable3);
                                 recentlyusedRegs.push_back( "$s0");
                                 reg3 = "$s0";

                                 }



                             }


                         else if(!((is_element_in_vector(activeVariable, variable2))
                                   || (is_element_in_vector(Variableinst, variable2))
                                   || (is_element_in_vector(Variableinss, variable2)) )
                                       &&((is_element_in_vector(activeVariable, variable3))
                                          || (is_element_in_vector(Variableinst, variable3))
                                          || (is_element_in_vector(Variableinss, variable3)))){



                             reg3 = VariablesinRegs[variable3];
                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg2] = false;
                                 recentlyusedRegs.push_back(reg2);
                                 VariablesinRegs[variable2] = reg2;
                                 VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);
                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable2, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable2);
                                 recentlyusedRegs.push_back( "$s0");
                                 reg2 = "$s0";

                                 }



                         }


                         else{
                             CheckRegisters(RegisterStatus, VacantRegisters);

                             if (VacantRegisters.size() <= 1){

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }
                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg1] = false;
                                 recentlyusedRegs.push_back(reg2);
                                 VariablesinRegs[variable2] = reg2;




                             }

                             reg2 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg2] = false;
                             recentlyusedRegs.push_back(reg2);
                             VariablesinRegs[variable2] = reg2;
                             VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);

                             CheckRegisters(RegisterStatus, VacantRegisters);

                             reg3 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg3 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg3] = false;
                             recentlyusedRegs.push_back(reg3);
                             VariablesinRegs[variable3] = reg3;
                             VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);


                         }

                         MIPSProgram << "    mult " << reg2 <<", " << reg3 << endl;
                         MIPSProgram << "    mflo " << reg1 << endl;


                     }
                     else if(!(CheckType(variable2)) && CheckType(variable3)){

                         if ((is_element_in_vector(activeVariable, variable2))
                                 || (is_element_in_vector(Variableinst, variable2))
                                 || (is_element_in_vector(Variableinss, variable2))){

                             reg2 = VariablesinRegs[variable2];


                         }
                         else{

                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable2] <<"($sp)" <<endl;
                                 RegisterStatus[reg2] = false;
                                 recentlyusedRegs = {reg2};
                                 VariablesinRegs[variable2] = reg2;
                                 VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable2, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable2);
                                 recentlyusedRegs = {"$s0"};
                                 reg1 = "$s0";

                                 }

                             }

                         MIPSProgram << "    addi " << reg1 <<", $zero, " << variable3 <<endl;
                         MIPSProgram << "    mult " << reg2 <<", " << reg1 << endl;
                         MIPSProgram << "    mflo " << reg1 << endl;


                     }
                     else{

                         if ((is_element_in_vector(activeVariable, variable3))
                                 || (is_element_in_vector(Variableinst, variable3))
                                 || (is_element_in_vector(Variableinss, variable3))){

                             reg3 = VariablesinRegs[variable3];


                         }
                         else{

                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg3 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<< reg3 << ", "<< VariableinMem[variable3] <<"($sp)" <<endl;
                                 RegisterStatus[reg3] = false;
                                 recentlyusedRegs = {reg3};
                                 VariablesinRegs[variable2] = reg3;
                                 VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable3, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable3);
                                 recentlyusedRegs = {"$s0"};
                                 reg1 = "$s0";

                                 }

                             }

                         //Get result from lo register
                         MIPSProgram << "    addi " << reg1 <<", $zero, " << variable2 <<endl;
                         MIPSProgram << "    mult " << reg1 <<", " << reg3 << endl;
                         MIPSProgram << "    mflo " << reg1 << endl;

                     }

                 }
                 else if(line.find('%') != -1 && line.find('%') > line.find('=')){
                     //Remainder calculation is actually the other half of division
                     //Results are stored in hi registers.

                     string variable1 = line.substr(line.find_first_not_of(' '), line.find('=')-1-line.find_first_not_of(' '));
                     string variable2 = line.substr(line.find('=')+2, line.find('%')-3-line.find('='));
                     string variable3 = line.substr(line.find('%')+2, line.find(';')-2-line.find('%'));
                     string reg1;
                     string reg2;
                     string reg3;


                     if ((is_element_in_vector(activeVariable, variable1))
                             || (is_element_in_vector(Variableinst, variable1))
                             || (is_element_in_vector(Variableinss, variable1))){

                         reg1 = VariablesinRegs[variable1];


                     }
                     else{

                         if(CheckRegisters(RegisterStatus, VacantRegisters)){

                             reg1 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg1 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg1] = false;
                             recentlyusedRegs = {reg1};
                             VariablesinRegs[variable1] = reg1;
                             VariableClassify(variable1, reg1, activeVariable, Variableinst, Variableinss);

                         }
                         else{

                             for(int v = 7; v >= 0; v --){

                                 string reg = VariablesinRegs[Variableinss[v]];
                                 MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                 VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                 VariablesinRegs.erase(Variableinss[v]);
                                 RegisterStatus[reg] = true;
                                 Variableinss.pop_back();

                                 memposition = memposition + 4;

                             }


                             MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                             VariablesinRegs.insert(pair<string,string>(variable1, "$s0"));
                             RegisterStatus["$s0"] = false;
                             Variableinss.push_back(variable1);
                             recentlyusedRegs = {"$s0"};
                             reg1 = "$s0";

                             }

                         }


                     if(CheckType(variable2) && CheckType(variable3)){

                         MIPSProgram << "    addi " << reg1 << ", $zero, " << strtoint(variable2) % strtoint(variable3) << endl;

                     }
                     else if(!(CheckType(variable2)) && !(CheckType(variable3))){

                         if(((is_element_in_vector(activeVariable, variable2))
                             || (is_element_in_vector(Variableinst, variable2))
                             || (is_element_in_vector(Variableinss, variable2)) )
                                 &&((is_element_in_vector(activeVariable, variable3))
                                    || (is_element_in_vector(Variableinst, variable3))
                                    || (is_element_in_vector(Variableinss, variable3)))){
                             reg2 = VariablesinRegs[variable2];
                             reg3 = VariablesinRegs[variable3];

                         }


                         else if(((is_element_in_vector(activeVariable, variable2))
                                  || (is_element_in_vector(Variableinst, variable2))
                                  || (is_element_in_vector(Variableinss, variable2)) )
                                      &&!((is_element_in_vector(activeVariable, variable3))
                                         || (is_element_in_vector(Variableinst, variable3))
                                         || (is_element_in_vector(Variableinss, variable3)))){

                             reg2 = VariablesinRegs[variable2];
                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg3 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg3 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg3] = false;
                                 recentlyusedRegs.push_back(reg3);
                                 VariablesinRegs[variable3] = reg3;
                                 VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable3, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable3);
                                 recentlyusedRegs.push_back( "$s0");
                                 reg3 = "$s0";

                                 }



                             }


                         else if(!((is_element_in_vector(activeVariable, variable2))
                                   || (is_element_in_vector(Variableinst, variable2))
                                   || (is_element_in_vector(Variableinss, variable2)) )
                                       &&((is_element_in_vector(activeVariable, variable3))
                                          || (is_element_in_vector(Variableinst, variable3))
                                          || (is_element_in_vector(Variableinss, variable3)))){



                             reg3 = VariablesinRegs[variable3];
                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg2] = false;
                                 recentlyusedRegs.push_back(reg2);
                                 VariablesinRegs[variable2] = reg2;
                                 VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);
                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable2, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable2);
                                 recentlyusedRegs.push_back( "$s0");
                                 reg2 = "$s0";

                                 }



                         }


                         else{
                             CheckRegisters(RegisterStatus, VacantRegisters);

                             if (VacantRegisters.size() <= 1){

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }
                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                                 RegisterStatus[reg1] = false;
                                 recentlyusedRegs.push_back(reg2);
                                 VariablesinRegs[variable2] = reg2;




                             }

                             reg2 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg2] = false;
                             recentlyusedRegs.push_back(reg2);
                             VariablesinRegs[variable2] = reg2;
                             VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);

                             CheckRegisters(RegisterStatus, VacantRegisters);

                             reg3 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg3 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg3] = false;
                             recentlyusedRegs.push_back(reg3);
                             VariablesinRegs[variable3] = reg3;
                             VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);


                         }

                         MIPSProgram << "    div " << reg2 <<", " << reg3 << endl;
                         MIPSProgram << "    mfhi " << reg1 << endl;


                     }
                     else if(!(CheckType(variable2)) && CheckType(variable3)){

                         if ((is_element_in_vector(activeVariable, variable2))
                                 || (is_element_in_vector(Variableinst, variable2))
                                 || (is_element_in_vector(Variableinss, variable2))){

                             reg2 = VariablesinRegs[variable2];


                         }
                         else{

                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg2 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<<reg2 << ", "<< VariableinMem[variable2] <<"($sp)" <<endl;
                                 RegisterStatus[reg2] = false;
                                 recentlyusedRegs = {reg2};
                                 VariablesinRegs[variable2] = reg2;
                                 VariableClassify(variable2, reg2, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable2, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable2);
                                 recentlyusedRegs = {"$s0"};
                                 reg1 = "$s0";

                                 }

                             }

                         MIPSProgram << "    addi " << reg1 <<", $zero, " << variable3 <<endl;
                         MIPSProgram << "    div " << reg2 <<", " << reg1 << endl;
                         MIPSProgram << "    mfhi " << reg1 << endl;


                     }
                     else{

                         if ((is_element_in_vector(activeVariable, variable3))
                                 || (is_element_in_vector(Variableinst, variable3))
                                 || (is_element_in_vector(Variableinss, variable3))){

                             reg3 = VariablesinRegs[variable3];


                         }
                         else{

                             if(CheckRegisters(RegisterStatus, VacantRegisters)){

                                 reg3 = VacantRegisters[0];

                                 MIPSProgram << "    lw "<< reg3 << ", "<< VariableinMem[variable3] <<"($sp)" <<endl;
                                 RegisterStatus[reg3] = false;
                                 recentlyusedRegs = {reg3};
                                 VariablesinRegs[variable2] = reg3;
                                 VariableClassify(variable3, reg3, activeVariable, Variableinst, Variableinss);

                             }
                             else{

                                 for(int v = 7; v >= 0; v --){

                                     string reg = VariablesinRegs[Variableinss[v]];
                                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                     VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                     VariablesinRegs.erase(Variableinss[v]);
                                     RegisterStatus[reg] = true;
                                     Variableinss.pop_back();

                                     memposition = memposition + 4;

                                 }


                                 MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                                 VariablesinRegs.insert(pair<string,string>(variable3, "$s0"));
                                 RegisterStatus["$s0"] = false;
                                 Variableinss.push_back(variable3);
                                 recentlyusedRegs = {"$s0"};
                                 reg1 = "$s0";

                                 }

                             }

                         //Get results from hi register
                         MIPSProgram << "    addi " << reg1 <<", $zero, " << variable2 <<endl;
                         MIPSProgram << "    div " << reg1 <<", " << reg3 << endl;
                         MIPSProgram << "    mfhi " << reg1 << endl;

                     }

                 }





             }

             //IF this line calls for function
             else if(CheckFunctionCall(Functions, line)){
                 //Save all the Progress
                 int memstart= memposition;

                 map <int, string> memtoVariables;
                 map<string, string> originRegs;

                 for (int i = 0;  i < activeVariable.size(); i++){

                     string reg = VariablesinRegs[activeVariable[i]];
                     originRegs.insert(pair<string, string>(activeVariable[i], reg));
                     memtoVariables.insert(pair<int, string>(memposition,activeVariable[i]));
                     VariableinMem.insert(pair<string, int>(activeVariable[i], memposition));
                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;

                     memposition = memposition + 4;

                 }

                 for (int i = 0;  i < Variableinst.size(); i++){

                     string reg = VariablesinRegs[Variableinst[i]];
                     originRegs.insert(pair<string, string>(Variableinst[i], reg));
                     memtoVariables.insert(pair<int, string>(memposition, Variableinst[i]));
                     VariableinMem.insert(pair<string, int>(Variableinst[i], memposition));
                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;


                     memposition = memposition + 4;

                 }

                 for (int i = 0;  i < Variableinss.size(); i++){

                     string reg = VariablesinRegs[Variableinss[i]];
                     originRegs.insert(pair<string, string>(Variableinss[i], reg));
                     memtoVariables.insert(pair<int, string>(memposition, Variableinss[i]));
                     VariableinMem.insert(pair<string, int>(Variableinss[i], memposition));
                     MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;

                     memposition = memposition + 4;

                 }

                 int memend = memposition;

                 //Load Data to Parameters

                 string fName;
                 if (line.find('=') == -1){

                     fName = line.substr(line.find_first_not_of(' '), line.find('(') - line.find_first_not_of(' '));
                 }
                 else{

                     fName = line.substr(line.find('=')+2, line.find_first_of('(') - line.find('=') - 2);


                 }

                 vector<string> varnames;
                 int newstart = 0;

                 for(int q = 0; q < ParaofFuncs[fName]; q++){

                     if(q == ParaofFuncs[fName] -1 && q == 0){

                         varnames.push_back(line.substr(line.find('(')+1, line.find(')') - line.find('(') - 1 ));

                     }


                     else if (q == 0){

                         varnames.push_back(line.substr(line.find('(')+1, line.find(',') - line.find('(') - 1 ));
                         newstart = line.find(',')+2;


                     }
                     else if (q == ParaofFuncs[fName] -1){

                         varnames.push_back(line.substr(line.find_last_of(',')+2, line.find(')') - line.find_last_of(',')-2));

                     }
                     else {

                         string newline = line.substr(newstart, line.length()-newstart);

                         varnames.push_back(newline.substr(0, newline.find(',')));
                         newstart = newline.find(',')+2;
                     }
                 }


                 for(int q = 0; q < varnames.size(); q ++){

                    MIPSProgram << "    lw " << RegisterNames[q] << ", " << VariableinMem[varnames[q]] << "($sp)" << endl;


                 }

                 //Jump to Function
                 MIPSProgram << "    jal " << fName << endl;

                 //Restore Progress

                 for(int k = memstart; k < memend; k += 4){

                     MIPSProgram << "    lw " << originRegs[memtoVariables[k]] <<", "<< k << "($sp)" << endl;

                 }

                 memposition = memstart;

                 string reg1;
                 // Read Result of Function Execution

                 if (line.find('=') != -1){

                     string variable1 = line.substr(line.find_first_not_of(' '), line.find('=')-1-line.find_first_not_of(' '));


                     if ((is_element_in_vector(activeVariable, variable1))
                             || (is_element_in_vector(Variableinst, variable1))
                             || (is_element_in_vector(Variableinss, variable1))){

                         reg1 = VariablesinRegs[variable1];


                     }
                     else{

                         if(CheckRegisters(RegisterStatus, VacantRegisters)){

                             reg1 = VacantRegisters[0];

                             MIPSProgram << "    lw "<<reg1 << ", "<< VariableinMem[variable1] <<"($sp)" <<endl;
                             RegisterStatus[reg1] = false;
                             recentlyusedRegs = {reg1};
                             VariablesinRegs[variable1] = reg1;
                             VariableClassify(variable1, reg1, activeVariable, Variableinst, Variableinss);

                         }
                         else{

                             for(int v = 7; v >= 0; v --){

                                 string reg = VariablesinRegs[Variableinss[v]];
                                 MIPSProgram << "    sw "<< reg <<", "<<memposition << "($sp)" << endl;
                                 VariableinMem.insert(pair<string, int>(Variableinss[v], memposition));
                                 VariablesinRegs.erase(Variableinss[v]);
                                 RegisterStatus[reg] = true;
                                 Variableinss.pop_back();

                                 memposition = memposition + 4;

                             }


                             MIPSProgram << "    sw " << "$s0" <<", " << memposition << "($sp)" << endl;
                             VariablesinRegs.insert(pair<string,string>(variable1, "$s0"));
                             RegisterStatus["$s0"] = false;
                             Variableinss.push_back(variable1);
                             recentlyusedRegs = {"$s0"};
                             reg1 = "$s0";

                             }

                         }



                 }

                 MIPSProgram << "    addi " << reg1 << ", $zero, $v0" << endl;

             }

             else if(line.find("return")  != -1){

                 int head = line.find("return") + 7;
                 string variable = line.substr(head, line.find(';') - head);

                 if ((is_element_in_vector(activeVariable, variable))
                        || (is_element_in_vector(Variableinst, variable))
                        || (is_element_in_vector(Variableinss, variable))){

                    string reg1 = VariablesinRegs[variable];

                    MIPSProgram << "    addi $v0, "<< reg1 << ", 0" << endl;


                 }
                 else{

                     MIPSProgram << "    lw $v0, " << VariableinMem[variable] << "($sp)" << endl;
                 }


             }
        }



    }



}




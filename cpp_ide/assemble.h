#ifndef _assemble_h
#define _assemble_h

#include <string>
#include <fstream>
#include <map>
#include <iostream>
//#include<algorithm> //windows系统
//#include<QString> //windows系统
#include "tools.h"

/*
 * Function: assemble
 * Usage: assemble(iname, oname);
 * -----------------------------
 * This function takes the paths of the mips files to assemble and the generated machine code file.
 */

void assemble(std::string iname,std::string oname);

/*
 * Function: name_number
 * Usage: registers=name_number(registername, registernumber);
 * -----------------------------------------------------------
 * This function returns a map of register name and register number.
 */

std::map<std::string,std::string> name_number(const std::string registername[32],const std::string registernumber[32]);

/*
 * Function: label_address
 * Usage: labels=label_address(ifile);
 * -----------------------------------
 * This function returns a map of label and address.
 */

std::map<std::string,size_t> label_address(std::istream & ifile);


#endif

#ifndef LINKER_H
#define LINKER_H
#include <string>
#include <fstream>
#include "tools.h"


/*
 * Function: assemble
 * Usage: linker(data1, text1, data2, text2, datasize);
 * -----------------------------
 * Link two files
 */
void linker(std::string dname1, std::string tname1, std::string dname2, std::string tname2, int datasize);

#endif // LINKER_H

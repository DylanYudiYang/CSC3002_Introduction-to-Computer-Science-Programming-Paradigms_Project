#include <string>
#include <fstream>
#include "linker.h"
using namespace std;
void linker(string dname1, string tname1, string dname2, string tname2, int datasize)
{
    ofstream linkdata, linktext;
    string dline1, tline1, dline2, tline2;
    ifstream data1, data2, text1, text2;
    data1.open(dname1);
    data2.open(dname2);
    text1.open(tname1);
    text2.open(tname2);

    //link data
    linkdata.open("linkdata.txt");

    while(getline(data1, dline1)){
        linkdata << dline1 << endl;
    }
    while(getline(data2, dline2)){
        linkdata << dline2 << endl;
    }

    data1.close();
    data2.close();
    linkdata.close();

    //link text, change absolute address
    linktext.open("linktext.txt");

    while(getline(text1, tline1)){
        linktext << tline1 << endl;
    }
    text1.close();

    while(getline(text2, tline2)){
        string op = tline2.substr(0,6);
        string rs = tline2.substr(6,5);
        string rt = tline2.substr(11,5);
        string imm = tline2.substr(16,16);
        string imm_new, tline2_new;
        int imm_int;

        //load
        if(op == "100000" && op == "100001"
                && op =="100010" && op == "100110"
                && op == "100011" && op == "100100"
                && op == "100101"){
            imm_int = signedbinarystringtoint(imm);
            imm_int += datasize;
            imm_new = inttobinarystring(imm_int, 32);
            tline2_new = op + rs + rt + imm_new;
            linktext << tline2_new << endl;
        }
        else if(op == "101000" && op == "101001"
                 && op == "101011" && op == "101010"
                 && op == "101110"){
            imm_int = signedbinarystringtoint(imm);
            imm_int += datasize;
            imm_new = inttobinarystring(imm_int, 32);
            tline2_new = op + rs + rt + imm_new;
            linktext << tline2_new << endl;
        }
        else{
            linktext << tline2 << endl;
        }

    }
    text2.close();
    linktext.close();

}

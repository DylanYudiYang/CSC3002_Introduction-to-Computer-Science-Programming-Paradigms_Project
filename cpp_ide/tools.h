#ifndef _tools_h
#define _tools_h

#include <string>
#include <cmath>

inline std::string removeinnerws(std::string iline);
template<typename inttype>
inline std::string inttobinarystring(inttype n,size_t digit);

const std::string registername[32]=
{
    "$ze","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
    "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra",
};

const std::string registernumber[32]=
{
    inttobinarystring(0,5),inttobinarystring(1,5),inttobinarystring(2,5),inttobinarystring(3,5),
    inttobinarystring(4,5),inttobinarystring(5,5),inttobinarystring(6,5),inttobinarystring(7,5),
    inttobinarystring(8,5),inttobinarystring(9,5),inttobinarystring(10,5),inttobinarystring(11,5),
    inttobinarystring(12,5),inttobinarystring(13,5),inttobinarystring(14,5),inttobinarystring(15,5),
    inttobinarystring(16,5),inttobinarystring(17,5),inttobinarystring(18,5),inttobinarystring(19,5),
    inttobinarystring(20,5),inttobinarystring(21,5),inttobinarystring(22,5),inttobinarystring(23,5),
    inttobinarystring(24,5),inttobinarystring(25,5),inttobinarystring(26,5),inttobinarystring(27,5),
    inttobinarystring(28,5),inttobinarystring(29,5),inttobinarystring(30,5),inttobinarystring(31,5),
};

const std::string Rname[32]=
{
    "add","addu","and","div","divu","mult","multu","nor",
    "or","sll","sllv","sra","srav","srl","srlv","sub",
    "subu","xor","slt","sltu","jalr","jr","teq","tne",
    "tge","tgeu","tlt","tltu","mfhi","mflo","mthi","mtlo",
};

const std::string R1[10]=
{
    "add","addu","and","nor","or", "sub","subu","xor","slt","sltu",
};

const std::string R2[3]=
{
    "sll","sra","srl",
};

const std::string R3[4]=
{
    "div","divu","mult","multu",
};

const std::string R4[6]=
{
    "teq","tne","tge","tgeu","tlt","tltu",
};

const std::string R5[3]=
{
    "jr","mthi","mtlo",
};

const std::string R6[1]=
{
    "jalr",
};

const std::string R7[2]=
{
    "mfhi","mflo",
};

const std::string R8[3]=
{
    "sllv","srav","srlv",
};

const std::string Iname[36]=
{
    "addi","addiu","andi","ori","xori","lui","slti","sltiu",
    "beq","bgez","bgezal","bgtz","blez","bltzal","bltz","bne",
    "teqi","tnei","tgei","tgeiu","tlti","tltiu","lb","lbu",
    "lh","lhu","lw","lwl","lwr","ll","sb","sh",
    "sw","swl","swr","sc",
};

const std::string I1[7]=
{
    "addi","addiu","andi","ori","xori","slti","sltiu",
};

const std::string I2[2]=
{
    "beq","bne",
};

const std::string I3[6]=
{
    "teqi","tnei","tgei","tgeiu","tlti","tltiu",
};

const std::string I4[6]=
{
    "bgez","bgezal","bgtz","blez","bltzal","bltz",
};

const std::string I5[14]=
{
    "lb","lbu","lh","lhu","lw","lwl","lwr","ll","sb","sh","sw","swl","swr","sc",
};

const std::string I6[1]=
{
    "lui",
};

const std::string Jname[2]=
{
    "j","jal",
};

/*
 * Function: removesidews
 * Usage: ilinewithoutsidewhitespace=removesidews(iline);
 * ------------------------------------------------------
 * This function requires an input of a string and returns it with the side blank spaces and tabs
 * removed.
 */

inline std::string removesidews(std::string iline)
{
    iline.erase(0,iline.find_first_not_of("\t"));
    iline.erase(iline.find_last_not_of("\t")+1);
    iline.erase(0,iline.find_first_not_of(" "));
    iline.erase(iline.find_last_not_of(" ")+1);
    return iline;
}

/*
 * Function: removeinnerws
 * Usage: ilinewithoutinnerwhitespace=removeinnerws(iline);
 * --------------------------------------------------------
 * This function requires an input of a string and returns it with the inner blank spaces and tabs
 * removed.
 */

inline std::string removeinnerws(std::string iline)
{
    while (iline.find(' ',0)!=std::string::npos) iline.erase(iline.find(' ',0),1);
    while (iline.find('\t',0)!=std::string::npos) iline.erase(iline.find('\t',0),1);
    return iline;
}

/*
 * Function: inttobinarystring
 * Usage: binarystring=inttobinarystring(n, digit);
 * ------------------------------------------------
 * This function requires an input of an integer and a digit number and returns a binary number of wanted
 * digits in a string.
 */

template<typename inttype>
inline std::string inttobinarystring(inttype n,size_t digit)
{
    std::string binary;

    for (size_t i=0;i<digit;i++)
    {
        if (n<0) n+=static_cast<long long>(pow(2,digit));

        std::string number=std::to_string(n%2);

        binary=number+binary;
        n/=2;
    }
    return binary;
}

/*
 * Function: signedbinarystringtoint
 * Usage: decimal=signedbinarystringtoint(binary);
 * -----------------------------------------------
 * This function takes an input of a signed binary string and returns the corresponding decimal number.
 */

inline int signedbinarystringtoint(std::string binary)
{
    int decimal=0;

    for (size_t i=0;i<binary.length();i++)
    {
        if (binary[i]=='1')
        {
            decimal+=static_cast<int>(pow(2,-i+binary.length()-1));
        }
    }
    if (binary[0]=='1')
    {
        decimal-=static_cast<long long>(pow(2,binary.length()));
    }
    return decimal;
}

/*
 * Function: unsignedbinarystringtoint
 * Usage: decimal=unsignedbinarystringtoint(binary);
 * -----------------------------------------------
 * This function takes an input of a signed binary string and returns the corresponding decimal number.
 */

inline int unsignedbinarystringtoint(std::string binary)
{
    int decimal=0;

    for (size_t i=0;i<binary.length();i++)
    {
        if (binary[i]=='1')
        {
            decimal+=static_cast<int>(pow(2,-i+binary.length()-1));
        }
    }
    return decimal;
}

#endif

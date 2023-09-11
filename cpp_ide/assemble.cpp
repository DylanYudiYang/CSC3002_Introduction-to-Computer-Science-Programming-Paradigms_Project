#include "assemble.h"
#include "tools.h"

// /Users/liujianing/Documents/study/CSC_3050/mipscompiler/output.txt
// /Users/liujianing/Desktop/test.asm

void assemble(std::string iname,std::string oname)
{
    std::string iline,line,oline;
    std::ifstream ifile;
    std::ofstream ofile;
    std::map<std::string,size_t> labels;
    std::map<std::string,std::string> registers=name_number(registername,registernumber);
    std::map<std::string,std::string> Rs;
    std::map<std::string,std::string> Is;
    std::map<std::string,std::string> Js;
    size_t linenumber,address;

    ifile.open(iname.c_str());
    labels=label_address(ifile);
    ifile.close();
    ifile.open(iname.c_str());
    ofile.open(oname.c_str());
    linenumber=address=0;
    while (getline(ifile,iline))
    {
        std::string label;

        if (iline.find('#',0)!=std::string::npos) iline.erase(iline.find('#',0));   /* clear comment */
        if (iline.find(':',0)!=std::string::npos)
        {
            label=iline.substr(0,iline.find(':',0));                                /* get label */
            iline.erase(0,iline.find(':',0)+1);                                     /* clear label */
        }
        iline=removesidews(iline);
        if (!iline.empty())
        {
            address++;
            line=removeinnerws(iline);
            while (line.find("$zero",0)!=std::string::npos) line.erase(line.find("$zero",0)+3,2);

            /* now that the instructions are in format like "syscall", "xori$v1,$ze,123", "beq$t0,$t1,L1", "jr$ra",etc. */

            if (line=="syscall")
            {
                oline="00000000000000000000000000001100";
                ofile<<oline<<std::endl;
            } else
            {
                size_t opposition=line.find('$',0);
                std::string oprt=line.substr(0,opposition);

                if ((find(begin(Rname),end(Rname),oprt)==end(Rname))==0)
                {
                    std::string op,rs,rt,rd,shamt,funct;

                    if ((find(begin(R1),end(R1),oprt)==end(R1))==0)
                    {
                        std::string rdname=line.substr(opposition,3);
                        std::string rsname=line.substr(opposition+4,3);
                        std::string rtname=line.substr(opposition+8,3);

                        op="000000";
                        rs=registers[rsname];
                        rt=registers[rtname];
                        rd=registers[rdname];
                        shamt="00000";
                        if (oprt=="add") funct="100000";
                        else if (oprt=="addu") funct="100001";
                        else if (oprt=="and") funct="100100";
                        else if (oprt=="nor") funct="100111";
                        else if (oprt=="or") funct="100101";
                        else if (oprt=="sub") funct="100010";
                        else if (oprt=="subu") funct="100011";
                        else if (oprt=="xor") funct="100110";
                        else if (oprt=="slt") funct="101010";
                        else if (oprt=="sltu") funct="101011";
                        oline=op+rs+rt+rd+shamt+funct;
                    } else if ((find(begin(R2),end(R2),oprt)==end(R2))==0)
                    {
                        std::string rdname=line.substr(opposition,3);
                        std::string rtname=line.substr(opposition+4,3);
                        std::string shamtname=line.substr(opposition+8,2);

                        op="000000";
                        rs="00000";
                        rt=registers[rtname];
                        rd=registers[rdname];
                        shamt=inttobinarystring(stoi(shamtname),5);
                        if (oprt=="sll") funct="000000";
                        else if (oprt=="sra") funct="000011";
                        else if (oprt=="srl") funct="000010";
                        oline=op+rs+rt+rd+shamt+funct;
                    } else if ((find(begin(R3),end(R3),oprt)==end(R3))==0)
                    {
                        std::string rsname=line.substr(opposition,3);
                        std::string rtname=line.substr(opposition+4,3);

                        op="000000";
                        rs=registers[rsname];
                        rt=registers[rtname];
                        rd=shamt="00000";
                        if (oprt=="div") funct="011010";
                        else if (oprt=="divu") funct="011011";
                        else if (oprt=="mult") funct="011000";
                        else if (oprt=="multu") funct="011001";
                        oline=op+rs+rt+rd+shamt+funct;
                    } else if ((find(begin(R4),end(R4),oprt)==end(R4))==0)
                    {
                        std::string rsname=line.substr(opposition,3);
                        std::string rtname=line.substr(opposition+4,3);

                        op="000000";
                        rs=registers[rsname];
                        rt=registers[rtname];
                        rd=shamt="00000";
                        if (oprt=="teq") funct="110100";
                        else if (oprt=="tne") funct="110110";
                        else if (oprt=="tge") funct="110000";
                        else if (oprt=="tgeu") funct="110001";
                        else if (oprt=="tlt") funct="110010";
                        else if (oprt=="tltu") funct="110011";
                        oline=op+rs+rt+rd+shamt+funct;
                    } else if ((find(begin(R5),end(R5),oprt)==end(R5))==0)
                    {
                        std::string rsname=line.substr(opposition,3);

                        op="000000";
                        rs=registers[rsname];
                        rt=rd=shamt="00000";
                        if (oprt=="jr") funct="001000";
                        else if (oprt=="mthi") funct="010001";
                        else if (oprt=="mtlo") funct="010011";
                        oline=op+rs+rt+rd+shamt+funct;
                    } else if ((find(begin(R6),end(R6),oprt)==end(R6))==0)
                    {
                        std::string rsname=line.substr(opposition,3);
                        std::string rdname=line.substr(opposition+4,3);

                        op="000000";
                        rs=registers[rsname];
                        rt="00000";
                        rd=registers[rdname];
                        shamt="00000";
                        funct="001001";
                        oline=op+rs+rt+rd+shamt+funct;
                    } else if ((find(begin(R7),end(R7),oprt)==end(R7))==0)
                    {
                        std::string rdname=line.substr(opposition,3);

                        op="000000";
                        rs=rt="00000";
                        rd=registers[rdname];
                        shamt="00000";
                        if (oprt=="mfhi") funct="010000";
                        else if(oprt=="mflo") funct="010010";
                        oline=op+rs+rt+rd+shamt+funct;
                    } else if ((find(begin(R8),end(R8),oprt)==end(R8))==0)
                    {
                        std::string rdname=line.substr(opposition,3);
                        std::string rtname=line.substr(opposition+4,3);
                        std::string rsname=line.substr(opposition+8,3);

                        op="000000";
                        rs=registers[rsname];
                        rt=registers[rtname];
                        rd=registers[rdname];
                        shamt="00000";
                        if (oprt=="sllv") funct="000100";
                        else if (oprt=="srav") funct="000111";
                        else if (oprt=="srlv") funct="000110";
                        oline=op+rs+rt+rd+shamt+funct;
                    }
                    ofile<<oline<<std::endl;
                } else if ((find(begin(Iname),end(Iname),oprt)==end(Iname))==0)
                {
                    std::string op,rs,rt,imm;

                    if ((find(begin(I1),end(I1),oprt)==end(I1))==0)
                    {
                        std::string rtname=line.substr(opposition,3);
                        std::string rsname=line.substr(opposition+4,3);
                        std::string immstr=line.substr(opposition+8);

                        if (oprt=="addi") op="001000";
                        else if (oprt=="addiu") op="001001";
                        else if (oprt=="andi") op="001100";
                        else if (oprt=="ori") op="001101";
                        else if (oprt=="xori") op="001110";
                        else if (oprt=="slti") op="001010";
                        else if (oprt=="sltiu") op="001011";
                        rs=registers[rsname];
                        rt=registers[rtname];
                        imm=inttobinarystring(stoi(immstr),16);
                        oline=op+rs+rt+imm;
                    } else if ((find(begin(I2),end(I2),oprt)==end(I2))==0)
                    {
                        std::string rsname=line.substr(opposition,3);
                        std::string rtname=line.substr(opposition+4,3);
                        std::string immstr=line.substr(opposition+8);

                        if (oprt=="beq") op="000100";
                        else if (oprt=="bne") op="000101";
                        rs=registers[rsname];
                        rt=registers[rtname];
                        imm=inttobinarystring(static_cast<int>(labels[immstr]-address),16);
                        oline=op+rs+rt+imm;
                    } else if ((find(begin(I3),end(I3),oprt)==end(I3))==0)
                    {
                        std::string rsname=line.substr(opposition,3);
                        std::string immstr=line.substr(opposition+4);

                        op="000001";
                        rs=registers[rsname];
                        if (oprt=="teqi") rt="01100";
                        else if (oprt=="tnei") rt="01110";
                        else if (oprt=="tgei") rt="01000";
                        else if (oprt=="tgeiu") rt="01001";
                        else if (oprt=="tlti") rt="01010";
                        else if (oprt=="tltiu") rt="01011";
                        imm=inttobinarystring(stoi(immstr),16);
                        oline=op+rs+rt+imm;
                    } else if ((find(begin(I4),end(I4),oprt)==end(I4))==0)
                    {
                        std::string rsname=line.substr(opposition,3);
                        std::string immstr=line.substr(opposition+4,10);

                        if ((oprt=="bgez")+(oprt=="bgezal")+(oprt=="bltzal")+(oprt=="bltz")) op="000001";
                        else if (oprt=="bgtz") op="000111";
                        else if (oprt=="blez") op="000110";
                        rs=registers[rsname];
                        if (oprt=="bgez") rt="00001";
                        else if (oprt=="bgezal") rt="10001";
                        else if ((oprt=="bgtz")+(oprt=="blez")+(oprt=="bltz")) rt="00000";
                        else if (oprt=="bltzal") rt="10000";
                        imm=inttobinarystring(static_cast<int>(labels[immstr]-address),16);
                        oline=op+rs+rt+imm;
                    } else if ((find(begin(I5),end(I5),oprt)==end(I5))==0)
                    {
                        size_t immposition=line.find('(',0);
                        std::string rtname=line.substr(opposition,3);
                        std::string immstr=line.substr(opposition+4,immposition-opposition-4);
                        std::string rsname=line.substr(immposition+1,3);

                        if (oprt=="lb") op="100000";
                        else if (oprt=="lbu") op="100100";
                        else if (oprt=="lh") op="100001";
                        else if (oprt=="lhu") op="100101";
                        else if (oprt=="lw") op="100011";
                        else if (oprt=="lwl") op="100010";
                        else if (oprt=="lwr") op="100110";
                        else if (oprt=="ll") op="110000";
                        else if (oprt=="sb") op="101000";
                        else if (oprt=="sh") op="101001";
                        else if (oprt=="sw") op="101011";
                        else if (oprt=="swl") op="101010";
                        else if (oprt=="swr") op="101110";
                        else if (oprt=="sc") op="111000";
                        rs=registers[rsname];
                        rt=registers[rtname];
                        imm=inttobinarystring(stoi(immstr),16);
                        oline=op+rs+rt+imm;
                    } else if((find(begin(I6),end(I6),oprt)==end(I6))==0)
                    {
                        std::string rtname=line.substr(opposition,3);
                        std::string immstr=line.substr(opposition+4,5);

                        op="001111";
                        rs="00000";
                        rt=registers[rtname];
                        imm=inttobinarystring(stoi(immstr),16);
                        oline=op+rs+rt+imm;
                    }
                    ofile<<oline<<std::endl;
                } else
                {
                    std::string op,jtg;

                    if (line.substr(0,3)=="jal")
                    {
                        std::string jtgstr=line.substr(3,20);

                        op="000011";
                        jtg=inttobinarystring(static_cast<int>(labels[jtgstr]),26);
                        oline=op+jtg;
                    } else
                    {
                        std::string jtgstr=line.substr(1,20);

                        op="000010";
                        jtg=inttobinarystring(static_cast<int>(labels[jtgstr]),26);
                        oline=op+jtg;
                    }
                    ofile<<oline<<std::endl;
                }
            }
        }
    }
    ifile.close();
    ofile.close();
}

std::map<std::string,std::string> name_number(const std::string registername[32],const std::string registernumber[32])
{
    std::map<std::string,std::string> registers;

    for (size_t i=0;i<32;i++)
    {
        registers[registername[i]]=registernumber[i];
    }
    return registers;
}

std::map<std::string,size_t> label_address(std::istream & ifile)
{
    std::map<std::string,size_t> labels;
    size_t linenumber,address;
    std::string iline;

    linenumber=address=0;
    while (getline(ifile,iline))
    {
        std::string label;

        if (iline.find('#',0)!=std::string::npos) iline.erase(iline.find('#',0));   /* clear comment */
        if (iline.find(':',0)!=std::string::npos)
        {
            label=iline.substr(0,iline.find(':',0));                                /* get label */
            labels[label]=address;                                                  /* save address*/
            iline.erase(0,iline.find(':',0)+1);                                     /* clear label */
        }
        if (!iline.empty())
        {
            iline=removesidews(iline);
            if (!iline.empty()) address++;
        }
        linenumber++;
    }
    return labels;
}

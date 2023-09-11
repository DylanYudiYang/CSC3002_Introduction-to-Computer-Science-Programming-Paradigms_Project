#include "simulate.h"
#include <algorithm>
#include <cstring>

void simulate(std::string iname)
{
    std::string dname,tname,mname,iline;
    std::ifstream ifile;
    std::ofstream data,text;
    int ilinectr,textctr;
    void * pc,* tmp;
    long datasize;
    int ze,at,v0,v1,a0,a1,a2,a3,t0,t1,t2,t3,t4,t5,t6,t7,s0,s1,s2,s3,s4,s5,s6,s7,t8,t9,k0,k1,gp,sp,fp,ra,lo,hi;
    ze=at=v0=v1=a0=a1=a2=a3=t0=t1=t2=t3=t4=t5=t6=t7=s0=s1=s2=s3=s4=s5=s6=s7=t8=t9=k0=k1=gp=sp=fp=ra=lo=hi=0;
    int registers[32]=
    {
        ze,at,v0,v1,a0,a1,a2,a3,t0,t1,t2,t3,t4,t5,t6,t7,s0,s1,s2,s3,s4,s5,s6,s7,t8,t9,k0,k1,gp,sp,fp,ra,
    };
    std::map<std::string,int> registers_=number_name(registernumber,registers);
    long long diff;
    std::fstream sfile;
    std::map<int,int> sw_lw;

    dname=iname.substr(0,iname.find_last_of("_"))+"_data.txt";
    tname=iname.substr(0,iname.find_last_of("_"))+"_text.txt";
    mname=iname.substr(0,iname.find_last_of("_"))+"_machinecode.txt";
    ifile.open(iname.c_str());
    ilinectr=0;
    textctr=-1;

    /* detect the position of ".text" */

    while (getline(ifile,iline))
    {
        if (iline.find('#',0)!=std::string::npos) iline.erase(iline.find('#',0));
        if (!iline.empty()) iline=removesidews(iline);
        if (iline==".text")
        {
            textctr=ilinectr;
            break;
        }
        ilinectr++;
    }
    ifile.close();
    ifile.open(iname.c_str());
    if (textctr==-1)
    {
        text.open(tname.c_str());
        while (getline(ifile,iline)) text<<iline<<std::endl;
        text.close();
    } else
    {
        data.open(dname.c_str());
        text.open(tname.c_str());
        ilinectr=0;
        while (getline(ifile,iline))
        {
            if ((ilinectr!=0)&&(ilinectr<textctr))
            {
                if (iline.find('#',0)!=std::string::npos) iline.erase(iline.find('#',0));
                if (iline!="") iline=removesidews(iline);
                if (iline!="") data<<iline<<std::endl;
            } else if (ilinectr>textctr) text<<iline<<std::endl;
            ilinectr++;
        }
        data.close();
        text.close();
    }
    ifile.close();
    assemble(tname,mname);
    pc=tmp=malloc(6291456);
    diff=(long long)0x00400000-(long long)pc;
    registers_["11100"]=int((long long)tmp+1048576+diff);
    registers_["11101"]=registers_["11110"]=int((long long)tmp+6291456+diff);

    /* text segment */

    ifile.open(mname);
    while (getline(ifile,iline))
    {
        * static_cast<int *>(pc)=signedbinarystringtoint(iline);
        pc=static_cast<int *>(pc)+1;
    }
    ifile.close();
    pc=static_cast<int *>(tmp)+262144;

    /* data segment */

    ifile.open(dname);
    while (getline(ifile,iline))
    {
        std::string vname,vtype,vvalue;

        vname=iline.substr(0,iline.find(':',0));
        vtype=iline.substr(iline.find('.',0)+1,-iline.find('.',0)+iline.find(' ',iline.find('.',0))-1);
        vvalue=iline.substr(iline.find(' ',iline.find('.',0))+1);
        if (vtype=="asciiz")
        {
            char charvvalue[]="";
            char * value=charvvalue;

            vvalue=vvalue.substr(1,vvalue.length()-2);
            strcpy(charvvalue,vvalue.c_str());
            strncpy(static_cast<char *>(pc),value,vvalue.length()+1);
            pc=static_cast<char *>(pc)+(((vvalue.length()+1)/4)+((vvalue.length()+1)%4!=0))*4;
        } else if ((vtype=="word")||(vtype=="half")||(vtype=="byte"))
        {
            size_t quantityposition=0;
            std::string quantity;
            size_t quantityamount=0;

            while (1)
            { // break the std::string of elements into single element
                quantity=vvalue.substr(quantityposition,vvalue.find(',',quantityposition)-quantityposition);
                quantityamount+=1;
                if (vtype=="word")
                {
                    int _32bit=stoi(quantity);

                    memcpy(static_cast<int *>(pc),& _32bit,4);
                    pc=static_cast<int *>(pc)+1;
                } else if (vtype=="half")
                {
                    short _16bit=static_cast<short>(stoi(quantity));

                    memcpy(static_cast<short *>(pc),& _16bit,2);
                    pc=static_cast<short *>(pc)+1;
                } else if (vtype=="byte")
                {
                    char _8bit=static_cast<char>(stoi(quantity));

                    memcpy(static_cast<char *>(pc),& _8bit,1);
                    pc=static_cast<char *>(pc)+1;
                }
                quantityposition=vvalue.find(',',quantityposition)+1;
                if (vvalue.find(',',quantityposition)==std::string::npos)
                { // detect the last element
                    quantity=vvalue.substr(quantityposition);
                    quantityamount+=1;
                    if (vtype=="word")
                    {
                        int _32bit=stoi(quantity);

                        memcpy(static_cast<int *>(pc),& _32bit,4);
                        pc=static_cast<int *>(pc)+1;
                    } else if (vtype=="half")
                    {
                        short _16bit=static_cast<short>(stoi(quantity));

                        memcpy(static_cast<short *>(pc),& _16bit,2);
                        pc=static_cast<short *>(pc)+1;
                    } else if (vtype=="byte")
                    {
                        char _8bit=static_cast<char>(stoi(quantity));

                        memcpy(static_cast<char *>(pc),& _8bit,1);
                        pc=static_cast<char *>(pc)+1;
                    }
                    break;
                }
            }
        }
    }
    ifile.close();
    datasize=static_cast<int *>(pc)-static_cast<int *>(tmp);
    pc=tmp;

    /* simulate */

    while (*static_cast<int *>(pc)!=0)
    {
        std::string machinecode=inttobinarystring(static_cast<long long>(* static_cast<int *>(pc)),32);
        std::string mc1=machinecode.substr(0,6);
        std::string mc2=machinecode.substr(6,5);
        std::string mc3=machinecode.substr(11,5);
        std::string mc4=machinecode.substr(16,5);
        std::string mc5=machinecode.substr(21,5);
        std::string mc6=machinecode.substr(26,6);
        bool executed=0;

        pc=static_cast<int *>(pc)+1;
        if ((mc1+mc2+mc3+mc4+mc5+mc6=="00000000000000000000000000001100")
                &&(executed==0))
        { // system call
            syscall_(registers_["00010"],registers_["00100"],registers_["00101"],registers_["00110"],pc,sfile,diff,executed);
        }
        if ((mc1=="000000")
                &&((find(begin(registernumber),end(registernumber),mc2)==end(registernumber))==0)
                &&((find(begin(registernumber),end(registernumber),mc3)==end(registernumber))==0)
                &&((find(begin(registernumber),end(registernumber),mc4)==end(registernumber))==0)
                &&(mc5=="00000")
                &&(executed==0))
        { // R1 & R8 group
            std::string rsname=machinecode.substr(6,5);
            std::string rtname=machinecode.substr(11,5);
            std::string rdname=machinecode.substr(16,5);

            if (mc6=="100000") add_(registers_[rdname],registers_[rsname],registers_[rtname],executed);
            else if (mc6=="100001") addu_(registers_[rdname],registers_[rsname],registers_[rtname],executed);
            else if (mc6=="100100") and_(registers_[rdname],registers_[rsname],registers_[rtname],executed);
            else if (mc6=="100111") nor_(registers_[rdname],registers_[rsname],registers_[rtname],executed);
            else if (mc6=="100101") or_(registers_[rdname],registers_[rsname],registers_[rtname],executed);
            else if (mc6=="100110") xor_(registers_[rdname],registers_[rsname],registers_[rtname],executed);
            else if (mc6=="100010") sub_(registers_[rdname],registers_[rsname],registers_[rtname],executed);
            else if (mc6=="100011") subu_(registers_[rdname],registers_[rsname],registers_[rtname],executed);
            else if (mc6=="101010") slt_(registers_[rdname],registers_[rsname],registers_[rtname],executed);
            else if (mc6=="101011") sltu_(registers_[rdname],registers_[rsname],registers_[rtname],executed);
            else if (mc6=="000100") sllv_(registers_[rdname],registers_[rsname],registers_[rtname],executed);
            else if (mc6=="000111") srav_(registers_[rdname],registers_[rtname],registers_[rsname],executed);
            else if (mc6=="000110") srlv_(registers_[rdname],registers_[rtname],registers_[rsname],executed);
        }
        if ((mc1=="000000")
                &&((find(begin(registernumber),end(registernumber),mc2)==end(registernumber))==0)
                &&((find(begin(registernumber),end(registernumber),mc3)==end(registernumber))==0)
                &&((find(begin(registernumber),end(registernumber),mc4)==end(registernumber))==0)
                &&(executed==0))
        { // R2 group
            std::string rtname=machinecode.substr(11,5);
            std::string rdname=machinecode.substr(16,5);
            int shamt=static_cast<int> (signedbinarystringtoint(machinecode.substr(21,5)));

            if (shamt<0) shamt+=32;
            if (mc6=="000000") sll_(registers_[rdname],registers_[rtname],shamt,executed);
            else if (mc6=="000011") sra_(registers_[rdname],registers_[rtname],shamt,executed);
            else if (mc6=="000010") srl_(registers_[rdname],registers_[rtname],shamt,executed);
        }
        if ((mc1=="000000")
                &&((find(begin(registernumber),end(registernumber),mc2)==end(registernumber))==0)
                &&((find(begin(registernumber),end(registernumber),mc3)==end(registernumber))==0)
                &&((mc4+mc5)=="0000000000")
                &&(executed==0))
        { // R3 & R4 group
            std::string rsname=machinecode.substr(6,5);
            std::string rtname=machinecode.substr(11,5);

            if (mc6=="011010") div_(lo,hi,registers_[rsname],registers_[rtname],executed);
            else if (mc6=="011011") divu_(lo,hi,registers_[rsname],registers_[rtname],executed);
            else if (mc6=="011000") mult_(hi,lo,registers_[rsname],registers_[rtname],executed);
            else if (mc6=="011001") multu_(hi,lo,registers_[rsname],registers_[rtname],executed);
            else if (mc6=="110100") teq_(registers_[rsname],registers_[rtname],executed);
            else if (mc6=="110110") tne_(registers_[rsname],registers_[rtname],executed);
            else if (mc6=="110000") tge_(registers_[rsname],registers_[rtname],executed);
            else if (mc6=="110001") tgeu_(registers_[rsname],registers_[rtname],executed);
            else if (mc6=="110010") tlt_(registers_[rsname],registers_[rtname],executed);
            else if (mc6=="110011") tltu_(registers_[rsname],registers_[rtname],executed);
        }
        if ((mc1=="000000")
                &&((find(begin(registernumber),end(registernumber),mc2)==end(registernumber))==0)
                &&((mc3+mc4+mc5)=="000000000000000")
                &&(executed==0))
        { // R5 group
            std::string rsname=machinecode.substr(6,5);

            if (mc6=="001000") jr_(& pc,registers_[rsname],diff,& tmp,executed);
            else if (mc6=="010001") mthi_(hi,registers_[rsname],executed);
            else if (mc6=="010011") mtlo_(lo,registers_[rsname],executed);
        }
        if ((mc1=="000000")
                &&((find(begin(registernumber),end(registernumber),mc2)==end(registernumber))==0)
                &&(mc3=="00000")
                &&((find(begin(registernumber),end(registernumber),mc4)==end(registernumber))==0)
                &&(mc5=="00000")
                &(executed==0))
        { // R6 group
            std::string rsname=machinecode.substr(6,5);
            std::string rdname=machinecode.substr(16,5);

            if (mc6=="001001") jalr_(& pc,registers_[rsname],registers_[rdname],diff,executed);
        }
        if ((mc1=="000000")
                &&(mc2+mc3=="0000000000")
                &&((find(begin(registernumber),end(registernumber),mc4)==end(registernumber))==0)
                &&(mc5=="00000")
                &&(executed==0))
        { // R7 group
            std::string rdname=machinecode.substr(16,5);

            if (mc6=="010000") mfhi_(registers_[rdname],hi,executed);
            else if (mc6=="010010") mflo_(registers_[rdname],lo,executed);
        }
        if (((find(begin(registernumber),end(registernumber),mc2)==end(registernumber))==0)
                &&((find(begin(registernumber),end(registernumber),mc3)==end(registernumber))==0)
                &&(executed==0))
        { // I1 & I2 & I5 group
            std::string rsname=machinecode.substr(6,5);
            std::string rtname=machinecode.substr(11,5);
            int imm=signedbinarystringtoint("0"+machinecode.substr(16,16));

            if (mc1=="001000") addi_(registers_[rtname],registers_[rsname],imm,executed);
            else if (mc1=="001001") addiu_(registers_[rtname],registers_[rsname],imm,executed);
            else if (mc1=="001100") andi_(registers_[rtname],registers_[rsname],imm,executed);
            else if (mc1=="001101") ori_(registers_[rtname],registers_[rsname],imm,executed);
            else if (mc1=="001110") xori_(registers_[rtname],registers_[rsname],imm,executed);
            else if (mc1=="001010") slti_(registers_[rtname],registers_[rsname],imm,executed);
            else if (mc1=="001011") sltiu_(registers_[rtname],registers_[rsname],imm,executed);
            else if (mc1=="000100") beq_(& pc,registers_[rsname],registers_[rtname],imm,executed);
            else if (mc1=="000101") bne_(& pc,registers_[rsname],registers_[rtname],imm,executed);
            else if (mc1=="100000") lb_(registers_[rtname],registers_[rsname],imm,diff,executed);
            else if (mc1=="100100") lbu_(registers_[rtname],registers_[rsname],imm,diff,executed);
            else if (mc1=="100001") lh_(registers_[rtname],registers_[rsname],imm,diff,executed);
            else if (mc1=="100101") lhu_(registers_[rtname],registers_[rsname],imm,diff,executed);
            else if (mc1=="100011")
            {
                registers_[rtname]=sw_lw[static_cast<int>(registers_[rsname]+imm-diff)];
                executed=1;
            }
            else if (mc1=="100010") lwl_(registers_[rtname],registers_[rsname],imm,diff,executed);
            else if (mc1=="100110") lwr_(registers_[rtname],registers_[rsname],imm,diff,executed);
            else if (mc1=="110000") ll_(registers_[rtname],registers_[rsname],imm,diff,executed);
            else if (mc1=="101000") sb_(registers_[rtname],registers_[rsname],imm,diff,executed);
            else if (mc1=="101001") sh_(registers_[rtname],registers_[rsname],imm,diff,executed);
            else if (mc1=="101011")
            {
                sw_lw[static_cast<int>(registers_[rsname]+imm-diff)]=registers_[rtname];
                executed=1;
            }
            else if (mc1=="101010") swl_(registers_[rtname],registers_[rsname],imm,diff,executed);
            else if (mc1=="101110") swr_(registers_[rtname],registers_[rsname],imm,diff,executed);
            else if (mc1=="111000") sc_(registers_[rtname],registers_[rsname],imm,diff,executed);
        }
        if ((mc1=="000001")
                &&((find(begin(registernumber),end(registernumber),mc2)==end(registernumber))==0)
                &&(executed==0))
        { // I3 group
            std::string rsname=machinecode.substr(6,5);
            int imm=signedbinarystringtoint(machinecode.substr(16,16));

            if (mc3=="01100") teqi_(registers_[rsname],imm,executed);
            else if (mc3=="01110") tnei_(registers_[rsname],imm,executed);
            else if (mc3=="01000") tgei_(registers_[rsname],imm,executed);
            else if (mc3=="01001") tgeiu_(registers_[rsname],imm,executed);
            else if (mc3=="01010") tlti_(registers_[rsname],imm,executed);
            else if (mc3=="01011") tltiu_(registers_[rsname],imm,executed);
        }
        if (((find(begin(registernumber),end(registernumber),mc2)==end(registernumber))==0)
                &&(executed==0))
        { // I4 group
            std::string rsname=machinecode.substr(6,5);
            int offset=signedbinarystringtoint(machinecode.substr(16,16));

            if ((mc1=="000001")&&(mc3=="00001")) bgez_(& pc,registers_[rsname],offset,executed);
            else if ((mc1=="000001")&&(mc3=="10001")) bgezal_(& pc,registers_[rsname],offset,ra,diff,executed);
            else if ((mc1=="000111")&&(mc3=="01000")) bgtz_(& pc,registers_[rsname],offset,executed);
            else if ((mc1=="000110")&&(mc3=="01001")) blez_(& pc,registers_[rsname],offset,executed);
            else if ((mc1=="000001")&&(mc3=="10000")) bltzal_(& pc,registers_[rsname],offset,ra,diff,executed);
            else if ((mc1=="000001")&&(mc3=="00000")) bltz_(& pc,registers_[rsname],offset,executed);
        }
        if ((mc2=="00000")
                &&((find(begin(registernumber),end(registernumber),mc3)==end(registernumber))==0)
                &&(executed==0))
        { // I6 group
            std::string rtname=machinecode.substr(11,5);
            int imm=signedbinarystringtoint(machinecode.substr(16,16));

            if (mc1=="001111") lui_(registers_[rtname],imm,executed);
        }
        if (!executed)
        { // J format
            int target=signedbinarystringtoint(machinecode.substr(6,26));

            if (mc1=="000010") j_(& pc,target,& tmp,executed);
            else if (mc1=="000011") jal_(& pc,target,registers_["11111"],diff,& tmp,executed);
        }
    }
}

std::map<std::string,int> number_name(const std::string registernumber[32],int registers[32])
{
    std::map<std::string,int> registers_;

    for (size_t i=0;i<32;i++) registers_[registernumber[i]]=registers[i];

    return registers_;
}

void add_(int & rd,int & rs,int & rt,bool & executed)
{
    long long result=rs+rt;

    if (fabs(result)>=pow(2.0,31))
    {
        std::cout<<"add: overflow"<<std::endl;
        exit(0);
    }
    rd=int(result);
    executed=1;
}

void addu_(int & rd,int & rs,int & rt,bool & executed)
{
    rd=rs+rt;
    executed=1;
}

void and_(int & rd,int & rs,int & rt,bool & executed)
{
    std::string rs_str=std::to_string(rs);
    std::string rt_str=std::to_string(rt);
    std::string rd_str;

    rs_str=inttobinarystring(stoi(rs_str),32);
    rt_str=inttobinarystring(stoi(rt_str),32);
    for (size_t i=0;i<32;i++)
    {
        if ((rs_str[i]=='1')&&(rt_str[i]=='1')) rd_str+="1";
        else rd_str+="0";
    }
    rd=signedbinarystringtoint(rd_str);
    executed=1;
}

void div_(int & quo,int & rem,int & rs,int & rt,bool & executed)
{
    quo=rs/rt;
    rem=rs%rt;
    executed=1;
}

void divu_(int & quo,int & rem,int & rs,int & rt,bool & executed)
{
    quo=rs/rt;
    rem=rs%rt;
    executed=1;
}

void mult_(int & hi,int & lo,int & rs,int & rt,bool & executed)
{
    long long product=rs*rt;
    std::string product_str=inttobinarystring(product,64);

    lo=signedbinarystringtoint(product_str.substr(32,32));
    hi=signedbinarystringtoint(product_str.substr(0,32));
    executed=1;
}

void multu_(int & hi,int & lo,int & rs,int & rt,bool & executed)
{
    long long product=rs*rt;
    std::string product_str=inttobinarystring(product,64);

    lo=signedbinarystringtoint(product_str.substr(32,32));
    hi=signedbinarystringtoint(product_str.substr(0,32));
    executed=1;
}

void nor_(int & rd,int & rs,int & rt,bool & executed)
{
    std::string rs_str=inttobinarystring(rs,32);
    std::string rt_str=inttobinarystring(rt,32);
    std::string rd_str="";

    for (size_t i=0;i<32;i++)
    {
        if ((rs_str[i]=='0')&&(rt_str[i]=='0')) rd_str+="1";
        else rd_str+="0";
    }
    rd=signedbinarystringtoint(rd_str);
    executed=1;
}

void or_(int & rd,int & rs,int & rt,bool & executed)
{
    std::string rs_str=inttobinarystring(rs,32);
    std::string rt_str=inttobinarystring(rt,32);
    std::string rd_str="";

    for (size_t i=0;i<32;i++)
    {
        if ((rs_str[i]=='0')&&(rt_str[i]=='0')) rd_str+="0";
        else rd_str+="1";
    }
    rd=signedbinarystringtoint(rd_str);
    executed=1;
}

void sll_(int & rd,int & rt,int & shamt,bool & executed)
{
    std::string rt_str=inttobinarystring(rt,32);
    std::string rd_str=rt_str.substr(static_cast<size_t>(shamt),32-static_cast<size_t>(shamt));

    for (size_t i=0;i<static_cast<size_t>(shamt);i++) rd_str+="0";
    rd=signedbinarystringtoint(rd_str);
    executed=1;
}

void sllv_(int & rd,int & rs,int & rt,bool & executed)
{
    std::string rt_str=inttobinarystring(rt,32);
    std::string rd_str=rt_str.substr(static_cast<size_t>(rs),static_cast<size_t>(32-rs));

    for (size_t i=0;i<static_cast<size_t>(rs);i++) rd_str+="0";
    rd=signedbinarystringtoint(rd_str);
    executed=1;
}

void sra_(int & rd,int & rt,int & shamt,bool & executed)
{
    bool negative_shift=0;
    std::string rd_str="";
    std::string remain_str=inttobinarystring(rt,32).substr(0,static_cast<size_t>(32-shamt));

    if (rt<0) negative_shift=1;
    if (negative_shift) for (int i=0; i<shamt; i++) rd_str+="1";
    else for (int i=0; i<shamt; i++) rd_str+="0";
    rd_str+=remain_str;
    rd=signedbinarystringtoint(rd_str);
    executed=1;
}

void srav_(int & rd,int & rs,int & rt,bool & executed)
{
    std::string rd_str="";
    std::string remain_str=inttobinarystring(rt,32).substr(0,static_cast<size_t>(32-rs));

    for (int i=0;i<rs;i++) rd_str+="0";
    rd_str+=remain_str;
    rd=signedbinarystringtoint(rd_str);
    executed=1;
}

void srl_(int & rd,int & rt,int & shamt,bool & executed)
{
    std::string rd_str="";
    std::string remain_str=inttobinarystring(rt,32).substr(0,static_cast<size_t>(32-shamt));

    for (int i=0;i<shamt;i++) rd_str+="0";
    rd_str+=remain_str;
    rd=signedbinarystringtoint(rd_str);
    executed=1;
}

void srlv_(int & rd,int & rs,int & rt,bool & executed)
{
    std::string rd_str="";
    std::string remain_str=inttobinarystring(rt,32).substr(0,static_cast<size_t>(32-rs));

    for (int i=0;i<rs;i++) rd_str+="0";
    rd_str+=remain_str;
    rd=signedbinarystringtoint(rd_str);
    executed=1;
}

void sub_(int & rd,int & rs,int & rt,bool & executed)
{
    long long result=rs-rt;

    if (fabs(result)>=pow(2,31))
    {
        std::cout<<"sub: overflow"<<std::endl;
        exit(0);
    }
    else rd=int(result);
    executed=1;
}

void subu_(int & rd,int & rs,int & rt,bool & executed)
{
    rd=rs-rt;
    executed=1;
}

void xor_(int & rd,int & rs,int & rt,bool & executed)
{
    std::string rd_str="";
    std::string rs_str=inttobinarystring(rs,32);
    std::string rt_str=inttobinarystring(rt,32);

    for (size_t i=0;i<32;i++) if(rs_str[i]==rt_str[i]) rd_str+="0";
    else rd_str+="1";
    rd=signedbinarystringtoint(rd_str);
    executed=1;
}

void slt_(int & rd,int & rs,int & rt,bool & executed)
{
    if (rs<rt) rd=1;
    else rd=0;
    executed=1;
}

void sltu_(int & rd,int & rs,int & rt,bool & executed)
{
    if (rs<rt) rd=1;
    else rd=0;
    executed=1;
}

void teq_(int & rs,int & rt,bool & executed)
{
    if(rs==rt)
    {
        std::cout<<"teq: trap exception"<<std::endl;
        exit(0);
    }
    executed=1;
}

void tne_(int & rs,int & rt,bool & executed)
{
    if(rs!=rt)
    {
        std::cout<<"tne: trap exception"<<std::endl;
        exit(0);
    }
    executed=1;
}

void tge_(int & rs,int & rt,bool & executed)
{
    if(rs>=rt)
    {
        std::cout<<"tge: trap exception"<<std::endl;
        exit(0);
    }
    executed=1;
}

void tgeu_(int & rs,int & rt,bool & executed)
{
    if(rs>=rt)
    {
        std::cout<<"tgeu: trap exception"<<std::endl;
        exit(0);
    }
    executed=1;
}

void tlt_(int & rs,int & rt,bool & executed)
{
    if(rs<rt)
    {
        std::cout<<"tlt: trap exception"<<std::endl;
        exit(0);
    }
    executed=1;
}

void tltu_(int & rs,int & rt,bool & executed)
{
    if(rs<rt)
    {
        std::cout<<"tltu: trap exception"<<std::endl;
        exit(0);
    }
    executed=1;
}

void mfhi_(int & rd,int & hi,bool & executed)
{
    rd=hi;
    executed=1;
}

void mflo_(int & rd,int & lo,bool & executed)
{
    rd=lo;
    executed=1;
}

void mthi_(int & hi,int & rs,bool & executed)
{
    hi=rs;
    executed=1;
}

void mtlo_(int & lo,int & rs,bool & executed)
{
    lo=rs;
    executed=1;
}

void addi_(int & rt,int & rs,int & imm,bool & executed)
{
    long long result=rs+imm;

    if(fabs(result)>=pow(2,31))
    {
        std::cout<<"addi: overflow"<<std::endl;
        exit(0);
    }
    rt=int(result);
    executed=1;
}

void addiu_(int & rt,int & rs,int & imm,bool & executed)
{
    rt=rs+ imm;
    executed=1;
}

void andi_(int & rt,int & rs,int & imm,bool & executed)
{
    std::string rt_str="";
    std::string rs_str=inttobinarystring(rs,32);
    std::string imm_str=inttobinarystring(imm,32);

    for (size_t i=0;i<32;i++) if ((rs_str[i]=='1')&&(imm_str[i]=='1')) rt_str+="1";
    else rt_str+="0";
    rt=unsignedbinarystringtoint(rt_str);
    executed=1;
}

void ori_(int & rt,int & rs,int & imm,bool & executed)
{
    std::string rt_str="";
    std::string rs_str=inttobinarystring(rs,32);
    std::string imm_str=inttobinarystring(imm,32);

    for (size_t i=0;i<32;i++) if ((rs_str[i]=='0')&&(imm_str[i]=='0')) rt_str+="0";
    else rt_str+="1";
    rt=unsignedbinarystringtoint(rt_str);
    executed=1;
}

void xori_(int & rt,int & rs,int & imm,bool & executed)
{
    std::string rt_str="";
    std::string rs_str=inttobinarystring(rs,32);
    std::string imm_str=inttobinarystring(imm,32);

    for (size_t i=0;i<32;i++) if (rs_str[i]==imm_str[i]) rt_str+="0";
    else rt_str+="1";
    rt=unsignedbinarystringtoint(rt_str);
    executed=1;
}

void lui_(int & rt,int & imm,bool & executed)
{
    rt=unsignedbinarystringtoint(inttobinarystring(imm,32).substr(16,16)+"0000000000000000");
    executed=1;
}

void slti_(int & rt,int & rs,int & imm,bool & executed)
{
    if (rs<imm) rt=1;
    else rt=0;
    executed=1;
}

void sltiu_(int & rt,int & rs,int & imm,bool & executed)
{
    if (rs<imm) rt=1;
    else rt=0;
    executed=1;
}

void bgez_(void ** pc,int & rs,int & imm,bool & executed)
{
    if (rs>=0)
    {
        int * intpc=static_cast<int *>(* pc);
        intpc+=imm;
        * pc=intpc;
    }
    executed=1;
}

void bgezal_(void ** pc,int & rs,int & imm,int & ra,long long diff,bool & executed)
{
    if (rs>=0)
    {
        int * intpc=static_cast<int *>(* pc);

        ra=*(intpc+diff);
        intpc+=imm;
        * pc=intpc;
    }
    executed=1;
}

void bltz_(void ** pc,int & rs,int & imm,bool & executed)
{
    if (rs<0)
    {
        int * intpc=static_cast<int *>(* pc);

        intpc+=imm;
        * pc=intpc;
        pc+=imm;
    }
    executed=1;
}

void bltzal_(void ** pc,int & rs,int & imm,int & ra,long long diff,bool & executed)
{
    if (rs<0)
    {
        int * intpc=static_cast<int *>(* pc);

        ra=*(intpc+diff);
        intpc+=imm;
        * pc=intpc;
    }
    executed=1;
}

void teqi_(int & rs,int & imm,bool & executed)
{
    if (rs==imm)
    {
        std::cout<<"teqi: trap exception"<<std::endl;
        exit(0);
    }
    executed=1;
}

void tnei_(int & rs,int & imm,bool & executed)
{
    if (rs!=imm)
    {
        std::cout<<"tnei: trap exception"<<std::endl;
        exit(0);
    }
    executed=1;
}

void tgei_(int & rs,int & imm,bool & executed)
{
    if (rs>=imm)
    {
        std::cout<<"tgei: Trap Exception."<<std::endl;
        exit(0);
    }
    executed=1;
}

void tgeiu_(int & rs,int & imm,bool & executed)
{
    if (rs>=imm)
    {
        std::cout<<"tgeiu: trap exception"<<std::endl;
        exit(0);
    }
    executed=1;
}

void tlti_(int & rs,int & imm,bool & executed)
{
    if (rs<imm)
    {
        std::cout<<"tlti: trap exception."<<std::endl;
        exit(0);
    }
    executed=1;
}

void tltiu_(int & rs,int & imm,bool & executed)
{
    if (rs<imm)
    {
        std::cout<<"tltiu: trap exception."<<std::endl;
        exit(0);
    }
    executed=1;
}

void bgtz_(void ** pc,int & rs,int & imm,bool & executed)
{
    if (rs>0)
    {
        int * intpc=static_cast<int *>(* pc);

        intpc+=imm;
        * pc=intpc;
    }
    executed=1;
}

void blez_(void ** pc,int & rs,int & imm,bool & executed)
{
    if (rs<=0)
    {
        int * intpc=static_cast<int *>(* pc);

        intpc+=imm;
        * pc=intpc;
    }
    executed=1;
}

void beq_(void ** pc,int & rs,int & rt,int & imm,bool & executed)
{
    if (rs==rt)
    {
        int * intpc=static_cast<int *>(* pc);

        intpc+=imm;
        * pc=intpc;
    }
    executed=1;
}

void bne_(void ** pc,int & rs,int & rt,int & imm,bool & executed)
{
    if (rs!=rt)
    {
        int * intpc=static_cast<int *>(* pc);

        intpc+=imm;
        * pc=intpc;
    }
    executed=1;
}

void j_(void ** pc,int target,void ** tmp,bool & executed)
{
    int * intpc=static_cast<int *>(* pc);
    int * inttmp=static_cast<int *>(* tmp);

    intpc=inttmp+target;
    * pc=intpc;
    executed=1;
}

void jal_(void ** pc,int target,int & ra,long long diff,void ** tmp,bool & executed)
{
    int * intpc=static_cast<int *>(* pc);
    int * inttmp=static_cast<int *>(* tmp);

    ra=static_cast<int>(intpc-inttmp);
    intpc=inttmp+target;
    * pc=intpc;
    executed=1;
}

void lb_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    int8_t * lbptr=(int8_t *)(rs+imm-diff);

    rt=* lbptr;
    executed=1;
}

void lbu_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    uint8_t * lbuptr=(uint8_t *)(rs+imm-diff);

    rt=* lbuptr;
    executed=1;
}

void lh_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    int16_t * lhptr=(int16_t *)(rs+imm-diff);

    rt=* lhptr;
    executed=1;
}

void lhu_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    uint16_t * lhuptr=(uint16_t *)(rs+imm-diff);

    rt=* lhuptr;
    executed=1;
}

void lwl_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    std::string lwlstr=inttobinarystring((int16_t)(rs+imm-diff),16);
    std::string rtstr=inttobinarystring(rt,16).substr(16,16);

    rt=signedbinarystringtoint(lwlstr+rtstr);
    executed=1;
}

void lwr_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    std::string lwrstr=inttobinarystring((int16_t)(rs+imm+2-diff),16);
    std::string rtstr=inttobinarystring(rt,16).substr(0,16);

    rt=signedbinarystringtoint(rtstr+lwrstr);
    executed=1;
}

void ll_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    int32_t * llptr=(int32_t *)(rs+imm-diff);

    rt=* llptr;
    executed=1;
}

void sb_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    int8_t * sbptr=(int8_t *)(rs+imm-diff);

    * sbptr=rt;
    executed=1;
}

void sh_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    int16_t * shptr=(int16_t *)(rs+imm-diff);

    * shptr=rt;
    executed=1;
}

void swl_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    int16_t * swlptr=(int16_t *)rs+imm-diff;

    * swlptr=signedbinarystringtoint(inttobinarystring(rt,16).substr(0,16)+inttobinarystring(*(int16_t *)(rs+imm),16));
    executed=1;
}

void swr_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    int16_t * swrptr=(int16_t *)(rs+imm-diff);

    * swrptr=signedbinarystringtoint(inttobinarystring(rt,16).substr(16,16)+inttobinarystring(*(int16_t *)(rs+imm+2),16));
    executed=1;
}

void sc_(int & rt,int & rs,int imm,long long diff,bool & executed)
{
    int32_t * sc_ptr=(int32_t *)(rs+imm-diff);

    * sc_ptr=rt;
    rt=1;
    executed=1;
}

void jalr_(void ** pc,int & rs,int & rd,long long diff,bool & executed)
{
    int * intpc=static_cast<int *>(* pc);

    rd=static_cast<int>((long long)pc+diff);
    intpc=(int *)rs-diff;
    * pc=intpc;
    executed=1;
}

void jr_(void ** pc,int & rs,long long diff,void ** tmp,bool & executed)
{
    int * intpc=static_cast<int *>(* pc);
    int * inttmp=static_cast<int *>(* tmp);

    intpc=inttmp+rs;
    * pc=intpc;
    executed=1;
}

void syscall_(int & v0,int & a0,int & a1,int & a2,void * pc,std::fstream & sfile,long long diff,bool & executed)
{
    executed=1;
    switch(v0)
    {
    case 1:
        std::cout<<a0<<std::endl;
        break;
    case 4:
    {
        long long straddress=(long long)a0-diff;
        char * strptr=(char *)straddress;
        std::string printstr="";
        while (* strptr!='\0')
        {
            char ch=* strptr;
            printstr+=ch;
            strptr++;
        }
        std::cout<<printstr<<std::endl;
        break;
    }
    case 5:
        std::cin>>v0;
        break;
    case 8:
    {
        std::string readstring;
        long long straddress=(long long)v0-diff;
        char * char_ptr=(char *)straddress;

        getline(std::cin,readstring);
        readstring.erase(static_cast<size_t>(a1));
        for (char ch:readstring)
        {
            * char_ptr=ch;
            char_ptr++;
        }
        * char_ptr='\0';
        break;
    }
    case 9:
    {
        v0=(long long)pc+diff;
        pc=static_cast<int *>(pc)+a0/4;
        break;
    }
    case 10:
        exit(0);
    case 11:
        std::cout<<static_cast<char>(a0);
        break;
    case 12:
        char ch;

        std::cin>>ch;
        v0=static_cast<int>(ch);
        break;
    case 13:
    {
        long long snameaddress=(long long)a0-diff;
        char * snameptr=(char *)snameaddress;
        std::string sname;

        while (* snameptr!='\0')
        {
            sname+=*snameptr;
            snameptr++;
        }
        switch(a1)
        {
        case 0:
            sfile.open(sname,std::ios::in);
            break;
        case 1:
            sfile.open(sname,std::ios::out);
            break;
        case 2:
            sfile.open(sname,std::ios::in|std::ios::out);
            if (!sfile.is_open()) sfile.open(sname,std::ios::in|std::ios::out|std::ios::trunc);
        }
        a0=3;
        break;
    }
    case 14:
        if (a0==3)
        {
            long long writeaddress=(long long)a1-diff;
            char * buffer=(char *)writeaddress;

            for (int i=0;i<a2;i++)
            {
                * buffer=static_cast<char>(sfile.get());
                buffer++;
            }
            a0=a2;
        }
        break;
    case 15:
        if (a0==3)
        {
            long long writeaddress=(long long)a1-diff;
            char * buffer=(char *)writeaddress;

            for (int i=0;i<a2;i++)
            {
                sfile<<* buffer;
                buffer++;
            }
            a0=a2;
        }
        break;
    case 16:
        sfile.close();
        break;
    case 17:
        exit(a0);
    }
}

#ifndef _simulate_h
#define _simulate_h

#include <fstream>

#include "assemble.h"

/*
 * Function: simulate
 * Usage: simulate(iname);
 * -----------------------
 * This function takes the paths of the mips files, generates the .data, .text, and machine code files,
 * and simulates.
 */

void simulate(std::string iname);

/*
 * Function: number_name
 * Usage: registers_=number_name(registernumber, registers);
 * ---------------------------------------------------------
 * This function retruns a map of register number and register name.
 */

std::map<std::string,int> number_name(const std::string registernumber[32],int registers[32]);

/*
 * Function: syscall_
 * Usage: syscall_(registers_["00010"], registers_["00100"], registers_["00101"], registers_["00110"], pc ,sfile, diff, executed);
 * -------------------------------------------------------------------------------------------------------------------------------
 * This function does the operations of print_int, print_string, read_int, read_string, sbrk, exit, print_char, read_char, open,
 * read, write, close, and exit2 according to the value of v0.
 */

void syscall_(int & v0,int & a0,int & a1,int & a2,void * pc,std::fstream & sfile,long long diff,bool & executed);

/*
 * Function: add_, addu_
 * Usage: add_(registers_[rdname], registers_[rsname], registers_[rtname], executed),
 * addu_(registers_[rdname], registers_[rsname], registers_[rtname], executed);
 * ----------------------------------------------------------------------------
 * Addition with overflow and addition without overflow. Put the sum of registers rs and rt into rd.
 */

void add_(int & rd,int & rs,int & rt,bool & executed);
void addu_(int & rd,int & rs,int & rt,bool & executed);

/*
 * Function: and_, nor_, or_
 * Usage: and_(registers_[rdname], registers_[rsname], registers_[rtname], executed),
 * nor_(registers_[rdname], registers_[rsname], registers_[rtname], executed),
 * or_(registers_[rdname], registers_[rsname], registers_[rtname], executed),
 * xor_(registers_[rdname], registers_[rsname], registers_[rtname], executed);
 * --------------------------------------------------------------------------
 * Put the logical AND, NOR, OR, and XOR of registers rs and rt into register rd.
 */

void and_(int & rd,int & rs,int & rt,bool & executed);
void nor_(int & rd,int & rs,int & rt,bool & executed);
void or_(int & rd,int & rs,int & rt,bool & executed);
void xor_(int & rd,int & rs,int & rt,bool & executed);

/*
 * Function: sub_, subu_
 * Usage: sub_(registers_[rdname], registers_[rsname], registers_[rtname], executed),
 * subu_(registers_[rdname], registers_[rsname], registers_[rtname], executed);
 * ----------------------------------------------------------------------------
 * Subtract with overflow and subtract without overflow. Put the difference of registers rs and rt into register rd.
 */

void sub_(int & rd,int & rs,int & rt,bool & executed);
void subu_(int & rd,int & rs,int & rt,bool & executed);

/*
 * Function: slt_, slt_
 * Usage: sub_(registers_[rdname], registers_[rsname], registers_[rtname], executed),
 * subu_(registers_[rdname], registers_[rsname], registers_[rtname], executed);
 * ----------------------------------------------------------------------------
 * Set less than and set less than unsigned. Set the register rd to 1 if register rs is less than rt, and to 0 otherwise.
 */

void slt_(int & rd,int & rs,int & rt,bool & executed);
void sltu_(int & rd,int & rs,int & rt,bool & executed);

/*
 * Function: sllv_, srav_, srlv_
 * Usage: sllv_(registers_[rdname], registers_[rsname], registers_[rtname], executed),
 * srav_(registers_[rdname], registers_[rsname], registers_[rtname], executed),
 * srlv_(registers_[rdname], registers_[rsname], registers_[rtname], executed);
 * --------------------------------------------------------------------------
 * Shift register rt left (right) by the distance indicated by the register rs and put the result in register rd.
 */

void sllv_(int & rd,int & rs,int & rt,bool & executed);
void srav_(int & rd,int & rs,int & rt,bool & executed);
void srlv_(int & rd,int & rs,int & rt,bool & executed);

/*
 * Function: sll_, sra_, srl_
 * Usage: sll_(registers_[rdname], registers_[rtname], shamt, executed),
 * sra_(registers_[rdname], registers_[rtname], shamt, executed),
 * srl_(registers_[rdname], registers_[rtname], shamt, executed);
 * --------------------------------------------------------------
 * Shift register rt left (right) by the distance indicated by immediate shamt and put the result in register rd.
 */

void sll_(int & rd,int & rt,int & shamt,bool & executed);
void sra_(int & rd,int & rt,int & shamt,bool & executed);
void srl_(int & rd,int & rt,int & shamt,bool & executed);

/*
 * Function: div_, divu_
 * Usage: div_(lo, hi, registers_[rsname], registers_[rtname], executed),
 * divu_(lo, hi, registers_[rsname], registers_[rtname], executed);
 * ----------------------------------------------------------------
 * Divide with overflow and divide without overflow. Divide register rs by register rt. Leave the quotient in register lo and
 * the remainder in register hi.
 */

void div_(int & quo,int & rem,int & rs,int & rt,bool & executed);
void divu_(int & quo,int & rem,int & rs,int & rt,bool & executed);

/*
 * Function: mult_, multu_
 * Usage: mult_(hi, lo, registers_[rsname], registers_[rtname], executed),
 * multu_(hi, lo, registers_[rsname], registers_[rtname], executed);
 * ----------------------------------------------------------------
 * Multiply and unsigned multiply. Multiply registers rs and rt. Leave the low-order word of the product in register lo and
 * the high-order word in register hi.
 */

void mult_(int & hi,int & lo,int & rs,int & rt,bool & executed);
void multu_(int & hi,int & lo,int & rs,int & rt,bool & executed);

/*
 * Function: teq_, tne_
 * Usage: teq_(registers_[rsname], registers_[rtname], executed),
 * tne_(registers_[rsname], registers_[rtname], executed);
 * -------------------------------------------------------
 * Trap if equal and trap if not equal. If register rs is equal or not equal to register rt, raise a Trap exception.
 */

void teq_(int & rs,int & rt,bool & executed);
void tne_(int & rs,int & rt,bool & executed);

/*
 * Function: tge_, tgeu_
 * Usage: tge_(registers_[rsname], registers_[rtname], executed),
 * tgeu_(registers_[rsname], registers_[rtname], executed);
 * -------------------------------------------------------
 * Trap if greater equal and unsigned trap if greater equal. If register rs is greater than or equal to register rt, raise a
 * Trap exception.
 */

void tge_(int & rs,int & rt,bool & executed);
void tgeu_(int & rs,int & rt,bool & executed);

/*
 * Function: tlt_, tltu_
 * Usage: tlt_(registers_[rsname], registers_[rtname], executed),
 * tltu_(registers_[rsname], registers_[rtname], executed);
 * -------------------------------------------------------
 * Trap if less than and unsigned trap if less than. If register rs is less than register rt, raise a Trap exception.
 */

void tlt_(int & rs,int & rt,bool & executed);
void tltu_(int & rs,int & rt,bool & executed);

/*
 * Function: jr_
 * Usage: jr_(& pc, registers_[rsname], diff, & tmp, executed);
 * ------------------------------------------------------------
 * Jump register. Unconditionally jumps to the instruction whose address is in register rs.
 */

void jr_(void ** pc,int & rs,long long diff,void ** tmp,bool & executed);

/*
 * Function: mthi_, mtlo_
 * Usage: mthi_(hi, registers_[rsname], executed),
 * mtlo_(lo, registers_[rsname], executed);
 * ----------------------------------------
 * Moves to hi and moves to lo. Move register rs to the hi (lo) register.
 */

void mthi_(int & hi,int & rs,bool & executed);
void mtlo_(int & lo,int & rs,bool & executed);

/*
 * Function: jalr_
 * Usage: jalr_(& pc, registers_[rsname], registers_[rdname], diff, executed);
 * ---------------------------------------------------------------------------
 * Jumps and links register. Unconditionally jumps to the instruction whose address is in register rs. Saves the address of
 * the next instruction in register rd (which defaults to 31).
 */

void jalr_(void ** pc,int & rs,int & rd,long long diff,bool & executed);

/*
 * Function: mfhi_, mflo_
 * Usage:mfhi_(registers_[rdname], hi, executed),
 * mflo_(registers_[rdname], lo, executed);
 * ----------------------------------------
 * Moves from hi and moves from lo.
 */

void mfhi_(int & rd,int & hi,bool & executed);
void mflo_(int & rd,int & lo,bool & executed);

/*
 * Function: addi_, addiu_
 * Usage: addi_(registers_[rtname], registers_[rsname], imm, executed),
 * addiu_(registers_[rtname], registers_[rsname], imm, executed);
 * --------------------------------------------------------------
 * Addition immediate with overflow and addition immediate without overflow. Put the sum of register rs and the sign-extended
 * immediate into register rt.
 */

void addi_(int & rt,int & rs,int & imm,bool & executed);
void addiu_(int & rt,int & rs,int & imm,bool & executed);

/*
 * Function: addi_, addiu_
 * Usage: andi_(registers_[rtname], registers_[rsname], imm, executed),
 * ori_(registers_[rtname], registers_[rsname], imm, executed),
 * xori_(registers_[rtname], registers_[rsname], imm, executed);
 * --------------------------------------------------------------
 * Put the logical AND, OR, and XOR of register rs and the zero-extended immediate into register rt.
 */

void andi_(int & rt,int & rs,int & imm,bool & executed);
void ori_(int & rt,int & rs,int & imm,bool & executed);
void xori_(int & rt,int & rs,int & imm,bool & executed);

/*
 * Function: slti_, sltiu_
 * Usage: slti_(registers_[rtname], registers_[rsname], imm, executed),
 * sltiu_(registers_[rtname], registers_[rsname], imm, executed);
 * --------------------------------------------------------------
 * Set less than immediate and set less than unsigned immediate. Set register rt to 1 if register rs is less than the sign-
 * extended immediate, and to 0 otherwise.
 */

void slti_(int & rt,int & rs,int & imm,bool & executed);
void sltiu_(int & rt,int & rs,int & imm,bool & executed);

/*
 * Function: beq_, bne_
 * Usage: beq_(& pc, registers_[rsname], registers_[rtname], imm, executed),
 * bne_(& pc, registers_[rsname], registers_[rtname], imm, executed);
 * ------------------------------------------------------------------
 * Branch on euqal and branch on not equal. Conditionally branch the number of instructions specified by the offset if register
 * rs is equal or not equal to rt.
 */

void beq_(void ** pc,int & rs,int & rt,int & imm,bool & executed);
void bne_(void ** pc,int & rs,int & rt,int & imm,bool & executed);

/*
 * Function: lb_, lbu_
 * Usage: lb_(registers_[rtname], registers_[rsname], imm, diff, executed),
 * lbu_(registers_[rtname], registers_[rsname], imm, diff, executed);
 * ------------------------------------------------------------------
 * Load byte and load unsigned byte. Load the byte at address into register rt.
 */

void lb_(int & rt,int & rs,int imm,long long diff,bool & executed);
void lbu_(int & rt,int & rs,int imm,long long diff,bool & executed);

/*
 * Function: lh_, lhu_
 * Usage: lh_(registers_[rtname], registers_[rsname], imm, diff, executed),
 * lhu_(registers_[rtname], registers_[rsname], imm, diff, executed);
 * ------------------------------------------------------------------
 * Load halfword and load unsigned halfword. Load the 16-bit quantity (halfword) at address into register rt.
 */

void lh_(int & rt,int & rs,int imm,long long diff,bool & executed);
void lhu_(int & rt,int & rs,int imm,long long diff,bool & executed);

/*
 * Function: lwl_, lwr_
 * Usage: lwl_(registers_[rtname], registers_[rsname], imm, diff, executed),
 * lwr_(registers_[rtname], registers_[rsname], imm, diff, executed);
 * ------------------------------------------------------------
 * Load word left and load word right. Load the left (right) bytes from the word at the possibly unaligned address into
 * register rt.
 */

void lwl_(int & rt,int & rs,int imm,long long diff,bool & executed);
void lwr_(int & rt,int & rs,int imm,long long diff,bool & executed);

/*
 * Function: ll_
 * Usage: ll_(registers_[rtname], registers_[rsname], imm, diff, executed);
 * ------------------------------------------------------------------------
 * Load linked. Loads the 32-bit quantity (word) at address into register rt and starts an atomic read-modify-write operation.
 * This operation is completed by a store conditional (sc) instruction, which will fail if another processor writes into the block
 * containing the loaded word. Since SPIM does not simulate multiple processors, the store conditional operation always succeeds.
 */

void ll_(int & rt,int & rs,int imm,long long diff,bool & executed);

/*
 * Function: sb_, sh_
 * Usage: sb_(registers_[rtname], registers_[rsname], imm, diff, executed),
 * sh_(registers_[rtname], registers_[rsname], imm, diff, executed);
 * -----------------------------------------------------------------
 * Store the low byte and the low halfword from register rt at address.
 */

void sb_(int & rt,int & rs,int imm,long long diff,bool & executed);
void sh_(int & rt,int & rs,int imm,long long diff,bool & executed);

/*
 * Function: swl_, swr_
 * Usage: swl_(registers_[rtname], registers_[rsname], imm, diff, executed),
 * swr_(registers_[rtname], registers_[rsname], imm, diff, executed);
 * ------------------------------------------------------------------
 * Store the left (right) bytes from register rt at the possibly unaligned address.
 */

void swl_(int & rt,int & rs,int imm,long long diff,bool & executed);
void swr_(int & rt,int & rs,int imm,long long diff,bool & executed);

/*
 * Function: sc_
 * Usage: sc_(registers_[rtname], registers_[rsname], imm, diff, executed);
 * ------------------------------------------------------------------------
 * Store conditional. Stores the 32-bit quantity (word) in register rt into memory at address and completes an atomic read-modify-
 * write operation. If this atomic operation is successful, the memory word is modified and register rt is set to 1. If the atomic
 * operation fails because another processor wrote to a location in the block containing the addressed word, this instruction does not
 * modify memory and writes 0 into register rt. Since SPIM does not simulate multiple processors, the instruction always succeeds.
 */

void sc_(int & rt,int & rs,int imm,long long diff,bool & executed);

/*
 * Function: teqi_, tnei_
 * Usage: teqi_(registers_[rsname], imm, executed),
 * tnei_(registers_[rsname], imm, executed);
 * -----------------------------------------
 * Trap if equal immediate. If register rs is equal or not equal to the sign-extended value imm, raise a Trap exception.
 */

void teqi_(int & rs,int & imm,bool & executed);
void tnei_(int & rs,int & imm,bool & executed);

/*
 * Function: tgei_, tgeiu_
 * Usage: tgei_(registers_[rsname], imm, executed),
 * tgeiu_(registers_[rsname], imm, executed);
 * ------------------------------------------
 * Trap if greater equal immediate and unsigned trap if greater equal. If register rs is greater than or equal to register rt,
 * raise a Trap exception.
 */

void tgei_(int & rs,int & imm,bool & executed);
void tgeiu_(int & rs,int & imm,bool & executed);

/*
 * Function: tlti_, tltiu_
 * Usage: tlti_(registers_[rsname], imm, executed),
 * tltiu_(registers_[rsname], imm, executed);
 * ------------------------------------------
 * Trap if less than immediate and unsigned trap if less than immediate. If register rs is less than the sign-extended value imm,
 * raise a Trap exception.
 */

void tlti_(int & rs,int & imm,bool & executed);
void tltiu_(int & rs,int & imm,bool & executed);

/*
 * Function: bgez_
 * Usage: bgez_(& pc, registers_[rsname], offset, executed);
 * ---------------------------------------------------------
 * Branch on greater than equal zero. Conditionally branches the number of instructions specified by the offset if register rs is
 * greater than or equal to 0.
 */

void bgez_(void ** pc,int & rs,int & imm,bool & executed);

/*
 * Function: bgezal_
 * Usage: bgezal_(& pc, registers_[rsname], offset, ra, diff, executed);
 * ---------------------------------------------------------------------
 * Branch on greater than equal zero and link. Conditionally branches the number of instructions specified by the offset if register
 * rs is greater than or equal to 0. Save the address of the next instruction in register 31.
 */

void bgezal_(void ** pc,int & rs,int & imm,int & ra,long long diff,bool & executed);

/*
 * Function: bgtz_
 * Usage: bgtz_(& pc, registers_[rsname], offset, executed);
 * ---------------------------------------------------------
 * Branch on greater than zero. Conditionally branch the number of instructions specified by the offset if register rs is greater
 * than 0.
 */

void bgtz_(void ** pc,int & rs,int & imm,bool & executed);

/*
 * Function: blez_
 * Usage: blez_(& pc, registers_[rsname], offset, executed);
 * ---------------------------------------------------------
 * Branch on less than equal zero. Conditionally branches the number of instructions specified by the offset if register rs is less
 * than or equal to 0.
 */

void blez_(void ** pc,int & rs,int & imm,bool & executed);

/*
 * Function: bltzal_
 * Usage: bltzal_(& pc, registers_[rsname], offset, ra, diff, executed);
 * ---------------------------------------------------------------------
 * Branch on less than and link. Conditionally branches the number of instructions specified by the offset if register rs is less
 * than 0. Save the address of the next instruction in register 31.
 */

void bltzal_(void ** pc,int & rs,int & imm,int & ra,long long diff,bool & executed);

/*
 * Function: bltz_
 * Usage: bltzal_(& pc, registers_[rsname], offset, ra, diff, executed);
 * ---------------------------------------------------------------------
 * Branch on less than zero and link. Conditionally branches the number of instructions specified by the offset if register rs is
 * less than 0.
 */

void bltz_(void ** pc,int & rs,int & imm,bool & executed);

/*
 * Function: lui_
 * Usage: lui_(registers_[rtname], imm, executed);
 * -----------------------------------------------
 * Load upper immediate. Loads the lower halfword of the immediate imm into the upper halfword of reg- ister rt. The lower bits of
 * the register are set to 0.
 */

void lui_(int & rt,int & imm,bool & executed);

/*
 * Function: j_
 * Usage: j_(& pc, target, & tmp,executed);
 * ----------------------------------------
 * Jump. Unconditionally jumps to the instruction at target.
 */

void j_(void ** pc,int target,void ** tmp,bool & executed);

/*
 * Function: jal_
 * Usage: jal_(& pc, target, registers_["11111"], diff, & tmp, executed);
 * ----------------------------------------------------------------------
 * Jump and link. Unconditionally jumps to the instruction at target. Saves the address of the next instruction in register $ra.
 */

void jal_(void ** pc,int target,int & ra,long long diff,void ** tmp,bool & executed);

#endif

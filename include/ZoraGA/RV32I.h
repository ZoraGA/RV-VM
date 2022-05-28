#ifndef __ZORAGA_RVVM_RV32I_H__
#define __ZORAGA_RVVM_RV32I_H__

#include "ZoraGA/RVdefs.h"

namespace ZoraGA::RVVM::RV32
{

class RV32I:public rv32_insts
{
    public:
        rv_err isValid(rv32_inst_fmt inst);
        rv_err exec(rv32_inst_fmt inst,  rv32_regs_base &regs, rv32_mem_infos &mem_infos);

    private:
        typedef struct inst_arg
        {
            rv32_inst_fmt inst;
            rv32_regs_base *regs;
            rv32_mem_infos *mems;
        }inst_args;

        rv_err lb(inst_args args);
        rv_err lh(inst_args args);
        rv_err lw(inst_args args);
        rv_err lbu(inst_args args);
        rv_err lhu(inst_args args);
        rv_err fence(inst_args args);
        rv_err addi(inst_args args);
        rv_err slti(inst_args args);
        rv_err sltiu(inst_args args);
        rv_err xori(inst_args args);
        rv_err ori(inst_args args);
        rv_err andi(inst_args args);
        rv_err slli(inst_args args);
        rv_err srli(inst_args args);
        rv_err srai(inst_args args);
        rv_err auipc(inst_args args);
        rv_err sb(inst_args args);
        rv_err sh(inst_args args);
        rv_err sw(inst_args args);
        rv_err add(inst_args args);
        rv_err sub(inst_args args);
        rv_err sll(inst_args args);
        rv_err slt(inst_args args);
        rv_err sltu(inst_args args);
        rv_err xxor(inst_args args);
        rv_err srl(inst_args args);
        rv_err sra(inst_args args);
        rv_err orr(inst_args args);
        rv_err andd(inst_args args);
        rv_err lui(inst_args args);
        rv_err beq(inst_args args);
        rv_err bne(inst_args args);
        rv_err blt(inst_args args);
        rv_err bge(inst_args args);
        rv_err bltu(inst_args args);
        rv_err bgeu(inst_args args);
        rv_err jalr(inst_args args);
        rv_err ecall(inst_args args);
        rv_err ebreak(inst_args args);

    private:
        bool op_aa_match(rv32_inst_fmt inst);
        bool op_bbb_match(rv32_inst_fmt inst);
        bool op_cc_match(rv32_inst_fmt inst);
};

}

#endif // __ZORAGA_RVVM_RV32I_H__

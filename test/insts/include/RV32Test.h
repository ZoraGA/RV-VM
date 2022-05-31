#ifndef __RV32TEST_H__
#define __RV32TEST_H__

#include "ZoraGA/RV32.h"
#include "ZoraGA/RV32I.h"

typedef struct rv32i_args
{
    ZoraGA::RVVM::RV32::RV32I *i;
    ZoraGA::RVVM::rv32_regs *regs;
    ZoraGA::RVVM::rv32_mem_infos *mems;
    ZoraGA::RVVM::rv32_inst_fmt *inst;
}rv32i_args;

rv32i_args *rv32i_exec(rv32i_args &a);

#endif
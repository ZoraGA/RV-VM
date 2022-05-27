#ifndef __ZORAGA_RVVM_RV32I_H__
#define __ZORAGA_RVVM_RV32I_H__

#include "ZoraGA/RVdefs.h"

namespace ZoraGA::RVVM::RV32
{

class RV32I:public rv32_insts
{
    public:
        rv_err isValid(uint32_t inst, size_t size);
        rv_err exec(uint32_t inst, size_t size, rv32_regs_base &regs, rv32_mem_infos &mem_infos);
};

}

#endif // __ZORAGA_RVVM_RV32I_H__

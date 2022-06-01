#ifndef __ZORAGA_RVVM_RV32Zicsr_H__
#define __ZORAGA_RVVM_RV32Zicsr_H__

#include "ZoraGA/RVdefs.h"

namespace ZoraGA::RVVM::RV32
{

class RV32Zicsr:public rv32_inst
{

    public:
        rv_err isValid(rv32_inst_fmt inst);
        rv_err exec(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos);
        rv_err set_log(rvlog *log);
        rv_err regist(rv32_regs &regs, std::vector<std::string> &isas);

    private:
        rv_err csrrw(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems);
        rv_err csrrs(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems);
        rv_err csrrc(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems);
        rv_err csrrwi(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems);
        rv_err csrrsi(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems);
        rv_err csrrci(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems);

    private:
        bool op_aa_match(rv32_inst_fmt inst);
        bool op_bbb_match(rv32_inst_fmt inst);
        bool op_cc_match(rv32_inst_fmt inst);

    private:
        rvlog *m_log = nullptr;
};

}

#endif // __ZORAGA_RVVM_RV32Zicsr_H__

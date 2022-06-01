#ifndef __ZORAGA_RVVM_RV32Privileged_H__
#define __ZORAGA_RVVM_RV32Privileged_H__

#include "ZoraGA/RVdefs.h"

namespace ZoraGA::RVVM::RV32
{

class RV32Privileged:public rv32_inst
{
    public:
        rv_err isValid(rv32_inst_fmt inst);
        rv_err exec(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos);
        rv_err set_log(rvlog *log);
        rv_err regist(rv32_regs &regs, std::vector<std::string> &isas);

    public:
        void s_mode(bool ena);

    private:
        rv_err sret(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos);
        rv_err mret(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos);
        rv_err wfi(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos);

        rv_err sfence_vma(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos);
        rv_err sinval_vma(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos);
        rv_err sfence_w_inval(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos);
        rv_err sfence_inval_ir(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos);

    private:
        bool op_aa_match(rv32_inst_fmt inst);
        bool op_bbb_match(rv32_inst_fmt inst);
        bool op_cc_match(rv32_inst_fmt inst);

    private:
        rvlog *m_log = nullptr;
        bool m_smode = false;
};

}

#endif

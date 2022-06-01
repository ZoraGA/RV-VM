#include "ZoraGA/RV32Privileged.h"

namespace ZoraGA::RVVM::RV32
{

rv_err RV32Privileged::isValid(rv32_inst_fmt inst)
{
    rv_err err = RV_EUNDEF;
    bool opcode = ( op_aa_match(inst) && op_bbb_match(inst) && op_cc_match(inst) );
    bool ok = true;
    if (!opcode) return err;
    do{
        if (inst.R.funct3 == 0b000) {

            // SRET/MRET/WFI
            switch(inst.R.funct7) {
                case 0b0001000:
                    if (!m_smode && inst.R.rs2 != 0b00101) {
                        // ok = false;
                        break;
                    }
                case 0b0011000:
                    err = RV_EOK;
                    break;
                default:
                    break;
            }
            if (err == RV_EOK || !ok) break;

            /* S-Mode */
            if (m_smode) {
                switch(inst.R.funct7) {
                    case 0b0010001:
                    case 0b0110001:
                    case 0b0010011:
                    case 0b0110011:
                        err = RV_EOK;
                        break;
                    default:
                        break;
                }
            }
            if (err == RV_EOK || !ok) break;

            //TODO Hypervisor
        }
    }while(0);
    return err;
}

rv_err RV32Privileged::exec(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos)
{}

rv_err RV32Privileged::set_log(rvlog *log)
{
    m_log = log;
    return RV_EOK;
}

rv_err RV32Privileged::regist(rv32_regs &regs, std::vector<std::string> &isas)
{
    std::bitset<32> x;

    /* Unprivileged */
    x.reset();
    for (size_t i=0xC00; i<0xC20; i++) {
        regs.ctl->csrs[rv_csr_addr(i)] = x;
    }
    for (size_t i=0xC80; i<0xCA0; i++) {
        regs.ctl->csrs[rv_csr_addr(i)] = x;
    }

    /* M-Mode CSR initialize */
    regs.ctl->csrs[rv_csr_addr(CSR_mvendorid)]  = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_marchid)]    = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mimpid)]     = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mhartid)]    = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mconfigptr)] = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mstatus)]    = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_misa)]       = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_medeleg)]    = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mideleg)]    = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mie)]        = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mtvec)]      = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mcounteren)] = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mstatush)]   = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mscratch)]   = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mepc)]       = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mcause)]     = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mtval)]      = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mip)]        = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mtinst)]     = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mtval2)]     = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_menvcfg)]    = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_menvcfgh)]   = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mseccfg)]    = 0;
    regs.ctl->csrs[rv_csr_addr(CSR_mseccfgh)]   = 0;

    /* PMP */
    for (size_t i= 0x3A0; i<0x3B0; i++) {
        regs.ctl->csrs[rv_csr_addr(i)] = 0;
    }
    for (size_t i= 0x3B0; i<0x3F0; i++) {
        regs.ctl->csrs[rv_csr_addr(i)] = 0;
    }

    /* Counter/Timers */
    regs.ctl->csrs[rv_csr_addr(CSR_mcycle)] = 0;
    for (size_t i= 0xB02; i<0xB20; i++) {
        regs.ctl->csrs[rv_csr_addr(i)] = 0;
    }
    regs.ctl->csrs[rv_csr_addr(CSR_mcycleh)] = 0;
    for (size_t i= 0xB82; i<0xBA0; i++) {
        regs.ctl->csrs[rv_csr_addr(i)] = 0;
    }
    regs.ctl->csrs[rv_csr_addr(CSR_mcountinhibit)] = 0;
    for (size_t i= 0x323; i<0x340; i++) {
        regs.ctl->csrs[rv_csr_addr(i)] = 0;
    }

    /* Debug */
    for (size_t i= 0x7A0; i<0x7A4; i++) {
        regs.ctl->csrs[rv_csr_addr(i)] = 0;
    }
    regs.ctl->csrs[rv_csr_addr(CSR_mcontext)] = 0;
    for (size_t i= 0x7B0; i<0x7B4; i++) {
        regs.ctl->csrs[rv_csr_addr(i)] = 0;
    }

    /* S-Mode CSR initialize */

    /* Hypervisor CSR initialize */
    return RV_EOK;
}

void RV32Privileged::s_mode(bool ena)
{
    m_smode = ena;
}

bool RV32Privileged::op_cc_match(rv32_inst_fmt inst)
{
    return inst.cc == 0b11;
}

bool RV32Privileged::op_bbb_match(rv32_inst_fmt inst)
{
    return inst.bbb == 0b100;
}

bool RV32Privileged::op_aa_match(rv32_inst_fmt inst)
{
    return inst.aa == 0b11;
}

}
#include "ZoraGA/RV32Zicsr.h"

#define LOGI(fmt, ...) if (m_log) m_log->I(fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) if (m_log) m_log->E(fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) if (m_log) m_log->W(fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) if (m_log) m_log->D(fmt, ##__VA_ARGS__)
#define LOGV(fmt, ...) if (m_log) m_log->V(fmt, ##__VA_ARGS__)
#define LOGINST(fmt, ...) if (m_log) m_log->inst(fmt, ##__VA_ARGS__)
#define LOGREGS(fmt, ...) if (m_log) m_log->regs(fmt, ##__VA_ARGS__)

namespace ZoraGA::RVVM::RV32
{

rv_err RV32Zicsr::isValid(rv32_inst_fmt inst)
{
    rv_err err = RV_EUNDEF;
    bool opcode = ( op_aa_match(inst) && op_bbb_match(inst) && op_cc_match(inst) );
    if (opcode) {
        switch(inst.I.funct3) {
            case 0b001:
            case 0b010:
            case 0b011:
            case 0b101:
            case 0b110:
            case 0b111:
                err = RV_EOK;
                break;
            default:
                break;
        }
    }
    return err;
}

rv_err RV32Zicsr::exec(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos)
{
    rv_err err = RV_EUNDEF;
    do{
        if (inst.opcode != 0b1110011) break;
        switch(inst.I.funct3) {
            case 0b001:
                err = csrrw(inst, regs, mem_infos);
                break;
            case 0b010:
                err = csrrs(inst, regs, mem_infos);
                break;
            case 0b011:
                err = csrrc(inst, regs, mem_infos);
                break;
            case 0b101:
                err = csrrwi(inst, regs, mem_infos);
                break;
            case 0b110:
                err = csrrsi(inst, regs, mem_infos);
                break;
            case 0b111:
                err = csrrci(inst, regs, mem_infos);
                break;
            default:
                break;
        }
    }while(0);
    return err;
}

rv_err RV32Zicsr::set_log(rvlog *log)
{
    m_log = log;
    return RV_EOK;
}

rv_err RV32Zicsr::regist(rv32_regs &regs, std::vector<std::string> &isas)
{
    return RV_EOK;
}

bool RV32Zicsr::op_cc_match(rv32_inst_fmt inst)
{
    return inst.cc == 0b11;
}

bool RV32Zicsr::op_bbb_match(rv32_inst_fmt inst)
{
    return inst.bbb == 0b100;
}

bool RV32Zicsr::op_aa_match(rv32_inst_fmt inst)
{
    return inst.aa == 0b11;
}

rv_err RV32Zicsr::csrrw(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems)
{
    rv_csr_addr_fmt addr = {
        .addr = inst.I.imm_11_0
    };
    if (regs.ctl->csrs.find(addr) == regs.ctl->csrs.end()) {
        LOGINST("csrrw: invalid csr %05x, rwro: %x, lowp: %x, num: %x", 
            inst.I.imm_11_0, addr.rwro, addr.lowp, addr.num);
        return RV_EININST;
    }

    //TODO permission check
    std::bitset<32> csr = regs.ctl->csrs[addr];
    regs.ctl->csrs[addr] = std::bitset<32>(regs.reg->x[inst.I.rs1]);
    if (inst.I.rd != 0){
        regs.reg->x[inst.I.rd] = csr.to_ulong();
    }

    LOGINST("csrrw: csr 0x%08x(%05x) -> x%u, and <- 0x%08x(x%u)", 
        csr.to_ulong(), inst.I.imm_11_0, inst.I.rd, regs.reg->x[inst.I.rs1], inst.I.rs1);
    return RV_EOK;
}

rv_err RV32Zicsr::csrrs(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems)
{
    rv_csr_addr_fmt addr = {
        .addr = inst.I.imm_11_0
    };
    if (regs.ctl->csrs.find(addr) == regs.ctl->csrs.end()) {
        LOGINST("csrrs: invalid csr %05x, rwro: %x, lowp: %x, num: %x", 
            inst.I.imm_11_0, addr.rwro, addr.lowp, addr.num);
        return RV_EININST;
    }

    //TODO permission check
    std::bitset<32> csr = regs.ctl->csrs[addr];
    if (inst.I.rs1 != 0) {
        regs.ctl->csrs[addr] |= std::bitset<32>(regs.reg->x[inst.I.rs1]);
    }
    regs.reg->x[inst.I.rd] = csr.to_ulong();

    LOGINST("csrrs: csr 0x%08x(%05x) -> x%u, and |= 0x%08x(x%u)", 
        regs.reg->x[inst.I.rd], inst.I.imm_11_0, inst.I.rd, regs.reg->x[inst.I.rs1], inst.I.rs1);
    return RV_EOK;
}

rv_err RV32Zicsr::csrrc(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems)
{
    rv_csr_addr_fmt addr = {
        .addr = inst.I.imm_11_0
    };
    if (regs.ctl->csrs.find(addr) == regs.ctl->csrs.end()) {
        LOGINST("csrrc: invalid csr %05x, rwro: %x, lowp: %x, num: %x", 
            inst.I.imm_11_0, addr.rwro, addr.lowp, addr.num);
        return RV_EININST;
    }

    //TODO permission check
    std::bitset<32> csr = regs.ctl->csrs[addr];
    if (inst.I.rs1 != 0) {
        regs.ctl->csrs[addr] &= ~std::bitset<32>(regs.reg->x[inst.I.rs1]);
    }
    regs.reg->x[inst.I.rd] = csr.to_ulong();

    LOGINST("csrrc: csr 0x%08x(%05x) -> x%u, and &= ~0x%08x(x%u)", 
        regs.reg->x[inst.I.rd], inst.I.imm_11_0, inst.I.rd, regs.reg->x[inst.I.rs1], inst.I.rs1);
    return RV_EOK;
}

rv_err RV32Zicsr::csrrwi(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems)
{
    rv_csr_addr_fmt addr = {
        .addr = inst.I.imm_11_0
    };
    if (regs.ctl->csrs.find(addr) == regs.ctl->csrs.end()) {
        LOGINST("csrrwi: invalid csr %05x, rwro: %x, lowp: %x, num: %x", 
            inst.I.imm_11_0, addr.rwro, addr.lowp, addr.num);
        return RV_EININST;
    }

    //TODO permission check
    std::bitset<32> csr = regs.ctl->csrs[addr];
    regs.ctl->csrs[addr] = std::bitset<32>(inst.I.zimm_4_0);
    if (inst.I.rd != 0){
        regs.reg->x[inst.I.rd] = csr.to_ulong();
    }

    LOGINST("csrrwi: csr 0x%08x(%05x) -> x%u, and <- 0x%08x", 
        regs.reg->x[inst.I.rd], inst.I.imm_11_0, inst.I.rd, inst.I.zimm_4_0);
    return RV_EOK;
}

rv_err RV32Zicsr::csrrsi(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems)
{
    rv_csr_addr_fmt addr = {
        .addr = inst.I.imm_11_0
    };
    if (regs.ctl->csrs.find(addr) == regs.ctl->csrs.end()) {
        LOGINST("csrrsi: invalid csr %05x, rwro: %x, lowp: %x, num: %x", 
            inst.I.imm_11_0, addr.rwro, addr.lowp, addr.num);
        return RV_EININST;
    }

    if (addr.rwro == 0b11) {
        LOGINST("csrrsi: csr %05x is read-only", inst.I.imm_11_0);
        return RV_EACCESS;
    }

    //TODO permission check
    std::bitset<32> csr = regs.ctl->csrs[addr];
    if (inst.I.zimm_4_0 != 0) {
        regs.ctl->csrs[addr] |= std::bitset<32>(inst.I.zimm_4_0);
    }
    regs.reg->x[inst.I.rd] = csr.to_ulong();

    LOGINST("csrrsi: csr 0x%08x(%05x) -> x%u, and |= 0x%08x", 
        regs.reg->x[inst.I.rd], inst.I.imm_11_0, inst.I.rd, inst.I.zimm_4_0);
    return RV_EOK;
}

rv_err RV32Zicsr::csrrci(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mems)
{
    rv_csr_addr_fmt addr = {
        .addr = inst.I.imm_11_0
    };
    if (regs.ctl->csrs.find(addr) == regs.ctl->csrs.end()) {
        LOGINST("csrrci: invalid csr %05x, rwro: %x, lowp: %x, num: %x", 
            inst.I.imm_11_0, addr.rwro, addr.lowp, addr.num);
        return RV_EININST;
    }

    if (addr.rwro == 0b11) {
        LOGINST("csrrci: csr %05x is read-only", inst.I.imm_11_0);
        return RV_EACCESS;
    }

    //TODO permission check
    std::bitset<32> csr = regs.ctl->csrs[addr];
    if (inst.I.zimm_4_0 != 0) {
        regs.ctl->csrs[addr] &= ~std::bitset<32>(inst.I.zimm_4_0);
    }
    regs.reg->x[inst.I.rd] = csr.to_ulong();

    LOGINST("csrrci: csr 0x%08x(%05x) -> x%u, and &= ~0x%08x", 
        regs.reg->x[inst.I.rd], inst.I.imm_11_0, inst.I.rd, inst.I.zimm_4_0);
    return RV_EOK;
}

}

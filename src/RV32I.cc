#include "ZoraGA/RV32I.h"

#define LOGI(fmt, ...) if (m_log) m_log->I(fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) if (m_log) m_log->E(fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) if (m_log) m_log->W(fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) if (m_log) m_log->D(fmt, ##__VA_ARGS__)
#define LOGV(fmt, ...) if (m_log) m_log->V(fmt, ##__VA_ARGS__)
#define LOGINST(fmt, ...) if (m_log) m_log->inst(fmt, ##__VA_ARGS__)
#define LOGREGS(fmt, ...) if (m_log) m_log->regs(fmt, ##__VA_ARGS__)

namespace ZoraGA::RVVM::RV32
{

rv_err RV32I::isValid(rv32_inst_fmt inst)
{
    return ( op_aa_match(inst) && op_bbb_match(inst) && op_cc_match(inst) ) ? RV_EOK : RV_EUNDEF;
}

rv_err RV32I::exec(rv32_inst_fmt inst,  rv32_regs &regs, rv32_mem_infos &mem_infos)
{
    bool ok = false;
    rv_err err = RV_EUNDEF;
    inst_args a;
    do{
        a.inst = inst;
        a.regs = &regs;
        a.mems = &mem_infos;
        if (inst.cc == 0b00) {
            switch(inst.bbb) {
                /* I type, lb/lh/lw/lbu/lhu */
                case 0b000:
                    if (inst.I.funct3 == 0b000) {
                        err = lb(a);
                    } else if (inst.I.funct3 == 0b001) {
                        err = lh(a);
                    } else if (inst.I.funct3 == 0b010) {
                        err = lw(a);
                    } else if (inst.I.funct3 == 0b100) {
                        err = lbu(a);
                    } else if (inst.I.funct3 == 0b101) {
                        err = lhu(a);
                    } else {
                        // err = RV_EUNDEF;
                    }
                    break;

                /* No matched */
                case 0b001:
                    break;

                /* I type, fence */
                case 0b011:
                    if (inst.I.funct3 == 0b000) {
                        err = fence(a);
                    } else {
                        // err = RV_EUNDEF;
                    }
                    break;

                /* I type, addi/slti/sltiu/xori/ori/andi/slli/srli/srai */
                case 0b100:
                    if (inst.I.funct3 == 0b000) {
                        err = addi(a);
                    } else if (inst.I.funct3 == 0b010) {
                        err = slti(a);
                    } else if (inst.I.funct3 == 0b011) {
                        err = sltiu(a);
                    } else if (inst.I.funct3 == 0b100) {
                        err = xori(a);
                    } else if (inst.I.funct3 == 0b110) {
                        err = ori(a);
                    } else if (inst.I.funct3 == 0b111) {
                        err = andi(a);
                    } else if (inst.I.funct3 == 0b001) {
                        err = slli(a);
                    } else if (inst.I.funct3 == 0b101) {
                        if (inst.R.funct7 == 0) {
                            err = srli(a);
                        } else if (inst.R.funct7 == 0b0100000) {
                            err = srai(a);
                        } else {
                            // err = RV_EUNDEF;
                        }
                    } else {
                        // err = RV_EUNDEF;
                    }
                    break;

                /* U type, auipc */
                case 0b101:
                    err = auipc(a);
                    break;

                /* No matched */
                case 0b110:
                    break;

                default:
                    break;
            }
            if (!ok) break;
        }
        else if (inst.cc == 0b01) {
            switch(inst.bbb) {
                /* S type, sb/sh/sw */
                case 0b000:
                    if (inst.S.funct3 == 0b000) {
                        err = sb(a);
                    } else if (inst.S.funct3 == 0b001) {
                        err = sh(a);
                    } else if (inst.S.funct3 == 0b010) {
                        err = sw(a);
                    } else {
                        err = RV_EUNDEF;
                    }
                    break;

                /* No matched */
                case 0b001:
                    break;
                
                /* No matched */
                case 0b011:
                    break;

                /* R type, add/sub/sll/slt/sltu/xor/srl/sra/or/and */
                case 0b100:
                    if (inst.R.funct3 == 0b000) {
                        if (inst.R.funct7 == 0) {
                            err = add(a);
                        } else if (inst.R.funct7 == 0b0100000) {
                            err = sub(a);
                        } else {
                            // err = RV_EUNDEF;
                        }
                    } else if (inst.R.funct3 == 0b001) {
                        err = sll(a);
                    } else if (inst.R.funct3 == 0b010) {
                        err = slt(a);
                    } else if (inst.R.funct3 == 0b011) {
                        err = sltu(a);
                    } else if (inst.R.funct3 == 0b100) {
                        err = xxor(a);
                    } else if (inst.R.funct3 == 0b101) {
                        if (inst.R.funct7 == 0) {
                            err = srl(a);
                        } else if (inst.R.funct7 == 0b0100000) {
                            err = sra(a);
                        } else {
                            // err = RV_EUNDEF;
                        }
                    } else if (inst.R.funct3 == 0b110) {
                        err = orr(a);
                    } else if (inst.R.funct3 == 0b111) {
                        err = andd(a);
                    }
                    break;

                /* U type, lui */
                case 0b101:
                    err = lui(a);
                    break;

                /* No matched */                
                case 0b110:
                    break;

                default:
                    break;
            }
            if (!ok) break;
        }
        else if (inst.cc == 0b11) {
            switch(inst.bbb) {
            	/* B type, beq/bne/blt/bge/bltu/bgeu */
                case 0b000:
                	if (inst.B.funct3 == 0b000) {
                        err = beq(a);
                    } else if (inst.B.funct3 == 0b001) {
                        err = bne(a);
                    } else if (inst.B.funct3 == 0b100) {
                        err = blt(a);
                    } else if (inst.B.funct3 == 0b101) {
                        err = bge(a);
                    } else if (inst.B.funct3 == 0b110) {
                        err = bltu(a);
                    } else if (inst.B.funct3 == 0b111) {
                        err = bgeu(a);
                    } else {
                        // err = RV_EUNDEF;
                    }
                    break;

                /* I type, jalr */
                case 0b001:
                    err = jalr(a);
                    break;

                /* No matched */
                case 0b011:
                    err = jal(a);
                    break;

                /* I type, ecall/ebreak */
                case 0b100:
                    if (inst.I.funct3 == 0b000) {
                        if (inst.I.imm_11_0 == 0) {
                            err = ecall(a);
                        } else if (inst.I.imm_11_0 == 1) {
                            err = ebreak(a);
                        } else {
                            // err = RV_EUNDEF;
                        }
                    }
                    break;

                /* No matched */
                case 0b101:
                    break;

                /* No matched */
                case 0b110:
                    break;
                default:
                    break;
            }
            if (!ok) break;
        }
    }while(0);
    return err;
}

rv_err RV32I::set_log(rvlog *log)
{
    m_log = log;
    return RV_EOK;
}

bool RV32I::op_aa_match(rv32_inst_fmt inst)
{
    return inst.aa == 0b11;
}

bool RV32I::op_bbb_match(rv32_inst_fmt inst)
{
    switch(inst.bbb)
    {
        case 0b000:
        case 0b001:
        case 0b011:
        case 0b100:
        case 0b101:
        case 0b110:
            return true;
        case 0b010:
        case 0b111:
        default:
            return false;
    }
}

bool RV32I::op_cc_match(rv32_inst_fmt inst)
{
    switch(inst.cc)
    {
        case 0b00:
        case 0b01:
        case 0b11:
            return true;
        case 0b10:
        default:
            return false;
    }
}

rv_err RV32I::lb(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm;
    imm.I.imm_11_0 = a.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);

    LOGINST("lb, rd: x%u, rs1: 0x%08x(x%u), off: 0x%03x, sext(off) = %08x = %d", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, imm.I.imm, dat.u32, dat.i32);

    uint32_t addr = a.regs->reg->x[a.inst.I.rs1] + dat.i32;
    uint8_t d;
    rv_err err = rv32_mem_read(addr, (void *)&d, 1, *a.mems);
    if (err != RV_EOK) return err;
    a.regs->reg->x[a.inst.I.rd] = rv32_sext(d, 7);
    return RV_EOK;
}

rv_err RV32I::lh(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm;
    imm.I.imm_11_0 = a.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);

    LOGINST("lh, rd: x%u, rs1: 0x%08x(x%u), off: 0x%03x, sext(off) = %08x = %d", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, imm.I.imm, dat.u32, dat.i32);

    uint32_t addr = a.regs->reg->x[a.inst.I.rs1] + dat.i32;
    #pragma pack(push, 1)
    union {
        uint8_t u8[2];
        uint16_t u16;
    } d;
    #pragma pack(pop)
    rv_err err = rv32_mem_read(addr, d.u8, 2, *a.mems);
    if (err != RV_EOK) return err;
    a.regs->reg->x[a.inst.I.rd] = rv32_sext(d.u16, 15);
    return RV_EOK;
}

rv_err RV32I::lw(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm;
    imm.I.imm_11_0 = a.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);

    LOGINST("lw, rd: 0x%08x(x%u), rs1: %u, off: 0x%03x, sext(off) = %08x = %d", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, imm.I.imm, dat.u32, dat.i32);

    uint32_t addr = a.regs->reg->x[a.inst.I.rs1] + dat.i32;
    union {
        uint8_t u8[4];
        uint32_t u32;
    } d;
    rv_err err = rv32_mem_read(addr, d.u8, 4, *a.mems);
    if (err != RV_EOK) return err;
    a.regs->reg->x[a.inst.I.rd] = d.u32;
    return RV_EOK;
}

rv_err RV32I::lbu(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm;
    imm.I.imm_11_0 = a.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);

    LOGINST("lbu, rd: x%u, rs1: 0x%08x(x%u), off: 0x%03x, sext(off) = %08x = %d", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, imm.I.imm, dat.u32, dat.i32);

    uint32_t addr = a.regs->reg->x[a.inst.I.rs1] + dat.i32;
    uint8_t d;
    rv_err err = rv32_mem_read(addr, (void *)&d, 1, *a.mems);
    if (err != RV_EOK) return err;
    a.regs->reg->x[a.inst.I.rd] = d;
    return RV_EOK;
}

rv_err RV32I::lhu(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm;
    imm.I.imm_11_0 = a.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);

    LOGINST("lhu, rd: x%u, rs1: 0x%08x(x%u), off: 0x%03x, sext(off) = %08x = %d", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, imm.I.imm, dat.u32, dat.i32);

    uint32_t addr = a.regs->reg->x[a.inst.I.rs1] + dat.i32;
    union {
        uint8_t u8[2];
        uint16_t u16;
    } d;
    rv_err err = rv32_mem_read(addr, d.u8, 2, *a.mems);
    if (err != RV_EOK) return err;
    a.regs->reg->x[a.inst.I.rd] = d.u16;
    return RV_EOK;
}

rv_err RV32I::fence(inst_args a)
{
    LOGINST("fence");
    return RV_EOK;
}

rv_err RV32I::addi(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = a.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);

    LOGINST("addi, rd: %u, rs1: 0x%08x(x%u), sext(0x%03x) = 0x%08x = %d", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, imm.I.imm, dat.u32, dat.i32);

    a.regs->reg->x[a.inst.I.rd] = a.regs->reg->x[a.inst.I.rs1] + dat.i32;
	return RV_EOK;
}

rv_err RV32I::slti(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = a.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);

    LOGINST("slti, rd: %u, rs1: 0x%08x(x%u), imm: 0x%03x, sext(imm) = 0x%08x = %d", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, imm.I.imm, dat.u32, dat.i32);

    a.regs->reg->x[a.inst.I.rd] = (int64_t)a.regs->reg->x[a.inst.I.rs1] < dat.i32 ? 1 : 0;
    return RV_EOK;
}

rv_err RV32I::sltiu(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = a.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);

    LOGINST("sltiu, rd: %u, rs1: 0x%08x(x%u), imm: 0x%03x, sext(imm) = 0x%08x = %d", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, imm.I.imm, dat.u32, dat.i32);

    a.regs->reg->x[a.inst.I.rd] = a.regs->reg->x[a.inst.I.rs1] < dat.u32 ? 1 : 0;
    return RV_EOK;
}

rv_err RV32I::xori(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = a.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);

    LOGINST("xori, rd: %u, rs1: 0x%08x(x%u), imm: 0x%03x, sext(imm) = 0x%08x = %d", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, imm.I.imm, dat.u32, dat.i32);

    a.regs->reg->x[a.inst.I.rd] = a.regs->reg->x[a.inst.I.rs1] ^ dat.i32;
    return RV_EOK;
}

rv_err RV32I::ori(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = a.inst.R.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);

    LOGINST("ori, rd: %u, rs1: 0x%08x(x%u), sext(0x%02x) = 0x%08x = %d", 
        a.inst.R.rd, a.regs->reg->x[a.inst.R.rs1], a.inst.R.rs1, imm.I.imm, dat.u32, dat.i32);

    a.regs->reg->x[a.inst.R.rd] = a.regs->reg->x[a.inst.R.rs1] | dat.i32;
    return RV_EOK;
}

rv_err RV32I::andi(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = a.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);
    LOGINST("andi, rd: %u, rs1: 0x%08x(x%u), sext(0x%03x) = 0x%08x = %d", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, imm.I.imm, dat.u32, dat.i32);

    a.regs->reg->x[a.inst.I.rd] = a.regs->reg->x[a.inst.I.rs1] & dat.i32;
    return RV_EOK;
}

rv_err RV32I::slli(inst_args a)
{
    LOGINST("slli, rd: %u, rs1: 0x%08x(x%u), shamt: %u", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, a.inst.I.shamt);

    if (a.inst.I.shamt & 0x20) return RV_EININST;
    a.regs->reg->x[a.inst.I.rd] = a.regs->reg->x[a.inst.I.rs1] << a.inst.I.shamt;
    return RV_EOK;
}

rv_err RV32I::srli(inst_args a)
{
    LOGINST("srli, rd: %u, rs1: 0x%08x(x%u), shamt: %u", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, a.inst.I.shamt);

    if (a.inst.I.shamt & 0x20) return RV_EININST;
    a.regs->reg->x[a.inst.I.rd] = a.regs->reg->x[a.inst.I.rs1] >> a.inst.I.shamt;
    return RV_EOK;
}

rv_err RV32I::srai(inst_args a)
{
    LOGINST("srai, rd: %u, rs1: 0x%08x(x%u), shamt: %u", 
        a.inst.I.rd, a.regs->reg->x[a.inst.I.rs1], a.inst.I.rs1, a.inst.I.shamt);

    if (a.inst.I.shamt & 0x20) return RV_EININST;
    union {
        uint32_t u32;
        uint32_t i32;
    } dat;
    dat.u32 = a.regs->reg->x[a.inst.I.rs1] >> a.inst.I.shamt;
    dat.i32 = rv32_sext(dat.u32, 31 - a.inst.I.shamt);
    a.regs->reg->x[a.inst.I.rd] = dat.u32;
    return RV_EOK;
}

rv_err RV32I::auipc(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.U.imm_31_12 = a.inst.U.imm_31_12;

    dat.i32 = rv32_sext(imm.U.imm, 31);

    LOGINST("auipc, rd:%u imm: 0x%05x, sext(imm) = %08x = %d", a.inst.U.rd, imm.U.imm_31_12, dat.u32, dat.i32);
    a.regs->reg->x[a.inst.I.rd] = a.regs->reg->pc + dat.i32;

    return RV_EOK;
}

rv_err RV32I::sb(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.S.imm_11_5 = a.inst.S.imm_11_5;
    imm.S.imm_4_0  = a.inst.S.imm_4_0;
    dat.i32 = rv32_sext(imm.S.imm, 11);
    uint32_t addr = a.regs->reg->x[a.inst.S.rs1] + dat.i32;

    LOGINST("sh, M[ (0x%08x(x%u) + {sext(0x%05x) = 0x%08x = %d}) = 0x%08x ] = 0x%08x(x%u)[7:0]", 
        a.regs->reg->x[a.inst.S.rs1], a.inst.S.rs1, imm.S.imm, dat.u32, dat.i32, addr, a.regs->reg->x[a.inst.S.rs2], a.inst.S.rs2);
    
    uint8_t  d    = a.regs->reg->x[a.inst.S.rs2];
    return rv32_mem_write(addr, &d, 1, *a.mems);
}

rv_err RV32I::sh(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.S.imm_11_5 = a.inst.S.imm_11_5;
    imm.S.imm_4_0  = a.inst.S.imm_4_0;
    dat.i32 = rv32_sext(imm.S.imm, 11);
    uint32_t addr = a.regs->reg->x[a.inst.S.rs1] + dat.i32;

    LOGINST("sh, M[ (0x%08x(x%u) + {sext(0x%05x) = 0x%08x = %d}) = 0x%08x ] = 0x%08x(x%u)[15:0]", 
        a.regs->reg->x[a.inst.S.rs1], a.inst.S.rs1, imm.S.imm, dat.u32, dat.i32, addr, a.regs->reg->x[a.inst.S.rs2], a.inst.S.rs2);
    
    union {
        uint8_t u8[2];
        uint16_t u16;
    } d;
    d.u16 = a.regs->reg->x[a.inst.S.rs2];
    return rv32_mem_write(addr, d.u8, 2, *a.mems);
}

rv_err RV32I::sw(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.S.imm_11_5 = a.inst.S.imm_11_5;
    imm.S.imm_4_0  = a.inst.S.imm_4_0;
    dat.i32 = rv32_sext(imm.S.imm, 11);
    uint32_t addr = a.regs->reg->x[a.inst.S.rs1] + dat.i32;

    LOGINST("sw, M[ (0x%08x(x%u) + {sext(0x%05x) = 0x%08x = %d}) = 0x%08x ] = 0x%08x(x%u)[31:0]", 
        a.regs->reg->x[a.inst.S.rs1], a.inst.S.rs1, imm.S.imm, dat.u32, dat.i32, addr, a.regs->reg->x[a.inst.S.rs2], a.inst.S.rs2);

    union {
        uint8_t u8[4];
        uint32_t u32;
    } d;
    d.u32 = a.regs->reg->x[a.inst.S.rs2];
    return rv32_mem_write(addr, d.u8, 4, *a.mems);
}

rv_err RV32I::add(inst_args a)
{
    LOGINST("add, rd: %u, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u)", 
        a.inst.R.rd, a.regs->reg->x[a.inst.R.rs1], a.inst.R.rs1, a.regs->reg->x[a.inst.R.rs2], a.inst.R.rs2);

    a.regs->reg->x[a.inst.R.rd] = a.regs->reg->x[a.inst.R.rs1] + a.regs->reg->x[a.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::sub(inst_args a)
{
    LOGINST("sub, rd: %u, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u)", 
        a.inst.R.rd, a.regs->reg->x[a.inst.R.rs1], a.inst.R.rs1, a.regs->reg->x[a.inst.R.rs2], a.inst.R.rs2);

    a.regs->reg->x[a.inst.R.rd] = a.regs->reg->x[a.inst.R.rs1] - a.regs->reg->x[a.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::sll(inst_args a)
{
    LOGINST("sll, rd: %u, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u)", 
        a.inst.R.rd, a.regs->reg->x[a.inst.R.rs1], a.inst.R.rs1, a.regs->reg->x[a.inst.R.rs2], a.inst.R.rs2);

    a.regs->reg->x[a.inst.R.rd] = a.regs->reg->x[a.inst.R.rs1] << a.regs->reg->x[a.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::slt(inst_args a)
{
    LOGINST("slt, rd: %u, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u)", 
        a.inst.R.rd, a.regs->reg->x[a.inst.R.rs1], a.inst.R.rs1, a.regs->reg->x[a.inst.R.rs2], a.inst.R.rs2);

    union ui32{
        uint32_t u32;
        int32_t i32;
    };
    union ui32 rs1, rs2;
    rs1.u32 = a.regs->reg->x[a.inst.R.rs1];
    rs2.u32 = a.regs->reg->x[a.inst.R.rs2];
    a.regs->reg->x[a.inst.R.rd] = rs1.i32 < rs2.i32 ? 1 : 0;
    return RV_EOK;
}

rv_err RV32I::sltu(inst_args a)
{
    LOGINST("sltu, rd: %u, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u)", 
        a.inst.R.rd, a.regs->reg->x[a.inst.R.rs1], a.inst.R.rs1, a.regs->reg->x[a.inst.R.rs2], a.inst.R.rs2);

    a.regs->reg->x[a.inst.R.rd] = a.regs->reg->x[a.inst.R.rs1] < a.regs->reg->x[a.inst.R.rs2] ? 1 : 0;
    return RV_EOK;
}

rv_err RV32I::xxor(inst_args a)
{
    LOGINST("xor, rd: %u, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u)", 
        a.inst.R.rd, a.regs->reg->x[a.inst.R.rs1], a.inst.R.rs1, a.regs->reg->x[a.inst.R.rs2], a.inst.R.rs2);

    a.regs->reg->x[a.inst.R.rd] = a.regs->reg->x[a.inst.R.rs1] ^ a.regs->reg->x[a.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::srl(inst_args a)
{
    LOGINST("srl, rd: %u, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u)", 
        a.inst.R.rd, a.regs->reg->x[a.inst.R.rs1], a.inst.R.rs1, a.regs->reg->x[a.inst.R.rs2], a.inst.R.rs2);

    a.regs->reg->x[a.inst.R.rd] = a.regs->reg->x[a.inst.R.rs1] >> a.regs->reg->x[a.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::sra(inst_args a)
{
    uint8_t rshift = a.regs->reg->x[a.inst.R.rs2] & 0x1f;

    LOGINST("sra, rd: %u, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u), shift: %u", 
        a.inst.R.rd, a.regs->reg->x[a.inst.R.rs1], a.inst.R.rs1, a.regs->reg->x[a.inst.R.rs2], a.inst.R.rs2, rshift);

    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    dat.u32 = (a.regs->reg->x[a.inst.R.rs1] >> rshift);
    dat.i32 = rv32_sext(dat.u32, 31 -  rshift);
    a.regs->reg->x[a.inst.R.rd] = dat.u32;
    return RV_EOK;
}

rv_err RV32I::orr(inst_args a)
{
    LOGINST("or, rd: %u, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u)", 
        a.inst.R.rd, a.regs->reg->x[a.inst.R.rs1], a.inst.R.rs1, a.regs->reg->x[a.inst.R.rs2], a.inst.R.rs2);

    a.regs->reg->x[a.inst.R.rd] = a.regs->reg->x[a.inst.R.rs1] | a.regs->reg->x[a.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::andd(inst_args a)
{
    LOGINST("and, rd: %u, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u)", 
        a.inst.R.rd, a.regs->reg->x[a.inst.R.rs1], a.inst.R.rs1, a.regs->reg->x[a.inst.R.rs2], a.inst.R.rs2);

    a.regs->reg->x[a.inst.R.rd] = a.regs->reg->x[a.inst.R.rs1] & a.regs->reg->x[a.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::lui(inst_args a)
{
    rv32_imm_fmt imm = {.u32 = 0};
    imm.U.imm_31_12 = a.inst.U.imm_31_12;
    LOGINST("lui, rd: %u, imm: 0x%08x", a.inst.U.rd, imm.U.imm);

    a.regs->reg->x[a.inst.U.rd] = imm.U.imm;
    return RV_EOK;
}

rv_err RV32I::beq(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = a.inst.B.imm_4_1;
    imm.B.imm_10_5 = a.inst.B.imm_10_5;
    imm.B.imm_11   = a.inst.B.imm_11;
    imm.B.imm_12   = a.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);

    LOGINST("beq, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u), sext(0x%04x) = 0x%08x, target: 0x%08x", 
        a.regs->reg->x[a.inst.B.rs1], a.inst.B.rs1, a.regs->reg->x[a.inst.B.rs2], a.inst.B.rs2, imm.B.imm, dat.u32, a.regs->reg->pc + dat.i32);

    if (a.regs->reg->x[a.inst.B.rs1] == a.regs->reg->x[a.inst.B.rs2]) {
        a.regs->reg->pc += dat.i32;
        a.regs->ctl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bne(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = a.inst.B.imm_4_1;
    imm.B.imm_10_5 = a.inst.B.imm_10_5;
    imm.B.imm_11   = a.inst.B.imm_11;
    imm.B.imm_12   = a.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);

    LOGINST("bne, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u), sext(0x%04x) = 0x%08x, target: 0x%08x", 
        a.regs->reg->x[a.inst.B.rs1], a.inst.B.rs1, a.regs->reg->x[a.inst.B.rs2], a.inst.B.rs2, imm.B.imm, dat.u32, a.regs->reg->pc + dat.i32);

    if (a.regs->reg->x[a.inst.B.rs1] != a.regs->reg->x[a.inst.B.rs2]) {
        a.regs->reg->pc +=  dat.i32;
        a.regs->ctl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::blt(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = a.inst.B.imm_4_1;
    imm.B.imm_10_5 = a.inst.B.imm_10_5;
    imm.B.imm_11   = a.inst.B.imm_11;
    imm.B.imm_12   = a.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);

    LOGINST("blt, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u), imm: 0x%04x, sext(imm) = 0x%08x, target: 0x%08x", 
        a.regs->reg->x[a.inst.B.rs1], a.inst.B.rs1, a.regs->reg->x[a.inst.B.rs2], a.inst.B.rs2, imm.B.imm, dat.u32, a.regs->reg->pc + dat.i32);

    if ( *((int32_t*)&a.regs->reg->x[a.inst.B.rs1]) < *((int32_t*)&a.regs->reg->x[a.inst.B.rs2]) )
    {
        a.regs->reg->pc += dat.i32;
        a.regs->ctl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bge(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = a.inst.B.imm_4_1;
    imm.B.imm_10_5 = a.inst.B.imm_10_5;
    imm.B.imm_11   = a.inst.B.imm_11;
    imm.B.imm_12   = a.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);

    LOGINST("bge, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u), imm: 0x%04x, sext(imm) = 0x%08x, target: 0x%08x", 
        a.regs->reg->x[a.inst.B.rs1], a.inst.B.rs1, a.regs->reg->x[a.inst.B.rs2], a.inst.B.rs2, imm.B.imm, dat.u32, a.regs->reg->pc + dat.i32);

    if ( *((int32_t*)&a.regs->reg->x[a.inst.B.rs1]) >= *((int32_t*)&a.regs->reg->x[a.inst.B.rs2]) )
    {
        a.regs->reg->pc += dat.i32;
        a.regs->ctl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bltu(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = a.inst.B.imm_4_1;
    imm.B.imm_10_5 = a.inst.B.imm_10_5;
    imm.B.imm_11   = a.inst.B.imm_11;
    imm.B.imm_12   = a.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);

    LOGINST("blty, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u), imm: 0x%04x, sext(imm) = 0x%08x, target: 0x%08x", 
        a.regs->reg->x[a.inst.B.rs1], a.inst.B.rs1, a.regs->reg->x[a.inst.B.rs2], a.inst.B.rs2, imm.B.imm, dat.u32, a.regs->reg->pc + dat.i32);

    if ( a.regs->reg->x[a.inst.B.rs1] < a.regs->reg->x[a.inst.B.rs2] )
    {
        a.regs->reg->pc += dat.i32;
        a.regs->ctl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bgeu(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = a.inst.B.imm_4_1;
    imm.B.imm_10_5 = a.inst.B.imm_10_5;
    imm.B.imm_11   = a.inst.B.imm_11;
    imm.B.imm_12   = a.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);

    LOGINST("bgeu, rs1: 0x%08x(x%u), rs2: 0x%08x(x%u), imm: 0x%04x, sext(imm) = 0x%08x, target: 0x%08x", 
        a.regs->reg->x[a.inst.B.rs1], a.inst.B.rs1, a.regs->reg->x[a.inst.B.rs2], a.inst.B.rs2, imm.B.imm, dat.u32, a.regs->reg->pc + dat.i32);

    if ( a.regs->reg->x[a.inst.B.rs1] >= a.regs->reg->x[a.inst.B.rs2] )
    {
        a.regs->reg->pc += dat.i32;
        a.regs->ctl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::jal(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.J.imm_20    = a.inst.J.imm_20;
    imm.J.imm_19_12 = a.inst.J.imm_19_12;
    imm.J.imm_11    = a.inst.J.imm_11;
    imm.J.imm_10_1  = a.inst.J.imm_10_1;

    dat.i32 = rv32_sext(imm.J.imm, 20);
    LOGINST("jal, rd: %u, imm: 0x%03x, sext(imm) = 0x%08x, target: 0x%08x", 
        a.inst.J.rd, imm.J.imm, dat.u32, a.regs->reg->pc + dat.i32);
    a.regs->reg->x[a.inst.J.rd] = a.regs->reg->pc+4;
    a.regs->reg->pc += dat.i32;
    a.regs->ctl->pc_changed = true;
    return RV_EOK;
}

rv_err RV32I::jalr(inst_args a)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm = a.inst.I.imm_11_0;
    dat.i32 = rv32_sext(a.inst.I.imm_11_0, 11);
    LOGINST("jalr, rd: %u, rs1: %u, imm: 0x%03x, sext(imm) = 0x%08x", a.inst.I.rd, a.inst.I.rs1, imm.I.imm, dat.u32);
    uint32_t t = a.regs->reg->pc+4;
    a.regs->reg->pc = (a.regs->reg->x[a.inst.I.rs1] + dat.i32) & ~(1UL);
    a.regs->reg->x[a.inst.I.rd] = t;
    a.regs->ctl->pc_changed = true;
    return RV_EOK;
}

rv_err RV32I::ecall(inst_args a)
{
    LOGINST("ecall");
    //TODO implement
    return RV_EOK;
}

rv_err RV32I::ebreak(inst_args a)
{
    LOGINST("ebreak");
    //TODO implement
    return RV_EOK;
}

}

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

rv_err RV32I::exec(rv32_inst_fmt inst, rv32_regs_base &regs, rv32_mem_infos &mem_infos, rv32_ctrl &ctrl)
{
    bool ok = false;
    rv_err err = RV_EUNDEF;
    inst_args args;
    do{
        args.inst = inst;
        args.regs = &regs;
        args.mems = &mem_infos;
        args.ctrl = &ctrl;
        if (inst.cc == 0b00) {
            switch(inst.bbb) {
                /* I type, lb/lh/lw/lbu/lhu */
                case 0b000:
                    if (inst.I.funct3 == 0b000) {
                        err = lb(args);
                    } else if (inst.I.funct3 == 0b001) {
                        err = lh(args);
                    } else if (inst.I.funct3 == 0b010) {
                        err = lw(args);
                    } else if (inst.I.funct3 == 0b100) {
                        err = lbu(args);
                    } else if (inst.I.funct3 == 0b101) {
                        err = lhu(args);
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
                        err = fence(args);
                    } else {
                        // err = RV_EUNDEF;
                    }
                    break;

                /* I type, addi/slti/sltiu/xori/ori/andi/slli/srli/srai */
                case 0b100:
                    if (inst.I.funct3 == 0b000) {
                        err = addi(args);
                    } else if (inst.I.funct3 == 0b010) {
                        err = slti(args);
                    } else if (inst.I.funct3 == 0b011) {
                        err = sltiu(args);
                    } else if (inst.I.funct3 == 0b100) {
                        err = xori(args);
                    } else if (inst.I.funct3 == 0b110) {
                        err = ori(args);
                    } else if (inst.I.funct3 == 0b111) {
                        err = andi(args);
                    } else if (inst.I.funct3 == 0b001) {
                        err = slli(args);
                    } else if (inst.I.funct3 == 0b101) {
                        if (inst.R.funct7 == 0) {
                            err = srli(args);
                        } else if (inst.R.funct7 == 0b0100000) {
                            err = srai(args);
                        } else {
                            // err = RV_EUNDEF;
                        }
                    } else {
                        // err = RV_EUNDEF;
                    }
                    break;

                /* U type, auipc */
                case 0b101:
                    err = auipc(args);
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
                        err = sb(args);
                    } else if (inst.S.funct3 == 0b001) {
                        err = sh(args);
                    } else if (inst.S.funct3 == 0b010) {
                        err = sw(args);
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
                            err = add(args);
                        } else if (inst.R.funct7 == 0b0100000) {
                            err = sub(args);
                        } else {
                            // err = RV_EUNDEF;
                        }
                    } else if (inst.R.funct3 == 0b001) {
                        err = sll(args);
                    } else if (inst.R.funct3 == 0b010) {
                        err = slt(args);
                    } else if (inst.R.funct3 == 0b011) {
                        err = sltu(args);
                    } else if (inst.R.funct3 == 0b100) {
                        err = xxor(args);
                    } else if (inst.R.funct3 == 0b101) {
                        if (inst.R.funct7 == 0) {
                            err = srl(args);
                        } else if (inst.R.funct7 == 0b0100000) {
                            err = sra(args);
                        } else {
                            // err = RV_EUNDEF;
                        }
                    } else if (inst.R.funct3 == 0b110) {
                        err = orr(args);
                    } else if (inst.R.funct3 == 0b111) {
                        err = andd(args);
                    }
                    break;

                /* U type, lui */
                case 0b101:
                    err = lui(args);
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
                        err = beq(args);
                    } else if (inst.B.funct3 == 0b001) {
                        err = bne(args);
                    } else if (inst.B.funct3 == 0b100) {
                        err = blt(args);
                    } else if (inst.B.funct3 == 0b101) {
                        err = bge(args);
                    } else if (inst.B.funct3 == 0b110) {
                        err = bltu(args);
                    } else if (inst.B.funct3 == 0b111) {
                        err = bgeu(args);
                    } else {
                        // err = RV_EUNDEF;
                    }
                    break;

                /* I type, jalr */
                case 0b001:
                    err = jalr(args);
                    break;

                /* No matched */
                case 0b011:
                    err = jal(args);
                    break;

                /* I type, ecall/ebreak */
                case 0b100:
                    if (inst.I.funct3 == 0b000) {
                        if (inst.I.imm_11_0 == 0) {
                            err = ecall(args);
                        } else if (inst.I.imm_11_0 == 1) {
                            err = ebreak(args);
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

rv_err RV32I::lb(inst_args args)
{
    LOGINST("lb, rd: %u, rs1: %u, off: 0x%03x", args.inst.I.rd, args.inst.I.rs1, args.inst.I.imm_11_0);
    uint32_t addr = args.regs->x[args.inst.I.rs1] + rv32_sext(args.inst.I.imm_11_0, 11);
    uint8_t dat;
    rv_err err = rv32_mem_read(addr, (void *)&dat, 1, *args.mems);
    if (err != RV_EOK) return err;
    args.regs->x[args.inst.I.rd] = rv32_sext(dat, 7);
    return RV_EOK;
}

rv_err RV32I::lh(inst_args args)
{
    LOGINST("lh, rd: %u, rs1: %u, off: 0x%03x", args.inst.I.rd, args.inst.I.rs1, args.inst.I.imm_11_0);
    uint32_t addr = args.regs->x[args.inst.I.rs1] + rv32_sext(args.inst.I.imm_11_0, 11);
    union {
        uint8_t u8[2];
        uint16_t u16;
    } dat;
    rv_err err = rv32_mem_read(addr, dat.u8, 2, *args.mems);
    if (err != RV_EOK) return err;
    args.regs->x[args.inst.I.rd] = rv32_sext(dat.u16, 15);
    return RV_EOK;
}

rv_err RV32I::lw(inst_args args)
{
    LOGINST("lw, rd: %u, rs1: %u, off: 0x%03x", args.inst.I.rd, args.inst.I.rs1, args.inst.I.imm_11_0);
    uint32_t addr = args.regs->x[args.inst.I.rs1] + rv32_sext(args.inst.I.imm_11_0, 11);
    union {
        uint8_t u8[4];
        uint32_t u32;
    } dat;
    rv_err err = rv32_mem_read(addr, dat.u8, 4, *args.mems);
    if (err != RV_EOK) return err;
    args.regs->x[args.inst.I.rd] = dat.u32;
    return RV_EOK;
}

rv_err RV32I::lbu(inst_args args)
{
    LOGINST("lbu, rd: %u, rs1: %u, off: 0x%03x", args.inst.I.rd, args.inst.I.rs1, args.inst.I.imm_11_0);
    uint32_t addr = args.regs->x[args.inst.I.rs1] + rv32_sext(args.inst.I.imm_11_0, 11);
    uint8_t dat;
    rv_err err = rv32_mem_read(addr, (void *)&dat, 1, *args.mems);
    if (err != RV_EOK) return err;
    args.regs->x[args.inst.I.rd] = dat;
    return RV_EOK;
}

rv_err RV32I::lhu(inst_args args)
{
    LOGINST("lhu, rd: %u, rs1: %u, off: 0x%03x", args.inst.I.rd, args.inst.I.rs1, args.inst.I.imm_11_0);
	uint32_t addr = args.regs->x[args.inst.I.rs1] + rv32_sext(args.inst.I.imm_11_0, 11);
    union {
        uint8_t u8[2];
        uint16_t u16;
    } dat;
    rv_err err = rv32_mem_read(addr, dat.u8, 2, *args.mems);
    if (err != RV_EOK) return err;
    args.regs->x[args.inst.I.rd] = dat.u16;
    return RV_EOK;
}

rv_err RV32I::fence(inst_args args)
{
    LOGINST("fence");
    return RV_EOK;
}

rv_err RV32I::addi(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = args.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);
    LOGINST("addi, rd: %u, rs1: %u, imm: 0x%03x, sext(imm) = 0x%08x = %d", args.inst.I.rd, args.inst.I.rs1, imm.I.imm, dat.u32, dat.i32);
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] + dat.i32;
	return RV_EOK;
}

rv_err RV32I::slti(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = args.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);

    LOGINST("slti, rd: %u, rs1: %u, imm: 0x%03x, sext(imm) = 0x%08x", args.inst.I.rd, args.inst.I.rs1, imm.I.imm, dat.u32);
    args.regs->x[args.inst.I.rd] = (int64_t)args.regs->x[args.inst.I.rs1] < dat.i32 ? 1 : 0;
    return RV_EOK;
}

rv_err RV32I::sltiu(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = args.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);
    LOGINST("sltiu, rd: %u, rs1: %u, imm: 0x%03x, sext(imm) = 0x%08x", args.inst.I.rd, args.inst.I.rs1, imm.I.imm, dat.u32);
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] < dat.u32 ? 1 : 0;
    return RV_EOK;
}

rv_err RV32I::xori(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = args.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);
    LOGINST("xori, rd: %u, rs1: %u, imm: 0x%03x, sext(imm) = 0x%08x", args.inst.I.rd, args.inst.I.rs1, imm.I.imm, dat.u32);
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] ^ dat.i32;
    return RV_EOK;
}

rv_err RV32I::ori(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = args.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);
    LOGINST("ori, rd: %u, rs1: %u, imm: 0x%03x, sext(imm) = 0x%08x", args.inst.I.rd, args.inst.I.rs1, imm.I.imm, dat.u32);
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] | dat.i32;
    return RV_EOK;
}

rv_err RV32I::andi(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    }dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm_11_0 = args.inst.I.imm_11_0;
    dat.i32 = rv32_sext(imm.I.imm, 11);
    LOGINST("andi, rd: %u, rs1: %u, imm: 0x%03x, sext(imm) = 0x%08x", args.inst.I.rd, args.inst.I.rs1, imm.I.imm, dat.u32);
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] & dat.i32;
    return RV_EOK;
}

rv_err RV32I::slli(inst_args args)
{
    if (args.inst.I.shamt & 0x10) return RV_EININST;
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] << args.inst.I.shamt;
    return RV_EOK;
}

rv_err RV32I::srli(inst_args args)
{
    LOGINST("srli, rd: %u, rs1: %u, shamt: %u", args.inst.I.rd, args.inst.I.rs1, args.inst.I.shamt);
    if (args.inst.I.shamt & 0x10) return RV_EININST;
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] >> args.inst.I.shamt;
    return RV_EOK;
}

rv_err RV32I::srai(inst_args args)
{
    LOGINST("srai, rd: %u, rs1: %u, shamt: %u", args.inst.I.rd, args.inst.I.rs1, args.inst.I.shamt);
    if (args.inst.I.shamt & 0x10) return RV_EININST;
    union {
        uint32_t u32;
        uint32_t i32;
    } dat;
    dat.u32 = args.regs->x[args.inst.I.rs1] >> args.inst.I.shamt;
    dat.i32 = rv32_sext(dat.u32, 31 - args.inst.I.shamt);
    args.regs->x[args.inst.I.rd] = dat.u32;
    return RV_EOK;
}

rv_err RV32I::auipc(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.U.imm_31_12 = args.inst.U.imm_31_12;

    dat.i32 = rv32_sext(imm.U.imm, 31 );
    LOGINST("auipc, rd:%u imm: 0x%05x, sext(imm) = %08x", args.inst.U.rd, imm.U.imm_31_12, dat.u32);
    args.regs->x[args.inst.I.rd] = args.regs->pc + dat.i32;
    return RV_EOK;
}

rv_err RV32I::sb(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.S.imm_11_5 = args.inst.S.imm_11_5;
    imm.S.imm_4_0  = args.inst.S.imm_4_0;
    dat.i32 = rv32_sext(imm.S.imm, 11);

    LOGINST("sb, rs1: %u, imm: 0x%08x, sext(imm) = 0x%08x, rs2: %u", args.inst.S.rs1, imm.S.imm, dat.u32, args.inst.S.rs2);
    uint32_t addr = args.regs->x[args.inst.S.rs1] + dat.i32;
    uint8_t  d    = args.regs->x[args.inst.S.rs2];
    return rv32_mem_write(addr, &d, 1, *args.mems);
}

rv_err RV32I::sh(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.S.imm_11_5 = args.inst.S.imm_11_5;
    imm.S.imm_4_0  = args.inst.S.imm_4_0;
    dat.i32 = rv32_sext(imm.S.imm, 11);

    LOGINST("sh, rs1: %u, imm: 0x%08x, sext(imm) = 0x%08x, rs2: %u", args.inst.S.rs1, imm.S.imm, dat.u32, args.inst.S.rs2);
    uint32_t addr = args.regs->x[args.inst.S.rs1] + dat.i32;
    union {
        uint8_t u8[2];
        uint16_t u16;
    } d;
    d.u16 = args.regs->x[args.inst.S.rs2];
    return rv32_mem_write(addr, d.u8, 2, *args.mems);
}

rv_err RV32I::sw(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.S.imm_11_5 = args.inst.S.imm_11_5;
    imm.S.imm_4_0  = args.inst.S.imm_4_0;
    dat.i32 = rv32_sext(imm.S.imm, 11);
    uint32_t addr = args.regs->x[args.inst.S.rs1] + dat.i32;
    LOGINST("sw, M[ ( x(%u) + {sext(%05x) = %d} ) = 0x%08x ] = x[%u][31:0]", args.inst.S.rs1, imm.S.imm, dat.i32, addr, args.inst.S.rs2);
    union {
        uint8_t u8[4];
        uint32_t u32;
    } d;
    d.u32 = args.regs->x[args.inst.S.rs2];
    return rv32_mem_write(addr, d.u8, 4, *args.mems);
}

rv_err RV32I::add(inst_args args)
{
    LOGINST("add, rd: %u, rs1: %u, rs2: %u", args.inst.R.rd, args.inst.R.rs1, args.inst.R.rs2);
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] + args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::sub(inst_args args)
{
    LOGINST("sub, rd: %u, rs1: %u, rs2: %u", args.inst.R.rd, args.inst.R.rs1, args.inst.R.rs2);
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] - args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::sll(inst_args args)
{
    LOGINST("sll, rd: %u, rs1: %u, rs2: %u", args.inst.R.rd, args.inst.R.rs1, args.inst.R.rs2);
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] << args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::slt(inst_args args)
{
    LOGINST("slt, rd: %u, rs1: %u, rs2: %u", args.inst.R.rd, args.inst.R.rs1, args.inst.R.rs2);
    union ui32{
        uint32_t u32;
        int32_t i32;
    };
    union ui32 rs1, rs2;
    rs1.u32 = args.regs->x[args.inst.R.rs1];
    rs2.u32 = args.regs->x[args.inst.R.rs2];
    args.regs->x[args.inst.R.rd] = rs1.i32 < rs2.i32 ? 1 : 0;
    return RV_EOK;
}

rv_err RV32I::sltu(inst_args args)
{
    LOGINST("sltu, rd: %u, rs1: %u, rs2: %u", args.inst.R.rd, args.inst.R.rs1, args.inst.R.rs2);
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] < args.regs->x[args.inst.R.rs2] ? 1 : 0;
    return RV_EOK;
}

rv_err RV32I::xxor(inst_args args)
{
    LOGINST("xor, rd: %u, rs1: %u, rs2: %u", args.inst.R.rd, args.inst.R.rs1, args.inst.R.rs2);
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] ^ args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::srl(inst_args args)
{
    LOGINST("srl, rd: %u, rs1: %u, rs2: %u", args.inst.R.rd, args.inst.R.rs1, args.inst.R.rs2);
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] >> args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::sra(inst_args args)
{
    LOGINST("sra, rd: %u, rs1: %u, rs2: %u", args.inst.R.rd, args.inst.R.rs1, args.inst.R.rs2);
    uint8_t rshift = args.regs->x[args.inst.R.rs2] & 0x1f;
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    dat.u32 = (args.regs->x[args.inst.R.rs1] >> rshift);
    dat.i32 = rv32_sext(dat.u32, 31 -  rshift);
    args.regs->x[args.inst.R.rd] = dat.u32;
    return RV_EOK;
}

rv_err RV32I::orr(inst_args args)
{
    LOGINST("or, rd: %u, rs1: %u, rs2: %u", args.inst.R.rd, args.inst.R.rs1, args.inst.R.rs2);
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] | args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::andd(inst_args args)
{
    LOGINST("and, rd: %u, rs1: %u, rs2: %u", args.inst.R.rd, args.inst.R.rs1, args.inst.R.rs2);
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] & args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::lui(inst_args args)
{
    rv32_imm_fmt imm = {.u32 = 0};
    imm.U.imm_31_12 = args.inst.U.imm_31_12;
    LOGINST("lui, rd: %u, imm: 0x%08x", args.inst.U.rd, imm.U.imm);
    args.regs->x[args.inst.U.rd] = imm.U.imm;
    return RV_EOK;
}

rv_err RV32I::beq(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = args.inst.B.imm_4_1;
    imm.B.imm_10_5 = args.inst.B.imm_10_5;
    imm.B.imm_11   = args.inst.B.imm_11;
    imm.B.imm_12   = args.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);
    LOGINST("beq, rs1: %u, rs2: %u, imm: 0x%04x, sext(imm) = 0x%08x, target: 0x%08x", 
        args.inst.B.rs1, args.inst.B.rs2, imm.B.imm, dat.u32, args.regs->pc + dat.i32);
    if (args.regs->x[args.inst.B.rs1] == args.regs->x[args.inst.B.rs2]) {
        args.regs->pc += dat.i32;
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bne(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = args.inst.B.imm_4_1;
    imm.B.imm_10_5 = args.inst.B.imm_10_5;
    imm.B.imm_11   = args.inst.B.imm_11;
    imm.B.imm_12   = args.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);
    LOGINST("bne, rs1: %u, rs2: %u, imm: 0x%04x, sext(imm) = 0x%08x, target: 0x%08x", 
        args.inst.B.rs1, args.inst.B.rs2, imm.B.imm, dat.u32, args.regs->pc + dat.i32);
    if (args.regs->x[args.inst.B.rs1] != args.regs->x[args.inst.B.rs2]) {
        args.regs->pc +=  dat.i32;
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::blt(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = args.inst.B.imm_4_1;
    imm.B.imm_10_5 = args.inst.B.imm_10_5;
    imm.B.imm_11   = args.inst.B.imm_11;
    imm.B.imm_12   = args.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);
    LOGINST("blt, rs1: %u, rs2: %u, imm: 0x%04x, sext(imm) = 0x%08x, target: 0x%08x", 
        args.inst.B.rs1, args.inst.B.rs2, imm.B.imm, dat.u32, args.regs->pc + dat.i32);
    if ( *((int32_t*)&args.regs->x[args.inst.B.rs1]) < *((int32_t*)&args.regs->x[args.inst.B.rs2]) )
    {
        args.regs->pc += dat.i32;
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bge(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = args.inst.B.imm_4_1;
    imm.B.imm_10_5 = args.inst.B.imm_10_5;
    imm.B.imm_11   = args.inst.B.imm_11;
    imm.B.imm_12   = args.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);
    LOGINST("bge, rs1: %u, rs2: %u, imm: 0x%04x, sext(imm) = 0x%08x, target: 0x%08x", 
        args.inst.B.rs1, args.inst.B.rs2, imm.B.imm, dat.u32, args.regs->pc + dat.i32);
    if ( *((int32_t*)&args.regs->x[args.inst.B.rs1]) >= *((int32_t*)&args.regs->x[args.inst.B.rs2]) )
    {
        args.regs->pc += dat.i32;
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bltu(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = args.inst.B.imm_4_1;
    imm.B.imm_10_5 = args.inst.B.imm_10_5;
    imm.B.imm_11   = args.inst.B.imm_11;
    imm.B.imm_12   = args.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);
    LOGINST("bltu, rs1: %u, rs2: %u, imm: 0x%04x, sext(imm) = 0x%08x, target = 0x%08x", 
        args.inst.B.rs1, args.inst.B.rs2, imm.B.imm, dat.u32, args.regs->pc + dat.i32);
    if ( args.regs->x[args.inst.B.rs1] < args.regs->x[args.inst.B.rs2] )
    {
        args.regs->pc += dat.i32;
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bgeu(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.B.imm_4_1  = args.inst.B.imm_4_1;
    imm.B.imm_10_5 = args.inst.B.imm_10_5;
    imm.B.imm_11   = args.inst.B.imm_11;
    imm.B.imm_12   = args.inst.B.imm_12;    
    dat.i32 = rv32_sext(imm.B.imm, 12);

    LOGINST("bgeu, rs1: %u, rs2: %u, imm: 0x%04x, sext(imm) = 0x%08x = %d, target: 0x%08x", 
        args.inst.B.rs1, args.inst.B.rs2, imm.B.imm, dat.u32, dat.i32, args.regs->pc + dat.i32);
    if ( args.regs->x[args.inst.B.rs1] >= args.regs->x[args.inst.B.rs2] )
    {
        args.regs->pc += dat.i32;
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::jal(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.J.imm_20    = args.inst.J.imm_20;
    imm.J.imm_19_12 = args.inst.J.imm_19_12;
    imm.J.imm_11    = args.inst.J.imm_11;
    imm.J.imm_10_1  = args.inst.J.imm_10_1;

    dat.i32 = rv32_sext(imm.J.imm, 20);
    LOGINST("jal, rd: %u, imm: 0x%03x, sext(imm) = 0x%08x, target: 0x%08x", 
        args.inst.J.rd, imm.J.imm, dat.u32, args.regs->pc + dat.i32);
    args.regs->x[args.inst.J.rd] = args.regs->pc+4;
    args.regs->pc += dat.i32;
    args.ctrl->pc_changed = true;
    return RV_EOK;
}

rv_err RV32I::jalr(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    rv32_imm_fmt imm = {.u32 = 0};
    imm.I.imm = args.inst.I.imm_11_0;
    dat.i32 = rv32_sext(args.inst.I.imm_11_0, 11);
    LOGINST("jalr, rd: %u, rs1: %u, imm: 0x%03x, sext(imm) = 0x%08x", args.inst.I.rd, args.inst.I.rs1, imm.I.imm, dat.u32);
    uint32_t t = args.regs->pc+4;
    args.regs->pc = (args.regs->x[args.inst.I.rs1] + dat.i32) & ~(1UL);
    args.regs->x[args.inst.I.rd] = t;
    args.ctrl->pc_changed = true;
    return RV_EOK;
}

rv_err RV32I::ecall(inst_args args)
{
    LOGINST("ecall");
    //TODO implement
    return RV_EOK;
}

rv_err RV32I::ebreak(inst_args args)
{
    LOGINST("ebreak");
    //TODO implement
    return RV_EOK;
}

}
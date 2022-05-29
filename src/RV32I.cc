#include "ZoraGA/RV32I.h"

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
    uint32_t addr = args.regs->x[args.inst.I.rs1] + rv32_sext(args.inst.I.imm_11_0, 11);
    uint8_t dat;
    rv_err err = rv32_mem_read(addr, (void *)&dat, 1, *args.mems);
    if (!err != RV_EOK) return err;
    args.regs->x[args.inst.I.rd] = rv32_sext(dat, 7);
    return RV_EOK;
}

rv_err RV32I::lh(inst_args args)
{
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
    uint32_t addr = args.regs->x[args.inst.I.rs1] + rv32_sext(args.inst.I.imm_11_0, 11);
    uint8_t dat;
    rv_err err = rv32_mem_read(addr, (void *)&dat, 1, *args.mems);
    if (err != RV_EOK) return err;
    args.regs->x[args.inst.I.rd] = dat;
    return RV_EOK;
}

rv_err RV32I::lhu(inst_args args)
{
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
    return RV_EOK;
}

rv_err RV32I::addi(inst_args args)
{
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] + rv32_sext(args.inst.I.imm_11_0, 11);
	return RV_EOK;
}

rv_err RV32I::slti(inst_args args)
{
    args.regs->x[args.inst.I.rd] = (int64_t)args.regs->x[args.inst.I.rs1] < rv32_sext(args.inst.I.imm_11_0, 11) ? 1 : 0;
    return RV_EOK;
}

rv_err RV32I::sltiu(inst_args args)
{
    union {
        uint32_t u32;
        int32_t i32;
    } dat;
    dat.i32 = rv32_sext(args.inst.I.imm_11_0, 11);
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] < dat.u32 ? 1 : 0;
    return RV_EOK;
}

rv_err RV32I::xori(inst_args args)
{
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] ^ rv32_sext(args.inst.I.imm_11_0, 11);
    return RV_EOK;
}

rv_err RV32I::ori(inst_args args)
{
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] | rv32_sext(args.inst.I.imm_11_0, 11);
    return RV_EOK;
}

rv_err RV32I::andi(inst_args args)
{
    args.regs->x[args.inst.I.rd] = args.regs->x[args.inst.I.rs1] & rv32_sext(args.inst.I.imm_11_0, 11);
    return RV_EOK;
}

rv_err RV32I::slli(inst_args args)
{
    if (args.inst.R.rs2 & 0x10) return RV_EININST;
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] << args.inst.R.rs2;
    return RV_EOK;
}

rv_err RV32I::srli(inst_args args)
{
    if (args.inst.R.rs2 & 0x10) return RV_EININST;
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] >> args.inst.R.rs2;
    return RV_EOK;
}

rv_err RV32I::srai(inst_args args)
{
    if (args.inst.R.rs2 & 0x10) return RV_EININST;
    union {
        uint32_t u32;
        uint32_t i32;
    } dat;
    dat.u32 = args.regs->x[args.inst.R.rs1] >> args.inst.R.rs2;
    dat.i32 = rv32_sext(dat.u32, 31 - args.inst.R.rs2);
    args.regs->x[args.inst.R.rd] = dat.u32;
    return RV_EOK;
}

rv_err RV32I::auipc(inst_args args)
{
    args.regs->pc += rv32_sext( ((uint32_t)args.inst.U.imm_31_12)<<12, 31 );
    args.ctrl->pc_changed = true;
    return RV_EOK;
}

rv_err RV32I::sb(inst_args args)
{
    uint32_t addr = args.regs->x[args.inst.S.rs1] + rv32_sext(args.inst.S.imm_11_5, 6);
    uint8_t dat = args.regs->x[args.inst.S.rs2];
    return rv32_mem_write(addr, &dat, 1, *args.mems);
}

rv_err RV32I::sh(inst_args args)
{
    uint32_t addr = args.regs->x[args.inst.S.rs1] + rv32_sext(args.inst.S.imm_11_5, 6);
    union {
        uint8_t u8[2];
        uint16_t u16;
    } dat;
    dat.u16 = args.regs->x[args.inst.S.rs2];
    return rv32_mem_write(addr, dat.u8, 2, *args.mems);
}

rv_err RV32I::sw(inst_args args)
{
    uint32_t addr = args.regs->x[args.inst.S.rs1] + rv32_sext(args.inst.S.imm_11_5, 6);
    union {
        uint8_t u8[4];
        uint32_t u32;
    } dat;
    dat.u32 = args.regs->x[args.inst.S.rs2];
    return rv32_mem_write(addr, dat.u8, 4, *args.mems);
}

rv_err RV32I::add(inst_args args)
{
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] + args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::sub(inst_args args)
{
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] - args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::sll(inst_args args)
{
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] << args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::slt(inst_args args)
{
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
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] < args.regs->x[args.inst.R.rs2] ? 1 : 0;
    return RV_EOK;
}

rv_err RV32I::xxor(inst_args args)
{
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] ^ args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::srl(inst_args args)
{
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] >> args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::sra(inst_args args)
{
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
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] | args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::andd(inst_args args)
{
    args.regs->x[args.inst.R.rd] = args.regs->x[args.inst.R.rs1] & args.regs->x[args.inst.R.rs2];
    return RV_EOK;
}

rv_err RV32I::lui(inst_args args)
{
	args.regs->x[args.inst.U.rd] = args.inst.U.imm_31_12<<12;
    return RV_EOK;
}

rv_err RV32I::beq(inst_args args)
{
    if (args.regs->x[args.inst.B.rs1] == args.regs->x[args.inst.B.rs2]) {
        args.regs->pc += rv32_sext(args.inst.B.offset, 4);
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bne(inst_args args)
{
    if (args.regs->x[args.inst.B.rs1] != args.regs->x[args.inst.B.rs2]) {
        args.regs->pc += rv32_sext(args.inst.B.offset, 4);
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::blt(inst_args args)
{
    if ( *((int32_t*)&args.regs->x[args.inst.B.rs1]) < *((int32_t*)&args.regs->x[args.inst.B.rs2]) )
    {
        args.regs->pc += rv32_sext(args.inst.B.offset, 4);
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bge(inst_args args)
{
    if ( *((int32_t*)&args.regs->x[args.inst.B.rs1]) >= *((int32_t*)&args.regs->x[args.inst.B.rs2]) )
    {
        args.regs->pc += rv32_sext(args.inst.B.offset, 4);
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bltu(inst_args args)
{
    if ( args.regs->x[args.inst.B.rs1] < args.regs->x[args.inst.B.rs2] )
    {
        args.regs->pc += rv32_sext(args.inst.B.offset, 4);
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::bgeu(inst_args args)
{
    if ( args.regs->x[args.inst.B.rs1] >= args.regs->x[args.inst.B.rs2] )
    {
        args.regs->pc += rv32_sext(args.inst.B.offset, 4);
        args.ctrl->pc_changed = true;
    }
    return RV_EOK;
}

rv_err RV32I::jalr(inst_args args)
{
    uint32_t t = args.regs->pc+4;
    args.regs->pc = args.regs->x[args.inst.I.rs1] + rv32_sext(args.inst.I.imm_11_0, 11);
    args.regs->x[args.inst.I.rd] = t;
    args.ctrl->pc_changed = true;
    return RV_EOK;
}

rv_err RV32I::ecall(inst_args args)
{
    //TODO implement
    return RV_EOK;
}

rv_err RV32I::ebreak(inst_args args)
{
    //TODO implement
    return RV_EOK;
}

}
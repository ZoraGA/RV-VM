#include <gtest/gtest.h>
#include "ZoraGA/RV32.h"
#include "ZoraGA/RV32I.h"
#include "RV32Mem.h"
#include "RV32Test.h"

using namespace ZoraGA;

TEST(RV32I, Control) {
    rv32i_args a;
    RVVM::RV32::RV32I rv32i;
    RVVM::rv32_regs_base reg;
    RVVM::rv32_regs_ctrl ctrl;
    RVVM::rv32_regs regs;
    RVVM::rv32_inst_fmt inst;
    RVVM::rv32_mem_info info;
    RVVM::rv32_mem_infos mems;
    RVVM::rv32_imm_fmt imm;
    RV32Mem mem(64*1024);

    // RVVM::rvlog log;
    // log.set_log_level(RVVM::rvlog::LV_VERBOSE);
    // log.set_log_inst(true);
    // log.set_log_stdout(true);
    // rv32i.set_log(&log);

    info.addr = 0;
    info.len  = 64*1024;
    info.mem  = &mem;
    mems.push_back(info);

    regs.ctl = &ctrl;
    regs.reg = &reg;
    a.i      = &rv32i;
    a.inst   = &inst;
    a.regs   = &regs;
    a.mems   = &mems;

    reg.pc = 0;
    memset(reg.x, 0, sizeof(reg.x));

    /* beq */
    inst.inst       = 0;
    inst.opcode     = 0b1100011;
    inst.B.funct3   = 0b000;
    inst.B.rs1      = 1;
    inst.B.rs2      = 2;
    imm.B.imm       = 0xFFE;
    inst.B.imm_4_1  = imm.B.imm_4_1;
    inst.B.imm_10_5 = imm.B.imm_10_5;
    inst.B.imm_11   = imm.B.imm_11;
    inst.B.imm_12   = imm.B.imm_12;

    reg.pc = 0;
    reg.x[1] = 0x1;
    reg.x[2] = 0x1;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0xFFE);

    reg.pc = 0;
    reg.x[2] = 0x0;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0);

    /* bne */
    inst.inst       = 0;
    inst.opcode     = 0b1100011;
    inst.B.funct3   = 0b001;
    inst.B.rs1      = 1;
    inst.B.rs2      = 2;
    imm.B.imm       = 0xFFE;
    inst.B.imm_4_1  = imm.B.imm_4_1;
    inst.B.imm_10_5 = imm.B.imm_10_5;
    inst.B.imm_11   = imm.B.imm_11;
    inst.B.imm_12   = imm.B.imm_12;

    reg.pc = 0;
    reg.x[1] = 0x1;
    reg.x[2] = 0x0;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0xFFE);

    reg.pc = 0;
    reg.x[2] = 0x1;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0);


    /* bge */
    inst.inst       = 0;
    inst.opcode     = 0b1100011;
    inst.B.funct3   = 0b101;
    inst.B.rs1      = 1;
    inst.B.rs2      = 2;
    imm.B.imm       = 0xFFE;
    inst.B.imm_4_1  = imm.B.imm_4_1;
    inst.B.imm_10_5 = imm.B.imm_10_5;
    inst.B.imm_11   = imm.B.imm_11;
    inst.B.imm_12   = imm.B.imm_12;

    reg.pc = 0;
    reg.x[1] = 0xFFFFFFFF;
    reg.x[2] = 0xFFFFFFFE;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0xFFE);

    reg.pc = 0;
    reg.x[2] = 0xFFFFFFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0xFFE);

    reg.pc = 0;
    reg.x[1] = 0xFFFFFFFE;
    reg.x[2] = 0xFFFFFFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0);

    /* bgeu */
    inst.B.funct3 = 0b111;
    reg.pc = 0;
    reg.x[1] = 0xFFFFFFFF;
    reg.x[2] = 0xFFFFFFFE;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0xFFE);

    reg.pc = 0;
    reg.x[2] = 0xFFFFFFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0xFFE);

    reg.pc = 0;
    reg.x[1] = 0xFFFFFFFE;
    reg.x[2] = 0xFFFFFFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0);

    /* blt */
    inst.B.funct3 = 0b100;

    reg.pc = 0;
    reg.x[1] = 0xFFFFFFFF;
    reg.x[2] = 0;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0xFFE);

    reg.pc = 0;
    reg.x[1] = 0;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0);

    /* bltu */
    inst.B.funct3 = 0b110;
    reg.pc = 0;
    reg.x[1] = 0xFFFFFFFF;
    reg.x[2] = 0;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0);

    reg.pc = 0;
    reg.x[1] = 0;
    reg.x[2] = 0xFFFFFFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->pc, 0xFFE);

    /* jal */
    inst.inst        = 0;
    inst.opcode      = 0b1101111;
    inst.J.rd        = 0;
    imm.J.imm        = 0x1FFFFE;
    inst.J.imm_20    = imm.J.imm_20;
    inst.J.imm_19_12 = imm.J.imm_19_12;
    inst.J.imm_11    = imm.J.imm_11;
    inst.J.imm_10_1  = imm.J.imm_10_1;
    reg.pc = 4;
    rv32i_exec(a);
    EXPECT_EQ(reg.x[0], 0x8);
    EXPECT_EQ(reg.pc, 0x2);

    /* jalr */
    inst.inst     = 0;
    inst.opcode   = 0b1100111;
    inst.I.rd     = 0;
    inst.I.funct3 = 0b010;
    inst.I.rs1    = 1;
    inst.I.imm_11_0 = 0xFEC;
    reg.x[1] = 0x800;
    reg.pc = 0x400;
    rv32i_exec(a);
    EXPECT_EQ(reg.x[0], 0x404);
    EXPECT_EQ(reg.pc, 0x7EC);
}

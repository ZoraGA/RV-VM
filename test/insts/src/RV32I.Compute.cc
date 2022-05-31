#include <gtest/gtest.h>
#include "ZoraGA/RV32.h"
#include "ZoraGA/RV32I.h"
#include "RV32Mem.h"
#include "RV32Test.h"

using namespace ZoraGA;


TEST(RV32I, Compute) {
    rv32i_args a;
    RVVM::RV32::RV32I rv32i;
    RVVM::rv32_regs_base reg;
    RVVM::rv32_regs_ctrl ctrl;
    RVVM::rv32_regs regs;
    RVVM::rv32_inst_fmt inst;
    RVVM::rv32_mem_info info;
    RVVM::rv32_mem_infos mems;
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

    /* add */
    inst.inst     = 0;
    inst.opcode   = 0b0110011;
    inst.R.rd     = 0;
    inst.R.funct3 = 0b000;
    inst.R.rs1    = 1;
    inst.R.rs2    = 2;
    inst.R.funct7 = 0b0000000;
    reg.x[1] = 0xFFFFFFFF;
    reg.x[2] = 0x1;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0);

    /* addi */
    inst.inst       = 0;
    inst.opcode     = 0b0010011;
    inst.I.rd       = 0;
    inst.I.funct3   = 0b000;
    inst.I.rs1      = 1;
    inst.I.imm_11_0 = 0xFFF;
    reg.x[1] = 0xFFFFFFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFFFFFE);

    /* sub */
    inst.inst     = 0;
    inst.opcode   = 0b0110011;
    inst.R.rd     = 0;
    inst.R.funct3 = 0b000;
    inst.R.rs1    = 1;
    inst.R.rs2    = 2;
    inst.R.funct7 = 0b0100000;
    reg.x[1] = 1;
    reg.x[2] = 2;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFFFFFF);

    /* and */
    inst.inst     = 0;
    inst.opcode   = 0b0110011;
    inst.R.rd     = 0;
    inst.R.funct3 = 0b111;
    inst.R.rs1    = 1;
    inst.R.rs2    = 2;
    inst.R.funct7 = 0b0000000;
    reg.x[1] = 0xFFFFFFFF;
    reg.x[2] = 0x00000001;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0x00000001);

    /* andi */
    inst.inst       = 0;
    inst.opcode     = 0b0010011;
    inst.I.rd       = 0;
    inst.I.funct3   = 0b111;
    inst.I.rs1      = 1;
    inst.I.imm_11_0 = 0x800;
    reg.x[1] = 0xFFFFFFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFFF800);

    /* or */
    inst.inst     = 0;
    inst.opcode   = 0b0110011;
    inst.R.rd     = 0;
    inst.R.funct3 = 0b110;
    inst.R.rs1    = 1;
    inst.R.rs2    = 2;
    inst.R.funct7 = 0b0000000;
    reg.x[1] = 0xFFFF0000;
    reg.x[2] = 0x0000FFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFFFFFF);

    /* ori */
    inst.inst       = 0;
    inst.opcode     = 0b0010011;
    inst.R.rd       = 0;
    inst.R.funct3   = 0b110;
    inst.R.rs1      = 1;
    inst.R.rs2      = 0;
    inst.R.imm_11_0 = 0x40;
    reg.x[1] = 0x00000FFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0x00000FFF);

    /* xor */
    inst.inst = 0;
    inst.opcode = 0b0110011;
    inst.R.rd = 0;
    inst.R.funct3 = 0b100;
    inst.R.rs1 = 1;
    inst.R.rs2 = 2;
    inst.R.funct7 = 0b0000000;
    reg.x[1] = 0xFFFFFFFF;
    reg.x[2] = 0x00000001;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFFFFFE);

    /* xori */
    inst.inst     = 0;
    inst.opcode   = 0b0010011;
    inst.I.rd     = 0;
    inst.I.funct3 = 0b100;
    inst.I.rs1    = 1;
    inst.I.imm_11_0 = 0x800;
    reg.x[1] = 0x00000FFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFFF7FF);

    /* sll */
    inst.inst   = 0;
    inst.opcode = 0b0110011;
    inst.R.rd   = 0;
    inst.R.funct3 = 0b001;
    inst.R.rs1    = 1;
    inst.R.rs2    = 2;
    inst.R.funct7 = 0b0000000;
    reg.x[1] = 0xFFFFFFFF;
    reg.x[2] = 0x00000002;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFFFFFC);

    /* slli */
    inst.inst     = 0;
    inst.opcode   = 0b0010011;
    inst.I.rd     = 0;
    inst.I.funct3 = 0b001;
    inst.I.rs1    = 1;
    inst.I.shamt  = 16;
    inst.I.v      = 0;
    reg.x[1] = 0xFFFFFFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFF0000);

    /* sra */
    inst.inst     = 0;
    inst.opcode   = 0b0110011;
    inst.R.rd     = 0;
    inst.R.funct3 = 0b101;
    inst.R.rs1    = 1;
    inst.R.rs2    = 2;
    inst.R.funct7 = 0b0100000;
    reg.x[1] = 0xFFFFFFFF;
    reg.x[2] = 0xFFFFFFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFFFFFF);

    /* srai */
    inst.inst   = 0;
    inst.opcode = 0b0010011;
    inst.I.rd   = 0;
    inst.I.funct3 = 0b101;
    inst.I.rs1    = 1;
    inst.I.shamt  = 16;
    inst.I.v      = 0b010000;
    reg.x[1] = 0xFFFFFFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFFFFFF);

    /* srl */
    inst.inst     = 0;
    inst.opcode   = 0b0110011;
    inst.R.rd     = 0;
    inst.R.funct3 = 0b101;
    inst.R.rs1    = 1;
    inst.R.rs2    = 2;
    inst.R.funct7 = 0b0000000;
    reg.x[1] = 0xFFFFFFFF;
    reg.x[2] = 0x00000002;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0x3FFFFFFF);

    /* srli */
    inst.inst     = 0;
    inst.opcode   = 0b0010011;
    inst.I.rd     = 0;
    inst.I.funct3 = 0b101;
    inst.I.rs1    = 1;
    inst.I.shamt  = 16;
    inst.I.v      = 0;
    reg.x[1] = 0xFFFFFFFF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0x0000FFFF);

    /* lui */
    inst.inst        = 0;
    inst.opcode      = 0b0110111;
    inst.U.rd        = 0;
    inst.U.imm_31_12 = 0x80001;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0x80001000);

    /* auipc */
    inst.inst        = 0;
    inst.opcode      = 0b0010111;
    inst.U.rd        = 0;
    inst.U.imm_31_12 = 0x80001;
    reg.pc           = 0x4;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0x80001004);

    /* slt */
    inst.inst     = 0;
    inst.opcode   = 0b0110011;
    inst.R.rd     = 0;
    inst.R.funct3 = 0b010;
    inst.R.rs1    = 1;
    inst.R.rs2    = 2;
    inst.R.funct7 = 0b0000000;
    reg.x[1] = 1;
    reg.x[2] = 2;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 1);

    /* slti */
    inst.inst       = 0;
    inst.opcode     = 0b0010011;
    inst.I.rd       = 0;
    inst.I.funct3   = 0b010;
    inst.I.rs1      = 1;
    inst.I.imm_11_0 = 1;
    reg.x[1] = 1;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0);

    /* sltiu */
    inst.inst = 0;
    inst.inst       = 0;
    inst.opcode     = 0b0010011;
    inst.I.rd       = 0;
    inst.I.funct3   = 0b011;
    inst.I.rs1      = 1;
    inst.I.imm_11_0 = 0x800;
    reg.x[1] = 0xFFFFF7FF;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 1);
}

#include <gtest/gtest.h>
#include "ZoraGA/RV32.h"
#include "ZoraGA/RV32I.h"
#include "RV32Mem.h"
#include "RV32Test.h"

using namespace ZoraGA;

TEST(RV32I, LoadStore) {
    rv32i_args a;
    RVVM::RV32::RV32I rv32i;
    RVVM::rv32_regs_base reg;
    RVVM::rv32_regs_ctrl ctrl;
    RVVM::rv32_regs regs;
    RVVM::rv32_inst_fmt inst;
    RVVM::rv32_mem_info info;
    RVVM::rv32_mem_infos mems;
    RV32Mem mem(64*1024);
    std::vector<uint8_t> &ram = *mem.raw();

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

    /* lb */
    inst.inst       = 0;
    inst.opcode     = 0b0000011;
    inst.I.rd       = 0;
    inst.I.funct3   = 0b000;
    inst.I.rs1      = 1;
    inst.I.imm_11_0 = 0;
    ram[0] = 0x80;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFFFF80);

    /* lh */
    reg.x[0] = 0;
    inst.I.funct3 = 0b001;
    ram[0] = 0x00;
    ram[1] = 0x80;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0xFFFF8000);

    /* lw */
    reg.x[0] = 0;
    inst.I.funct3 = 0b010;
    ram[0] = 0x00;
    ram[1] = 0x00;
    ram[2] = 0x00;
    ram[3] = 0x80;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0x80000000);

    /* sb */
    inst.inst       = 0;
    inst.opcode     = 0b0100011;
    inst.S.imm_11_5 = 0;
    inst.S.imm_4_0  = 0;
    inst.S.rs1      = 1;
    inst.S.rs2      = 2;
    reg.x[1] = 0;
    reg.x[2] = 0x08;
    ram[0] = 0x00;
    rv32i_exec(a);
    EXPECT_EQ(ram[0], 0x08);

    /* sh */
    inst.S.funct3 = 0b001;
    reg.x[2] = 0x0800;
    ram[0] = 0x00;
    ram[1] = 0x00;
    rv32i_exec(a);
    EXPECT_EQ(ram[0], 0x00);
    EXPECT_EQ(ram[1], 0x08);

    /* sw */
    inst.S.funct3 = 0b010;
    reg.x[2] = 0xFFAA71BB;
    ram[0] = 0x00;
    ram[1] = 0x00;
    ram[2] = 0x00;
    ram[3] = 0x00;
    rv32i_exec(a);
    EXPECT_EQ(ram[0], 0xBB);
    EXPECT_EQ(ram[1], 0x71);
    EXPECT_EQ(ram[2], 0xAA);
    EXPECT_EQ(ram[3], 0xFF);

    /* lbu */
    inst.inst       = 0;
    inst.opcode     = 0b0000011;
    inst.I.rd       = 0;
    inst.I.funct3   = 0b100;
    inst.I.rs1      = 1;
    inst.I.imm_11_0 = 0;
    ram[0] = 0x80;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0x80);

    /* lhu */
    inst.I.funct3 = 0b101;
    ram[0] = 0x00;
    ram[1] = 0x80;
    reg.x[0] = 0;
    EXPECT_EQ(rv32i_exec(a)->regs->reg->x[0], 0x8000);
}

#ifndef __ZORAGA_RVVM_RV32INSTFMT_H__
#define __ZORAGA_RVVM_RV32INSTFMT_H__

#include <stdint.h>

namespace ZoraGA::RVVM
{

#pragma pack(push, 1)
typedef union rv32_inst_fmt
{
    uint32_t inst;
    union{
        struct{
            uint32_t opcode:7;
            uint32_t :25;
        };

        struct{
            uint32_t aa:2;
            uint32_t bbb:3;
            uint32_t cc:2;
            uint32_t :25;
        };

        union {
            struct {
                uint32_t :7;
                uint32_t rd: 5;
                uint32_t funct3: 3;
                uint32_t rs1: 5;
                uint32_t rs2: 5;
                uint32_t funct7: 7;
            };
            struct {
                uint32_t :25;
                uint32_t imm_11_0: 7;
            };
        } R;

        union {
            struct {
                uint32_t :7;
                uint32_t rd: 5;
                uint32_t funct3: 3;
                uint32_t rs1: 5;
                uint32_t imm_11_0: 12;
            };

            struct {
                uint32_t :20;
                uint32_t shamt:6;
                uint32_t v:6;
            };

            struct{
                uint32_t :15;
                uint32_t zimm_4_0:5;
                uint32_t :12;
            };
        } I;

        struct {
            uint32_t :7;
            uint32_t imm_4_0: 5;
            uint32_t funct3: 3;
            uint32_t rs1: 5;
            uint32_t rs2: 5;
            uint32_t imm_11_5: 7;
        } S;

        struct {
                uint32_t :7;
                uint32_t imm_11: 1;
                uint32_t imm_4_1: 4;
                uint32_t funct3: 3;
                uint32_t rs1: 5;
                uint32_t rs2: 5;
                uint32_t imm_10_5: 6;
                uint32_t imm_12:1;
        } B;

        struct {
            uint32_t :7;
            uint32_t rd: 5;
            uint32_t imm_31_12: 20;
        } U;

        struct {
            uint32_t :7;
            uint32_t rd: 5;
            uint32_t imm_19_12: 8;
            uint32_t imm_11: 1;
            uint32_t imm_10_1: 10;
            uint32_t imm_20: 1;
        } J;
    };
}rv32_inst_fmt;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union rv32_imm_fmt
{
    uint32_t u32;
    union {
        struct {
            uint32_t imm_11_0:12;
            uint32_t :20;
        };
        struct {
            uint32_t imm:12;
            uint32_t :20;
        };
    } I;

    union {
        struct {
            uint32_t imm_4_0:5;
            uint32_t imm_11_5:7;
            uint32_t :20;
        };

        struct {
            uint32_t imm:12;
            uint32_t :20;
        };
    } S;

    union {
        struct {
            uint32_t :1;
            uint32_t imm_4_1:4;
            uint32_t imm_10_5:6;
            uint32_t imm_11:1;
            uint32_t imm_12:1;
            uint32_t :19;
        };

        struct {
            uint32_t imm:13;
            uint32_t :19;
        };
    } B;

    union {
        struct {
            uint32_t :12;
            uint32_t imm_31_12:20;
        };
        struct {
            uint32_t imm;
        };
    } U;

    union {
        struct {
            uint32_t :1;
            uint32_t imm_10_1:10;
            uint32_t imm_11:1;
            uint32_t imm_19_12:8;
            uint32_t imm_20:1;
            uint32_t :11;
        };

        struct {
            uint32_t imm:21;
            uint32_t :11;
        };
    } J;
}rv32_imm_fmt;
#pragma pack(pop)

}

#endif

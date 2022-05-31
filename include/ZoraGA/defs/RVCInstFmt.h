#ifndef __ZORAGA_RVVM_RVCINSTFMT_H__
#define __ZORAGA_RVVM_RVCINSTFMT_H__

#include <stdint.h>

namespace ZoraGA::RVVM
{

#pragma pack(push, 1)
typedef union rvc_inst_fmt
{
    uint16_t inst;
    struct {
        uint16_t op:2;
        union {
            /* Register */
            struct {
                uint16_t rs2: 5;
                uint16_t rd_rs1: 5;
                uint16_t funct4: 4;
            } CR;

            /* Immediate */
            struct {
                uint16_t imm_lsb: 5;
                uint16_t rd_rs1: 5;
                uint16_t imm_msb: 1;
                uint16_t funct3: 3;
            } CI;

            /* Stack-relative Store */
            struct {
                uint16_t rs2: 5;
                uint16_t imm: 6;
                uint16_t funct3: 3;
            } CSS;

            /* Wide Immediate */
            struct {
                uint16_t rd: 3;
                uint16_t imm: 8;
                uint16_t funct3: 3;
            } CIW;

            /* Load */
            struct {
                uint16_t rd: 3;
                uint16_t imm_lsb: 2;
                uint16_t rs1: 3;
                uint16_t imm_msb: 3;
            } CL;

            /* Store */
            struct {
                uint16_t rs2: 3;
                uint16_t imm_lsb: 2;
                uint16_t rs1: 3;
                uint16_t imm_msb: 3;
            } CS;

            /* Arithmetic */
            struct {
                uint16_t rs2: 3;
                uint16_t funct2: 2;
                uint16_t rd_rs1: 3;
                uint16_t funct6: 6;
            } CA;

            /* Branch */
            struct {
                uint16_t off_lsb: 5;
                uint16_t rs1: 3;
                uint16_t off_msb: 3;
                uint16_t funct3: 3;
            } CB;

            /* Jump */
            struct {
                uint16_t target: 11;
                uint16_t funct3: 3;
            } CJ;
        };
    };
} rvc_inst_fmt;
#pragma pack(pop)

}

#endif // __ZORAGA_RVVM_RV32INSTFMT_H__
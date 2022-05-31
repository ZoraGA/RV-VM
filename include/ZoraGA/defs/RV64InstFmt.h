#ifndef __ZORAGA_RVVM_RV64INSTFMT_H__
#define __ZORAGA_RVVM_RV64INSTFMT_H__

#include <stdint.h>

namespace ZoraGA::RVVM
{

#pragma pack(push, 1)
typedef union rv64_inst_fmt
{
    uint64_t inst;
    union{
        struct{
            uint64_t opcode:7;
            uint64_t :57;
        };

        struct{
            uint64_t aa:2;
            uint64_t bbb:3;
            uint64_t cc:2;
            uint64_t :57;
        };

        //TODO implement
    };
}rv64_inst_fmt;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union rv64_imm_fmt
{
    uint64_t u64;
    //TODO implement
}rv64_imm_fmt;
#pragma pack(pop)

}

#endif
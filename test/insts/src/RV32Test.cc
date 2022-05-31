#include "ZoraGA/RV32.h"
#include "ZoraGA/RV32I.h"
#include "RV32Test.h"

rv32i_args *rv32i_exec(rv32i_args &a)
{
    do{

        if (a.i->isValid(*a.inst) != ZoraGA::RVVM::RV_EOK) {
            break;
        }
        if (a.i->exec(*a.inst, *a.regs, *a.mems) != ZoraGA::RVVM::RV_EOK) {
            printf("exec error\n");
            break;
        }
    }while(0);
    return &a;
}
#ifndef __ZORAGA_RVVM_CSRADDR_H__
#define __ZORAGA_RVVM_CSRADDR_H__

#include <stdint.h>
#include "ZoraGA/defs/RVCSRFmt.h"

namespace ZoraGA::RVVM
{

rv_csr_addr_fmt rv_csr_addr(uint16_t add);

typedef enum CSR_Unp
{
    /* Unprivileged Floating-Point CSRS */
    CSR_fflags = 0x001,
    CSR_frm,
    CSR_fcsr,

    /* Unprivileged Counter/Timers */
    CSR_cycle = 0xc00,
    CSR_time,
    CSR_instret,
    CSR_hpmcounter3,
    CSR_hpmcounter4,
    CSR_hpmcounter5,
    CSR_hpmcounter6,
    CSR_hpmcounter7,
    CSR_hpmcounter8,
    CSR_hpmcounter9,
    CSR_hpmcounter10,
    CSR_hpmcounter11,
    CSR_hpmcounter12,
    CSR_hpmcounter13,
    CSR_hpmcounter14,
    CSR_hpmcounter15,
    CSR_hpmcounter16,
    CSR_hpmcounter17,
    CSR_hpmcounter18,
    CSR_hpmcounter19,
    CSR_hpmcounter20,
    CSR_hpmcounter21,
    CSR_hpmcounter22,
    CSR_hpmcounter23,
    CSR_hpmcounter24,
    CSR_hpmcounter25,
    CSR_hpmcounter26,
    CSR_hpmcounter27,
    CSR_hpmcounter28,
    CSR_hpmcounter29,
    CSR_hpmcounter30,
    CSR_hpmcounter31,

    CSR_cycleh = 0xc80,
    CSR_timeh,
    // 3~31
    CSR_hpmcounterh3,
}CSR_Unp;

typedef enum CSR_S
{
    /* Supervisor Trap Setup */
    CSR_sstatus = 0x100,
    CSR_sie,
    CSR_stvec,
    CSR_scounteren,

    /* Supervisor Configuration */
    CSR_senvcfg = 0x10A,

    /* Supervisor Trap Handling */
    CSR_sscratch = 0x140,
    CSR_sepc,
    CSR_scause,
    CSR_stval,
    CSR_sip,

    /* Supervisor Protection and Translation */
    CSR_satp = 0x180,

    /* Debug/Trace Registers */
    CSR_scontext = 0x5A8,
}CSR_S;

/**
 * @brief CSR addr for Hypervisor and Virtual
 * 
 */
typedef enum CSR_HV
{
    /* Hypervisor Trap Setup */
    CSR_hstatus = 0x600,
    CSR_hedeleg = 0x602,
    CSR_hideleg,
    CSR_hie,
    CSR_hcounteren = 0x606,
    CSR_hgeie,

    /* Hypervisor Trap Handling */
    CSR_htval = 0x643,
    CSR_hip,
    CSR_hvip,
    CSR_htinst = 0x64A,
    CSR_hgeip  = 0xE12,

    /* Hypervisor Configuration */
    CSR_henvcfg  = 0x60A,
    CSR_henvcfgh = 0x61A,

    /* Hypervisor Protection and Translation */
    CSR_hgatp = 0x680,
    
    /* Debug/Trace registers */
    CSR_hcontext = 0x6A8,

    /* Hypervisor Counter/Timer Virtualization Registers */
    CSR_htimedelta  = 0x605,
    CSR_htimedeltah = 0x615,

    /* Virtual Supervisor Registers */
    CSR_vsstatus = 0x200,
    CSR_vsie     = 0x204,
    CSR_vstvec,
    CSR_vsscratch = 0x240,
    CSR_vsepc,
    CSR_vscause,
    CSR_vstval,
    CSR_vsip,
    CSR_vsatp = 0x280,
}CSR_HV;

/**
 * @brief CSR addr for Machine-Level
 * 
 */
typedef enum CSR_M
{
    /* Machine Info Registers */
    CSR_mvendorid = 0xf11,
    CSR_marchid,
    CSR_mimpid,
    CSR_mhartid,
    CSR_mconfigptr = 0xf15,

    /* Machine Trap Setup */

    CSR_mstatus = 0x300,
    CSR_misa,
    CSR_medeleg,
    CSR_mideleg,
    CSR_mie,
    CSR_mtvec,
    CSR_mcounteren,
    CSR_mstatush = 0x310,
    
    /* Machine Trap Handling */

    CSR_mscratch = 0x340,
    CSR_mepc,
    CSR_mcause,
    CSR_mtval,
    CSR_mip,
    CSR_mtinst = 0x34A,
    CSR_mtval2 = 0x34B,

    /* Machine Configuration */

    CSR_menvcfg  = 0x30A,
    CSR_menvcfgh = 0x31A,
    CSR_mseccfg  = 0x747,
    CSR_mseccfgh = 0x757,

    /* Machine Memory Protection */
    // 0~15
    CSR_pmpcfg0 = 0x3A0,
    CSR_pmpcfg1,
    CSR_pmpcfg2,
    CSR_pmpcfg3,
    CSR_pmpcfg4,
    CSR_pmpcfg5,
    CSR_pmpcfg6,
    CSR_pmpcfg7,
    CSR_pmpcfg8,
    CSR_pmpcfg9,
    CSR_pmpcfg10,
    CSR_pmpcfg11,
    CSR_pmpcfg12,
    CSR_pmpcfg13,
    CSR_pmpcfg14,
    CSR_pmpcfg15,
    // 0~63
    CSR_pmpaddr0 = 0x3B0,
    CSR_pmpaddr1,
    CSR_pmpaddr2,
    CSR_pmpaddr3,
    CSR_pmpaddr4,
    CSR_pmpaddr5,
    CSR_pmpaddr6,
    CSR_pmpaddr7,
    CSR_pmpaddr8,
    CSR_pmpaddr9,
    CSR_pmpaddr10,
    CSR_pmpaddr11,
    CSR_pmpaddr12,
    CSR_pmpaddr13,
    CSR_pmpaddr14,
    CSR_pmpaddr15,
    CSR_pmpaddr16,
    CSR_pmpaddr17,
    CSR_pmpaddr18,
    CSR_pmpaddr19,
    CSR_pmpaddr20,
    CSR_pmpaddr21,
    CSR_pmpaddr22,
    CSR_pmpaddr23,
    CSR_pmpaddr24,
    CSR_pmpaddr25,
    CSR_pmpaddr26,
    CSR_pmpaddr27,
    CSR_pmpaddr28,
    CSR_pmpaddr29,
    CSR_pmpaddr30,
    CSR_pmpaddr31,
    CSR_pmpaddr32,
    CSR_pmpaddr33,
    CSR_pmpaddr34,
    CSR_pmpaddr35,
    CSR_pmpaddr36,
    CSR_pmpaddr37,
    CSR_pmpaddr38,
    CSR_pmpaddr39,
    CSR_pmpaddr40,
    CSR_pmpaddr41,
    CSR_pmpaddr42,
    CSR_pmpaddr43,
    CSR_pmpaddr44,
    CSR_pmpaddr45,
    CSR_pmpaddr46,
    CSR_pmpaddr47,
    CSR_pmpaddr48,
    CSR_pmpaddr49,
    CSR_pmpaddr50,
    CSR_pmpaddr51,
    CSR_pmpaddr52,
    CSR_pmpaddr53,
    CSR_pmpaddr54,
    CSR_pmpaddr55,
    CSR_pmpaddr56,
    CSR_pmpaddr57,
    CSR_pmpaddr58,
    CSR_pmpaddr59,
    CSR_pmpaddr60,
    CSR_pmpaddr61,
    CSR_pmpaddr62,
    CSR_pmpaddr63,

    /* Machine Counter/Timers */

    CSR_mcycle   = 0xb00,
    CSR_minstret = 0xb02,
    CSR_mhpmcounter3,
    CSR_mhpmcounter4,
    CSR_mhpmcounter5,
    CSR_mhpmcounter6,
    CSR_mhpmcounter7,
    CSR_mhpmcounter8,
    CSR_mhpmcounter9,
    CSR_mhpmcounter10,
    CSR_mhpmcounter11,
    CSR_mhpmcounter12,
    CSR_mhpmcounter13,
    CSR_mhpmcounter14,
    CSR_mhpmcounter15,
    CSR_mhpmcounter16,
    CSR_mhpmcounter17,
    CSR_mhpmcounter18,
    CSR_mhpmcounter19,
    CSR_mhpmcounter20,
    CSR_mhpmcounter21,
    CSR_mhpmcounter22,
    CSR_mhpmcounter23,
    CSR_mhpmcounter24,
    CSR_mhpmcounter25,
    CSR_mhpmcounter26,
    CSR_mhpmcounter27,
    CSR_mhpmcounter28,
    CSR_mhpmcounter29,
    CSR_mhpmcounter30,
    CSR_mhpmcounter31,

    CSR_mcycleh   = 0xb80,
    CSR_minstreth = 0xb82,
    CSR_mhpmcounter3h,
    CSR_mhpmcounter4h,
    CSR_mhpmcounter5h,
    CSR_mhpmcounter6h,
    CSR_mhpmcounter7h,
    CSR_mhpmcounter8h,
    CSR_mhpmcounter9h,
    CSR_mhpmcounter10h,
    CSR_mhpmcounter11h,
    CSR_mhpmcounter12h,
    CSR_mhpmcounter13h,
    CSR_mhpmcounter14h,
    CSR_mhpmcounter15h,
    CSR_mhpmcounter16h,
    CSR_mhpmcounter17h,
    CSR_mhpmcounter18h,
    CSR_mhpmcounter19h,
    CSR_mhpmcounter20h,
    CSR_mhpmcounter21h,
    CSR_mhpmcounter22h,
    CSR_mhpmcounter23h,
    CSR_mhpmcounter24h,
    CSR_mhpmcounter25h,
    CSR_mhpmcounter26h,
    CSR_mhpmcounter27h,
    CSR_mhpmcounter28h,
    CSR_mhpmcounter29h,
    CSR_mhpmcounter30h,
    CSR_mhpmcounter31h,

    CSR_mcountinhibit = 0x320,
    CSR_mhpmevent3    = 0x323,
    CSR_mhpmevent4,
    CSR_mhpmevent5,
    CSR_mhpmevent6,
    CSR_mhpmevent7,
    CSR_mhpmevent8,
    CSR_mhpmevent9,
    CSR_mhpmevent10,
    CSR_mhpmevent11,
    CSR_mhpmevent12,
    CSR_mhpmevent13,
    CSR_mhpmevent14,
    CSR_mhpmevent15,
    CSR_mhpmevent16,
    CSR_mhpmevent17,
    CSR_mhpmevent18,
    CSR_mhpmevent19,
    CSR_mhpmevent20,
    CSR_mhpmevent21,
    CSR_mhpmevent22,
    CSR_mhpmevent23,
    CSR_mhpmevent24,
    CSR_mhpmevent25,
    CSR_mhpmevent26,
    CSR_mhpmevent27,
    CSR_mhpmevent28,
    CSR_mhpmevent29,
    CSR_mhpmevent30,
    CSR_mhpmevent31,

}CSR_M;

/**
 * @brief CSR addr for Debug
 * 
 */
typedef enum CSR_D
{
    /* Debug/Trace Registers(shared with Debug Mode) */
    CSR_tselect = 0x7A0,
    CSR_tdata1,
    CSR_tdata2,
    CSR_tdata3,
    CSR_mcontext = 0x7A8,

    /* Debug Mode Registers */
    CSR_dcsr = 0x7B0,
    CSR_dpc,
    CSR_scratch0,
    CSR_scratch1,
}CSR_D;

}

#endif // __ZORAGA_RVVM_CSRFMT_H__

#ifndef __ZORAGA_RVVM_CSRFMT_H__
#define __ZORAGA_RVVM_CSRFMT_H__

#include <stdint.h>

/** 
 * 
 * ╔═══════════════════════════════════╗ 
 * ║       Special csr register        ║
 * ╠═════════╦═══════╦═══════╦═════════╣
 * ║ [11:10] ║ [9:8] ║ [7:4] ║ Special ║
 * ╠═════════╬═══════╬═══════╬═════════╣
 * ║   10    ║   00  ║  XXXX ║  CRW    ║
 * ║   11    ║   00  ║  11XX ║  CRO    ║
 * ║   01    ║   01  ║  11XX ║  CRW    ║
 * ║   10    ║   01  ║  11XX ║  CRW    ║
 * ║   11    ║   01  ║  11XX ║  CRO    ║
 * ║   01    ║   10  ║  11XX ║  CRW    ║
 * ║   10    ║   10  ║  11XX ║  CRW    ║
 * ║   11    ║   10  ║  11XX ║  CRW    ║
 * ║   01    ║   11  ║  1010 ║  DRW    ║
 * ║   01    ║   11  ║  1011 ║  DBG    ║
 * ║   01    ║   11  ║  11XX ║  CRW    ║
 * ║   10    ║   11  ║  11XX ║  CRW    ║
 * ║   11    ║   11  ║  11XX ║  CRO    ║
 * ╚═════════╩═══════╩═══════╩═════════╝
 * 
 * CRW: Custom register with read write access
 * CRO: Custom register with read only access
 * DRW: Debug register with read write access
 * DBG: Debug mode only register
 */

namespace ZoraGA::RVVM
{

/**
 * @brief CSR Register Address Format
 * 
 */
#pragma pack(push, 1)
typedef union rv_csr_addr_fmt
{
    uint16_t addr;
    struct {
        uint16_t num:8;
        /* 0b00 User-Level, 0b01 Supervisor-Level, 0b11 Machine-Level */
        uint16_t lowp:2;
        /* 0b00/0b01/0b10 for read/write, 0b11 for read-only */
        uint16_t rwro:2;
    };
}rv_csr_addr_fmt;
#pragma pack(pop)

typedef uint32_t rv32_csr_misa_t;
typedef uint64_t rv64_csr_misa_t;

#pragma pack(push, 1)
typedef union rv_csr_mvendorid_t
{
    uint32_t v;
    struct{
        uint32_t offset:7;
        uint32_t bank:25;
    };
}rv_csr_mvendorid_t;
#pragma pack(pop)
typedef rv_csr_mvendorid_t rv32_csr_mvendorid_t;
typedef rv_csr_mvendorid_t rv64_csr_mvendorid_t;

typedef uint32_t rv32_csr_marchid_t;
typedef uint64_t rv64_csr_marchid_t;

typedef uint32_t rv32_csr_mimpid_t;
typedef uint64_t rv64_csr_mimpid_t;

typedef uint32_t rv32_csr_mhartid_t;
typedef uint64_t rv64_csr_mhartid_t;

#pragma pack(push, 1)
typedef union rv32_csr_mstatus_t
{
    uint32_t v;
    struct{
        uint32_t :1;
        /* Global interrupt enable for S-mode */
        uint32_t SIE:1;
        uint32_t :1;
        /* Global interrupt enable for M-mode */
        uint32_t MIE:1;
        uint32_t :1;
        /* Interrupt enable bits for S-mode before trap */
        uint32_t SPIE:1;
        /* U-mode memory access by Big-Endian(1) or Little-Endian(0) */
        uint32_t UBE:1;
        /* Previous IE value for M-mode */
        uint32_t MPIE:1;
        /* Previous privilege for S-mode */
        uint32_t SPP:1;
        /* Encodes vector Extension state */
        uint32_t VS:2;
        /* Previous privilege for M-mode */
        uint32_t MPP:2;
        /* Encodes floating-point unit state */
        uint32_t FS:2;
        /* Encodes user-mode extensions and associated state */
        uint32_t XS:2;
        /* 
            The MPRV (Modify PRiVilege) bit modifies the effective privilege mode, 
            i.e., the privilege level at which loads and stores execute. When MPRV=0, 
            loads and stores behave as normal, using the translation and protection 
            mechanisms of the current privilege mode. When MPRV=1, load and store memory 
            addresses are translated and protected, and endianness is applied, as though 
            the current privilege mode were set to MPP. Instruction address-translation 
            and protection are unaffected by the setting of MPRV. MPRV is read-only 0 
            if U-mode is not supported.
         */
        uint32_t MPRV:1;
        /* 
            The SUM (permit Supervisor User Memory access) bit modifies the privilege with which S-mode
            loads and stores access virtual memory. When SUM=0, S-mode memory accesses to pages that are
            accessible by U-mode (U=1 in Figure 4.18) will fault. When SUM=1, these accesses are permitted.
            SUM has no effect when page-based virtual memory is not in effect. Note that, while SUM is
            ordinarily ignored when not executing in S-mode, it is in effect when MPRV=1 and MPP=S. SUM
            is read-only 0 if S-mode is not supported or if satp.MODE is read-only 0.
         */
        uint32_t SUM:1;
        /* 
            The MXR (Make eXecutable Readable) bit modifies the privilege with which loads access virtual
            memory. When MXR=0, only loads from pages marked readable (R=1 in Figure 4.18) will succeed.
            When MXR=1, loads from pages marked either readable or executable (R=1 or X=1) will succeed.
            MXR has no effect when page-based virtual memory is not in effect. MXR is read-only 0 if S-mode
            is not supported.
         */
        uint32_t MXR:1;
        /* 
            The TVM (Trap Virtual Memory) bit is a WARL field that supports intercepting supervisor
            virtual-memory management operations. When TVM=1, attempts to read or write the satp CSR
            or execute an SFENCE.VMA or SINVAL.VMA instruction while executing in S-mode will raise an
            illegal instruction exception. When TVM=0, these operations are permitted in S-mode. TVM is
            read-only 0 when S-mode is not supported.
         */
        uint32_t TVM:1;
        /* 
            The TW (Timeout Wait) bit is a WARL field that supports intercepting the WFI instruction (see
            Section 3.3.3). When TW=0, the WFI instruction may execute in lower privilege modes when
            not prevented for some other reason. When TW=1, then if WFI is executed in any less-privileged
            mode, and it does not complete within an implementation-specific, bounded time limit, the WFI
            instruction causes an illegal instruction exception. The time limit may always be 0, in which case
            WFI always causes an illegal instruction exception in less-privileged modes when TW=1. TW is
            read-only 0 when there are no modes less privileged than M.
         */
        uint32_t TW:1;
        /* 
            The TSR (Trap SRET) bit is a WARL field that supports intercepting the supervisor exception
            return instruction, SRET. When TSR=1, attempts to execute SRET while executing in S-mode
            will raise an illegal instruction exception. When TSR=0, this operation is permitted in S-mode.
            TSR is read-only 0 when S-mode is not supported.
         */
        uint32_t TSR:1;
        uint32_t :8;
        /* 
            The SD bit is a read-only bit that summarizes whether either the FS, VS, or XS fields signal the
            presence of some dirty state that will require saving extended user context to memory. If FS, XS,
            and VS are all read-only zero, then SD is also always zero.
         */
        uint32_t SD:1;
    };
}rv32_csr_mstatus_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union rv32_csr_mstatush_t
{
    uint32_t v;
    struct{
        uint32_t :4;
        /* S-mode memory access by Big-Endian(1) or Little-Endian(0) */
        uint32_t SBE:1;
        /* M-mode memory access by Big-Endian(1) or Little-Endian(0) */
        uint32_t MBE:1;
        uint32_t :26;
    };
}rv32_csr_mstatush_t;
#pragma pack(pop)


typedef union rv32_csr_mtvec_t
{
    uint32_t v;
    struct{
        uint32_t mode:2;
        uint32_t base_31_2:30;
    };
}rv32_csr_mtvec_t;

typedef union rv64_csr_mtvec_t
{
    uint64_t v;
    struct{
        uint64_t mode:2;
        uint64_t base_63_2:62;
    };
}rv64_csr_mtvec_t;

typedef uint32_t rv32_mideleg_t;
typedef uint64_t rv64_mideleg_t;

}

#endif
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

}

#endif
#ifndef __ZORAGA_RVVM_RVDEFS_H__
#define __ZORAGA_RVVM_RVDEFS_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <thread>
#include <functional>
#include "ZoraGA/RVLog.h"
#include "ZoraGA/RVEvent.h"
#include "ZoraGA/defs/RV32InstFmt.h"
#include "ZoraGA/defs/RV64InstFmt.h"
#include "ZoraGA/defs/RVCInstFmt.h"

namespace ZoraGA::RVVM
{

typedef enum rv_err
{
    RV_EOK = 0,
    RV_ERANGE,      // out of range
    RV_ECACHE,      // Cache error
    RV_EMISSING,    // Missing instruction or cache
    RV_ESYNC,       // Sync error
    RV_EFETCH,      // fetch error
    RV_EIALIGN,     // Instruction alignment error
    RV_EDALIGN,     // Data alignment error
    RV_EUNDEF,      // Undefined instruction
    RV_EININST,     // Invalid instruction
    RV_EACCESS,     // Access error
    RV_EFAULT,
}rv_err;

/**
 * @brief Base register template
 * 
 * @tparam T uint32_t or uint64_t, for RV32 or RV64
 * @tparam N usually 32
 */
template<typename T, size_t N>
struct regs
{
    regs(){
        memset(x, 0, sizeof(x));
        pc = 0;
    }
    T x[N];
    T pc;
};

typedef struct regs<uint32_t, 32> rv32_regs_base;
typedef struct regs<uint64_t, 32> rv64_regs_base;

template<typename T, size_t N>
struct rv_regs_ctrl
{
    rv_regs_ctrl() {
        memset(csrs, 0, sizeof(csrs));
        pc_changed = false;
    }
    T csrs[N];
    bool pc_changed;
};

typedef struct rv_regs_ctrl<uint32_t, 32> rv32_regs_ctrl;
typedef struct rv_regs_ctrl<uint64_t, 32> rv64_regs_ctrl;

typedef struct rv_regs_fp
{
    rv_regs_fp() {
        for (size_t i=0; i<32; i++)
            u[i] = 0;
        fcsr.u32 = 0;
    }
    union {
        uint64_t u[32];
        struct {
            float fp[32];
            uint32_t rsv;
        } s;

        struct {
            double fp[32];
        } d;
    };
    
    union {
        uint32_t u32;
        struct {
            uint32_t nx:1;
            uint32_t uf:1;
            uint32_t of:1;
            uint32_t dz:1;
            uint32_t nv:1;
            uint32_t frm:3;
            uint32_t :24;
        };
    } fcsr;
}rv_regs_fp;

typedef rv_regs_fp rv32_regs_fp;
typedef rv_regs_fp rv64_regs_fp;

template<typename TB, typename TF, typename TC>
struct rv_regs
{
    rv_regs(): reg(nullptr), fp(nullptr), ctl(nullptr) {};
    TB *reg;
    TF *fp;
    TC *ctl;
};

typedef struct rv_regs<rv32_regs_base, rv32_regs_fp, rv32_regs_ctrl> rv32_regs;
typedef struct rv_regs<rv64_regs_base, rv64_regs_fp, rv64_regs_ctrl> rv64_regs;

/**
 * @brief Memory interface
 * 
 * @tparam T uint32_t or uint64_t, for RV32 or RV64
 */
template<typename T>
class mem
{
    public:
        virtual rv_err read(T addr, void *data, T len)  = 0;
        virtual rv_err write(T addr, void *data, T len) = 0;
};

typedef mem<uint32_t> rv32_mem;
typedef mem<uint64_t> rv64_mem;

/**
 * @brief Memory information
 * 
 * @tparam T uint32_t or uint64_t, for RV32 or RV64
 * @tparam M rv32_mem or rv64_mem
 */
template<typename T, typename M>
struct mem_info
{
    T addr;
    T len;
    M *mem;
};

typedef struct mem_info<uint32_t, rv32_mem> rv32_mem_info;
typedef struct mem_info<uint64_t, rv64_mem> rv64_mem_info;

typedef std::vector<rv32_mem_info> rv32_mem_infos;
typedef std::vector<rv64_mem_info> rv64_mem_infos;

template<typename T, typename TM>
bool mem_is_range(T addr, TM info) {
    return (addr >= info.addr) && (addr < (info.addr + info.len));
}

template<typename T, typename TM, typename TI>
rv_err mem_read(T addr, void *p, T len, TI &info) {
    rv_err err = RV_EFAULT;
    for (auto it:info) {
        if ( mem_is_range<T, TM>(addr, it) ) {
            err = it.mem->read(addr - it.addr, p, len);
            break;
        }
    }
    return err;
}

template<typename T, typename TM, typename TI>
rv_err mem_write(T addr, void *p, T len, TI &info) {
    rv_err err = RV_EFAULT;
    for (auto it:info) {
        if ( mem_is_range<T, TM>(addr, it) ) {
            err = it.mem->write(addr - it.addr, p, len);
            break;
        }
    }
    return err;
}

#define rv32_mem_range(addr, info) mem_is_range<uint32_t, rv32_mem_info>(addr, info)
#define rv64_mem_range(addr, info) mem_is_range<uint64_t, rv64_mem_info>(addr, info)

#define rv32_mem_read(addr, p, len, infos) mem_read<uint32_t, rv32_mem_info, rv32_mem_infos>(addr, p, len, infos)
#define rv64_mem_read(addr, p, len, infos) mem_read<uint64_t, rv64_mem_info, rv64_mem_infos>(addr, p, len, infos)

#define rv32_mem_write(addr, p, len, infos) mem_write<uint32_t, rv32_mem_info, rv32_mem_infos>(addr, p, len, infos)
#define rv64_mem_write(addr, p, len, infos) mem_write<uint64_t, rv64_mem_info, rv64_mem_infos>(addr, p, len, infos)

/**
 * @brief Cache interface template
 * 
 * @tparam T 
 * @tparam MT 
 */
template<typename T, typename MT>
class cache
{
    public:
        virtual rv_err read(T addr, void *data, size_t len, MT &mems)  = 0;
        virtual rv_err write(T addr, void *data, size_t len, MT &mems) = 0;
};

typedef cache<uint32_t, rv32_mem_infos> rv32_cache;
typedef cache<uint64_t, rv64_mem_infos> rv64_cache;


/**
 * @brief C instruction
 * 
 * @tparam H Half instruction
 * @tparam F Full instruction
 */
template<typename H, typename O>
class comprs
{
    public:
        /**
         * @brief Check if the instruction is compressed, and decompress it
         * 
         * @param half Half of the instruction
         * @param out Decompressed instruction output, if the instruction is compressed
         * @return rv_err 
         */
        virtual rv_err isCompress(H half, O &out) = 0;
};

typedef comprs<uint16_t, rv32_inst_fmt> rv32_comprs;
typedef comprs<uint32_t, uint64_t> rv64_comprs;

/**
 * @brief Instruction template
 * 
 * @tparam T uint32_t or uint64_t, for RV32 or RV64
 * @tparam RT Base register type, rv32_regs_base or rv64_regs_base
 * @tparam MT Memory vector reference, rv32_mem_infos or rv64_mem_infos
 */
template<typename T, typename RT, typename MT>
class insts
{
    public:
        /**
         * @brief Is valid instruction
         * 
         * @param inst 
         * @return rv_err RV_EOK if valid, RV_EUNDEF if invalid
         */
        virtual rv_err isValid(T inst)   = 0;
        
        /**
         * @brief Execute instruction
         * 
         * @param inst Instruction data
         * @param regs Register reference
         * @param mems Memory reference
         * @return rv_err 
         */
        virtual rv_err exec(T inst, RT &regs, MT &mems) = 0;

        /**
         * @brief Set the logger
         * 
         * @param log 
         * @return rv_err 
         */
        virtual rv_err set_log(rvlog *log) = 0;
};

typedef insts<rv32_inst_fmt, rv32_regs, rv32_mem_infos> rv32_insts;
typedef insts<rv64_inst_fmt, rv32_regs, rv64_mem_infos> rv64_insts;

typedef std::map<std::string, rv32_insts*> rv32_insts_map;
typedef std::map<std::string, rv64_insts*> rv64_insts_map;

template<typename T, typename S>
S sext(T v, uint8_t bit)
{
    union {
        T ui;
        S si;
    }ret;
    ret.ui = 0;
    if (v & (1<<bit)) {
        ret.ui |= ((~((T)0)) << bit);
    }
    ret.ui |= v;
    return ret.si;
}

#define rv32_sext(v, b) sext<uint32_t, int32_t>(v, b)
#define rv64_sext(v, b) sext<uint64_t, int64_t>(v, b)

}

#endif
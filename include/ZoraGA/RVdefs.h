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
#include <condition_variable>

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

/**
 * @brief Float register template
 * 
 * @tparam T uint32_t or uint64_t, for RV32 or RV64
 * @tparam N usually 32
 */
template<typename T, size_t N>
struct regs_fp
{
    regs_fp() {
        for (size_t i=0; i<N; i++)
            f[i] = 0;
        fcsr = 0;
    }
    T f[N];
    T fcsr;
};

typedef struct regs_fp<uint32_t, 32> regs_sfp;
typedef struct regs_fp<uint64_t, 32> regs_dfp;

typedef regs_sfp rv32_regs_sfp;
typedef regs_sfp rv64_regs_sfp;
typedef regs_dfp rv32_regs_dfp;
typedef regs_dfp rv64_regs_dfp;

/**
 * @brief Memory interface
 * 
 * @tparam T uint32_t or uint64_t, for RV32 or RV64
 */
template<typename T>
class mem
{
    public:
        virtual rv_err read(T addr, void *data, size_t len)  = 0;
        virtual rv_err write(T addr, void *data, size_t len) = 0;
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
    uint64_t len;
    M *mem;
};

typedef struct mem_info<uint32_t, rv32_mem> rv32_mem_info;
typedef struct mem_info<uint64_t, rv64_mem> rv64_mem_info;

typedef std::vector<rv32_mem_info> rv32_mem_infos;
typedef std::vector<rv64_mem_info> rv64_mem_infos;

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

typedef comprs<uint16_t, uint32_t> rv32_comprs;
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
         * @param size 
         * @return rv_err RV_EOK if valid, RV_EUNDEF if invalid
         */
        virtual rv_err isValid(T inst, size_t size)   = 0;
        
        /**
         * @brief Execute instruction
         * 
         * @param inst Instruction data
         * @param size Instruction size, 2, 4 or 8
         * @param regs Register reference
         * @param mems Memory reference
         * @return rv_err 
         */
        virtual rv_err exec(T inst, size_t size, RT &regs, MT &mems) = 0;
};

typedef insts<uint32_t, rv32_regs_base, rv32_mem_infos> rv32_insts;
typedef insts<uint64_t, rv64_regs_base, rv64_mem_infos> rv64_insts;

typedef std::map<std::string, rv32_insts*> rv32_insts_map;
typedef std::map<std::string, rv64_insts*> rv64_insts_map;

}

#endif
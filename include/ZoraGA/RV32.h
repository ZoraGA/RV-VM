#ifndef __ZORAGA_RVVM_RV32_H__
#define __ZORAGA_RVVM_RV32_H__

#include "ZoraGA/RVdefs.h"
#include "ZoraGA/defs/RVCSRFmt.h"
#include "ZoraGA/defs/RVCSRAddr.h"

namespace ZoraGA::RVVM::RV32
{

class rv32
{
    public:
        rv32();
        ~rv32();
        /**
         * @brief Add a instruction to the instruction set
         * 
         * @param name The name of the instruction, like "I" or "M" or "C" or others
         * @param inst The instruction class pointer
         * @return true If the instruction is added successfully
         * @return false If the instruction is already added
         */
        bool add_inst(std::string name, rv32_inst *inst);

        /**
         * @brief Add memory to the memory set
         * 
         * @param addr Start address
         * @param length Length of the memory
         * @param mem 
         * @return true 
         * @return false 
         */
        bool add_mem(uint32_t addr, uint32_t length, rv32_mem *mem);

        /**
         * @brief Set logger
         * 
         * @param log 
         * @return true 
         * @return false 
         */
        bool set_log(rvlog *log);

        /**
         * @brief Set the start addr of PC register
         * 
         * @param addr 
         * @return true 
         * @return false 
         */
        bool set_start_addr(uint32_t addr);

        /**
         * @brief Set the compress mode
         * 
         * @param compr 
         * @return true 
         * @return false 
         */
        bool set_compress(rv32_comprs *compr);

        /**
         * @brief Start VM
         * 
         * @return true 
         * @return false 
         */
        bool start();

        /**
         * @brief Stop VM
         * 
         * @return true 
         * @return false 
         */
        bool stop();

        /**
         * @brief Wait for VM start
         * 
         * @param timeout_ms 
         * @return true 
         * @return false 
         */
        bool wait_for_start(uint32_t timeout_ms = 0);

        /**
         * @brief Wait for VM stop
         * 
         * @param timeout_ms 
         * @return true 
         * @return false 
         */
        bool wait_for_stop(uint32_t timeout_ms = 0);
    
    private:
        void run();
        // bool mem_isRange(rv32_mem_info &info, uint32_t addr);
        bool inst_fetch(uint32_t addr, rv32_inst_fmt &out, bool &is_compress);
        bool inst_exec(rv32_inst_fmt inst);
        void regs_dump();

    private:
    	// rv32_ctrl      m_ctrl;
        // rv32_regs_base m_regs;
        rv32_regs      m_regs;
        rv32_insts_map m_insts;
        rv32_mem_infos m_mems;
        rv32_event     m_event;
        rv32_comprs   *m_comprs = nullptr;
        rvlog         *m_log = nullptr;

        std::mutex m_mutex;
        std::thread *m_thread = nullptr;

        bool m_running         = false;
        bool m_started         = false;
        bool m_exit_req        = false;
};

}

#endif // __ZORAGA_RVVM_RV32_H__

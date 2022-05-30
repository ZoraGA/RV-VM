#ifndef __RVVM_LOADER_MEM_ROM_H__
#define __RVVM_LOADER_MEM_ROM_H__

#include "ZoraGA/RVdefs.h"
#include <fstream>

class mem_rom:public ZoraGA::RVVM::rv32_mem
{
    public:
        mem_rom();
        ~mem_rom();

        bool load(std::string file);

        ZoraGA::RVVM::rv_err read(uint32_t addr, void *p, uint32_t len);
        ZoraGA::RVVM::rv_err write(uint32_t addr, void *p, uint32_t len);
    private:
        std::mutex m_mutex;
        std::ifstream m_file;
        size_t m_size;
};

#endif
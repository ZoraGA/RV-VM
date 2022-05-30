#ifndef __RVVM_LOADER_MEM_RAM_H__
#define __RVVM_LOADER_MEM_RAM_H__

#include "ZoraGA/RVdefs.h"
#include <vector>

class mem_ram:public ZoraGA::RVVM::rv32_mem
{
    public:
        mem_ram();
        ~mem_ram();

        bool set_size(size_t sz);

        ZoraGA::RVVM::rv_err read(uint32_t addr, void *p, uint32_t len);
        ZoraGA::RVVM::rv_err write(uint32_t addr, void *p, uint32_t len);

    private:
        std::vector<uint8_t> m_mem;
};

#endif
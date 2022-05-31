#include "ZoraGA/RVdefs.h"
#include <vector>

class RV32Mem:public ZoraGA::RVVM::rv32_mem
{
    public:
        RV32Mem(size_t size);
        ~RV32Mem();
        void reset();
        std::vector<uint8_t> *raw();
        ZoraGA::RVVM::rv_err read(uint32_t addr, void *data, uint32_t len);
        ZoraGA::RVVM::rv_err write(uint32_t addr, void *data, uint32_t len);

    private:
        std::vector<uint8_t> m_mem;
};

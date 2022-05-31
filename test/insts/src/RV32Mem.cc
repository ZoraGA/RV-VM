#include "RV32Mem.h"

using namespace ZoraGA::RVVM;

RV32Mem::RV32Mem(size_t size)
{
    m_mem.resize(size);
}

RV32Mem::~RV32Mem()
{}

std::vector<uint8_t> *RV32Mem::raw()
{
    return &m_mem;
}

void RV32Mem::reset()
{
    memset(&m_mem[0], 0, m_mem.size());
}

rv_err RV32Mem::read(uint32_t addr, void *p, uint32_t sz)
{
    if (addr + sz > m_mem.size()) return RV_ERANGE;
    memcpy(p, &m_mem[addr], sz);
    return RV_EOK;
}

rv_err RV32Mem::write(uint32_t addr, void *p, uint32_t sz)
{
    if (addr + sz > m_mem.size()) return RV_ERANGE;
    memcpy(&m_mem[addr], p, sz);
    return RV_EOK;
}

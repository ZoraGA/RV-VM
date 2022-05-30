#include "mem_ram.h"

using namespace ZoraGA::RVVM;

mem_ram::mem_ram()
{}

mem_ram::~mem_ram()
{}

bool mem_ram::set_size(size_t sz)
{
    m_mem.resize(sz);
    return true;
}

rv_err mem_ram::read(uint32_t addr, void *p, uint32_t len)
{
    if (addr + len > m_mem.size())
        return RV_ERANGE;
    memcpy(p, &m_mem[addr], len);
    return RV_EOK;
}

rv_err mem_ram::write(uint32_t addr, void *p, uint32_t len)
{
    if (addr + len > m_mem.size())
        return RV_ERANGE;
    memcpy(&m_mem[addr], p, len);
    return RV_EOK;
}

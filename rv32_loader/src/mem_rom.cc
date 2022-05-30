#include "mem_rom.h"
#include <filesystem>

#define stdfs std::filesystem

using namespace ZoraGA::RVVM;

mem_rom::mem_rom()
{}

mem_rom::~mem_rom()
{
    if (m_file.is_open()) m_file.close();
}

bool mem_rom::load(std::string file)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!stdfs::exists(stdfs::path(file))) return false;
    if (m_file.is_open()) m_file.close();
    m_file.open(file, std::ios::binary);
    bool ret = m_file.is_open();
    m_file.seekg(0, std::ifstream::end);
    m_size = m_file.tellg();
    m_file.seekg(0, std::ifstream::beg);
    return ret;
}

rv_err mem_rom::read(uint32_t addr, void *p, uint32_t len)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // if (addr + len > m_size) return RV_ERANGE;
    if (addr + len > m_size) {
        memset(p, 0, len);
        return RV_EOK;
    }
    m_file.seekg(addr, std::ifstream::beg);
    m_file.read(reinterpret_cast<char*>(p), len);
    return RV_EOK;
}

rv_err mem_rom::write(uint32_t addr, void *p, uint32_t len)
{
    return RV_EACCESS;
}

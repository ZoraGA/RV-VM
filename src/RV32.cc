#include "ZoraGA/RV32.h"

namespace ZoraGA::RVVM::RV32
{

rv32::rv32()
{}

rv32::~rv32()
{
    stop();
}

bool rv32::add_inst(std::string name, rv32_insts *inst)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    bool ret = false;
    do{
        if (m_started) break;
        if (m_insts.find(name) != m_insts.end()) break;
        m_insts[name] = inst;
        ret = true;
    }while(0);
    return ret;
}

bool rv32::add_mem(uint32_t addr, uint32_t length, rv32_mem *mem)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    bool ret = false;
    bool err = false;
    do{
        if (m_started) break;
        for (auto it:m_mems) {
            if (it.addr == addr) {
                err = true;
                break;
            }

            if (addr > it.addr || addr < it.addr + it.len) {
                err = true;
                break;
            }

            if (addr + length > it.addr || addr + length < it.addr + it.len) {
                err = true;
                break;
            }
        }
        if (err) break;
        m_mems.push_back(rv32_mem_info{addr, length, mem});
        ret = true;
    }while(0);
    return ret;
}

bool rv32::set_start_addr(uint32_t addr)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    bool ret = false;
    do{
        if (m_started) break;
        m_regs.pc = addr;
        ret = true;
    }while(0);
    return ret;
}

bool rv32::set_compress(rv32_comprs *comprs)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    bool ret = false;
    do{
        if (m_started) break;
        if (m_comprs) break;
        m_comprs = comprs;
        ret = true;
    }while(0);
    return ret;
}

bool rv32::start()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    bool ret = false;
    do{
        if (m_started || m_thread) break;

        m_exit_req = false;

        auto fn = std::bind(&rv32::run, this);
        m_thread = new std::thread(fn);
        if (m_thread == nullptr) break;

        m_thread->detach();

        ret       = true;
        m_started = true;
    }while(0);
    return ret;
}

bool rv32::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    bool ret = false;
    do{
        if (!m_started || m_thread == nullptr) break;
        m_exit_req = true;
        while(m_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        delete m_thread;
        m_thread  = nullptr;
        m_started = false;
        ret       = true;
    }while(0);
    return ret;
}

void rv32::run()
{
    uint32_t pc_prv = 0;
    rv32_inst_fmt inst;
    rv_err err;
    rv32_mem_info mem_info;
    bool is_compress = false;

    m_running = true;
    while(1) {
        if (m_exit_req) break;

        if (!inst_fetch(m_regs.pc, inst, is_compress))
        {
            printf("inst fetch error\n");
            break;
        }
        pc_prv = m_regs.pc;

        m_ctrl.pc_changed = false;
        if (!inst_exec(inst))
        {
            printf("inst exec error\n");
            break;
        }

        if (m_regs.pc != pc_prv || m_ctrl.pc_changed) {
            printf("pc changed\n");
        } else {
            m_regs.pc += is_compress ? 2 : 4;
        }
    }
    m_running = false;
}

bool rv32::inst_fetch(uint32_t addr, rv32_inst_fmt &out, bool &is_compress)
{
    bool ret = false;
    bool found = false;
    rv_err err;
    rv32_mem_info info;
    union {
        uint8_t u8[4];
        uint16_t u16[2];
        uint32_t u32;
    } inst;
    do{
        /* find mem for fetch */
        for (auto it:m_mems) {
            if (rv32_mem_range(addr, it))
            {
                info = it;
                found = true;
                break;
            }
        }
        if (!found) break;

        /* fetch */
        if (m_comprs) {

            /* read fist 16bit for check compress */
            err = info.mem->read(addr, inst.u8, 2);
            if (err != RV_EOK) {
                printf("instruction fetch err: %d\n", err);
                break;
            }

            /* if is compress instruction, break while */
            if (m_comprs->isCompress(inst.u16[0], out)) {
                is_compress = true;
                ret = true;
                break;
            }

            /* read last 16bit if not compress */
            err = info.mem->read(addr, inst.u8+2, 2);
            if (err != RV_EOK) {
                printf("instruction fetch err: %d\n", err);
                break;
            }
            is_compress = false;
            out.inst    = inst.u32;
            ret         = true;
        } else {
            err = info.mem->read(addr, inst.u8, 4);
            if (err != RV_EOK) {
                printf("instruction fetch err: %d\n", err);
                break;
            }
            ret = true;
            out.inst = inst.u32;
        }
    }while(0);
    return ret;
}

bool rv32::inst_exec(rv32_inst_fmt inst)
{
    rv_err err;
    bool ret = false;
    do{
        for (auto it:m_insts) {
            if (!it.second->isValid(inst)) continue;
            err = it.second->exec(inst, m_regs, m_mems, m_ctrl);
            ret = (err == RV_EOK);
            break;
        }
    }while(0);
    return ret;
}

}
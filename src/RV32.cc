#include "ZoraGA/RV32.h"

#define LOGI(fmt, ...) if (m_log) m_log->I(fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) if (m_log) m_log->E(fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) if (m_log) m_log->W(fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) if (m_log) m_log->D(fmt, ##__VA_ARGS__)
#define LOGV(fmt, ...) if (m_log) m_log->V(fmt, ##__VA_ARGS__)
#define LOGINST(fmt, ...) if (m_log) m_log->inst(fmt, ##__VA_ARGS__)
#define LOGREGS(fmt, ...) if (m_log) m_log->regs(fmt, ##__VA_ARGS__)

#define RV32_EVT_START (1UL << 0)
#define RV32_EVT_STOP  (1UL << 1)

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

            if (addr > it.addr && addr < it.addr + it.len) {
                err = true;
                break;
            }

            if (((addr + length) > it.addr) && ((addr + length) < (it.addr + it.len))) {
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

bool rv32::set_log(rvlog *log)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    bool ret = false;
    do{
        if (m_started || m_log) break;
        if (log == nullptr) break;
        m_log = log;
        for (auto it:m_insts) {
            it.second->set_log(m_log);
        }
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

bool rv32::wait_for_start(uint32_t toms)
{
    return m_event.wait(RV32_EVT_START, toms);
}

bool rv32::wait_for_stop(uint32_t toms)
{
    return m_event.wait(RV32_EVT_STOP, toms);
}

void rv32::run()
{
    uint32_t pc_prv = 0;
    rv32_inst_fmt inst;
    bool is_compress = false;

    m_running = true;
    m_event.set(RV32_EVT_START);
    while(1) {
        if (m_exit_req) break;

        if (!inst_fetch(m_regs.pc, inst, is_compress))
        {
            LOGE("inst fetch err");
            break;
        }
        pc_prv = m_regs.pc;
        LOGD("PC %08x, fetch instruction: %08x, opcode: %02x, aa: %01x, bbb: %01x, cc: %01x", pc_prv, inst.inst, inst.opcode, inst.aa, inst.bbb, inst.cc);

        if (inst.inst == 0) {
            LOGE("illegal instruction");
            break;
        }

        m_ctrl.pc_changed = false;
        if (!inst_exec(inst))
        {
            LOGE("inst exec err");
            break;
        }
        regs_dump();

        if (m_regs.pc != pc_prv || m_ctrl.pc_changed) {
            LOGD("pc changed");
        } else {
            m_regs.pc += is_compress ? 2 : 4;
        }
    }
    m_running = false;
    m_event.set(RV32_EVT_STOP);
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
            err = info.mem->read(addr - info.addr, inst.u8, 2);
            if (err != RV_EOK) {
                LOGE("instruction fetch err: %d", err);
                break;
            }

            /* if is compress instruction, break while */
            if (m_comprs->isCompress(inst.u16[0], out)) {
                is_compress = true;
                ret = true;
                break;
            }

            /* read last 16bit if not compress */
            err = info.mem->read(addr - info.addr, inst.u8+2, 2);
            if (err != RV_EOK) {
                LOGE("instruction fetch err: %d", err);
                break;
            }
            is_compress = false;
            out.inst    = inst.u32;
            ret         = true;
        } else {
            err = info.mem->read(addr - info.addr, inst.u8, 4);
            if (err != RV_EOK) {
                LOGE("instruction fetch err: %d", err);
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
            err = it.second->isValid(inst);
            if (err != RV_EOK) continue;
            err = it.second->exec(inst, m_regs, m_mems, m_ctrl);
            ret = (err == RV_EOK);
            break;
        }
    }while(0);
    return ret;
}

void rv32::regs_dump()
{
    LOGREGS("    %-8d %-8d %-8d %-8d", 0, 1, 2, 3);
    LOGREGS(" 0: %08x %08x %08x %08x", m_regs.x[0], m_regs.x[1], m_regs.x[2], m_regs.x[3]);
    LOGREGS(" 1: %08x %08x %08x %08x", m_regs.x[4], m_regs.x[5], m_regs.x[6], m_regs.x[7]);
    LOGREGS(" 2: %08x %08x %08x %08x", m_regs.x[8], m_regs.x[9], m_regs.x[10], m_regs.x[11]);
    LOGREGS(" 3: %08x %08x %08x %08x", m_regs.x[12], m_regs.x[13], m_regs.x[14], m_regs.x[15]);
    LOGREGS(" 4: %08x %08x %08x %08x", m_regs.x[16], m_regs.x[17], m_regs.x[18], m_regs.x[19]);
    LOGREGS(" 5: %08x %08x %08x %08x", m_regs.x[20], m_regs.x[21], m_regs.x[22], m_regs.x[23]);
    LOGREGS(" 6: %08x %08x %08x %08x", m_regs.x[24], m_regs.x[25], m_regs.x[26], m_regs.x[27]);
    LOGREGS(" 7: %08x %08x %08x %08x", m_regs.x[28], m_regs.x[29], m_regs.x[30], m_regs.x[31]);
    LOGREGS("PC: %08x", m_regs.pc);
}

}
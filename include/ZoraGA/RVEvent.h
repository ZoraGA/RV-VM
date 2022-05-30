#ifndef __ZORAGA_EVENT_H__
#define __ZORAGA_EVENT_H__

#include <mutex>
#include <stdint.h>
#include <condition_variable>

namespace ZoraGA
{

template<typename ET>
class Event
{
    public:
        bool wait(ET mask, size_t toms, ET *out = nullptr, bool allwait = false, bool clear = true)
        {
            std::unique_lock<std::mutex> lck(m_mutex);

            auto cond_chk = [&]() -> bool{
                if (allwait) {
                    return (m_event & mask) == mask;
                }else{
                    return (m_event & mask) != 0;
                }
            };

            if (toms == 0) {
                m_cond.wait(lck, cond_chk);

            } else {
                if (!m_cond.wait_for(lck, std::chrono::milliseconds(toms), cond_chk)) {
                    return false;
                }
            }

            if(out) *out = m_event & mask;
            if(clear) m_event &= ~mask;
            return true;
        }

        bool set(ET mask)
        {
            std::lock_guard<std::mutex> lck(m_mutex);
            m_event |= mask;
            m_cond.notify_all();
            return true;
        }
    
    private:
        std::mutex m_mutex;
        std::condition_variable m_cond;
        ET m_event = 0;
};

typedef Event<uint32_t> rv32_event;
typedef Event<uint64_t> rv64_event;

}

#endif
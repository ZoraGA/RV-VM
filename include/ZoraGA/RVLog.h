#ifndef __ZORAGA_RVVM_RVLOG_H__
#define __ZORAGA_RVVM_RVLOG_H__

#include <mutex>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

namespace ZoraGA::RVVM
{

class rvlog
{
    public:
        typedef enum Level{
            LV_INFO = 0,
            LV_ERROR,
            LV_WARN,
            LV_DEBUG,
            LV_VERBOSE,
            LV_INST,
            LV_REGS
        }Level;
    public:
        rvlog();
        ~rvlog();

        bool set_log_file(std::string path);
        bool set_log_level(Level lv);
        bool set_log_inst(bool ena);
        bool set_log_regs(bool ena);
        bool set_log_stdout(bool ena);
        void I(const char *fmt, ...);
        void E(const char *fmt, ...);
        void W(const char *fmt, ...);
        void D(const char *fmt, ...);
        void V(const char *fmt, ...);

        void inst(const char *fmt, ...);
        void regs(const char *fmt, ...);

    private:
        void log(const char *fmt, va_list args, Level lv);
        std::string time_str(std::string fmt);

    private:
        std::mutex m_mutex;
        std::string m_path;
        std::fstream m_file;
        Level m_level      = LV_INFO;
        bool  m_log_inst   = false;
        bool  m_log_regs   = false;
        bool  m_log_stdout = false;
};

}

#endif // __ZORAGA_RVVM_RVLOG_H__
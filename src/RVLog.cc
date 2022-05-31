#include "ZoraGA/RVLog.h"

namespace ZoraGA::RVVM
{

rvlog::rvlog()
{}

rvlog::~rvlog()
{
    if (m_file.is_open()) m_file.close();
}

bool rvlog::set_log_file(std::string path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) m_file.close();
    m_path = path;
    m_file.open(m_path, std::ios::out | std::ios::app);
    return m_file.is_open();
}

bool rvlog::set_log_level(Level lv)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_level = lv;
    return true;
}

bool rvlog::set_log_inst(bool ena)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_log_inst = ena;
    return true;
}

bool rvlog::set_log_stdout(bool ena)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_log_stdout = ena;
    return true;
}

bool rvlog::set_log_regs(bool ena)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_log_regs = ena;
    return true;
}

void rvlog::I(const char *fmt, ...)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_level < LV_INFO) return;
    va_list args;
    va_start(args, fmt);
    log(fmt, args, LV_INFO);
    va_end(args);
}

void rvlog::E(const char *fmt, ...)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_level < LV_ERROR) return;
    va_list args;
    va_start(args, fmt);
    log(fmt, args, LV_ERROR);
    va_end(args);
}

void rvlog::W(const char *fmt, ...)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_level < LV_WARN) return;
    va_list args;
    va_start(args, fmt);
    log(fmt, args, LV_WARN);
    va_end(args);
}

void rvlog::D(const char *fmt, ...)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_level < LV_DEBUG) return;
    va_list args;
    va_start(args, fmt);
    log(fmt, args, LV_DEBUG);
    va_end(args);
}

void rvlog::V(const char *fmt, ...)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_level < LV_VERBOSE) return;
    va_list args;
    va_start(args, fmt);
    log(fmt, args, LV_VERBOSE);
    va_end(args);
}

void rvlog::inst(const char *fmt, ...)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_log_inst) return;
    va_list args;
    va_start(args, fmt);
    log(fmt, args, LV_INST);
    va_end(args);
}

void rvlog::regs(const char *fmt, ...)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_log_regs) return;
    va_list args;
    va_start(args, fmt);
    log(fmt, args, LV_REGS);
    va_end(args);
}

void rvlog::log(const char *fmt, va_list args, Level lv)
{
    std::string fmt_append;
    std::string str;
    int len;

    fmt_append += time_str("[%Y-%m-%d %H:%M:%S] ");
    if (lv == LV_INFO) {
        fmt_append += "\033[0;32mI: ";
    }
    else if (lv == LV_ERROR) {
        fmt_append += "\033[0;31mE: ";
    }
    else if (lv == LV_WARN) {
        fmt_append += "\033[0;33mW: ";
    }
    else if (lv == LV_DEBUG) {
        fmt_append += "\033[0;34mD: ";
    }
    else if (lv == LV_VERBOSE) {
        fmt_append += "\033[0;37mV: ";
    }
    else if (lv == LV_INST) {
        fmt_append += "\033[0;35mINST:\033[0;32m ";
    }
    else if (lv == LV_REGS) {
        fmt_append += "\033[0;36mREGS:\033[0;32m ";
    }
    fmt_append += fmt;
    fmt_append += "\033[0m\n";

    len = vsnprintf(nullptr, 0, fmt_append.c_str(), args);
    str.reserve(len + 1);
    str.resize(len);
    vsnprintf(&str[0], len + 1, fmt_append.c_str(), args);

    if (m_file.is_open())
    {
        m_file << str;
    }

    if (m_log_stdout)
    {
        std::cout << str;
    }
}

std::string rvlog::time_str(std::string fmt)
{
    time_t t;
    struct tm tm;
    char buf[64];
    std::string str;

    t = time(nullptr);
    #if defined(_MSC_VER) || defined(__MINGW64__)
    localtime_s(&tm, &t);
    #else
    localtime_r(&t, &tm);
    #endif
    strftime(buf, sizeof(buf), fmt.c_str(), &tm);
    str = buf;
    return str;
}

}

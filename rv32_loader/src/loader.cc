#include "ZoraGA/RVVM.h"
#include "ZoraGA/RV32I.h"
#include "mem_ram.h"
#include "mem_rom.h"
#include <CLI/CLI.hpp>

bool endswith(std::string str, std::string end);
bool startswith(std::string str, std::string start);

using namespace ZoraGA::RVVM;

int main(int argc, char **argv)
{
    RV32::rv32 vm;
    RV32::RV32I rv32i;
    mem_rom rom;
    mem_ram ram;
    rvlog rvlog;
    CLI::App app{"RV32I Loader"};
    std::string rom_file = "test.bin";
    std::string rom_szstr, ram_szstr;
    uint32_t rom_addr = 0x80000000, rom_size = 128 * 1024;
    uint32_t ram_addr = 0x00000000, ram_size = 64 * 1024;

    app.add_option("-f,--rom", rom_file, "ROM file");
    app.add_option("--rom_addr", rom_addr, "ROM address");
    app.add_option("--rom_size", rom_szstr, "ROM size");
    app.add_option("--ram_addr", ram_addr, "RAM address");
    app.add_option("--ram_size", ram_szstr, "RAM size");

    CLI11_PARSE(app, argc, argv);

    try
    {
        if (app.count("--rom_size")) {
            if (endswith(rom_szstr, "K")) {
                rom_size = std::stoul(rom_szstr.substr(0, rom_szstr.size()-1), nullptr, 0) * 1024;
            }
            else if (endswith(rom_szstr, "M")) {
                rom_size = std::stoul(rom_szstr.substr(0, rom_szstr.size()-1), nullptr, 0) * 1024 * 1024;
            }
            else {
                rom_size = std::stoul(rom_szstr, nullptr, 0);
            }
        }

        if (app.count("--ram-size")) {
            if (endswith(ram_szstr, "K")) {
                ram_size = std::stoul(ram_szstr.substr(0, ram_szstr.size()-1), nullptr, 0) * 1024;
            }
            else if (endswith(ram_szstr, "M")) {
                ram_size = std::stoul(ram_szstr.substr(0, ram_szstr.size()-1), nullptr, 0) * 1024 * 1024;
            }
            else {
                ram_size = std::stoul(ram_szstr, nullptr, 0);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }

    if (!rom.load(rom_file)) {
        return -1;
    }

    if (!ram.set_size(ram_size)) {
        return -2;
    }

    printf("set logger\n");
    rvlog.set_log_file("rvvm_loader.log");
    rvlog.set_log_level(rvlog::LV_VERBOSE);
    rvlog.set_log_stdout(true);
    rvlog.set_log_inst(true);
    rvlog.set_log_regs(true);

    printf("set mem_rom\n");
    vm.add_mem(rom_addr, rom_size, &rom);
    printf("set mem_ram\n");
    vm.add_mem(ram_addr, ram_size, &ram);
    printf("add RV32I instruction collect\n");
    vm.add_inst("I", &rv32i);
    printf("set log\n");
    vm.set_log(&rvlog);
    printf("set start addr: %08x\n", rom_addr);
    vm.set_start_addr(rom_addr);
    printf("start VM\n");
    vm.start();
    if (vm.wait_for_start(1000)) {
        printf("VM start\n");
    }
    if (vm.wait_for_stop(0)) {
        printf("VM stop\n");
    }
    return 0;
}

bool endswith(std::string str, std::string end)
{
    if (str.length() < end.length())
        return false;
    return str.substr(str.length() - end.length(), end.length()) == end;
}

bool startswith(std::string str, std::string start)
{
    if (str.length() < start.length())
        return false;
    return str.substr(0, start.length()) == start;
}

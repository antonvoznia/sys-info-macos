#ifndef _PROCHANDLING
#define _PROCHANDLING

#include <iostream>
#include <libproc.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/user.h>
#include <utility>
#include <format>

constexpr char NA[] = "N/A";

enum PROC_INFO {
    PID = 0,
    NAME,
    USER_NAME,
    MEM,
    CPU
};

// See definition in prochandling.cpp
extern const char* PROC_INFO_FIELDS[];

/*
 Own implementation of proc_info_t
 to store only specific/needed information.
 */
struct proc_info_t {
    pid_t pid = 0;
    std::string name = NA;
    std::string userName = NA;
    std::string mem = NA;
    std::string cpu = NA;
};

namespace prochandling {
    constexpr char NA[] = "N/A";

    // Retrieve list of all running PIDs using macOS-specific API
    std::pair<std::vector<proc_info_t>, int> getAndFillAllPids();
    
    // Fill stcuture proc_info_t passed as argument.
    void extractProcInfo(proc_info_t &ps);

    // Fill process name and user name of proc_info struct.
    void fillProcBaseInfo(proc_info_t &ps);

    // Fill memory (MiB) consumption and CPU usage in time (sec).
    void fillExtendedInfo(proc_info_t &ps);

    // Return username who started process with pid.
    std::string getUserNameForPid(const pid_t pid);

    // Generate JSON string from vector of proc_info_t
    std::string generateJson(const std::vector<proc_info_t> & ps, const int num);
    
    // It will generate a string table from vector of proc_info_t 
    std::string generateTable(const std::vector<proc_info_t> & ps, const int num);
}

#endif
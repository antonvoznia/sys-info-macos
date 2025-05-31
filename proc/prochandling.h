#include <iostream>
#include <libproc.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/user.h>
#include <utility>

constexpr char NA[] = "N/A";

struct proc_info_t {
    pid_t pid = 0;
    std::string name = NA;
    std::string userName = NA;
    std::string mem = NA;
    std::string cpu = NA;
};

namespace prochandling {
    constexpr char NA[] = "N/A";
    std::pair<std::vector<proc_info_t>, int> getAndFillAllPids();
    void extractProcInfo(proc_info_t &ps);
    void fillProcBaseInfo(proc_info_t &ps);
    void fillExtendedInfo(proc_info_t &ps);
    std::string getUserNameForPid(const pid_t);
}
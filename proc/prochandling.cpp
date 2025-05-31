#include "prochandling.h"

std::pair<std::vector<proc_info_t>, int>  prochandling::getAndFillAllPids() {
  int numProcs = proc_listpids(PROC_ALL_PIDS, 0, nullptr, 0);

  if (numProcs < 0) {
    perror("proc_listpids");
    return {{}, 0};
  }
  numProcs /= sizeof(pid_t);
  // std::cout << "Total Num Processes: " << numProcs << std::endl;
  std::vector<pid_t> pids(numProcs);
  std::vector<proc_info_t> ps_info(numProcs);
  numProcs =
      proc_listpids(PROC_ALL_PIDS, 0, pids.data(), sizeof(pid_t) * numProcs);
  numProcs /= sizeof(pid_t);
  if (numProcs < 0) {
    perror("proc_listpids");
    return {{}, 0};
  }

  for (int i = 0; i < numProcs; ++i) {
    ps_info[i].pid = pids[i];
  }

  return {std::move(ps_info), numProcs};
}



void prochandling::fillProcBaseInfo(proc_info_t &ps) {
  char name[PROC_PIDPATHINFO_MAXSIZE];
  if (proc_name(ps.pid, name, sizeof(name)) > 0) {
    ps.name = name;
  }
  ps.userName = getUserNameForPid(ps.pid);
}

void prochandling::extractProcInfo(proc_info_t &ps) {
  fillProcBaseInfo(ps);
  fillExtendedInfo(ps);
}

void prochandling::fillExtendedInfo(proc_info_t &ps) {
  struct proc_taskinfo proc_tinfo;
  if (proc_pidinfo(ps.pid, PROC_PIDTASKINFO, 0, (void *)&proc_tinfo,
                   sizeof(struct proc_taskinfo)) > 0) {
    ps.mem = std::to_string(proc_tinfo.pti_resident_size);

    uint64_t total_nanos = proc_tinfo.pti_total_user + proc_tinfo.pti_total_system;
    // convert to seconds
    double total_seconds = static_cast<double>(total_nanos) / 1'000'000'000.0;
    ps.cpu = std::to_string(total_seconds);
  }
}


// void prochandling::printExtendedInfo(const struct proc_taskinfo &proc_info) {
//   std::cout << "Mem:        " << proc_info.pti_virtual_size
//             << " bytes\n";
//   // std::cout << "Resident Size:       " << proc_info.pti_resident_size
//   //           << " bytes\n";
//   std::cout << "CPU:     " << proc_info.pti_total_user << " ns\n";
//   std::cout << "CPU:   " << proc_info.pti_total_system << " ns\n";
//   // std::cout << "Threads user:        " << proc_info.pti_threads_user << "\n";
//   // std::cout << "Threads system:      " << proc_info.pti_threads_system << "\n";
//   // std::cout << "Policy:              " << proc_info.pti_policy << "\n";
//   // std::cout << "Faults:              " << proc_info.pti_faults << "\n";
//   // std::cout << "Pageins:             " << proc_info.pti_pageins << "\n";
//   // std::cout << "COW Faults:          " << proc_info.pti_cow_faults << "\n";
//   // std::cout << "Messages Sent:       " << proc_info.pti_messages_sent << "\n";
//   // std::cout << "Messages Received:   " << proc_info.pti_messages_received
//   //           << "\n";
//   // std::cout << "Syscalls Mach:       " << proc_info.pti_syscalls_mach << "\n";
//   // std::cout << "Syscalls Unix:       " << proc_info.pti_syscalls_unix << "\n";
//   // std::cout << "Context Switches:    " << proc_info.pti_csw << "\n";
//   // std::cout << "Thread Inspections:  " << proc_info.pti_threadnum << "\n";
//   // std::cout << "Num Running Threads: " << proc_info.pti_numrunning << "\n";
// }

std::string prochandling::getUserNameForPid(const pid_t pid) {
    struct kinfo_proc kp;
    size_t len = sizeof(kp);
    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, pid };

    if (sysctl(mib, 4, &kp, &len, NULL, 0) == -1) {
        return NA;
    }

    uid_t uid = kp.kp_eproc.e_ucred.cr_uid;
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        return std::move(std::string(pw->pw_name));
    }
    return NA;
}
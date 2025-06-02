#include "prochandling.h"

const char *PROC_INFO_FIELDS[] = {"PID", "Name", "User", "Mem", "CPU"};

std::pair<std::vector<proc_info_t>, int> prochandling::getAndFillAllPids() {
  /*
    Get the number of PIDs currently running.
    So we can put empty buffer for the firs interation
    of this function.
  */
  int numProcs = proc_listpids(PROC_ALL_PIDS, 0, nullptr, 0);

  // No process found, see perror for more information.
  if (numProcs < 0) {
    perror("proc_listpids");
    return {{}, 0};
  }
  numProcs /= sizeof(pid_t);
  std::vector<pid_t> pids(numProcs);
  std::vector<proc_info_t> ps_info(numProcs);
  numProcs =
      proc_listpids(PROC_ALL_PIDS, 0, pids.data(), sizeof(pid_t) * numProcs);
  numProcs /= sizeof(pid_t);

  // No process found, see perror for more information.
  if (numProcs < 0) {
    perror("proc_listpids");
    return {{}, 0};
  }

  bool pid0_found = false;
  for (int i = 0; i < numProcs; ++i) {
    switch (pids[i]) {
      //skip process with PID 0
      case 0: pid0_found = true; break;
      default:
          ps_info[i].pid = pids[i];
    }
  }

  // Decrease number of PIDs since we skipped PID 0.
  numProcs -= pid0_found ? 1 : 0;

  return {ps_info, numProcs};
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
    // convert bytes to megabytes
    double mem_mib =
        static_cast<double>(proc_tinfo.pti_resident_size) / (1024 * 1024);
    ps.mem = std::to_string(mem_mib);

    uint64_t total_nanos =
        proc_tinfo.pti_total_user + proc_tinfo.pti_total_system;
    // convert nanoseconds to seconds
    double total_seconds = static_cast<double>(total_nanos) / 1'000'000'000.0;
    ps.cpu = std::to_string(total_seconds);
  }
}

std::string prochandling::getUserNameForPid(const pid_t pid) {
  struct kinfo_proc kp;
  size_t len = sizeof(kp);
  int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, pid};

  if (sysctl(mib, 4, &kp, &len, NULL, 0) == -1) {
    return NA;
  }

  uid_t uid = kp.kp_eproc.e_ucred.cr_uid;
  struct passwd *pw = getpwuid(uid);
  if (pw) {
    return std::string(pw->pw_name);
  }
  return NA;
}

std::string prochandling::generateJson(const std::vector<proc_info_t> &ps, const int num) {
  std::string ret = "[";

  /*
    To avoid any complication with JSON
    third party libraries, I used simple std::format
    print in json format.
  */
  for (int i = 0; i < num; ++i) {
    std::string obj = std::format(
        "{{"
        "\"{}\": \"{}\", \n"
        "\"{}\": \"{}\", \n"
        "\"{}\": \"{}\", \n"
        "\"{}\": \"{}\", \n"
        "\"{}\": \"{}\""
        "}}",
        PROC_INFO_FIELDS[PROC_INFO::PID], ps[i].pid,
        PROC_INFO_FIELDS[PROC_INFO::NAME], ps[i].name,
        PROC_INFO_FIELDS[PROC_INFO::USER_NAME], ps[i].userName,
        PROC_INFO_FIELDS[PROC_INFO::MEM], ps[i].mem,
        PROC_INFO_FIELDS[PROC_INFO::CPU], ps[i].cpu);

    ret += obj;
    if (i != (int) num - 1) {
      ret += ",\n";
    }
  }

  ret += "]";
  return ret;
}

std::string prochandling::generateTable(const std::vector<proc_info_t> &ps, const int num) {
  std::string ret;

  constexpr const char* headerFmt = "{:<6} {:<30} {:<25} {:>10} {:>12}\n";
  constexpr const char* rowFmt    = "{:<6} {:<30} {:<25} {:>10} {:>12}\n";

  ret += std::vformat(
      headerFmt, std::make_format_args(PROC_INFO_FIELDS[PROC_INFO::PID],
                                       PROC_INFO_FIELDS[PROC_INFO::NAME],
                                       PROC_INFO_FIELDS[PROC_INFO::USER_NAME],
                                       PROC_INFO_FIELDS[PROC_INFO::MEM],
                                       PROC_INFO_FIELDS[PROC_INFO::CPU]));

  ret += std::string(87, '-') + "\n";

  for (int i = 0; i < num; ++i) {
    ret += std::vformat(
        rowFmt, std::make_format_args(ps[i].pid, ps[i].name, ps[i].userName, ps[i].mem, ps[i].cpu));
  }

  return ret;
}
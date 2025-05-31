#include <iostream>
#include <vector>

#include "proc/prochandling.h"

int main(int argc, char **argv) {

  std::pair<std::vector<proc_info_t>, int> ps = prochandling::getAndFillAllPids();

  // Fill entire information about all pids we collected above.
  for (int i = 0; i < ps.second; ++i) {
    prochandling::extractProcInfo(ps.first[i]);
  }

  // print it according format
  return 0;
}

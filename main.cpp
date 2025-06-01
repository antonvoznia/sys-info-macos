#include <iostream>
#include <vector>

#include "proc/prochandling.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

constexpr int SUCCESS_CODE = 0;
constexpr int ERROR_CODE = 1;

int main(int argc, char **argv) {
  bool jsonOutput = false;
  std::string outputFile;
  bool showHelp = false;

  // Parse arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--json" || arg == "-j") {
      jsonOutput = true;
    } else if (arg == "--help" || arg == "-h") {
      showHelp = true;
    } else if ((arg == "--output-file" || arg == "-o") && i + 1 < argc) {
      outputFile = argv[++i];
    } else {
      std::cerr << "Unknown argument: " << arg << "\n";
      showHelp = true;
      break;
    }
  }

  if (showHelp) {
    std::cout <<
      "Usage: sys-info [--json|-j] [--output-file <file>|-o <file>] [--help|-h]\n"
      "\nOptions:\n"
      "  -j, --json          Output in JSON format\n"
      "  -o, --output-file   Write output to a specified file\n"
      "  -h, --help          Show this help message\n";
    return SUCCESS_CODE;
  }

  // Gather and process data
  auto ps = prochandling::getAndFillAllPids();
  if (ps.second < 0) {
    std::cerr << "Error: Cannot collect processes.\n";
  }
  for (int i = 0; i < ps.second; ++i) {
    prochandling::extractProcInfo(ps.first[i]);
  }

  std::string output;
  if (jsonOutput) {
    output = prochandling::generateJson(ps.first, ps.second);
  } else {
    output = prochandling::generateTable(ps.first, ps.second);
  }

  if (!outputFile.empty()) {
    std::ofstream ofs(outputFile);
    if (!ofs) {
      std::cerr << "Error: Cannot open file '" << outputFile << "' for writing.\n";
      return ERROR_CODE;
    }
    ofs << output;
  }
  std::cout << output << std::endl;

  return SUCCESS_CODE;
}


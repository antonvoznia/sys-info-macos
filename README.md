# macOS Process Info Tool

This is a simple macOS-native command-line tool written in C++ that prints information about all currently running processes.

It shows:
- Process ID (pid)
- Process name
- CPU time usage (in seconds since the process started)
- Memory usage (in megabytes)
- User name (owner of the process)

## Build Instructions

To build the tool, use:
```
make
```

This compiles main.cpp and proc/prochandling.cpp into a main executable using g++.

Alternatively, compile manually with:
```
clang++ main.cpp -std=c++20 -o main proc/prochandling.cpp
```

## Output Formats:

1. Default (table format):

PID    Name                           User                          Mem         CPU (s)
---------------------------------------------------------------------------------------
1      launchd                        root                          1.41        0.00
542    WindowServer                   SuperUser                     9.99        2.91
...

- Memory is shown in megabytes (rounded to string format)
- CPU time is total (user + system) in seconds

2. JSON format:

```
[
  {
    "PID": "2112",
    "Name": "WindowServer",
    "User": "SuperUser",
    "Mem": "45.06",
    "CPU": "3.12"
  },
  {
    "PID": "3928",
    "Name": "SomeApp",
    "User": "root",
    "Mem": "12.34",
    "CPU": "1.01"
  }
]
```
## Command-line Flags:

The following options are supported:

  -j, --json           Output results in JSON format
  -o, --output-file    Write the output to a specified file
  -h, --help           Show help and usage information

## Implementation Details:

- Uses macOS system calls such as proc_listpids, proc_pidinfo, and proc_pidtaskinfo to extract process information.
- Resolves user IDs to login names using getpwuid().
- Converts CPU usage time from nanoseconds to seconds.

## Requirements:

- macOS 11+ (ARM64 or Intel)
- C++20 or later
- Command-line tools (install with: xcode-select --install)

## File Structure:

- main.cpp               Entry point that iterates over processes and prints info
- proc/prochandling.cpp  Implementation of process extraction functions
- proc/prochandling.h    Declarations of utility functions
- Makefile               Build instructions


Notes:

- PID: 0 entries are filtered out internally — only valid processes are shown.
- CPU usage time is the sum of system and user mode times in seconds.

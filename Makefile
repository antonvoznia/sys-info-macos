# Makefile for sys-info

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20
TARGET = sys-info
SRCS = main.cpp proc/prochandling.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

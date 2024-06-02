# Makefile for compiling a simple C++ program

# Define the compiler and compiler flags
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Define the target executable
TARGET = main

# Automatically find all .cpp files in the directory
SRCS = $(wildcard *.cpp)

# Define the object files
OBJS = $(SRCS:.cpp=.o)

# Default target to build the executable
all: $(TARGET)

# Rule to link the object files and create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile the source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
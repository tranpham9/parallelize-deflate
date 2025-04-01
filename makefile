
# Compiler
CXX = g++

# Compiler flags
CXXFLAGS =  -g -pthread -std=c++20

# Target executable
TARGET = build/deflate.out

# For deleting the target
TARGET_DEL = build/deflate.out

# Source files
SRCS = src/main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Rule to link object files into the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) 

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove generated files
.PHONY: clean
clean:
	rm $(TARGET_DEL) $(OBJS)






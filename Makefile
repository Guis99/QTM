# Compiler and flags
CXX = clang++
BASE_CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra -g
DEBUG_FLAGS = -DVERBOSE
CXXFLAGS = $(BASE_CXXFLAGS) $(if $(DEBUG),$(DEBUG_FLAGS))

# Directories
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = test
BUILD_DIR = build

# Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
SRC_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))
MAIN_SRC = main.cpp
MAIN_EXEC = main

# Default target
all: main

# Create build directory if not exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile each source file into an object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling source file $(notdir $<)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Build the main executable by linking all object files
main: $(SRC_OBJS) $(MAIN_SRC)
	@echo "Building main executable: $(MAIN_EXEC)"
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) $(SRC_OBJS) -o $(MAIN_EXEC)

# Build a specific test file
test: $(SRC_OBJS)
	@if [ -z "$(TEST)" ]; then \
		echo "Error: Specify a test file with TEST=<filename>.cpp"; \
		exit 1; \
	fi
	@echo "Building test: $(TEST)"
	$(CXX) $(CXXFLAGS) $(TEST_DIR)/$(TEST) $(SRC_OBJS) -o $(BUILD_DIR)/$(basename $(TEST))

# Clean build artifacts
clean:
	@echo "Cleaning build directory and main executable"
	rm -rf $(BUILD_DIR)/*
	rm -f $(MAIN_EXEC)

# Phony targets
.PHONY: all main test clean

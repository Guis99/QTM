#!/bin/bash

# Exit immediately if any command fails
set -e

# Directory containing test files
TEST_DIR="test"
BUILD_DIR="build"

# Ensure we are in the root directory of the project
if [ ! -f "Makefile" ]; then
  echo "Error: Makefile not found. Run this script from the root directory of your project."
  exit 1
fi

# Check for the debug flag
DEBUG_FLAG=""
if [ "$1" == "-debug" ]; then
  DEBUG_FLAG="DEBUG=1"
fi

# Loop over all test*.cpp files in the test directory
for test_file in "$TEST_DIR"/test*.cpp; do
  # Extract the base name of the test file (e.g., test1 from test1.cpp)
  test_name=$(basename "$test_file" .cpp)

  echo "Compiling and running test: $test_name"

  # Compile the test using the Makefile
  make test TEST="$test_name.cpp" $DEBUG_FLAG

  # Run the generated test executable
  "./$BUILD_DIR/$test_name"

  echo "Test $test_name completed successfully."
done

# Clean up the build directory
echo "Cleaning up build artifacts"
make clean

echo "All tests completed and cleaned up successfully."

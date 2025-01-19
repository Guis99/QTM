#include <vector>
#include <memory>
#include <array>
#include <stack>
#include <iostream>
#include <unordered_map>
#include <math.h>
#include <set>

#define PI 3.1415926535897932384

// Helper function to handle variadic arguments
template<typename... Args>
void debug_print(Args&&... args) {
    (std::cout << ... << args) << std::endl; // Fold expression to print all arguments
}

// Define the DEBUG_PRINT macro
#ifdef VERBOSE
#define DEBUG_PRINT(...) debug_print(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#pragma once

#pragma once

#define ein_assert(cond) \
  do { \
    if ein_unlikely (!(cond)) { \
      spdlog::error("Assertion failed: {} in function {} at {}:{}.", #cond, __func__, __FILE__, __LINE__); \
      std::abort(); \
    } \
  } while (0)

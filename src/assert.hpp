#pragma once

/// This provides a variadic assert macro that is left around even in our release configuration
/// \hideinitializer
/// \hideinlinesource
#define ein_assert(cond,...) \
  do { \
    if (!cond) [[unlikely]] { \
      []<typename ... Args>(Args && ... args) { \
        if constexpr (sizeof...(Args) == 0) \
          spdlog::error("{}:{}: assertion failed ({}) in function {}",__FILE__,__LINE__,#cond,__PRETTY_FUNCTION__); \
        else \
          spdlog::error("{}:{}: assertion failed ({}) in function {}: {}",__FILE__,__LINE__,#cond,__PRETTY_FUNCTION__,fmt::vformat(std::forward<Args>(args)...)); \
        std::abort(); \
      }(__VA_ARGS__); \
    } \
  } while (0)

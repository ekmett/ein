#pragma once

/// \file
/// \brief assert even during release
/// \ingroup macros
/// \license
/// SPDX-FileType: Source
/// SPDX-FileCopyrightText: 2021-2024 Edward Kmett <ekmett@gmail.com>
/// SPDX-FileCopyrightText: 2012 William Swanson
/// SPDX-License-Identifier: MIT
/// \endlicense

/// \def ein_assert
/// \ingroup macros
/// \hideinitializer \hideinlinesource
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

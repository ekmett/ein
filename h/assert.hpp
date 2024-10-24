#pragma once

/**
  \file
  \brief assert even during release
  \ingroup macros
  \license
  SPDX-FileType: Source
  SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
  SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
  \endlicense
*/

/** \def ein_assert
    \ingroup macros
    \hideinitializer \hideinlinesource */
#define ein_assert(cond,...) \
  do { \
    if (!cond) [[unlikely]] { \
      []<typename ... Args> static (Args && ... args) noexcept { \
        if constexpr (sizeof...(Args) == 0) \
          spdlog::error("{}:{}: assertion failed ({}) in function {}",__FILE__,__LINE__,#cond,__PRETTY_FUNCTION__); \
        else \
          spdlog::error("{}:{}: assertion failed ({}) in function {}: {}",__FILE__,__LINE__,#cond,__PRETTY_FUNCTION__,fmt::vformat(std::forward<Args>(args)...)); \
        std::abort(); \
      }(__VA_ARGS__); \
    } \
  } while (0)

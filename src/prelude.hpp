#pragma once

/// \defgroup headers headers

/// \file
/// \ingroup headers
/// \license
/// SPDX-FileType: Source
/// SPDX-FileCopyrightText: 2021-2024 Edward Kmett <ekmett@gmail.com>
/// SPDX-FileCopyrightText: 2012 William Swanson
/// SPDX-License-Identifier: MIT
/// \endlicense
/// \brief precompiled header
/// \hideincludedbygraph hideincludegraph

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cassert>
#include <chrono>
#include <cmath>
#include <concepts>
#include <condition_variable>
#include <coroutine>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <random>
#include <ratio>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <version>

#include "attributes.hpp" // EIN(..)
#include "assert.hpp" // ein_assert(...)

// simd intrinsics
// #include <immintrin.h>

// json
//#include <nlohmann/json.hpp>

// logging/formatting
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ranges.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/bundled/color.h>
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#pragma once

/// \file
/// \brief precompiled header
/// \author Edward Kmett
/// \hideincludedbygraph
/// \hideincludegraph

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

/// \defgroup internals_group internals
/// stuff the user shouldn't need to care much about

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

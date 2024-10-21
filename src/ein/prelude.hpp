#pragma once // precompiled header

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <coroutine>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
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
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <version>

#include "attributes.hpp" // EIN(..)
#include "assert.hpp" // EIN_ASSERT(...), assume

// simd intrinsics
#include <immintrin.h>

// json
//#include <nlohmann/json.hpp>

// logging/formatting
//#include <spdlog/spdlog.h>
//#include <spdlog/fmt/ranges.h>
//#include <spdlog/fmt/ostr.h>
//#include <spdlog/fmt/bundled/color.h>
//#include <spdlog/cfg/env.h>
//#include <spdlog/sinks/stdout_color_sinks.h>

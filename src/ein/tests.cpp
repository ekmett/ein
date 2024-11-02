#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#define EIN_TESTING
// add headers below to include them in the test suite
#include "cpuid.hpp"
#include "concepts.hpp"
#include "memory.hpp"
#include "numerics/bf16.hpp"
#include "numerics/fp16.hpp"
#include "numerics.hpp"
#include "profiling.hpp"
#include "static_string.hpp"
#include "types.hpp"
#include "wait.hpp"

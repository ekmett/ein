
/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

/// \cond
#ifdef ENABLE_TESTS
#include <doctest.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include "static_string.hpp"
#include "profiling.hpp"

using namespace ein;

TEST_SUITE("profiling") {

  TEST_CASE("scope enum serialization") {
    nlohmann::json j = ein::profiling::scope::global;
    CHECK(j == "g");

    j = ein::profiling::scope::process;
    CHECK(j == "p");

    j = ein::profiling::scope::thread;
    CHECK(j == "t");
  }

  TEST_CASE("event_type enum serialization") {
    nlohmann::json j = ein::profiling::event_type::duration_begin;
    CHECK(j == "B");

    j = ein::profiling::event_type::duration_end;
    CHECK(j == "E");

    j = ein::profiling::event_type::complete;
    CHECK(j == "X");
  }

  TEST_CASE("profile_event JSON serialization") {
    ein::profiling::profile_event<std::chrono::nanoseconds> event{
        .name = "test_event"_ss,
        .cat = "test_category"_ss,
        .ph = ein::profiling::event_type::instant,
        .s = ein::profiling::scope::thread
    };

    nlohmann::json j = event;
    CHECK(j["name"] == "test_event");
    CHECK(j["cat"] == "test_category");
    CHECK(j["ph"] == "i");
    CHECK(j["s"] == "t");
  }

  TEST_CASE("profile logging") {
    ein::profiling::profile<> profiler;
    ein::profiling::profile_event<std::chrono::nanoseconds> event{
        .name = "log_event"_ss,
        .ph = ein::profiling::event_type::instant,
        .s = ein::profiling::scope::thread
    };

    profiler.log(event);

    CHECK(profiler.events.size() == 1);
    CHECK(profiler.events[0].name == "log_event"_scs);
    CHECK(profiler.events[0].ph == ein::profiling::event_type::instant);
    CHECK(profiler.events[0].s == ein::profiling::scope::thread);
  }

  TEST_CASE("profile_scope automatic saving") {
    std::filesystem::path test_path = "test_profile.json";
    {
        ein::profiling::profile_scope<> profiler(test_path);
        ein::profiling::profile_event<std::chrono::nanoseconds> event{
            .name = "scope_event"_ss,
            .ph = ein::profiling::event_type::instant,
            .s = ein::profiling::scope::thread
        };

        profiler.p.log(event);
    } // `profile_scope` destructor should save the profile here

    CHECK(std::filesystem::exists(test_path));

    // Clean up
    std::filesystem::remove(test_path);
  }

} // TEST_SUITE("profiling")

#endif
/// \endcond

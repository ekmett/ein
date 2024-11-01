#include <catch2/catch_all.hpp>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include "ein/static_string.hpp"
#include "ein/profiling.hpp"

using namespace ein;

TEST_CASE("scope enum serialization") {
    nlohmann::json j = ein::profiling::scope::global;
    REQUIRE(j == "g");

    j = ein::profiling::scope::process;
    REQUIRE(j == "p");

    j = ein::profiling::scope::thread;
    REQUIRE(j == "t");
}

TEST_CASE("event_type enum serialization") {
    nlohmann::json j = ein::profiling::event_type::duration_begin;
    REQUIRE(j == "B");

    j = ein::profiling::event_type::duration_end;
    REQUIRE(j == "E");

    j = ein::profiling::event_type::complete;
    REQUIRE(j == "X");
}

TEST_CASE("profile_event JSON serialization") {
    ein::profiling::profile_event<std::chrono::nanoseconds> event{
        .name = "test_event"_ss,
        .cat = "test_category"_ss,
        .ph = ein::profiling::event_type::instant,
        .s = ein::profiling::scope::thread
    };

    nlohmann::json j = event;
    REQUIRE(j["name"] == "test_event");
    REQUIRE(j["cat"] == "test_category");
    REQUIRE(j["ph"] == "i");
    REQUIRE(j["s"] == "t");
}

TEST_CASE("profile logging") {
    ein::profiling::profile<> profiler;
    ein::profiling::profile_event<std::chrono::nanoseconds> event{
        .name = "log_event"_ss,
        .ph = ein::profiling::event_type::instant,
        .s = ein::profiling::scope::thread
    };

    profiler.log(event);

    REQUIRE(profiler.events.size() == 1);
    REQUIRE(profiler.events[0].name == "log_event"_scs);
    REQUIRE(profiler.events[0].ph == ein::profiling::event_type::instant);
    REQUIRE(profiler.events[0].s == ein::profiling::scope::thread);
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

    REQUIRE(std::filesystem::exists(test_path));

    // Clean up
    std::filesystem::remove(test_path);
}

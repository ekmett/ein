#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <cstring> // for strcmp
#include <sys/mman.h> // for mmap, PROT_READ, MAP_PRIVATE, MAP_ANONYMOUS
#include "memory.hpp"

TEST_CASE("unique_str manages strdup-allocated strings") {
    const char* original = "Hello, World!";
    ein::unique_str managed_str = ein::dup(original);

    REQUIRE(managed_str != nullptr);
    REQUIRE(std::strcmp(managed_str.get(), original) == 0);
}

TEST_CASE("unique_c_ptr manages malloc-allocated memory") {
    constexpr size_t size = 1024;
    ein::unique_c_ptr<char> managed_ptr(static_cast<char*>(std::malloc(size)));

    REQUIRE(managed_ptr != nullptr);
    std::memset(managed_ptr.get(), 0, size);
}

TEST_CASE("mmap_ptr manages mmap-allocated memory") {
    constexpr size_t size = 4096;
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    REQUIRE(ptr != MAP_FAILED);

    ein::mmap_ptr managed_mmap = ein::make_mmap_ptr(ptr, size);
    REQUIRE(managed_mmap != nullptr);
}

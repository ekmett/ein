include(FetchContent)

fetchcontent_declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.1.0 # or a later release
)

set(CATCH_INSTALL_DOCS OFF CACHE BOOL "skip catch docs")

fetchcontent_makeavailable(Catch2)

exclude(Catch2::Catch2 Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)
include(CatchShardTests)

# silence a third-party warning on clang++-18+
target_compile_options(Catch2 PRIVATE
  $<$<AND:$<CXX_COMPILER_ID:Clang>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,18>>:-Wno-unused-but-set-variable>
)

# install(TARGETS t_simd RUNTIME DESTINATION bin)

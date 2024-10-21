# ein library
# ===========

add_library(ein STATIC)

target_sources(ein PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS ein
  FILES
    assert.cppm
    memory.cppm
    types.cppm
    util.cppm)

target_link_libraries(ein PUBLIC
  spdlog::spdlog
  nlohmann_json::nlohmann_json
  ${COMPILER_RT_BUILTINS})

target_include_directories(ein PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>)

install(TARGETS ein
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  INCLUDES DESTINATION include)

set_target_properties(ein PROPERTIES PUBLIC_HEADER "assert.hpp;attributes.hpp;map.hpp;prelude.hpp")

lto(ein)

if(PRECOMPILE_HEADERS)
  target_precompile_headers(ein PRIVATE prelude.hpp)
endif()

target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-msse4.2;-mavx;-mbmi2;-mfma;-mavx2;-ftree-vectorize;-funroll-loops>")
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-mavx512f;-mavx512dq;-mavx512bw;-mavx512vl;-mavx512vbmi2;-mavx512vp2intersect;-mavx512bf16>")
# target_compile_options(ein PUBLIC -stdlib=libc++ -std=c++20)


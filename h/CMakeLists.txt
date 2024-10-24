include(CheckRestrict)

# Configure config.hpp from config.hpp.in
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.hpp"
)

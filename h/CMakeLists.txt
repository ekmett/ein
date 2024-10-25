include(CheckRestrict)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.hpp"
)

list(APPEND SOURCE_HEADER_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/assert.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/attributes.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/intrinsics.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/prelude.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/std.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/third-party.hpp"
)


# Replace SOURCE_DIR with BINARY_DIR for each header
set(OUTPUT_HEADER_FILES "")
foreach(header ${SOURCE_HEADER_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" replaced_header "${header}")
    list(APPEND OUTPUT_HEADER_FILES "${replaced_header}")
endforeach()

# Custom command to copy header files
add_custom_command(
  OUTPUT ${OUTPUT_HEADER_FILES}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_HEADER_FILES} "${CMAKE_CURRENT_BINARY_DIR}"
  DEPENDS ${SOURCE_HEADER_FILES}
)

list(APPEND OUTPUT_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)

# Custom target with dependency
add_custom_target(ein_headers ALL DEPENDS ${OUTPUT_HEADER_FILES})

# Get current git hash

# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Check if the repo is dirty
execute_process(
  COMMAND git diff --quiet || echo "-dirty"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_DIRTY
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Combine them into a single variable
if(GIT_DIRTY STREQUAL "")
  set(GIT_STATUS "${GIT_HASH}")
else()
  set(GIT_STATUS "${GIT_HASH}${GIT_DIRTY}")
endif()

# Output variables to be used in documentation or elsewhere
message(STATUS "Git commit hash: ${GIT_HASH}")
message(STATUS "Git status: ${GIT_STATUS}")

# Export as cache variables to use in other parts of the project (like Doxygen)
set(CURRENT_GIT_HASH "${GIT_HASH}" CACHE STRING "Current Git hash")
set(CURRENT_GIT_STATUS "${GIT_STATUS}" CACHE STRING "Git commit hash and dirty status")

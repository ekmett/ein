include(FetchContent)

# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

function(FetchContentCached)
  # Capture ARGN for delegation later
  set(original_ARGS ${ARGN})

  # Parse specific arguments
  cmake_parse_arguments(FCD "" "NAME;TARGET_DIR" "URL;URL_HASH;GIT_REPOSITORY;GIT_TAG;GIT_SHALLOW;GIT_PROGRESS" ${ARGN})

  # Ensure that the NAME argument is specified
  if(NOT FCD_NAME)
    message(FATAL_ERROR "FetchContent_CachedDownload requires a NAME argument.")
  endif()

  # Determine the base directory for dependencies
  if(DEFINED FETCHCONTENT_BASE_DIR)
    set(BASE_DIR "${FETCHCONTENT_BASE_DIR}")
  else()
    set(BASE_DIR "${CMAKE_BINARY_DIR}/_deps")
  endif()

  # Determine the target directory for the specific dependency, allow override by TARGET_DIR
  if(FCD_TARGET_DIR)
    set(TARGET_DIR "${FCD_TARGET_DIR}")
  else()
    set(TARGET_DIR "${BASE_DIR}/${FCD_NAME}-src")
  endif()

  # Check if the main target already exists (to avoid redefinition errors)
  if(TARGET ${FCD_NAME})
#     message(NOTICE "Target '${FCD_NAME}' already exists. Skipping download and configuration.")
    return()
  endif()

  # Check if the target directory exists
  if(EXISTS "${TARGET_DIR}")
#    message(NOTICE "Found target directory for '${FCD_NAME}' at ${TARGET_DIR}.")

    # Check if the directory contains a CMakeLists.txt file
    if(EXISTS "${TARGET_DIR}/CMakeLists.txt")
#      message(NOTICE "Using cached dependency '${FCD_NAME}' from ${TARGET_DIR}.")
      add_subdirectory("${TARGET_DIR}" "${FCD_NAME}-build")
    else()
#      message(NOTICE "Target directory exists but no CMakeLists.txt found in '${TARGET_DIR}'.")
#      message(NOTICE "Dependency '${FCD_NAME}' might be non-CMake or requires manual setup.")
    endif()
  else()
    # If the directory does not exist, use FetchContent to download the dependency
#    message(NOTICE "Downloading and configuring dependency '${FCD_NAME}'...")

    # Declare the dependency with the full original arguments
    FetchContent_Declare(${FCD_NAME} SOURCE_DIR "${TARGET_DIR}" ${original_ARGS})

    # Ensure the dependency is available and configured
    FetchContent_MakeAvailable(${FCD_NAME})
  endif()
endfunction()

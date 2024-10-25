# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# Specify the required Doxygen version for cmake module support
set(REQUIRED_DOXYGEN_VERSION "1.12.0")

set(CXX_Clang "$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>")
fetchcontent_declare(
  doxygen-awesome-css
  GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
  GIT_TAG        v2.3.4
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
fetchcontent_makeavailable(doxygen-awesome-css)

find_package(Doxygen)

function(check_doxygen_version)
  if(DOXYGEN_FOUND)
    execute_process(
      COMMAND "${DOXYGEN_EXECUTABLE}" --version
      OUTPUT_VARIABLE DOXYGEN_VERSION_OUTPUT
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(DOXYGEN_VERSION_OUTPUT VERSION_LESS "${REQUIRED_DOXYGEN_VERSION}")
      set(DOXYGEN_FOUND FALSE PARENT_SCOPE)
    else()
      set(DOXYGEN_FOUND TRUE PARENT_SCOPE)
      set(DOXYGEN_VERSION "${DOXYGEN_VERSION_OUTPUT}" PARENT_SCOPE)
    endif()
  else()
    set(DOXYGEN_FOUND FALSE PARENT_SCOPE)
  endif()
endfunction()

# Check if the required Doxygen version is available
check_doxygen_version()

# Set the base URL to MathJax's CDN path
set(MATHJAX_URL "https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.min.js")
set(MATHJAX_OUTPUT_PATH "${CMAKE_BINARY_DIR}/third_party/mathjax/tex-mml-chtml.min.js")
set(EXPECTED_SHA256 "f182aef990a28dd3a816c866dcb18727d42f221f30a5946761b4272573d44148")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/third_party/mathjax")
message(STATUS "Downloading MathJax 3 minified JavaScript from ${MATHJAX_URL}...")
file(DOWNLOAD ${MATHJAX_URL} ${MATHJAX_OUTPUT_PATH} STATUS download_status)
if(NOT download_status EQUAL 0)
  message(FATAL_ERROR "Failed to download MathJax 3 minified JavaScript from ${MATHJAX_URL}")
endif()
file(SHA256 ${MATHJAX_OUTPUT_PATH} DOWNLOADED_SHA256)
if(NOT DOWNLOADED_SHA256 STREQUAL EXPECTED_SHA256)
  message(FATAL_ERROR "SHA256 mismatch for ${MATHJAX_OUTPUT_PATH}. Expected ${EXPECTED_SHA256}, but got ${DOWNLOADED_SHA256}.")
else()
  message(STATUS "SHA256 verified successfully for ${MATHJAX_OUTPUT_PATH}.")
endif()

if(DOXYGEN_FOUND)
  message(STATUS "Found Doxygen ${DOXYGEN_VERSION}: ${DOXYGEN_EXECUTABLE}")

  # Configure Doxyfile
  set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

  # Add a custom target for generating documentation
  add_custom_target(
    docs
    ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen ${DOXYGEN_EXECUTABLE}"
    VERBATIM
    DEPENDS "${CMAKE_BINARY_DIR}/h/config.hpp"
  )

  # Make sure the documentation target depends on DoxygenProject if we built it
  if(TARGET DoxygenProject)
    add_dependencies(docs DoxygenProject)
  endif()
endif()

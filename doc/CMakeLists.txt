# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# Specify the required Doxygen version for cmake module support
set(REQUIRED_DOXYGEN_VERSION "1.12.0")

# Make sure the copy runs whenever building the documentation
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

# TODO: just download it ourselves if not

if(DOXYGEN_FOUND)
  message(STATUS "Found Doxygen ${DOXYGEN_VERSION}: ${DOXYGEN_EXECUTABLE}")

  # get doxygen-awesome-css
  fetchcontentcached(
    NAME doxygen-awesome-css
    GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
    GIT_TAG        v2.3.4
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )

  # get mathjax
  fetchcontentcached(
    NAME mathjax
    URL https://github.com/mathjax/MathJax/archive/refs/tags/3.2.2.zip
    URL_HASH SHA256=95a32928f18ec9170e7f16cab77709cc58a43a8633486288cbbd599cdf625a35
  )

  # https://www.doxygen.nl/manual/emojisup.html
  # Set the URL, destination, and hash for the download
  set(GITHUB_EMOJIS_URL "https://www.doxygen.nl/dl/github_emojis.zip")
  set(GITHUB_EMOJIS_ZIP "${CMAKE_SOURCE_DIR}/lib/github_emojis.zip")
  set(GITHUB_EMOJIS_DIR "${CMAKE_SOURCE_DIR}/lib/github_emojis-src")
  set(GITHUB_EMOJIS_HASH "0dc6f2f19abd5303bf4653a275c0aad734c985e0f81bc39b26a6f27306916423")

  # Download the file only if it doesn't already exist
  if(NOT EXISTS "${GITHUB_EMOJIS_ZIP}")
      message(STATUS "Downloading GitHub emojis...")
      file(DOWNLOAD "${GITHUB_EMOJIS_URL}" "${GITHUB_EMOJIS_ZIP}" EXPECTED_HASH SHA256=${GITHUB_EMOJIS_HASH})
  endif()

  # Verify and unzip the file to the specified directory
  if(EXISTS "${GITHUB_EMOJIS_ZIP}")
      # Create the target directory if it doesn't exist
      file(MAKE_DIRECTORY "${GITHUB_EMOJIS_DIR}")

      # Unzip only if the directory is empty
      file(GLOB EMOJI_FILES "${GITHUB_EMOJIS_DIR}/*")
      if(NOT EMOJI_FILES)
          message(STATUS "Extracting GitHub emojis to ${GITHUB_EMOJIS_DIR}...")
          execute_process(
            COMMAND unzip -q -o "${GITHUB_EMOJIS_ZIP}" -d "${GITHUB_EMOJIS_DIR}"
            WORKING_DIRECTORY "${GITHUB_EMOJIS_DIR}"
          )
      else()
          message(STATUS "GitHub emojis already extracted.")
      endif()
  else()
      message(FATAL_ERROR "Failed to download or verify github_emojis.zip.")
  endif()


  # Set source and destination paths
  set(MATHJAX_SRC "${FETCHCONTENT_BASE_DIR}/mathjax-src")
  set(MATHJAX_DST "${CMAKE_CURRENT_BINARY_DIR}/html/mathjax")

  # arrange to copy mathjax into the html folder
  add_custom_target(copy_mathjax ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${MATHJAX_DST}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MATHJAX_DST}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MATHJAX_SRC} ${MATHJAX_DST}
    DEPENDS ${MATHJAX_SRC}
  )

  # Ensure docs directory exists before copy
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html")

  # Define path for .nojekyll file
  set(NOJEKYLL_FILE "${CMAKE_CURRENT_BINARY_DIR}/html/.nojekyll")

  # Add a custom target to touch .nojekyll
  add_custom_target(touch_nojekyll ALL
    COMMAND ${CMAKE_COMMAND} -E touch ${NOJEKYLL_FILE}
  )

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
   # COMMENT "Generating API documentation with Doxygen ${DOXYGEN_EXECUTABLE}"
    VERBATIM
    DEPENDS "${CMAKE_BINARY_DIR}/h/config.hpp"
  )

  # Make sure the documentation target depends on DoxygenProject if we built it
#  if(TARGET DoxygenProject)
#    add_dependencies(docs DoxygenProject)
#  endif()

  # make sure we put mathjax in the target folder and 
  # make sure there's an empty .nojekyll file in the root of our html
  # to speed up gh_pages load
  add_dependencies(docs copy_mathjax touch_nojekyll)

  # Note: we can't copy the trace.json in right now, because we're busy actually
  # doing the thing being traced! So it is copied by the GNUmakefile outside of cmake.
endif()

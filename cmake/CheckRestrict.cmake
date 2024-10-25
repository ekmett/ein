# CheckRestrict.cmake - A reusable CMake module to detect the correct restrict keyword.

# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

include(CheckCXXSourceCompiles)

function(check_quiet_cxx_source_compiles source result_var)
    # Create a temporary file to hold the source code
    file(WRITE "${CMAKE_BINARY_DIR}/check_quiet_compile.cpp" "${source}")

    # Now use try_compile with the temporary source file
    try_compile(${result_var} ${CMAKE_BINARY_DIR}
        SOURCES "${CMAKE_BINARY_DIR}/check_quiet_compile.cpp"
        OUTPUT_VARIABLE QUIET_COMPILE_OUTPUT
        COPY_FILE ON
    )
endfunction()

# Check for "restrict"
check_quiet_cxx_source_compiles("
  int main() {
    int * restrict p;
    return 0;
  }
" HAS_RESTRICT)

# Check for "__restrict"
check_quiet_cxx_source_compiles("
  int main() {
    int * __restrict p;
    return 0;
  }
" HAS___RESTRICT)

# Check for "__restrict__"
check_quiet_cxx_source_compiles("
  int main() {
    int * __restrict__ p;
    return 0;
  }
" HAS___RESTRICT__)

# Set the result in a CMake variable
if (HAS_RESTRICT)
  set(RESTRICT_KEYWORD "restrict")
elseif (HAS___RESTRICT)
  set(RESTRICT_KEYWORD "__restrict")
elseif (HAS___RESTRICT__)
  set(RESTRICT_KEYWORD "__restrict__")
else()
  set(RESTRICT_KEYWORD "")
endif()


# CheckRestrict.cmake - A reusable CMake module to detect the correct restrict keyword.

# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

include(CheckCXXSourceCompiles)

# Check for "restrict"
check_cxx_source_compiles("
  int main() {
    int * restrict p;
    return 0;
  }
" HAS_RESTRICT)

# Check for "__restrict"
check_cxx_source_compiles("
  int main() {
    int * __restrict p;
    return 0;
  }
" HAS___RESTRICT)

# Check for "__restrict__"
check_cxx_source_compiles("
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


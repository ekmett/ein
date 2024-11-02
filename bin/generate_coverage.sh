#!/bin/bash
# generate_coverage.sh

# Arguments
OBJ_DIR=$1
PROFRAW_FILE=$2

# Create temporary files for intermediate results
COVERAGE_PROFDATA=$(mktemp)
COVERAGE_INFO=$(mktemp)

# Function to clean up temporary files on exit
cleanup() {
  rm -f "$COVERAGE_PROFDATA" "$COVERAGE_INFO"
}
trap cleanup EXIT

# Run `find` to locate .o files
OBJECT_FILES=$(find "$OBJ_DIR/src" -name '*.cpp.o' -o -name '*.cxx.o' -print)

# Directory containing bitcode object files (pass as first argument)
BUILD_DIR="$OBJ_DIR/src"

# Find all .o files (bitcode format) and convert them to native object files
echo "$OBJECT_FILES" | while read -r bitcode_file; do
    native_file="${bitcode_file%.o}_native.o"
    echo "Converting $bitcode_file to $native_file"
    llc "$bitcode_file" -filetype=obj -o "$native_file"
done

OBJECT_FILES=$(find "$OBJ_DIR/src" -name '*_native.o' -print)

# Merge profraw data
llvm-profdata-19 merge -sparse "$PROFRAW_FILE" -o coverage.profdata

# Run llvm-cov to generate coverage information in lcov format
llvm-cov-19 export --format=lcov --instr-profile=coverage.profdata $OBJECT_FILES > coverage.info

# Generate HTML report
genhtml coverage.info --output-directory "$OBJ_DIR/doc/html/coverage-report"

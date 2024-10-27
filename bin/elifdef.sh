#!/bin/bash

# Check if input was provided as a file argument; if not, use stdin.
if [ -t 0 ] && [ -z "$1" ]; then
  echo "Usage: $0 [file]"
  exit 1
fi

# Use file input if given, otherwise read from stdin
input="${1:-/dev/stdin}"

exec sed -E \
  -e 's/#( *)elifdef (.*)/#\1elif defined\(\2\)/' \
  -e 's/#( *)elifndef (.*)/#\1elif !defined\(\2\)/' \
  < "$input"

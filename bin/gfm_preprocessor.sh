#!/bin/bash

# Check if input was provided as a file argument; if not, use stdin.
if [ -t 0 ] && [ -z "$1" ]; then
  echo "Usage: $0 [file]"
  exit 1
fi

# Use file input if given, otherwise read from stdin
input="${1:-/dev/stdin}"

# Perform replacements for GitHub-flavored markdown
# also look for <!-- GITHUB -->
exec sed -E \
  -e 's/<!-- GITHUB//g' \
  -e 's/GITHUB -->//g' \
  -e 's/:white_check_mark:/✓/g' \
  -e 's/:x:/✗/g' \
  -e 's/:warning:/⚠️/g' \
  -e 's/:information_source:/ℹ️/g' \
  -e 's/:memo:/📝/g' \
  -e 's/:rocket:/🚀/g' \
  -e 's/:bulb:/💡/g' < "$input"

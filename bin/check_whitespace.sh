#!/bin/bash

# Default to checking only staged changes unless "unstaged" is passed
CHECK_SCOPE=${1:-"unstaged"}  # Accepts "staged" or "unstaged" as argument, defaults to "unstaged"

# Set the appropriate git diff command based on the scope
if [[ "$CHECK_SCOPE" == "staged" ]]; then
  DIFF_COMMAND="git diff --cached"
elif [[ "$CHECK_SCOPE" == "unstaged" ]]; then
  DIFF_COMMAND="git diff"
else
  echo "Invalid argument. Use 'unstaged' to check unstaged files, or leave blank for staged only."
  exit 1
fi

# Get a list of text files that are modified, excluding binaries and specified paths
text_files=$($DIFF_COMMAND --name-only --diff-filter=ACM ':!lib/**' | \
             while read -r file; do
               if [[ "$(git check-attr binary -- "$file" | cut -d: -f3)" != "set" ]]; then
                 echo "$file"
               fi
             done)

# Check for trailing whitespace in the selected files
if [[ -n "$text_files" ]]; then
  $DIFF_COMMAND --check -- $text_files || {
    echo "Trailing whitespace detected in $CHECK_SCOPE text files. Please run 'make format'."
    exit 1
  }
fi

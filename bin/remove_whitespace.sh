#!/bin/bash

# This script recursively scans for source files and removes trailing
# whitespace in-place. It also fixes source files that don't end with
# a newline.

# Set the directory to the current directory if not provided.
DIR=${1:-.}

pushd $DIR > /dev/null

# Skip searching directories in the search root directory named 'gen'
# or 'lib'.

git grep -I --name-only -e . | grep -v -e '^gen/' -e '^lib/' -e '^.ccache/' | while IFS= read -r file; do
    echo "Processing: $file"

    # Remove trailing whitespace in place
    if [[ "$OSTYPE" == "darwin"* ]]; then
      sed -i '' -e 's/[[:space:]]*$//' "$file"
    else
      sed -i -e 's/[[:space:]]*$//' "$file"
    fi

    # Remove blank lines at the end of the file
    # (https://www.pement.org/sed/sed1line.txt)
    if [[ "$OSTYPE" == "darwin"* ]]; then
      sed -i '' -e :a -e '/^\n*$/{$d;N;ba' -e '}' "$file"
    else
      sed -i -e :a -e '/^\n*$/{$d;N;ba' -e '}' "$file"
    fi

    # Check for newline at end of file and add one if missing
    if [ -n "$(tail -c 1 "$file")" ]; then
      echo >> "$file"
    fi

    # Run `nbstripout` on Jupyter notebooks
    if ! command -v nbstripout &> /dev/null; then
        echo "nbstripout is missing. Please run `pip install nbstripout`."
        exit 1
    fi
    if [[ "$file" == *.ipynb ]]; then
        nbstripout "$file"
    fi
done

echo "Trailing whitespace cleanup completed."

popd > /dev/null

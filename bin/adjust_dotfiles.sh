#!/bin/bash

input_dir="$1"
output_dir="$2"

mkdir -p "$output_dir"

for file in "$input_dir"/*; do
    sed -E 's/([a-zA-Z0-9_]+)\\n\(\1::\1\)/\1/g; s/([a-zA-Z0-9_]+)::\1/\1/g' "$file" > "$output_dir/$(basename "$file")"
done

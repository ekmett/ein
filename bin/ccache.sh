#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PARENT_DIR="$(dirname "$SCRIPT_DIR")"

# Set the custom ccache directory to the parent directory of the script
export CCACHE_DIR="$PARENT_DIR/.ccache"

# Check if CCACHE_DEBUG is set; if not, set it and configure the log file
if [[ "${CCACHE_DEBUG:-0}" -eq 1 ]]; then
    export CCACHE_LOGFILE="$CCACHE_DIR/ccache.log"
fi

# Check for custom ccache binary, fall back to system ccache if not found
if [[ -x "$PARENT_DIR/lib/cache/ccache-src/ccache" ]]; then
    CCACHE_BIN="$PARENT_DIR/lib/cache/ccache-src/ccache"
else
    echo "Warning: Updated ccache binary not found. Using system-installed ccache."
    CCACHE_BIN="ccache"
fi

# Run the selected ccache with all arguments passed to the script
exec "$CCACHE_BIN" "$@"

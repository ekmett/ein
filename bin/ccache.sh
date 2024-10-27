#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PARENT_DIR="$(dirname "$SCRIPT_DIR")"

# Set the custom ccache directory to the parent directory of the script
export CCACHE_DIR="$PARENT_DIR/.ccache"

# enable this to see details about ccache hit rates
#export CCACHE_DEBUG=1
#export CCACHE_LOGFILE="$CCACHE_DIR/ccache.log"

# Run ccache with all arguments passed to the script
exec ccache "$@"


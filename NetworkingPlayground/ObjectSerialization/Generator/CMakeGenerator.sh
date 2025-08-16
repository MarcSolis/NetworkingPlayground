#!/usr/bin/env bash

set -e  # exit on error

# === Compute paths ===
SOURCE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$SOURCE_DIR/../Solution"
BUILD_TEST_DIR="$BUILD_DIR/Test"
BUILD_TARGET="all"

if [[ -n "$1" ]]; then
    BUILD_TARGET="$1"
fi

# Normalize paths for Windows Git Bash / MSYS if needed
case "$(uname -s)" in
    MINGW*|MSYS*)
        BUILD_DIR=$(cygpath -m "$BUILD_DIR")
        BUILD_TEST_DIR=$(cygpath -m "$BUILD_TEST_DIR")
        SOURCE_DIR=$(cygpath -m "$SOURCE_DIR")
        ;;
esac

# === MAIN BUILD CHECK ===
if [[ "$BUILD_TARGET" == "all" || "$BUILD_TARGET" == "main" ]]; then
    mkdir -p "$BUILD_DIR"
    cmake -B "$BUILD_DIR" -S "$SOURCE_DIR"
fi

# === TEST BUILD CHECK ===
if [[ "$BUILD_TARGET" == "all" || "$BUILD_TARGET" == "test" ]]; then
    mkdir -p "$BUILD_TEST_DIR"
    cmake -DTEST=ON -B "$BUILD_TEST_DIR" -S "$SOURCE_DIR"
fi

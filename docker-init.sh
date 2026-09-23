#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
LIBRAC5_DIR=${LIBRAC5_DIR:-"$SCRIPT_DIR/../librac5"}

if [ ! -f "$LIBRAC5_DIR/Makefile" ]; then
    echo "Mount the Metroynome folder so sm-cheats and librac5 are both available." >&2
    exit 1
fi

apk add --no-cache make git build-base

BUILD_DIR=$(mktemp -d)
trap 'rm -rf "$BUILD_DIR"' EXIT
git clone --depth 1 --branch main https://github.com/Dnawrkshp/bin2code "$BUILD_DIR/bin2code"
make -C "$BUILD_DIR/bin2code"
make -C "$BUILD_DIR/bin2code" release
make -C "$LIBRAC5_DIR" clean
make -C "$SCRIPT_DIR" clean
make -C "$LIBRAC5_DIR" install

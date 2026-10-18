#!/bin/sh
# Packs the build into dist/clani-<target>.tar.gz for av: the binary at the
# top, with README.md, LICENSE and PATCHES.md.
#
#   asciiverse/package.sh x86_64-unknown-linux-musl
set -eu
cd "$(dirname "$0")/.."
target="$1"
stage=stage/clani
rm -rf "$stage" && mkdir -p "$stage" dist
cp out/clani README.md LICENSE PATCHES.md "$stage/"
tar -czf "dist/clani-$target.tar.gz" -C "$stage" .
ls dist

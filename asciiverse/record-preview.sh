#!/bin/sh
# Records the catalog preview at 100x30 with hub/tools/castrec.py: clani
# started the way av starts it (-f voronoi), with a fixed seed, for about
# 25 s, then a key ends it.
#
#   asciiverse/record-preview.sh out/clani asciiverse/clani.cast
set -eu
binary=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
out=$(pwd)/$2
castrec=$(cd "$(dirname "$0")/../../../hub/tools" && pwd)/castrec.py
home=$(mktemp -d)
trap 'rm -rf "$home"' EXIT
mkdir "$home/bin" && cp "$binary" "$home/bin/clani"
HOME="$home" PATH="$home/bin:$PATH" python3 "$castrec" "$out" "$home" "w25;kq" -- clani -f voronoi -s 2026

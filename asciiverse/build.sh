#!/bin/sh
# Builds clani into out/clani with CMake (only the clani target; the unit
# tests and their Boost dependency stay off). Curses libraries and linker
# flags come from the environment, so one script serves every target (see
# .github/workflows/release.yml):
#
#   asciiverse/build.sh                                           # Linux
#   LDFLAGS="-static -s" asciiverse/build.sh                      # Alpine, static
#   CURSES_LIBS="$nc/lib/libmenuw.a;$nc/lib/libncursesw.a" \
#     CMAKE_ARGS="-DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_CXX_FLAGS=-I$nc/include" \
#     asciiverse/build.sh                                         # macOS, ncurses 6.5 in $nc
set -eu
cd "$(dirname "$0")/.."
JOBS="${JOBS:-2}"
CURSES_LIBS="${CURSES_LIBS:-menuw;ncursesw}"
LDFLAGS="${LDFLAGS:-}"
CMAKE_ARGS="${CMAKE_ARGS:-}"
rm -rf build-av
# shellcheck disable=SC2086
nice -n 10 cmake -S . -B build-av -DCMAKE_BUILD_TYPE=Release \
  -DCLANI_CURSES_LIBS="$CURSES_LIBS" -DCMAKE_EXE_LINKER_FLAGS="$LDFLAGS" $CMAKE_ARGS >/dev/null
nice -n 10 cmake --build build-av --target clani -j "$JOBS"
mkdir -p out
cp build-av/clani out/clani
echo "built clani"

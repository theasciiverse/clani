# Changes from upstream

This is the Asciiverse packaging of [CLiAni](https://github.com/hu-ry/CLiAni)
by Ryan Huth (hury), with resize handling by Sergeant-Salz: endlessly
scrolling noise patterns (Perlin noise, Voronoi, cellular Voronoi) drawn
with ASCII characters, with effects (raining, zig-zag, static) and a menu
to pick them (C++14, ncurses with its menu library, 2020 to 2024).
Upstream is the `upstream` remote; the `asciiverse` branch starts from its
last commit, `14bd726` (2024-03-02). Upstream has no tags or version, so
the version here is 2024.03.02.

## Licence

GPL-3.0-only. LICENSE is the GPL version 3 text and every source file
says "License: GNU GPLv3"; nothing offers "any later version".

## Build

- **Unit tests optional.** Boost is needed only for the unit tests, but
  CMake required it for every build. The tests and the separate `model`
  and `view` programs now build only with `-DCLANI_TESTS=ON`.
- **Curses libraries settable.** `find_library(ncurses 6.4 COMPONENTS
  menu REQUIRED)` was not a valid lookup; clani links `CLANI_CURSES_LIBS`
  (`menuw;ncursesw` by default, `menu;ncurses` on macOS).
- **Optimisation from the build type** instead of a fixed `-O0 -g`;
  `-fcheck-new` only for GCC.
- **Opaque ncurses.** Current ncurses hides the `WINDOW` and `ITEM`
  structures. `_HeaderWindow->_maxy` is `getmaxy()`, `subWin->_parent`
  is `wgetparent()`, and the option values beside the menu items live in
  padded buffers of our own instead of rewriting the item's description
  pointer and length; a new value now shows at once (it used to appear only
  after the next key).
- The fast inverse square root's float/int punning goes through `memcpy`
  (the pointer casts broke strict aliasing; GCC warned that it read the
  value uninitialised once optimised).

## Code

- **Options that skip the menu**: `-f perlin|voronoi|cell`, `-e
  scrolling|raining|zigzag|zigzag-old|static`, `-s seed`, `-c chars`.
  Any of them starts the animation straight away, the rest at their
  defaults and the seed random unless given; `-h` lists them. Without
  options the menu runs as before. av starts `clani -f voronoi`.
- **Any key ends the animation** (it only stopped on Ctrl+C): the frame
  wait is a curses `timeout()` instead of `napms()`.
- **Ctrl+C, SIGTERM and SIGHUP quit, in the menu too** (the menu ignored
  them and waited for a key), and clani exits with status 0 (it exited
  with 2, the number of SIGINT).
- **Rows fit the terminal.** Each row is 120 characters; below 120
  columns it wrapped onto the next line, and above it sat at the left. It
  is now cut to the width, centred on wider terminals, and printed with
  `mvaddnstr` (it went through `printw` as a format string, so a `%` in
  the chosen characters broke it).
- **Resize once per resize.** The resize flag was never cleared, so every
  frame after the first resized the screen.
- **The menu's seed is read properly.** The parser assigned instead of
  adding and skipped the first digit, so the default seed "1" gave 0 and
  "123" gave 20.
- **Enter ends the variation input** with the characters typed so far, the
  last one filling the rest (it waited for all ten). The seed and
  variation fields start from the current values (the variation field was
  uninitialised until typed).
- **Zig-zag (old) read past its table** (ASan): four of its neighbour
  lookups were missing the wrap-around. `delete[]` for an array made with
  `new[]`.

## Packaging (new files)

- `asciiverse/build.sh` (CMake, clani target only, `JOBS=2`),
  `asciiverse/package.sh`: one archive per target with the binary,
  README.md, LICENSE and this file.
- `.github/workflows/release.yml`: static Linux in Alpine (x86_64,
  aarch64) and macOS (x86_64, arm64), plus SHA256SUMS, on our tags
  (`*-av.*`). No Windows build: POSIX signals and `ioctl`, and PDCurses
  has no menu library.
- `asciiverse/record-preview.sh`, `asciiverse/clani.cast`: the catalog
  preview, 100x30, 25 s of `clani -f voronoi -s 2026` (hub's
  `castrec.py`).
- `.gitignore`: the build folders.

## Tested

Linux: native build; every flavour in direct mode at 100x30; the menu
driven in tmux at 120x40 (flavour, seed and variation set, start, a key
quits with status 0); SIGTERM in the menu quits with status 0; resized to
60x15 and 140x40 during the animation (cut, then centred). ASan and UBSan:
every effect for 12 to 30 s and a scripted pass through the menu were
clean after the zig-zag fix. macOS: not built here.

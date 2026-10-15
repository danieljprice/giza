<!-- Copyright (c) 2026 Daniel Price. Licensed under the GNU Lesser General Public License (LGPL, see COPYING.LESSER) -->
Changes in v2.0.2
-----------------
- fixed X11 BadAccess/BadShmSeg errors and missing images on the /xw device on macOS with the XQuartz cairo, by disabling cairo's MIT-SHM uploads there
- added missing copyright statements to test files and README.md (#128; thanks to Joachim Frieben)
- fixed compiler warnings (#129, #130)

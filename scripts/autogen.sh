#!/bin/sh
# Regenerate configure, Makefile.in, etc. after editing configure.ac or Makefile.am.
# Commit the updated files; CI uses the pre-generated autotools output.
set -e
cd "$(dirname "$0")/.."
autoreconf -fi

#!/usr/bin/env bash
# Run upstream CPAN PGPLOT Perl module smoke tests against an installed giza.
#
# Primary target: Ubuntu CI (after ./configure && make && sudo make install).
# Also works on macOS with Homebrew giza (set PKG_CONFIG_PATH if needed).
#
# Smoke tests use PGPLOT_DEV=/NULL by default: they verify that Perl can call
# PGPLOT routines without crashing. They do NOT check plot appearance.
#
# Environment:
#   PGPLOT_VERSION   CPAN release to test (default: 2.35)
#   PGPLOT_DEV       Graphics device (default: /NULL)
#   PKG_CONFIG_PATH  Where to find giza's pgplot.pc / cpgplot.pc
#   WORKDIR          Build directory (default: mktemp under $TMPDIR)
#   KEEP_WORKDIR     If set to 1, do not delete WORKDIR on exit
#   VERBOSE          If set to 1, show full make output (default: silent make)
#   PGPLOT_SHA256    Expected SHA256 of the CPAN tarball (default: PGPLOT-2.35)
#
# Prerequisites (Ubuntu):
#   sudo apt-get install perl cpanminus libextutils-f77-perl \
#       libdevel-checklib-perl libtest-simple-perl
#
# Prerequisites (macOS Homebrew):
#   brew install perl; cpanm --notest ExtUtils::F77 Devel::CheckLib Test::More
#   export PERL5LIB="$HOME/perl5/lib/perl5${PERL5LIB:+:$PERL5LIB}"

set -euo pipefail

PGPLOT_VERSION="${PGPLOT_VERSION:-2.35}"
PGPLOT_DEV="${PGPLOT_DEV:-/NULL}"
KEEP_WORKDIR="${KEEP_WORKDIR:-0}"
# SHA256 of PGPLOT-2.35.tar.gz from MetaCPAN (update if PGPLOT_VERSION changes).
PGPLOT_SHA256="${PGPLOT_SHA256:-89d18b0157f26c309197411912361357b66e19f978556d0e181019dce754ea3c}"
WORKDIR_CREATED_BY_SCRIPT=0

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PATCH_FILE="${SCRIPT_DIR}/patches/perl-pgplot-giza-cpgconx.patch"
CPAN_URL="https://cpan.metacpan.org/authors/id/E/ET/ETJ/PGPLOT-${PGPLOT_VERSION}.tar.gz"
TARBALL="PGPLOT-${PGPLOT_VERSION}.tar.gz"
SRCDIR="PGPLOT-${PGPLOT_VERSION}"

cleanup() {
  if [[ "${KEEP_WORKDIR}" != "1" && "${WORKDIR_CREATED_BY_SCRIPT}" == "1" \
        && -n "${WORKDIR:-}" && -d "${WORKDIR}" ]]; then
    rm -rf "${WORKDIR}"
  fi
}
trap cleanup EXIT

# ---------------------------------------------------------------------------
# Locate giza via pkg-config (works on Ubuntu /usr/local and Homebrew)
# ---------------------------------------------------------------------------
find_giza_paths() {
  local pc_paths
  pc_paths="/usr/local/lib/pkgconfig:/usr/local/lib64/pkgconfig:/usr/lib/pkgconfig"
  if [[ "$(uname -s)" == "Darwin" ]]; then
    pc_paths="/opt/homebrew/lib/pkgconfig:/usr/local/lib/pkgconfig:${pc_paths}"
  fi
  export PKG_CONFIG_PATH="${PKG_CONFIG_PATH:-${pc_paths}}"

  if ! command -v pkg-config >/dev/null 2>&1; then
    echo "error: pkg-config is required but not installed." >&2
    exit 1
  fi

  if ! pkg-config --exists cpgplot 2>/dev/null; then
    echo "error: cpgplot not found via pkg-config." >&2
    echo "  Install giza first (make install) and set PKG_CONFIG_PATH if needed." >&2
    exit 1
  fi

  GIZA_LDIR="$(pkg-config --variable=libdir cpgplot)"
  GIZA_IDIR="$(pkg-config --variable=includedir cpgplot)"
  GIZA_VERSION="$(pkg-config --modversion cpgplot)"
  echo "Found giza ${GIZA_VERSION} (libs: ${GIZA_LDIR}, headers: ${GIZA_IDIR})"
}

# ---------------------------------------------------------------------------
# Perl tooling
# ---------------------------------------------------------------------------
check_perl_deps() {
  if ! command -v perl >/dev/null 2>&1; then
    echo "error: perl not found" >&2
    exit 1
  fi
  if ! command -v prove >/dev/null 2>&1; then
    echo "error: prove not found (install perl core or libtest-simple-perl)" >&2
    exit 1
  fi
  perl -MExtUtils::F77 -MDevel::CheckLib -MTest::More -e 1 2>/dev/null || {
    echo "error: missing Perl modules (ExtUtils::F77, Devel::CheckLib, Test::More)" >&2
    echo "  Ubuntu: sudo apt-get install libextutils-f77-perl libdevel-checklib-perl libtest-simple-perl" >&2
    echo "  macOS:  cpanm --notest ExtUtils::F77 Devel::CheckLib Test::More" >&2
    exit 1
  }
}

# ---------------------------------------------------------------------------
# Download, patch, build
# ---------------------------------------------------------------------------
verify_tarball_sha256() {
  local file="$1" expected="$2"
  echo "Verifying SHA256 of ${file}..."
  if command -v sha256sum >/dev/null 2>&1; then
    echo "${expected}  ${file}" | sha256sum -c -
  else
    echo "${expected}  ${file}" | shasum -a 256 -c -
  fi
}

prepare_source() {
  if [[ -z "${WORKDIR:-}" ]]; then
    WORKDIR="$(mktemp -d "${TMPDIR:-/tmp}/pgplot-perl-XXXXXX")"
    WORKDIR_CREATED_BY_SCRIPT=1
  fi
  echo "Working directory: ${WORKDIR}"
  cd "${WORKDIR}"

  if [[ ! -f "${TARBALL}" ]]; then
    echo "Downloading PGPLOT-${PGPLOT_VERSION} from CPAN..."
    curl -fsSL "${CPAN_URL}" -o "${TARBALL}"
  fi
  verify_tarball_sha256 "${TARBALL}" "${PGPLOT_SHA256}"

  if [[ ! -d "${SRCDIR}" ]]; then
    tar xzf "${TARBALL}"
  fi

  if [[ ! -f "${PATCH_FILE}" ]]; then
    echo "error: patch not found: ${PATCH_FILE}" >&2
    exit 1
  fi

  echo "Applying giza compatibility patch..."
  cd "${SRCDIR}"
  patch -p1 --forward -r - < "${PATCH_FILE}" || {
    # patch returns 1 if already applied; check marker
    if ! grep -q 'giza (and modern cpgplot.h)' pgfun.c; then
      echo "error: failed to apply ${PATCH_FILE}" >&2
      exit 1
    fi
    echo "Patch already applied."
  }
}

build_module() {
  cd "${WORKDIR}/${SRCDIR}"

  echo "Configuring Perl PGPLOT against giza..."
  perl Makefile.PL "LDIR=${GIZA_LDIR}" "IDIR=${GIZA_IDIR}"

  echo "Building Perl PGPLOT..."
  local make_silent="-s"
  if [[ "${VERBOSE:-0}" == "1" ]]; then
    make_silent=""
  fi
  if ! make ${make_silent} 2>&1; then
    echo "error: make failed" >&2
    exit 1
  fi

  export PERL5LIB="${PWD}/blib/lib:${PWD}/blib/arch${PERL5LIB:+:${PERL5LIB}}"
}

run_tests() {
  cd "${WORKDIR}/${SRCDIR}"

  echo "Running smoke tests (PGPLOT_DEV=${PGPLOT_DEV})..."
  # t/t1.t–t/t11.t exercise the routines from Karl's perl-pgplot reports.
  # t/t12.t skips with giza (non-numeric version string).
  # t/lut.t and t/pdl-graphics.t skip without PDL (optional).
  PGPLOT_DEV="${PGPLOT_DEV}" prove -v t/
}

# ---------------------------------------------------------------------------
main() {
  find_giza_paths
  check_perl_deps
  prepare_source
  build_module
  run_tests
  echo "perl-PGPLOT tests passed."
}

main "$@"

AC_PREREQ(2.61)
m4_define(giza_version_major,0)
m4_define(giza_version_minor,9)
m4_define(giza_version_micro,4)
AC_INIT([giza],
        [giza_version_major.giza_version_minor.giza_version_micro],
        [daniel.price@monash.edu],[],
        [http://giza.sf.net])
AC_CONFIG_AUX_DIR([build])
AM_INIT_AUTOMAKE([1.11 -Wall -Werror -Wno-extra-portability])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIRS([m4])
AM_MAINTAINER_MODE([disable])
AC_PROG_CC
AC_PROG_FC
AC_CONFIG_HEADERS([config.h])
AC_SUBST(GIZA_VERSION_MAJOR,giza_version_major)
AC_SUBST(GIZA_VERSION_MINOR,giza_version_minor)
AC_SUBST(GIZA_VERSION_MICRO,giza_version_micro)
#
# The following specify the interface version numbers for giza.
# LT_CURRENT is the current interface number
# LT_REVISION is the implementation number of the current interface
# LT_AGE is the difference between the newest and oldest interface implemented
#
# Rules are:
# 1) update LT_REVISION for every public giza release
# 2) update LT_CURRENT if giza interface updated and set LT_REVISION to 0
# 3) increment LT_AGE if interface is backwards compatible
# 4) set LT_AGE to 0 if interface is not backwards compatible
#
AC_SUBST(LT_CURRENT, 1)
AC_SUBST(LT_REVISION, giza_version_micro)
AC_SUBST(LT_AGE, 1)
LT_INIT([shared])
#
# cairo and X11 libraries. Use pkg-config if available but provide fallback options if not (OS/X)
#
x11_libs="-L/usr/X11/lib -lX11"
cairo_libs_osx="-L/usr/X11/lib -lcairo"
cairo_cflags_osx="-I/usr/X11/include"
PKG_CHECK_MODULES([CAIRO], [cairo],[],[AC_SUBST(CAIRO_LIBS,$cairo_libs_osx) AC_SUBST(CAIRO_CFLAGS,$cairo_cflags_osx)])
PKG_CHECK_MODULES([X11], [x11], [], [AC_SUBST(X11_LIBS,$x11_libs)])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
 src/cpgplot.pc
 src/giza.pc
 src/pgplot.pc
 src/giza-version.h
 test/Makefile
 test/C/Makefile
 test/F90/Makefile
])
AC_OUTPUT

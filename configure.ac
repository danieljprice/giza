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
AM_SILENT_RULES([yes])
AC_CONFIG_MACRO_DIRS([m4])
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
PKG_CHECK_MODULES([CAIRO], [cairo])
PKG_CHECK_MODULES([X11], [x11])
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

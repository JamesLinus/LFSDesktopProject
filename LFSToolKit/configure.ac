#
#

m4_define([LFSTOOLKIT_MAJ_N],[0])
m4_define([LFSTOOLKIT_MIN_N],[0])
m4_define([LFSTOOLKIT_REV_N],[1])

AC_INIT([LFSToolKit],[0.0.1],[PROJ],[LFSToolKit])

AC_CONFIG_HEADER([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC
LT_INIT([disable-static])
AC_DISABLE_STATIC

# libtool
LFSTOOLKIT_MINOR=LFSTOOLKIT_MIN_N()
LFSTOOLKIT_INTERFACE=$(( LFSTOOLKIT_MAJ_N() + ${LFSTOOLKIT_MINOR} ))
LFSTOOLKIT_REVISION=LFSTOOLKIT_REV_N()
LFSTOOLKIT_LIBTOOL_VERSION=$LFSTOOLKIT_INTERFACE:$LFSTOOLKIT_REVISION:$LFSTOOLKIT_MINOR

AC_SUBST(LFSTOOLKIT_LIBTOOL_VERSION)

AC_OUTPUT(Makefile LFSToolKit/app/Makefile)

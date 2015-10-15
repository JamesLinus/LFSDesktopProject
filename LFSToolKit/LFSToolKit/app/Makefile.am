AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources
include ../headers

include_HEADERS = $(APIHEADERS)

lib_LTLIBRARIES = liblfstoolkit.la
liblfstoolkit_la_SOURCES = $(COMMONSRC)

liblfstoolkit_la_CXXFLAGS = -Wl,--version-script=../src/exportmap
liblfstoolkit_la_LDFLAGS = -version-info $(LFSTOOLKIT_LIBTOOL_VERSION)

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

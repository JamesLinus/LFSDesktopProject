AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = liblfstoolkit.la

liblfstoolkit_la_SOURCES = ../src/lfstoolkit.cpp
liblfstoolkit_la_CXXFLAGS =  -Wl,--version-script=../src/exportmap
liblfstoolkit_la_LDFLAGS = -version-info $(LFSTOOLKIT_LIBTOOL_VERSION)




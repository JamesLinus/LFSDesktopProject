AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources


include_HEADERS = ../src/sessionmanager.h
lib_LTLIBRARIES = liblfstoolkit.la
liblfstoolkit_la_SOURCES = $(COMMONSRC)
liblfstoolkit_la_LDFLAGS = -no-undefined  -version-info $(LFSTOOLKIT_LIBTOOL_VERSION) \
 -export-symbols /media/LinuxData/Development/Projects/LFSDesktopProject/LFSToolKit/LFSToolKit/app/exportmap

AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

xfwmanager_SOURCES = $(COMMONSRC)

xfwmanager_CFLAGS = -O3 -export-dynamic
xfwmanager_CXXFLAGS = -O3 -export-dynamic

defaultsdir = $(pkgdatadir)
defaults_DATA = ../resources/defaults

bin_PROGRAMS = xfwmanager

man1_MANS = ../resources/man/xfwmanager.1

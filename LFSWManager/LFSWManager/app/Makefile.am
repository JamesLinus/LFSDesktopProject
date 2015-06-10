AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

lfswmanager_SOURCES = $(COMMONSRC)

lfswmanager_CFLAGS = -O3 -export-dynamic
lfswmanager_CXXFLAGS = -O3 -export-dynamic

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

defaultsdir = $(pkgdatadir)
defaults_DATA = ../resources/defaults

bin_PROGRAMS = lfswmanager

man1_MANS = ../resources/man/lfswmanager.1



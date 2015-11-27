AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources
include ../headers

include_HEADERS = $(APIHEADERS)

lib_LTLIBRARIES = liblfstoolkit.la
liblfstoolkit_la_SOURCES = $(COMMONSRC)

pixfilesdir = ${prefix}/share/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

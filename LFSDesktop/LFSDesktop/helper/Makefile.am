AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs

lfshelper_SOURCES = lfshelper.cpp

lfshelper_CFLAGS = -O3

helperdir = $(libexecdir)/LFSDesktop
helper_PROGRAMS = lfshelper

install-exec-hook:
	sudo chown root:root $(DESTDIR)${prefix}/${libexecdir}/LFSDesktop/lfshelper
	sudo chmod ug+s $(DESTDIR)${prefix}/${libexecdir}/LFSDesktop/lfshelper

PACKAGENAME=LFSToolKit
LIBNAME=lfstoolkit
VERSION=0.0.1
HEADERS=lfstoolkit.h
PC=$(LIBNAME).pc
INLUDEDIR=$(PREFIX)/include
include sources

LIBDIRSUFFIX=64
PREFIX=/usr
LIBDIR=$(PREFIX)/lib$(LIBDIRSUFFIX)
PKCONFIGDIR=$(LIBDIR)/pkgconfig

CFLAGS=
LIBS=

all:
	gcc -fPIC -Wall -g -c src/lfstoolkit.cpp  -o $(LIBNAME).o
	gcc -shared -Wl,--version-script=exportmap,-soname,lib$(LIBNAME).so -o lib$(LIBNAME).so.$(VERSION) $(LIBNAME).o -lc

clean: distclean

distclean:
	rm lib$(LIBNAME).so.$(VERSION)||true
	rm $(LIBNAME).o||true

install:
		mkdir -vp $(DESTDIR)$(LIBDIR) || true
		cp -v lib$(LIBNAME).so.$(VERSION) $(DESTDIR)$(LIBDIR)
		mkdir -vp $(DESTDIR)$(INLUDEDIR) $(DESTDIR)$(PKCONFIGDIR)|| true
		cp -v $(PC) $(DESTDIR)$(PKCONFIGDIR)
		cp -v src/$(HEADERS) $(DESTDIR)$(INLUDEDIR)

uninstall:
	rm -rv $(DESTDIR)$(LIBDIR)/lib$(LIBNAME).so.$(VERSION) $(DESTDIR)$(INLUDEDIR)/$(HEADERS) $(DESTDIR)$(PKCONFIGDIR)/$(PC)


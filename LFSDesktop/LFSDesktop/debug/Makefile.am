

include ../flagsandlibs
include ../sources

lfsdesktopdebug_SOURCES = $(COMMONSRC)

lfsdesktopdebug_CFLAGS = -ggdb -O0

bin_PROGRAMS = lfsdesktopdebug

install-exec-local:
	cd $(DESTDIR)$(bindir) && \
	rm lfsdesktopdebug


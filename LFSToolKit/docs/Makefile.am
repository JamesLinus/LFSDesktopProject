if HAVE_DOXYGEN

all:
	$(DOXYGEN) Doxyfile

install-data-hook:
	$(INSTALL) -d $(DESTDIR)$(docdir)
	cp -rvp html $(DESTDIR)$(docdir)

endif 
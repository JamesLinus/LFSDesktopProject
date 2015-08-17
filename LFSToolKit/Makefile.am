
SUBDIRS = LFSToolKit/app

.PHONY: docs

docs:
	cd docs && $(MAKE)

clean-docs:
	rm -r docs/html

docs-install:
	$(INSTALL) -d $(DESTDIR)$(docdir)
	cp -rvp docs/html/* $(DESTDIR)$(docdir)

remake:
	$(shell ./remakesourcelist)



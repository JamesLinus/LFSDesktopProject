
SUBDIRS = LFSApplications/app

install-exec-hook:
	mkdir -vp $(DESTDIR)${datadir}/applications $(DESTDIR)${datadir}/pixmaps
	cp LFSApplications/resources/desktopdata/lfs*.desktop $(DESTDIR)${datadir}/applications
	cp LFSApplications/resources/desktopdata/lfs*.png $(DESTDIR)${datadir}/pixmaps
	
uninstall-hook:
	rm $(DESTDIR)${datadir}/applications/lfs*.desktop $(DESTDIR)${datadir}/pixmaps/lfs*.png


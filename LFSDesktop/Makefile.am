
SUBDIRS = LFSDesktop/app LFSDesktop/helper

remake:
	$(shell ./remakesourcelist)

install-exec-hook:
	sudo chown root:root $(DESTDIR)${libexecdir}/LFSDesktop/lfshelper
	sudo chmod ug+s $(DESTDIR)${libexecdir}/LFSDesktop/lfshelper

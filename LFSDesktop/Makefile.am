
SUBDIRS = LFSDesktop/app LFSDesktop/helper
#WHO = $(origin UID)
#$(warning $(FOO) $(origin FOO)) 
#MY_VAR = $(shell  whoami) 
#IAM = keithhedger
#TARGET_CPU = i386
#ifeq ($(MY_VAR),$(IAM))
#DOCHOWN = 1
#endif

remake:
	$(shell ./remakesourcelist)

install-exec-hook:
	$(shell if [ $$UID = 1000 ];then chown root:root $(DESTDIR)${libexecdir}/LFSDesktop/lfshelper;fi)
#	ifeq ($(TARGET_CPU),x86)
#	TARGET_CPU_IS_X86 := 1
#	else ifeq ($(TARGET_CPU),x86_64)
#	TARGET_CPU_IS_X86 := 1
#	else
#	TARGET_CPU_IS_X86 := 0
#	endif
#	WHO=`echo $$UID`
#	#who = `sh echo $UID`
#	$(warning $(WHO) $(origin WHO)) 
#	ifeq ($(MY_VAR),$(IAM))
#	shell echo "XXXXXXXXXXX"
#	endif
#	echo $(WHO)
#	if $(WHO) = 0
#	ifeq (0,$(shell(echo $(WHO))))
#	echo "xxx"
#	endif
#	if `echo $$UID` = 0
#		chown root:root $(DESTDIR)${libexecdir}/LFSDesktop/lfshelper
#	endif
	chmod ug+s $(DESTDIR)${libexecdir}/LFSDesktop/lfshelper
#	else
#	echo "Must be root ...."
#	fi

#
# GENESIS/XODUS : network simulator
#

INSTALL_STARTUPS	=	$(INSTALLDIR)/startup

OBJS = escapelist.g grey hot rainbow rainbow2 redhot minschedule.g schedule.g xmacros.g xstartup.g xtools.g Xdefaults.example

#==========================================================================
default: 
	@echo "nothing to really make in startup"

clean:

rcsclean:
	rcsclean
	co -r$(RCSRELEASE) Makefile $(OBJS) simrc nxsimrc minsimrc Usermake Libmake

install:
	- cp $(OBJS) $(INSTALL_STARTUPS) 
	- ./configrc simrc $(INSTALLDIR) > $(INSTALLDIR)/.simrc 
	- ./configrc nxsimrc $(INSTALLDIR) > $(INSTALLDIR)/.nxsimrc 
	- ./configrc minsimrc $(INSTALLDIR) > $(INSTALLDIR)/.minsimrc 
	- cp $(INSTALLDIR)/.simrc $(INSTALL_STARTUPS)/.simrc 
	- cp $(INSTALLDIR)/.nxsimrc $(INSTALL_STARTUPS)/.nxsimrc 
	- cp $(INSTALLDIR)/.minsimrc $(INSTALL_STARTUPS)/.minsimrc 
	- cp Usermake $(INSTALLDIR)/Usermake 
	- cp Libmake $(INSTALLDIR)/Libmake 


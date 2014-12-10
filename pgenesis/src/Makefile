# DO NOT EDIT THIS MAKEFILE

SIMLIB 	= 	$(GEN_ROOT)/lib
SIMSRC 	= 	$(GEN_ROOT)/src
PARDIR	=	./par

CFLAGS	= $(TOPFLAGS)

INSTLIBDIR = $(INST_DIR)/lib/$(PVM_ARCH)
INSTBINDIR = $(INST_DIR)/bin/$(PVM_ARCH)

LIBDIR = ./lib/$(PVM_ARCH)
 
default: clean $(EXECUTABLES) install

install: pgenesis 
	$(MAKE) USELIBS="$(BASELIBS) $(OBJLIBS) $(XODUS)" instlibs
	mv pgenesis $(INSTBINDIR)/pgenesis
	cat liblist > $(INSTLIBDIR)/liblist

nxinstall: nxpgenesis
	$(MAKE) USELIBS="$(BASELIBS) $(OBJLIBS)" instlibs
	mv nxpgenesis $(INSTBINDIR)/nxpgenesis
	cat nxliblist > $(INSTLIBDIR)/nxliblist

mininstall: minpgenesis
	$(MAKE) USELIBS="$(BASELIBS)" instlibs
	mv minpgenesis $(INSTBINDIR)/minpgenesis
	cat minliblist > $(INSTLIBDIR)/minliblist

# this target copies all libraries and executables to the installation tree
instlibs: $(INSTLIBDIR) $(INSTBINDIR) inststartup 
	(for i in $(PARLIBS) $(USELIBS); \
	 do $(CPLIB) $(LIBDIR)/$$i $(INSTLIBDIR)/$$i; done)
	(for i in $(CODE_EXECS); \
	 do cp $(SIMLIB)/$$i $(INSTLIBDIR)/$$i; done)
	cp $(SIMSRC)/libsh $(INSTLIBDIR)/libsh

inststartup:
	-mkdir -p $(INST_DIR)/startup
	(cd startup; $(MAKE) install)

pardefault:
	$(MAKE) USELIBS="$(BASELIBS) $(OBJLIBS) $(XODUS)" \
		EXECNAME=pgenesis LOADLIB=loadlib.o somegenesis

parnxdefault: 
	$(MAKE) USELIBS="$(BASELIBS) $(OBJLIBS)" XLIBS=""\
		EXECNAME=nxpgenesis LOADLIB=nxloadlib.o somegenesis

parmindefault:
	$(MAKE) USELIBS="$(BASELIBS)" XLIBS=""\
		EXECNAME=minpgenesis LOADLIB=minloadlib.o somegenesis

somegenesis: $(LIBDIR) $(LIBDIR)/$(PARLIBS) $(LOADLIB)
	(for i in $(USELIBS); do \
	 if [ -f $(LIBDIR)/$$i ]; then \
		find $(SIMLIB)/$$i -newer $(LIBDIR)/$$i \
			 -exec $(CPLIB) $(SIMLIB)/$$i $(LIBDIR)/$$i \; ; \
	 else \
		$(CPLIB) $(SIMLIB)/$$i $(LIBDIR)/$$i; \
	 fi; \
	 done )
	@if [ $(PVM_ARCH) = SUN4 ];  then \
	  echo "(cd $(LIBDIR); ranlib *.a)" ; \
	  (cd $(LIBDIR); ranlib *.a); \
	fi
	$(CC) $(CFLAGS) -o $(EXECNAME) $(LOADLIB) \
          `echo $(USELIBS) $(PARLIBS) | awk '{ for (i = 1; i <= NF; i++) printf "$(LIBDIR)/"$$i " " }'` \
 $(XLIBS) $(EXT_LIBS)
	rm -f $(USELIBS) $(PARLIBS)

loadlib.c: liblist
	$(SIMSRC)/libsh < liblist > loadlib.c

nxloadlib.c: nxliblist
	$(SIMSRC)/libsh < nxliblist > nxloadlib.c

minloadlib.c: minliblist
	$(SIMSRC)/libsh < minliblist > minloadlib.c

liblistfilter: excludedliblist
	awk '{ print "$$" "1 == \"" $$1 "\" {next}"}' excludedliblist >liblistfilter
	echo '{print}' >>liblistfilter

liblist: $(SIMSRC)/liblist liblistfilter Makefile
	awk -f liblistfilter $(SIMSRC)/liblist >liblist
	(for i in $(PARDIR); do $(MAKE) -f $$i/Makefile -p -q clean | grep "LIBRARY_NAME *=" | sed s/"LIBRARY_NAME *= *"// >> liblist; done)

nxliblist: $(SIMSRC)/nxliblist liblistfilter Makefile
	awk -f liblistfilter $(SIMSRC)/nxliblist >nxliblist
	(for i in $(PARDIR); do $(MAKE) -f $$i/Makefile -p -q clean | grep "LIBRARY_NAME *=" | sed s/"LIBRARY_NAME *= *"// >> nxliblist; done)

minliblist: $(SIMSRC)/minliblist liblistfilter Makefile
	awk -f liblistfilter $(SIMSRC)/minliblist >minliblist
	(for i in $(PARDIR); do $(MAKE) -f $$i/Makefile -p -q clean | grep "LIBRARY_NAME *=" | sed s/"LIBRARY_NAME *= *"// >> minliblist; done)


$(LIBDIR)/$(PARLIBS): parlibs
	 if [ -f $(LIBDIR)/$(PARLIBS) ]; then \
		find $(PARDIR)/$(PARLIBS) -newer $(LIBDIR)/$(PARLIBS) \
			 -exec $(CPLIB) $(PARDIR)/$(PARLIBS) $(LIBDIR)/$(PARLIBS) \; ; \
	 else \
		$(CPLIB) $(PARDIR)/$(PARLIBS) $(LIBDIR)/$(PARLIBS); \
	 fi

parlibs:
	(for i in $(PARDIR); do cd $$i; $(MAKE); cd ..; done)

install-libs: $(LIBDIR) parlibs
	(cd $(LIBDIR); rm -f $(BASELIBS))
	(for i in $(PARLIBS); do $(CPLIB) $(PARDIR)/$$i $(LIBDIR)/$$i; done)
	(for i in $(BASELIBS); do ln -s $(SIMLIB)/$$i $(LIBDIR)/$$i; done)
	(for i in $(OBJLIBS); do ln -s $(SIMLIB)/$$i $(LIBDIR)/$$i; done)
	(for i in $(XODUS); do ln -s $(SIMLIB)/$$i $(LIBDIR)/$$i; done)

$(LIBDIR):
	-@mkdir -p $(LIBDIR)

clean:
	@(for i in $(PARDIR); do echo cd $$i; cd $$i; make clean; cd ..;done)
	-rm -f $(LIBDIR)/*.o $(LIBDIR)/*.a
	-rm -f *loadlib.*
	-rm -f liblistfilter minliblist nxliblist liblist

spotless: clean
	-rm -rf lib
	-rm -f *loadlib.* pgenesis minpgenesis nxpgenesis
	-rm -f *~ #*






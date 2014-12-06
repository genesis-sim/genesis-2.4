# $Id: Makefile,v 1.2 2005/08/04 19:58:47 svitak Exp $
# $Log: Makefile,v $
# Revision 1.2  2005/08/04 19:58:47  svitak
# Removed references and files related to fortran and c++ modules not used by
# Genesis.
#
# Revision 1.1.1.1  2005/06/14 04:38:29  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.3  2000/06/22 22:54:15  mhucka
# Removed directives that built the FORTRAN and C++ portions.
# GENESIS doesn't use them.
#

include macros.make


PACKAGE		= netcdf
SUBDIRS		= libsrc nctest nc_test ncdump ncgen man 
DIST_GARBAGE	= \
		config.cache	\
		config.status	\
		MANIFEST
PACKING_LIST	= \
		INSTALL		\
		COMPATIBILITY	\
		MANIFEST	\
		Makefile	\
		README		\
		VERSION		\
		aclocal.m4	\
		configure	\
		configure.in	\
		macros.make.in	\
		macros.make.def	\
		rules.make


all:		libsrc/all	\
		ncdump/all	\
		ncgen/all

test:		libsrc/test \
		nctest/test nc_test/test \
		ncdump/test ncgen/test

check:		test

install:	libsrc/install \
		ncdump/install \
		ncgen/install \
		$(MANDIR)/$(WHATIS)

clean:		libsrc/clean \
		nctest/clean nc_test/clean \
		ncdump/clean ncgen/clean \
		clean_config

distclean:	libsrc/distclean \
		nctest/distclean nc_test/distclean \
		ncdump/distclean ncgen/distclean \
		clean_macros 

clean_config:
	-rm -f config.cache

clean_macros:
	-cp macros.make.def macros.make

libsrc/all	\
libsrc/test	\
libsrc/install	\
libsrc/clean	\
libsrc/distclean	\
nctest/all	\
nctest/test	\
nctest/clean	\
nctest/distclean	\
nc_test/all	\
nc_test/test	\
nc_test/clean	\
nc_test/distclean	\
ncdump/all	\
ncdump/test	\
ncdump/install	\
ncdump/clean	\
ncdump/distclean	\
ncgen/all	\
ncgen/test	\
ncgen/install	\
ncgen/clean	\
ncgen/distclean	\
:
	@subdir=`echo $@ | sed 's,/.*,,'`; \
	target=`echo $@ | sed 's,.*/,,'`; \
	$(MAKE) SUBDIR=$$subdir TGET=$$target subdir_target

subdir_target:
	@echo ""
	@cd $(SUBDIR) && \
	    echo "Making \`$(TGET)' in directory `pwd`" && \
	    echo "" && \
	    $(MAKE) $(TGET) || exit 1
	@echo ""
	@echo "Returning to directory `pwd`"
	@echo ""


################################################################################
# Distribution:

# The following rule checks to see that were on the right system.  If we're
# not, then the distribution mightn't contain the intended ncgen(1) (not
# to mention that dvips(1) is necessary for building the C++ User's Guide).
#
check_system:	FORCE
	@case "$(OVERRIDE)" in \
	'') case `uname -sr` in \
	    'SunOS 5'*) \
		exit 0;; \
	    *)  echo 1>&2 "Error: Not on a SunOS 5 system."; \
		echo 1>&2 "Set macro OVERRIDE to override."; \
		exit 1;; \
	    esac;; \
	*) exit 0;; \
	esac

# Make a compressed, tar(1) file of the source distribution in the current 
# directory.
#
tar.Z:		check_system FORCE
	@version=`cat VERSION`; \
	    $(MAKE) $(MFLAGS) $(PACKAGE)-$$version.tar.Z VERSION=$$version

$(PACKAGE)-$(VERSION).tar.Z:	ensure_manifest MANIFEST
	id=$(PACKAGE)-$(VERSION) \
	&& rm -rf $$id \
	&& mkdir $$id \
	&& ln -s .. $$id/src \
	&& tar $(TARFLAGS) - `sed "s|^|$$id/src/|" MANIFEST` | compress > $@ \
	&& rm -r $$id

MANIFEST:	FORCE
	$(MAKE) MANIFEST.echo >$@

# Make a compressed, tar(1) file of the source distribution in the
# appropriate FTP directory.
#
# NOTE: Making "ftp" will cause the "tar.Z" file to be made even if it
# already exists.  This is because the MANIFEST file upon which it
# depends must be remade every time.  This is not a waste of time,
# however, if the "tar.Z" target is made in private directories and the
# "ftp" target is made in the "/upc/$(PACKAGE)/build/" directory.
#
ftp:		check_system FORCE
	version=`cat VERSION`; \
	$(MAKE) $(MFLAGS) $(FTPDIR)/$(PACKAGE)-$$version.tar.Z \
	    VERSION=$$version; \
	test -r $(FTPDIR)/$(PACKAGE).tar.Z || exit 0; \
	    cd $(FTPDIR) || exit 1; \
	    rm $(PACKAGE).tar.Z || exit 1; \
	    ln -s $(PACKAGE)-$$version.tar.Z $(PACKAGE).tar.Z;

$(FTPDIR)/$(PACKAGE)-$(VERSION).tar.Z:	$(PACKAGE)-$(VERSION).tar.Z
	rm -f $@
	cp $(PACKAGE)-$(VERSION).tar.Z $@ 
	chmod u+rw,g+rw,o=r $@

# Make a compressed, tar(1) file of the binary distribution in the 
# appropriate FTP directory.
#
binftp:		FORCE
	version=`cat VERSION`; \
	$(MAKE) $(MFLAGS) $(FTPBINDIR)/$(PACKAGE)-$$version.tar.Z \
	    VERSION=$$version
ftpbin:		binftp

$(FTPBINDIR)/$(PACKAGE)-$(VERSION).tar.Z:
	rm -f $@
	id=$(PACKAGE)-$(VERSION) \
	&& rm -f $$id \
	&& ln -s $(prefix) $$id \
	&& tar $(TARFLAGS) - README $$id/bin $$id/include \
	    $$id/lib $$id/man | compress > $@ \
	&& rm $$id
	chmod u+rw,g+rw,o=r $@
	test -r $(FTPBINDIR)/$(PACKAGE).tar.Z || exit 0; \
	    cd $(FTPBINDIR) || exit 1; \
	    rm $(PACKAGE).tar.Z || exit 1; \
	    ln -s $(PACKAGE)-$(VERSION).tar.Z $(PACKAGE).tar.Z;


include rules.make

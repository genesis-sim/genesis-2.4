# $Id: Makefile,v 1.5 2005/10/04 22:05:13 svitak Exp $
# $Log: Makefile,v $
# Revision 1.5  2005/10/04 22:05:13  svitak
# Merged branch tagged rel-2-3beta-fixes back to main trunk. All comments from
# the branch modifications should be included.
#
# Revision 1.4  2005/09/29 21:30:25  ghood
# removed 'env -' to allow most environment variables to pass down to netcdf subdirectories
#
# Revision 1.3  2005/07/29 16:11:19  svitak
# INSTALL changed to INSTALLDIR. Consistent usage of CFLAGS.
#
# Revision 1.2  2005/06/17 19:34:16  svitak
# Cygwin fix: netcdflib failed to build if $PATH contained spaces. This is
# unlikely to happen on most build environments, but it is quite common in
# Windows. Adding quotes around $$PATH solves the problem.
#
# Revision 1.1.1.1  2005/06/14 04:38:29  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.5  2000/07/03 18:19:43  mhucka
# The command line for the library wasn't passing CFLAGS.
#
# Revision 1.4  2000/06/07 05:39:22  mhucka
# Don't bother with CPP, which doesn't seem to be needed and does screw up
# compilation under IRIX 6.5.3.
#
# Revision 1.3  1999/11/29 07:50:10  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.2  1999/10/16 22:46:40  mhucka
# Merged in changes from Upi to support netCDF 3.4.
# Also added a comment header at the top of the file.
#
# Revision 1.11  1997/08/04 05:36:37  dhb
# Added CPICOPT="" to environment for netcdf configuration to
# prevent configure from selecting -fpic gode generation option
# with gcc.  It results in link errors for some older Linux systems.
#
# Revision 1.10  1997/08/04 04:22:31  dhb
# Added setting of PATH to env - commands to pass along command path.
# Otherwise env - results in empty PATH.
#
# Revision 1.9  1997/08/03 19:48:51  dhb
# Added missing "env -" to call to netcdf configure.
#
# Revision 1.8  1997/08/03 19:19:10  dhb
# Added env - to call to netcdf configure command which avoids passing
# RANLIB macro value through the environment.  Have to set AR and YACC
# to avoid configure request for these programs.
#
# Revision 1.7  1997/07/31 23:58:59  dhb
# Suppress abnormal exit status from subdir cleans.  Netcdf, for example,
# comes without a Makefile and a clean before configure gives an error.
#
# Revision 1.6  1997/07/31 23:08:20  dhb
# Fixed comment leader from C style to Makefile hash
# ,
#
# Revision 1.5  1997/07/29 23:00:44  venkat
# Used the env command to clean the environment before compiling the
# netcdf library. Also added RCS Id and Log headers

CFLAGS		=	$(CFLAGS_IN)

OBJECTS = \
	netcdf_diskio.o \
	netcdf_variable.o \
	netcdf_metadata.o \
	netcdf_utilfunc.o \
	netcdf_copyright.o

# ===========================================
# File-format specific source-code locations
# ===========================================

# 1. Netcdf-3.4
#  -----------
# Passing this thro from top-level makefiles would require an unset
# We just define it here instead

NCDFDIR = netcdf-3.4
NCDFSRCDIR = $(NCDFDIR)/src
NCDFINCLUDEDIR = $(NCDFDIR)/src/libsrc

default: netcdflib.o

netcdflib.o: netcdflib $(OBJECTS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJECTS)

netcdflib:
	@(for subdir in $(NCDFSRCDIR); do echo cd $$subdir; cd $$subdir; PATH="$$PATH" CPICOPT="" CXX="" FC="" CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS=$(LDFLAGS) AR="$(AR)" YACC="$(YACC)" ./configure; PATH="$$PATH" make all; done)
	-touch netcdflib

clean:
	@(for subdir in $(NCDFSRCDIR); do echo cd $$subdir; cd $$subdir; make clean; cd ../..; done)
	-rm -f netcdflib *.[ao]

install:
	@cp netcdflib.o $(INSTALLDIR)/lib
	-cp $(NCDFINCLUDEDIR)/libnetcdf.a $(INSTALLDIR)/lib
	-$(RANLIB) $(INSTALLDIR)/lib/libnetcdf.a
	-cp *.h $(INSTALLDIR)/include
	-cp $(NCDFINCLUDEDIR)/netcdf.h $(INSTALLDIR)/include


#################################

# Suffix Rules

################################

.c.o:
	$(CC) $(CFLAGS) -I$(NCDFINCLUDEDIR) -I.. $< -c

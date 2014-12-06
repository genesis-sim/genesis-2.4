# $Id: Makefile,v 1.2 2005/07/29 15:53:04 svitak Exp $
# $Log: Makefile,v $
# Revision 1.2  2005/07/29 15:53:04  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.1.1.1  2005/06/14 04:38:29  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.18  1999/11/29 07:50:10  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.17  1999/10/16 21:53:19  mhucka
# Trivial comment cleanup.
#
# Revision 1.16  1999/10/16 21:25:06  mhucka
# Venkat had a version of the diskio code in his home directory that he
# appears to have been working on, and that did not make it into the
# source tree that I had originally used as the starting point of my
# GENESIS 2.2.2 DR reorganization effort.  In order to merge Venkat's
# changes into the distribution, I'm reverting to his version, then will
# update his version to incorporate recent changes by Upi for netCDF 3.4.
#
# Revision 1.13  1998/01/15 02:23:35  venkat
# Explicit inclusion of the netcdf and FMT1 subdirs as a -I option when
# compiling the object files.
#
# Revision 1.12  1998/01/14 00:59:00  venkat
# Changes to accomodate the change in directory structure where the
# abstract interface is seperated from the individual file format
# interfaces and libraries.
#
# Revision 1.11  1997/08/04  05:36:37  dhb
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
	absff_diskio.o \
	absff_metadata.o \
	absff_variable.o \
	absff_utilfunc.o


default: fflib.o


fflib.o: fflibs $(OBJECTS) 
	$(LD) $(LDFLAGS) -r -o $@ $(OBJECTS) 

fflibs: 
	@(for subdir in  $(DISKIOSUBDIR); do echo cd $$subdir; cd $$subdir; make CC="$(CC)" CFLAGS_IN="$(CFLAGS)" LD="$(LD)" LDFLAGS="$(LDFLAGS)"; cd ..; done)
	-touch fflibs

clean:
	@(for subdir in $(DISKIOSUBDIR); do echo cd $$subdir; cd $$subdir; make clean; cd ..; done)
	-rm -f fflibs *.[ao]

install:
	@(for subdir in $(DISKIOSUBDIR); do echo cd $$subdir; cd $$subdir; make RANLIB="$(RANLIB)" INSTALLDIR="$(INSTALLDIR)" install; cd ..; done)
	-cp *.h $(INSTALLDIR)/include

#################################

# Suffix Rules

################################

.c.o:
	$(CC) $(CFLAGS) -Inetcdf -IFMT1 $< -c

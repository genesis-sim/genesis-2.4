# $Id: Makefile,v 1.2 2005/07/29 15:53:07 svitak Exp $
# $Log: Makefile,v $
# Revision 1.2  2005/07/29 15:53:07  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.1.1.1  2005/06/14 04:38:32  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.10  2000/07/03 18:09:56  mhucka
# Removed RCS directives and added c flag to ar command invocation.
#
# Revision 1.9  1999/11/29 07:50:09  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.8  1996/06/18 06:46:15  dhb
# Removed redundant define for CC macro which on some systems
# was causing a recursive macro definition error.
#
# Revision 1.7  1996/06/06  23:06:29  dhb
# Now use AR macro for archive command.
#
# Revision 1.6  1995/02/13  21:17:42  dhb
# Removed checkout of RCSRELEASE versions.
#
# Revision 1.5  1995/01/09  23:36:25  dhb
# Commented the COMPILE.c macro and added the CPPFLAGS to the CFLAGS
# line.  Appears the COMPILE.c is not portable.
#
# Revision 1.4  1994/12/19  22:23:32  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.3  1994/12/06  01:59:22  dhb
# Added install target and support for ranlib.
#
# Revision 1.2  1994/12/06  00:09:49  dhb
# Nov 8 1994 changes from Upi Bhalla
#
# Revision 1.1.1.4  1994/04/11  22:29:21  dhb
# Missing some include dirs on RCS checkout actions
#
# Revision 1.1.1.3  1994/04/11  21:28:28  dhb
# Needed tabs on RCS checkout actions
#
# Revision 1.1.1.2  1994/04/11  21:15:38  dhb
# Added RCS checkout rules
#
# Revision 1.1.1.1  1994/04/11  17:44:07  dhb
# Update from Dave Bilitch.
#
# Revision 1.1  1994/01/13  18:34:14  bhalla
# Initial revision
#

CFLAGS    = $(TOPFLAGS) -I..
LIBS      = -lXt -lXext -lX11

OBJS      = \
	Initialize.o \
	Error.o \
	TopLevel.o \
	XoGeometry.o \
	XoUtil.o \
	XoChildRes.o \
	XoArgLists.o \
	XoDraw.o \
	XoColor.o \
	XoCursor.o \
	XoPS.o

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

libXo.a: $(OBJS)
	$(AR) rc libXo.a $(OBJS)
	$(RANLIB) libXo.a

clean:
	-rm -f *.[oa]

install:
	cp libXo.a $(INSTALLDIR)/lib
	$(RANLIB) $(INSTALLDIR)/lib/libXo.a

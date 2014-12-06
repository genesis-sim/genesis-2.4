# $Id: Makefile,v 1.2 2005/07/29 15:53:06 svitak Exp $
# $Log: Makefile,v $
# Revision 1.2  2005/07/29 15:53:06  svitak
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
# Revision 1.11  2000/07/03 18:09:55  mhucka
# Removed RCS directives and added c flag to ar command invocation.
#
# Revision 1.10  1999/11/29 07:50:08  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.9  1996/06/18 06:51:56  dhb
# Removed redundant define for CC macro which causes recursive
# macro definition errors on some machines.
#
# Revision 1.8  1996/06/06  23:03:35  dhb
# New use AR macro for archiving command.
#
# Revision 1.7  1995/07/17  20:44:54  dhb
# Added DrawUtil.o to OBJS line.
#
# Revision 1.6  1995/02/13  21:16:25  dhb
# Removed checkout of RCSRELEASE verions.
#
# Revision 1.5  1995/01/09  23:38:44  dhb
# Commented nonportable COMPILE.c macro and added CPPFLAGS to CFLAGS.
#
# Revision 1.4  1994/12/19  22:22:01  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.3  1994/12/06  02:03:23  dhb
# Added install target and ranlib handling.
#
# Revision 1.2  1994/12/06  00:17:22  dhb
# Nov 8 1994 changes from Upi Bhalla
#
# Revision 1.1.1.4  1994/04/11  22:30:14  dhb
# Missing some include dirs on RCS checkout actions
#
# Revision 1.1.1.3  1994/04/11  21:22:02  dhb
# Needed tabs on RCS checkout actions
#
# Revision 1.1.1.2  1994/04/11  21:14:14  dhb
# Added RCS checkout rules
#
# Revision 1.1.1.1  1994/04/11  17:36:01  dhb
# Update from Dave Bilitch
#
#CFLAGS    = -g -temp=../temp
CFLAGS    = -I.. $(TOPFLAGS)
LIBS      = -lXt -lXext -lX11 -lm -ldl -lw

WIDGETS   = \
	CoreDraw.o \
	DumbDraw.o \
	Graph.o \
	Draw.o \
	Pix.o \
	Sphere.o \
	Shape.o \
	Plot.o \
	Axis.o \
	Cell.o \
	Var.o \
	View.o \
	Tree.o \
	Gif.o
OBJS      = $(WIDGETS) DrawUtil.o

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $*.c
	rm -f $*.c

libDraw.a: $(OBJS)
	$(AR) rc libDraw.a $(OBJS)
	$(RANLIB) libDraw.a

clean:
	-rm -f *.[oa]

install:
	cp libDraw.a $(INSTALLDIR)/lib
	$(RANLIB) $(INSTALLDIR)/lib/libDraw.a

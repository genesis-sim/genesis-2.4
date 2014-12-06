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
# Revision 1.1.1.1  2005/06/14 04:38:33  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.16  2001/04/25 17:16:57  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.15  2000/09/21 19:22:02  mhucka
# Added a public-domain implementation of alloca() for those systems
# like AIX that don't seem to provide one.
#
# Revision 1.14  2000/07/03 18:09:56  mhucka
# Removed RCS directives and added c flag to ar command invocation.
#
# Revision 1.13  2000/05/19 18:20:56  mhucka
# 1) Some reformatting for consistency with other GENESIS makefiles.
# 2) Added files for new xgifload.
#
# Revision 1.12  1999/11/29 07:50:09  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.11  1998/07/15 06:18:07  dhb
# Upi update
#
# Revision 1.10  1996/06/18 06:53:22  dhb
# Removed explicit setting of CC macro which was probably benign
# since CC is set on the make command line taking precedence over
# the Makefile setting.
#
# Revision 1.9  1996/06/06  23:05:36  dhb
# Now use AR macro for archive command.
#
# Revision 1.8  1995/06/16  05:55:37  dhb
# Moved comment character commenting lines of the HEADERS macro
# to the first character of the line.  This is required for some
# versions of make.
#
# Revision 1.7  1995/03/17  23:11:39  venkat
# Included XoComposit.o to the list of OBJS
#
# Revision 1.6  1995/02/22  01:37:23  venkat
# Added XoCore.o to the list of OBJS
#
# Revision 1.5  1995/02/13  21:21:10  dhb
# Removed checkout of RCSRELEASE versions.
#
# Revision 1.4  1995/01/09  23:40:06  dhb
# Commented nonportable COMPILE.c macro and added CPPFLAGS to CFLAGS.
#
# Revision 1.3  1994/12/19  22:22:39  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.2  1994/12/06  02:04:49  dhb
# Added install target and ranlib handling.
#
# Revision 1.1  1994/12/06  00:15:26  dhb
# Initial revision
#
# Revision 1.4  1994/04/11  23:16:50  dhb
# Missing tab at beginning of RCS checkout actions
#
# Revision 1.3  1994/04/11  22:30:47  dhb
# Missing some include dirs on RCS checkout actions
#
# Revision 1.2  1994/04/11  21:15:04  dhb
# Added RCS checkout rules
#
# Revision 1.1  1994/04/11  17:42:49  dhb
# Initial revision
#

CFLAGS      = $(TOPFLAGS) -I.. -I../../shell -I../../sys -I../../ss
LIBS        = -lXt -lXext -lX11

WIDGETS     = \
		Label.o \
		Frame.o \
		Framed.o \
		XoCore.o \
		XoComposit.o \
		Form.o \
		Button.o \
		Text.o \
		Dialog.o \
		Image.o \
		xgifload.o \
		dgif_lib.o \
		gif_io.o \
		alloca.o \
		FastPlot.o

OBJS         = $(WIDGETS) \
		FormLayout.o

#Label.o: LabelP.h Label.h
#Frame.o: FrameP.h Frame.h
#Framed.o:	Framed.h FramedP.h 
#Form.o:	Form.h FormP.h
#Button.o:	Button.h ButtonP.h
#Text.o:	Text.h TextP.h
#Dialog.o:	Dialog.h DialogP.h
#Image.o: Image.h ImageP.h
#
#HEADERS = \
#	../Xo/XoDefs.h \
#	LabelP.h Label.h \
#	FrameP.h Frame.h \
#	Framed.h FramedP.h  \
#	Form.h FormP.h \
#	Button.h ButtonP.h \
#	Text.h TextP.h \
#	Dialog.h DialogP.h \
#	Image.h ImageP.h

#$(OBJS): ../Xo/XoDefs.h

# I have to force a compile here every time a header is changed
#.c.o:
# $(CC) $(CFLAGS) $< -c

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $*.c
	rm -f $*.c

libWidg.a: $(OBJS) $(HEADERS)
	$(AR) rc libWidg.a $(OBJS)
	$(RANLIB) libWidg.a

XoFormLayout.o: Form.h

clean:
	-rm -f *.[oa]

install:
	cp libWidg.a $(INSTALLDIR)/lib
	$(RANLIB) $(INSTALLDIR)/lib/libWidg.a

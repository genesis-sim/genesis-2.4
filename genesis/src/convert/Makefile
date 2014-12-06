# $Id: Makefile,v 1.6 2005/08/08 10:42:26 svitak Exp $
# $Log: Makefile,v $
# Revision 1.6  2005/08/08 10:42:26  svitak
# Changed full path $(TEST) to unpathed test.
#
# Revision 1.5  2005/08/08 09:22:27  svitak
# Changed convert.doc to convert.txt for install target.
#
# Revision 1.4  2005/08/07 21:24:26  svitak
# Replaced test with $(TEST).
#
# Revision 1.3  2005/07/29 15:52:45  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.2  2005/06/17 20:27:08  svitak
# The script for installing executables needs the .exe extension to be
# added to the program names under Cygwin. This was done by exporting
# the $(EXE_EXT) variable from the top level makefile and appending it
# to the program names.
#
# Revision 1.1.1.1  2005/06/14 04:38:28  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.15  2000/06/12 04:32:30  mhucka
# Added IRIX_HACK.  See src/Makefile.dist for more info.
#
# Revision 1.14  2000/05/26 23:39:50  mhucka
# Added new INSTALLBIN for things like the convert program (and other
# utilities in the future).
#
# Revision 1.13  1999/12/29 10:24:26  mhucka
# Added ../sys to list of directories searched for .h files.
#
# Revision 1.12  1999/11/29 07:50:09  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.11  1999/10/17 23:14:23  mhucka
# Added y.output to the make clean targets.
#
# Revision 1.10  1999/04/26 03:53:19  mhucka
# Found it necessary to include the CFLAGS macro in the directive for
# compiling convert.  Not sure why it was ever left off in the first place.
# It's needed under Solaris 2.x using gcc if gcc and its libraries are
# installed in a nonstandard location.
#
# Revision 1.9  1996/07/15 23:20:03  venkat
# Added -D$(MACHINE) to CFLAGS macro for dealing with the order of
# include of lex.yy.c in script.y.
#
# Revision 1.8  1995/12/13  23:45:41  dhb
# Changed explict cc to use the CC macro.
#
# Revision 1.7  1995/05/08  21:44:17  dhb
# Now removes y.tab.[ch] and lex.yy.c on make clean.
#
# Revision 1.6  1995/04/29  01:14:15  dhb
# Added use of YACC, LEX and LEXLIB macros to control processing of
# yacc and lex files and what lex library to use.
#
# Revision 1.5  1995/04/01  19:11:59  dhb
# Added installation of convert.doc and convert.man to install target.
#
# Revision 1.4  1995/03/27  18:51:30  dhb
# - Changed to use top level CFLAGS
# - Added a chmod in the install target to make sure installed X1compat
#   scripts are writable
#
# Revision 1.3  1995/02/13  21:36:16  dhb
# Removed checkout of RCSRELEASE versions.
# Added convert to things removed in clean target.
#
# Revision 1.2  1995/01/25  02:10:09  dhb
# Modified install target to install convert program and X1compat
# scripts.
#
# Revision 1.1  1995/01/13  01:09:48  dhb
# Initial revision
#

CFLAGS       	= $(CFLAGS_IN) -D$(OS) -D$(MACHINE)
CONVINCLUDE	= -I../sys
# The binary itself is actually installed in INSTALLBIN.
# INSTALLDIR is used for other things.
X1COMPAT_DIR 	= $(INSTALLDIR)/Scripts/X1compat
OBJ = \
      convert.o \
      output.o \
      mapcommand.o \
      script.o \
      strsave.o \
      symtab.o \
      ../sys/getopt.o

default: convert

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(CONVINCLUDE) $*.c
	rm -f $*.c

convert: $(OBJ)
	$(CC) $(CFLAGS) $(CONVINCLUDE) $(OBJ) $(LEXLIB) -o convert

script.o: y.tab.c lex.yy.c
	$(CC) -c $(CFLAGS) $(IRIX_HACK) $(CONVINCLUDE) y.tab.c
	mv y.tab.o script.o

lex.yy.c: script.l
	$(LEX) script.l

y.tab.h y.tab.c: script.y
	$(YACC) -dv script.y

realclean:
	-rm -f *.o y.tab.h y.tab.c lex.yy.c convert

install:
	-cp convert$(EXE_EXT) $(INSTALLBIN)
	-if test ! -d $(X1COMPAT_DIR); then mkdir $(X1COMPAT_DIR); fi
	-cp X1compat/*.g $(X1COMPAT_DIR)
	-chmod +w $(X1COMPAT_DIR)/*.g
	-if test ! -d $(INSTALLDIR)/man; then mkdir $(INSTALLDIR)/man $(INSTALLDIR)/man/man1; fi
	-if test ! -d $(INSTALLDIR)/man/man1; then mkdir $(INSTALLDIR)/man/man1; fi
	-cp convert.man $(INSTALLDIR)/man/man1/convert.1
	-cp convert.txt $(INSTALLDIR)/Doc

freeze:
	rcsclean
	rcsfreeze $(VERSION)
	co Makefile *.h,v script.y script.l

clean:
	-rm -f *.o y.tab.h y.tab.c lex.yy.c y.output convert

rcsclean: clean
	- rcsclean
	co Makefile *.h,v script.y script.l

# mds3
# New line needed to prevent getting default rule if using cross compilers
.c.o:
	$(CC) $(CFLAGS) $(CONVINCLUDE) $< -c

# $Id: Makefile,v 1.5 2005/07/29 15:52:47 svitak Exp $
# $Log: Makefile,v $
# Revision 1.5  2005/07/29 15:52:47  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.4  2005/06/20 21:20:15  svitak
# Fixed compiler warnings re: unused variables and functions with no return type.
# Default of int causes compiler to complain about return with no type, so void
# used as return type when none present.
#
# Revision 1.3  2005/06/17 20:25:40  svitak
# The script for installing executables needs the .exe extension to be
# added to the program names under Cygwin. This was done by exporting
# the $(EXE_EXT) variable from the top level makefile and appending it
# to the program names.
#
#
# Revision 1.2  2005/06/17 20:22:55  svitak
# The do_cmd_args in src/ss/eval.c is a recursive function that has a
# local buffer variable of 10000 chars. This causes a stack overflow
# under Cygwin when code_g tries to process the xfastplot object in
# src/Xodus/widget/widglib.g. The solution was to increase the stack
# size for code_g. This was done by exporting the $(CODE_G_LFLAGS)
# variable from the top level makefile and including it in the options
# for linking code_g.
#
# Revision 1.1.1.1  2005/06/14 04:38:34  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.24  2001/04/25 17:17:04  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.23  2000/07/13 18:59:06  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.22  2000/06/19 06:09:52  mhucka
# The include directory search path needs to include '..'.
#
# Revision 1.21  2000/06/07 05:34:36  mhucka
# Don't need $(TERMCAP), but do need to pay attention to $(LDFLAGS).
#
# Revision 1.20  1999/12/31 08:25:44  mhucka
# Added CFLAGS to cc lines, needed for some architectures.
#
# Revision 1.19  1999/11/29 07:50:21  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.18  1999/10/17 23:56:19  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.17  1999/04/26 03:49:09  mhucka
# Added the CFLAGS macro to the compile directory for code_g.
# It's not clear to me why it was ever left off.  It's necessary under
# Solaris 2.x if you put your gcc and libraries in a nonstandard location.
#
# Revision 1.16  1997/08/12 19:15:05  dhb
# Added dependency lines for code_g needed objects
#
# Revision 1.15  1996/05/23 23:01:31  dhb
# t3d/e port
#
# Revision 1.14  1995/11/02  02:27:50  venkat
# Hardcoded use of -ll changed to use the top-level LEXLIB macro
#
# Revision 1.13  1995/11/01  22:33:59  venkat
# Removed-install-target-dependencies
#
# Revision 1.12  1995/04/01  18:22:55  dhb
# Changed hardcoded uses of cc in actions to use CC macro value.
#
# Revision 1.11  1994/12/19  22:19:19  dhb
# Added rules to work with RCS subdirectory.
#
# Revision 1.10  1994/03/21  05:28:17  dhb
# Changes to compile code_g program.
#
# Revision 1.9  1993/08/24  21:28:56  dhb
# Removed -Bstatic from compile lines for code_*.c files
#
# Revision 1.8  1993/08/12  15:58:26  dhb
# Added system_deps.o to the library (was in src dir)
#
# Revision 1.7  1993/07/17  00:35:37  dhb
# Separate clean and rcsclean targets
#
# Revision 1.6  1993/07/01  16:34:14  dhb
# clean target wasn't keeping code_*.c checked out.  Fixed it.
#
# Revision 1.5  1993/07/01  15:58:11  dhb
# Added RCSRELEASE stuff
#
# Revision 1.4  1993/03/11  20:36:23  dhb
# Added code_*.c to the list of files to keep checked out.
#
# Revision 1.3  1993/02/12  17:33:46  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  20:00:08  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:05:43  dhb
# Initial revision
#

#
# makefile for utillib.o
#
LIBS 		=  	-lm
CFLAGS 		= 	$(CFLAGS_IN) -D$(OS) -D$(MACHINE)
SH  		= 	../shell
SS  		= 	../ss
SIMINCLUDE 	= 	-I. -I.. -I$(SH) -I$(SS)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	= 	utillib.$(LIBEXT)

OBJECTS = \
		getopt.o \
		hash.o \
		jump.o \
		popen.o \
		proc.o \
		rstats.o \
		signal.o \
		system.o \
		system_deps.o \
		segv.o  

default: $(TARGET_OBJ) code_lib code_func code_sym

CODE_G_OBJ = code_g.o \
	     ../ss/ss.$(LIBEXT) \
	     ../shell/shelllib-codeg.$(LIBEXT) \
	     utillib.$(LIBEXT)

code_g: $(CODE_G_OBJ)
	$(CC) $(CFLAGS) $(CODE_G_LFLAGS) $(CODE_G_OBJ) $(LEXLIB) $(TERMCAP) -lm  -o code_g

.SUFFIXES: .c,v .c .h,v .h

$(TARGET_OBJ): $(OBJECTS)
	$(LIBBUILD) $(LIBBUILDFLAGS) $(TARGET_OBJ) $(OBJECTS)
	$(LIBORDER) $(TARGET_OBJ)

.c.o:
	$(CC) $(CFLAGS) $(SIMINCLUDE) $< -c 

code_lib:
	$(CC) $(CFLAGS) -D$(OS) -o code_lib code_lib.c

code_func:
	$(CC) $(CFLAGS) -D$(OS) -o code_func code_func.c

code_sym:
	$(CC) $(CFLAGS) -D$(OS) -o code_sym code_sym.c

install:
	-cp $(TARGET_OBJ) code_lib$(EXE_EXT) code_func$(EXE_EXT) code_sym$(EXE_EXT) code_g$(EXE_EXT) $(INSTALL_LIB)
	-$(LIBORDER) $(INSTALL_LIB)/$(TARGET_OBJ)
	-cp *.h $(INSTALL_INCLUDE)

freeze:
	rcsclean
	rcsfreeze $(VERSION)
	co Makefile *.h,v code_lib.c code_func.c code_sym.c code_g.c

clean:
	-rm -f *.a *.o code_lib code_func code_sym code_g

rcsclean: clean
	-rcsclean
	co -r$(RCSRELEASE) Makefile *.h,v code_lib.c code_func.c code_sym.c code_g.c

all: 
	-rm -f code_lib code_func code_sym
	make default

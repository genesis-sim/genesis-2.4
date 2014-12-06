# $Id: Makefile,v 1.3 2005/07/29 15:52:47 svitak Exp $
# $Log: Makefile,v $
# Revision 1.3  2005/07/29 15:52:47  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.2  2005/07/20 19:07:51  svitak
# Changed $(CPP) lines to redirect output. Some architectures did not have
# the form of $(CPP) where the output is specified as an argument.
#
# Removed redundant 'CPP = $(CPP)' line.
#
# Made sure $(TMPDIR) was used instead of /tmp.
#
# Revision 1.1.1.1  2005/06/14 04:38:34  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.20  2001/04/25 17:16:59  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.19  2000/09/21 19:36:09  mhucka
# Typo: shellib.o should be shelllib.o.
#
# Revision 1.18  2000/07/13 18:58:28  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.17  2000/06/07 05:32:37  mhucka
# Don't include $(TERMCAP) in the builds of the targets, because they're not
# needed and can cause compilation problems on some platforms.  (E.g., under
# IRIX, something goofy happens and it doesn't properly find the shared
# library for libtermcap, causing compilation to fail.)
#
# Revision 1.16  1999/11/29 07:50:17  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.15  1999/10/17 23:57:02  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.14  1996/05/23 23:00:47  dhb
# t3d/e port
#
# Revision 1.13  1995/11/01  22:33:52  venkat
# Removed-install-target-dependencies
#
# Revision 1.12  1995/02/13  22:55:40  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h files to those removed by clean.
#
# Revision 1.11  1994/12/19  22:20:57  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.10  1994/03/22  23:57:52  dhb
# Changed codeg target to code_g.
#
# Revision 1.9  1994/03/21  05:29:09  dhb
# codeg target command was not outputing to the target object file.
#
# Revision 1.8  1994/03/21  04:53:27  dhb
# Changes to support initial compilation of the codeg program.
#
# Revision 1.7  1994/03/20  21:18:19  dhb
# Changes for C code startups
#
# Revision 1.6  1993/09/17  16:39:13  dhb
# Use CPP macro defined from top level Makefile.
#
# Revision 1.5  1993/07/17  00:27:27  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:53:28  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:27:52  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  19:57:27  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:04:57  dhb
# Initial revision
#

#
# Makefile for 'shelllib.o'
#
# Updated by Mike Speight (mds3@cns.caltech.edu)
#
# There are basically 3 options supported depending on definitions in CFLAGS :-
#	(1) BSD  terminal handling	: Default if none of below are defined
#	(2) SYSV terminal handling			: Add -DTERMIO
#	(3) No special terminal handling (ie. generic)	: Add -DPLAINTERM
# In addition BSD and SYSV both need '-ltermcap' added to the LIBS definition
#
# Starting with the 1.4 release, these settings are set in the
# src/Makefile.$(MACHINE) in the TERMOPT macro.  The TERMCAP macro
# should be defined to be -ltermcap if PLAINTERM is not selected.
#
# [DHB]
#

CFLAGS	= $(CFLAGS_IN) -D$(OS) -D$(MACHINE) -DNEW -DNLIST_BUG $(TERMOPT)
LIBS 		=  	-lm $(TERMCAP)

# The following includes :'sgtty.h' aka BSD
#CFLAGS	= $(CFLAGS_IN) -D$(OS) -D$(MACHINE) -DNEW -DNLIST_BUG
# The following includes :'termio.h' aka SYSV
#CFLAGS	= $(CFLAGS_IN) -D$(OS) -D$(MACHINE) -DNEW -DNLIST_BUG -DTERMIO
# The following includes none of the above
#CFLAGS	= $(CFLAGS_IN) -D$(OS) -D$(MACHINE) -DNEW -DNLIST_BUG -DPLAINTERM

SYS  		= 	../sys
SS		= 	../ss
SIM 		= 	../sim
SIMINCLUDE 	= 	-I. -I.. -I$(SYS) -I$(SS) -I$(SIM)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
LIBRARY_NAME 	= 	shell
STARTUP		=	shelllib.g
FUNCTIONS 	= 	shellfuncs
EXT_HEADER	=	shell_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	=	shelllib.$(LIBEXT)
CODEG_OBJ	=	shelllib-codeg.$(LIBEXT)

default: $(TARGET_OBJ)
code_g: $(CODEG_OBJ)

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS =	\
		shell_argv.o \
		shell_batch.o \
		shell_convert.o \
		shell_char.o \
		shell_debug.o \
		shell_edit.o \
		shell_env.o \
		shell_error.o \
		shell_esc.o \
		shell_exec.o \
		shell_func.o \
		shell_hash.o \
		shell_history.o \
		shell_info.o \
		shell_io.o \
		shell_job.o \
		shell_lib.o \
		shell_load.o \
		shell_log.o \
		shell_misc.o \
		shell_numeric.o \
		shell_ops.o \
		shell_prompt.o \
		shell_script.o \
		shell_setup.o \
		shell_sh.o \
		shell_status.o \
		shell_string.o \
		shell_sys.o \
		shell_tty.o 


HEADERS = 	\
		shell.h \
		shell_struct.h

.c.o:
	$(CC) $(CFLAGS) $(SIMINCLUDE) $< -c 

# $(OBJECTS) : $(HEADERS)

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	- $(SYS)/code_g $(STARTUP) shell_ext.h $(LIBRARY_NAME) -novar

# make the function list section of the symbol table

$(LIBRARY_NAME)_f@.c : $(FUNCTIONS)
	- $(SYS)/code_func $(FUNCTIONS) $(LIBRARY_NAME) \
	  > $(LIBRARY_NAME)_f@.c

SYMBOLTAB = $(LIBRARY_NAME)_g@.o 

$(TARGET_OBJ):  $(SYMBOLTAB) $(OBJECTS)
	$(LIBBUILD) $(LIBBUILDFLAGS) $(TARGET_OBJ) $(OBJECTS) $(SYMBOLTAB)
	$(LIBORDER) $(TARGET_OBJ)

$(CODEG_OBJ): $(OBJECTS) null_startup.o
	$(LIBBUILD) $(LIBBUILDFLAGS) $(CODEG_OBJ) $(OBJECTS) null_startup.o
	$(LIBORDER) $(CODEG_OBJ)

demo: interp_demo.o $(TARGET_OBJ) ../sys/utillib.o ../shell/shelllib.o
	$(CC) interp_demo.o $(TARGET_OBJ) \
	../shell/shelllib.o ../sys/utillib.o -o demo $(LIBS)

install:
	-cp $(TARGET_OBJ) $(INSTALL_LIB)
	-$(LIBORDER) $(INSTALL_LIB)/$(TARGET_OBJ)
	-cp *.h $(INSTALL_INCLUDE)

freeze:
	rcsclean
	rcsfreeze $(VERSION)
	co Makefile *.h,v *.g,v $(FUNCTIONS)

clean: 
	-rm -f *.a *.o *@.[ch]

rcsclean: clean
	- rcsclean
	co Makefile *.h,v *.g,v $(FUNCTIONS)

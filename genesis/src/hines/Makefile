# $Id: Makefile,v 1.3 2005/07/29 15:52:46 svitak Exp $
# $Log: Makefile,v $
# Revision 1.3  2005/07/29 15:52:46  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.2  2005/07/20 19:07:49  svitak
# Changed $(CPP) lines to redirect output. Some architectures did not have
# the form of $(CPP) where the output is specified as an argument.
#
# Removed redundant 'CPP = $(CPP)' line.
#
# Made sure $(TMPDIR) was used instead of /tmp.
#
# Revision 1.1.1.1  2005/06/14 04:38:32  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.22  2001/04/25 17:16:58  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.21  2000/07/03 18:06:26  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.20  1999/11/29 07:50:11  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.19  1999/10/17 23:56:16  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.18  1999/10/17 21:39:36  mhucka
# New version of Hines code from Erik De Schutter, dated 30 March 1999
# and including an update to hines.c from Erik from June 1999.  This version
# does not include changes to support Red Hat 6.0 and the Cray T3E, which
# will need to be incorporated in a separate revision update.
#
# Revision 1.15  1997/05/28 23:26:37  dhb
# Update for Antwerp GENESIS 21e version
#
# EDS22c revision 97/11/28: replaced h_init by hines_init
#
# EDS21e revision 97/05/05: deleted readhines and parallel files
#
# EDS21d revision 97/03/21: updated chanmodes and introduced calcmode
#
# EDS20l revison 95/12/09: EDS BBF-UIA 95/12/09-96/01/10
# Added connection stuff
#
# Revision 1.14  1996/05/23 22:59:04  dhb
# t3d/e port
#
# Revision 1.13  1995/11/02  01:42:15  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.12  1995/11/01  22:33:35  venkat
# Removed-install-target-dependencies
#
# Revision 1.11  1995/02/13  21:48:36  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h files to those removed by clean.
#
# Revision 1.10  1994/12/19  22:29:24  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.9  1994/03/21  05:36:26  dhb
# make code_g run first
#
# Revision 1.8  1994/03/20  06:33:11  dhb
# *** empty log message ***
#
# Revision 1.7  1994/03/20  06:12:38  dhb
# Changes for C code startups
#
# Revision 1.6  1993/09/17  16:31:09  dhb
# Use CPP macro defined from top level Makefile.
#
# Revision 1.5  1993/07/17  00:19:59  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:39:09  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:21:08  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  19:54:31  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:03:15  dhb
# Initial revision
#

#
# Makefile for hineslib.o
#

CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
SYS		=	../sys
SIMINCLUDE 	= 	-I. -I../sim -I../ss -I../shell -I../segment -I../olf -I../concen -I../device -I../newconn -I$(SYS)
LIBRARY_NAME 	= 	hines
FUNCTIONS 	= 	hinesfuncs
STARTUP		=	hineslib.g
STRUCTURES 	= 	hines_struct.h
EXT_HEADER	=	hines_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	= 	hineslib.$(LIBEXT)

default: $(TARGET_OBJ) 


OBJECTS =	\
		hines.o \
		hines_init.o \
		hh_init.o \
		h_calc.o \
		hh_update.o \
		compt_update.o \
		hines_child.o \
		hines_chip_init.o \
		hines_2chip.o \
		hines_2nichip.o \
		hines_4chip.o \
		hines_4nichip.o \
		hines_data.o \
		hines_msgs.o \
		hines_rchange.o \
		hines_conc_init.o \
		hines_conc.o \
		hines_conc_chip.o \
		hines_conc_4chip.o \
		hines_conn.o \
		hines_duplic.o \
		hines_solve.o

HEADERS = 	\
		hines_defs.h \
		hines_ext.h \
		hines_struct.h

.c.o:
	$(CC) $(CFLAGS) $(SIMINCLUDE) $< -c 

$(OBJECTS) : $(HEADERS)

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	- $(SYS)/code_g $(STARTUP) $(EXT_HEADER) $(LIBRARY_NAME)

# make the data structure section of the symbol table

$(LIBRARY_NAME)_d@.c : $(STRUCTURES)
	- $(CPP) $(SIMINCLUDE) $(STRUCTURES) > $(TMPDIR)/$(STRUCTURES)
	- $(SYS)/code_sym $(TMPDIR)/$(STRUCTURES) $(LIBRARY_NAME) \
	  -I $(EXT_HEADER) -NI -o $(LIBRARY_NAME)_d@.c
	- rm $(TMPDIR)/$(STRUCTURES)

# make the function list section of the symbol table

$(LIBRARY_NAME)_f@.c : $(FUNCTIONS)
	- $(SYS)/code_func $(FUNCTIONS) $(LIBRARY_NAME) \
	  > $(LIBRARY_NAME)_f@.c

# make the library header function
$(LIBRARY_NAME)_l@.c : $(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_d@.c $(OBJECTS)
	 - $(SYS)/code_lib $(LIBRARY_NAME) -o $(LIBRARY_NAME)_l@.c



SYMBOLTAB = $(LIBRARY_NAME)_d@.o $(LIBRARY_NAME)_g@.o $(LIBRARY_NAME)_l@.o

$(TARGET_OBJ): $(SYMBOLTAB) $(OBJECTS)
	$(LIBBUILD) $(LIBBUILDFLAGS) $(TARGET_OBJ) $(OBJECTS) $(SYMBOLTAB)
	$(LIBORDER) $(TARGET_OBJ)

install:
	-cp $(TARGET_OBJ) $(INSTALL_LIB)
	-$(LIBORDER) $(INSTALL_LIB)/$(TARGET_OBJ)
	-cp *.h $(INSTALL_INCLUDE)

clean:
	-rm -f *.a *.o *@.[ch]


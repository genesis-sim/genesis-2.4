# $Id: Makefile,v 1.4 2006/01/09 16:29:34 svitak Exp $
# $Log: Makefile,v $
# Revision 1.4  2006/01/09 16:29:34  svitak
# Added leech channels.
#
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
# Revision 1.1.1.1  2005/06/14 04:38:29  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.26  2001/04/25 17:16:59  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.25  2000/07/13 18:56:09  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.24  2000/07/03 18:24:59  mhucka
# 1) Added LDFLAGS to LIBBUILDFLAGS.
# 2) The definition of ASSERTON only worked with GNU make.  Changed
#    to use a more regular approach of setting the value of ASSERT.
#
# Revision 1.23  2000/04/27 07:29:08  mhucka
# Added fix from Mike Vanier for synchan objects involving SET and SHOW
# operations.  The problem was that they failed to trap attempts to set
# fields in nonexistent synapses.
#
# Revision 1.22  1999/11/29 07:52:23  mhucka
# Renamed synchan2.c to synchan.c.
#
# Revision 1.21  1999/11/29 07:50:51  mhucka
# Renamed newconn.g to newconnlib.g.
#
# Revision 1.20  1999/11/29 07:50:11  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.19  1999/11/27 08:44:55  mhucka
# Added M. Vanier's facsynchan object.
#
# Revision 1.18  1999/10/17 23:56:17  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.17  1999/10/15 02:04:46  mhucka
# Renamed hebbsynchan3.c to hebbsynchan3.c.  No name collision is caused by
# doing this, and Mike Vanier believes there is no reason why the file
# should have to remain named hebbsynchan3.c.
#
# Revision 1.16  1997/07/29 00:55:14  dhb
# Added delay2.o to OBJECTS
#
# Revision 1.15  1997/05/29 06:45:00  dhb
# Changes from Antwerp GENESIS 21e
#
#
# EDS21b revison: EDS BBF-UIA 96/08/02
# Added weight2
#
# Revision 1.14  1996/11/03 22:07:05  dhb
# Renamed hebbsynchan2.o to hebbsynchan3.o to avoid version
# mismatch problems.
#
# Revision 1.13  1996/11/01  22:49:47  dhb
# Changed synchan and hebbsynchan source file names back to
# synchan2.c and hebbsynchan2.c.  Necessary so correct patches
# can be generated against old versions.
#
# Revision 1.12  1996/07/23  18:33:29  venkat
# Added the segment directory to the include file search path
#
# Revision 1.11  1996/07/23  00:30:06  dhb
# Removed synchan2.o and hebbsynchan2.o from OBJECTS
#
# Revision 1.10  1996/05/23  22:59:18  dhb
# t3d/e port
#
# Revision 1.9  1995/11/02  01:42:41  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.8  1995/11/01  22:34:09  venkat
# Removed-install-target-dependencies
#
# Revision 1.7  1995/09/28  00:33:41  venkat
# Removed reference to hebbsynchan2.o
#
# Revision 1.6  1995/02/13  21:52:35  dhb
# Added @.h files to those removed by clean.
#
# Revision 1.5  1995/02/12  23:17:00  mvanier
# Removed -r$(RCSRELEASE) from %.h and %.c suffixes
#
# Revision 1.4  1995/02/12  23:06:49  mvanier
# Removed commented-out CFLAGS line with -g option
# Removed dependencies on $(SYS)/code_lib, $(SYS)/code_sym, $(SYS)/code_func
#
# Revision 1.3  1995/01/12  00:46:41  venkat
# Removed -Xs option to compile under Sun4
#
# Revision 1.2  1995/01/11  23:32:35  dhb
# Added checkout targets for RCS/*.[ch].
#
# Revision 1.1  1995/01/11  23:09:02  dhb
# Initial revision
#

#
# Makefile for newconnlib.o
#

LIBS 		=  	-lm
CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE) $(ASSERT) $(HAVE_NEW_CELLREADER)
SIM 		= 	../sim
SS 		= 	../ss
SYS 		= 	../sys
INTERP 		= 	../shell
SIMINCLUDE 	= 	-I. -I$(SYS) -I$(SS) -I$(SIM) -I$(INTERP)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
LIBRARY_NAME 	= 	newconn
FUNCTIONS 	= 	funcs
STARTUP		=	newconnlib.g
STRUCTURES 	= 	newconn_struct.h
EXT_HEADER	=	newconn_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	=	newconnlib.$(LIBEXT)

default : $(TARGET_OBJ)

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS = 	\
		connect.o		\
		connect3.o		\
		delay.o			\
		delay2.o		\
		delay3.o		\
		facsynchan.o		\
		getsyn.o		\
		hebbsynchan.o		\
		misc_connfuncs.o	\
		randomspike.o		\
		spikegen.o		\
		synaptic_event_list.o	\
		synchan.o		\
		synchan_utils.o	\
		SynE.o SynG.o SynS.o \
		weight.o		\
		weight2.o		\
		weight3.o		\
		stdp_rules.o

HEADERS = 	\
		fac_debug.h		\
		fac_defs.h		\
		fac_ext.h		\
		fac_struct.h		\
		newconn_defs.h		\
		newconn_ext.h		\
		newconn_struct.h

HINESINCLUDE = -I../hines
SEGINCLUDE = -I../segment

# The following ASSERT flag is used in the facsynchan code.
# By default, assertion checking is turned off.  To turn on assertion
# checking, uncomment one of the other ASSERT statements below.
ASSERT          = -DNDEBUG
# ASSERT          = -DPRECONDITIONS -DPOSTCONDITIONS -DDEBUG -DDEBUG0
# ASSERT          = -DPRECONDITIONS -DPOSTCONDITIONS -DINVARIANT -DDEBUG

# Set the following to 1 if you have Mike Vanier's cellreader library.
# Otherwise, leave it blank.

HAVE_NEW_CELLREADER     =

$(OBJECTS) : $(HEADERS) 

.c.o:
	$(CC) $(CFLAGS) $(SIMINCLUDE) $(HINESINCLUDE) $(SEGINCLUDE) $< -c 

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	- $(SYS)/code_g $(STARTUP) $(EXT_HEADER) $(LIBRARY_NAME)

# make the data structure section of the symbol table

$(LIBRARY_NAME)_d@.c : $(STRUCTURES)
	- $(CPP) $(SIMINCLUDE) $(HINESINCLUDE) $(SEGINCLUDE) $(STRUCTURES) > $(TMPDIR)/$(STRUCTURES)
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

freeze:

clean:
	-rm -f *.a *.o *@.[ch]

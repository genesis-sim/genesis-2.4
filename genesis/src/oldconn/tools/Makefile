# $Id: Makefile,v 1.3 2005/07/29 15:53:05 svitak Exp $
# $Log: Makefile,v $
# Revision 1.3  2005/07/29 15:53:05  svitak
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
# Revision 1.11  2001/04/25 17:16:59  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.10  2000/07/13 18:57:28  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.9  2000/07/03 18:06:29  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.8  1999/11/29 07:50:12  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.7  1996/07/16 22:18:32  dhb
# TARGET_OBJ wasn't using LIBEXT for the target file extension
#
# Revision 1.6  1996/05/23  23:05:20  dhb
# t3d/e port
#
# Revision 1.5  1995/11/02  01:42:44  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.4  1995/11/01  22:34:12  venkat
# Removed-install-target-dependencies
#
# Revision 1.3  1995/02/13  22:39:21  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h files to those removed by clean.
#
# Revision 1.2  1994/12/19  22:34:19  dhb
# Added rules for RCS subdirectiry.
#
# Revision 1.1  1994/09/23  16:16:45  dhb
# Initial revision
#

#
# Makefile for toollib.o
#

CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
SYS			= ../../sys
SIMINCLUDE 	= 	-I. -I../sim -I../../sys -I../../shell -I../../sim -I../../ss -I../../segment -I../../hh -I../../buffer -I../../olf  -I../../concen -I../../newconn
LIBRARY_NAME 	= 	toolconn
FUNCTIONS 	= 	toolfuncs
STARTUP		=	toolconnlib.g
STRUCTURES 	= 	toolconn_struct.h
EXT_HEADER	=	toolconn_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no need to order
LIBEXT		=	o
TARGET_OBJ	= 	toolconnlib.$(LIBEXT)

default: $(TARGET_OBJ) 

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS =	\
		conn_3d.o


HEADERS = 	\
		toolconn_ext.h 

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

freeze:
	rcsclean
	rcsfreeze $(VERSION)
	co Makefile *.h,v *.g,v $(FUNCTIONS)

clean:
	-rm -f *.a *.o *@.[ch]

rcsclean: clean
	- rcsclean
	co Makefile *.h,v *.g,v $(FUNCTIONS)

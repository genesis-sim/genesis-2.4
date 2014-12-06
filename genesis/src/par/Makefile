#
# Makefile for par library
#
# Written by Greg Hood (ghood@psc.edu), 10 Aug 2005
# Copyright (c) 2005 Pittsburgh Supercomputing Center
# Released under the GNU Library General Public License.
#

SHELL           =       /bin/sh
CC              =       $(CC)
CFLAGS 		= 	$(COPT) -D$(OS) -D$(MACHINE) $(DEC_HACK)

INSTALL_DIR	= 	$(INSTALLDIR)
INSTALL_LIB 	= 	$(INSTALL_DIR)/lib
INSTALL_INCLUDE	= 	$(INSTALL_DIR)/include

LIBRARY_NAME    =       par
EXT_HEADER      =       par_ext.h
TARGET_OBJ	= 	parlib.o
LIBORDER	=	@echo no lib ordering needed for

OBJECTS         =       par.o

HEADERS = $(EXT_HEADER)

default: $(TARGET_OBJ) 

$(OBJECTS) : $(HEADERS)

.c.o:
	$(CC) $(CFLAGS) -I. $< -c

$(OBJECTS) : $(HEADERS)

$(TARGET_OBJ): $(OBJECTS)
	$(LD) $(LDFLAGS) -r -o $(TARGET_OBJ) $(OBJECTS)

install:
	-cp $(TARGET_OBJ) $(INSTALL_LIB)
	-$(LIBORDER) $(INSTALL_LIB)/$(TARGET_OBJ)
	-cp *.h $(INSTALL_INCLUDE)

clean:
	-rm -f *.o; rm -rf *@.[ch]

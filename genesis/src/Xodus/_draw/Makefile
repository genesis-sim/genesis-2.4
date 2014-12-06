# ============================================
#
# Makefile for creating a new user library
#
# It is recommended that each library have its own makefile and
# its own subdirectory for clarity of organization.
# This subdirectory should contain all of the object files
# needed to compile the library as well as all header files,
# support files, and the script file for loading the library (e.g. userlib.g).
#
# The user should specify the values of the following makefile variables :
#
# GENESIS 
#	set this to the directory in which the simulator was installed
# LIBRARY_NAME
#	set this to the desired name of the library 
# FUNCTIONS
#	set this to the name of the file containing the list of
#	public function names in the library
# STARTUP
#	set this to the name of the startup script which defines the
#	commands and objects for this library
# STRUCTURES
#	set this to the name of the file containing the structure definitions
#	for objects in the library
# EXT_HEADER
#	set this to the name of the header file containing the appropriate
#	include references needed to compile the library files
# TARGET_OBJ
#	set this to the name of the library object which will be compiled
#	This is normally just the LIBRARY_NAME with 'lib.o' appended to it
# OBJECTS
#	list the user object files (with the .o extension) which will be 
#	a part of the library
#
# ============================================

LIBRARY_NAME 	= 	draw
FUNCTIONS 	= 	drawfuncs
STARTUP		=	drawlib.g
STRUCTURES 	=	draw_struct.h

EXT_HEADER	= 	draw_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	echo no need to reorder
LIBEXT		=	o
TARGET_OBJ	=	drawlib.$(LIBEXT)

OBJECTS =		\
	xcoredraw.o \
	xdumbdraw.o \
	xdraw.o		\
	xgraph.o	\
	xpix.o		\
	xsphere.o	\
	xshape.o	\
	xaxis.o		\
	xgif.o		\
	xplot.o		\
	xcell.o		\
	xvar.o		\
	xview.o		\
	xtree.o		



# ============================================
# everything below here should maintain itself
# ============================================

# If you want to use the debug option (which will cost you in both
# speed and memory), use the alternate CFLAGS = -g.
# Otherwise use "-O" to optimize.

#CFLAGS      =   $(TOPFLAGS) -D$(MACHINE) -g -temp=$(TMPDIR)
# the -temp flag no longer works in Solaris C. Instead set your
# TMPDIR environment variable to the correct place
CFLAGS      	= $(TOPFLAGS) -D$(MACHINE)
LIBS 		= -lm
SYS		= ../../sys
INCLUDE		= -I. -I.. -I../../sim -I../../shell -I../../sys -I../../ss
HEADERS 	= $(STRUCTURES) \
	xcoredraw_struct.h	\
	xdumbdraw_struct.h	\
	xgraph_struct.h		\
	xpix_struct.h		\
	xsphere_struct.h	\
	xshape_struct.h		\
	xplot_struct.h		\
	xaxis_struct.h		\
	xgif_struct.h		\
	xcell_struct.h		\
	xvar_struct.h		\
	../Draw/VarShape.h	\
	xview_struct.h		\
	xtree_struct.h

default : $(TARGET_OBJ)

$(OBJECTS) : $(HEADERS)

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $*.c
	rm -f $*.c

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) $< -c 

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	$(SYS)/code_g $(STARTUP) $(EXT_HEADER) $(LIBRARY_NAME)

# make the data structure section of the symbol table

$(LIBRARY_NAME)_d@.c : $(HEADERS)
	- $(CPP) $(INCLUDE) $(STRUCTURES) > $(TMPDIR)/$(STRUCTURES)
	- $(SYS)/code_sym $(TMPDIR)/$(STRUCTURES) $(LIBRARY_NAME) \
	  -I $(EXT_HEADER) -NI -o $(LIBRARY_NAME)_d@.c
	- rm $(TMPDIR)/$(STRUCTURES)

# make the function list section of the symbol table

$(LIBRARY_NAME)_f@.c : $(FUNCTIONS)
	- $(SYS)/code_func $(FUNCTIONS) $(LIBRARY_NAME) \
	  > $(LIBRARY_NAME)_f@.c

# make the library header function

$(LIBRARY_NAME)_l@.c: $(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_d@.c $(OBJECTS)
	- $(SYS)/code_lib $(LIBRARY_NAME) -o $(LIBRARY_NAME)_l@.c

SYMBOLTAB = $(LIBRARY_NAME)_d@.o $(LIBRARY_NAME)_g@.o $(LIBRARY_NAME)_l@.o

$(TARGET_OBJ): $(SYMBOLTAB) $(OBJECTS)
	$(LIBBUILD) $(LIBBUILDFLAGS) $(TARGET_OBJ) $(OBJECTS) $(SYMBOLTAB)
	$(LIBORDER) $(TARGET_OBJ)

clean:
	-rm -f *.a *.o; rm -f *@.[ch]

install:
	cp $(TARGET_OBJ) $(INSTALLDIR)/lib
	$(LIBORDER) $(INSTALLDIR)/lib/$(TARGET_OBJ)
	cp *.h $(INSTALLDIR)/include 

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

GENESIS		=	$(GEN_ROOT)
LIBRARY_NAME 	= 	par
FUNCTIONS 	= 	parfuncs
STARTUP		=	parlib.g
STRUCTURES 	= 	par_struct.h
EXT_HEADER	= 	par_ext.h

#
# note that target object is an archive and NOT an object file
#
TARGET_OBJ	=	parlib.a

#cmc Added Pvm-emulation.o to OBJECTS
OBJECTS =	\
		par_msg.o \
		par_connect.o \
		par_volume.o \
		par_show.o \
		bufman.o \
		post.o \
		rsync.o \
		zones.o \
		rfunc.o \
		parinit.o \
		pvm-shared.o \
		pvm-master.o \
		pvm-worker.o \
		setup_handler.o	\
		rpath.o	\
		util_funcs_par.o \
		libacct.o \
                pvm-emulation.o

#cmc Added Pvm-emulation.c to RELEASE 
RELEASE = 	Makefile \
		par_msg.c \
		par_connect.c \
		par_volume.c \
		par_show.c \
		bufman.c \
		post.c \
		rsync.c \
		zones.c \
		rfunc.c \
		parinit.c \
		pvm-shared.c \
		pvm-master.c \
		pvm-worker.c \
                pvm-emulation.c \
		setup_handler.c	\
		rpath.c	\
		util_funcs_par.c \
		bufman.h \
		fake_mesh.h \
		par_defs.h \
		par_ext.h \
		par_struct.h \
		pvm-defs.h \
		parlib.g \
		version.c \
		interactive \
		debugger \
		.simrc \
		.nxsimrc \
		schedule.g


# ============================================
# everything below here should maintain itself
# ============================================

# If you want to use the debug option (which will cost you in both
# speed and memory), use the alternate CFLAGS = -g.
# Otherwise use "-O" to optimize.

#CC		= $(CC)
#CFLAGS 		= 	-common -g $(TOPFLAGS) -D$(MACHINE) -I/usr/local/include
#CFLAGS 		= 	-g $(TOPFLAGS) -D$(MACHINE) -I/usr/local/include
CFLAGS = $(TOPFLAGS)
#LIBS 		=  	-lm -lpvm3
#CPP 		= 	/lib/cpp
GENESIS_LIB	= 	$(GENESIS)/lib
GENESIS_INCLUDE	= 	-I. -I$(GENESIS)/include
HEADERS 	=  	$(STRUCTURES)

default : $(TARGET_OBJ)

install: default
	$(CPLIB) $(TARGET_OBJ) ../lib

release:
	chmod +w ../rel/par/* ../rel/par/.simrc ../rel/par/.nxsimrc
	chmod -w $(RELEASE)
	cp $(RELEASE) ../rel/par
	chmod +w $(RELEASE)

$(OBJECTS) : $(HEADERS) 

.c.o:
	$(CC) $(CFLAGS) $(GENESIS_INCLUDE) $< -c 

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	- $(GENESIS_LIB)/code_g $(STARTUP) $(EXT_HEADER) $(LIBRARY_NAME)

# make the data structure section of the symbol table

$(LIBRARY_NAME)_d@.c : $(STRUCTURES) $(GENESIS_LIB)/code_sym
	- $(CPP) -P $(GENESIS_INCLUDE) $(STRUCTURES) > /tmp/$(STRUCTURES) 
	- $(GENESIS_LIB)/code_sym /tmp/$(STRUCTURES) $(LIBRARY_NAME) \
	  -I $(EXT_HEADER) -NI -o $(LIBRARY_NAME)_d@.c
	- rm /tmp/$(STRUCTURES)

# make the function list section of the symbol table

$(LIBRARY_NAME)_f@.c : $(FUNCTIONS) $(GENESIS_LIB)/code_func
	- $(GENESIS_LIB)/code_func $(FUNCTIONS) $(LIBRARY_NAME) \
	  > $(LIBRARY_NAME)_f@.c

# make the library header function

$(LIBRARY_NAME)_l@.c: $(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_d@.c $(GENESIS_LIB)/code_lib $(OBJECTS)
	- $(GENESIS_LIB)/code_lib $(LIBRARY_NAME) -o $(LIBRARY_NAME)_l@.c

SYMBOLTAB = $(LIBRARY_NAME)_d@.o $(LIBRARY_NAME)_g@.o $(LIBRARY_NAME)_l@.o

$(TARGET_OBJ): $(SYMBOLTAB) $(OBJECTS)
#	ld -r -o $(TARGET_OBJ) $(OBJECTS) $(SYMBOLTAB)
	- rm -f $(TARGET_OBJ)
	$(AR) -r $(TARGET_OBJ) $(OBJECTS) $(SYMBOLTAB)
	$(RANLIB) $(TARGET_OBJ)

clean:
	rm -rf *.o; rm -rf *@.[ch] *~ \#* *.a


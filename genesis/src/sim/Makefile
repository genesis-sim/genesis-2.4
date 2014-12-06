# $Id: Makefile,v 1.7 2005/08/12 19:54:06 svitak Exp $
# $Log: Makefile,v $
# Revision 1.7  2005/08/12 19:54:06  svitak
# Removed redirect of code_sym output to symout.
#
# Revision 1.6  2005/08/12 01:09:24  ghood
# Added ../par subdirectory to search path for included files.
#
# Revision 1.5  2005/07/29 15:52:47  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.4  2005/07/20 19:07:52  svitak
# Changed $(CPP) lines to redirect output. Some architectures did not have
# the form of $(CPP) where the output is specified as an argument.
#
# Removed redundant 'CPP = $(CPP)' line.
#
# Made sure $(TMPDIR) was used instead of /tmp.
#
# Revision 1.3  2005/07/10 05:23:29  svitak
# Uncommented line which removed $TMPDIR/$STRUCTURES.
#
# Revision 1.2  2005/06/27 19:00:45  svitak
# Added explicit types to untyped functions and fixed return values as
# appropriate. Initialized values explicitly when it was unclear if
# they were being used uninitialized (may not apply to all files being
# checked in in this batch).
#
# Revision 1.1.1.1  2005/06/14 04:38:28  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.33  2001/05/10 16:16:29  mhucka
# Accidentally put an unnecessary -$(SHELL) in the flags at some point.
#
# Revision 1.32  2001/04/25 17:17:00  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.31  2000/07/13 18:58:41  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.30  2000/07/03 18:06:30  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.29  2000/04/24 07:59:44  mhucka
# Minor fix to definition of SPRNG.
#
# Revision 1.28  1999/11/29 07:50:18  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.27  1999/10/17 23:56:18  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.26  1999/10/14 03:41:06  mhucka
# Fixed the reference to the SPRNG include directory.
#
# Revision 1.25  1998/01/08 23:51:11  dhb
# Support for SPRNG random number generators.
#
# Revision 1.24  1997/07/22 17:36:36  venkat
# Added sim_quit.o to the list of OBJECTS that determine the dependancy
# of the default target, simlib.o
#
# Revision 1.23  1996/05/31  01:27:58  dhb
# Removed RCS merge overlap delimiters.
#
# Revision 1.22  1996/05/30  19:18:33  dhb
# Merged in 1.20.1.1 (t3d/e port)
#
# Revision 1.21  1996/05/09  18:03:12  dhb
# Added sim_hash.o to OBJECTS for element hash table.
#
# Revision 1.20.1.1  1996/05/23  23:01:02  dhb
# t3d/e port
#
# Revision 1.20  1995/11/02  01:42:30  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.19  1995/11/01  22:33:49  venkat
# Removed-install-target-dependencies
#
# Revision 1.18  1995/04/01  17:16:30  dhb
# Added a dependency for sim_main.o and sim_help.o on sim_version.h
# which should cause these to be recompiled on version changes.
#
# Revision 1.17  1995/02/13  22:58:33  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h to files removed by clean.
#
# Revision 1.16  1994/12/19  22:20:15  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.15  1994/09/20  18:56:17  dhb
# Removed connection related objects.
#
# Added sim_markers.o which now contains the element marker related
# functions.
#
# Revision 1.14  1994/03/23  03:39:17  dhb
# Added sim_dump.o to object files list.
#
# Revision 1.13  1994/03/20  19:28:48  dhb
# Fixes re: C code startup
#
# Revision 1.12  1994/03/20  19:18:40  dhb
# Changes for C code startups
#
# Revision 1.11  1993/09/17  18:00:08  dhb
# Use CPP macro from top level Makefile.
#
# Revision 1.10  1993/07/17  00:29:48  dhb
# Separate clean and rcsclean targets
#
# Revision 1.9  1993/06/29  23:22:53  dhb
# Fixed RCSRELEASE stuff
#
# Revision 1.8  1993/06/29  23:18:49  dhb
# Added RCSRELEASE stuff
#
# Revision 1.7  1993/06/29  23:08:13  dhb
# Extended objects (Incomplete)
#
# Revision 1.6  1993/03/18  17:23:49  dhb
# Element field protection.  Added the sim_fieldlist.c module.
#
# Revision 1.5  1993/03/10  23:24:45  dhb
# Extended element fields.  Added the sim_extfields.c module.
#
# Revision 1.4  1993/02/12  16:31:50  dhb
# clean now does rcsclean followed by co Makefile *.h,v *.g,v simfuncs,v
# freeze does a similar thing
#
# Revision 1.3  1993/02/12  16:16:29  dhb
# Added freeze target to generate a configuration and added
# rcsclean command to the clean target
#
# Revision 1.2  1992/12/11  19:58:15  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:23:07  dhb
# Initial revision
#

CFLAGS 		= 	$(CFLAGS_IN) -D$(OS) -D$(MACHINE)
SYS 		= 	../sys
SS 		= 	../ss
INTERP 		= 	../shell
SPRNG		=	../sprng
PAR             =       ../par
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
SIMINCLUDE 	= 	-I. -I$(SYS) -I$(SS) -I$(INTERP) -I$(SPRNG)/include -I$(PAR)
LIBRARY_NAME 	= 	basic
FUNCTIONS 	= 	simfuncs
STARTUP		=	simlib.g
STRUCTURES 	= 	sim_struct.h
EXT_HEADER	=	sim_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	= 	simlib.$(LIBEXT)

default: $(TARGET_OBJ) 

# added RCS rules previously

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS =	\
		sim_access.o \
		sim_action.o \
		sim_address.o \
		sim_attach.o \
		sim_base.o \
		sim_bindings.o \
		sim_buffer.o \
		sim_call.o \
		sim_check.o \
		sim_class.o \
		sim_clock.o \
		sim_copy.o \
		sim_create.o \
		sim_current.o \
		sim_delete.o \
		sim_detach.o \
		sim_dump.o \
		sim_element.o \
		sim_elemlist.o \
		sim_estack.o \
		sim_event.o \
		sim_extfields.o \
		sim_extobject.o \
		sim_fieldlist.o \
		sim_fields.o \
		sim_file.o \
		sim_find.o \
		sim_get.o \
		sim_hash.o \
		sim_help.o \
		sim_index.o \
		sim_intgrt.o \
		sim_item.o \
		sim_list.o \
		sim_main.o \
		sim_malloc.o \
		sim_markers.o \
		sim_mask.o \
		sim_map.o \
		sim_method.o \
		sim_move.o \
		sim_msg.o \
		sim_name.o \
		sim_notes.o \
		sim_object.o \
		sim_ops.o \
		sim_position.o \
		sim_random.o \
		sim_reset.o \
		sim_response.o \
		sim_save.o \
		sim_sched.o \
		sim_set.o \
		sim_simulate.o \
		sim_stack.o \
		sim_status.o \
		sim_show.o \
		sim_step.o \
		sim_strings.o \
		sim_tform.o \
		sim_tree.o \
		sim_tstack.o \
		sim_valid.o \
		sim_value.o \
		sim_quit.o \
		nsim_wildcard.o

HEADERS = 	\
		$(SYS)/header.h \
		$(SYS)/profile.h \
		sim_defs.h \
		sim_ext.h \
		sim_struct.h 

.c.o:
	$(CC) $(CFLAGS) $(SIMINCLUDE) $< -c 

# make sim_main and sim_help recompile on version changes
sim_main.o sim_help.o: sim_version.h

# $(OBJECTS) : $(HEADERS)

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	- $(SYS)/code_g $(STARTUP) $(EXT_HEADER) $(LIBRARY_NAME)

# make the data structure section of the symbol table

$(LIBRARY_NAME)_d@.c : $(STRUCTURES)
	$(CPP) $(SIMINCLUDE) $(STRUCTURES) > $(TMPDIR)/$(STRUCTURES)
	$(SYS)/code_sym $(TMPDIR)/$(STRUCTURES) $(LIBRARY_NAME) \
	  -I $(EXT_HEADER) -NI -o $(LIBRARY_NAME)_d@.c
	- rm $(TMPDIR)/$(STRUCTURES)

# make the function list section of the symbol table

$(LIBRARY_NAME)_f@.c : $(FUNCTIONS)
	- $(SYS)/code_func $(FUNCTIONS) $(LIBRARY_NAME) \
	  > $(LIBRARY_NAME)_f@.c

SYMBOLTAB = $(LIBRARY_NAME)_d@.o $(LIBRARY_NAME)_f@.o $(LIBRARY_NAME)_g@.o 

$(TARGET_OBJ): $(OBJECTS) $(SYMBOLTAB)
	$(LIBBUILD) $(LIBBUILDFLAGS) $(TARGET_OBJ) $(OBJECTS) $(SYMBOLTAB)
	$(LIBORDER) $(TARGET_OBJ)

install:
	-cp $(TARGET_OBJ) $(INSTALL_LIB)
	-$(LIBORDER) $(INSTALL_LIB)/$(TARGET_OBJ)
	-cp *.h $(INSTALL_INCLUDE)

freeze:
	rcsclean
	rcsfreeze $(VERSION)
	co Makefile *.h,v *.g,v $(FUNCTIONS),v

clean:
	-rm -f *.a *.o *@.[ch]

rcsclean: clean
	- rcsclean
	co Makefile *.h,v *.g,v $(FUNCTIONS),v

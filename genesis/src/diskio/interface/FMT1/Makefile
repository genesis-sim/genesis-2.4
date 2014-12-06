CFLAGS		=	$(CFLAGS_IN)

OBJECTS = \
	FMT1_diskio.o \
	FMT1_variable.o \
	FMT1_metadata.o \
	FMT1_utilfunc.o \
	FMT1_libfuncs.o


default: FMT1lib.o


FMT1lib.o: $(OBJECTS) 
	$(LD) $(LDFLAGS) -r -o $@ $(OBJECTS) 

clean:
	-rm -f *.[o]

install:
	@cp FMT1lib.o $(INSTALLDIR)/lib
	-cp *.h $(INSTALLDIR)/include

#################################

# Suffix Rules

################################

.c.o:
	$(CC) $(CFLAGS) -I.. $< -c

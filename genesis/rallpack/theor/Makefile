TARGET = currinj
CFLAGS = -g
CC= cc
LIBS= -lm

default : $(TARGET)

OBJECTS = \
	nrutil.o \
	gammp.o \
	gammq.o \
	gser.o \
	gcf.o \
	gammln.o \
	erfc.o \
	inj.o

HEADERS = nrutil.h malloc.h

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS) : $(HEADERS)


clean:
	-(rm -rf *.o $(TARGET) core)

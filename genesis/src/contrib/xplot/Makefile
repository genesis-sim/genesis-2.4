#
#  xplot: the program: written by Matt Wilson. 
#         *heavily* modified and ANSI-fied by Mike Vanier.
#

CFLAGS		=	$(CFLAGS_IN)

OBJECTS = \
    arg.o          \
    axis.o         \
    buttons.o      \
    color.o        \
    command.o      \
    event_loop.o   \
    graph.o        \
    init.o         \
    keys.o         \
    label.o        \
    load.o         \
    main.o         \
    primitives.o   \
    psdriver.o     \
    save.o         \
    save2.o        \
    scale.o        \
    text.o         \
    tick.o

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

default: xplot

xplot: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LIBS)


install:
	strip xplot
	cp xplot $(INSTALLDIR)


clean:
	-rm -f *.o xplot
	-if [ -f make.out ]; then rm make.out; fi


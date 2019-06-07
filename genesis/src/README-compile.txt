Notes on the first steps in the genesis "make"
----------------------------------------------

These notes on the compilation process for genesis or nxgenesis may
help you to understand the output after an unsuccessful attempt to
compile GENESIS.

The target for "make" or "make nxall" in genesis/src/Makefile
begins with making "code_g" before compiling the simulation object
libraries. This is used to convert the various library SLI script files
"<libname>lib.g" into linkable object files. The target for "code_g:"
indirectly invokes Makefile.BASE, which performs these four steps:

(1) cd sys - compiles gcmalloc.c and other system files with no linking
    (gcc -c). These are linked to produce utillib.o:
    
     ld  -r -o utillib.o gcmalloc.o ... segv.o
     gcc (flags) -o code_lib code_lib.c (D and -o code_func code_func.c)
  
(2) cd ss - uses bison and flex to make the command parser.
    This produces C files lex.yy.c, y.tab.c, and yyless.c that are compiled
    and linked. lex.yy.c uses the function "yywrap()" that is defined in
    libfl.a, which should be in the system library, or installed in
    "genesis/locallibs".
    
(3) cd shell - creates the 'genesis' shell with terminal I/O.
    shell_tty.c makes extensive includes of header files for the
    chosen TERMCAP options. These are highly dependent on compiler
    version, and may result in undefined references to symbols that
    are used in shell_tty.c. Choosing "TERMOPT=-DPLAINTERM" results in
    a much simpler, but adequate parser for entering commands in the
    'genesis' shell.
    
(4) cd sys - compiles code_g, resulting in a message "code_g Compiled"
    This also compiles code_func and code_sym, to be used later.
    
After the basecode is compiled, the following steps are performed:

(5) cd sys - Nothing to be done for 'default'.
(6) cd ss - Nothing to be done for 'default'.
(7) cd sim - gcc -c many files and link them with "ld  -r -o simlib.o"
(8) cd sprng - builds scalable portable random number generator
    sprng/lib/liblfg.a

(9) cd shell - here code_g is used to produce shelllib.o
  ../sys/code_g shelllib.g shell_ext.h shell -novar

  gcc (flags) shell_g@.c -c
  ld  -r -o shelllib.o shell_argv.o ... shell_tty.o  shell_g@.o 

(10) cd newconn - here ../sys/code_sym is used instead of code_g
  cpp -P (include flags) newconn_struct.h > /tmp/newconn_struct.h
  ../sys/code_sym /tmp/newconn_struct.h newconn \
    -I newconn_ext.h -NI -o newconn_d@.c

  - compile a lot of files to .o, then
    ld -r -o newconnlib.o connect.o ... facdep_rules2.o newconn_d@.o
      newconn_g@.o newconn_l@.o

This is followed by compiles in other library directories.
If code_g and code_sym are not properly built in steps (1 - 4),
these compiles will fail with errors at the end such as:

  *** No rule to make target `sim/simlib.o', needed by `nxgenesis'.

If the "make" is successful, the output will end with:

    Full GENESIS Compiled -- All Done
or
    Non-X GENESIS Compiled -- All Done

Notes on code_g
---------------

The 'code_g' executeable is built on the second enty into sys, after
returning from shell. It is compiled from sys/code_g.c and linked with
object files in ss and shell that were produced previously. They may
have compiled with no errors, but there might be missing references
that are needed in the final linking.

cd sys
cc -O2 -D__NO_MATH_INLINES -DLONGWORDS -DBSD -DLinux -I. -I.. -I../shell
  -I../ss code_g.c -c 
gcc -O2 -D__NO_MATH_INLINES -DLONGWORDS -DBSD -DLinux  code_g.o ../ss/ss.o
  ../shell/shelllib-codeg.o utillib.o -lfl -lncurses -lfl -lm  -o code_g
code_g Compiled

The options for code_g.c: int main(argc, argv) are

"startup-file struct-file library-name -novariables -noobjects -cstartup
 c-function-name")

Typical uses are when the Makefile descends into sim:
   '../sys/code_g simlib.g sim_ext.h basic'
creates basic_g@.c, which is compiled to basic_g@.o and linked to other
object files to produce simlib.o.

Similarly in shell:

  ../sys/code_g shelllib.g shell_ext.h shell -novar

creates shell@.c, which is used to create shelllib.o

If code_g is not properly built, these compiles will fail.

As always, we are grateful for updates and fixes submitted to the
Repository for Continued Development of the GENESIS 2.4 Neural
Simulator (https://github.com/genesis-sim/genesis-2.4).

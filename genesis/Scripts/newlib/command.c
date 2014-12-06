#include "sim_ext.h"

/*
** this is an example of an interpreter callable C-function
** the name of this function must be added to the function list
** file to be accessible from within the interpreter (see examplefuncs)
*/
int do_example(argc,argv)
int argc;
char **argv;
{
    printf("%d arguments passed to %s\n",argc,argv[0]);
    /*
    ** functions can return values which can be used in the interpreter
    */
    return(argc);
}

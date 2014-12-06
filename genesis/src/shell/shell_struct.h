/*
** $Id: shell_struct.h,v 1.3 2005/06/26 08:29:26 svitak Exp $
** $Log: shell_struct.h,v $
** Revision 1.3  2005/06/26 08:29:26  svitak
** Added comment regarding include directives in this file.
**
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  2000/06/19 06:06:26  mhucka
** The Script structure and the handling of scripts had this bogosity in which
** FILE pointers were being cast to char * and back again.  Why on earth
** didn't the authors just keep a separate pointer in the Script structure for
** FILE pointers, instead of shoehorning the pointers into the .ptr char *
** field?  Anyway, for better robustness and portability, I reimplemented this
** part using two separate pointers.
**
** Revision 1.1  1992/12/11 19:04:55  dhb
** Initial revision
**
*/

/*
** This file is processed by the code_sym program, a code-generation
** utility of genesis. It requires definitions for FILE from <stdio.h>
** and PFD from "header.h". These files should be included before this file.
**
** Do not include any other files within this file!
*/

#ifndef SHELL_STRUCT_H
#define SHELL_STRUCT_H

#ifndef PFI_DEFINED
#define PFI_DEFINED
typedef int	(*PFI)();
#endif

typedef struct _EscapeCommand {
    char	*escseq;
    char	*string;
    int		argc;
    char	**argv;
    int		exec_mode;
    char	*id;
    struct _EscapeCommand *next;
} EscapeCommand;

typedef struct script_type {
    char	*ptr;
    char	*current;
    short	type;
    int		argc;
    char	**argv;
    short	line;
    FILE    *file;
} Script;

typedef struct func_table_type {
	char 	*name;
	PFI	adr;
	char 	*type;
} FuncTable;

typedef struct {
    char 	*name;
    char 	*type;
    short 	offset;
    short 	type_size;
    short 	field_indirection;
    short 	function_type;
    short 	dimensions;
    int 	dimension_size[4];
} Info;

typedef struct {
    PFI		function;
    int		count;
    int		priority;
} Job; 


#endif

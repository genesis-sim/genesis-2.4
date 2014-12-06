static char rcsid[] = "$Id: output.c,v 1.2 2005/06/27 19:00:34 svitak Exp $";

/*
** $Log: output.c,v $
** Revision 1.2  2005/06/27 19:00:34  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2000/07/12 06:11:33  mhucka
** Added #include of string.h.
**
** Revision 1.2  1997/08/08 19:28:43  dhb
** Added include of stdlib.h for malloc() return type
**
** Revision 1.1  1995/01/13 01:09:48  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
** output.c
**
**	Script converter output routines.
*/


static char	Indentation[500];
static int	TabStop = 8;
static int	WrapMargin = 72;
static int	Column = 0;
static int	LineDirty = 0;
static int	Disable = 0;

void EnableOutput()
{
	Disable = 0;
	Column = 0;
	LineDirty = 0;
	Indentation[0] = '\0';
}

void DisableOutput()
{
	Disable = 1;
}

void SetOutputIndentation(newIndentation)

char*	newIndentation;

{	/* SetOutputIndentation --- Save the current output indentation */

	strcpy(Indentation, newIndentation);

}	/* SetOutputIndentation */


static void UpdateColumn(str)

char*	str;

{
	for (; *str != '\0'; str++)
	    if (*str == '\t')
		Column += TabStop - Column%TabStop;
	    else if (*str == '\n')
	      {
		Column = 0;
		LineDirty = 0;
	      }
	    else
		Column++;
}


void OutputIndentation()
{
	if (Disable) return;

	printf("%s", Indentation);

	Column = 0;
	LineDirty = 0;
	UpdateColumn(Indentation);
}


static void OutputOne(s)

char*	s;

{
	if (Disable) return;

	UpdateColumn(s);
	if (Column > WrapMargin && LineDirty)
	  {
	    /* line continuation */
	    printf(" \\\n");
	    OutputIndentation();
	    printf("    ");
	    Column += 4;
	    UpdateColumn(s);
	  }

	if (Column > 0)
	    LineDirty = 1;
	printf("%s", s);
}


void Output(s1, s2, s3, s4, s5, s6, s7, s8, s9)

char *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8, *s9;

{	/* Output --- Output the given args up to a NULL */

	if (Disable) return;

	if (s1 == NULL)
	    return;
	OutputOne(s1);

	if (s2 == NULL)
	    return;
	OutputOne(s2);

	if (s3 == NULL)
	    return;
	OutputOne(s3);

	if (s4 == NULL)
	    return;
	OutputOne(s4);

	if (s5 == NULL)
	    return;
	OutputOne(s5);

	if (s6 == NULL)
	    return;
	OutputOne(s6);

	if (s7 == NULL)
	    return;
	OutputOne(s7);

	if (s8 == NULL)
	    return;
	OutputOne(s8);

	if (s9 == NULL)
	    return;
	OutputOne(s9);

}	/* Output */


void OutputArgList(argv)

char*	argv[];

{	/* OutputArgList --- Output values in argv up to a NULL */

	int	i;

	if (Disable) return;

	if (argv[0] != NULL)
	    OutputOne(argv[0]);

	for (i = 1; argv[i] != NULL; i++)
	  {
	    OutputOne(" ");
	    OutputOne(argv[i]);
	  }

}	/* OutputArgList */



char* Combine(s1, s2, s3, s4, s5, s6, s7, s8, s9)

char	*s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8, *s9;

{	/* Combine --- Combine the given args up to a NULL */

	char*	combined;
	int	len;

	/*
	** first we need to know how long the combined strings
	** will be
	*/

	if (s1 == NULL)
	    return NULL;
	len = strlen(s1);

	if (s2 == NULL)
	    goto do_combine;
	len += strlen(s2);

	if (s3 == NULL)
	    goto do_combine;
	len += strlen(s3);

	if (s4 == NULL)
	    goto do_combine;
	len += strlen(s4);

	if (s5 == NULL)
	    goto do_combine;
	len += strlen(s5);

	if (s6 == NULL)
	    goto do_combine;
	len += strlen(s6);

	if (s7 == NULL)
	    goto do_combine;
	len += strlen(s7);

	if (s8 == NULL)
	    goto do_combine;
	len += strlen(s8);

	if (s9 == NULL)
	    goto do_combine;
	len += strlen(s9);

do_combine:
	/*
	** Now we allocate the new string memory and combine
	** the strings
	*/

	combined = malloc(len + 1);
	if (combined == NULL)
	  {
	    perror("Combine");
	    fprintf(stderr, "conversion failed\n");
	    exit(1);
	    /* No return */
	  }

	strcpy(combined, s1);

	if (s2 == NULL)
	    return combined;
	strcat(combined, s2);

	if (s3 == NULL)
	    return combined;
	strcat(combined, s3);

	if (s4 == NULL)
	    return combined;
	strcat(combined, s4);

	if (s5 == NULL)
	    return combined;
	strcat(combined, s5);

	if (s6 == NULL)
	    return combined;
	strcat(combined, s6);

	if (s7 == NULL)
	    return combined;
	strcat(combined, s7);

	if (s8 == NULL)
	    return combined;
	strcat(combined, s8);

	if (s9 == NULL)
	    return combined;
	strcat(combined, s9);

	return combined;

}	/* Combine */


char* ArgListToStr(argv)

char*	argv[];

{	/* ArgListToStr --- Combine args in argv up to a NULL */

	char*	combined;
	int	len;
	int	i;

	len = 0;
	for (i = 0; argv[i] != NULL; i++)
	    len += strlen(argv[i]) + 1;

	if (len == 0)
	    return "";

	combined = malloc(len); /* Note: above len calc includes term '\0' */
	if (combined == NULL)
	  {
	    perror("ArgListToStr");
	    fprintf(stderr, "conversion failed");
	    exit(1);
	    /* No return */
	  }

	strcpy(combined, argv[0]);
	for (i = 1; argv[i] != NULL; i++)
	  {
	    strcat(combined, " ");
	    strcat(combined, argv[i]);
	  }

	return combined;

}	/* ArgListToStr */


char** AllocateArgList(max)

int	max;

{	/* AllocateArgList --- Allocate an argv of max items and intialize */

	char**	argv;

	max = 500;
	argv = (char**) malloc(sizeof(char*)*max);
	if (argv == NULL)
	  {
	    perror("AllocateArgList");
	    fprintf(stderr, "conversion failed");
	    exit(1);
	    /* No return */
	  }

	argv[0] = NULL;
	return argv;

}	/* AllocateArgList */


void AddItemToArgList(argv, item, max)

char*	argv[];
char*	item;
int	max;

{	/* AddItemToArgList --- Add item at the end of argv */

	int	i;

	max = 500;
	for (i = 0; i < max-1; i++)
	    if (argv[i] == NULL)
	      {
		argv[i] = item;
		argv[i+1] = NULL;

		return;
	      }

	fprintf(stderr, "AddItemToArgList: arg list overflow (max = %d)\n",
									max);
	fprintf(stderr, "conversion failed");
	exit(1);
	/* No return */

}	/* AddItemToArgList */


void InsertItemInArgList(argv, loc, item, max)

char*	argv[];
int	loc;
char*	item;
int	max;

{

	char*	temp;
	int	i;

	max = 500;
	if (loc >= max - 1)
	  {
	    fprintf(stderr, "InsertItemInArgList: bad arg list index\n");
	    fprintf(stderr, "conversion failed\n");
	    exit(1);
	    /* no return */
	  }

	for (i = 0; i < loc; i++)
	  {
	    if (argv[i] == NULL)
	      {
		fprintf(stderr, "InsertItemInArgList: bad arg list index\n");
		fprintf(stderr, "conversion failed\n");
		exit(1);
		/* no return */
	      }
	  }

	temp = argv[i];
	argv[i] = item;
	item = temp;
	do
	  {
	    i++;
	    if (i >= max)
	      {
		fprintf(stderr,
		    "InsertItemInArgList: arg list overflow (max = %d)\n", max);
		fprintf(stderr, "conversion failed");
		exit(1);
		/* no return */
	      }
	    temp = argv[i];
	    argv[i] = item;
	    item = temp;
	  }
	while (argv[i] != NULL);

}

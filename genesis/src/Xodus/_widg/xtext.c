/* $Id: xtext.c,v 1.2 2005/07/01 10:02:59 svitak Exp $ */
/*
 * $Log: xtext.c,v $
 * Revision 1.2  2005/07/01 10:02:59  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.13  2000/06/12 04:22:14  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.12  2000/04/28 06:11:50  mhucka
 * 1) Added forward declaration of ReadTextFile to silence compilation warnings.
 * 2) WriteTextFile returns an int, so returning a NULL is not correct.  Changed
 *    return stmt to return 0.  (But in practice it doesn't matter what's
 *    returned, since the only place that uses the function doesn't even check
 *    the return value!)
 *
 * Revision 1.11  1998/07/15 06:27:19  dhb
 * Upi update
 *
 * Revision 1.10  1995/07/08 01:34:58  venkat
 * Changes-to-allocate-and-deallocate-string-member-resources-in-the-SET-and-DELETE-actions
 *
 * Revision 1.9  1995/02/02  00:25:57  dhb
 * Extra checking for actions called without arguments.  This can happen
 * when the user uses the call command.
 *
 * Revision 1.8  1994/12/16  20:35:41  venkat
 * Fixed do_xtextload to pass local variable and avoid freeing
 * the text buffer multiple times
 *
 * Revision 1.7  1994/12/05  23:54:07  dhb
 * Bug fixes by Venkat
 *
 * Revision 1.6.1.1  1994/12/01  23:42:42  venkat
 * Side branch created to override lock created on 1.6 by bhalla
 * Fixed core dump bug on xtext with invalid ancestor
 *
 * Revision 1.6  1994/06/30  21:54:24  bhalla
 * Cleaned up a lot of unnecessary allocation relating to the hiddentext
 * field. This field is now a direct pointer copy of the widget text.
 * Added PROCESS action to copy the hiddentext over to the initialtext field.
 * perhaps I should rename the initialtext field.
 *
 * Revision 1.5  1994/06/29  19:05:00  bhalla
 * Added stuff for dump and reload of text
 *
 * Revision 1.4  1994/05/25  13:41:53  bhalla
 * Uses SearchForNonScript instead of fopen now, so it can find files that
 * are on the search path as well as the current directory.
 *
 * Revision 1.3  1994/02/08  22:27:13  bhalla
 * using xoFullName for widget name during create, added xoGetGeom
 *
 * Revision 1.2  1994/02/02  20:30:34  bhalla
 * Lots of stuff to get it to work with files
 * Added xoParseCreateArgs
 *
 * Revision 1.2  1994/02/02  20:30:34  bhalla
 * Lots of stuff to get it to work with files
 * Added xoParseCreateArgs
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "Xo/XoDefs.h"
#include "_xo/xo_defs.h"
#include "Widg/Framed.h"
#include "Widg/Frame.h"
#include "Widg/Text.h"
#include "Widg/Label.h"
#include "widg_ext.h"

#define XTEXTMAXLEN 500

/* Forward declarations */

static char *ReadTextFile();


/*
 * The GXconvert structure must be filled in for each xodus element, and
 * must have all the fields specific to that element/widget. Note that the
 * special fields (which are common to all xodus objects) are NOT entered
 * in this table. The data structure Gen2xo lives in the file xo_struct.h :
 * typedef struct {
 * 	char*	g;	** Genesis field name **
 * 	char*   x;	** Xodus  field name **
 * } Gen2xo;
 */

/* @@SECTION XObject */

static Gen2Xo   GXconvert[] = {
	{"editable",XtNeditable},
	{"font",XtNfont},
};


/*
 * * The action function handles all the actions that the xodus object *
 * carries out. This similar to the usual genesis object definition *
 * function
 */
int XText(textXObj, action)
	struct xtext_type *textXObj;
	Action         *action;
{
	int             ac = action->argc;
	char          **av = action->argv;
	Widget         parentW, newWidget, xoFindParentForm();
	char *textFileData = NULL;

/* ActionHeader is used for debugging purposes. It will print
 * out the header ("xobject" in this case), the pathname of the element,
 * and the action name. Defined in: sim/sim_response.c
 */
	if (Debug(0) > 1)
		ActionHeader("XText", textXObj, action);

	SELECT_ACTION(action) {
case PROCESS:
		/*
		 * This updates the initialtext field from the hiddentext
		 */
		if (textXObj->hiddentext)
			strncpy(textXObj->initialtext,textXObj->hiddentext,
								XTEXTMAXLEN - 2);
		break;
case RESET:
		/*
		 * Restore widget initial conditions. For example, a graph
		 * widget might wish to restore the current time to zero
		 * and clear the existing plots
		 */
		break;
case CREATE:	/* create the widget */
		/* Check that the parent is an xform element */

		if (!(parentW = xoFindParentForm(textXObj))) {
			XgError("Xtext: At least one ancestor must be an xform");
			return 0;
		}

		/* Create the widget */
		newWidget = XtVaCreateManagedWidget(
						    xoFullName(textXObj),
							framedWidgetClass, parentW,

						    /* By default I am making the text uneditable */
						    XtNeditable, False,
						    XtNtextHeight, 200,
						    XtNjustify, XoJustifyLeft,
						    XtNelevation, XoInsetFrame,
						    XtNchildClass, textWidgetClass,
						    XtNmappedWhenManaged, False,
						    NULL
						    );


		textXObj->widget = (char *) XoGetFramedChild(newWidget);
		textXObj->initialtext =(char *)calloc(XTEXTMAXLEN,sizeof(char));
		textXObj->filename = NULL;

		XtAddCallback((Widget)textXObj->widget, XtNcallback,
			xoCallbackFn, (XtPointer) textXObj);
		ac--, av++;	/* object type */
		ac--, av++;	/* path */
		xoParseCreateArgs(textXObj,ac,av);
		XtMapWidget(newWidget);
		xoGetGeom(textXObj);

		return(1);
		/* NOTREACHED */
		break;
case SET:
		if (ac) {
			for (; ac > 0; ac -= 2, av += 2) {
	/* If it's the file parameter then load the file into the widget.
	 * If the user has specified this twice then only the later file
	 * is the one kept.
	 */
				if (strcmp(av[0],"filename") == 0) {
					if (av[1] && (int)strlen(av[1]) > 0)
						textXObj->filename = (char *) malloc(sizeof(char) * (strlen(av[1])+1));
					else
						continue;

					if (textXObj->filename) 
				      strcpy(textXObj->filename, av[1]);
					else
				      XgError("Unable to allocate memory for text filename in text widget");

					textFileData = ReadTextFile(av[1]);
					if (!textFileData)
						continue;
					/* have to bypass the gx_convert here because
					** we are assigning a private field to the widget
					*/
					XoSetTextValue((Widget)textXObj->widget,
						textFileData,1);
					free(textFileData);
					textFileData = NULL;
					return(1);
				} else if (strcmp(av[0],"initialtext") == 0) {
					if (av[1]) {
					/* have to bypass the gx_convert here because
					** we are assigning a private field to the widget
					*/
						strncpy(textXObj->initialtext,av[1],
							XTEXTMAXLEN - 2);
						XoSetTextValue((Widget)textXObj->widget,
							av[1],1);
						return(1);
					}
				} else {

					/* Handle other command options like geometry etc... */
					gx_convert(textXObj, GXconvert, XtNumber(GXconvert),
						   ac, av);
				}
			}
			return(0);
		}
		break;
case DELETE:			/* Get rid of the widget and all kids */
		if (textXObj->filename) XtFree(textXObj->filename);
		if (textXObj->initialtext) XtFree(textXObj->initialtext);
  		if(!textXObj->widget) return 0;
		XtDestroyWidget(XtParent((Widget)textXObj->widget));
		break;
case SAVE2:
		/* This lets you save the text using the SAVE OBJECT FILENAME
		 * command in GENESIS.  The problem is that it saves 2 files.
		 * 1 for the GENESIS object and the other for the actual text.
		 * The actual text is stored in a file indicated by textXObj->file.
		 * This is a hook put in so that later when we can decide on how
		 * we want to handle saving of text we have it available.
		 */
		WriteTextFile(textXObj->filename, XoGetTextValue(textXObj->widget));
		break;
case XUPDATE:			/* update xobject fields due to changes in
				 * widget */
		/* xg_convert does the opposite of gx_convert and is defined
		 * in sim/xo_cvt.c
		 */
		if(ac > 0)
		xg_convert(textXObj, GXconvert, XtNumber(GXconvert), ac, av);
		break;
case KEYPRESS:
		/* Do a special check here for the hiddentext update */
		if (ac > 0)
		    textXObj->hiddentext = av[0];
		break;
case DUMP:
		if (ac == 1) {
			if (strcmp(av[0],"pre") == 0) {
				return(0);
			}
			if (strcmp(av[0],"post") == 0) {
				FILE *fp = (FILE *)action->data;
				char *str = textXObj->hiddentext;
				char *lineend;
				char *quotes;
				if (!str) return(0);

				fprintf(fp,"xtextload %s \\\n",Pathname(textXObj));
				for(lineend = strchr(str,'\n'); lineend ;
					lineend = strchr(str,'\n')) {
					/* a nasty hack here. We change the character
					** in the original string, then change it back */
					*lineend = '\0';

					fprintf(fp,"\"");
					/* prepend quotes in the string with backslashes */
					for (quotes = strchr(str,'"'); quotes;
						quotes = strchr(str,'"')) {
						*quotes = '\0';
						fprintf(fp,"%s\\\"",str);
						*quotes = '"';
						str = quotes + 1;
					}
					fprintf(fp,"%s\" \\\n",str);
					*lineend = '\n';
					str = lineend + 1;
				}
				/* wind up the output */
				fprintf(fp,"\"");
				/* prepend quotes in the string with backslashes */
				for (quotes = strchr(str,'"'); quotes;
					quotes = strchr(str,'"')) {
					*quotes = '\0';
					fprintf(fp,"%s\\\"",str);
					*quotes = '"';
					str = quotes + 1;
				}
				fprintf(fp,"%s\"\n",str);
				return(0);
			}
		}
		break;
	}
	return 0;
}

/* Utility function for loading in an xtext from a script file */
void do_xtextload(argc,argv) 
	int argc;
	char **argv;
{
	Element *elm;
	struct xtext_type *xtext;
	int total_text_len = 0;
	char	*textstr;
	char	*str;
	int		i;

	if (argc < 3) {
		printf("usage: %s xtext_element line1 ...\n",argv[0]);
		return;
	}
	if (!(elm = GetElement(argv[1]))) {
		Error();
		printf("could not find element %s\n",argv[1]);
		return;
	}
	if (ElementIsA(elm, "xtext")==0){
		Error();
		printf("%s is not an xtext_element\n",argv[1]);
		printf("usage: %s xtext_element line1 ...\n",argv[0]);
		return;
	}
	xtext = (struct xtext_type *)elm;
	for(i = 2; i < argc; i++) {
		if (!argv[i]) continue; /* should never happen */
		total_text_len += strlen(argv[i]) + 1;
	}
	str = textstr = (char *) malloc(sizeof(char) * total_text_len);
	/* append new line, replacing closing char with line break */
	for(i = 2; i < (argc-1); i++) {
		if (!argv[i]) continue; /* should never happen */
		strcpy(str,argv[i]);
		str = str + strlen(str);
		*str = '\n';
		str++;
	}
	/* finish off copy */
	strcpy(str,argv[i]);

	/* have to bypass the gx_convert here because
	** we are assigning a private field to the widget
	*/
	XoSetTextValue((Widget)xtext->widget,
		textstr, TRUE);
	free(textstr);
}

static char *ReadTextFile(fileName)
char *fileName;
{
	FILE *inputFile;
	FILE *SearchForNonScript();
	long sizeOfFile, i;

	char *inputData;

	inputFile = SearchForNonScript(fileName,"r");

	if (inputFile == NULL) {
		XgError("Cannot open file for text widget");
		return NULL;
	}

	/* Jump to the end of file. */
	fseek(inputFile, 0, 2);
	/* Figure out size of file. */
	sizeOfFile = ftell(inputFile);

	/* Return to beginning of file. */
	fseek(inputFile, 0, 0);

	/* Allocate memory for file (plus 1 for /0 character) */
	inputData = (char *) malloc(sizeof(char) * (sizeOfFile+1));

	/* Grab text from file */
	for(i = 0; i < sizeOfFile; i++) {
		inputData[i] = fgetc(inputFile);
	}

	inputData[sizeOfFile] = '\0';
	fclose(inputFile);
	return inputData;
}

/* Save away output file. */
int WriteTextFile(fileName, text)
char *fileName, *text;
{
	FILE *outputFile;

	outputFile = fopen(fileName,"w");

	if (outputFile == NULL) {
		XgError("Cannot open output file for text widget");
		return 0;
	}

	fprintf(outputFile,"%s",text);
	fclose(outputFile);
	return 0;
}

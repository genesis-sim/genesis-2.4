/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 2001 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

static char rcsid[] = "$Id: text.c,v 1.6 2005/07/29 16:13:15 svitak Exp $";
 
/*
 *$Log: text.c,v $
 *Revision 1.6  2005/07/29 16:13:15  svitak
 *Various changes for MIPSpro compiler warnings.
 *
 *Revision 1.5  2005/07/01 10:03:05  svitak
 *Misc fixes to address compiler warnings, esp. providing explicit types
 *for all functions and cleaning up unused variables.
 *
 *Revision 1.4  2005/06/20 21:08:32  svitak
 *Changed sprng() calls to G_RNG(). This is so the kinetics code can still
 *be compiled even if sprng cannot.
 *
 *Revision 1.3  2005/06/17 21:57:29  svitak
 *Changed widg to _widg for Xodus includes. See top-level Makefile.dist
 *for more details.
 *
 *Revision 1.2  2005/06/16 23:22:27  svitak
 *Updated June 2004 by Upi Bhalla to correct a bug which caused failure
 *on some non-Linux systems such as IRIX.
 *
 *Revision 1.1.1.1  2005/06/14 04:38:34  svitak
 *Import from snapshot of CalTech CVS tree of June 8, 2005
 */

/*
** old log
** Revision 1.1  2003/05/30 18:07:56  gen-dbeeman
** New version of kinetics library from Upi Bhalla, April 2003
**
*
*/

#ifndef NO_X
#  ifdef Cygwin
#    undef SYSV
#  endif
#  include <X11/Intrinsic.h>
#  ifdef Cygwin
#    define SYSV
#  endif
#endif
#include "kin_ext.h"
#ifdef ADDMSGIN
#  undef ADDMSGIN
#endif
#ifdef DELETEMSGIN
#  undef DELETEMSGIN
#endif
#ifndef NO_X
#  include "Widg/Text.h"
#  include "_widg/widg_defs.h"
#  include "_widg/xtext_struct.h"
#endif
#define TO_XTEXT 225
#define FROM_XTEXT 226
#define LOAD 227

/*
** An object for handling text strings used in kinetikit. The
** object is able to talk to xtext as well as to files so that
** text of arbitrary length can be stored, retrieved, edited
** and displayed.
** Reading and writing to file has to be done with a command which
** handles multiple short strings because of GENESIS string length
** limitations.
** Because of my pathetic coding in the genesis part of the xtext
** widget, I cannot set and arbitrarily long strings. Hence I have to 
** read and write directly to the widget.
*/

/*
**  To write to xtext: XoSetTextValue ((Widget)textXobj->widget,
**	const char* str, 1)
**	To read from xtext:
**	const char* XoGetTextValue((Widget)textXobj->widget)
**  To write to file: print out:		call <elm> LOAD \
**                  line1 \
**                  line2... \
**  To read from the file: the LOAD command handles it.
** 
*/

/* Replaces carriage returns with string terminated newlines with
** continuation characters. Same format as xtext. Original string
** is manipulated but comes back unchanged.
*/
void text_dump(fp, text)
FILE* fp;
struct text_type *text;
{
	char *s = text->str;
	char *n = NULL;

	fprintf(fp, "call %s LOAD \\\n", Pathname(text));
	
	while ( (n = strchr(s, '\n')) ) {
		*n = '\0';
		fprintf(fp, "\"%s\" \\\n", s);
		*n = '\n';
		s = n + 1;
	}
	fprintf(fp, "\"%s\"\n", s);
}

void text_load_str(text, action)
struct text_type *text;
Action* action;
{
			if (action->argc > 0) {
				int i;
				int len = 0;
				char *str;
				for (i = 0; i < action->argc; i++)
					len += strlen(action->argv[i]) + 1;
				if (len > text->len) {
					text->str = realloc(text->str, len);
					text->len = len;
				}
				str = text->str;
				*str = '\0';
				for (i = 0; i < action->argc - 1; i++) {
					strcpy(str, action->argv[i]);
					str = str + strlen(str);
					*str = '\n';
					str++;
				}
				strcpy(str, action->argv[i]);
			}
}

int TextFunc(text, action)
struct text_type* text;
Action* action;
{
    if(debug > 1) {
		ActionHeader("Reaction",text,action);
    }

    SELECT_ACTION(action) {
		case TO_XTEXT:
			if (action->argc == 1) {
				Element *elm;
				if ( (elm = GetElement(action->argv[0])) ) {
#ifndef NO_X
					XoSetTextValue( (Widget)
						( (struct xtext_type *)elm )->widget,
						text->str, 1);
#endif
				}
			}
			break;
		case FROM_XTEXT:
			if (action->argc == 1) {
				Element *elm;
				if ( (elm = GetElement(action->argv[0])) ) {
#ifndef NO_X
					char* ret = (char *)XoGetTextValue( (Widget)
						( (struct xtext_type *)elm )->widget);
					int len = strlen(ret);

					if (len > text->len) {
						text->str = realloc(text->str, len);
						text->len = len;
					}
					strcpy(text->str, ret);
#endif
				}
			}
			break;
		case SAVE: /* call text SAVE file */
			if (action->argc == 1 && strlen(text->str) > 0) {
				FILE *fp = fopen(action->argv[0], "a");
				if (!fp)
					return 0;
				text_dump(fp, text);
				fclose(fp);
			}
			break;
		case DUMP: /* simdump format */
			if (action->argc == 1) {
				if (strcmp(action->argv[0], "post") == 0) {
					FILE *fp = (FILE *)action->data;
					text_dump(fp, text);
					return(0);
				}
			}
			break;
		case LOAD: /* call text LOAD line1 line2 line3... */
			text_load_str(text, action);
			return 1;
			break;
		case SET: /* copy strings over. OK for short strings,
			** but it falls apart for really long ones as the
			GENESIS command line mechanism fails.*/
			if (action->argc == 2 && strcmp(action->argv[0], "str") == 0) {
				Action temp = *action;
				temp.argc--;
				temp.argv++;
				text_load_str(text, &temp);
				return 1;
			}
			break;
		case COPY: {
				struct text_type *copy =
					((struct text_type *)(action->data));
				copy->len = text->len;
				copy->str = malloc(text->len * sizeof(char));
				strcpy(copy->str, text->str);
			}
			break;
		case CREATE:
			text->str = calloc(TEXT_START_SIZE, sizeof(char));
			text->len = TEXT_START_SIZE;
			return 1;
			break;
	}
	return 0;
}

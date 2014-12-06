static char rcsid[] = "$Id: shell_prompt.c,v 1.2 2005/06/26 08:25:37 svitak Exp $";

/*
** $Log: shell_prompt.c,v $
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  2000/03/27 10:33:07  mhucka
** Commented out statusline functionality, because it caused GENESIS to send
** control character sequences upon exit and thereby screw up the user's
** terminal.  Also added return type declarations for various things.
**
** Revision 1.3  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/26 22:09:38  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:41  dhb
** Initial revision
**
*/

#include "shell_ext.h"

static char 	default_prompt[80];
static int 	command_cnt = 0;
static char	prompt_str[100];

void do_set_prompt(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "[new-prompt]");
    if (G_getopt(argc, argv) != 0 || optargc > 2)
      {
	printoptusage(argc, argv);
	return;
      }

    if(optargc > 1)
	SetPrompt(optargv[1]);
}

void DefaultPrompt()
{
    strcpy(prompt_str,default_prompt);
}

void SetDefaultPrompt(s)
char *s;
{
    if(s == NULL) return;
	strcpy(default_prompt,s);
}

void SetPrompt(s)
char *s;
{
    if(s == NULL) return;
	strcpy(prompt_str,s);
}

void inc_prompt()
{
    if(IsHistoryEnabled())
	command_cnt++; 
}

static char alternate_prompt_str[100];
static int alternate_prompt = 0;
void AlternatePrompt(str)
char *str;
{
    if(str){
	alternate_prompt = 1;
	strcpy(alternate_prompt_str,str);
    } else {
	alternate_prompt = 0;
    }
}

void show_prompt()
{ 
char 	string[100];
char	tmp[20];
char 	*numstr;
extern 	char *itoa();

if (IsSilent() > 0) {
#ifdef i860
	if (do_realmynode(0,0) > 0)
#endif
	return;
}

#ifdef NEW
    if(NestedLevel()){
	lprintf("? ");
	return;
    } else 
    if(alternate_prompt){
	lprintf("%s ",alternate_prompt_str);
	return;
    }
#else
    if(alternate_prompt){
	lprintf("%s ",alternate_prompt_str);
	return;
    }
#endif
    strcpy(string,prompt_str);
    if((numstr = strchr(string,'!')) != NULL){
	/*
	** substitute the command count for the !
	*/
	*numstr = '\0';
	sprintf(tmp,"%d",command_cnt);
	strcat(string,tmp);
	strcat(string,strchr(prompt_str,'!')+1);
    } 
    lprintf("%s >",string); 
    /*
    ** show the level of nesting
    */
#ifndef NEW
    int 	i;
    for(i=0;i<NestingLevel();i++)
	lprintf(">");
#endif
    lprintf(" ");
}


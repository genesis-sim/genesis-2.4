static char rcsid[] = "$Id: shell_history.c,v 1.2 2005/06/26 08:25:37 svitak Exp $";

/*
** $Log: shell_history.c,v $
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
** Revision 1.1  1992/12/11  19:04:31  dhb
** Initial revision
**
*/

#include "shell_ext.h"

static char	*history[1000];
static int	history_cnt = 0;
static int history_enabled = 1;

char *History(number)
int number;
{
    return(history[number]);
}

int HistoryCnt()
{
    return(history_cnt);
}

int IsHistoryEnabled() 
{ 
    return(history_enabled); 
}

void EnableHistory(state)
int state;
{
    history_enabled = state;
}

void AddHistory(string)
char *string;
{
char *CopyString();
char *ptr;

    if(string != NULL && history_enabled){
	/*
	** copy the string in and increment the history counter
	*/
	history[history_cnt] = CopyString(string);
	if ((ptr = strchr(history[history_cnt],'\n'))) {
	    /*
	    ** remove CRs from the string
	    */
	    *ptr = '\0';
	}
	history_cnt++;
    }
}

char *GetHistory(string)
char *string;
{
int	index;
char	cmp_string[100];
char	*nptr;
char	*sptr;

    if(string != NULL && strlen(string) > 1){
	/*
	** get the last command
	*/
	if(string[1] == '!'){
	    return(history[history_cnt-1]);
	} else
	if(string[1] >= '0' && string[1] <= '9'){
	    /*
	    ** retrieve by number
	    */
	    sscanf(string+1,"%d",&index);
	    if(index >= 0 && index < history_cnt){
		return(history[index]);
	    }
	} else {
	    /*
	    ** retrieve by match
	    */
	    /*
	    ** get the match string
	    */
	    sptr = string+1;
	    nptr = cmp_string;
	    while(!IsWhiteSpace(*sptr) && *sptr != '\0'){
		*nptr++ = *sptr++;
	    }
	    *nptr = '\0';
	    for(index=history_cnt-1;index>=0;index--){
		if(strncmp(cmp_string,history[index],strlen(cmp_string)) == 0){
		    return(history[index]);
		}
	    }
	}
    } 
    return("\n");
}

void ShowHistory(argc,argv)
int	argc;
char	**argv;
{
int	i;
int	start,end;

    start = 0;
    end = history_cnt - 1;

    initopt(argc, argv, "[start [end]]");
    if (G_getopt(argc, argv) != 0 || optargc > 3)
      {
	printoptusage(argc, argv);
	return;
      }

    if(optargc > 1){
	start = atoi(optargv[1]);
	if(start < 0 || start > history_cnt -1){
	    start = 0;
	}
    }
    if(optargc > 2){
	end = atoi(optargv[2]);
	if(end < 0 || end > history_cnt -1){
	    end = history_cnt - 1;
	}
    }
    for(i=start;i<=end;i++){
	printf("%6d    %s\n",i,history[i]);
    }
}


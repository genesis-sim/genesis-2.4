static char rcsid[] = "$Id: shell_ops.c,v 1.2 2005/07/20 20:02:03 svitak Exp $";

/*
** $Log: shell_ops.c,v $
** Revision 1.2  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.15  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.14  2001/04/01 19:30:47  mhucka
** Small cleanup fixes involving variable inits and printf args.
**
** Revision 1.13  1997/08/08 20:06:16  dhb
** Fixed return statement after printing command usage in do_findchar()
** and do_countchar() to return the correct type and value.
**
** Revision 1.12  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.11  1995/05/09 23:26:55  dhb
** Added additional checks to arguments of do_substring().  Was allowing
** out of bounds indexing of string characters.
**
** Revision 1.10  1995/03/17  19:33:56  dhb
** Changed use of strsave() in do_replacestr() to CopyString().
**
** Revision 1.9  1995/03/15  00:25:41  dhb
** Fixed bug in do_replacesubstr() which resulted in garbage
** being returned when no replacement is done on the original
** input string.  The old code returned the original string
** which was probably being freed twice.
**
** Revision 1.8  1994/10/21  00:56:12  dhb
** do_getarg() was calling CopyString() with the return from itoa().  Since
** itoa() returns allocated memory, the CopyString() was removed.
**
** Revision 1.7  1994/10/13  23:57:30  dhb
** Fixed ptr/int type conversion errors.
**
** Revision 1.6  1994/10/04  23:07:00  dhb
** Fixed incorrect naming of standard-deviation argument to gaussian
** command as a variance.
**
** Revision 1.5  1994/07/13  00:02:59  dhb
** Added do_replacesubstr (strsub) command which does string
** substitutions.
**
** Revision 1.4  1994/06/02  18:32:24  dhb
** Fixed syntax error in 1.3 revisions.
**
** Revision 1.3  1994/06/02  16:26:20  dhb
** Fixed return values from do_getarg() which must return allocated values.
**
** Revision 1.2  1993/02/23  22:40:40  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:40  dhb
** Initial revision
**
*/

#include <math.h>
#include <string.h>
#include "shell_ext.h"

#ifdef NEW
char *do_INPUT(argc,argv)
int argc;
char **argv;
{
char str[1000];
char prompt[100];
char *ptr;
char *retstr;
char *defvalue;
char defstr[100];

    initopt(argc, argv, "[prompt [default-reponse]]");
    if (G_getopt(argc, argv) != 0 || optargc > 3)
      {
	printoptusage(argc, argv);
	return NULL;
      }

    if(optargc > 1){
	ptr = optargv[1];
    } else {
	ptr = "?";
    }
    if(optargc > 2){
	defvalue = optargv[2];
	sprintf(defstr,"(def = %s) ",defvalue);
    } else {
	defvalue = "";
	defstr[0] = '\0';
    }
    sprintf(prompt,"%s%s ",defstr,ptr);
    AlternatePrompt(prompt);
    fflush(stdout);
    GetLineFp(str,stdin,1000);
    /*
    ** remove the CR
    */
    if ((ptr = strchr(str,'\n'))) {
	*ptr = '\0';
    }
    /*
    ** use the default value if nothing is entered
    */
    if(str[0] == '\0'){
	retstr = CopyString(defvalue);
    } else {
	retstr = CopyString(str);
    }
    AlternatePrompt(NULL);
    return(retstr);
}

float do_Random(argc,argv)
int argc;
char **argv;
{
float val;

    initopt(argc, argv, "low high");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    val = frandom(Atof(optargv[1]),Atof(optargv[2]));
    return(val);
}

float do_Gaussian(argc,argv)
int argc;
char **argv;
{
float val;

    initopt(argc, argv, "mean standard-deviation");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0.0);
      }

    val = rangauss(Atof(optargv[1]),Atof(optargv[2])*Atof(optargv[2]));
    return(val);
}

float do_max(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(MAX(Atof(optargv[1]),Atof(optargv[2])));
}

float do_min(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(MIN(Atof(optargv[1]),Atof(optargv[2])));
}

int do_trunc(argc,argv) 
int argc; 
char **argv; 
{ 
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    return((int)(Atof(optargv[1]))); 
}

int do_round(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    return(round(Atof(optargv[1])));
}

float do_tan(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(tan(Atof(optargv[1])));
}

float do_sin(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(sin(Atof(optargv[1])));
}

float do_cos(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(cos(Atof(optargv[1])));
}

float do_asin(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(asin(Atof(optargv[1])));
}

float do_acos(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(acos(Atof(optargv[1])));
}

float do_atan(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(atan(Atof(optargv[1])));
}

float do_pow(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "base exponent");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(pow(Atof(optargv[1]),Atof(optargv[2])));
}

float do_sqrt(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(sqrt(Atof(optargv[1])));
}

float do_exp(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(exp(Atof(optargv[1])));
}

float do_log(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(log(Atof(optargv[1])));
}

float do_fabs(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    return(fabs(Atof(optargv[1])));
}

#define COUNT	1
#define	ARG	2

char *do_getarg(argc,argv)
int argc;
char **argv;
{
int n = 0;
int option = 0;
int status;
char *itoa();

    option = 0;

    initopt(argc, argv, "[arguments] -count -arg #");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-arg") == 0)
	  {
	    option = ARG;
	    n = atoi(optargv[1]);
	  }
	else if (strcmp(G_optopt, "-count") == 0)
	    option = COUNT;
      }

    if(status < 0 || option == 0){
	printoptusage(argc, argv);
	printf("\tOne of -count or -arg must be given\n");
	return(NULL);
    }

    if(option == COUNT)
	return(itoa(optargc-1));

    if(n >= optargc || n < 1){
	printf("invalid argument number '%d'\n",n);
	return(NULL);
    }

    return(CopyString(optargv[n]));
}

#undef COUNT
#undef	ARG

char *do_cat(argc,argv)
int argc;
char **argv;
{
char *str;

    initopt(argc, argv, "string string");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(NULL);
      }

    str = (char *)malloc(strlen(optargv[2])+strlen(optargv[1])+1);
    sprintf(str,"%s%s",optargv[1],optargv[2]);
    return(str);
}

char *do_replacesubstr(argc, argv)

int argc;
char **argv;

{	/* do_replacesubstr --- Replace a substring with another */

	char	*origstr;
	char	*oldsub;
	char	*newsub;
	char	*oldstr;
	char	*newstr;
	char	*changesub;
	size_t	oldsublen;
	size_t	newsublen;
	size_t	sublendiff;
	int	all;
	int	status;

	all = 0;

	initopt(argc, argv, "string old-substring new-substring -all");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-all") == 0)
		all = 1;
	  }

	if (status < 0)
	  {
	    printoptusage(argc, argv);
	    return NULL;
	  }

	origstr = optargv[1];
	oldsub = optargv[2];
	newsub = optargv[3];

	oldsublen = strlen(oldsub);
	newsublen = strlen(newsub);
	sublendiff = newsublen - oldsublen;

	oldstr = origstr;
	newstr = CopyString(origstr);
	changesub = strstr(oldstr, oldsub);
	while (changesub != NULL)
	  {
	    char	*newsearch;

	    newstr = (char *)malloc(strlen(oldstr) + sublendiff + 1);
	    if (newstr == NULL)
	      {
		Error(); perror(argv[0]);
		return NULL;
	      }

	    strncpy(newstr, oldstr, changesub - oldstr);
	    newstr[changesub - oldstr] = '\0';
	    strcat(newstr, newsub);
	    strcat(newstr, changesub + oldsublen);

	    if (!all)
		break;

	    newsearch = newstr + (changesub - oldstr) + newsublen;
	    changesub = strstr(newsearch, oldsub);
	    if (oldstr != origstr)
		free(oldstr);

	    oldstr = newstr;
	  }

	return newstr;

}	/* do_replacesubstr */


char *do_substring(argc,argv)
int argc;
char **argv;
{
int start;
int end;
int len;
int slen;
char *s;
char *copy;

    initopt(argc, argv, "string startindex [endindex]");
    if (G_getopt(argc, argv) != 0 || optargc > 4)
      {
	printoptusage(argc, argv);
	return(NULL);
      }

    s = optargv[1];
    slen = (int)strlen(s);
    start = atoi(optargv[2]);
    if(optargc > 3){
	end = atoi(optargv[3]);
    } else {
	end = slen - 1;
    }
    if (start < 0 || start > slen-1 || end < -1 || end > slen-1){
	return(NULL);
    }
    if((len = end -start+1) < 0){
	return(NULL);
    }
    copy = (char *)malloc(len +1);
    strncpy(copy,s+start,len);
    copy[len] = '\0';
    return(copy);
}

int do_findchar(argc,argv)
int argc;
char **argv;
{
char *s;
char c;
char *loc;

    initopt(argc, argv, "string character");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(-1);
      }

    s = optargv[1];
    c = optargv[2][0];
    if(( loc =strchr(s,c)) == NULL){
	return(-1);
    }
    return (int)(loc-s);
}

int do_countchar(argc,argv)
int argc;
char **argv;
{
char *s;
char c;
int count;

    initopt(argc, argv, "string character");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    s = optargv[1];
    c = optargv[2][0];

    count = 0;
    s--;
    while(( s =strchr(s+1,c)) != NULL){
	count++;
    }
    return(count);
}

int do_strcmp(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "string string");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    return(strcmp(optargv[1],optargv[2]));
}

int do_strncmp(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "string string number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    return(strncmp(optargv[1],optargv[2],atoi(optargv[3])));
}

int do_strlen(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "string");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    return (int)(strlen(optargv[1]));
}

char **do_arglist(argc,argv)
int argc;
char **argv;
{
int nargc;
char **nargv;

    initopt(argc, argv, "string");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(NULLArgv());
      }

    StringToArgList(optargv[1],&nargc,&nargv,0);
    return(nargv);
}

#endif

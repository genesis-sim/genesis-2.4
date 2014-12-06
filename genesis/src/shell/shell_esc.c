static char rcsid[] = "$Id: shell_esc.c,v 1.2 2005/06/26 08:25:37 svitak Exp $";

/*
** $Log: shell_esc.c,v $
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/23 23:56:13  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:27  dhb
** Initial revision
**
*/

#include "shell_ext.h"

EscapeCommand	*esc_command;

EscapeCommand *AddEscapeString(seq,string,exec_mode,id)
char 	*seq;
char 	*string;
int	exec_mode;
char	*id;
{
char line[1000];
EscapeCommand *esc;
EscapeCommand *new_esc;
char	*ptr,*lptr;
char tmp[10];
char *tmpptr,*tptr;

    /*
    ** search the escape list for any existing definition of the
    ** character
    */
    for(esc = esc_command;esc;esc=esc->next){
	if(strcmp(esc->escseq,seq) == 0){
	    /*
	    ** if found then replace it with the new definition
	    */
	    break;
	}
    }
    /*
    ** add the sequence to the list
    */
    if(esc == NULL){
	/*
	** not found therefore allocate a new one
	*/
	new_esc = (EscapeCommand *)calloc(1,sizeof(EscapeCommand));
	new_esc->next = esc_command;
	esc_command = new_esc;
	new_esc->escseq = CopyString(seq);
    } else {
	new_esc = esc;
    }
    /*
    ** set the ID of the escape sequence
    */
    new_esc->id = CopyString(id);
    /*
    ** set the escape exec mode
    */
    new_esc->exec_mode = exec_mode;
    StringToArgList(string,&new_esc->argc,&new_esc->argv,0);
    /*
    ** set the escape command string
    */
    lptr = line;
    ptr = string;
    while(*ptr != '\0'){
	if(*ptr == '<'){
	    switch(*(ptr+1)){
	    case 'c':
		/*
		** get the control character equivalent
		*/
		tmpptr = tmp;
		for(tptr=ptr+2;*tptr!='>';tptr++,tmpptr++){
		    *tmpptr = *tptr;
		}
		*tmpptr = '\0';
		*lptr = (char)atoi(tmp);
		break;
	    case '^':
		/*
		** get the control character equivalent
		*/
		*lptr = *(ptr+2) & ~0x40;
		break;
	    case 'C':
		if(strncmp(ptr,"<CR>",4) == 0){
		    /*
		    ** additional way of specifying carriage return
		    */
		    *lptr = '\n';
		}
		break;
	    }
	    ptr = strchr(ptr,'>');
	} else {
	    *lptr = *ptr;
	}
	lptr++;
	ptr++;
    }
    *lptr = '\0';
    new_esc->string = CopyString(line);
    return(new_esc);
}

int do_escape_seq(argc,argv)
int argc;
char **argv;
{
int exec_mode = 0;
char	*id;
int	status;

    id = "";

    initopt(argc, argv, "esc-sequence string -execute -id string");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-execute") == 0)
	    exec_mode = 1;
	else if (strcmp(G_optopt, "-id") == 0)
	    id = optargv[1];
      }

    if(status < 0){
	printoptusage(argc, argv);
	return(0);
    }

    AddEscapeString(optargv[1],optargv[2],exec_mode,id);
    return(1);
}

void do_list_esc(argc,argv)
int argc;
char **argv;
{
int i;
EscapeCommand *esc;

    initopt(argc, argv, "");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    printf("AVAILABLE ESCAPE KEYS\n");
    printf("---------------------\n");

    for(esc = esc_command;esc;esc=esc->next){
	printf("%-10s %-15s ",esc->escseq,esc->id);
	if(esc->exec_mode){
	    printf("%-10s","EXEC");
	} else {
	    printf("%-10s","REPLACE");
	}
	for(i=0;i<esc->argc;i++){
	    printf("%s ",esc->argv[i]);
	}
	printf("\n");
    }
    printf("\n");
}

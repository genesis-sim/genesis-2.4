static char rcsid[] = "$Id: code_func.c,v 1.3 2005/06/27 19:01:13 svitak Exp $";

/*
** $Log: code_func.c,v $
** Revision 1.3  2005/06/27 19:01:13  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.2  2005/06/25 22:04:40  svitak
** Added explicit return types to untyped functions. Fixed return statements.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2000/09/21 19:46:22  mhucka
** #include some files to get some function declarations.
**
** Revision 1.1  1992/12/11 19:05:26  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <string.h>

#define MAXFUNCS 1000
int is_space_char(c)
char	c;
{
	return(c== '\n' || c == ' ' || c=='\t');
}

void GetLine(line,fp,size)
char 	*line;
FILE	*fp;
int		size;
{
int		eol;

	if(fgets(line,size,fp) == NULL) line[0] = '\0';;
	if((eol = strlen(line) - 1) >= 0){
		if(line[eol] == '\n') line[eol] = '\0';
	}
}


void MakeFuncTableCode(file,funcname)
char *file;
char *funcname;
{
FILE *fp;
char line[80];
char *ptr;
char name[80];
char type[80];
struct {
    char name[40];
    char type[40];
} table[MAXFUNCS];
int count;
int i;

    /*
    ** open the function list file
    */
    if((fp = fopen(file,"r")) == NULL){
	fprintf(stderr,"unable to find function list file %s\n",file);
	return;
    }
    printf("#define PF(F,T) HashFunc(\"F\",F,\"T\")\n");

    printf("void FUNC_%s(){\n",funcname);

    count = 0;
    while(!feof(fp) && count < MAXFUNCS){
	GetLine(line,fp,80);
	if(strlen(line) == 0) continue;
	ptr = line;
	/*
	** extract the type and name
	*/
	/*
	*** skip white space
	*/
	while(*ptr != '\0' && is_space_char(*ptr)) ptr++;
	sscanf(ptr,"%s",type);
	/*
	** scan to the next field
	*/
	while(*ptr != '\0' && !is_space_char(*ptr)) ptr++;
	while(*ptr != '\0' && is_space_char(*ptr)) ptr++;
	/*
	** there are no more fields on the line
	*/
	if(*ptr == '\0'){
	    /*
	    ** assume that the first field is the name and
	    ** the type is int
	    */
	    strcpy(name,type);
	    strcpy(type,"int");
	} else {
	    /*
	    ** skip any indirection
	    */
	    while(*ptr == '*') {
		strcat(type,"*");
		ptr++;
	    }
	    sscanf(ptr,"%s",name);
	}
	strcpy(table[count].name,name);
	strcpy(table[count].type,type);
	count++;
    }
    for(i=0;i<count;i++){
	printf("extern %s %s();\n",table[i].type,table[i].name);
    }
    for(i=0;i<count;i++){
	printf("HashFunc(\"%s\",%s,\"%s\");\n",
	table[i].name,table[i].name,table[i].type);
    }
    printf("}\n");
    printf("#undef PF\n");
}

int main(argc,argv)
int argc;
char **argv;
{
    if(argc < 3) {
	printf("usage: %s file name\n",argv[0]);
	return 0;
    }
    MakeFuncTableCode(argv[1],argv[2]);
    return 0;
}

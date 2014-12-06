static char rcsid[] = "$Id: code_sym.c,v 1.5 2005/06/29 16:38:34 svitak Exp $";

/*
** $Log: code_sym.c,v $
** Revision 1.5  2005/06/29 16:38:34  svitak
** Removed ifdef SYSV lines. If they don't work in system_deps.h, they won't
** work here. Also, removed void in front of generated INFO_ function calls.
** Apparently, "void functioncall();" is a no-op.
**
** Revision 1.4  2005/06/27 19:01:15  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.3  2005/06/25 22:04:40  svitak
** Added explicit return types to untyped functions. Fixed return statements.
**
** Revision 1.9  2001/06/29 21:30:22  mhucka
** Various error checks from Hugo C.
**
** Revision 1.8  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.6  2000/09/21 19:45:10  mhucka
** Defined the return types of some functions and added declarations at the top.
**
** Revision 1.5  2000/07/12 06:15:55  mhucka
** Added #include of stdlib.h, malloc.h and string.h, and
** removed unused variable "line" in MakeStructCode().
**
** Revision 1.4  1997/07/18 20:02:42  dhb
** Changes from PSC: allow whitespace between indirection chars (e.g. '*'
**   in parsing 'item names'.
**
** Revision 1.3  1997/05/29 09:09:45  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS20l revison: EDS BBF-UIA 95/12/25
 * Increased Info fields size
 *
** Revision 1.2  1994/04/14  17:49:10  dhb
** Changes from ngoddard@psc.edu to use caddr_t instead of ints in
** address calculations.
**
** Revision 1.1  1992/12/11  19:05:28  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_DIMENSIONS 4

#define MAX_DIMENSIONS 4

#define MAX_FIELDS_SIZE 2500

static int fields_size = 0;

int no_include = 0;

void CInc();
void SkipWhiteSpace();
void SkipComments();
void GetItemname();
void PrintIt();

int is_space_char(c)
char	c;
{
	return(c== '\n' || c == ' ' || c=='\t' || c=='\0');
}

int is_alpha(c)
char	c;
{
    return((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c=='_') ||
    (c >= '0' && c <= '9'));
}

#define Check(P)	if(*(P) == '\0') break;

void NextField(ptr)
char **ptr;
{
    while(!is_space_char(**ptr) && **ptr != '\0') CInc(ptr);
    SkipWhiteSpace(ptr);
}

void Inc(ptr)
char **ptr;
{
    if(**ptr != '\0') 
	(*ptr)++;
    else 
	return;
}

void CInc(ptr)
char **ptr;
{
    if(**ptr != '\0') 
	(*ptr)++;
    else 
	return;
    SkipComments(ptr);
}

void SkipComments(ptr)
char **ptr;
{
    if((**ptr == '/') && (*(*ptr +1) == '*')){
	while(**ptr != '\0' && strncmp(*ptr,"*/",2) != 0) (*ptr)++;
	if(**ptr != '\0') (*ptr) += 2;
    }
}

void SkipWhiteSpace(ptr)
char **ptr;
{
    SkipComments(ptr);
    while(is_space_char(**ptr) && **ptr != '\0') CInc(ptr);
}

int MakeStructCode(file,out,name,include)
char *file;
FILE *out;
char *name;
char *include;
{
FILE *fp;
char type[80];
char itemname[80];
char itemtype[80];
char tmp[80];
int indirection;
char *ptr;
char *tptr;
char *iptr;
short structure;
char *buf;
struct stat stbuf;
off_t fsize;
int count = 0;
int i;
char table[1000][80];
short dimensions;
int dimension_size[MAX_DIMENSIONS];

    /*
    ** open the function list file
    */
    if((fp = fopen(file,"r")) == NULL){
	fprintf(stderr,"unable to read function list file %s\n",file);
	return(0);
    }
    /*
    ** load the file into the buffer
    */
    stat(file,&stbuf);
    fsize = stbuf.st_size;
    buf = (char *)malloc(fsize+1);
    fread(buf,fsize,1,fp);
    buf[fsize] = '\0';
    fprintf(out,"%s\n",include);

    ptr = buf;
/* mds3 changes */
/* Only needed if compiling on a System V machine */
    fprintf(out,"#define __BZ BZERO(&info,sizeof(Info))\n");
    fprintf(out,"#define __IFI(F) info.field_indirection = F\n");
    fprintf(out,"#define __IFT info.function_type = 1\n");
    fprintf(out,"#define __IND(N) info.dimensions = N\n");
    fprintf(out,"#define __IDS(S,N) info.dimension_size[S] = N\n");

    while(*ptr != '\0'){
	/*
	*** skip white space
	*/
	SkipWhiteSpace(&ptr);
	Check(ptr);
	/*
	** COMPILER DIRECTIVES
	*/
	if(*ptr == '#'){
	    /*
	    ** literal include
	    */
	    while(*ptr != '\n'){
		if(!no_include)
		    fprintf(out,"%c",*ptr);
		Inc(&ptr);
	    }
	    fprintf(out,"\n");
	    continue;
	}
	sscanf(ptr,"%s",type);
	/*
	** STRUCT
	*/
	if(strcmp(type,"struct") == 0){
	    /*
	    ** scan to the next field
	    */
	    NextField(&ptr);
	    sscanf(ptr,"%s",type);
	} else
	/*
	** TYPEDEFS
	*/
	if(strcmp(type,"typedef") == 0){
	    /*
	    ** skip it
	    */
	    while(*ptr != '{' && 
	    *ptr != ';' &&
	    *ptr != '\0') Inc(&ptr);
	    if(*ptr == '{'){
		/*
		** find the matching bracket
		*/
		while(*ptr != '}' && *ptr != '\0') Inc(&ptr);
		NextField(&ptr);
	    }
	    NextField(&ptr);
	    continue;
	} else {
	    /*
	    ** ignore it by skipping to the the end
	    */
	    while(*ptr != ';' && *ptr != '\0') Inc(&ptr);
	    Check(ptr);
	    Inc(&ptr);
	    continue;
	}
	/*
	** STRUCT SPECIFICATION
	**
	** find the left bracket
	*/
	while(*ptr != '{' && *ptr != '\0') Inc(&ptr);
	if(*ptr == '\0'){
	    fprintf(stderr,"missing matching bracket\n");
	    return(0);
	}
	strcpy(table[count++],type);
	fprintf(out,"void INFO_%s(){\n",type);
	fprintf(out,"struct %s var;Info info;char fields[%i];fields[0]='\\0';info.name=\"%s\";info.type_size=sizeof(var);InfoHashPut(&info);\n",type,MAX_FIELDS_SIZE,type);

	/*- clear field string */

	fields_size = 0;

	/*
	** FIELDS
	**
	** process the item fields
	*/
	while(*ptr != '\0'){
	    NextField(&ptr);
	    /*
	    ** END OF STRUCTURE SPEC
	    */
	    if(*ptr == '}') break;
	    Check(ptr);
	    /*
	    ** ITEMTYPE
	    ** get the item type by reading the type specification up
	    ** to either white space or an asterisk
	    */
	    tptr = ptr;
	    iptr = itemtype;
	    while(!is_space_char(*tptr) && *tptr != '*'){
		*iptr++ = *tptr++;
	    }
	    *iptr = '\0';
	    structure = 0;
	    /*
	    ** is it a structure?
	    */
	    if(strcmp(itemtype,"struct") == 0){
		/*
		** then use the structure name as the type
		** and flag it as a structure
		*/
		NextField(&ptr);
		Check(ptr);
		sscanf(ptr,"%s",itemtype);
		structure = 1;
	    } else 
	    if(strcmp(itemtype,"unsigned") == 0){
		/*
		** then use the structure name as the type
		** and flag it as a structure
		*/
		NextField(&ptr);
		Check(ptr);
		sscanf(ptr,"%s",tmp);
		sprintf(itemtype,"unsigned %s",tmp);
	    } 
	    NextField(&ptr);
	    Check(ptr);
	    dimensions = 0;
	    GetItemname(&ptr,itemname,&indirection,&dimensions,dimension_size);
	    PrintIt(out,type,itemname,itemtype,indirection,structure,
	    dimensions,dimension_size);
	    /*
	    ** check for a comma or a semicolon
	    */
	    while(*ptr != ';'){
		while(*ptr != ';' && *ptr != ',') Inc(&ptr);
		if(*ptr == ','){
		    Inc(&ptr);
		    GetItemname(&ptr,itemname,&indirection,&dimensions,dimension_size);
		    PrintIt(out,type,itemname,itemtype,indirection,structure,
		    dimensions,dimension_size);
		}
	    }
	}
	fprintf(out,"FieldHashPut(\"%s\",fields);\n}\n",type);
	Check(ptr);
	NextField(&ptr);

	/*- if run-time limit reached */

	if (fields_size > MAX_FIELDS_SIZE)
	{
	    fprintf(stderr,"%s : warning : needed field size : %i, currently max size : %i\n","code_sym",fields_size + 1,MAX_FIELDS_SIZE);
	    fprintf(stderr,"%s : warning : Increase the value of MAX_FIELDS_SIZE in sys/code_sym.c\n","code_sym");
	    fprintf(stderr,"%s : warning : Afterwards do 'make clean' in this directory and the\n","code_sym");
	    fprintf(stderr,"%s : warning : directory sys/ and remake.\n","code_sym");
	}	
    }
    fprintf(out,"#undef __BZ\n");
    fprintf(out,"#undef __IFI\n");
    fprintf(out,"#undef __IFT\n");
    fprintf(out,"#undef __IND\n");
    fprintf(out,"#undef __IDS\n");
    /*
    ** make the calling function
    */
    fprintf(out,"void DATA_%s(){\n",name);
    for(i=0;i<count;i++){
	fprintf(out,"INFO_%s();\n",table[i]);
    }
    fprintf(out,"}\n");
    free(buf);
	return 1;
}

void GetItemname(ptr,itemname,indirection,dimensions,dimension_size)
char **ptr;
char *itemname;
int *indirection;
short *dimensions;
int dimension_size[MAX_DIMENSIONS];
{
char *tmp;
char *iptr;
char index[80];

    /*
    ** skip any white space
    */
    while(is_space_char(**ptr) && **ptr != '\0') Inc(ptr);
    /*
    ** skip any indirection
    */
    *indirection = 0;
    while(**ptr == '*' || (is_space_char(**ptr) && **ptr != '\0')) {
	if (**ptr == '*')
	  (*indirection)++;
	Inc(ptr);
    }
    /*
    ** get the itemname
    */
    tmp = itemname;
    while(is_alpha(**ptr)){
	*tmp = *(*ptr);
	tmp++;
	Inc(ptr);
    }
    /*
    ** get the array information
    */
    while(**ptr == '['){
	/*
	** get the index
	*/
	iptr = index;
	while(**ptr != ']'){
	    Inc(ptr);
	    *iptr++  = **ptr;
	}
	*iptr = '\0';
	Inc(ptr);
	dimension_size[(*dimensions)++] = atoi(index);
    }
    *tmp = '\0';
}

void PrintIt(out,type,itemname,itemtype,indirection,structure,dimensions,dimension_size)
FILE *out;
char *type;
char *itemname;
char *itemtype;
short indirection;
short structure;
short dimensions;
int dimension_size[MAX_DIMENSIONS];
{
short i;
    fprintf(out,"__BZ;");
    if(dimensions == 0){	/* is offset 32 or 64 bits? */
	fprintf(out,"info.name=\"%s.%s\";info.offset=(caddr_t)(&(var.%s))-(caddr_t)(&var);\t\t",type,itemname,itemname);
    } else {
	fprintf(out,"info.name=\"%s.%s\";info.offset=(caddr_t)((var.%s))-(caddr_t)(&var);\t\t",type,itemname,itemname);
    }
    fprintf(out,"info.type=\"%s\";",itemtype);
    if(structure){
	fprintf(out,"info.type_size=sizeof(struct %s);",itemtype);
    } else
	fprintf(out,"info.type_size=sizeof(%s);",itemtype);
    if(indirection > 0){
	fprintf(out,"__IFI(%d);",indirection);
    }
    if(dimensions > 0){
	fprintf(out,"__IND(%d);",dimensions);
	if(dimensions > MAX_DIMENSIONS){
	    fprintf(stderr,"too many array dimensions!!\n");
	}
	for(i=0;i<dimensions;i++){
	    fprintf(out,"__IDS(%d,%d);",i,dimension_size[i]);
	}
    }
    if(strncmp(itemtype,"PF",2) == 0){
	fprintf(out,"__IFT;");
    }
    fprintf(out,"InfoHashPut(&info);strcat(fields,\"%s\");strcat(fields,\"\\n\");\n",itemname);
    fprintf(out,"if (strlen(fields) > %i) { Error(); printf(\"Field size too long for object %s, field %s\\n**See sys/code_sym.c and increase the value of MAX_FIELDS_SIZE\\n\\n\"); }\n",MAX_FIELDS_SIZE,type,itemname);

    /*- if run-time limit reached */

    fields_size += 1 + strlen(itemname);

    if (fields_size > MAX_FIELDS_SIZE)
    {
	/*- give some diag's */

	fprintf(stderr,"%s : warning : maximum field size reached for structure (%s), field (%s)\n","code_sym",type,itemname);
	fprintf(stderr,"%s : warning : Currently needed : %i\n","code_sym",fields_size + 1);
    }
}

int main(argc,argv)
int argc;
char **argv;
{
int nxtarg;
char line[80];
char include[1000];
FILE *outfp;

    if(argc < 3) {
	printf("usage: %s file name [-o file][-I file -I ..]\n",argv[0]);
	exit(0);
    }
    nxtarg=2;
    include[0] = '\0';
    outfp = stdout;
    while(++nxtarg < argc){
	if(strcmp(argv[nxtarg],"-I") == 0){
	    sprintf(line,"#include \"%s\"\n",argv[++nxtarg]);
	    strcat(include,line);
	} else
	if(strcmp(argv[nxtarg],"-NI") == 0){
	    no_include = 1;
	} else
	if(strcmp(argv[nxtarg],"-o") == 0){
	    /*
	    ** open the function list file
	    */
	    if((outfp = fopen(argv[++nxtarg],"w")) == NULL){
		fprintf(stderr,"unable to write function list file %s\n",
		argv[nxtarg]);
		return(0);
	    }
	}
    }
    MakeStructCode(argv[1],outfp,argv[2],include);
    return(0);
}

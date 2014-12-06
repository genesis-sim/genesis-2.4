static char rcsid[] = "$Id: nsim_wildcard.c,v 1.2 2005/06/27 19:00:57 svitak Exp $";

/*
** $Log: nsim_wildcard.c,v $
** Revision 1.2  2005/06/27 19:00:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.12  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.11  2000/06/12 05:07:04  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.10  1996/05/30 19:28:12  venkat
** Defined types of arguments in WildcardGetElement()
** Systems with different pointer and int sizes (DEC alpha) no longer
** mishandle the passed args.
**
 * Revision 1.9  1996/05/16  22:07:13  dhb
 * Replaced explicit element path hash table lookup with
 * call to GetElement() which does the hash table lookup
 * anyways.
 *
 * Revision 1.8  1996/05/16  22:01:14  dhb
 * Support for element path hash table.
 * Before initiating a complicated wildcard search, check if the
 * path is a non-wildcard.  Look for the fullly qualified path in
 * the element path hash table.
 *
 * Revision 1.7  1995/11/07  00:43:52  dhb
 * Changed delimiter for multiple wildcard paths from space to
 * comma.  The delimiter is now specified using the WCP_DELIM
 * macro near the top of the file.
 *
 * Revision 1.6  1995/09/26  22:00:44  dhb
 * Changes from Upi Bhalla to wildcarding to accept multiple path
 * wildcards in a wildcard path specification including all elements
 * in all paths in the list.
 *
 * Revision 1.5  1994/05/27  21:43:31  dhb
 * Added the [ISA=object] test to element wildcarding.  If the given object
 * name matches the element object name or the object name of any base object
 * of the element object then the element is selected.
 *
 * Also removed some dead code.
 *
 * Revision 1.4  1993/09/17  18:18:34  dhb
 * Fixed access to previously freed memory in ClearPinfo().
 *
 * Revision 1.3  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.2  1993/03/10  23:11:35  dhb
 * Extended element fields
 *
 * Revision 1.1  1992/10/27  19:24:17  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

/*
** valid wildcard path specifications:
**
** path:	/path 	identifier 	identifier/path
** identifier:	. 	.. 		name[index][class][type][var]
** name:	a#b 	a?b		##	#	""
** index:	[] 	[n] 		[n-m]
** class:	CLASS op classname
** type:	TYPE op objectname    --> just for back compatibility
** type:	OBJECT op objectname  --> this is the correct one
** type:	ISA op objectname --> if its derived from objectname
** var:		field=val	field<>val	field!=val
**		field>val	field<val
** 		field>=val	field<=val	
*/

/* Character for delimiting multiple wildcard paths */

#define	WCP_DELIM	','

/* test op codes */
#define BAD	-1
#define NOP	0
#define GT	1
#define LT	2
#define GE	3
#define LE	4
#define EQ	5
#define NE	6
#define AND	7

#define NUMMODE		0
#define STRMODE		1
#define CLASSMODE	2
#define ISAMODE		3

typedef struct test_type {
    char	field_id;
    char	name[80];
    short	op;
    float	value;
    struct test_type *next;
} Test;

typedef struct {
    char	path[1000];
    Test	*test;
} PathInfo;

Test *AddTest(pinfo)
PathInfo *pinfo;
{
Test *test;

    test = (Test *)calloc(1,sizeof(Test));
    /*
    ** insert into list
    */
    test->next = pinfo->test;
    pinfo->test = test;
    return(test);
}

void ClearPinfo(pinfo)
PathInfo *pinfo;
{
Test *test;
Test *nexttest;

    if(pinfo== NULL) return;
    for(test=pinfo->test;test;test=nexttest){
	nexttest = test->next;
	free(test);
    }
    pinfo->test = NULL;
}

/*
** returns the op code and leaves the string pointer at the first
** location following the op string
*/
int LocateOp(str)
char **str;
{
    /*
    ** skip to the first valid op character
    */
    while(**str != '\0' && 
	**str != ']' && 
	**str != '[' && 
	**str != '=' &&
	**str != '&' &&
	**str != '<' &&
	**str != '>' &&
	**str != '!'){
	(*str)++;
    }
    if(**str == '='){
	(*str)++;
	return(EQ);
    }
    if(**str == '!' && *((*str)+1) == '='){
	(*str)++;
	(*str)++;
	return(NE);
    }
    if(**str == '<' && *((*str)+1) == '>'){
	(*str)++;
	(*str)++;
	return(NE);
    }
    if(**str == '<' && *((*str)+1) == '='){
	(*str)++;
	(*str)++;
	return(LE);
    }
    if(**str == '>' && *((*str)+1) == '='){
	(*str)++;
	(*str)++;
	return(GE);
    }
    if(**str == '>'){
	(*str)++;
	return(GT);
    }
    if(**str == '<'){
	(*str)++;
	return(LT);
    }
    if(**str == '&'){
	(*str)++;
	return(AND);
    }
    return(BAD);
}

char *GetIdent(str,ident)
char *str;
char *ident;
{
    /*
    ** skip to the first valid op character
    */
    while(*str != '\0' && 
    *str != ']' && 
    *str != '[' && 
    *str != '=' &&
    *str != '<' &&
    *str != '>' &&
    *str != '!'){
	*ident = *str;
	ident++;
	str++;
    }
    *ident = '\0';
    return str;
}

char *NextDelimiter(s)
char *s;
{
    if(s == NULL) return(NULL);
    while(*s != '[' && *s != ']' && *s != '\0'){
	s++;
    }
    if(*s == '['){
	return(s);
    }
    if(*s == ']'){
	Error();
	printf("missing '[' delimiter\n");
	return(NULL);
    }
    return(NULL);
}

char *CloseDelimiter(s)
char *s;
{
    if(s == NULL) return(NULL);
    while(*s != '[' && *s != ']' && *s != '\0'){
	s++;
    }
    if(*s == ']'){
	return(s+1);
    }
    if(*s == '['){
	Error();
	printf("missing ']' delimiter\n");
	return(s);
    }
    return(s);
}

int MatchWildcard(s,pattern)
char	*s;
char	*pattern;
{
char *wild;
int last_len;
int first_len;
int wild_len=1;

    /*
    ** the pattern can be of the form a#b
    ** or a???b
    */
    if((wild = strchr(pattern,'?')) != NULL){
	/*
	** if it is the single character match then check for length
	** requirements
	*/
	if(strlen(s) != strlen(pattern)){
	    return(0);
	}
	while(*(wild +wild_len) == '?'){
	    wild_len++;
	}
    } else
    if((wild = strchr(pattern,'#')) == NULL){
	/*
	** just do simple matching
	*/
	return(strcmp(s,pattern) == 0);
    }
    first_len = (int)(wild - pattern);
    last_len = (int)strlen(wild+wild_len);
    /*
    ** match the first part of the pattern
    */
    if(strncmp(s,pattern,first_len) != 0){
	return(0);
    }
    /*
    ** match the last part of the pattern
    */
    if(strncmp(s + strlen(s) - last_len, wild+wild_len,last_len) != 0){
	return(0);
    }
    return(1);
}

/*
** Search for / which separate levels in the tree.
** Terminate the local path at the separator and return the
** string starting at the separator character
*/
char *LocalPath(path,local)
char *path;
char *local;
{
    /*
    ** copy the path up to the separator into local_path
    */
    while (*path != '/' && *path != '\0') {
	*local = *path;
	path++; 
	local++;
    }
    *local = '\0' ;
    /*
    ** return the string following the separator
    */
    if(*path != '\0')
	return(path+1);
    else
	return(NULL);
}


/*
** parse the path string
*/
void GetPathInfo(path,pinfo)
char *path;
PathInfo *pinfo;
{
char	tmp[100];
char	*pathstr;
char 	*str;
char	field_id;
int	op;
Test	*test;
char		ident[100];

    /*
    ** initialize the pinfo fields
    */
    strcpy(pinfo->path,path);
    pathstr = pinfo->path;
    pinfo->test = NULL;
    /*
    ** look for the [ delimiter indicating a modifier
    */
    while ((pathstr = NextDelimiter(pathstr))) {
	pathstr++;
	pathstr = GetIdent(pathstr,ident);
	/*
	** INDEX
	*/
	if(*ident == '\0'){
	    /*
	    ** an empty modifier is a shorthand for all indices 
	    ** which is equivalent to no index checking
	    ** The NOP is needed to prevent the default index 0
	    ** from being added when no index is specified.
	    */
	    test = AddTest(pinfo);
	    test->field_id = 'i';
	    test->op = NOP;
	    pathstr = CloseDelimiter(pathstr);
	    continue;
	} 
	if(*ident >= '0' && *ident <= '9'){
	    /*
	    ** index notation
	    */
	    if(strchr(ident,'-')){
		/*
		** shorthand index range notation
		*/
		test = AddTest(pinfo);
		test->field_id = 'i';
		test->op = GE;
		sscanf(ident,"%f-",&test->value);
		test = AddTest(pinfo);
		test->field_id = 'i';
		test->op = LE;
		sscanf(strchr(ident,'-'),"-%f]",&test->value);
	    } else {
		/*
		** a number alone is shorthand for an index equality comparison
		*/
		test = AddTest(pinfo);
		test->field_id = 'i';
		test->op = EQ;
		sscanf(ident,"%f]",&test->value);
	    }
	    pathstr = CloseDelimiter(pathstr);
	    continue;
	}
	/*
	**  TYPE   [TYPE=objname]
	*/
	if((strcmp(ident,"TYPE") == 0) ||
	(strcmp(ident,"OBJECT") == 0)){
	    /*
	    ** get the operator
	    */
	    if((op = LocateOp(&pathstr)) == BAD){
		printf("illegal operator in the path specification '%s'\n",
		ident);
		break;
	    }
	    /*
	    ** add the new type test to the list
	    */
	    test = AddTest(pinfo);
	    test->op = op;
	    test->field_id = 't';
	    str = test->name;
	    /*
	    ** get the type name
	    */
	    while(*pathstr != ']' && *pathstr != '\0'){
		*str = *pathstr;
		str++;
		pathstr++;
	    }
	    *str ='\0';
	    pathstr = CloseDelimiter(pathstr);
	    continue;
	}
	/*
	**  ISA   [ISA=objname]
	*/
	if(strcmp(ident,"ISA") == 0){
	    /*
	    ** get the operator
	    */
	    if((op = LocateOp(&pathstr)) == BAD){
		printf("illegal operator in the path specification '%s'\n",
		ident);
		break;
	    }
	    /*
	    ** add the new type test to the list
	    */
	    test = AddTest(pinfo);
	    test->op = op;
	    test->field_id = 'o';
	    str = test->name;
	    /*
	    ** get the type name
	    */
	    while(*pathstr != ']' && *pathstr != '\0'){
		*str = *pathstr;
		str++;
		pathstr++;
	    }
	    *str ='\0';
	    pathstr = CloseDelimiter(pathstr);
	    continue;
	}
	/*
	**  CLASS   [CLASS=classname]
	*/
	if(strcmp(ident,"CLASS") == 0){
	    /*
	    ** get the operator
	    */
	    if((op = LocateOp(&pathstr)) == BAD){
		printf("illegal operator in the path specification '%s'\n",
		ident);
		break;
	    }
	    /*
	    ** add the new class test to the list
	    */
	    test = AddTest(pinfo);
	    test->op = op;
	    test->field_id = 'c';
	    str = tmp;
	    /*
	    ** get the class name
	    */
	    while(*pathstr != ']' && *pathstr != '\0'){
		*str = *pathstr;
		str++;
		pathstr++;
	    }
	    *str ='\0';
	    pathstr = CloseDelimiter(pathstr);
	    /*
	    ** lookup the class name
	    */
	    if((test->value = ClassID(tmp)) == INVALID_CLASS){
		Error();
		printf("invalid class '%s'.\n",tmp);
	    }
	    continue;
	}
	/*
	**  RANGE   [x,y,z=value]
	*/
	if((strcmp(ident,"x") == 0) ||
	(strcmp(ident,"y") == 0) ||
	(strcmp(ident,"z") == 0)){
	    field_id = *ident;
	    /*
	    ** get the operator
	    */
	    if((op = LocateOp(&pathstr)) == BAD){
		printf("illegal operator in the path specification '%s'\n",
		pathstr);
		break;
	    }
	    /*
	    ** add the new test to the list
	    */
	    test = AddTest(pinfo);
	    test->op = op;
	    test->field_id = field_id;
	    /*
	    ** get the range value
	    */
	    sscanf(pathstr,"%f]",&test->value);
	    pathstr = CloseDelimiter(pathstr);
	    continue;
	}
	/*
	**  FIELD   [field=value]
	*/
	/*
	** get the operator
	*/
	if((op = LocateOp(&pathstr)) == BAD){
	    printf("illegal operator in the path specification '%s'\n",
	    ident);
	    break;
	}
	/*
	** add the new field test to the list
	*/
	test = AddTest(pinfo);
	test->op = op;
	test->field_id = 'f';
	/*
	** get the field name
	*/
	strcpy(test->name,ident);
	/*
	** get the field value
	*/
	sscanf(pathstr,"%f]",&test->value);
	pathstr = CloseDelimiter(pathstr);
	continue;
    }
    /*
    ** check for the default indexing case of 0
    */
    if(pinfo->test == NULL){
	test = AddTest(pinfo);
	test->field_id = 'i';
	test->op = EQ;
	test->value = 0;
    }

    /*
    ** clear the extra stuff at the end of the path
    ** by searching for the first option delimiter and zeroing it
    */
    if((pathstr = strchr(pinfo->path,'['))){
	*pathstr = '\0';
    }
}

int CheckOptions(element,pinfo)
Element	*element;
PathInfo pinfo;
{
Test 	*test;
int 	reject = FALSE;
float 	val;
char 	*strval;
char 	*str;
int 	mode;

    for(test=pinfo.test;test;test=test->next){
	mode = NUMMODE;
	switch(test->field_id){
	case 'i':			/* INDEX */
	    val = element->index;
	    break;
	case 'o':			/* ISA object */
	    mode = ISAMODE;
	    break;
	case 't':			/* TYPE */
	    strval = element->object->name;
	    mode = STRMODE;
	    break;
	case 'x':			/* X */
	    val = element->x;
	    break;
	case 'y':			/* Y */
	    val = element->y;
	    break;
	case 'z':			/* Z */
	    val = element->z;
	    break;
	case 'c':			/* CLASS */
	    mode = CLASSMODE;
	    break;
	case 'f':			/* FIELD */
	    str = ElmFieldValue(element,test->name);
	    if(str == NULL){
		/*
		** doesnt have the field
		*/
		return(0);
	    }
	    val = Atof(str);
	    break;
	default:
	    return(0);
	    /* NOTREACHED */
	    break;
	}
	switch(mode){
	/*
	** check object isa
	*/
	case ISAMODE:
	    reject = ElementIsA(element, test->name);
	    if (test->op == EQ)
		reject = !reject;
	    break;
	/*
	** check string fields
	*/
	case STRMODE:
	    switch(test->op){
	    case EQ:
		if(strcmp(test->name,strval) != 0){
		    reject = TRUE;
		}
		break;
	    case NE:
		if(strcmp(test->name,strval) == 0){
		    reject = TRUE;
		}
		break;
	    }
	    break;
	case CLASSMODE:
	    switch(test->op){
	    case EQ:
		if(!CheckClass(element,(int)(test->value))){
		    reject = TRUE;
		}
		break;
	    case NE:
		if(CheckClass(element,(int)(test->value))){
		    reject = TRUE;
		}
		break;
	    }
	    break;
	case NUMMODE:
	    /*
	    ** check numeric fields
	    */
	    switch(test->op){
	    case EQ:
		if(val != test->value){ 
		    reject = TRUE; 
		}
		break;
	    case NE:
		if(val == test->value ){ 
		    reject = TRUE; 
		}
		break;
	    case GE:
		if(val < test->value ){
		    reject = TRUE;
		}
		break;
	    case LE:
		if(val > test->value){
		    reject = TRUE;
		}
		break;
	    case LT:
		if(val >= test->value){
		    reject = TRUE;
		}
		break;
	    case GT:
		if(val <= test->value){
		    reject = TRUE;
		}
		break;
	    case AND:
		if(!((int)val & (int)(test->value))){
		    reject = TRUE;
		}
		break;
	    }
	}
	if(reject) return(0);
    }
    return(1);
}

/*
** use the path specification to fill the element list
**
** blockmode parameter now has two functions.  The previous function
** of blocking inclusion of elements under a blocked element remains
** under control of the (blockmode&0x1) bit.  In addition, the
** (blockmode&0x2) bit controls inclusion of elements which are
** components of an extended object.  When the bit is set, component
** elements are visible.
*/

void FillList(parent,resultlist,blockmode,pinfo)
Element		*parent;
ElementList	*resultlist;
int		blockmode;
PathInfo	pinfo;
{
ElementStack 	*stk;
Element 	*element;

    /*
    ** CASE: .
    */
    if(strcmp(pinfo.path,"^") == 0){
	if((element = RecentElement()) != NULL &&
		((blockmode&0x2) || VISIBLE(element))){
	    AddElementToList(element,resultlist);
	}
	return;
    }
    /*
    ** CASE: .
    */
    if(strcmp(pinfo.path,".") == 0){
	AddElementToList(parent,resultlist);
	return;
    }
    /*
    ** CASE: ..
    */
    if(strcmp(pinfo.path,"..") == 0){
	AddElementToList(parent->parent,resultlist);
	return;
    }
    /*
    ** CASE: 	##
    */
    if(strcmp(pinfo.path,"##") == 0){
	/*
	** fill the list with all descendents of the parent
	*/
	stk = NewPutElementStack(parent);
	while ((element = NewFastNextElement(blockmode,stk))) {
	    if(CheckOptions(element,pinfo)){
		AddElementToList(element,resultlist);
	    }
	}
	NewFreeElementStack(stk);
	return;
    }
    /*
    ** CASE: 	#
    */
    if(strcmp(pinfo.path,"#") == 0){
	for(element=parent->child;element;element=element->next){
	    if(!(blockmode&0x1) || (IsEnabled(element))){
		if ((blockmode&0x2) || VISIBLE(element))
		    if(CheckOptions(element,pinfo)){
			AddElementToList(element,resultlist);
		    }
	    }
	}
	return;
    }
    /*
    ** Handle partial string matching
    **
    ** CASE:		a#b a?b
    */
    if(strchr(pinfo.path,'#') || strchr(pinfo.path,'?')){
	for(element=parent->child;element;element=element->next){
	    /*
	    ** check the name
	    */
#ifdef FIXBLOCKMODE
	    if(!blockmode || (IsEnabled(element))){
#endif
		if ((blockmode&0x2) || VISIBLE(element))
		    if(MatchWildcard(element->name,pinfo.path)){
			if(CheckOptions(element,pinfo)){
			    AddElementToList(element,resultlist);
			}
		    }
#ifdef FIXBLOCKMODE
	    }
#endif
	}
    } else {
	/*
	** Normal case without wildcards
	*/
	for(element=parent->child;element;element=element->next){
#ifdef FIXBLOCKMODE
	    if(!blockmode || (IsEnabled(element))){
#endif
	    /*
	    ** check the name
	    */
	    if ((blockmode&0x2) || VISIBLE(element))
		if(*pinfo.path == '\0' || strcmp(element->name,pinfo.path) == 0){
		    if(CheckOptions(element,pinfo)){
			AddElementToList(element,resultlist);
		    }
		}
#ifdef FIXBLOCKMODE
	    }
#endif
	}
    }
}


/*
** if blockmode is set then blocked element subtrees will be ignored
*/
ElementList *SubWildcardGetElement(path,blockmode)
char	*path;
int	blockmode;
{
ElementList	*parentlist;
ElementList	*childlist;
ElementList	*tmp;
ElementList	*CreateElementList();
Element		*element;
Element		*parent;
char		*local;
int		i;
PathInfo	pinfo;
    
    parentlist = CreateElementList(10);
    if(path == NULL){
	return(parentlist);
    }

    /*
    ** Try the path as a non-wildcard
    */
    element = GetElement(path);
    if (element != NULL) {
	AddElementToList(element, parentlist);
	return parentlist;
    }

    /*
    ** Path is not a simple single element path; do non-hash
    ** search
    */

    if(*path == '/') {
	element = RootElement();
	path++;
    } else {
	element = WorkingElement();
    }
    childlist = CreateElementList(10);
    /*
    ** add the parent to the list
    */
    AddElementToList(element,parentlist);
    /*
    ** go through each component of the path 
    */
    local = (char *)malloc(strlen(path)+1);
    while(path){
	/*
	** get the next path component
	*/
	path = LocalPath(path,local);
	/* 
	** skip empty paths which can arise from this sort of
	** specification /path//path
	*/
	if(local[0] == '\0')
	    continue;
	/*
	** empty the childlist
	*/
	childlist->nelements = 0;
	/*
	** go through all of the parents
	*/
	GetPathInfo(local,&pinfo);
	for(i=0;i<parentlist->nelements;i++){
	    parent = parentlist->element[i];
	    FillList(parent,childlist,blockmode,pinfo);
	}
	ClearPinfo(&pinfo);
	/*
	** swap lists. Make the child the new parent list for the next pass
	*/
	tmp = parentlist;
	parentlist = childlist;
	childlist = tmp;
    };
    free(local);
    FreeElementList(childlist);
    return(parentlist);
}

/*
** if blockmode is set then blocked element subtrees will be ignored
**
** This wrapper for the old WildcardGetElement looks for spaces
** separating individual wildcard paths, and scans through each
** individual path separately. Then it concatenates the whole mess.
** At some point we may wish to get rid of duplicates, but later.
*/
ElementList *WildcardGetElement(path,blockmode)
char *path;
int blockmode;
{
	char *currpath;
	char *subpath;
	char *origpath = CopyString(path);
	int	npaths;
	int i,j,k;
	int nelms;
	ElementList	*CreateElementList();
	ElementList *elists[100];
	ElementList *retlist;
		/* 100 is an absurdly large number, but it is not expensive */

	for (npaths = 0, currpath = origpath;
		currpath && *currpath && npaths < 100;
		currpath = subpath, npaths++) {
		/* look for a space in the currpath */
 		subpath = strchr(currpath,(int)WCP_DELIM);
 		if (subpath) { /* terminate the path so that currpath
 					 ** will be understood by SubWildcardGetElement */
 			*subpath = '\0';
			subpath++; /* this is so that the next currpath will advance */
		}
		elists[npaths] = SubWildcardGetElement(currpath,blockmode);
	}
	if (npaths == 1) /* shortcut if there is only one elist */
		return(elists[0]);

	/* here we have to allocate a big elist for all the parts */
	/* later we may wish to put in a check to avoid duplications */
	nelms = 0;
	for (i = 0; i < npaths; i++)
		nelms += elists[i]->nelements;
	retlist = CreateElementList(nelms);
	retlist->nelements = nelms;
	k = 0;
	for (i = 0; i < npaths; i++) {
		for (j = 0; j < elists[i]->nelements; j++, k++)
			retlist->element[k] = elists[i]->element[j];
		FreeElementList(elists[i]);
	}

	free(origpath);
	return(retlist);
}

#undef BAD
#undef NOP
#undef GT
#undef LT
#undef GE
#undef LE
#undef EQ
#undef NE
#undef AND


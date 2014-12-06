static char rcsid[] = "$Id: sim_dump.c,v 1.3 2005/10/24 06:33:05 svitak Exp $";

/*
** $Log: sim_dump.c,v $
** Revision 1.3  2005/10/24 06:33:05  svitak
** Fixed decls missing types for argc and argv.
**
** Revision 1.2  2005/06/27 19:00:59  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.15  2003/03/28 20:43:33  gen-dbeeman
** *** empty log message ***
**
** Revision 1.14  2001/06/29 21:27:37  mhucka
** Fixes from Hugo C.
**
** Revision 1.13  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.11  2000/06/12 04:52:17  mhucka
** 1) Added a missing typecast in DumpMessages().
** 2) Added NOTREACHED comments where appropriate.
**
** Revision 1.10  1998/08/28 17:13:25  dhb
** Added options processing code for do_substitute_info().
**
** Revision 1.9  1998/07/21 18:58:50  dhb
** Fixed ANSI C isms.
**
 * Revision 1.8  1998/07/15 06:52:26  dhb
 * Upi update
 *
 * Revision 1.5  1995/09/26 22:00:44  dhb
 * Changes from Upi Bhalla:
 *
 * 	Special handling of first message in dump restores.
 *
 * 	Use of loadtab -continue for long lists of table data.
 *
 * Revision 1.4  1995/02/22  19:02:04  dhb
 * Update from Upi.
 *
 * Revision 1.7  1994/10/03  20:21:01  bhalla
 * Added in facility (on by default) for merging files with overlapping
 * messages. Does this by aliasing the usual addmsg command with a
 * dump-specific addmsg. This dump-specific version checks if the
 * addmsg is between pre-existing elements, and is the same msgtype
 * as an existing msg between those elements. In this case the message
 * is not added, otherwise it is. This turns out to be a phenomenally
 * useful option !
 *
 * Revision 1.6  1994/09/29  19:02:27  bhalla
 * trivial bugfix re ignoreorphans
 *
 * Revision 1.5  1994/09/29  18:29:10  bhalla
 * Added IgnoreOrphans flag. This option tells the undump to ignore
 * all elements whose parents are missing
 * -- Also fixed bug with fields. If the number of fields is changed
 * in a new version of an object, the bug caused an error when the
 * default fieldlist did not match the fieldlist being loaded in. The
 * new version just identifies and NULLs out fields that no longer
 * exist, so it can read old objects.
 *
 * Revision 1.4  1994/06/30  21:47:30  bhalla
 * Added FilterStr and ParseString functions to enable the dumps to
 * handle long strings with carriage returns and quotes.
 *
 * Revision 1.3  1994/06/13  22:32:11  bhalla
 * Update from Upi's current version
 *
 * Revision 1.3  1994/06/13  20:27:57  bhalla
 * FORMAT CHANGE: added an obligatory first field for the flags of the element.
 * Added code for handling the DumpVersion. This is dump version 3. This
 * makes it possible to automatically identify the sim-dump version and
 * read old ones. This version can cleanly read old (version 2) dumpfiles.
 * Added the do_swap_dumplist command which is used when one wants to
 * keep the saving dumplist separate from the loading dumplist.
 *
 * Revision 1.2  1994/04/08  23:25:39  dhb
 * Update from Upi
 *
 * Revision 1.6  1994/04/04  17:39:21  bhalla
 * Corrected problem in DumpMsgFields where it failed to save
 * string fields in messages correctly.
 *
 * Revision 1.5  1994/04/04  16:56:17  bhalla
 * Added facility for combining multiple -path options to simdump
 *
 * Revision 1.4  1994/04/04  16:40:20  bhalla
 * Belated RCS entry of stuff I did for handling messages in dumps
 *
 * Revision 1.3  1994/02/22  22:30:46  bhalla
 * Added code to handle 'pre' and 'post' calls to the DUMP action
 * Added the DumpInterpol utility function for dumping tables.
 *
 * Revision 1.2  1994/02/17  15:02:20  bhalla
 * First pass at complete dump functionality. It works, but with severe
 * limitations and many 'features'.
 *
 * Revision 1.1  1994/02/16  22:16:57  bhalla
 * Initial revision
 * */

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"
#include "hash.h"

/* DUMPVERSION must be an integer */
#define DUMPVERSION 3

/* 
** This set of functions is meant to allow the complete state of
** a simulation to be dumped and restored.
**
** simobjdump: This function outputs the name of each
**	object and a list of the object fields stored in this file. This
**	arrangement, although it makes for a largish header, also keeps
**	the dump files readable even if the dump fields change.
**	The simobjdump function is distinct from the saveobj function
**	which writes a library file with all the information needed
** 	to reconstruct an object.
**
** simobjundump: This function is used to read in info stored using
**		simobjdump.

**
** simobjdump: This function loads in the list of the object fields
**	saved during the dump, and puts them in the dumplist. It is used
**	both during dumping and undumping. The actual info is saved to
**	the dump file by simdump if it is dumping.
**
** simdump: The output of the
**	simdump function is a script file with standard script calls.
**	To speed up reconstruction, a new function 'simundump' has
**	been implemented, which does the creation and field assignment
**	of elements, with some extra options.
**	The information needed to rebuild messages is saved simply by
**	outputting a addmsg command with suitable args, for every message
**	found in the dump list.
**
** simundump: Function to create and restore values to an element.
**	Uses the list of fields specified in simobjdump.
**
** This method of handling dumps and restores is slow and the
** dumpfiles are large. However, it is pretty general and should be
** fairly immune to version incompatibilities.
**
*/

struct dumplist_type {
	int nfields;
	char	**fields;
	int	do_dump_action;
	GenesisObject	*object;
};

struct obj_substitute_type {
	char *orig;
	char *new;
};

struct msg_substitute_type {
	char *destobj;
	char *orig_msgtype;
	int argc;
	char *argv[10];
};

#define N_SUBSTITUTE 100

static int n_obj_substitute = 0;
static int n_msg_substitute = 0;
static struct obj_substitute_type obj_substitute[N_SUBSTITUTE];
static struct msg_substitute_type msg_substitute[N_SUBSTITUTE];
static int msg_substitution = 0;
static int targc = 0;
static char *targv[10];

#define MSGDUPELISTSIZE 100

static struct dumplist_type *DumpList = NULL;
static struct dumplist_type *SwapDumpList = NULL;
static int 	DumpListSize = -1;
static int 	SwapDumpListSize = -1;
static int	CurrDumpListNum = 0;
static int	DumpVersion = 3;
static int	IgnoreOrphans = 0;
/* This flag tells us to ignore elements whose parents are missing.
** Normally this situation would give rise to an error message */

static int	AllowMsgDup = 0;
/* This flag disables the mechanism that protects dumps from
** duplicating msgs when the elements specified in the dumps overlap 
*/
static ElementList	*MsgDupElist;

static struct dumplist_type *FindOnDumpList();
static char *AppendField();
static char *ParseString();

static void DumpMessages();
static void DumpMsgIn();
static char *FindFieldName();
static void DumpMsgFields();
static void DumpElements();
static ADDR dump_hash_function();
static ENTRY *dump_hash_find();
static ENTRY *dump_hash_enter();
void do_dump_add_msg(); /* Forward declaration */
#ifdef __STDC__
char* do_dump_call(int argc, char** argv); /* Forward declaration */
char* do_call_element(int argc, char** argv); /* Original declaration*/
#else
char* do_dump_call(); /* Forward declaration */
char* do_call_element(); /* Original declaration*/
#endif

void do_simobjdump(argc,argv)
	int 	argc;
	char	**argv;
{
	int status;
	int		use_coords = 0;
	int		use_default = 0;
	char	*objname;
	FILE	*fopen();
	int		do_dump_action = 1;

	/* later might add -script args */
	initopt(argc,argv,
		"object ... -coords -default -noDUMP");

	while ((status = G_getopt(argc,argv)) == 1) {
#if 0
		if (strcmp(G_optopt,"-tree") == 0) {
			Element	*tree;
			/* If it is using a treepath, then it assumes that
			** the objects listed know their own list of fields,
			** or are happy with the defaults. It does not try
			** to use the fieldlist from -object */
			if (tree = GetElement(optargv[1])) {
				kk
			} else {
				Error();
				printf("Cannot find tree %s\n",optargv[1]);
				break;
			}
		}
#endif
		if (strcmp(G_optopt,"-coords") == 0) {
			use_coords = 1;
			/* If use_coords is set, it will add the x y z fields
			** to every object saved */
		}
		if (strcmp(G_optopt,"-noDUMP") == 0) {
			do_dump_action = 0;
			/* This option suppresses the call to the DUMP action.
			** it is useful if you do not want a table to be
			** dumped automatically */
		}
		if (strcmp(G_optopt,"-default") == 0) {
			use_default = 1;
			/* if -default is set then  it uses the default fieldlist */
		}
	}
	if (status < 0) {
		printoptusage(argc,argv);
		return;
	}

	objname = optargv[1];
	AddObjToDumpList(objname,optargv+2,
		use_default ? 0 :  optargc-2,
		use_coords,do_dump_action);

}

int AddObjToDumpList(objname,fields,nfields,use_coords,do_dump_action)
	char *objname;
	char **fields;
	int nfields;
	int use_coords;
	int do_dump_action;
{
	GenesisObject	*object;
	char	*fieldlist[50];
	int		i;
	int		k = 0;
	struct dumplist_type *dumpitem;
	char	**items;

	if (!(object = GetObject(objname))) {
		if (IgnoreOrphans)
			return(1);
		Error(); printf("Could not find object '%s'\n",objname);
		return(0);
	}

	do_initdump(0,NULL);

	if (nfields > 0) { /* use the specified fieldlist rather than
						** the defaults */
		for(i=0; i < nfields; i++) {
			/* screen out invalid fields */
			if (FieldListExists(object, fields[i])) {
				fieldlist[k] = fields[i];
			} else {
				fieldlist[k] = NULL;
			}
			k++;
		}
	} else { /* Use the default field list */
		FieldList *fl = object->fieldlist;
		char	name[40];
		Info	info;

		while(fl != NULL) {
			int prot = fl->flags & FIELD_PROT;
			if (prot == FIELD_READWRITE) {
				if (fl->flags & FIELD_EXTENDED) {
					fieldlist[k] = fl->name;
					k++;
					fl = fl->next;
					continue;
				}
				if (use_coords || !(
					strcmp("x",fl->name) == 0 ||
					strcmp("y",fl->name) == 0 ||
					strcmp("z",fl->name) == 0)) {
					sprintf(name,"%s.%s",object->type,fl->name);
					if (GetInfo(name,&info)) {
						if (info.dimensions == 0 &&
							(info.field_indirection == 0 ||
							(info.field_indirection == 1 &&
							strcmp(info.type,"char") == 0))) {
							fieldlist[k] = fl->name;
							k++;
						}
					}
				}
			}
			fl = fl->next;
		}
	}
/*
	if (use_coords) {
		fieldlist[k] = "x"; k++;
		fieldlist[k] = "y"; k++;
		fieldlist[k] = "z"; k++;
	}
*/
	nfields = k;

	if ((dumpitem = FindOnDumpList(object))) {
		/* Get rid of last entry */
		if (dumpitem->nfields > 0) {
			int j;
			for(j = 0; j < dumpitem->nfields; j++) {
				free(dumpitem->fields[j]);
			}
			free(dumpitem->fields);
		}
		/* Fix up new version of entry */
		dumpitem->nfields = nfields;
		if (nfields > 0) {
			items = dumpitem->fields = (char **) calloc(
				nfields, sizeof(char *));
			for(i = 0; i < nfields; i++) {
				items[i] = CopyString(fieldlist[i]);
			}
		}
		dumpitem->do_dump_action = do_dump_action;
	} else {
		DumpList[DumpListSize].object = object;
		DumpList[DumpListSize].nfields = nfields;
		DumpList[DumpListSize].do_dump_action = do_dump_action;
		if (nfields > 0) {
			items =
				DumpList[DumpListSize].fields = (char **) calloc(
				nfields, sizeof(char *));
			for(i = 0; i < nfields; i++) {
				items[i] = CopyString(fieldlist[i]);
			}
		}
		DumpListSize++;
	}
	return 1;
}

#define DUMPLINEWIDTH 76
#define DUMPLINELENGTH 1000
#define DUMPFIELDLENGTH 800

static int FilterStr(str,retstr,do_quote)
	char *str;
	char **retstr;
	int *do_quote;
{
	static char newstr[DUMPFIELDLENGTH];
	int i,j;
	char curr;
	*do_quote = 0;
	*retstr = newstr;

	for(i = 0,j = 0; j < (DUMPFIELDLENGTH - 1); i++,j++) {
		curr = str[i];
		switch(curr) {
			case ' ':
				*do_quote = 1;
				newstr[j] = curr;
			break;
			case '\n':
				*do_quote = 1;
				newstr[j] = '\\';
				j++; newstr[j] = 'n';
			break;
			case '"':
				*do_quote = 1;
				newstr[j] = '\\';
				j++; newstr[j] = '"';
			break;
			case '\0':
				newstr[j] = curr;
				return(j);
				/* NOTREACHED */
			break;
			default:
				newstr[j] = curr;
			break;
		}
	}
	Warning();
	printf("Line too long for simdump. Truncated at %d chars\n",
		DUMPFIELDLENGTH - 1);
	newstr[j] = '\0';
	return(j);
}

static char *AppendField(line,str,lastline,field)
	char	*line;
	char	*str;
	char	**lastline;
	char	*field;
{
	char *filteredfield;
	int do_quote;
	int len = FilterStr(field,&filteredfield,&do_quote);
	if (len == 0)
		do_quote = 1;
	if (do_quote)
		len += 2;

#if 0
	/* Check for empty strings or strings with blanks in them */
	if (len == 0 || strchr(field,' ')) {
		len += 2;
		do_quote = 1;
	}
#endif

	if (str + len > line + DUMPLINELENGTH) {
		Error();
			printf("In AppendField line length overflow\n");
			return(NULL);
	}
	if (str + len - *lastline > DUMPLINEWIDTH) {
		strcpy(str, " \\\n  ");
		str += 5;
		*lastline = str - 2;
	} else {
		*str = ' ';
		str++;
	}
	if (do_quote) {
		sprintf(str,"\"%s\"",filteredfield);
	} else {
		strcpy(str,filteredfield);
	}
	str += len;
	return(str);
}

void DumpObj(fp,objname,fields,nfields,use_coords)
	FILE	*fp;
	char *objname;
	char **fields;
	int nfields;
	int use_coords;
{
	GenesisObject	*object;
	char	dumpline[DUMPLINELENGTH];
	char	*str;
	char	*lastline; /* point of last carriage return */
	int		i;

	if (!(object = GetObject(objname))) {
		Error(); printf("Could not find object '%s'\n",objname);
		return;
	}
	sprintf(dumpline,"simobjdump %s",objname);
	lastline = dumpline;
	str = dumpline + strlen(dumpline);
	if (nfields > 0) { /* use the specified fieldlist rather than
						** the defaults */
		for(i=0; i < nfields; i++) {
			/* screen out invalid fields which are NULL */
			if (fields[i]) {
				if (!(str =
					AppendField(dumpline,str,&lastline,fields[i])))
				break;
			}
		}
	}
	fprintf(fp,"%s\n",dumpline);
}

void do_initdump(argc,argv)
	int 	argc;
	char	**argv;
{
	int	status;
	/* Assume the oldest dump version, which doesn't even
	** set the version option. This is relevant only for
	** input, not for outputting dumps, since they are
	** always dumped in the current version */

	if (argv != NULL) {
		DumpVersion = 0;
	
		initopt(argc,argv, "-version # -ignoreorphans # -allowmsgdup");
		while ((status = G_getopt(argc,argv)) == 1) {
			if (strcmp(G_optopt,"-version") == 0) {
				DumpVersion = atoi(optargv[1]);
			}
			if (strcmp(G_optopt,"-ignoreorphans") == 0) {
				IgnoreOrphans = atoi(optargv[1]);
			}
			if (strcmp(G_optopt,"-allowmsgdup") == 0) {
				AllowMsgDup = 1;
			}
		}
		if (status < 0) {
			Error();
			printoptusage(argc,argv);
			return;
		}
	}

	if (DumpList != NULL && DumpListSize >= 0) {
		/* The dump list has already been inited */
	} else {
		DumpList = (struct dumplist_type *)
			calloc(100,sizeof(struct dumplist_type));
		DumpListSize = 0;
	}

	if (MsgDupElist != NULL && MsgDupElist->size >= 0) {
		/* The dump list has already been inited */
	} else {
		ElementList *CreateElementList();
		MsgDupElist = CreateElementList(MSGDUPELISTSIZE);
	}

	if (!AllowMsgDup) {
		/* Replace the normal addmsg command with dumpaddmsg */
		MsgDupElist->nelements = 0;
		AddFunc("addmsg", do_dump_add_msg, "void");
	}
	/* Replace the normal 'call' command with do_dump_call */
	AddFunc("call", do_dump_call, "char*");
}

void do_enddump(argc,argv)
	int 	argc;
	char	**argv;
{
	int i,j;

	if (DumpList == NULL || DumpListSize < 0)
		return;

	for(i = 0; i < DumpListSize; i++) {
		DumpList[i].object = NULL;
		for(j = 0; j < DumpList[i].nfields; j++) {
			free(DumpList[i].fields[j]);
		}
		free(DumpList[i].fields);
		DumpList[i].nfields = 0;
	}
	free(DumpList);
	DumpList = NULL;
	DumpListSize = -1;
	if (!AllowMsgDup) {
		/* Replace the fake addmsg command with the real thing */
		MsgDupElist->nelements = 0;
		AddFunc("addmsg", do_add_msg, "void");
	}
	AddFunc("call", do_call_element, "char*");
	AllowMsgDup = 0;
}

/* Adds objects to obj_substitute list */
#ifdef __STDC__
void do_obj_substitute(int argc, char** argv) {
#else
void do_obj_substitute(argc, argv)
int argc;
char** argv;
{
#endif
	int i;

	initopt(argc,argv, "orig_obj_name new_obj_name");
	if (G_getopt(argc,argv) != 0) {
		printoptusage(argc,argv);
		return;
	}
	for (i = 0; i < n_obj_substitute; i++) {
		if (strcmp(optargv[1], obj_substitute[i].orig) == 0) {
			Error();
			printf("Duplicating object substitution for %s, %s\n",
				optargv[1], optargv[2]);
			return;
		}
	}
	if (n_obj_substitute >= N_SUBSTITUTE) {
		Error();
		printf("Exceeded max number of object substitutes %d\n",
			N_SUBSTITUTE);
	}
	obj_substitute[n_obj_substitute].orig = CopyString(optargv[1]);
	obj_substitute[n_obj_substitute].new = CopyString(optargv[2]);
	n_obj_substitute++;
}

/* Add msgs to msg_substitute list. Note that if we
** are also doing obj substitution, we must ensure that we use
** the _substituted_ name of the dest obj is used.
*/
#ifdef __STDC__
void do_msg_substitute(int argc, char** argv) {
#else
void do_msg_substitute(argc, argv)
int argc;
char** argv;
{
#endif
	int i;

	initopt(argc,argv, "destobj orig_msg_type new_msg_type field1 ...");
	if (G_getopt(argc,argv) != 0) {
		printoptusage(argc,argv);
		return;
	}
	for (i = 0; i < n_msg_substitute; i++) {
		if (strcmp(optargv[1], msg_substitute[i].destobj) == 0 &&
			strcmp(optargv[2], msg_substitute[i].orig_msgtype) == 0) {
			Error();
			printf("Duplicating msg substitution for %s, %s\n",
				optargv[1], optargv[2]);
			return;
		}
	}
	if (n_msg_substitute >= N_SUBSTITUTE) {
		Error();
		printf("Exceeded max number of msg substitutes %d\n",
			N_SUBSTITUTE);
	}
	msg_substitute[n_msg_substitute].destobj = CopyString(optargv[1]);
	msg_substitute[n_msg_substitute].orig_msgtype =
		CopyString(optargv[2]);
	for (i = 0; i < optargc - 3; i++) {
		msg_substitute[n_msg_substitute].argv[i] =
			CopyString(optargv[i + 3]);
	}
	msg_substitute[n_msg_substitute].argc = optargc - 3;
	n_msg_substitute++;
}

/* Just an info function on substitutions */
void do_substitute_info(argc,argv)
int argc;
char **argv;
{
	int i,j;

	initopt(argc, argv, "");
	if (G_getopt(argc, argv) != 0) {
		printoptusage(argc, argv);
		return;
	}

	printf("Number of object substitutions: %d\n", n_obj_substitute);
	for(i = 0; i < n_obj_substitute; i++) {
		printf("%d	orig=%s		new=%s\n",i,
			obj_substitute[i].orig, obj_substitute[i].new);
	}

	printf("\nNumber of msg substitutions: %d\n", n_msg_substitute);
	printf("No.	Obj type	Orig msgtype	New msg type	fields\n");
	for(i = 0; i < n_msg_substitute; i++) {
		printf("%d	%s		%s		%s	",i,
			msg_substitute[i].destobj, msg_substitute[i].orig_msgtype,
			msg_substitute[i].argv[0]);
		for (j = 1; j < msg_substitute[i].argc; j++) 
			printf("	%s",msg_substitute[i].argv[j]);
		printf("\n");
	}
}

/* Finds the dump item corresponding to a given object */
static struct dumplist_type *FindOnDumpList(obj)
	GenesisObject *obj;
{
	int i;
	static GenesisObject *lastobj = NULL;
	static int lasti = 0;

	if (DumpList == NULL) return(NULL);

	/* This might speed up searches a little bit */
	if (lastobj == obj && lasti >= 0) {
		if (DumpList[lasti].object == obj)
			return(&(DumpList[lasti]));
	} else {
		lastobj = obj;
	}

	for (i = 0 ; i < DumpListSize; i++) {
		if (DumpList[i].object == obj)
			return(&(DumpList[i]));
	}
	return(NULL);
}

/* Dumps a simulation */
void do_simdump(argc,argv)
	int 	argc;
	char	**argv;
{
	int status;
	int do_messages = 0;
	int do_all = 0;
	FILE	*fp,*fopen();
	ElementList	*elist = NULL;

	initopt(argc,argv, "file -path path -tree tree -messages -all");
	while ((status = G_getopt(argc,argv)) == 1) {
		if (strcmp(G_optopt,"-tree") == 0) {
			Element	*tree;
			if ((tree = GetElement(optargv[1]))) {
			} else {
				Error();
				printf("Cannot find tree %s\n",optargv[1]);
				return;
			}
			/* Get the elist from the tree here */
		}
		if (strcmp(G_optopt,"-path") == 0) {
			char *path;
			/* If use_coords is set, it will add the x y z fields
			** to every object saved */
			path = optargv[1];
			if (elist == NULL) { /* a new elist */
				elist = WildcardGetElement(path);
				if (!elist) {
					Error(); printf("no elements on path %s\n",path);
					return;
				}
			} else { /* we want to append an extra path */
				ElementList *tempelist = WildcardGetElement(path);
				int i;
				if (!tempelist) continue;
				for (i = 0 ; i < tempelist->nelements; i++)
					AddElementToList(tempelist->element[i],elist);
				FreeElementList(tempelist);
			}
		}
		/* This option includes all msgdests located on the specified
		** path. This simplifies the process of saving a part of a 
		** model, where there are dependencies on elements outside
		** the dump path
		*/
		if (strcmp(G_optopt,"-msgdests")) {
			char *path;
			path = optargv[1];
			if (elist != NULL) { /* an existing elist */
				/* This is the base path of the allowed dest elements */
				/* When such an element is found, the entire
				** heirarchy down to this specified base path is
				** rebuilt */
				(void) GetElement(path);
				/* scan for msgdests of elms on elist */
				/* Check if tempelm is the ancestor of the msgdest */
				/* Add the ancestors of tempelm to elist. */
			}
		}
		/* Perhaps we should put in a cleaning operation here
		** to get rid of duplications on the elist. A quicksort would
		** do the job */
		if (strcmp(G_optopt,"-all") == 0) {
			/* If use_coords is set, it will add the x y z fields
			** to every object saved */
			elist = WildcardGetElement("/##[]");
			if (!elist) {
				Error(); printf("no elements on path /##[]\n");
				return;
			}
			do_all = 1;
		}
		if (strcmp(G_optopt,"-messages") == 0) {
			do_messages = 1;
		}
	}
	if (status < 0) {
		printoptusage(argc,argv);
		return;
	}

	if (!(fp =  fopen(optargv[1],"a"))) {
		Error(); printf("could not open file %s\n",optargv[1]);
		return;
	}

	/* It scans through the list and identifies all outgoing obj types.
	** Each is entered into the DumpList if it hasn't already been,
	** by the simobjdump functions */
	FillDumpList(elist);

	/* It saves the standard dump headers */
	WriteDumpHeaders(fp);

	/* It stores the DumpList */
	WriteDumpList(fp);

	/* It scans through the list again and starts dumping elements.
	** First it checks the element's DUMP action
	** Then it dumps the known fields
	** Then it appends extras like tree coords
	*/
	DumpElements(fp,elist);

	/* if do_messages is set, but not do_all, it makes a hash table of
	** all the elms in the list 
	** and goes through each elm's messages looking
	** for srcs that are in the list. If a message is entirely
	** contained in the list, that msg is dumped */

	/* If do_messages is set and also do_all, it just dumps all
	** messages without worrying about where the src is */
	DumpMessages(fp,elist,do_all);
	
	/* It writes the standard dump tail */
	WriteDumpTail(fp);

	FreeElementList(elist);
	fclose(fp);
}



void FillDumpList(elist)
	ElementList	*elist;
{
	int i;
	Element *elm;
	
	if (elist == NULL) return;
	for(i = 0; i < elist->nelements; i++) {
		elm = elist->element[i];
		if (!(FindOnDumpList(elm->object))) {
			/* Add the object to the DumpList with default fields */
			AddObjToDumpList(elm->object->name,NULL,0,0,1);
		}
	}
}


/* We always write out in the latest dump version */
void WriteDumpHeaders(fp)
	FILE	*fp;
{
	fprintf(fp,"//genesis\n\n");
	fprintf(fp,"initdump -version %d", DUMPVERSION);
	if (IgnoreOrphans)
		fprintf(fp," -ignoreorphans 1");
	fprintf(fp,"\n");
}

void WriteDumpTail(fp)
	FILE	*fp;
{
	fprintf(fp,"enddump\n");
	fprintf(fp,"// End of dump\n\n");
}

void WriteDumpList(fp)
	FILE	*fp;
{
	int i;
	
	if (!DumpList || DumpListSize <= 0) return;
	for(i = 0 ; i < DumpListSize; i++)
		DumpObj(fp,DumpList[i].object->name,
			DumpList[i].fields,
			DumpList[i].nfields, FALSE);
}

static void DumpElements(fp,elist)
	FILE	*fp;
	ElementList	*elist;
{
	struct dumplist_type *dumpitem;
	Action	*action;
	Element	*elm;
	int	i,j;
	char	dumpline[DUMPLINELENGTH];
	char	*str;
	char	*lastline; /* point of last carriage return */
	char	*value;
	char	*targs[1];

	if ((action = GetAction("DUMP")) != NULL) {
		action->argc = 1;
		targs[0] = "pre";
		action->argv = targs;
		action->data = (char *)fp;
	}

	for (i = 0 ; i < elist->nelements; i++) {
		elm = elist->element[i];
		if (!(dumpitem = FindOnDumpList(elm->object)))
			continue;
		sprintf(dumpline,"simundump %s %s %d",
			elm->object->name,Pathname(elm),elm->flags);
		lastline = dumpline;
		str = dumpline + strlen(dumpline);
		/* do the pre-dump check with the object */
		/* If the action returns 1, then no further fields should
		** be set. This should happen very rarely */
		if (dumpitem->do_dump_action && action) {
			targs[0] = "pre";
			if (CallActionFunc(elm,action) == 1)
				continue;
		}
		for(j = 0 ; j < dumpitem->nfields; j++) {
			/* Invalid fields have been NULLed */
			if (dumpitem->fields[j]) {
				value = FieldValue(elm,elm->object,dumpitem->fields[j]);
				if (value == NULL)
					value = GetExtField(elm,dumpitem->fields[j]);
				if (value == NULL)
					value = "0"; /* last resort default */
				if (!(str = AppendField(dumpline,str,&lastline,value)))
					break;
			}
		}
		fprintf(fp,"%s\n",dumpline);
		/* do the post-dump check with the object */
		/* This is to let the object add stuff after the basic dump.
		** This is a much more common case. e.g, tables will use this
		** to set up a TABCREATE call and fill the table. */
		if (dumpitem->do_dump_action && action) {
			targs[0] = "post";
			CallActionFunc(elm,action);
		}
	}
}

/* This is a duplication of a function in xo. I think we should
** add this more sensible set of hash utilities to the sys library */
static ADDR dump_hash_function(key,table)
	char	*key;
	HASH	*table;
{
	return((((ADDR)key & 0xff) + ((ADDR)key >> 8)) % (ADDR)table->size);
}

static ENTRY *dump_hash_find(key,table)
	char	*key;
	HASH	*table;
{
int size;
ENTRY *hash_ptr;
ENTRY *start_ptr;
 
    if(key == NULL || table == NULL) return(NULL);
    start_ptr = hash_ptr = table->entry + dump_hash_function(key,table);
    size = table->size;
    while(hash_ptr->key){
        if(key == hash_ptr->key){
            return(hash_ptr);
        }
        if(++hash_ptr >= table->entry + size) hash_ptr = table->entry;
        if(hash_ptr == start_ptr){
            return(NULL);
        }
    }
    return(NULL);
}

static ENTRY *dump_hash_enter(item,table)
ENTRY *item;
HASH *table;
{
int size;
ENTRY *hash_ptr;
ENTRY *start_ptr;
char *key;
 
    if(item == NULL || table == NULL) return(NULL);
    /*
    ** look for the next available spot
    */
    key = item->key;
    start_ptr = hash_ptr = table->entry + dump_hash_function(key,table);
    size = table->size;
    while(hash_ptr->key){
        /*
        ** copy over existing keys if they exist
        */
        if(key==hash_ptr->key){
            break;
        }
        if(++hash_ptr >= table->entry + size) hash_ptr = table->entry;
        if(hash_ptr == start_ptr){
            return(NULL);
        }
    }
    /*
    ** leave the last entry empty to identify the end of the table
    */
    BCOPY(item,hash_ptr,sizeof(ENTRY));
    return(hash_ptr);
}

static void DumpMessages(fp,elist,do_all)
	FILE	*fp;
	ElementList	*elist;
	int		do_all;
{
	int i;
	Element *elm;
	int count;
	MsgIn	*msg;

	/* here we simply scan through the elist, saving all msgs */
	if (do_all) {
		for(i = 0 ; i < elist->nelements; i++) {
			elm = elist->element[i];
			msg = elm->msgin;
			for(count = 0 ; count < elm->nmsgin; count++) {
				DumpMsgIn(fp,msg,elm);
				msg = MSGINNEXT(msg);
			}
		}
	} else {
	/* here we check each msg->src to see if it is on the elist */
		HASH *hash_table,*hash_create();
		ENTRY	entry;
		ENTRY	*tempentry;
		ENTRY	*dump_hash_find(), *dump_hash_enter();
		int nelms = elist->nelements;

		/* First, set up a hash table */
		/* since we wish to optimize speed here, we give the
		** hash table lots of extra room */
		hash_table = hash_create(elist->nelements * 2);
		for(i = 0; i < nelms; i++) {
			entry.data = (char *)i;
			entry.key = (char *)elist->element[i];
			dump_hash_enter(&entry,hash_table);
		}
		for(i = 0 ; i < elist->nelements; i++) {
			elm = elist->element[i];
			msg = elm->msgin;
			for(count = 0 ; count < elm->nmsgin; count++) {
				/* check if the msg src is on the hash table */
				if ((tempentry = dump_hash_find((char *) msg->src,hash_table))) {
					DumpMsgIn(fp,msg,elm);
				}
				msg = MSGINNEXT(msg);
			}
		}
		if (hash_table) {
			if (hash_table->size > 0)
				free(hash_table->entry);
			free(hash_table);
		}
	}
}

/* Closely derived from ShowMsgIn in sim_msg.c */
static void DumpMsgIn(fp,msg,elm)
	FILE *fp;
	MsgIn	*msg;
	Element *elm;
{
	GenesisObject *object=elm->object;
	char dumpline[400];
	char destpath[200];
	MsgList	*ml;

	if (VISIBLE(msg->src)) {
		ml = GetMsgListByType(object,msg->type);
		strcpy(destpath,Pathname(elm));
		if (ml) {
			sprintf(dumpline,"addmsg %s %s %s ",
				Pathname(msg->src),destpath,ml->name);
			DumpMsgFields(dumpline + strlen(dumpline),msg);
			fprintf(fp,"%s\n",dumpline);
		}
	}
}

/* Here we figure out from the pointer and msg->src which field
** sent the msg. For now this only works for ordinary fields
** in ordinary objects. Later we can extend it to tables.
** Still later, to extended fields and objects
**
** If this becomes a big speed problem we should put the field
** offsets as keys in a hash table with the field names.
*/
static void DumpMsgFields(dumpline, msg)
	char	*dumpline;
	MsgIn	*msg;
{
	int fieldoffset;
	Element	*elm = msg->src;
	Slot	*slot;
	int		i;
	char	*slotfield;
	char	*GetStringMsgData();

	slot = (Slot *)(msg + 1);
	for(i= 0; i < (int)msg->nslots; i++) {
		/* check if it is a string msg slot */
		if ((slotfield = GetStringMsgData(msg,i))) {
			strcat(dumpline,"*");
			strcat(dumpline,slotfield);
			strcat(dumpline," ");
		} else { /* no, we need to look for a field */
			char *fname;
			fieldoffset = slot[i].data - (char *)elm;
			if ((fname = FindFieldName(elm->object,fieldoffset))) {
				strcat(dumpline,fname);
				strcat(dumpline," ");
			}
		}
	}
}

static char *FindFieldName(object,offset)
	GenesisObject *object;
	int	offset;
{
	char	*str;
	char	*tmp;
	char	*fieldlist;
	static char	fieldname[200];
	char	fullname[200];
	extern char	*FieldHashFindAndCopy();
	Info	info;

	/* in str the fieldnames are separated by '\n'
	** so we make a copy of str and munge through it
	** to get the individual names */
	if ((fieldlist = FieldHashFindAndCopy(object->type))) {
		/* strcpy(fieldlist,str); */
		for(str = fieldlist; *str != '\0'; ) {
			tmp = str;
			while (*tmp != '\n') tmp++;
			*tmp = '\0';
			sprintf(fullname,"%s.%s",object->type,str);
			if (GetInfo(fullname,&info)) {
				if (info.offset == offset) {
					strcpy(fieldname,str);
					free(fieldlist);
					return(fieldname);
				}
			}
			str = tmp+1;
		}

		free(fieldlist);
	} else {
	}
	return("x"); /* safe fallback field name */
}

/* This function looks for the \n string in its argument,
** and if it finds any, it replaces them with a carriage return */
static char *ParseString(str)
	char *str;
{
	static char newstr[1000];
	char *tstr = newstr;
	char *maxstr = newstr+999;
	newstr[999] = '\0';

	for (*tstr = *str; (*tstr = *str) && tstr < maxstr; str++,tstr++) {
		if (*str == '\\' && *(str+1) == 'n') {
			*tstr = '\n';
			str++;
		}
	}
	return newstr;
}

/* Here we scan the list of objects to be substituted to see if the
** requested object needs to be changed */
#ifdef __STDC__
int ObjectSubstitute(char* objname, char** retname) {
#else
int ObjectSubstitute(objname, retname)
char* objname;
char** retname;
{
#endif
	int i;

	for (i = 0; i < n_obj_substitute; i++) {
		if (strcmp(objname, obj_substitute[i].orig) == 0) {
			*retname = obj_substitute[i].new;
			return 1;
		}
	}
	*retname = objname;
	return 0;
}

void do_simundump(argc,argv)
	int 	argc;
	char	**argv;
{
	int	i;
	Element	*elm;
	char	*targv[3];
	int		status;
	struct dumplist_type *dumpitem;
	int orig_optargc;
	int subst_flag = 0;

	initopt(argc,argv, "object element ... -tree tree x y z");
	while ((status = G_getopt(argc,argv)) == 1) {
		if (strcmp(G_optopt,"-tree") == 0) {
			Element	*tree;
			if (!(tree = GetElement(optargv[1]))) {
				Error();
				printf("Cannot find tree %s\n",optargv[1]);
				return;
			}
			/* Get the elist from the tree here */
		}
	}
	if (status < 0) {
		Error();
		printoptusage(argc,argv);
		return;
	}
	orig_optargc = optargc;

	/* check if the element has already been created. */
	if ((elm = GetElement(optargv[2]))) {
		if (strcmp(elm->object->name,optargv[1]) != 0) {
			Error();
			printf("%s: element %s exists but is not of type %s\n",
				optargv[0],optargv[2],optargv[1]);
			return;	
		}
		AddElementToList(elm,MsgDupElist);
	} else {

	/* Check if the IgnoreOrphans flag is set. This tells us to
	** ignore all elements whose parents are missing 
	*/
	if (IgnoreOrphans) {
		/* check for existence of parents */
		char *parent_name;

		if (strlen(parent_name = GetParentComponent(optargv[2])) == 0)
			return;

		if (!GetElement(parent_name))
			return;
	}

	/* create the element. It is simplest to use the usual 'create'
	** command here */
		targv[0] = "simundump";
	/* Here we check if object substitution is enabled */
		subst_flag = ObjectSubstitute(optargv[1], &targv[1]);
		targv[2] = optargv[2];
	/* Nasty surprise here: if the arguments are changed, then
	** subsequent values of optargv and optargc are also changed. Ugh */
		do_create(3,targv);
		elm = RecentElement();
		if (!elm) return;
	}

	if (!DumpList || DumpListSize < 0) {
		Error(); printf("dump not initialized\n");
		return;
	}
	if (!(dumpitem = FindOnDumpList(elm->object))) {
		Error();
		printf("object '%s' not initialized for undump\n",
			elm->object->name);
		return;
	}

	/* Here we eliminate all fields, except the flags, if substitution
	** is enabled */
	if (subst_flag) {
		if (DumpVersion < 3)
			orig_optargc = 3;
		else
			orig_optargc = 4;
	}

	if (DumpVersion < 3)
		i = orig_optargc - 3;
	else
		i = orig_optargc - 4;

	/* We bypass this check if object substitution has occurred,
	** because the fields are unknown */
	if (dumpitem->nfields != i && !subst_flag) {
		Error();
		printf("dump fields for object '%s' do not match data list\n",
			elm->object->name);
		return;
	}
	/* We need to use argv rather than optargv, because 
	** optargv changes sometimes in the do_create function */
	if (DumpVersion < 3) {
		for (i = 3; i < orig_optargc; i++) {
			SetElement(elm,dumpitem->fields[i - 3],argv[i]);
			/* The SetElement issues its own warnings */
		}
	} else {
		/* Load in the flags of the element */
		elm->flags = atoi(argv[3]);
		for (i = 4; i < orig_optargc; i++) {
			SetElement(elm,dumpitem->fields[i - 4],
				ParseString(argv[i]));
			/* The SetElement issues its own warnings */
		}
	}
}

/* Utility function for dumping interpol structs. This is used
** by a lot of elements which have interpols
** Modified by Upi Bhalla Aug 95 to handle large tables using
** the new subdivision options for loadtab.
*/

void DumpInterpol(fp,elm,ip,name)
    FILE    *fp;
	Element *elm;
	struct interpol_struct *ip;
	char	*name;
{
	char	dumpline[DUMPLINELENGTH];
	char	*str;
	char	val[25];
	int		i,len;


    if (!fp || !ip) return;

    fprintf(fp,"loadtab %s %s %d %d %g %g \\\n",
        Pathname(elm), name, ip->calc_mode, ip->xdivs,
        ip->xmin, ip->xmax);
    
	dumpline[0] = '\0';
	str = dumpline;
	for(i = 0; i <= ip->xdivs; i++) {
		sprintf(val,"%g",ip->table[i]);
		len = strlen(val);
		if (i > 0 && i % 200 == 0) {
			fprintf(fp,"%s \n",dumpline);
			fprintf(fp,"loadtab -continue \\\n");
			str = dumpline;
		} else if (len + (str - dumpline) > 76) {
			fprintf(fp,"%s \\\n",dumpline);
			str = dumpline;
		}
		*str = ' ';
		str++;
		strcpy(str,val);
		str += len;
	}
	fprintf(fp,"%s\n",dumpline);
}

/* Function for swapping the dumplist. Useful if one wants
** to preserve a dumplist while reading in another dump.
*/

int do_swap_dumplist(argc,argv)
	int	argc;
	char **argv;
{
	struct dumplist_type *swap;
	int			swapsize;

	swap = DumpList;
	DumpList = SwapDumpList;
	SwapDumpList = swap;

	swapsize = DumpListSize;
	DumpListSize = SwapDumpListSize;
	SwapDumpListSize = swapsize;

	CurrDumpListNum = 1 - CurrDumpListNum;

	return(CurrDumpListNum);
}

/* All the below should be defined in sim_access.c and in
** sim_defs.h, but till Dave Bilitch approves the change I
** will keep it here. Also these may not be necessary
*/

#define DUMPBLOCKMASK 0x0200

int IsDumpDisabled(elm)
	Element *elm;
{
	if (elm->flags & DUMPBLOCKMASK)
		return(1);
	return(0);
}

void EnableDump(element)
	Element *element;
{
	if (element)
		element->flags &= ~DUMPBLOCKMASK;
}

void DisableDump(element)
	Element *element;
{
	if (element)
		element->flags |= DUMPBLOCKMASK;
}

int do_disable_dump(argc,argv)
int argc;
char    **argv;
{
Element     *element;
 
    initopt(argc, argv, "element");
    if (G_getopt(argc, argv) != 0)
      {
    printoptusage(argc, argv);
    return 0;
      }
 
    if((element = GetElement(optargv[1])) == NULL){
    Error();
    printf("cannot find element '%s'\n", optargv[1]);
    return(0);
    }
	DisableDump(element);
    OK();
    return(1);
}
 
int do_enable_dump(argc,argv)
int argc;
char    **argv;
{
Element     *element;
 
    initopt(argc, argv, "element");
    if (G_getopt(argc, argv) != 0)
      {
    printoptusage(argc, argv);
    return 0;
      }
 
    if((element = GetElement(optargv[1])) == NULL){
    Error();
    printf("cannot find element '%s'\n", optargv[1]);
    return(0);
    }
	EnableDump(element);
    OK();
    return(1);
}

/* This function checks if the specified msg is to be substituted,
** and if so it sets up the alternative arguments for the msg.
** Note that at this point the dest obj may have been converted.
** We have options:
**	- just check for the converted object, and assume that the
**		converted object will always be of a new class.
**	- convert back to the original object for checks. Again, we
**		must assume that the converted object will be of a new clas.
*/
#ifdef __STDC__
void MessageSubstitute(Element *dest, char* msgtype, int argc, char** argv) {
#else
void MessageSubstitute(dest, msgtype, argc, argv)
Element *dest;
char* msgtype;
int argc;
char** argv;
{
#endif
	char *destobj = dest->object->name;
	int i, j;
	
	for (i = 0; i < n_msg_substitute; i++) {
		if (strcmp(msgtype, msg_substitute[i].orig_msgtype) == 0) {
			if (strcmp(destobj, msg_substitute[i].destobj) == 0) {
				msg_substitution = 1;
				targc = msg_substitute[i].argc + 3;
				targv[0] = argv[0];
				targv[1] = argv[1];
				targv[2] = argv[2];
				for (j = 3; j < targc; j++) {
					if (strcmp(msg_substitute[i].argv[j-3],".") == 0)
						targv[j] = argv[j];
					else
						targv[j] = msg_substitute[i].argv[j-3];
				}
				return;
			}
		}
	}
	msg_substitution = 0;
}


/* This simple function switches between arguments if msg_substitution
** is on */
#ifdef __STDC__
int add_msg_wrapper(int argc, char **argv) {
#else
int add_msg_wrapper(argc, argv)
int argc;
char **argv;
{
#endif
	if (!msg_substitution)
		return do_add_msg(argc,argv);
	
	return do_add_msg(targc,targv);
}

/* This is basically a shell for the normal addmsg command.
** It is designed to prevent duplication of existing msgs when
** a dumpfile is loaded on top of an existing model.
** Before invoking the addmsg, it checks whether the src and
** dest are both on the MsgDupElist. If so, then it checks the
** msgtype of the requested msg and sees if it matches an
** existing msg between the two elements. If it does, then the
** new msg is not created.
*/

void do_dump_add_msg(argc,argv)
	int argc;
	char	**argv;
{
	int i,j;
	Element *src,*dest;
	int GetMsgNumBySrcDest();

	initopt(argc, argv,
		"source-element dest-delement msg-type [msg-fields]");
	if (G_getopt(argc,argv) != 0) {
		TraceScript();
		printoptusage(argc,argv);
		return;
	}
	/* Note that the addmsg in dumps always refers to individual
	** elements */
	src = GetElement(optargv[1]);
	if (!src) /* Cant do anything anyway ! */
		return;
	dest = GetElement(optargv[2]);
	if (!dest) /* Cant do anything anyway ! */
		return;

	/* Here we change things around if MessageSubstitution is active */
	MessageSubstitute(dest,optargv[3],argc,argv);

	/* If there are no elements on the MsgDupElist, just call
	** the usual do_add_msg function */
	if (MsgDupElist->nelements == 0) {
		add_msg_wrapper(argc,argv);
        return;
	}

	for (i = 0; i < MsgDupElist->nelements; i++) {
		if (src == MsgDupElist->element[i]) {
			for (j = 0; j < MsgDupElist->nelements; j++) {
				if (dest == MsgDupElist->element[j]) {
					/* uh, oh. This may be an overlapped msg. The
					** func returns -1 if no msg was found */
					if (GetMsgNumBySrcDest(src,dest,optargv[3]) >= 0){
						/* bag it ! */
						return;
					}
					/* The dest was already found, so break out of
					** the loop */
					break;
				}
			}
			/* None of the dests matched. Break out of the loop */
			break;
		}
	}
	add_msg_wrapper(argc,argv);
}


/* This is identical to the usual call_element function, except
** this just ignores it if the element is not there */
#ifdef __STDC__
char* do_dump_call(int argc, char** argv) {
#else
char* do_dump_call(argc, argv)
int argc;
char** argv;
{
#endif
	if (GetElement(argv[1]))
		return do_call_element(argc,argv);
	return NULL;
}

#undef DUMPLINEWIDTH
#undef DUMPLINELENGTH

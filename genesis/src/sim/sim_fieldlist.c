static char rcsid[] = "$Id: sim_fieldlist.c,v 1.5 2005/06/29 16:49:51 svitak Exp $";

/*
** Code to handle the object field list.  The field list is a list of
** information about the fields of an object.  The information currently
** includes:
**
**	- The name of the field
**	- A description of the field
**	- Field accessability information; a field protection is one of
**	  readwrite, readonly or hidden.
**	- Field type information; a field type is one of builtin (e.g.
**	  defined in the element structure), extended or indirect (a field
**	  which is an alias for another field, probably in another element).
**	- Additional field type dependent data.  For extended fields this is
**	  the index of the extended field value in the element.  For indirect
**	  fields this is the element and field name relative to the element
**	  with the indirect field.
**
** Function provided are:
**
**	AddFieldList		- Add a new field to the field list
**	DelFieldList		- Delete an existing field to the field list
**	AddDefaultFieldList	- Add field list for all element fields
**	GetFieldListProt	- Return the protection status of a field
**	SetFieldListProt	- Set the protection status of a field
**	SetFieldListDebug	- Set the debug level for field protections
**				  currently a non-zero value causes hidden
**				  fields to be treated as readonly
**	GetFieldListDebug	- Return current field list debug state
**	GetFieldListInfo	- Return pointer to the type dependent info
**	GetFieldListType	- Return the type of the field
**	SetFieldListType	- Set the type of the field
**	GetFieldListDesc	- Return the field description
**	SetFieldListDesc	- Set the field description
**	FieldIsPermanent	- Return status of the permanent flag
**	FieldListExists		- Check for field existence
**	CopyFieldList		- Duplicate the fieldlist of one object into
**				  another for the addobject (extended objects)
**				  command
**	DisplayFieldList	- Display the field list for the showobject
**				  command
**	FieldListEqual		- Determine if two fieldlists are the same.
**	FieldListMakePermanent	- Make all the new fields in the fieldlist
**				  permanent.
**
** The following functions allow the caller to iterate over the fieldlist
** and look at the information therein:
**
**	FLIOpen			- Open a field list iterator
**	FLIClose		- Close a field list iterator
**	FLIValid		- TRUE if the fli currently points to a valid
**				  field list entry
**	FLINext			- Go to the next entry in the field list
**	FLIType			- The type of the current field list entry
**	FLIName			- The name of the field
**	FLIInfo			- The info structure address
**	FLIProt			- The protection value
**
** $Log: sim_fieldlist.c,v $
** Revision 1.5  2005/06/29 16:49:51  svitak
** Removed local strtok proto...included in string.h.
**
** Revision 1.4  2005/06/27 21:36:16  svitak
** Undo of last accidental commit.
**
** Revision 1.3  2005/06/27 21:28:52  svitak
** Initial revision. Some decls from output.c.
**
** Revision 1.2  2005/06/27 19:01:00  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.18  2001/06/29 21:27:55  mhucka
** Fixes from Hugo C.
**
** Revision 1.17  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.16  2000/06/12 04:53:10  mhucka
** Added a missing argument declaration.
**
** Revision 1.15  1997/05/28 23:56:21  dhb
** Changes from Antwerp GENESIS 21e
**
 *
 * EDS20j revison: EDS BBF-UIA 95/08/05
 * Increased AddDefaultFieldList string size
 *
** Revision 1.14  1996/07/16 22:12:27  dhb
** Now allow -debug option to setfieldprot command which treats
** hidden fields as if they were readonly.  Also now allow any
** manual relaxing of field protection using setfieldprot with
** a verbose warning message.  NOTE: the latter change makes it
** possible for a user to crash GENESIS by making critical
** internal fields writable and clobbering the fields.
**
 * Revision 1.13  1994/07/13  23:39:04  dhb
 * Change to AddDefaultFieldList() to make the object field READONLY
 * instead of HIDDEN.  This may be a temporary change and is being made
 * to allow scripts to examine object->name.
 *
 * Revision 1.12  1994/06/03  21:44:50  dhb
 * Merged in 1.9.1.1 changes
 *
 * Revision 1.11  1994/06/03  21:12:56  dhb
 * Element field "index" was hidden.  Made it readonly.
 *
 * Revision 1.10  1994/05/27  23:23:00  dhb
 * Merged in 1.8.1.1 changes.
 *
 * Revision 1.9.1.1  1994/06/03  21:41:42  dhb
 * Made element field "index" readonly instead of hidden.
 * Added check for NULL default value in DisplayFieldList().
 *
 * Revision 1.9  1994/04/01  21:18:31  dhb
 * Change to DisplayFieldList() to show hidden fields too.
 *
 * Revision 1.8.1.1  1994/05/27  23:20:14  dhb
 * Removed the additional intoptr field from GetFieldListType() and
 * added GetFieldListInfo() to return the extra fieldlist info.
 *
 * Added a complete set of fieldlist iterator and access functions.
 *
 * Revision 1.8  1994/03/22  21:47:46  dhb
 * Changes to SetDefaultFieldList() to handle non-element based objects.
 * The field names are compared against those for struct element_type which
 * should describe the basic element.  Non-element based objects set all
 * fields readwrite.
 *
 * Revision 1.7  1994/03/22  21:13:45  dhb
 * Minor display formatting change for DisplayFieldList().
 *
 * Revision 1.6  1994/03/22  21:07:32  dhb
 * Syntax error in 1.5 changes
 *
 * Revision 1.5  1994/03/22  21:03:58  dhb
 * Changed DisplayFieldList() to display all non-hidden fields and
 * to mark readonly fields as such.
 *
 * Revision 1.4  1994/03/19  01:05:50  dhb
 * Added AddDefaultFieldList() to add an initial fieldlist for the builtin
 * fields of an element.  All basic fields are hidden with the exception of
 * name which is readonly and x, y, and z which are readwrite.
 *
 * Revision 1.3  1993/12/23  02:29:47  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.2  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.1  1993/03/18  17:15:55  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <string.h>	/* strtok */
#include "shell_func_ext.h"
#include "sim_ext.h"

static int FieldListDebug = 0;

int SetFieldListDebug(newDebugLevel)

int	newDebugLevel;

{	/* SetFieldListDebug --- Set field list debug level; return old */

	int	old;

	old = FieldListDebug;
	FieldListDebug = newDebugLevel;

	return old;

}	/* SetFieldListDebug */



int GetFieldListDebug()

{	/* GetFieldListDebug --- Return current field list debug level */

	return FieldListDebug;

}	/* GetFieldListDebug */



int AddFieldList(object, fieldname)

GenesisObject*	object;
char*		fieldname;

{	/* AddFieldList --- Add a new field to the field list */

	FieldList*	fl;
	FieldList*	prev;

	fl = (FieldList*) malloc(sizeof(FieldList));
	if (fl == NULL)
	  {
	    perror("AddFieldList");
	    return 0;
	  }

	fl->name = CopyString(fieldname);
	fl->desc = CopyString("");
	fl->flags = FIELD_PROT_DEFAULT|FIELD_BUILTIN;
	fl->next = NULL;

	if (object->fieldlist == NULL)
	    object->fieldlist = fl;
	else
	  {
	    prev = object->fieldlist;
	    while (prev->next != NULL)
		prev = prev->next;

	    prev->next = fl;
	  }

	return 1;

}	/* AddFieldList */


/*
** FieldListFree
**
** FUNCTION
**	Free the entire fieldlist of the object.
**
** ARGUMENTS
**	Object whose freelist we want freed.
**
** RETURN VALUE
**	None.
**
** SIDE EFFECTS
**
** NOTES
**
** AUTHOR
**	David Bilitch
*/

void FieldListFree(object)

GenesisObject*	object;

{	/* FieldListFree  --- Delete entire fieldlist in object */

	FieldList*	next;

	next = object->fieldlist;
	while (next != NULL)
	  {
	    FieldList*	cur;

	    cur = next;
	    next = cur->next;

	    free(cur->name);
	    free(cur->desc);
	    free(cur);
	  }
	object->fieldlist = NULL;

}	/* FieldListFree */


int DelFieldList(object, fieldname)

GenesisObject*	object;
char*		fieldname;

{	/* DelFieldList --- Delete existing field from the field list */

	FieldList*	cur;
	FieldList*	prev;

	cur = object->fieldlist;
	prev = NULL;
	while (cur != NULL)
	  {
	    if (strcmp(cur->name, fieldname) == 0)
	      {
		if (prev == NULL)
		    object->fieldlist = cur->next;
		else
		    prev->next = cur->next;

		free(cur->name);
		free(cur->desc);
		free(cur);

		return 1;
	      }

	    prev = cur;
	    cur = cur->next;
	  }

	return 0;

}	/* DelFieldList */



int AddDefaultFieldList(object)

GenesisObject*	object;

{	/* AddDefaultFieldList --- Add field list for all builtin fields */

    char*	FieldHashFindAndCopy();
    char*	FieldHashFind();
    char*	ptr;

    ptr = FieldHashFindAndCopy(object->type);
    if (ptr != NULL)
      {
	char*	fieldname;
	char*	elmfields;
	int	iselementbased;
	int	pastbasic;

	elmfields = FieldHashFind("element_type");
	iselementbased = elmfields == NULL ||
			    strncmp(elmfields, ptr, strlen(elmfields)) == 0;

	fieldname = strtok(ptr, " \t\n");
	pastbasic = !iselementbased;
	while (fieldname != NULL)
	  {
	    if (!AddFieldList(object, fieldname))
	      {
		printf("AddDefaultFieldList: error adding to field list\n");

		free(ptr);

		return 0;
	      }

	    if (!pastbasic)
	      {
		SetFieldListProt(object, fieldname, FIELD_HIDDEN);
		pastbasic = strcmp(fieldname, "next") == 0;
	      }

	    fieldname = strtok(NULL, " \t\n");
	  }

	if (iselementbased)
	  {
	    SetFieldListProt(object, "name", FIELD_READONLY);
	    SetFieldListProt(object, "index", FIELD_READONLY);
	    SetFieldListProt(object, "object", FIELD_READONLY);
	    SetFieldListProt(object, "x", FIELD_READWRITE);
	    SetFieldListProt(object, "y", FIELD_READWRITE);
	    SetFieldListProt(object, "z", FIELD_READWRITE);
	 }

	free(ptr);
      }

    return 1;

}	/* AddDefaultFieldList */



static FieldList* FindFieldList(fl, fieldname)

FieldList*	fl;
char*		fieldname;

{	/* FindFieldList --- Return pointer to fieldlist named fieldname */

	while (fl != NULL)
	  {
	    if (strcmp(fl->name, fieldname) == 0)
		return fl;

	    fl = fl->next;
	  }

	return NULL;

}	/* FindFieldList */


void* GetFieldListInfo(object, fieldname)

GenesisObject*	object;
char*		fieldname;

{	/* GetFieldListInfo --- Return type specific info for fieldname */

	FieldList*	fl;

	fl = FindFieldList(object->fieldlist, fieldname);
	if (fl == NULL)
	    return NULL;

	return (void*) &fl->info;

}	/* GetFieldListType */



int GetFieldListType(object, fieldname)

GenesisObject*	object;
char*		fieldname;

{	/* GetFieldListType --- Return type for fieldname */

	FieldList*	fl;

	fl = FindFieldList(object->fieldlist, fieldname);
	if (fl == NULL)
	    return -1;

	return fl->flags&FIELD_TYPE;

}	/* GetFieldListType */


int SetFieldListType(object, fieldname, newtype)

GenesisObject*	object;
char*		fieldname;
int		newtype;

{	/* SetFieldListType --- Set type for fieldname */

	FieldList*	fl;
	int		oldtype;

	if ((newtype&FIELD_TYPE) != newtype)
	  {
	    printf("SetFieldListType: bad field type 0x%x\n", newtype);
	    return -1;
	  }

	fl = FindFieldList(object->fieldlist, fieldname);
	if (fl == NULL)
	  {
	    printf("SetFieldListType: fieldlist for '%s' not found\n",
								    fieldname);
	    return -1;
	  }

	oldtype = fl->flags&FIELD_TYPE;
	fl->flags = (fl->flags&~FIELD_TYPE) | newtype;

	return oldtype;

}	/* SetFieldListType */


int GetFieldListProt(object, fieldname)

GenesisObject*	object;
char*		fieldname;

{	/* GetFieldListProt --- Return protection status for fieldname */

	FieldList*	fl;

	fl = FindFieldList(object->fieldlist, fieldname);
	if (fl == NULL)
	    return -1;

	if (FieldListDebug != 0 && (fl->flags&FIELD_PROT) == FIELD_HIDDEN)
	    return FIELD_READONLY;

	return fl->flags&FIELD_PROT;

}	/* GetFieldListProt */


int SetFieldListProt(object, fieldname, newprot)

GenesisObject*	object;
char*		fieldname;
int		newprot;

{	/* SetFieldListProt --- Set protection status for fieldname */

	FieldList*	fl;
	int		oldprot;

	if ((newprot&FIELD_PROT) != newprot)
	  {
	    printf("SetFieldListProt: bad protection status 0x%x\n", newprot);
	    return -1;
	  }

	fl = FindFieldList(object->fieldlist, fieldname);
	if (fl == NULL)
	  {
	    printf("SetFieldListProt: fieldlist for '%s' not found\n",
								    fieldname);
	    return -1;
	  }

	oldprot = fl->flags&FIELD_PROT;
	fl->flags = (fl->flags&~FIELD_PROT) | newprot;

	return oldprot;

}	/* SetFieldListProt */


char* GetFieldListDesc(object, fieldname)

GenesisObject*	object;
char*		fieldname;

{	/* GetFieldListDesc --- Return description for fieldname */

	FieldList*	fl;

	fl = FindFieldList(object->fieldlist, fieldname);
	if (fl == NULL)
	  {
	    printf("GetFieldListDesc: fieldlist for '%s' not found\n",
								    fieldname);
	    return NULL;
	  }

	return fl->desc;

}	/* GetFieldListDesc */


int SetFieldListDesc(object, fieldname, newdesc)

GenesisObject*	object;
char*		fieldname;
char*		newdesc;

{	/* SetFieldListDesc --- Set protection status for fieldname */

	FieldList*	fl;

	fl = FindFieldList(object->fieldlist, fieldname);
	if (fl == NULL)
	  {
	    printf("SetFieldListDesc: fieldlist for '%s' not found\n",
								    fieldname);
	    return 0;
	  }

	free(fl->desc);
	fl->desc = CopyString(newdesc);

	return 1;

}	/* SetFieldListDesc */


/*
** FieldIsPermanent
**
** FUNCTION
**	Return the status of the permanent flag on a field.
**
** ARGUMENTS
**	GenesisObject* object	- The object to test against
**	char* fieldname		- The name of the field to test
**
** RETURN VALUE
**	int			- Return 1 if the field is permanent, 0 if not.
**
** AUTHOR
**	David Bilitch
*/

int FieldIsPermanent(object, fieldname)

GenesisObject*	object;
char*		fieldname;

{	/* FieldIsPermanent --- Return status of field permanence */

	FieldList*	fl;

	fl = FindFieldList(object->fieldlist, fieldname);
	if (fl == NULL)
	    return 0;

	return (fl->flags&FIELD_PERMANENT) == FIELD_PERMANENT;

}	/* FieldIsPermanent */


/*
** FieldListExists
**
** FUNCTION
**	Determine if a field exists in the FieldList.
**
** ARGUMENTS
**	GenesisObject* object	- Object with fieldlist to check
**	char* fieldname		- field name to check for
**
** RETURN VALUE
**	int			- 1 if field is in the list, 0 if not
**
** AUTHOR
**	David Bilitch
*/

int FieldListExists(object, fieldname)

GenesisObject*	object;
char*		fieldname;

{	/* FieldListExists --- */

	return FindFieldList(object->fieldlist, fieldname) != NULL;

}	/* FieldListExists */


void CopyFieldList(src, dst)

GenesisObject*	src;
GenesisObject*	dst;

{	/* CopyFieldList --- Copy the fieldlist from one object onto another */

	FieldList*	srcfl;
	FieldList*	dstfl;

	for (srcfl = src->fieldlist; srcfl != NULL; srcfl = srcfl->next)
	  {
	    AddFieldList(dst, srcfl->name);
	    dstfl = FindFieldList(dst->fieldlist, srcfl->name);
	    dstfl->desc = CopyString(srcfl->desc);
	    dstfl->flags = srcfl->flags&(~FIELD_PERMANENT);
	    dstfl->info = srcfl->info;
	  }

}	/* CopyFieldList */


void DisplayFieldList(object)

GenesisObject*	object;

{	/* DisplayFieldList --- Display the field list for showobject */

	FieldList*	fl;

	fl = object->fieldlist;
	if (fl == NULL)
	    return;

	printf("\nFIELDS%s\n", FieldListDebug != 0 ? " (debugging on)" : "");
	while (fl != NULL)
	  {
	    int		prot;
	    char*	protstr;
	    char*	defval;

	    prot = fl->flags&FIELD_PROT;
	    switch (prot)
	      {

	      default:
	      case FIELD_READWRITE:
		protstr = "";
		break;

	      case FIELD_READONLY:
		protstr = "(ro)";
		break;

	      case FIELD_HIDDEN:
		protstr = "(hidden)";
		break;

	      }

	    defval = ElmFieldValue(object->defaults, fl->name);
	    if (defval == NULL)
		defval = "";
	    printf("  %-10s %-15s %-10s %s\n", protstr,
		fl->name, defval, fl->desc);

	    fl = fl->next;
	  }

}	/* DisplayFieldList */


/*
** FieldListEqual
**
** FUNCTION
**	Determine if two fieldlists are the same.
**
** ARGUMENTS
**	FieldList* list1	- The two lists to compare.
**	FieldList* list2
**
** RETURN VALUE
**	int			- 1 if the lists are the same, 0 if not.
**
** AUTHOR
**	David Bilitch
*/

int FieldListEqual(list1, list2)

FieldList*	list1;
FieldList*	list2;

{	/* FieldListEqual --- */

	FieldList*	f1;
	FieldList*	f2;

	f1 = list1;
	f2 = list2;
	while (f1 != NULL && f2 != NULL)
	  {
	    f1 = f1->next;
	    f2 = f2->next;
	  }

	if (f1 != f2)
	    return 0;

	for (f1 = list1; f1 != NULL; f1 = f1->next)
	  {
	    for (f2 = list2; f2 != NULL; f2 = f2->next)
	      {
		if (f1->flags != f2->flags)
		    continue;

		if (strcmp(f1->name, f2->name) == 0)
		    break;
	      }

	    if (f2 == NULL)
		return 0;
	  }

	return 1;

}	/* FieldListEqual */


/*
** FieldListMakePermanent
**
** FUNCTION
**	Sets the FIELDLIST_PERMANENT flag for all the new fields in the
**	fieldlist.
**
** ARGUMENTS
**	GenesisObject* object	- object with fieldlist to make permanent
**
** RETURN VALUE
**	None.
**
** AUTHOR
**	David Bilitch
*/

void FieldListMakePermanent(object)

GenesisObject*	object;

{	/* FieldListMakePermanent --- Make object's field permanent */

	FieldList*	fl;

	fl = object->fieldlist;
	while (fl != NULL && (fl->flags&FIELD_PERMANENT) != FIELD_PERMANENT)
	  {
	    fl->flags |= FIELD_PERMANENT;
	    fl = fl->next;
	  }

}	/* FieldListMakePermanent */


FLI* FLIOpen(object)

GenesisObject*	object;

{	/* FLIOpen --- Open a field list iterator for the given object */

	FLI*	fli;

	fli = (FLI*) malloc(sizeof(FLI));
	if (fli == NULL)
	  {
	    perror("FLIOpen");
	    return NULL;
	  }

	fli->fli_object = object;
	fli->fli_fl = object->fieldlist;

	return fli;

}	/* FLIOpen */


void FLIClose(fli)

FLI*	fli;

{	/* FLIClose --- Close the field list iterator */

	if (fli != NULL)
	    free(fli);

}	/* FLIClose */


int FLIValid(fli)

FLI*	fli;

{	/* FLIValid --- Return TRUE if we have a valid field list reference */

	return fli != NULL && fli->fli_fl != NULL;

}	/* FLIValid */


void FLINext(fli)

FLI*	fli;

{	/* FLINext --- Go to the next field list */

	if (fli != NULL && fli->fli_fl != NULL)
	    fli->fli_fl = fli->fli_fl->next;

}	/* FLINext */


int FLIType(fli)

FLI*	fli;

{	/* FLIType --- Return the field type of the field list entry */

	if (fli == NULL || fli->fli_fl == NULL)
	    return -1;

	return fli->fli_fl->flags&FIELD_TYPE;

}	/* FLIType */



int FLIProt(fli)

FLI*	fli;

{	/* FLIProt --- Return the field protection of the field list entry */

	if (fli == NULL || fli->fli_fl == NULL)
	    return -1;

	if (FieldListDebug != 0 && (fli->fli_fl->flags&FIELD_PROT) == FIELD_HIDDEN)
	    return FIELD_READONLY;

	return fli->fli_fl->flags&FIELD_PROT;

}	/* FLIProt */



char* FLIName(fli)

FLI*	fli;

{	/* FLIName --- Return field name for the field list entry */

	if (fli == NULL || fli->fli_fl == NULL)
	    return NULL;

	return fli->fli_fl->name;

}	/* FLIName */



void* FLIInfo(fli)

FLI*	fli;

{	/* FLIInfo --- Return pointer to info for the field list entry */

	if (fli == NULL || fli->fli_fl == NULL)
	    return NULL;

	return (void*) &fli->fli_fl->info;

}	/* FLIInfo */


int FLIFind(fli, fieldName)

FLI*	fli;
char*   fieldName;

{	/* FLIFind --- Find a field list entry by field name */

	if (fli == NULL)
	    return 0;

	fli->fli_fl = FindFieldList(fli->fli_object->fieldlist, fieldName);
	return fli->fli_fl != NULL;

}	/* FLIFind */

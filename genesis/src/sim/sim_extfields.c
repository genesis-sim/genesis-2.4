static char rcsid[] = "$Id: sim_extfields.c,v 1.3 2005/07/01 10:03:09 svitak Exp $";

/*
** Code to handle extended element fields.  GENESIS command functions are:
**
**	do_addfield	- Add an extended field to an object or element
**	do_deletefield	- Delete a previously defined extended field from
**			  an object or element
**
** Externally callable functions are:
**
**	AddExtField	- Do the work for the addfield command
**	DelExtField	- Do the work for the deletefield command
**	DelAllExtFields	- Delete all fields for the delete command
**	SetExtField	- Sets an extended fields value
**	GetExtField	- Gets an extended fields value
**	GetExtFieldAdr	- Returns the address of the extended field for the
**			  message sending code
**	FreeExtFieldAdr - Free an extended field address if necessary
**	GetExtFieldNameByAdr
**			- Returns the extended field name referenced by the
**			  given address of the extended field value for the
**			  message copying code
**	CopyExtFields	- Copy extended fields defined on one element onto a
**			  second for the copy command ... destination of copy
**			  is assumed to have no valid extended fields
**	MemUsageExtFields
**			- Return number of bytes of memory used for extended
**			  fields
**	DisplayExtFields
**			- Displays all the extended fields for the show command
**	AddExtFieldsToString
**			- Concatenates the names of extended fields to a string
**			  for the getfields command
**	AddExtFieldsToFieldList
**			- Adds the existing extended fields on an element to
**			  the fieldlist of an object for the addobject command
**			  (extended objects)
**
** $Log: sim_extfields.c,v $
** Revision 1.3  2005/07/01 10:03:09  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
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
** Revision 1.16  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.15  2001/04/01 19:31:21  mhucka
** Small cleanup fixes involving variable inits.
**
** Revision 1.14  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.13  1996/07/19 23:57:33  dhb
** Modulated previous change so that relaxing field protections
** past base object level is only possible in setfieldprot debug
** mode.
**
 * Revision 1.12  1996/07/16  22:12:27  dhb
 * Now allow -debug option to setfieldprot command which treats
 * hidden fields as if they were readonly.  Also now allow any
 * manual relaxing of field protection using setfieldprot with
 * a verbose warning message.  NOTE: the latter change makes it
 * possible for a user to crash GENESIS by making critical
 * internal fields writable and clobbering the fields.
 *
 * Revision 1.11  1995/07/08  02:00:13  dhb
 * SetExtField() changed to switch ActiveElement and working
 * elements back to original values before calling SetElement().
 *
 * Revision 1.10  1995/03/02  18:25:14  dhb
 * Modified check for permanent fields in do_deletefield().
 *
 * Revision 1.9  1994/08/31  02:37:23  dhb
 * Fix to do_addfield() to handle complex field addressing in indirect
 * fields (e.g. addfield elm fieldname X->table[0]).
 *
 * Revision 1.8  1994/05/27  22:24:13  dhb
 * Merged in 1.5.1.1 changes
 *
<<<<<<< 1.7
 * Revision 1.7  1994/04/02  01:11:09  dhb
 * Syntax error in previous change.
 *
 * Revision 1.6  1994/04/02  01:09:41  dhb
 * Changed DisplayExtFields() not to display hidden fields.
 *
=======
 * Revision 1.5.1.1  1994/05/27  22:00:02  dhb
 * Changed implementation of extended fields.  Element maintains an array
 * of extended field values rather than name value pairs.  The fieldlist
 * stores the extended field value index in its info data.
 *
 * Added indirect fields.
 *
 * Added the FreeExtFieldAdr() function to free memory associated with
 * addressing information for extended and indirect fields in message
 * slots.
 *
>>>>>>> 1.5.1.1
 * Revision 1.5  1993/12/23  02:29:47  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.4  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.3  1993/03/18  17:15:28  dhb
 * Element field protection.
 *
 * Revision 1.2  1993/03/11  18:11:41  dhb
 * Made messages work for extended fields.
 *
 * Revision 1.1  1993/03/10  23:11:35  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

int AddExtField(element)

Element*	element;

{	/* AddExtField --- Add an extended field */

	int	i;

	for (i = 0; i < element->nextfields; i++)
	  {
	    if (element->extfields[i] == NULL)
		break;
	  }

	if (i >= element->nextfields)
	  {
	    i = element->nextfields;
	    if (element->extfields == NULL)
		element->extfields = (char**) malloc(sizeof(char*) * (i+1));
	    else
		element->extfields = (char**) realloc(element->extfields,
							  sizeof(char*) * (i+1));

	    if (element->extfields == NULL)
	      {
		printf("Error allocating memory for additional extended field\n");
		printf("Extended fields on element %s lost\n", Pathname(element));
		element->nextfields = 0;

		return -1;
	      }

	    element->nextfields++;
	  }

	element->extfields[i] = CopyString("");

	return i;

}	/* AddExtField */


int DelExtField(element, index)

Element*	element;
int		index;

{	/* DelExtField --- Delete an extended field from the element */


	if (index < 0 || element->nextfields <= index)
	    return 0;

	element->extfields[index] = NULL;
	return 1;

}	/* DelExtField */


void DelAllExtFields(element)

Element*	element;

{	/* DelAllExtFields --- Remove all the extended fields */


	if (element->extfields != NULL)
	    free(element->extfields);

	element->extfields = NULL;
	element->nextfields = 0;

}	/* DelAllExtFields */


int SetExtField(element, fieldname, value)

Element*	element;
char*		fieldname;
char*		value;

{	/* SetExtField --- Set an extended/indirect field value */

	int	result = 1;

	switch (GetFieldListType(element->object, fieldname))
	  {
	  case FIELD_INDIRECT:
	      {
		Element*	indElm;
		Element*	cwe;
		Element*	actElm;
		IndFieldInfo*	indInfo;

		indInfo = GetFieldListInfo(element->object, fieldname);

		/* Get the indirect element relative to the current element */
		cwe = WorkingElement();
		SetWorkingElement(element);
		actElm = ActiveElement;
		ActiveElement = element;
		indElm = GetElement(indInfo->ind_elm);
		SetWorkingElement(cwe);
		ActiveElement = actElm;

		if (indElm == NULL)
		  {
		    printf("SetExtField: dangling indirect field reference\n");
		    printf("from element %s field %s to element %s field %s\n",
			     Pathname(element), fieldname,
			     indInfo->ind_elm, indInfo->ind_field);
		    result = 0;
		  }
		else
		    result = SetElement(indElm, indInfo->ind_field, value);
	      }
	    break;

	  case FIELD_EXTENDED:
	      {
		ExtFieldInfo*	extInfo;
		char**		pField;
		int		fieldIndex;

		extInfo = GetFieldListInfo(element->object, fieldname);

		fieldIndex = extInfo->ext_index;
		if (fieldIndex < 0 || element->nextfields <= fieldIndex)
		  {
		    printf("SetExtField: bad field index from fieldlist\n");
		    result = 0;
		  }
		else
		  {
		    pField = element->extfields + fieldIndex;
		    if (*pField != NULL)
			free(*pField);

		    *pField = CopyString(value);
		  }
	      }
	    break;

	  default:
	    result = 0;
	    break;

	  }

	return result;

}	/* SetExtField */

char* GetExtField(element, fieldname)

Element*	element;
char*		fieldname;

{	/* GetExtField --- Return value of an extended field */

	char*	result = NULL;

	switch (GetFieldListType(element->object, fieldname))
	  {
	  case FIELD_INDIRECT:
	      {
		Element*	indElm;
		Element*	cwe;
		Element*	actElm;
		IndFieldInfo*	indInfo = GetFieldListInfo(element->object, fieldname);

		/* Get the indirect element relative to the current element */
		cwe = WorkingElement();
		SetWorkingElement(element);
		actElm = ActiveElement;
		ActiveElement = element;
		indElm = GetElement(indInfo->ind_elm);

		if (indElm == NULL)
		  {
		    printf("GetExtField: dangling indirect field reference\n");
		    printf("from element %s field %s to element %s field %s\n",
			     Pathname(element), fieldname,
			     indInfo->ind_elm, indInfo->ind_field);
		  }
		else
		    result = ElmFieldValue(indElm, indInfo->ind_field);

		SetWorkingElement(cwe);
		ActiveElement = actElm;
	      }
	    break;

	  case FIELD_EXTENDED:
	      {
		ExtFieldInfo*	extInfo = GetFieldListInfo(element->object, fieldname);
		int		fieldIndex;

		fieldIndex = extInfo->ext_index;
		if (fieldIndex < 0 || element->nextfields <= fieldIndex)
		  {
		    printf("GetExtField: bad field index from fieldlist\n");
		  }
		else
		    result = element->extfields[fieldIndex];
	      }
	    break;

	  }

	return result;

}	/* GetExtField */


int CopyExtFields(src, dst)

Element*	src;
Element*	dst;

{	/* CopyExtFields --- Copy extended fields from src to dst element */

	char**	fld;
	int	ntocopy;
	int	i;

	ntocopy = 0;
	fld = src->extfields;
	for (i = 0; i < src->nextfields; i++, fld++)
	    if (*fld != NULL)
		ntocopy++;

	dst->extfields = NULL;
	dst->nextfields = ntocopy;

	if (ntocopy > 0)
	  {
	    dst->extfields = (char**) 
				malloc(sizeof(char*)*ntocopy);
	    if (dst->extfields == NULL)
	      {
		printf("CoptExtFields: Unable to allocate memory for extended fields on element '%s'\n", Pathname(dst));
		dst->nextfields = 0;
		return 0;
	      }

	    fld = dst->extfields;
	    for (i = 0; i < src->nextfields; i++)
	      {
		if (src->extfields[i] != NULL)
		  {
		    *fld = CopyString(src->extfields[i]);
		    fld++;
		  }
	      }
	  }

	return 1;

}	/* CopyExtFields */


int MemUsageExtFields(element)

Element*	element;

{	/* MemUsageExtFields --- Return bytes used in extended fields */

	int	nbytes;
	int	i;

	nbytes = sizeof(char*)*element->nextfields;
	for (i = 0; i < element->nextfields; i++)
	  {
	    if (element->extfields[i] != NULL)
	      {
		nbytes += strlen(element->extfields[i]) + 1;
	      }
	  }

	return nbytes;

}	/* MemUsageExtFields */


void DisplayExtFields(element)

Element*	element;

{	/* DisplayExtFields --- Display the extended fields */

	void*	fli;

	fli = FLIOpen(element->object);

	while (FLIValid(fli))
	  {
	    if (FLIProt(fli) != FIELD_HIDDEN)
		switch (FLIType(fli))
		  {

		  case FIELD_EXTENDED:
		    FieldFormat(FLIName(fli),
			element->extfields[((ExtFieldInfo*)FLIInfo(fli))->ext_index]);
		    break;

		  case FIELD_INDIRECT:
		      {
			Element*	cwe;
			Element*	actElm;
			Element*	indElm;
			IndFieldInfo*	indInfo = FLIInfo(fli);

			/* Get the indirect element relative to the element */
			cwe = WorkingElement();
			SetWorkingElement(element);
			actElm = ActiveElement;
			ActiveElement = element;
			indElm = GetElement(indInfo->ind_elm);
			SetWorkingElement(cwe);
			ActiveElement = actElm;

			if (indElm == NULL)
			  {
			    printf("DisplayExtField: dangling indirect field reference\n");
			    printf("from element %s field %s to element %s field %s\n",
				     Pathname(element), FLIName(fli),
				     indInfo->ind_elm, indInfo->ind_field);
			  }
			else
			FieldFormat(FLIName(fli),
				    ElmFieldValue(indElm, indInfo->ind_field));
		      }
		    break;

		  }

	    FLINext(fli);
	  }

	FLIClose(fli);

}	/* DisplayExtFields */


void AddExtFieldsToString(element, string)

Element*	element;
char*		string;

{	/* AddExtFieldsToString --- Concatenate ext field names to string */

	void*	fli;

	fli = FLIOpen(element->object);
	while (FLIValid(fli))
	  {
	    if (FLIType(fli) != FIELD_BUILTIN)
	      {
		char	buf[100];

		sprintf(buf, " %s", FLIName(fli));
		strcat(string, buf);
	      }

	    FLINext(fli);
	  }
	
	FLIClose(fli);

}	/* AddExtFieldsToString */


typedef struct _ext_field_adr
  {
    Element*	efa_src;
    int		efa_index;
  } ExtFieldAdr;

typedef struct _ind_field_adr
  {
    Element*		ifa_src;
    IndFieldInfo*	ifa_info;
  } IndFieldAdr;

double ExtFieldMessageData(slot)
Slot *slot;
{
    ExtFieldAdr*	efa;
    char*		data;
    extern double	VoidMessageData();

    efa = (ExtFieldAdr*) (slot->data);
    data = efa->efa_src->extfields[efa->efa_index];
    if (data == NULL)	/* External field has been removed!!! */
      {
	slot->func = VoidMessageData;
	return 0.0;
      }

    return Atof(data);
}


double IndFieldMessageData(slot)
Slot *slot;
{
    Element*		indElm;
    Element*		actElm;
    Element*		cwe;
    IndFieldAdr*	ifa;

    ifa = (IndFieldAdr*) slot->data;

    cwe = WorkingElement();
    SetWorkingElement(ifa->ifa_src);
    actElm = ActiveElement;
    ActiveElement = ifa->ifa_src;
	indElm = GetElement(ifa->ifa_info->ind_elm);
    SetWorkingElement(cwe);
    ActiveElement = actElm;

    if (indElm == NULL)
      {
	printf("IndFieldMessageData: dangling indirect field reference\n");
	printf("from element %s to element %s field %s\n",
		 Pathname(ifa->ifa_src),
		 ifa->ifa_info->ind_elm, ifa->ifa_info->ind_field);
	return 0.0;
      }

    return Atof(ElmFieldValue(indElm, ifa->ifa_info->ind_field));
}


char* GetExtFieldAdr(element, fieldname, func)

Element*	element;
char*		fieldname;
PFD*		func;

{	/* GetExtFieldAdr --- Return address of extended field value */

	char*		result;

	result = NULL;
	switch (GetFieldListType(element->object, fieldname))
	  {
	  case FIELD_INDIRECT:
	      {
		IndFieldInfo*	indInfo;
		IndFieldAdr*	ifa;

		indInfo = GetFieldListInfo(element->object, fieldname);
		ifa = (IndFieldAdr*) malloc(sizeof(IndFieldAdr));
		if (ifa == NULL)
		  {
		    perror("GetExtFieldAdr");
		    return NULL;
		  }

		ifa->ifa_src = element;
		ifa->ifa_info = indInfo;

		if (func != NULL)
		    *func = IndFieldMessageData;
		result = (char*) ifa;
	      }
	    break;

	  case FIELD_EXTENDED:
	      {
		ExtFieldInfo*	extInfo;
		int		fieldIndex;

		extInfo = GetFieldListInfo(element->object, fieldname);

		fieldIndex = extInfo->ext_index;
		if (fieldIndex < 0 || element->nextfields <= fieldIndex)
		  {
		    printf("GetExtFieldAdr: bad extended field index from fieldlist\n");
		  }
		else
		  {
		    ExtFieldAdr*	efa;

		    efa = (ExtFieldAdr*) malloc(sizeof(ExtFieldAdr));
		    if (efa == NULL)
		      {
			perror("GetExtFieldAdr");
			return NULL;
		      }

		    efa->efa_src = element;
		    efa->efa_index = fieldIndex;

		    if (func != NULL)
			*func = ExtFieldMessageData;
		    result = (char*) efa;
		  }
	      }
	    break;

	  }

	return result;

}	/* GetExtFieldAdr */


void FreeExtFieldAdr(adr, func)
	void*	adr;
	PFD	func;

{	/* FreeExtFieldAdr --- Free the extended field address structure */

	if (func == ExtFieldMessageData || func == IndFieldMessageData)
	    free(adr);

}	/* FreeExtFieldAdr */


char* GetExtFieldNameByAdr(element, adr)

Element*	element;
char*		adr;

{	/* GetExtFieldNameByAdr --- Return extended field name of referenced
				    field */

	FLI*		fli;
	char*		fieldName;
	int		fieldIndex;
	ExtFieldAdr*	efa;

	fli = FLIOpen(element->object);
	fieldName = NULL;
	while (fieldName == NULL && FLIValid(fli))
	  {
	    switch (FLIType(fli))
	      {

	      case FIELD_EXTENDED:
		fieldIndex = ((ExtFieldInfo*) FLIInfo(fli))->ext_index;
		efa = (ExtFieldAdr*) adr;
		if (efa->efa_index == fieldIndex)
		    fieldName = FLIName(fli);
		break;

	      case FIELD_INDIRECT:
		if (FLIInfo(fli) == ((IndFieldAdr*)adr)->ifa_info)
		    fieldName = FLIName(fli);
		break;

	      }

	    FLINext(fli);
	  }
	FLIClose(fli);

	return fieldName;

}	/* GetExtFieldNameByAdr */


/*
** do_addfield
**
** FUNCTION
**	The addfield command adds a field to the current working element or
**	a given element.
**
** ARGUMENTS
**	int argc	- The number of command arguments
**	char** argv	- The command arguments
**
** RETURN VALUE
**	int		- 1 if the command succeeds, 0 if the field
**			  already exists, a modified object cannot be
**			  created or a named element does not exist.
**
** SIDE EFFECTS
**	Adding a field to an element changes its object.  If this is the
**	first modification of the object this results in an object derived
**	from the original object.  All subsequent modifiactions of the object
**	continue to be derivations from this same original object.
**
** AUTHOR
**	David Bilitch
*/

int do_addfield(argc, argv)

int	argc;
char**	argv;

{	/* do_addfield --- Add an extended field to an object or element */

	GenesisObject*	object;
	Element*	element;
	Element*	indElement = NULL;
	char*		fieldName;
	char*		fieldDesc;
	char*		indElmName;
	char*		indFieldName = NULL;
	int		status;

	fieldDesc = NULL;
	indElmName = NULL;
	initopt(argc, argv, "[element] field-name -indirect element field -description text");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-description") == 0)
		fieldDesc = optargv[1];
	    else if (strcmp(G_optopt, "-indirect") == 0)
	      {
		indElmName = optargv[1];
		indFieldName = optargv[2];
	      }
	  }

	if (status < 0 || optargc > 3)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	if (optargc == 2)
	    element = WorkingElement();
	else /* optargc == 3 */
	  {
	    element = GetElement(optargv[1]);
	    if (element == NULL)
	      {
		InvalidPath(optargv[0], optargv[1]);
		return 0;
	      }
	  }
	fieldName = optargv[optargc-1];

	if (FieldListExists(element->object, fieldName))
	  {
		Error();
	    printf("%s: Field '%s' already exists on element '%s'\n",
		optargv[0], fieldName, Pathname(element));
	    return 0;
	  }

	if (indElmName != NULL)
	  {
	    Element*	cwe;
	    Element*	actElm;

	    /* find indElement relative to element */
	    cwe = WorkingElement();
	    SetWorkingElement(element);
	    actElm = ActiveElement;
	    ActiveElement = element;
		indElement = GetElement(indElmName);
	    SetWorkingElement(cwe);
	    ActiveElement = actElm;

	    if (indElement == NULL)
	      {
		Error(); printf("%s: can't find indirect element %s\n",
				    optargv[0], indElmName);
		return 0;
	      }

	    if (ElmFieldValue(indElement, indFieldName) == NULL)
	      {
		Error(); printf("%s: can't find indirect element field %s\n",
				    optargv[0], indFieldName);
		return 0;
	      }
	  }

	object = ObjectModify(element->object);
	if (object == NULL)
	  {
	    fprintf(stderr, "%s: Failure to modify object; cannot add field '%s' to element '%s'\n",
		optargv[0], fieldName, Pathname(element));
	    return 0;
	  }

	AddFieldList(object, fieldName);
	SetFieldListProt(object, fieldName, FIELD_READWRITE);
	if (fieldDesc != NULL)
	    SetFieldListDesc(object, fieldName, fieldDesc);

	if (indElmName != NULL)
	  {
	    IndFieldInfo*	info;

	    info = (IndFieldInfo*) GetFieldListInfo(object, fieldName);
	    info->ind_elm = CopyString(indElmName);
	    info->ind_field = CopyString(indFieldName);
	    SetFieldListType(object, fieldName, FIELD_INDIRECT);
	    SetFieldListProt(object, fieldName,
		GetFieldListProt(indElement->object,
					FieldNameProper(indFieldName)));
	  }
	else
	  {
	    ExtFieldInfo*	info;

	    info = (ExtFieldInfo*) GetFieldListInfo(object, fieldName);
	    info->ext_index = AddExtField(element);
	    SetFieldListType(object, fieldName, FIELD_EXTENDED);
	  }

	object = ObjectCacheCheck(object);
	element->object = object;

	OK();
	return 1;

}	/* do_addfield */


/*
** do_deletefield
**
** FUNCTION
**	The deletefield command deletes a field from the current working
**	element or a named element.
**
** ARGUMENTS
**	int argc	- The number of command arguments
**	char** argv	- The command arguments
**
** RETURN VALUE
**	int		- 1 if the command succeeds, 0 if the field is
**			  permanent (e.g. part of a named object), the
**			  field does not exist on the element, a modified
**			  object cannot be created or a named element does
**			  not exist.
**
** SIDE EFFECTS
**	Deleting a field from an element changes its object.
**
** AUTHOR
**	David Bilitch
*/

int do_deletefield(argc, argv)

int	argc;
char**	argv;

{	/* do_deletefield --- Delete extended field from an element */

	GenesisObject*	object;
	Element*	element;
	char*		field;

	initopt(argc, argv, "[element] field-name");
	if (G_getopt(argc, argv) != 0 || optargc > 3)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	field = optargv[optargc-1];

	if (optargc == 2)
	    element = WorkingElement();
	else /* optargc == 3 */
	  {
	    char*	elementname;

	    elementname = optargv[1];
	    element = GetElement(elementname);
	    if (element == NULL)
	      {
		InvalidPath(optargv[0], elementname);
		return 0;
	      }
	  }

	if (!FieldListExists(element->object, field))
	  {
		Error();
	    printf("%s: Field '%s' does not exist on element '%s'\n",
		optargv[0], field, Pathname(element));
	    return 0;
	  }

	if (!ObjectIsModified(element->object) ||
	    FieldListExists(SuperObject(element->object), field))
	  {
		Error();
	    printf("%s: Cannot delete permanent field '%s' on element '%s'\n",
		optargv[0], field, Pathname(element));
	    return 0;
	  }

	object = ObjectModify(element->object);
	if (object == NULL)
	  {
		Error();
	    printf("%s: Failure to modify object; cannot delete field '%s' from element '%s'\n",
		optargv[0], field, Pathname(element));
	    return 0;
	  }

	switch (GetFieldListType(object, field))
	  {
	  case FIELD_INDIRECT:
	      {
		IndFieldInfo*	indInfo;

		indInfo = (IndFieldInfo*) GetFieldListInfo(object, field);

		free(indInfo->ind_elm);
		free(indInfo->ind_field);
	      }
	    break;

	  case FIELD_EXTENDED:
	      {
		ExtFieldInfo*	extInfo;

		extInfo = (ExtFieldInfo*) GetFieldListInfo(object, field);

		DelExtField(element, extInfo->ext_index);
	      }
	    break;

	  }


	DelFieldList(object, field);

	object = ObjectCacheCheck(object);
	element->object = object;

	OK();
	return 1;

}	/* do_deletefield */


/*
** do_setfieldprot
**
** FUNCTION
**	The setfieldprot command changes the protection for an element field.
**
** ARGUMENTS
**	int argc	- number of command arguments
**	char** argv	- command arguments
**
** RETURN VALUE
**	int		- 1 on successful or 0 if not (element not found,
**			  field not found or field cannot be unprotected)
**
** SIDE EFFECTS
**	Changing a field protection changes the element object.  The first
**	change to an object results in a new object derived from the original.
**	All subsequent changes to the object continue to be derived from this
**	original object.
**
** AUTHOR
**	David Bilitch
*/

int do_setfieldprot(argc, argv)

int	argc;
char**	argv;

{	/* do_setfieldprot --- Change a element field protection */

	static char*	initoptStr = "[element] -debug -nodebug -readwrite fields ... -readonly fields ... -hidden fields ...";
	static int	dbgWarnedOnce = 0;

	GenesisObject*	object;
	Element*	elm;
	char**		fields;
	int		nFields;
	char*		fieldName;
	int		minProt;
	int		prot;
	int		status;
	int		i;

	initopt(argc, argv, initoptStr);
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-debug") == 0)
	      {
		SetFieldListDebug(1);
		printf("%s debug on\n", argv[0]);
		if (!dbgWarnedOnce)
		  {
		    printf("NOTE: this option is provided for debugging of\n");
		    printf("      new objects.  If this option is used\n");
		    printf("      routinely to access hidden or readonly fields\n");
		    printf("      please notify the GENESIS development group\n");
		    printf("      so that the default field protection might be\n");
		    printf("      reconsidered.\n");

		    dbgWarnedOnce = 1;
		  }
	      }
	    else if (strcmp(G_optopt, "-nodebug") == 0)
	      {
		SetFieldListDebug(0);
		printf("%s debug off\n", argv[0]);
	      }
	  }

	if (status < 0 || optargc > 2)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	if (optargc < 2)
	  {
	    OK();
	    return 1;
	  }

	elm = GetElement(optargv[1]);
	if (elm == NULL)
	  {
	    InvalidPath(optargv[0], optargv[1]);
	    return 0;
	  }

	object = NULL;
	initopt(argc, argv, initoptStr);
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    prot = -1;
	    if (strcmp(G_optopt, "-readonly") == 0)
		prot = FIELD_READONLY;
	    else if (strcmp(G_optopt, "-hidden") == 0)
		prot = FIELD_HIDDEN;
	    else if (strcmp(G_optopt, "-readwrite") == 0)
		prot = FIELD_READWRITE;

	    if (prot == -1)
		continue;

	    nFields = optargc -1;
	    fields = optargv+1;
	    for (i = 0; i < nFields; i++)
	      {
		fieldName = fields[i];
		if (!FieldListExists(elm->object, fieldName))
		  {
			Error();
		    printf("%s: Field '%s' does not exist on element '%s'\n",
			optargv[0], fieldName, Pathname(elm));
		    continue; /* to the next field */
		  }

		if (FieldIsPermanent(elm->object, fieldName))
		    minProt = GetFieldListProt(elm->object, fieldName);
		else
		  {
		    if (SuperObject(elm->object) == NULL)
			minProt = GetFieldListProt(elm->object, fieldName);
		    else
			minProt = GetFieldListProt(SuperObject(elm->object), fieldName);
		  }

		if (prot < minProt)
		  {
		    /* relaxing protection past base object setting */

		    if (GetFieldListDebug() == 0)
		      {
			fprintf(stderr, "%s: access to field '%s' on element '%s' cannot exceed %s\n",
				argv[0], fieldName, Pathname(elm),
				minProt == FIELD_READONLY ? "readonly" : "hidden");
			continue; /* next field */
		      }

		    /* debug mode: allow greater field accessibility */

		    fprintf(stderr, "%s: access to field '%s' on element '%s' exceeding default of %s\n",
			    argv[0], fieldName, Pathname(elm),
			    minProt == FIELD_READONLY ? "readonly" : "hidden");
		  }

		if (object == NULL)
		  {
		    object = ObjectModify(elm->object);

		    if (object == NULL)
		      {
			fprintf(stderr, "%s: Failure to modify object; cannot change field protections on element '%s'\n",
			    argv[0], Pathname(elm));
			return 0;
		      }
		  }

		SetFieldListProt(object, fieldName, prot);
	      }
	  }

	if (object != NULL)
	    elm->object = ObjectCacheCheck(object);

	OK();
	return 1;

}	/* do_setfieldprot */

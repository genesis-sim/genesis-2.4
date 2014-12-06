/* $Id: XoArgLists.c,v 1.2 2005/06/17 17:37:05 svitak Exp $ */
/*
 * $Log: XoArgLists.c,v $
 * Revision 1.2  2005/06/17 17:37:05  svitak
 * Deprecated varargs.h replaced by stdarg.h. Old-style argument lists updated.
 * These changes were part of varargs-genesis.patch. Systems without stdarg.h
 * will no longer compile.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.6  2000/06/12 04:28:20  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.5  2000/05/26 23:36:13  mhucka
 * The declaration for _XoXtNestedArgtoArg() had a stupid typo.
 *
 * Revision 1.4  1995/12/06 01:14:36  venkat
 * XtConvertAmnd
 *
 * XtConvertAndStore() has a buggy implementation in R4. So changed the calll
 * to XtConvert
 *
 * Revision 1.3  1995/09/27  20:15:41  venkat
 * Changed XPointer to XtPointer to avoid compilation errors
 *
 * Revision 1.2  1995/09/27  00:19:19  venkat
 * Contains the Xodus wrapper XoXtVaSetValues() for the Xtoolkit's XtVaSetValues()
 * and other supporting local functions, the use of which avoids FPE's and other
 * problems on the DEC alpha.
 *
 * Revision 1.1  1994/01/13  18:34:14  bhalla
 * Initial revision
 * */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <stdarg.h>
#include <stdio.h>

 static String XtNxtConvertVarToArgList = "xtConvertVarToArgList";
 static String XtCXtToolkitError = "xttoolkiterror";
 typedef struct {
    String      name;   /* resource name */
    String      type;   /* representation type name */
    XtArgVal    value;  /* representation */
    int         size;   /* size of representation */
} XoXtTypedArg, *XoXtTypedArgList;

 static void _XoXtVaToArgList();
 static int _XoXtTypedArgToArg();
 static int  _XoXtNestedArgtoArg();
 static void GetResources();

/*
 *    _XoXtTypedArgToArg() invokes a resource converter to convert the
 *    passed typed arg into a name/value pair and stores the name/value
 *    pair in the passed Arg structure. It returns 1 if the conversion
 *    succeeded and 0 if the conversion failed.
 */
static int
_XoXtTypedArgToArg(widget, typed_arg, arg_return, resources, num_resources)
    Widget              widget;
    XoXtTypedArgList      typed_arg;
    ArgList             arg_return;
    XtResourceList      resources;
    Cardinal            num_resources;
{     
    String              to_type = NULL;
    XrmValue            from_val, to_val;
    register int        i;
      

    if (widget == NULL) {
        XtAppWarningMsg(XtWidgetToApplicationContext(widget),
            "nullWidget", XtNxtConvertVarToArgList, XtCXtToolkitError,
	    "XtVaTypedArg conversion needs non-NULL widget handle",
            (String *)NULL, (Cardinal *)NULL);
        return(0);
    }
       
    /* again we assume that the XtResourceList is un-compiled */

    for (i = 0; i < num_resources; i++) {
        if (XrmStringToName(typed_arg->name) ==
            XrmStringToName(resources[i].resource_name)) {
            to_type = resources[i].resource_type;
            break;
        }
    }

    if (to_type == NULL) {
        XtAppWarningMsg(XtWidgetToApplicationContext(widget),
            "unknownType", XtNxtConvertVarToArgList, XtCXtToolkitError,
            "Unable to find type of resource for conversion",
            (String *)NULL, (Cardinal *)NULL);
        return(0);
    }
       
    to_val.addr = NULL;
    from_val.size = typed_arg->size;
    if ((strcmp(typed_arg->type, XtRString) == 0) ||
            (typed_arg->size > sizeof(XtArgVal))) {
        from_val.addr = (XtPointer)typed_arg->value;
    } else {
            from_val.addr = (XtPointer)&typed_arg->value;
    }
    XtConvert(widget, typed_arg->type, &from_val, to_type, &to_val);
    if (to_val.addr == NULL) {
        XtAppWarningMsg(XtWidgetToApplicationContext(widget),
            "conversionFailed", XtNxtConvertVarToArgList, XtCXtToolkitError,
            "Type conversion failed", (String *)NULL, (Cardinal *)NULL);
        return(0);
    }

    arg_return->name = typed_arg->name;

    if (strcmp(to_type, XtRString) == 0) {
	arg_return->value = (XtArgVal) to_val.addr;
    }
    else {
	if (to_val.size == sizeof(long))
	    arg_return->value = (XtArgVal) *(long *)to_val.addr;
	else if (to_val.size == sizeof(short))
	    arg_return->value = (XtArgVal) *(short *)to_val.addr;
	else if (to_val.size == sizeof(char))
	    arg_return->value = (XtArgVal) *(char *)to_val.addr;
	else if (to_val.size == sizeof(int))
	    arg_return->value = (XtArgVal) *(int *)to_val.addr;
	else if (to_val.size == sizeof(XtArgVal))
	    arg_return->value = *(XtArgVal *)to_val.addr;
    }
       
    return(1);
}
/* 
 *    Given a variable argument list, _XoXtVaToArgList() returns the 
 *    equivalent ArgList and count. _XoXtVaToArgList() handles nested 
 *    lists and typed arguments. 
#if NeedFunctionPrototypes
void
_XtVaToArgList(
    Widget		widget,
    va_list     	var,
    int			max_count,
    ArgList		*args_return,
    Cardinal		*num_args_return)
#else
 */
static void
_XoXtVaToArgList(widget, var, max_count, args_return, num_args_return)
    Widget		widget;
    va_list     	var;
    int			max_count;
    ArgList		*args_return;
    Cardinal		*num_args_return;
{
    String		attr;
    int			count = 0;
    ArgList		args = (ArgList)NULL;
    XoXtTypedArg		typed_arg;
    XtResourceList	resources = (XtResourceList)NULL;
    Cardinal		num_resources;
    Boolean		fetched_resource_list = False;

    if (max_count  == 0) {
	*num_args_return = 0;
	*args_return = (ArgList)NULL;
	return;
    }


    args = (ArgList)XtMalloc((unsigned)(max_count * sizeof(Arg)));

    for(attr = va_arg(var, String) ; attr != NULL;
			attr = va_arg(var, String)) {
	if (strcmp(attr, XtVaTypedArg) == 0) {
	    typed_arg.name = va_arg(var, String);
	    typed_arg.type = va_arg(var, String);
	    typed_arg.value = va_arg(var, XtArgVal);
	    typed_arg.size = va_arg(var, int);

	    /* if widget is NULL, typed args are ignored */
	    if (widget != NULL) {
		if (!fetched_resource_list) {
		    GetResources(widget, &resources, &num_resources);
		    fetched_resource_list = True;
		}
		count += _XoXtTypedArgToArg(widget, &typed_arg, &args[count],
			     resources, num_resources);
	    }
	} else if (strcmp(attr, XtVaNestedList) == 0) {
	    if (widget != NULL || !fetched_resource_list) {
		GetResources(widget, &resources, &num_resources);
		fetched_resource_list = True;
	    }

	    count += _XoXtNestedArgtoArg(widget, va_arg(var, XoXtTypedArgList),
			&args[count], resources, num_resources);
	} else {
	    args[count].name = attr;
	    args[count].value = va_arg(var, XtArgVal);
	    count ++;
	}
    }

    XtFree((XtPointer)resources);

    *num_args_return = (Cardinal)count;
    *args_return = (ArgList)args;
}
/*
#if NeedVarargsPrototypes
void
XoXtVaSetValues(Widget widget, ...)
#else
VARARGS1*/
void XoXtVaSetValues(Widget widget, ...)
{
    va_list                 var;
    ArgList                 args = NULL;
    Cardinal                num_args;
    int			    total_count, typed_count;
/*     WIDGET_TO_APPCON(widget); */

    /* LOCK_APP(app); */
    va_start(var, widget);
    _XtCountVaList(var, &total_count, &typed_count);
    va_end(var);

    va_start(var, widget);

    _XoXtVaToArgList(widget, var, total_count, &args, &num_args);
    XtSetValues(widget, args, num_args);
    if (args != NULL) {
	XtFree((XtPointer)args);
    }
/*     UNLOCK_APP(app); */
    va_end(var);
}
/*	Function Name: GetResources
 *	Description: Retreives the normal and constraint resources
 *                   for this widget.
 *	Arguments: widget - the widget.
 * RETURNED        res_list - the list of resource for this widget
 * RETURNED        number - the number of resources in the above list.
 *	Returns: none
 */

static void
GetResources(widget, res_list, number)
Widget widget;
XtResourceList * res_list;
Cardinal * number;
{
    Widget parent = XtParent(widget);

    XtInitializeWidgetClass(XtClass(widget));
    XtGetResourceList(XtClass(widget), res_list, number);
    
    /* assert: !XtIsShell(w) => (XtParent(w) != NULL) */
    if (!XtIsShell(widget) && XtIsConstraint(parent)) {
	XtResourceList res, constraint, cons_top;
	Cardinal num_constraint, temp;

	XtGetConstraintResourceList(XtClass(parent), &constraint, 
				    &num_constraint);

	cons_top = constraint;
	*res_list = (XtResourceList) XtRealloc((char*)*res_list, 
					       ((*number + num_constraint) * 
						sizeof(XtResource)));

	for (temp= num_constraint, res= *res_list + *number; temp != 0; temp--)
	    *res++ = *constraint++;

	*number += num_constraint;
	XtFree( (XtPointer) cons_top);
    }
}
/*
 *    _XoXtNestedArgtoArg() converts the passed nested list into
 *    an ArgList/count.
 */
static int _XoXtNestedArgtoArg(widget, avlist, args, resources, num_resources)
    Widget              widget;
    XoXtTypedArgList      avlist;
    ArgList             args;
    XtResourceList      resources;
    Cardinal            num_resources;
{
    int         count = 0;
 
    for (; avlist->name != NULL; avlist++) {
        if (avlist->type != NULL) {
            /* If widget is NULL, the typed arg is ignored */
            if (widget != NULL) {
                /* this is a typed arg */
                count += _XoXtTypedArgToArg(widget, avlist, (args+count),
                             resources, num_resources);
            }
        } else if (strcmp(avlist->name, XtVaNestedList) == 0) {
            count += _XoXtNestedArgtoArg(widget, (XoXtTypedArgList)avlist->value,
                        (args+count), resources, num_resources);
        } else {
            (args+count)->name = avlist->name;
            (args+count)->value = avlist->value;
            ++count;
        }
    }

    return(count);
}
/*
  void XoCopyArgListOverride (out_list, out_num, in_list, in_num)
     ArgList     out_list;
     Cardinal   *out_num;
     ArgList     in_list;
     Cardinal    in_num;

  merge the list of Args in_list into the list out_list, using arguments
  in in_list to override any of the same name in out_list.  This function 
  assumes that out_list points to enough memory to hold both it's own 
  contents and the new ones.  It will also not destroy any duplications 
  already in out_list.

  

  void XoCopyUnknownArgs (w, out_list, out_num, in_list, in_num)
     Widget      w;
     ArgList     out_list;
     Cardinal   *out_num;
     ArgList     in_list;
     Cardinal    in_num;

  copy to out_list any arguments in in_list that do not refer to valid
  resources for Widget w.


  void XoCopyPrefixArgs (out_list, out_num, in_list, in_num, prefix)
     ArgList     out_list;
     Cardinal   *out_num;
     ArgList     in_list;
     Cardinal    in_num;
     String      prefix;

  copy to out_list any arguments in in_list whose name begins with prefix.
  The prefix is stripped from the name as it is copied.
*/

#define nameeq(a,b)     (strcmp ((a).name, (b).name) == 0)  
#define streq(a,b)      (strcmp ((a), (b)) == 0)  
#define strbegins(a,b)  (strncmp ((a), (b), strlen(b)) == 0)


void XoCopyArgListOverride (out_list, out_num, in_list, in_num)
     ArgList      out_list;
     Cardinal    *out_num;
     ArgList      in_list;
     Cardinal     in_num;
{
  int out_index, in_index;

  for (in_index = 0; in_index < in_num; in_index++) {
    for (out_index = 0; out_index < *out_num; out_index++)
      if ( nameeq (out_list[out_index], in_list[in_index]) ) break;
    out_list[out_index] = in_list[in_index];
    if (out_index == *out_num) (*out_num)++;
  }
}
  

void XoCopyUnknownArgs (w, out_list, out_num, in_list, in_num)
     Widget      w;
     ArgList     out_list;
     Cardinal   *out_num;
     ArgList     in_list;
     Cardinal    in_num;
{
  XtResourceList  res_list;
  Cardinal        res_num;
  int in_index, res_index;

  XtGetResourceList ( XtClass(w), &res_list, &res_num );

  for (in_index = 0; in_index < in_num; in_index++) {
    Boolean found = False;
      
    for (res_index = 0; res_index < res_num; res_index++)
      if ( streq (in_list[in_index].name, res_list[res_index].resource_name) )
	found = True;

    if (! found)
      out_list[(*out_num)++] = in_list[in_index];
      
  }
}


void XoCopyPrefixArgs (out_list, out_num, in_list, in_num, prefix)
     ArgList     out_list;
     Cardinal   *out_num;
     ArgList     in_list;
     Cardinal    in_num;
     String      prefix;
{
  int in_index;

  for (in_index = 0; in_index < in_num; in_index++)
    if ( strbegins (in_list[in_index].name, prefix) ) {
      out_list[*out_num].value = in_list[in_index].value;
      out_list[*out_num].name  = in_list[in_index].name + strlen (prefix);
      (*out_num)++;
    }
}

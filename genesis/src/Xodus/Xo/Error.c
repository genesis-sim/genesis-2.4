/* $Id: Error.c,v 1.2 2005/06/17 17:37:05 svitak Exp $ */
/*
 * $Log: Error.c,v $
 * Revision 1.2  2005/06/17 17:37:05  svitak
 * Deprecated varargs.h replaced by stdarg.h. Old-style argument lists updated.
 * These changes were part of varargs-genesis.patch. Systems without stdarg.h
 * will no longer compile.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2000/06/12 04:28:20  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.1  1994/01/13 18:34:14  bhalla
 * Initial revision
 * */

#include <stdarg.h>
#include <stdio.h>

void _XgPrintToStderr ();

static void (*error_handler)() = _XgPrintToStderr;


void XgError (char *format, ...)
{
  char error[256];
  va_list args;

  va_start(args, format);
  vsprintf (error, format, args);
  va_end(args);
  
  (*error_handler) (error);
}


void _XgPrintToStderr (error) 
     char *error;
{
  fprintf (stderr, "Xodus: %s\n", error);
}


/* $Id: ss_func_ext.h,v 1.4 2006/02/15 18:50:12 svitak Exp $
**
** Script language module interface functions
**
** $Log: ss_func_ext.h,v $
** Revision 1.4  2006/02/15 18:50:12  svitak
** Changes to make asc_file objects use float_format.
**
** Revision 1.3  2005/06/24 21:07:32  svitak
** Fixed type of ExecuteFunction (void back to int).
**
** Revision 1.2  2005/06/24 20:48:22  svitak
** Correct typing of functions returning void.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2003/03/28 21:01:47  gen-dbeeman
** Changes from Hugo Cornelis to enable debugging commands gctrace and gftrace
**
** Revision 1.5  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  2000/10/09 23:01:40  mhucka
** Portability fixes.
**
** Revision 1.3  1996/07/01 20:12:24  dhb
** Added extern for ExecuteStrFunction().
**
 * Revision 1.2  1994/03/19  22:55:58  dhb
 * CreateScriptStr() --> CreateScriptString()
 *
 * Revision 1.1  1994/03/19  22:37:27  dhb
 * Initial revision
 *
*/

/*
** Tracing of commands and functions
*/

int GetFunctionTraceLevel(void);
void SetFunctionTraceLevel(int iLevel);

#ifndef SS_FUNC_EXT_H
#define SS_FUNC_EXT_H

#include "parse.h"

/*
** Script variables
*/

extern double GetScriptDouble(/* char* name */);
extern int GetScriptInt(/* char* name */);
extern char* GetScriptStr(/* char* name */);

extern void SetScriptDouble(/* char* name, double value */);
extern void SetScriptInt(/* char* name, int value */);
extern void SetScriptStr(/* char* name, char* value */);

extern void CreateScriptFloat(/* char* name */);
extern void CreateScriptInt(/* char* name */);
extern void CreateScriptString(/* char* name */);

/*
** Various functions used in scripts and a few other places.
*/

extern int        PTArgc();
extern void       PTCall();
extern Result     PTEval();
extern int        PTFree();
extern void       PTInit();
extern ParseNode *PTNew();
extern void       FreePTValues();

extern char *do_cmd_args();
extern char *strsave(/* char *cp */);
extern char* ExecuteStrFunction(/* int argc, char* argv[] */);
extern int   ArgListType();
extern void  CastToFloat();
extern void  CastToInt();
extern void  CastToStr();
extern int   ExecuteFunction(/* int argc, char* argv[] */);
extern int   FloatType();
extern void  FreeScriptInfo();
extern int   InControlStructure();
extern int   IntType();
extern void  PopLocalVars();
extern void  PushLocalVars();
extern void  SetLine();
extern int   StrType();
extern char *get_float_format();
extern void  set_float_format();
extern char *ConvertBuf();
extern double GetScriptDouble();
extern void SetScriptDouble();
extern int GetScriptInt();
extern void SetScriptInt();
extern char* GetScriptStr();
extern void SetScriptStr();
extern void CreateScriptFloat();
extern void CreateScriptInt();
extern void CreateScriptString();
extern void CompileScriptVars();

/*
** Obsolete functions from tools library
*/

extern float get_script_float(/* char* name */);
extern void set_script_float(/* char* name, float value */);
extern char* get_glob_val(/* char* name */);


#endif

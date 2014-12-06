/*
** $Id: shell_func_ext.h,v 1.7 2005/07/07 19:03:54 svitak Exp $
** $Log: shell_func_ext.h,v $
** Revision 1.7  2005/07/07 19:03:54  svitak
** Eliminated traces of old TraceScript function so as not to confuse it
** with the newer, less usable TraceScript found in ss/eval.c.
**
** Revision 1.6  2005/06/27 19:00:45  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.5  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.4  2005/06/24 20:48:18  svitak
** Correct typing of functions returning void.
**
** Revision 1.3  2005/06/20 21:20:15  svitak
** Fixed compiler warnings re: unused variables and functions with no return type.
** Default of int causes compiler to complain about return with no type, so void
** used as return type when none present.
**
** Revision 1.2  2005/06/17 17:37:06  svitak
** Deprecated varargs.h replaced by stdarg.h. Old-style argument lists updated.
** These changes were part of varargs-genesis.patch. Systems without stdarg.h
** will no longer compile.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.11  2003/03/28 21:10:46  gen-dbeeman
** Changes from Hugo Cornelis to enable debugging commands gctrace and gftrace
**
** Revision 1.10  2001/06/29 21:24:17  mhucka
** Lots of additions, and a reorganization.
**
** Revision 1.9  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.8  2000/09/21 19:38:20  mhucka
** Added declarations for a few more functions.
**
** Revision 1.7  2000/09/07 05:15:58  mhucka
** Added declaration for Autoshell() and new function SetAutoShell().
**
** Revision 1.6  2000/05/19 18:40:05  mhucka
** Added declaration for SearchForNonScript.
**
** Revision 1.5  2000/03/27 10:33:07  mhucka
** Commented out statusline functionality, because it caused GENESIS to send
** control character sequences upon exit and thereby screw up the user's
** terminal.  Also added return type declarations for various things.
**
** Revision 1.4  1999/08/22 04:42:15  mhucka
** Various fixes, mostly for Red Hat Linux 6.0
**
** Revision 1.3  1995/02/22 19:12:58  dhb
** Added GetHistory() extern statement.
**
** Revision 1.2  1994/04/14  16:22:29  dhb
** Added PFI GetFuncAddress() to extern declarations.
**
** Revision 1.1  1992/12/11  19:04:53  dhb
** Initial revision
**
*/

#ifndef SHELL_FUNC_EXT_H
#define SHELL_FUNC_EXT_H

#include <stdlib.h>
#include <string.h>	/* strchr */
#include "result.h"
#include "shell_struct.h"

/*
** PFI is defined in header.h, but some places where PFI is needed can't
** include header.h without a lot of trouble.  So, define it here if needed.
*/
#ifndef PFI_DEFINED
#define PFI_DEFINED
typedef int	(*PFI)();
#endif

#include <string.h>

/*
** Tracing of commands and functions
*/

int GetCommandTraceLevel(void);
int SetCommandTraceLevel(int iLevel);

extern int      AddCleanup();
extern void     AddFunc();
extern void     AddHistory();
extern int      AddJob();
extern void     AddScript();
extern void     AlternatePrompt();
extern char    *ArgListToString();
extern double   Atod();
extern float    Atof();
extern int      Autoshell();
extern int      AuxFuncIndex();
extern int      AvailableCharacters();
extern void     cleareol();
extern void     ClearScriptStack();
extern void     CloseLog();
extern char   **CopyArgv();
extern char    *CopyString();
extern Script  *CurrentScript();
extern FILE    *CurrentScriptFp();
extern int      CurrentScriptLine();
extern int      CurrentScriptLine();
extern char    *CurrentScriptName();
extern void     DefaultPrompt();
extern void     DisableStatusLine();
extern void     do_add_func();
extern void     do_autoshell();
extern int      do_cd();
extern int      do_debug();
extern int      do_debug_func();
extern void     do_echo();
extern void     do_execute();
extern void     do_exit();
extern char    *do_getenv();
extern void     do_getinfo();
extern void     do_load_func();
extern void     do_load_lib();
extern void     do_logoff();
extern int      do_mkdir();
extern void     do_printargs();
extern void     do_printenv();
extern void     do_return();
extern void     do_setenv();
extern void     do_set_prompt();
extern void     do_shell();
extern int      do_silent();
extern void     do_source();
extern int      do_status_message();
extern void     do_where();
extern int      Dynamic();
extern int      EmptyLine();
extern int      EmptyString();
extern void     EnableDynamicLoad();
extern void     EnableHistory();
extern int      EnableScrollRegion();
extern void     EnableStatusLine();
extern void     EndScript();
extern void     EndScript();
extern void     Error();
extern void     ExecuteCleanups();
extern Result   ExecuteCommand();
extern void     ExecuteJobs();
extern void     FreeArgv();
extern void     FreeArgv();
extern void     FreeString();
extern char    *ftoa();
extern int      FuncIndex();
extern void     genesis_tty();
extern char    *getenv();
extern PFI      GetFuncAddress();
extern char    *GetFuncName();
extern char*    GetHistory();
extern int      GetInfo();
extern int      GetLine();
extern char    *GetLineFp();
extern void     HashFunc();
extern void     HashInit();
extern char    *History();
extern int      HistoryCnt();
extern int      IncludeScript();
extern void     inc_prompt();
extern void     InitJobs();
extern void     Interpreter();
extern int      is_alpha();
extern int      is_alphanum();
extern int      IsBatchMode();
extern int      IsBatchMode();
extern int      IsCommand();
extern int      is_data_char();
extern int      is_eol_char();
extern int      IsFile();
extern int      IsHistoryEnabled();
extern int      IsInclude();
extern int      is_index_delimiter();
extern int      is_num();
extern int      IsSilent();
extern int      is_space_char();
extern int      is_str_delimiter();
extern int      IsStringDelimiter();
extern int      IsStringEnd();
extern int      is_str_var_delimiter();
extern int      IsTtyMode();
extern int      is_var_char();
extern int      IsWhiteSpace();
extern char    *itoa();
extern void     LoadFuncInit();
extern void     LogHeader();
extern void     LogTime();
extern void     LogTrailer();
extern void     lprintf(char *com, ...);
extern void     lprint_only(char *com, ...);
extern int      NestedLevel();
extern Script  *NextScript();
extern Script  *NextScript();
extern void     normal_tty();
extern char   **NULLArgv();
extern void     OpenLog();
extern FILE    *OpenScriptFile();
extern void     ParseInit();
extern void     ProcessStatus();
extern void     PutAuxFunc();
extern float    rangauss();
extern void     ReallocateArgList();
extern void     RecordStartTime();
extern void     RemoveCleanup();
extern void     RemoveJob();
extern void     ResetErrors();
extern void     RestoreCursorPosition();
extern void     restore_tty();
extern void     SaneTerm();
extern void     SaveCursorPosition();
extern int      ScriptArgc();
extern char    *ScriptArgv();
extern int      ScriptDepth();
extern int      ScriptEnded();
extern int      ScrollRegion();
extern FILE    *SearchForExecutable();
extern FILE    *SearchForNonScript();
extern FILE    *SearchForScript();
extern void     SetAutoshell();
extern void     SetBatchMode();
extern void     SetDefaultPrompt();
extern void     SetPrompt();
extern void     SetScript();
extern void     SetSilent(); 
extern void     SetTtyMode();
extern void     SetUpFuncTable();
extern int      SetupInterp();
extern char    *sgets();
extern void     ShowFuncNames();
extern void     ShowHistory();
extern void     show_prompt();
extern void     Sleep();
extern void     Standout();
extern int      StatusActive();
extern void     StringToArgList();
extern char    *StripCR();
extern int      TermGoto();
extern void     terminal_setup();
extern void     TermParse();
extern char    *termstr();
extern void     tset();
extern int      ValidScript();
extern void     Warning();

#endif /* SHELL_FUNC_EXT_H */

static char rcsid[] = "$Id: out_script.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: out_script.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2000/09/11 15:50:55  mhucka
** Several misc. fixes from Hugo Cornelis.
**
** Revision 1.1  1992/12/11 19:03:27  dhb
** Initial revision
**
*/

#include "out_ext.h"

int ScriptOutput(script,action)
struct script_output_type *script;
Action		*action;
{
    int iResult = 1;

    SELECT_ACTION(action){
    case PROCESS:
	/*
	** execute the script
	*/
	if(script->argc > 0){
	    ExecuteFunction(script->argc,script->argv);
	}
	break;
    case SET:
	if(strcmp(action->argv[0],"command") == 0)
	{
	    /*- free old command */

	    if (script->command)
		free(script->command);

	    script->command = CopyString(action->argv[1]);

	    /*
	    ** remember : field set by object
	    */

	    iResult = 1;
	}
	else
	{
	    /*
	    ** remember : field should be set by kernel
	    */

	    iResult = 0;

	    break;
	}
/* 	} else { */
/* 	    printf("field '%s' can not be set by the user\n",action->argv[0]); */
/* 	    return(iResult); */
/* 	} */
    case RESET:
    case RECALC:
	if(script->command){
	    StringToArgList(script->command,&(script->argc),&(script->argv));
	}
	break;
    default:
	printf("'%s' cannot handle action '%s'.\n",
	Pathname(script),
	Actionname(action));
	break;
    }

    return(iResult);
}


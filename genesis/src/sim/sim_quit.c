static char rcsid[] = "$Id: sim_quit.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: sim_quit.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

/** 
    Callback for the quit command - calls stage 1 quit
    which invokes the QUIT action for all elements in element tree 
**/

void quitCallback()
{
	void QuitStage1();

	Element * element = RootElement();
	QuitStage1(element);
}

void QuitStage1(element)
 Element *element;
{

	static Action quit_action = { "QUIT", QUIT, NULL, 0, NULL, NULL };

	if(element->child){
		/*
		** recursively do it for the children
		*/
		QuitStage1(element->child);
	}

    /*
    ** try to call any special quit function nestled away in the QUIT action
    */

    	CallActionsSubToBase(element, element->object, &quit_action);

    	if(element->next){
       	 /*
       	 ** and the siblings
       	 */
       	 QuitStage1(element->next);
    	}

}

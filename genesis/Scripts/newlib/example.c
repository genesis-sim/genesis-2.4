#include "example_ext.h"

#define ADD 		0
#define SUBTRACT 	1
#define TWOARGS 	2
#define NEWACTION 	20

/*
** example of how to define a new object function
*/
/* M.Wilson Caltech 1/89 */
/*
** The user can give the object function any unique name.
** Similarly, the arguments to the function can have arbitrary names.
*/
ExampleObject(element,action)
struct example_type *element;
Action		*action;
{
/* If the element is to receive messages, this pointer (MsgIn *msg) must be
** declared.
*/
MsgIn	*msg;
double	value;

    /*
    ** The debugging level can be assigned at runtime within the
    ** interpreter using the 'debug' command.  The function ActionHeader
    ** will cause GENESIS to print a standard message consisting
    ** of the name of the function called, the name of the element,
    ** and the name of the action being executed.
    */
    if(debug > 1){
	/* just prints out information which helps see what is happening */
	ActionHeader("ExampleObjectt",element,action);
    }

    SELECT_ACTION(action){
    /*
    ** SELECT_ACTION is a macro for a switch-case statement switching on the
    ** action requested. 
    ** There are a number of predefined actions (see sim_defs.h)
    ** which are typically used by elements. PROCESS is one of them
    ** New actions can be added in any element. Use the 'addaction'
    ** command in the object definition script to inform the simulator
    ** of the new action. The case number asssigned to new actions
    ** is relatively arbitrary as long as it does not conflict with
    ** the case numbers of other actions defined in the element.
    ** (you should get a compiler error if there is a conflict).
    */
    case NEWACTION:
	printf("code for the new action\n");
	break;
    case PROCESS:
	element->input = 0;
	/*
	** This is the way in which messages are processed
	** MSGLOOP is a macro which scans all incoming messages and
	** executes the code in the appropriate case statement
	** depending on the message type
	**
	*/
	MSGLOOP(element,msg) {
	    /*
	    ** The case number assigned here must be defined in the
	    ** in the message section of the object definition 
	    ** (see examplelib.g)
	    */
	    case ADD:
		/* 
		** The function MSGVALUE allows you to access the contents
		** of the message arguments passed into the element.
		** The first argument is just the msg pointer,
		** the second argument is the argument number
		** Thus to get the first argument of a message use
		** MSGVALUE(msg,0). To get the second (assuming there are
		** more than one argument in the message) use
		** MSGVALUE(msg,1).
		** Note that the return type from 
		** MSGVALUE is always type double 
		**
		** You are free to place whatever code you would like in here
		*/
		value = MSGVALUE(msg,0);
		element->input += value;
		printf("adding a message value of %f\n",value);
		break;
	    case SUBTRACT:
		value = MSGVALUE(msg,0);
		element->input -= value;
		printf("subtracting a message value of %f\n",value);
		break;
	    case TWOARGS:
		printf("processing arguments %f and %f\n",
			MSGVALUE(msg,0),MSGVALUE(msg,1));
		break;
	    default:
		printf("Unknown message\n");
		break;
	}
	/* 
	** In this case we add the element field 'input' to 'output',
	** using output to maintain a running sum.
	** You are free to place whatever code you would like in here.
	*/ 
	element->output += element->input;
	printf("element has been processed\n");
	break;
    /*
    ** The RESET action is used to return the element to a known
    ** initial state
    */
    case RESET:
	element->input = 0;
	element->output = 0;
	printf("element has been reset\n");
	break;
    }
}

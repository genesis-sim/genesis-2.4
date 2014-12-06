//genesis
//
// Add the function defined in command.c (do_example), give it the name
// 'example', and specify that it returns an integer value.
//
addfunc example do_example int
//
/*
** Now deal with the new object
**
** new class definitions can be added using the 'newclass' command
** Note that classes are simply used for grouping of related elements
** and do not alter the element functionality in any way
** See scheduling (genesis/startup/schedule.g) for an example of class use
*/

newclass	exampleclass

/*
** New actions which are used by elements are defined with the 'addaction'
** command. The action number must be consistent with the case number
** used in the element code (see example.c)
*/
addaction	NEWACTION	20

/*
******************************************************************************
**                          Example OBJECT TABLE                            **
******************************************************************************
*/
//============================================================================
//	name		data_type	function	class		
//============================================================================

object	example_object	example_type	ExampleObject	exampleclass device \
	-author 	"M.Wilson Caltech 2/89" \
	-actions	RESET PROCESS NEWACTION \
	-messages	ADD 0		1 input \
			SUBTRACT 1	1 input \
			TWOARGS	2	2 arg1 arg2 \
	-readwrite	input  "Input variable, altered  by ADD and SUBTRACT" \
	-readonly	output "Running total of input at each step" \
	-description	"exercise in creating new objects" \
			"keeps a running sum of its inputs"

/*
** mandatory arguments to the 'object' command are the
** 1) the object name (must be unique)
** 2) the name of the object data structure (must be the same as that
**    used in the structure definition (see example_struct.h)
** 3) the default object function (must correspond to the function name
**    used in the code definition (see example.c)
** 4) object class. This can be one or more of either existing or newly
**    defined classes.
**
** The actions which the element can perform must be given following the
** -actions argument. The action names must correspond to predefined or
** newly added (the addaction command) actions. The actions listed
** must be consistent with the actions defined in the code definition
** (see the SELECT_ACTION in example.c)
**
** If messages are used by the object then the '-message' option must
** be defined with the following arguments:
** 1) the name of the message can be any string. This is used by the
**    'addmsg' command.
** 2) the case number of the message type must correspond to the
**    value defined in the code definition (see the MSGLOOP of example.c)
** 3) the number of arguments to the message
** 4) the names of the arguments are arbitrary and are use for
**    documenting purposes (there MUST be as many names as there are
**    arguments as defined in 3)
**
** The remaining fields are added for the purpose of documenting the
** object and are optional.
*/


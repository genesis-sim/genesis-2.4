//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: stack.g 1.8 Tue, 21 Feb 2006 16:51:16 +0100 hugo $
//

//////////////////////////////////////////////////////////////////////////////
//'
//' Purkinje tutorial
//'
//' (C) 1998-2002 BBF-UIA
//' (C) 2005-2006 UTHSCSA
//'
//' functional ideas ... Erik De Schutter, erik@tnb.ua.ac.be
//' genesis coding ..... Hugo Cornelis, hugo.cornelis@gmail.com
//'
//' general feedback ... Reinoud Maex, Erik De Schutter, Dave Beeman, Volker Steuber, Dieter Jaeger, James Bower
//'
//////////////////////////////////////////////////////////////////////////////


// stack.g : a simple stack implementation

int include_stack

if ( {include_stack} == 0 )

	include_stack = 1


extern StackCreate
extern StackPop
extern StackPush
extern StackTopElement
extern StackTop


include utility.g


///
/// SH:	StackCreate
///
/// PA:	path..:	path to contain stack elements ending in '/'
///
/// DE:	Create a stack
///

function StackCreate(path)

str path

	//- create stack container

	create neutral {path}stack

	//- add field for number of elements

	addfield {path}stack \
		elements -description "Number of elements -1 in stack"

	//- initialize field

	setfield {path}stack elements -1
end


///
/// SH:	StackDelete
///
/// PA:	path..:	path with stack elements ending in '/'
///
/// DE:	Delete stack
///

function StackDelete(path)

str path

	//- delete stack container

	delete {path}stack
end


///
/// SH:	StackElements
///
/// PA:	path..:	path with stack elements ending in '/'
///
/// DE:	Get elements of stack as a string.
///

function StackElements(path)

str path

	str result = ""

	//- loop over all elements

	str element

	foreach element ( {el {path}/stack/##} )

		//- add element to result

		result = {{result} @ " " @ {element}}
	end

	return {result}
end


///
/// SH:	StackElementValues
///
/// PA:	path..:	path with stack elements ending in '/'
///
/// RE: value of top of stack
///
/// DE:	Get top value of stack
///

function StackElementValues(path)

str path

	//- get stack elements

	str elements = {StackElements {path}}

	//- get values of elements

	str result = ""

	str element

	foreach element ({arglist {elements}})

		result = {{result} @ " " @ {getfield {element} value}}
	end

	//- return value

	return {result}
end


///
/// SH:	StackNumberOfElements
///
/// PA:	path..:	path with stack elements ending in '/'
///
/// RE: number of values in stack
///
/// DE:	Get number of values in stack
///

function StackNumberOfElements(path)

str path

	return {{getfield {path}stack elements} + 1}
end


///
/// SH:	StackPop
///
/// PA:	path..:	path with stack elements ending in '/'
///
/// RE: value of previous top of stack
///
/// DE:	Pop value onto stack
///

function StackPop(path)

str path

	str top
	str val

	//- get number of elements in stack

	int iElements = {StackNumberOfElements {path}}

	//- if no underflow

	if (iElements > 0)

		//- get stack top

		top = {StackTopElement {path}}

		//- get value of stacktop

		val = {getfield {top} value}

		//- delete top

		delete {top}

		//- adjust element count

		setfield {path}stack \
			elements {{getfield {path}stack elements} - 1}

	//- else

	else
		//- set empty result

		val = ""
	end

	//- return value

	return {val}
end


///
/// SH:	StackPush
///
/// PA:	path..:	path with stack elements ending in '/'
///	val...: value to push
///
/// DE:	Push value onto stack
///

function StackPush(path,val)

str path
str val

	//- get number of elements in stack

	int iElements = {StackNumberOfElements {path}}

	//- if no underflow

	str top

	if (iElements > 0)

		//- get stack top

		top = {StackTopElement {path}}
	else

		top = {{path} @ "stack"}
	end

	//- create new element 

	str new = {{top} @ "/1"}

	create neutral {new}

	//- set value

	addfield {new} value

	setfield {new} value {val}

	//- adjust element count

	setfield {path}stack \
		elements {{getfield {path}stack elements} + 1}
end


///
/// SH:	StackTopElement
///
/// PA:	path..:	path with stack elements ending in '/'
///
/// RE: top of stack
///
/// DE:	Get top of stack
///

function StackTopElement(path)

str path

	//- get number of elements in stack

	int iElements = {StackNumberOfElements {path}}

	//- if no underflow

	if (iElements > 0)

		//- get stack top

		str top = {LastArgument {el {path}##}}

		//- return element

		return {top}
	else
		return ""
	end
end


///
/// SH:	StackTop
///
/// PA:	path..:	path with stack elements ending in '/'
///
/// RE: value of top of stack
///
/// DE:	Get top value of stack
///

function StackTop(path)

str path

	//- get number of elements in stack

	int iElements = {StackNumberOfElements {path}}

	//- if no underflow

	if (iElements > 0)

		//- get stack top

		str top = {StackTopElement {path}}

		//- get value of stacktop

		str val = {getfield {top} value}

		//- return value

		return {val}
	else
		return ""
	end
end


end



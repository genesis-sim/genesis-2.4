//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: utility.g 1.13 Thu, 10 Aug 2006 21:08:48 +0200 hugo $
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


// utility.g : various general utility functions


extern CountArguments
extern LastArgument
extern NumberOfElements


int include_utility

if ( {include_utility} == 0 )

	include_utility = 1


///
/// SH:	CountArguments
///
/// PA:	args..:	any number of arguments
///
/// RE:	Number of arguments
///
/// DE:	Determine the number of given arguments
///

function CountArguments

	//- return argument count

	return {argc}
end


///
/// SH:	LastArgument
///
/// PA:	args..:	any number of arguments
///
/// RE:	Last given argument
///
/// DE:	Give last argument
///

function LastArgument

	//- return last argument

	return {argv {argc}}
end


///
/// SH:	NumberOfElements
///
/// PA:	path..:	path with elements ending in '/'
///
/// RE:	Number of elements in the given path
///
/// DE:	Determine the number of elements in the given path
///	The {path} argument can include part of wildcard specification
///

function NumberOfElements(path)

str path

	//- enumerate all elements with a wildcard

	return {CountArguments {el {path}#[]}}
end


///
/// SH:	Reverse
///
/// PA:	args..:	argument list to be reversed.
///
/// RE:	reversed argument list.
///
/// DE:	Reverse the given argument list.
///

function Reverse

	str result = ""

	int i

	for ( i = 1 ; i <= {argc} ; i = i + 1 )

		result = {{argv {i}} @ " " @ {result}}

	end

	return {result}
end


end



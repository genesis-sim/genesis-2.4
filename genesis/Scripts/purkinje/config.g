//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: config.g 1.7.1.2.1.6 Tue, 21 Feb 2006 16:51:16 +0100 hugo $
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


// config.g : configuration file manipulation

int include_config

if ( {include_config} == 0 )

	include_config = 1


///
/// SH:	ConfigFilename
///
/// PA:	cell..:	cellfile that has been read with readcell command
///	path..:	path to cell that has been read
///
/// RE:	Filename for config file
///
/// DE:	Construct a filename for a config file for the given cell and path
///	For the same combination of parameters this routine will allways 
///	give the same result
///

function ConfigFilename(cell,path)

str cell
str path

	//- return cell filename and cat '.config'

	return {cell} @ ".config"
end


///
/// SH:	ConfigHostType
///
/// PA:	cell..:	cellfile that has been read with readcell command
///	path..:	path to cell that has been read
///
/// RE:	string with the type of OS for current config file
///	if there is no valid config file, "nil" is returned.
///
/// DE:	Return the type of OS the current config file was created on
///

function ConfigHostType(cell,path)

str cell
str path

	//- create a filename for the config file

	str filename = {ConfigFilename {cell} {path}}

	//- create an empty config file if it does not exist

	sh "touch "{filename}

	//= read data from config file

	str argline
	str arg1
	str arg2

	//- open the config file

	openfile {filename} r

	//- read header line

	argline = {readfile {filename} -line}

	//- if the argument count is different from 1

	if ( argline == "" || {CountArguments {argline}} != 1)

		//- return nil

		return "nil"
	end

	//- read line with type of OS

	argline = {readfile {filename} -line}

	//- obtain arguments

	arg1 = {getarg {arglist {argline}} -arg 1}
	arg2 = {getarg {arglist {argline}} -arg 2}

	//- if the line does not contain OS information

	if (arg1 != "hostType")

		//- return nil

		return "nil"
	end

	//- return the type of the OS

	return {arg2}
end


///
/// SH:	ConfigWrite
///
/// PA:	cell..:	cellfile that has been read with readcell command
///	path..:	path to cell that has been read
///
/// DE:	Make and write a configuration file.
///

function ConfigWrite(cell,path)

str cell
str path

	//{
	//1 first declare locals for all config settings
	//2 determine the value for all config settings
	//3 write all locals to config file
	//}

	//- create a filename for the config file

	str filename = {ConfigFilename {cell} {path}}

	//- give diagnostics

	echo "Making "{filename}" configuration file for "{cell}

	//{

	//1 first declare locals for all config settings

	//v type of host

	str hostType

	//v number of spines in the cell

	int iSpines = 0

	//v xcell wildcard

	str xCellPath

	//v number of elements in xcell

	int xCellElements = 0

	//2 determine the value for all config settings

	//- determine type of host

	//! since this does not work correctly on all systems,
	//! it is commented out

	//hostType = {getenv HOSTTYPE}

	//! and replaced by a default

	hostType = "default"

	//- loop over all spines

	str spine

	foreach spine ( {el {path}/spine[]} )

		//- increment count

		iSpines = {iSpines + 1}
	end

	//- set xcell wildcard

	xCellPath = {cellpath} @ "/soma[TYPE=compartment]," \
			@ {cellpath} @ "/main[][TYPE=compartment]," \
			@ {cellpath} @ "/b#[][TYPE=compartment]," \
			@ {cellpath} @ "/d#[][TYPE=compartment]," \
			@ {cellpath} @ "/p#[][TYPE=compartment]"

	//- loop over the elements in the xcell

	str xcomp

	foreach xcomp ( {el {xCellPath}} )

		//- increment count

		xCellElements = {xCellElements + 1}

	end

	//3 write all locals to config file

	//- open config file for output

	openfile {filename} w

	//- write the cell file as header

	writefile {filename} "cellfile" {cell}

	//- write the hostType

	writefile {filename} "hostType" {hostType}

	//- write the number of spines

	writefile {filename} "iSpines" {iSpines}

	//- write xcell wildcard

	writefile {filename} "xCellPath" {xCellPath}

	//- write number of elements in xcell

	writefile {filename} "xCellElements" {xCellElements}

	//- close config file

	closefile {filename}

	//}
end


///
/// SH:	ConfigWriteUpdate
///
/// PA:	cell..:	cellfile that has been read with readcell command
///	path..:	path to cell that has been read
///
/// DE:	Update a config file if necessary
///	If the config file corresponds to the cell file, nothing is done.
///

function ConfigWriteUpdate(cell,path)

str cell
str path

	//- create a filename for the config file

	str filename = {ConfigFilename {cell} {path}}

	//- default : we do not create a config file

	int bCreate = 0

	sh "touch "{filename}

	//v read data from config file

	str argline
	str arg1
	str arg2

	//- open the config file

	openfile {filename} r

	//- read first line

	argline = {readfile {filename} -line}

	//echo config : count args = {CountArguments {argline}}

	//- if the argument count is different from 1

	if ( argline == "" || {CountArguments {argline}} != 1)

		//- force a new config file

		bCreate = 1

	//- else check the contents

	else

		//- obtain arguments

		arg1 = {getarg {arglist {argline}} -arg 1}
		arg2 = {getarg {arglist {argline}} -arg 2}

		//- close the config file

		closefile {filename}

		//- if this is not a valid config file

		if (arg1 != "cellfile")

			//- force a new config file

			bCreate = 1
		end

		//- if the file does not match the cell

		if (arg2 != cellfile)

			//- force a new config file

			bCreate = 1
		end
	end

	//- if we should create a config file

	if (bCreate)

		//- make a new config file

		ConfigWrite {cell} {path}
	end
end


///
/// SH:	ConfigRead
///
/// PA:	cell..:	cellfile that has been read with readcell command
///	path..:	path to cell that has been read
///
/// RE:	0 for failure
///	1 for success
///
/// DE:	Read or make the configuration file.
///	If no configuration file is found, one will be made from the current 
///	read cell in {cellpath}.
///	This function can produce a 'getarg' failure when the config file is 
///	not valid.
///

function ConfigRead(cell,path)

str cell
str path

	//- make sure the config file exists and is up to date

	ConfigWriteUpdate {cell} {path}

	//- create a filename for the config file

	str filename = {ConfigFilename {cell} {path}}

	//- give diagnostics

	echo "Using configuration from "{filename}

	//v line argument

	str argline

	//v two arguments

	str arg1
	str arg2

	//{
	//| Read in the configuration
	//|
	//1 first declare locals for all config settings
	//2 read all locals from config file
	//3 set the value for all config settings
	//}

	//{

	//1 first declare locals for all config settings

	//v type of OS

	//! this is of no use anymore, it is always "default"

	str hostType

	//v number of spines

	int iSpines

	//v xcell wildcard

	str xCellPath

	//v number of elements in xcell

	int xCellElements

	//2 read all locals from config file

	//- open the config file

	openfile {filename} r

	//- read first line

	argline = {readfile {filename} -line}

	//- obtain arguments

	arg1 = {getarg {arglist {argline}} -arg 1}
	arg2 = {getarg {arglist {argline}} -arg 2}

	//- if this is not a valid config file

	if (arg1 != "cellfile")

		//- return a failure

		return 0
	end

	//- if the file does not match the cell

	if (arg2 != cellfile)

		//- return a failure

		return 0
	end

	//! from now on we assume a valid config file,
	//! the tags identifying the data are ignored,
	//! so the serial order is the only method for 
	//! identifying the information

	//- read line with type of OS

	argline = {readfile {filename} -line}

	//- set type of OS

	hostType = {getarg {arglist {argline}} -arg 2}

	//- read line with number of spines

	argline = {readfile {filename} -line}

	//- set number of spines

	iSpines = {getarg {arglist {argline}} -arg 2}

	//- read line with xcell wildcard

	argline = {readfile {filename} -line}

	//- set xcell wildcard

	xCellPath = {getarg {arglist {argline}} -arg 2}

	//- read line with number of elements in xcell

	argline = {readfile {filename} -line}

	//- set number of elements in xcell

	xCellElements = {getarg {arglist {argline}} -arg 2}

	//3 set the value for all config settings

	//- create a configuration element

	create neutral /config

	//- add a field for purkinje cell name

	addfield /config \
		cellfile -description "Cell read with readcell"

	//- add a field for hostType

	addfield /config \
		hostType -description "Type of host"

	//- add a field for the number of spines

	addfield /config \
		iSpines -description "Number of attached spines"

	//- add a field for the xcell wildcard

	addfield /config \
		xCellPath -description "Elements displayed by xcell"

	//- add a field for the number of elements in xcell

	addfield /config \
		xCellElements -description "Number of elements in xcell"

	//- set the config values

	setfield /config \
		cellfile {cell} \
		hostType {hostType} \
		iSpines {iSpines} \
		xCellPath {xCellPath} \
		xCellElements {xCellElements}

	//- close the config file

	closefile {filename}

	//}

	//- return success with configuration

	return 1
end


end



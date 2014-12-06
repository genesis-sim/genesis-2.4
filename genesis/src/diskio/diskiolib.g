//genesis

/*********************************************************************

** This library uses the netcdf - version 2.4.3 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license

** See interface/netcdf-2.4.3/COPYRIGHT for the full notice

********************************************************************/


addaction	FLUSH	1001
addaction	UPDATE	10010



/*
*************************************************************************
**                              DISKIO OBJECTS                         **
*************************************************************************
*/
//==========================================================================
//	name		data            function	class	
//==========================================================================


object	diskio	Gen_Diskio_Type 	Gen_Diskio 	output					\
	-author 	"V.Jagadish Caltech 9/96" 						\
	-messages 	SAVE 0		1  data							\
	-actions	CREATE SET RESET PROCESS DELETE UPDATE FLUSH ADDMSGIN DELETEMSGIN SHOW QUIT	\
	-readonly	is_open		"Open state of file" 					\
	-readonly	is_writable	"Writable state of file" 				\
	-readonly	accessmode	"Mode with which file is accessed" 			\
	-hidden		adi		"Hidden handle to the diskio interface struct" 		\
	-hidden		coordi		"Hidden handle to the coordinate variable" 		\
	-hidden 	start_time_index	"Hidden count of time step"			\
	-hidden 	currenttime	"Hidden cache of time value"	\
	-hidden		autoSAVEavi	"Hidden array of handles to the auto generated BC vars" \
	-hidden		autoSAVEamdi	"Hidden array of handles to the auto generated BC 3-D position attribs" \
	-hidden		valsallocated	"flag - indicates whether the vals and autoSAVEavi arrays are allocated" \
	-hidden		already_reset	"flag - indicates whether this has been reset: Additions/Deletions of messages are not allowed after a reset" \
	-fields		"val		:The frame of values retrieved from disk" 		\
			"nx		:Number of frames retrieved - 1 by default"		\
			"ny		:Number of values (size) of a frame"			\
			"xpts, ypts, zpts: Coords of src element points saved on disk"		\
	-description	"Portable binary file reading/writing Interface"

object	variable	Gen_Variable_Type 	Gen_Variable output 				\
	-author 	"V.Jagadish Caltech 9/96" 						\
	-messages	SAVE 0 		1  data 						\
	-actions	CREATE RESET PROCESS DELETE SET SHOW ADDMSGIN UPDATE FLUSH \
	-readonly	output 		"The value of the variable for a particular input" 	\
	-hidden		avi 		"Hidden handle to the variable interface struct"	\
	-hidden		coordi 		"Hidden handle to the coordinate variable" 		\
	-hidden		start_time_index	"Hidden count of time step"			\
	-description	"Portable binary file data manipulator"

object	metadata	Gen_Metadata_Type  	Gen_Metadata 	output				\
	-author 	"V.Jagadish Caltech 9/96" 						\
	-messages 	SAVE 0		1 data							\
	-actions	CREATE RESET PROCESS DELETE SET SHOW ADDMSGIN UPDATE FLUSH \
	-hidden 	amdi 		"Hidden handle to the metadata interface struct " 	\
	-description	"Portable binary file metadata manipulator"

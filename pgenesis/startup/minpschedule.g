// genesis
/*
***************************************************************************
**                            SIMULATION SCHEDULE                        **
***************************************************************************
*/

deletetasks

//=========================================================================
// 		function	arguments
//=========================================================================
addtask	Simulate /##[CLASS=buffer]	-action INIT
addtask	Simulate /##[CLASS=segment]	-action INIT

addtask	Simulate /##[CLASS=postmaster]	-action PROCESS

addtask	Simulate /##[CLASS=buffer]	-action PROCESS
addtask	Simulate /##[CLASS=projection]	-action PROCESS
addtask	Simulate /##[CLASS=segment]	-action PROCESS

resched

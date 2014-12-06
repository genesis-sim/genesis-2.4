// genesis
/*
***************************************************************************
**              MINIMUM SIMULATION SCHEDULE                              **
***************************************************************************
*  This schedule is intended to be used for mingenesis. While it should
*  follow the format of schedule.g, it can only be used for objects included
*  in mingenesis. Please see Doc/Schedules.txt for further information.
*/

deletetasks

//=========================================================================
// 		function	arguments
//=========================================================================
addtask	Simulate /##[CLASS=buffer]	-action INIT
addtask	Simulate /##[CLASS=segment]	-action INIT

addtask	Simulate /##[CLASS=buffer]	-action PROCESS
addtask	Simulate /##[CLASS=projection]	-action PROCESS
addtask	Simulate /##[CLASS=segment] -action PROCESS

resched

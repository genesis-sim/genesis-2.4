// genesis
/*
***************************************************************************
**                            SIMULATION SCHEDULE                        **
***************************************************************************
*/

clearschedule

//=========================================================================
// 		function	arguments
//=========================================================================
addschedule	Simulate /##[CLASS=postmaster]	-action INIT
addschedule	Simulate /##[CLASS=buffer]	-action INIT
addschedule	Simulate /##[CLASS=segment]	-action INIT

addschedule	Simulate /##[CLASS=output]	-action PROCESS
addschedule	Simulate /##[CLASS=buffer]	-action PROCESS
addschedule	Simulate /##[CLASS=projection]	-action PROCESS
addschedule Simulate /##[CLASS=gate] -action PROCESS
addschedule	Simulate /##[CLASS=segment][CLASS!=membrane][CLASS!=gate][CLASS!=concentration] -action PROCESS

addschedule	Simulate /##[CLASS=membrane]	-action PROCESS
addschedule	Simulate /##[CLASS=hsolver]	-action PROCESS
addschedule	Simulate /##[CLASS=concentration]	-action PROCESS

addschedule	Simulate /##[CLASS=device]	-action PROCESS
addschedule	Simulate /##[CLASS=postmaster]	-action PROCESS

reschedule

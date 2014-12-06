// genesis
/* version EDS20i 95/06/02 */
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
addtask	Simulate /##[CLASS=device]	-action INIT

addtask	Simulate /##[CLASS=buffer]	-action PROCESS
addtask	Simulate /##[CLASS=projection]	-action PROCESS
addtask	Simulate /##[CLASS=spiking]	-action PROCESS
addtask Simulate /##[CLASS=gate] -action PROCESS
addtask	Simulate /##[CLASS=segment][CLASS!=membrane][CLASS!=gate][CLASS!=concentration][CLASS!=concbuffer] -action PROCESS

addtask	Simulate /##[CLASS=membrane]	-action PROCESS
addtask	Simulate /##[CLASS=hsolver]	-action PROCESS
addtask	Simulate /##[CLASS=concentration]	-action PROCESS
addtask	Simulate /##[CLASS=concbuffer]	-action PROCESS

addtask	Simulate /##[CLASS=device]	-action PROCESS
addtask	Simulate /##[CLASS=output]	-action PROCESS

resched

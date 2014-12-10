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
addtask	Simulate /##[CLASS=spiking]	-action PROCESS
addtask Simulate /##[CLASS=gate] -action PROCESS
addtask	Simulate /##[CLASS=segment][CLASS!=membrane][CLASS!=gate][CLASS!=concentration] -action PROCESS

addtask	Simulate /##[CLASS=membrane]	-action PROCESS
addtask	Simulate /##[CLASS=hsolver]	-action PROCESS
addtask	Simulate /##[CLASS=concentration]	-action PROCESS

addtask	Simulate /##[CLASS=device]	-action PROCESS
addtask	Simulate /##[CLASS=output]	-action PROCESS

resched
// $Log: schedule.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:07  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.1  1996/05/03 19:11:48  ngoddard
// Initial revision
//

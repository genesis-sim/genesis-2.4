// genesis
/*
**********************************************************************
**                       SIMULATION SCHEDULE                        **
**********************************************************************
*/

deletetasks

//====================================================================
// 		function	arguments
//====================================================================
addtask Simulate /##[CLASS=segment] -action INIT

addtask Simulate /pulsegen -action PROCESS
addtask Simulate /lowpass -action PROCESS
addtask Simulate /Iclamp -action PROCESS
addtask Simulate /Vclamp -action PROCESS
addtask Simulate /PID -action PROCESS

addtask Simulate /axon/##[CLASS=gate] -action PROCESS
addtask Simulate /axon/##[CLASS=segment][CLASS!=membrane][CLASS!=gate]  \
    -action PROCESS
addtask Simulate /##[CLASS=membrane] -action PROCESS

addtask Simulate /##[CLASS=output] -action PROCESS


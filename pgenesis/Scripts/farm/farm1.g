// This illustrates farm mode of operation using barriers and synchronous
// rpc to control master-worker work distribution and termination.
// It iterates <nodes> times.

paron -farm -nodes 3 

int node = {mytotalnode}

// functions and variables existing on both master and worker
include common-decls1

// functions and variables existing on mastor xor worker
if ({node} == 0)
	include master-decls1
else
	include worker-decls1
end

// reset ready for simulation
reset
// rendezvouz
barrierall

// top-level actions for master and workers
if ({node} == 0)
	include master-run1
else
	include worker-run1
end

// notify master we are exiting
echo@0.0 {node} exiting
// rendezvous to ensure master prints message before exiting
barrierall

// cleanup and quit
paroff
quit


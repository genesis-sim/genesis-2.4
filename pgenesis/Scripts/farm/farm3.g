// farm operation with single-node zones
paron -nodes 10 -farm

int node = {myzone}

function runsim(nst)
	int nst
	echo@0.0 {node} : starting runsim
	step {nst}
	echo@0.0 {node} : ending runsim
end

// reset ready to simulate and rendezvous so all have seen "runsim"
reset
barrierall 1

// master asynchronously starts simulation on each node (zone)
if (node == 0)
	async runsim@0.all 2
end

// master waits here until the asynchronous rpcs have completed
// workers wait here until master is ready to pass the barrier
barrierall 2

paroff
quit

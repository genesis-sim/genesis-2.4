// farm operation with single-node zones
paron -nodes 10 -parallel

int node = {mynode}

function runsim(nst)
	int nst
	echo@0 {node} : starting runsim
	step {nst}
	echo@0 {node} : ending runsim
end

// reset and rendezvous so all nodes have seen "runsim" declaration
reset
barrier 1

// master asynchronously starts simulation on each node
if (node == 0)
	async runsim@all 2
end

// master waits here until the asynchronous rpcs have completed
// workers wait here until master is ready to pass the barrier
barrier 3

paroff
quit

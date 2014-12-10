// farm operation with multi-node zones
paron -nodes 10 -mixed 3 3

int node = {mynode}
int zone = {myzone}

// this function runs the simulation on one node
function runsim(nst)
	int nst
	echo@0.0 {mynode} . {myzone} : starting runsim
	step {nst}
	echo@0.0 {mynode} . {myzone} : ending runsim
end

// This function runs the simulation in one zone.
// The zero node calls runsim everywhere then waits at barrier.
// Everybody else just waits at the barrier.
function runzone(nst)
	int nst
	if (node == 0)
		echo@0.0 {zone} : starting runzone
		async runsim@all {nst}
	end
	// zero won't pass until asyncs have completed
	// non-zero's won't pass until zero is ready to
	barrier 1
	// thus everyone has executed "runsim" by now
	if (node == 0)
		echo@0.0 {zone} : ending runzone
	end
end

reset
barrierall 1				// ensure all have seen functions

// The global zero node calls runzone on everybody and continues
if (node == 0 && zone == 0)
	async runzone@all.all 2
end

// Everybody waits at the barrier
barrierall 2				// synchronizes the zones

paroff
quit

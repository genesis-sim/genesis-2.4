// master
int nodes = {ntotalnodes}
int i, done
// iterate until done
for (sum = 1; done == 0; done = done)
    for (i = 0; i <= {nodes}-1; i = i+1)// iterate over nodes
      compute_next_pars {i+1}		// compute paremters for node i
      setpars@0.{i} {pa} {pb} {pc} {pd} // communicate them to node i
    end
    barrierall	1			// 1 parameters are set
    runsim				// run the simulation
    report_match {node+1} {mymatch+1}   // note the matching value
    barrierall  2  			// 2 matches have been reported
    if ({sum} >= ({nodes}*{nodes}))	// compute termination condition
      done = 1
    end
end

// notify workers we're done
for (i = 1; i <= {nodes}-1;i = i+1)
  setflag@0.{i}				// set the flag on worker i
end
// let workers continue
barrierall 1				// 1 flags have been set

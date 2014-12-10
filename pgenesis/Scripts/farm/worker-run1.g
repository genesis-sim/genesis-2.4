// worker
barrierall	1			// 1 wait until parameters set
// iterate until master notifies us we are done
for ( flag = 0; flag == 0; flag = flag)
    runsim				// run the simulation
    report_match@0.0 {node+1} {mymatch+1} // communicate match value to master
    barrierall 2			// 2 match has been reported
    barrierall 1			// 1 flag or new parameters are set
end

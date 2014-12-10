// genesis

// farm.g: a script for farming out tasks to worker nodes

int try_node = 1		// which node to try next

// initialization
function init_farm
	int i

	create neutral /farm
	disable /farm
	create neutral /farm/free
	addfield /farm/free value
	createmap /farm/free / {n_nodes} 1
	setfield /free[0] value 0		// the master is never free
	for (i = 1; i < n_nodes; i = i + 1)
		setfield /free[{i}] value 1
	end
end

// delegate_task finds a free worker and assigns the current task to it;
// the parameters of this function should be changed by the user of this
// package to conform to the task
function delegate_task (index, a, b, c, d)
	int i

	// find a free node
	while (1)
		for (i = 1; i < n_nodes; i = i + 1)
			if ({getfield /free[{try_node}] value})
				// assign this task to that node
				setfield /free[{try_node}] value 0
				async worker_task@0.{try_node} \
					{index} {a} {b} {c} {d}
				try_node = try_node % (n_nodes - 1) + 1
				return
			end
			try_node = try_node % (n_nodes - 1) + 1
		end
		clearthreads	// handle remote commands from other nodes
				//   which will contain incoming results
	end
end
	
// finish waits for the results from all outstanding tasks
function finish
	int i
	int done = 0

	while (!done)
		clearthreads	// handle remote commands from other nodes
				//   which will contain incoming results
		done = 1
		for (i = 1; i < n_nodes; i = i + 1)
			if (!{getfield /free[{i}] value})
				done = 0;
			end
		end
	end
end

// $Log: farm.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:08  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.2  1996/09/20 18:58:54  ghood
// Cleaned up for PSC release
//
// Revision 1.1  1996/08/16 19:04:25  ghood
// Initial revision
//

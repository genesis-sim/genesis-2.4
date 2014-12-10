// genesis

// main orient2 script file

// echo all parameters
echo Genesis started at {getdate}
echo "n_slices = " {n_slices}
echo "display = " {display}
echo "output = " {output}
echo "retina_nx = " {retina_nx}
echo "retina_ny = " {retina_ny}
echo "v1_nx = " {v1_nx}
echo "v1_ny = " {v1_ny}
echo "sim_steps = " {sim_steps}
echo "batch = " {batch}
echo ""

// variables
int n_nodes			// total number of nodes
int output_node			// which node is handling output
int i_am_control_node, i_am_worker_node	// booleans indicating the function
int i_am_output_node, i_am_spare_node	//   assigned to this node
int slice			// what slice this node holds
str workers			// a list of nodes holding slices
int i

// subordinate files
include constants.g
include protodefs.g
include params.g
include control.g
include retina.g
include V1.g
include display.g
include output.g

// START UP
if (display || output)
	n_nodes = n_slices + 2
else
	n_nodes = n_slices + 1
end
output_node = n_slices + 1
workers = "0"
for (i = 1; i < n_slices; i = i + 1)
	workers = workers @ "," @ {i}
end
paron -parallel -silent 0 -nodes {n_nodes} -output o.out \
	-executable pgenesis
setfield /post msg_hang_time 100000
i_am_control_node = {mynode} == n_slices
i_am_worker_node =  {mynode} <  n_slices
i_am_output_node =  (display || output) && ({mynode} == (n_slices + 1))
i_am_spare_node =   !i_am_control_node && !i_am_worker_node && !i_am_output_node
randseed {mynode * 347}		// this avoids artifacts of each node using
				//   the same random number sequence
echo "n_nodes = " {n_nodes}
echo I am node {mynode}
echo Workers are {workers}
echo Completed startup at {getdate}

// CREATE ELEMENTS ON EACH NODE
if (i_am_control_node)
	setup_control
elif (i_am_worker_node)
	slice = {mynode}
	create_retinal_slice
	create_V1_horiz_slice
	create_V1_vert_slice
end
echo Completed element creation at {getdate}

// CONNECT UP ELEMENTS
barrier		// wait for all elements to be created
		//   on every node before trying to
		//   connect them up
echo Connecting up all slices
if (i_am_worker_node)
	connect_retinal_slice
else
	barrier	// we need to do 2 barriers here to correspond
	barrier	// with the 2 barriers that are internal to
		// connect_retinal_slice
end
echo Completed connections at {getdate}

// SET UP NETWORK DISPLAY
if (display)
	if (i_am_output_node)
		create_display
	end
end

// SET UP NETWORK OUTPUT
if (output)
	if (i_am_output_node)
		create_output
	end
end

// SET UP MESSAGES FROM SLICE NODES TO DISPLAY NODE
if (display)
	if (i_am_control_node)
		setup_control_display
	elif (i_am_worker_node)
		setup_retinal_slice_display
		setup_V1_horiz_slice_display
		setup_V1_vert_slice_display
	end
	echo Completed display setup at {getdate}
end

// SET UP MESSAGES FROM SLICE NODES TO OUTPUT NODE
if (output)
	if (i_am_worker_node)
		setup_V1_horiz_slice_output
		setup_V1_vert_slice_output
	end
	echo Completed output setup at {getdate}
end
		
// START SIMULATION
reset       // this does an implicit "barrier" so we are guaranteed to
            //    to have all messages set up before proceeding with
            //    the simulation
if (i_am_control_node)
	if (batch)
		echo Simulation started at {getdate}
		autosweep horizontal
		echo Simulation finished at {getdate}
		barrier 7
	else
		// handle user input from the control panel
		barrier 8 100000
		// now all nodes are at a barrier executing remote threads
		// quit button starts "quit" thread on all nodes
	end
else
	// worker and display nodes
	// wait for commands from the control node
	barrier 7 100000
end

// only arrive here in batch mode
paroff
quit

// $Log: o.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.2  2000/10/11 18:14:32  mhucka
// Updates for 2.2 from Dave Beeman and Greg Hood.
//
// Revision 1.1  1999/12/18 03:02:07  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.6  1997/08/11 04:13:14  ngoddard
// corrected termination for interactive
//
// Revision 1.5  1997/03/02 08:45:02  ngoddard
// clean up termination of batch run
//

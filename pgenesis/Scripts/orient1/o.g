// genesis

// main orient1 script file

// echo all parameters
echo Genesis started at {getdate}
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
int i_am_retina, i_am_V1	// booleans indicating what cells are assigned
				//   to this node
str dir				// directionality of cells on this node;
				//   may be "horiz" or "vert"

// subordinate files
include constants.g
include protodefs.g
include params.g
include retina.g
include V1.g

// START UP
paron -parallel -silent 0 -nodes 3 -output o.out \
	-executable pgenesis
setfield /post msg_hang_time 100000	// set a very long timeout in case
					// we need to do debugging
i_am_retina = {mynode} == 0
i_am_V1 = {mynode} == 1 || {mynode} == 2
if ({mynode} == 1)
	dir = "horiz"
elif ({mynode} == 2)
	dir = "vert"
end
echo I am node {mynode}
echo Completed startup at {getdate}

// CREATE ELEMENTS ON EACH NODE
if (i_am_retina)
	create_retina
	setup_retinal_input
elif (i_am_V1)
	create_V1
end
echo Completed element creation at {getdate}

// CONNECT UP ELEMENTS
barrier		// wait for all elements to be created
		//   on every node before trying to
		//   connect them up
if (i_am_retina)
	connect_retina
end
echo Completed connections at {getdate}

// SET UP NETWORK DISPLAY
if (display)
	if (i_am_retina)
		display_retina
	elif (i_am_V1)
		display_V1
	end
end

// SET UP NETWORK OUTPUT
if (output)
	if (i_am_V1)
		output_V1
	end
end

// START SIMULATION
reset
if (i_am_retina)
	if (batch)
		echo Simulation started at {getdate}
		autosweep horizontal
		echo Simulation finished at {getdate}
		barrier 7
	else
		barrier 8 1000000
		// wait for commands from control panel
	end
else
	// wait for commands from the retinal node's
	//   control panel
	barrier 7 100000
end

paroff
quit

// $Log: o.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:05  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.4  1997/03/02 09:29:19  ngoddard
// cleaned up termination
//
// Revision 1.3  1996/09/20 18:36:56  ghood
// cleaned up for PSC release
//
// Revision 1.2  1996/06/21 16:09:31  ghood
// General debugging
//
// Revision 1.1  1996/05/03 19:11:38  ngoddard
// Initial revision
//

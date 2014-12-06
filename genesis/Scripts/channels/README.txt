Channel juggling tutorial for Neurokit
By Upinder S. Bhalla. March 1991

This tutorial gives an introduction to the channel editing facilities
provided by neurokit, and illustrates the use of the library of prototype
channels.  This tutorial should be run after the user has tried out neurokit
itself.  Be sure that your SIMPATH is set to include neurokit and
neurokit/prototypes.
----------------------------------------------------------------------------

Steps

1 start genesis             type : genesis

2 start neurokit            type : Neurokit

3 load soma                click : file
			   click : load from file

4 select channel editor    click : edit channel
	(Four windows should pop up - 
	 top left - library window = LW
	 top right - compartment window = COW
	 lower left - cell window = CEW
	 lower right - channel window = CHW)

5 Select soma              click : green circle in CEW
	(There is only the soma in this cell, and it is represented
	 by this green circle. The circle turns red and the
	 compartment window acquires the name of the soma. There are
	 no channels on the soma at this stage.)

6 Choose cut/paste mode
  for compartment window   click : select toggle in COW
	(it now says 'Cut-n-Paste', and the MOUSE window on top tells
	 you what the mouse buttons do)

7 Select library channel   click : Na_squid_hh in LW
	(it gets outlined in red)

8 Paste channel to compt   click : compt in COW with left button
    (the Na channel now appears in the COW)

9 Select a K channel       click : K_squid_hh in LW

10 Paste it to compt       click : compt in COW with left button
    (the K channel now appears in the COW)

--------------------------------------------------------------------
You now have a cell with a couple of channels on it. Let us take
a look at the kinetics of these channels.

11 Choose 'select' mode
   for COW                 click : Cut-n-Paste toggle

12 Select Na channel       click : Na_squid_hh in COW
    (It gets outlined in red)

13 Select X gate for display
   in CHW                    set : 'gate' dialog value to X
	(The Na channel activation properties are displayed)

14 Select Y gate for display
   in CHW                    set : 'gate' dialog value to Y
	(The Na channel inactivation properties are displayed)

15 Select overlay mode     click : 'overlay off' toggle in CHW
	(It now says 'overlay on)

16 Select X gate again       set : 'gate' dialog value to X
	(The activation properties are now displayed simulataneously
	 with the previous inactivation curves)

--------------------------------------------------------------------
Let us now try out these channels. 
   
17 Select 'run cell' mode  click : 'run cell' on title bar
	(The familiar cell run environment appears.)

18 Stick a recording electrode
   into the cell           click : soma in cell window, left button.

19 Stick in a current 
   injection electrode     click : Iclamp in SIMULATION CONTROL PANEL
			   click : soma in cell window, left button

20 run cell simulation     click : 'reset' button in SCP
			   click : 'run' button in SCP
   (The simulation chugs along with spikes. You have made a cell!
	You can stop it with the 'stop' button)

--------------------------------------------------------------------
Let us now mess with the channel parameters a little. The most obvious
things to change are the channel densities. 

21 Edit the cell
   parameters             click : 'edit cell' in title bar
    (3 familiar windows pop up, plus the 'CELL PARAMETERS' (CP) window
	 in the lower right.)

22 Select the cell again  click : red circle in CEW (lower left)
	(All sorts of parameters magically appear in the CP window)

23 Set the channel densities to a more
   modest 10 S/m^2          set : Conductance to 10
                          click : K_squid_hh in COW
                            set : Conductance to 10

24 Return to the simulation window
   You will have to reset the injection
   current to run as before
						  click : run cell
						  click : inject
    (This will set the current back to what it was)
						  click : reset
						  click : run
    (Your cell will not spike now. The channel densities
	 are too low. Or are they ... ? I leave this as an exercise
	 to the reader to work out what the lower limits on channel
	 density are. Another exercise would be to play with the 
	 reversal potential to see what it does to the spikes)

--------------------------------------------------------------------
Let us now mess with the channel parameters a lot. We will change
the time constants and voltage dependence of the channels. We could
do this to the hh_channels (like the squid channels) by altering 
their parameters from the script interface. This is an exercise
for the reader. We can also do this to the tabchannels (like the mit
channels) from the interface. So let's replace the squid channels with
mit channels.

25 Set the channel conductances to the mitral tabchannels
   by deleting the existing channels and pasting the new ones
   in from the library.
                          click : edit channel
                          click : 'select' toggle in COW
                          click : Na_squid_hh with middle button in COW
    (the channel should disappear)
                          click : K_squid_hh with middle button in COW
    (the channel should disappear)
                          click : Na_mit_tchan with left button in LW
			  click : soma with left button in COW
                          click : K_mit_tchan with left button in LW
			  click : soma with left button in COW

26 Halve the time constant of the Na channel activation
                          click : 'cut-n-Paste' toggle in COW
			  click : Na_mit_tchan in COW
			  click : 'overlay' toggle in CHW to 
					  'overlay off' state
                          set : 'gate' dialog in CHW to X
		 	  set : 'sy' dialog in CHW to 0.5, hit return
                          click : 'tau' button in CHW, so tau will be scaled
			  set : 'sy' dialog in CHW to 1, hit return
			   (this will prevent a second accidental rescale)

27 Observe the results of your manipulation  
                          click : 'overlay' toggle in COW to the
				  'overlay on' state.
                          click : 'gate' dialog again.

Similary, you could mess with the voltage dependence by scaling or offsetting
the voltage scale with sx and ox.  Or, you may try shifting the activation
curve by using the ox dialog and the 'minf' button.

--------------------------------------------------------------------

You have now been exposed to the main channel editing facilities in
neurokit. These should provide you with the tools to do most of
the tweaking required in simple single cell simulations.

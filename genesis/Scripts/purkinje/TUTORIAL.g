//genesis - Purkinje cell version M9 genesis2 master script
// Copyright: Theoretical Neurobiology, Born-Bunge Foundation - UA, 1998-1999.
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: TUTORIAL.g 1.1.1.3.1.6.3.1.1.1.3.1.1.1.1.1.2.4.1.19 Mon, 21 Aug 2006 23:07:20 +0200 hugo $
//

//////////////////////////////////////////////////////////////////////////////
//'
//' Purkinje tutorial
//'
//' (C) 1998-2002 BBF-UIA
//' (C) 2005-2006 UTHSCSA
//'
//' functional ideas ... Erik De Schutter, erik@tnb.ua.ac.be
//' genesis coding ..... Hugo Cornelis, hugo.cornelis@gmail.com
//'
//' general feedback ... Reinoud Maex, Erik De Schutter, Dave Beeman, Volker Steuber, Dieter Jaeger, James Bower
//'
//////////////////////////////////////////////////////////////////////////////

//- give header

echo "--------------------------------------------------------------------------"
echo
echo "Purkinje tutorial, version " -n
// $Format: "echo \"$ProjectVersion$ ($ProjectDate$)\""$
echo "Release2-2.17 (Wed, 14 Mar 2007 22:13:17 -0500)"
echo "                       Simulation script"
echo
echo "--------------------------------------------------------------------------"


//- let Genesis know where to find the library of prototypes

setenv SIMPATH {getenv SIMPATH} ./library/cells/purkinje_eds1994
setenv SIMPATH {getenv SIMPATH} ./library/cells/purkinje_guineapig
setenv SIMPATH {getenv SIMPATH} ./library/cells/purkinje_rat
setenv SIMPATH {getenv SIMPATH} ./library/cells/purkinje_turtle
setenv SIMPATH {getenv SIMPATH} ./library/segments
setenv SIMPATH {getenv SIMPATH} ./library/channels

//v cell path of cell to simulate

addglobal str cellpath "/Purkinje"

//v set default output rate

addglobal int outputRate 5

//v set default chanmode for solver : normalized

addglobal int iChanMode 5

//v set default mode : in vitro

addglobal int iVVMode 0

//v set default for current : current clamp off

addglobal int iCurrentMode 0

//v in vivo : parallel cell firing rate

addglobal float phertz 25

//v in vivo : basket cell firing rate

addglobal float ihertz 1

//v speed of climbing fiber volley (in sec)

addglobal float climbingDelay 0.00020

//v strength of climbing fiber synapses

addglobal float strength 1.0

//v speed of climbing fiber volley (in steps == delay / dt)

addglobal int delaysteps {{climbingDelay} / 1}

//v default simulation time (msec)

addglobal float runtime 500

//v target element for current injection

addglobal str iClampCurrentTarget "soma"

//v current injection without pulses (nA)

addglobal float iClampCurrentBase 0.5

//v current injection with pulses, first delay (msec)

addglobal float iClampCurrentDelay1 0.0

//v current injection with pulses, first level (nA)

addglobal float iClampCurrentOffset -0.5

//v current injection with pulses, first pulse width (msec)

addglobal float iClampCurrentWidth 100

//v current injection with pulses, second level (nA)

addglobal float iClampCurrentOffset2 2.5

//v current injection with pulses, second pulse delay (msec)

addglobal float iClampCurrentPeriod 200

//v current injection with pulses, second pulse width (msec)

addglobal float iClampCurrentWidth2 50

//v boolean to enable local synchro firing

addglobal int bSynchroLocal 0

//v branch for synchro parallel fiber firing

addglobal str synchroBranch "b3s44"

//v level of synchro activation, distributed

addglobal float synchroDistrLevel 1

//v level of synchro activation, local

addglobal float synchroLocalLevel 10

//v number of synchro synapses to activate

addglobal int synchroSynapses 50

//v basket synaptical strength

addglobal float basketLevel 1


//- cell that is read from pattern file

echo "cellpath is " {cellpath}

include cell.g {cellpath}

//- include the utility module, it is needed by multiple others modules

utility.g

//- include the config module

config.g

// //- default we do not update the config file

// addglobal int bUpdateConfig 0

//- read cell data from .p file

readcell {cellfile} {cellpath}

//- analyze the cell structure

cell_path.g

CellPathInitialize {cellpath}


//- include other files of interest

info.g
bounds.g
config.g
control.g
xcell.g
xgraph.g


//- read configuration file

ConfigRead {cellfile} {cellpath}

//- initialize experiments specific for this cell

CellInit

//- set simulation clocks

int i, j

for (i = 0; i <= 8; i = i + 1)
	setclock {i} {dt}
end

//- set the output clock

setclock 9 {dt * outputRate}

//- set clock for refresh elements

setclock 10 {dt * 239}

//- set delay in steps for climbing fiber

delaysteps = {climbingDelay / dt}

//- setup the hines solver

echo preparing hines solver {getdate}
ce {cellpath}
create hsolve solve

//- We change to current element solve and then set the fields of the parent
//- (solve) to get around a bug in the "." parsing of genesis

ce solve

setfield . \
        path "../##[][TYPE=compartment]" \
        comptmode 1 \
        chanmode {getglobal iChanMode} \
        calcmode 0
// 	storemode 1

//- create all info widgets

InfoCreate

//- create all settings widgets

SettingsCreate

//- setup the solver with all messages from the settings

call /Purkinje/solve SETUP

//- Use method to Crank-Nicolson

setmethod 11

//- go back to simulation element

ce /Purkinje

//- set colorscale

xcolorscale rainbow3

//- create the graph widgets

// str unit
// str outputSource
// str outputValue
// str outputFlags
// str outputDescription
// int iChannelMode

XGraphCreate \
	Vm \
	"" \
	"Vm" \
	1 \
	"Compartmental voltage" \
	{getglobal iChanMode}

XGraphCreate \
	Im \
	"" \
	"Im" \
	1 \
	"Compartmental current" \
	{getglobal iChanMode}

XGraphCreate \
	leak \
	"" \
	"leak" \
	1 \
	"Compartmental leak current" \
	{getglobal iChanMode}

XGraphCreate \
	Ca \
	"Ca_pool" \
	"Ca" \
	6 \
	"Compartmental [Ca2+]" \
	{getglobal iChanMode}

XGraphCreate \
	Ik \
	-1 \
	"Ik" \
	2 \
	"Channel current (Ik)" \
	{getglobal iChanMode}

XGraphCreate \
	Gk \
	-1 \
	"Gk" \
	2 \
	"Channel conductance (Gk)" \
	{getglobal iChanMode}

XGraphCreate \
	Em \
	-1 \
	"Ek" \
	2 \
	"Channel reversal potential" \
	{getglobal iChanMode}

//- create the control panel

ControlPanelCreate

//- create the output menus

output.g

OutputInitialize

sh "mkdir simulation_sequences/Purkinje"

echo "Prepared directory for simulation plots : simulation_sequences/Purkinje"
echo "You can find ascii file recordings overthere."



//- include a GUI configuration script

//! note : cwe is {cellpath}, must not be changed.

create neutral /configuration

include configuration/main.g


//- reset simulation

PurkinjeReset


//genesis - this file is obsolete

/*********************************************************************** 

          LIBRARY.G : library definition file, neuron builder kit. 

          By U.S.Bhalla, Nov 1990

************************************************************************

Some conventions in using the HH_CHANNELS and the VDEP_GATES

HH_CONVENTIONS
==============
Activation state variable is called x for all channels
Inactivation state variable is called y for all channels
In the traditional hh notations: x=m, y=h for Na channel; x=n for K_channel

There are three functional forms for alpha and beta for each state variable:
FORM 1: alpha(v) = A exp((v-V0)/B)                  (EXPONENTIAL)
FORM 2: alpha(v) = A / (exp((v-V0)/B) + 1)          (SIGMOID)
FORM 3: alpha(v) = A (v-V0) / (exp((v-V0)/B) - 1)   (LINOID)
The same functional forms are used for beta.
In the simulator, the FORM, A, B and V0 are designated by:
X_alpha_FORM, X_alpha_A, X_alpha_B, X_alpha_V0  alpha function for state var x
X_beta_FORM,  X_beta_A,  X_beta_B,  X_beta_V0   beta  function for state var x
Y_alpha_FORM, Y_alpha_A, Y_alpha_B, Y_alpha_V0  alpha function for state var y
Y_beta_FORM,  Y_beta_A,  Y_beta_B,  Y_beta_V0   beta  function for state var y

The conductance is calculated as g = Gbar*x^Xpower * y^Ypower
For a squid axon Na channel: Xpower = 3, Ypower = 1 (m^3 h)
                 K  channel: Xpower = 4, Ypower = 0 (n^4)

These are linked to the soma by two messages :
sendmsg /soma/hh_channel /soma CHANNEL Gk Ek
sendmsg /soma /soma/hh_channel VOLTAGE Vm

----------------------------------------------------------------------

For the VDEP Gates, the form of each gate is
alpha = (A+B*V)/(C+exp((V+D)/F))
This is related to the above forms as follows :
EXPONENTIAL :
gate variables		Value of gate variable in terms of hh-channel variables
A			A
B			0
C			0
D			-V0
F			-B

SIGMOID :
Gate			in terms of hh-ch variables
A			A
B			0
C			1
D			-V0
F			B

LINOID :
A			-A * V0
B			A
C			-1
D			-V0
F			B


These are linked to the soma and the channel proper as follows :
sendmsg /soma/channel /soma CHANNEL Gk Ek
sendmsg /soma /soma/channel VOLTAGE Vm

sendmsg /soma/channel/m /soma/channel {gate_type} m {Power}
(eg, senndmsg Na_mitral/m      Na_mitral MULTGATE    m 3_

sendmsg /soma /soma/channel/gate EREST Vm

********************************************************************* ******/

/*
** VARIABLES USED BY ACTIVE COMPONENTS
*/
int EXPONENTIAL = 1
int SIGMOID = 2
int LINOID = 3

/*
** VARIABLES ASSUMED DEFINED BY CELL_PARSER
*/
// Farads/m^2
float CM = 0.01
// Ohms m
float RA = 0.5
// Ohms m^2
float RM = 0.2
// Volts
float EREST_ACT = -0.070

/*********************************************************************** \
    ******/

if (!({exists /library}))
	create neutral /library
end
// We dont want the library to try to calculate anything,
// so we disable it
disable /library

// To ensure that all subsequent elements are made in the library
ce /library

// The only 'permanent' element in the library needs to be the compartment

if (!({exists compartment}))
	create compartment compartment
end
// F
// ohm
// V
// ohm
setfield compartment Cm {CM*1e-9} Ra {RA*1e5} Em {EREST_ACT}  \
    Rm {RM/1e-9} inject 0.0



function setup_table3(gate, table, xdivs, xmin, xmax, A, B, C, D, F)
	str gate, table
	int xdivs
	float xmin, xmax, A, B, C, D, F

	int i
	float x, dx, y

	dx = xdivs
	dx = (xmax - xmin)/dx
	x = xmin

	for (i = 0; i <= (xdivs); i = i + 1)
		y = (A + B*x)/(C + ({exp {(x + D)/F}}))
		setfield {gate} {table}->table[{i}] {y}
		x = x + dx
	end
end

function setup_table2(gate, table, xdivs, xmin, xmax, A, B, C, D, F)
	str gate, table
	int xdivs
	float xmin, xmax, A, B, C, D, F

	if (xdivs <= 9)
		echo must have at least 9, preferably over 100 elements  \
		    in table
		return
	end
	call {gate} TABCREATE {table} {xdivs} {xmin} {xmax}
	setup_table3 {gate} {table} {xdivs} {xmin} {xmax} {A} {B} {C}  \
	    {D} {F}
end

function setup_table(gate, table, xdivs, A, B, C, D, F)
	str gate, table
	int xdivs
	float A, B, C, D, F

	setup_table2 {gate} {table} {xdivs} -0.1 0.1 {A} {B} {C} {D} {F}
end











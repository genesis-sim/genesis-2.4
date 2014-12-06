//genesis

/* FILE INFORMATION
** tabulated implementation of bsg cell voltage and Ca-dependent
** channels.
** Implemented in Neurokit format by : Upinder S. Bhalla.
**
**	Source : Yamada, Koch, and Adams
**	Methods in Neuronal Modeling, MIT press, ed Koch and Segev.
**
** This file depends on functions and constants defined in library.g
*/

// CONSTANTS

float V_OFFSET = 0.0
float VKTAU_OFFSET = 0.0
float VKMINF_OFFSET = 0.02
float ECA = 0.070
float ENa = 0.050
float EK = -0.07

/********************************************************************
**                       Fast Na Current
********************************************************************/
function make_Na_bsg_yka
	if (({exists Na_bsg_yka}))
		return
	end

    create tabchannel Na_bsg_yka
    setfield Na_bsg_yka Ek {ENa} Gbar {1200.0*{SOMA_A}} Ik 0 Gk 0  \
        Xpower 2 Ypower 1 Zpower 0

	setupalpha Na_bsg_yka X {-360e3*(0.033 - V_OFFSET)} -360e3  \
	    -1.0 {0.033 - V_OFFSET} -0.003 {4.0e5*(0.042 - V_OFFSET)}  \
	    4.0e5 -1.0 {0.042 - V_OFFSET} 0.020

	setupalpha Na_bsg_yka Y {1e5*(0.055 - V_OFFSET)} 1.0e5 -1.0  \
	    {0.055 - V_OFFSET} 0.006 4500.0 0.0 1.0 {-V_OFFSET} -0.010
end

/********************************************************************
**                       Fast Ca Current
********************************************************************/

function make_Ca_bsg_yka
	if (({exists Ca_bsg_yka}))
		return
	end

	int ndivs, i
	float x, dx, y

	create vdep_channel Ca_bsg_yka
	setfield Ca_bsg_yka Ek {ECA} gbar {200*{SOMA_A}}

	create tabgate Ca_bsg_yka/mgate
	call Ca_bsg_yka/mgate TABCREATE alpha 100 -0.2 0.1
	call Ca_bsg_yka/mgate TABCREATE beta 100 -0.2 0.1
	x = {getfield Ca_bsg_yka/mgate alpha->xmin}
	dx = {getfield Ca_bsg_yka/mgate alpha->dx}
	ndivs = {getfield Ca_bsg_yka/mgate alpha->xdivs}
	V_OFFSET = -0.065 	// Mit definitions

	for (i = 0; i <= ndivs; i = i + 1)
		if (x < -0.032)
		    setfield Ca_bsg_yka/mgate alpha->table[{i}] 0.0

		     y = ({exp {(x + 0.006 - V_OFFSET)/0.016}} + {exp {-(x + 0.006 - V_OFFSET)/0.016}})/7.8e-3
		     setfield Ca_bsg_yka/mgate beta->table[{i}] {y}
		else
		     y = ({exp {(x + 0.006 - V_OFFSET)/0.016}} + {exp {-(x + 0.006 - V_OFFSET)/0.016}})/(7.8e-3*(1.0 + {exp {-(x - 0.003 - V_OFFSET)/0.008}}))
		     setfield Ca_bsg_yka/mgate alpha->table[{i}] {y}
		     y = ({exp {(x + 0.006 - V_OFFSET)/0.016}} + {exp {-(x + 0.006 - V_OFFSET)/0.016}})/7.8e-3*(1.0 - 1.0/(1.0 + {exp {-(x - 0.003 - V_OFFSET)/0.008}}))
		     setfield Ca_bsg_yka/mgate beta->table[{i}] {y}
		end
		x = x + dx
	end


	create table Ca_bsg_yka/hgate
	call Ca_bsg_yka/hgate TABCREATE 100 0.0 1.0
	x = {getfield Ca_bsg_yka/hgate table->xmin}
	dx = {getfield Ca_bsg_yka/hgate table->dx}
	ndivs = {getfield Ca_bsg_yka/hgate table->xdivs}
	for (i = 0; i <= ndivs; i = i + 1)
		y = 0.01/(0.01 + x)
		setfield Ca_bsg_yka/hgate table->table[{i}] {y}
		x = x + dx
	end

	create table Ca_bsg_yka/nernst
	call Ca_bsg_yka/nernst TABCREATE 1000 0.00005 0.01
	x = {getfield Ca_bsg_yka/nernst table->xmin}
	dx = {getfield Ca_bsg_yka/nernst table->dx}
	ndivs = {getfield Ca_bsg_yka/nernst table->xdivs}
	for (i = 0; i <= ndivs; i = i + 1)
		y = 12.5e-3*{log {4.0/x}}
		setfield Ca_bsg_yka/nernst table->table[{i}] {y}
		x = x + dx
	end

/*
	create Ca_concen Ca_bsg_yka/conc
	set  Ca_bsg_yka/conc \
		tau		0.00001 \					// sec
		B		5.2e-6 \	// Moles per coulomb, later scaled to conc
		Ca_base	1.0e-4						// Moles per cubic m
*/

/* Send messages to and from conc, which is not on Ca_bsg_yka */
	addfield  Ca_bsg_yka addmsg1
	addfield  Ca_bsg_yka addmsg2
	addfield  Ca_bsg_yka addmsg3
	addfield  Ca_bsg_yka addmsg4
	setfield  Ca_bsg_yka  \
	    addmsg1 ".			../Ca_bsg_conc I_Ca Ik"  \
	    addmsg2 "../Ca_bsg_conc	nernst	INPUT Ca"  \
	    addmsg3 "../Ca_bsg_conc	hgate	INPUT Ca"  \
	    addmsg4 ".. mgate VOLTAGE Vm"

/*
	addmsg Ca_bsg_yka Ca_bsg_yka/conc I_Ca Ik
	addmsg Ca_bsg_yka/conc Ca_bsg_yka/nernst INPUT Ca
	addmsg Ca_bsg_yka/conc Ca_bsg_yka/hgate INPUT Ca
*/
	addmsg Ca_bsg_yka/nernst Ca_bsg_yka EK output
	addmsg Ca_bsg_yka/hgate Ca_bsg_yka MULTGATE output 1
	addmsg Ca_bsg_yka/mgate Ca_bsg_yka MULTGATE m 1
end

function make_Ca_bsg_conc
	if (({exists Ca_bsg_conc}))
		return
	end

	create Ca_concen Ca_bsg_conc
	setfield Ca_bsg_conc \
                tau     0.00001 \       // sec
                B       5.2e-6 \ // Moles per coulomb, later scaled to conc
                Ca_base 1.0e-4          // Moles per cubic m
end

/********************************************************************
**            Transient outward K current
********************************************************************/
function make_KA_bsg_yka
	if (({exists KA_bsg_yka}))
		return
	end

    create tabchannel KA_bsg_yka
    setfield KA_bsg_yka Ek {EK} Gbar {1200.0*{SOMA_A}} Ik 0 Gk 0  \
        Xpower 1 Ypower 1 Zpower 0

	setuptau KA_bsg_yka X 1.38e-3 0.0 1.0 -1.0e3 1.0 1.0  \
	    0.0 1.0 {0.042 - V_OFFSET} -0.013

	setuptau KA_bsg_yka Y 0.150 0.0 1.0 -1.0e3 1.0 1.0 0.0  \
	    1.0 {0.110 - V_OFFSET} 0.018
end

/********************************************************************
**            Non-inactivating Muscarinic K current
********************************************************************/
function make_KM_bsg_yka
	if (({exists KM_bsg_yka}))
		return
	end

	int i
	float x, dx, y

    create tabchannel KM_bsg_yka
    setfield KM_bsg_yka Ek {EK} Gbar {1200.0*{SOMA_A}} Ik 0 Gk 0  \
        Xpower 1 Ypower 0 Zpower 0

	call KM_bsg_yka TABCREATE X 49 -0.1 0.1
	x = -0.1
	dx = 0.2/49.0

	for (i = 0; i <= 49; i = i + 1)
		y = 1.0/(3.3*({exp {(x + 0.035 - V_OFFSET)/0.04}}) + {exp {-(x + 0.035 - V_OFFSET)/0.02}})
		setfield KM_bsg_yka X_A->table[{i}] {y}

		y = 1.0/(1.0 + {exp {-(x + 0.035 - V_OFFSET)/0.01}})
		setfield KM_bsg_yka X_B->table[{i}] {y}
		x = x + dx
	end
	tweaktau KM_bsg_yka X
	setfield KM_bsg_yka X_A->calc_mode 0 X_B->calc_mode 0
	call KM_bsg_yka TABFILL X 3000 0
end

/********************************************************************
**                  Delayed rectifying K current
********************************************************************/

function make_K_bsg_yka
	if (({exists K_bsg_yka}))
		return
	end
	int i
	float x, dx, y
	float a, b


    create tabchannel K_bsg_yka
    setfield K_bsg_yka Ek {EK} Gbar 1.17e-6 Ik 0 Gk 0 Xpower 2 Ypower 1  \
        Zpower 0

	call K_bsg_yka TABCREATE X 49 -0.1 0.1
	x = -0.1
	dx = 0.2/49.0

	for (i = 0; i <= 49; i = i + 1)
		a = -4.7e3*(x + 0.012 - VKTAU_OFFSET)/({exp {(x + 0.012 - VKTAU_OFFSET)/-0.012}} - 1.0)
		b = 1.0e3*{exp {-(x + 0.147 - VKTAU_OFFSET)/0.030}}
		setfield K_bsg_yka X_A->table[{i}] {1.0/(a + b)}

		a = -4.7e3*(x + 0.012 - VKMINF_OFFSET)/({exp {(x + 0.012 - VKMINF_OFFSET)/-0.012}} - 1.0)
		b = 1.0e3*{exp {-(x + 0.147 - VKMINF_OFFSET)/0.030}}
		setfield K_bsg_yka X_B->table[{i}] {a/(a + b)}
		x = x + dx
	end
	tweaktau K_bsg_yka X
	setfield K_bsg_yka X_A->calc_mode 0 X_B->calc_mode 0
	call K_bsg_yka TABFILL X 3000 0


	call K_bsg_yka TABCREATE Y 49 -0.1 0.1
	x = -0.1
	dx = 0.2/49.0

	for (i = 0; i <= 49; i = i + 1)
		if (x < -0.025)
			setfield K_bsg_yka Y_A->table[{i}] 6.0
		else
			setfield K_bsg_yka Y_A->table[{i}] 0.050
		end

		y = 1.0 + {exp {(x + 0.025 - V_OFFSET)/0.004}}
		setfield K_bsg_yka Y_B->table[{i}] {1.0/y}
		x = x + dx
	end
	tweaktau K_bsg_yka Y
	setfield K_bsg_yka Y_A->calc_mode 0 Y_B->calc_mode 0
	call K_bsg_yka TABFILL Y 3000 0
end

/********************************************************************/

// genesis

/* Channels used in:
'Exploring parameter space in detailed single neuron models:
simulations of the mitral and granule cells of the olfactory bulb'
Upinder S. Bhalla and James M. Bower
Journal of Neurophysiology Vol 69 No. 6, June 1993 pp 1948-1965
*/

/* FILE INFORMATION
** Some Ca channels for the purkinje cell
** L Channel data from :
**	T. Hirano and S. Hagiwara Pflugers A 413(5) pp463-469, 1989
** 
** T Channel data from :
**     	Kaneda, Wakamori, Ito and Akaike J Neuroph 63(5), pp1046-1051 1990
** 
** Implemented by Eric De Schutter - January 1991
** Converted to NEUROKIT format by Upinder S. Bhalla. Feb 1991
** This file depends on functions and constants defined in defaults.g
*/
// CONSTANTS
float ECa = 0.07	// (I-current)
float ENa = 0.045	// (I-current)
float SOMA_A = 1e-9	// sq m


/* FILE INFORMATION
** Rat Na channel, cloned, in oocyte expression system.
** Data from :
** Stuhmer, Methfessel, Sakmann, Noda an Numa, Eur Biophys J 1987
**	14:131-138
**
** Expts carred out at 16 deg Celsius.
** 
** Implemented in tabchan format by Upinder S. Bhalla March 1991
** This file depends on functions and constants defined in defaults.g
*/

//========================================================================
//                        Adjusted LCa channel
//========================================================================
function make_LCa3_mit_usb
	if ({exists LCa3_mit_usb})
		return
	end

float ECa = 0.07	// (I-current)

    create tabchannel LCa3_mit_usb
    setfield LCa3_mit_usb Ek {ECa} Gbar {1200.0*SOMA_A} Ik 0 Gk 0  \
        Xpower 1 Ypower 1 Zpower 0

	setupalpha LCa3_mit_usb X 7500.0 0.0 1.0 -0.013 -0.007 1650.0 \
	     0.0 1.0 -0.014 0.004

	setupalpha LCa3_mit_usb Y 6.8 0.0 1.0 0.030 0.012 60.0 0.0  \
	    1.0 0.0 -0.011
end



/*********************************************************************
**                          I-Current (Na)
*********************************************************************/

function make_Na_rat_smsnn// Na current

	float ENa = 0.045		// (I-current)
	float x, y, dx
	int i
    if ({exists Na_rat_smsnn})
        return
    end

    create tabchannel Na_rat_smsnn
    setfield Na_rat_smsnn Ek {ENa} Gbar {1200.0*SOMA_A} Ik 0 Gk 0  \
        Xpower 3 Ypower 1 Zpower 0

	call Na_rat_smsnn TABCREATE X 30 -0.1 0.05
    setfield	Na_rat_smsnn \
		X_A->table[0] 	1.0e-4 \	// -0.100 Volts
		X_A->table[1] 	1.0e-4 \	// -0.095 Volts
		X_A->table[2] 	1.2e-4 \	// -0.090 Volts
		X_A->table[3] 	1.45e-4 \	// -0.085 Volts
		X_A->table[4] 	1.67e-4 \	// -0.080 Volts
		X_A->table[5] 	2.03e-4 \	// -0.075 Volts
		X_A->table[6] 	2.47e-4 \	// -0.070 Volts
		X_A->table[7] 	3.20e-4 \	// -0.065 Volts
		X_A->table[8] 	3.63e-4 \	// -0.060 Volts
		X_A->table[9] 	4.94e-4 \	// -0.055 Volts
		X_A->table[10] 	4.07e-4 \	// -0.050 Volts
		X_A->table[11] 	4.00e-4 \	// -0.045 Volts
		X_A->table[12] 	3.56e-4 \	// -0.040 Volts
    	X_A->table[13] 	3.49e-4 \       
    	X_A->table[14]	3.12e-4 \	// -0.030
    	X_A->table[15]	2.83e-4 \
    	X_A->table[16]	2.62e-4 \	// -0.020
    	X_A->table[17]	2.25e-4 \
    	X_A->table[18]	2.03e-4 \	// -0.010
    	X_A->table[19]	1.74e-4 \
    	X_A->table[20]	1.67e-4 \	// 0.0
    	X_A->table[21]	1.31e-4 \
    	X_A->table[22]	1.23e-4 \	// 0.010
    	X_A->table[23]	1.16e-4 \
    	X_A->table[24]	1.02e-4 \	// 0.020
    	X_A->table[25]	0.87e-4 \	     
    	X_A->table[26]	0.73e-4 \	// 0.030
    	X_A->table[27]	0.80e-4 \ 	     
    	X_A->table[28]	0.80e-4 \	// 0.040
    	X_A->table[29]	0.80e-4 \ 	     
    	X_A->table[30]	0.80e-4	// 0.050

	x = -0.1
	dx = 0.15/30.0

	for (i = 0; i <= 30; i = i + 1)
		y = 1.0/(1.0 + {exp {-(x + 0.041)/0.0086}})
		setfield Na_rat_smsnn X_B->table[{i}] {y}
		x = x + dx
	end
	tweaktau Na_rat_smsnn X
	setfield Na_rat_smsnn X_A->calc_mode 0 X_B->calc_mode 0
	call Na_rat_smsnn TABFILL X 3000 0


	call Na_rat_smsnn TABCREATE Y 30 -0.1 0.05
   // settab2const(Na_rat_smsnn,Y_A,0,10,6.4e-3)
	//-0.1 thru -0.05=>0.0

    setfield		Na_rat_smsnn \
		Y_A->table[0] 	0.9e-3 \	// -0.100 Volts
		Y_A->table[1] 	1.0e-3 \	// -0.095 Volts
		Y_A->table[2] 	1.2e-3 \	// -0.090 Volts
		Y_A->table[3] 	1.45e-3 \	// -0.085 Volts
		Y_A->table[4] 	1.7e-3 \	// -0.080 Volts
		Y_A->table[5] 	2.05e-3 \	// -0.075 Volts
		Y_A->table[6] 	2.55e-3 \	// -0.070 Volts
		Y_A->table[7] 	3.2e-3 \	// -0.065 Volts
		Y_A->table[8] 	4.0e-3 \	// -0.060 Volts
		Y_A->table[9] 	5.0e-3 \	// -0.055 Volts
		Y_A->table[10] 	6.49e-3 \	// -0.050 Volts
		Y_A->table[11] 	6.88e-3 \	// -0.045 Volts
		Y_A->table[12] 	4.07e-3 \	// -0.040 Volts
    	Y_A->table[13] 	2.71e-3 \       
    	Y_A->table[14]	2.03e-3 \	// -0.030
    	Y_A->table[15]	1.55e-3 \
    	Y_A->table[16]	1.26e-3 \	// -0.020
    	Y_A->table[17]	1.07e-3 \
    	Y_A->table[18]	0.87e-3 \	// -0.010
    	Y_A->table[19]	0.78e-3 \
    	Y_A->table[20]	0.68e-3 \	// 0.0
    	Y_A->table[21]	0.63e-3 \
    	Y_A->table[22]	0.58e-3 \	// 0.010
    	Y_A->table[23]	0.53e-3 \
    	Y_A->table[24]	0.48e-3 \	// 0.020
    	Y_A->table[25]	0.48e-3 \	     
    	Y_A->table[26]	0.48e-3 \	// 0.030
    	Y_A->table[27]	0.48e-3 \ 	     
    	Y_A->table[28]	0.48e-3 \	// 0.040
    	Y_A->table[29]	0.43e-3 \ 	     
    	Y_A->table[30]	0.39e-3	// 0.050

	x = -0.1
	dx = 0.15/30.0
	for (i = 0; i <= 30; i = i + 1)
		y = 1.0/(1.0 + {exp {(x + 0.064)/0.0102}})
		setfield Na_rat_smsnn Y_B->table[{i}] {y}
		x = x + dx
	end
	tweaktau Na_rat_smsnn Y
	setfield Na_rat_smsnn Y_A->calc_mode 0 Y_B->calc_mode 0
	call Na_rat_smsnn TABFILL Y 3000 0
end

function make_Na2_rat_smsnn
	if (({exists Na2_rat_smsnn}))
		return
	end
	float EK = -0.07
	int i, xdivs, di
	float temp
	float ox = 0.01

	if (({exists Na_rat_smsnn}))
		move Na_rat_smsnn Na2_rat_smsnn
		make_Na_rat_smsnn
	else
		make_Na_rat_smsnn
		move Na_rat_smsnn Na2_rat_smsnn
	end

	/* Upi change 5 Apr 1992 : functionally the same but this
	** works with the hsolver */
	xdivs = {getfield Na2_rat_smsnn X_A->xdivs}
	temp = {getfield Na2_rat_smsnn X_A->invdx}
	temp = ox*temp
	di = temp
	for (i = xdivs; i >= di; i = i - 1)
		temp = {getfield Na2_rat_smsnn X_A->table[{i - di}]}
		setfield Na2_rat_smsnn X_A->table[{i}] {temp}
		temp = {getfield Na2_rat_smsnn X_B->table[{i - di}]}
		setfield Na2_rat_smsnn X_B->table[{i}] {temp}
	end

	xdivs = {getfield Na2_rat_smsnn Y_A->xdivs}
	temp = {getfield Na2_rat_smsnn Y_A->invdx}
	temp = ox*temp
	di = temp
	for (i = xdivs; i >= di; i = i - 1)
		temp = {getfield Na2_rat_smsnn Y_A->table[{i - di}]}
		setfield Na2_rat_smsnn Y_A->table[{i}] {temp}
		temp = {getfield Na2_rat_smsnn Y_B->table[{i - di}]}
		setfield Na2_rat_smsnn Y_B->table[{i}] {temp}
	end

	setfield Na2_rat_smsnn X_A->sy 0.5 X_B->sy 0.5 Y_A->sy 0.5  \
	    Y_B->sy 0.5

	if (0)
	// A change below -- UPI Aug3
	setfield Na2_rat_smsnn X_A->ox 0.01 X_B->ox 0.01 Y_A->ox 0.01  \
	    Y_B->ox 0.01 X_A->sy 0.5 X_B->sy 0.5 Y_A->sy 0.5 Y_B->sy 0.5
	end
end

/********************************************************************
**            Transient outward K current - see yamadachan.g
********************************************************************/

// CONSTANTS

float V_OFFSET = 0.0
float EK = -0.07

function make_KA_bsg_yka
	if (({exists KA_bsg_yka}))
		return
	end

    create tabchannel KA_bsg_yka
    setfield KA_bsg_yka Ek {EK} Gbar {1200.0*SOMA_A} Ik 0 Gk 0 Xpower 1  \
        Ypower 1 Zpower 0

	setuptau KA_bsg_yka X 1.38e-3 0.0 1.0 -1.0e3 1.0 1.0  \
	    0.0 1.0 {0.042 - V_OFFSET} -0.013

	setuptau KA_bsg_yka Y 0.150 0.0 1.0 -1.0e3 1.0 1.0 0.0  \
	    1.0 {0.110 - V_OFFSET} 0.018
end

/********************************************************************
**
**      Non-inactivating Muscarinic K current 
**		Modified from yamadachan.g by Upinder S. Bhalla - the
**			minf curve has been steepened.
**	
********************************************************************/
float VKTAU_OFFSET = 0.00
float VKMINF_OFFSET = 0.00

function make_KM_bsg_upi
	if (({exists KM_bsg_upi}))
		return
	end

	int i
	float x, dx, y, b

    create tabchannel KM_bsg_upi
    setfield KM_bsg_upi Ek {EK} Gbar {1200.0*SOMA_A} Ik 0 Gk 0 Xpower 1  \
        Ypower 0 Zpower 0

	/*
	** Replaced for hsolve compatibility on 5 Apr 1992
	**
	call KM_bsg_upi TABCREATE X 49 -0.1 0.1
	x = -0.1
	dx = 0.2/49.0
	*/
	call KM_bsg_upi TABCREATE X 49 -0.1 0.05
	x = -0.1
	dx = 0.15/49.0

	// UPI Aug3
	for (i = 0; i <= 49; i = i + 1)
		y = 1.0/(3.3*({exp {(x + 0.035 - VKTAU_OFFSET)/0.04}}) + {exp {-(x + 0.035 - VKTAU_OFFSET)/0.02}})
		setfield KM_bsg_upi X_A->table[{i}] {y}

		y = 1.0/(1.0 + {exp {-(x + 0.035 - VKMINF_OFFSET)/0.005}})
		setfield KM_bsg_upi X_B->table[{i}] {y}
		x = x + dx
	end

	tweaktau KM_bsg_upi X
	setfield KM_bsg_upi X_A->calc_mode 0 X_B->calc_mode 0
	call KM_bsg_upi TABFILL X 3000 0
end

/**********************************************************************
**                      Mitral K current
**  Heavily adapted from :
**	K current activation from Thompson, J. Physiol 265, 465 (1977)
**	(Tritonia (LPl	2 and LPl 3 cells)
** Inactivation from RW Aldrich, PA Getting, and SH Thompson, 
** J. Physiol, 291, 507 (1979)
**
**********************************************************************/
function make_K_mit_usb// K-current     

    if ({exists K_mit_usb})
        return
    end
	float EK = -0.07

    create tabchannel K_mit_usb
    setfield K_mit_usb Ek {EK} Gbar {1200*SOMA_A} Ik 0 Gk 0 Xpower 2  \
        Ypower 1 Zpower 0

    call K_mit_usb TABCREATE X 30 -0.100 0.050
    settab2const K_mit_usb X_A 0 12 0.0    //-0.1 thru -0.045=>0.0

    setfield K_mit_usb X_A->table[13] 0.00 \
    	X_A->table[14]	2.87 \	// -0.030
    	X_A->table[15]	4.68 \
    	X_A->table[16]	7.46 \	// -0.020
    	X_A->table[17]	10.07 \
    	X_A->table[18]	14.27 \	// -0.010
    	X_A->table[19]	17.87 \
    	X_A->table[20]	22.9 \	// 0.0
    	X_A->table[21]	33.6 \
    	X_A->table[22]	49.3 \	// 0.010
    	X_A->table[23]	65.6 \
    	X_A->table[24]	82.0 \	// 0.020
    	X_A->table[25]	110.0 \	     
    	X_A->table[26]	147.1 \	// 0.030
    	X_A->table[27]	147.1 \ 	     
    	X_A->table[28]	147.1 \	// 0.040
    	X_A->table[29]	147.1 \ 	     
    	X_A->table[30]	147.1	// 0.050


    setfield K_mit_usb \
		X_B->table[0]	 	36.0	\	// -0.100 Volts
		X_B->table[1]	 	34.4	\	// -0.095 Volts
		X_B->table[2]	 	32.8	\	// -0.090 Volts
		X_B->table[3]	 	31.2	\	// -0.085 Volts
		X_B->table[4]	 	29.6	\	// -0.080 Volts
		X_B->table[5]	 	28.0	\	// -0.075 Volts
		X_B->table[6]	 	26.3	\	// -0.070 Volts
		X_B->table[7]	 	24.7	\	// -0.065 Volts
		X_B->table[8]	 	23.1	\	// -0.060 Volts
		X_B->table[9]	 	21.5	\	// -0.055 Volts
		X_B->table[10]	 	19.9	\	// -0.050 Volts
		X_B->table[11]	 	18.3	\	// -0.045 Volts
		X_B->table[12]	 	16.6	\	// -0.040 Volts
    	X_B->table[13] 		15.4	\
    	X_B->table[14]		13.5	\	// -0.030
    	X_B->table[15]		13.2	\
    	X_B->table[16]		11.9	\	// -0.020
    	X_B->table[17]		11.5	\
    	X_B->table[18]		10.75	\	// -0.010
    	X_B->table[19]		9.30	\
    	X_B->table[20]		8.30	\	// 0.00
    	X_B->table[21]		6.00	\
    	X_B->table[22]		5.10	\	// 0.010
    	X_B->table[23]		4.80	\
    	X_B->table[24]		3.20	\	// 0.020
    	X_B->table[25]		1.60	\
    	X_B->table[26]		0.00	\	// 0.030
    	X_B->table[27]		0.00	\
    	X_B->table[28]		0.00	\	// 0.040
    	X_B->table[29]		0.00	\
    	X_B->table[30]		0.00		// 0.050


		/* Setting the calc_mode to NO_INTERP for speed */
		setfield K_mit_usb X_A->calc_mode 0 X_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweakalpha K_mit_usb X

		/* Filling the tables using B-SPLINE interpolation */
		call K_mit_usb TABFILL X 3000 0


    call K_mit_usb TABCREATE Y 30 -0.100 0.050
    settab2const K_mit_usb Y_A 0 11 1.0    //-0.1 thru -0.035 => 1.0
    setfield		K_mit_usb \
		Y_A->table[12]	1.00	\	// -0.040	Volts
		Y_A->table[13]	0.97	\	// 
		Y_A->table[14]	0.94	\	// -0.030	Volts
    	Y_A->table[15]	0.88	\
    	Y_A->table[16]	0.75	\	// -0.020
    	Y_A->table[17]	0.61	\
    	Y_A->table[18]	0.43	\	// -0.010
    	Y_A->table[19]	0.305	\ 
    	Y_A->table[20]	0.220	\ 	// 0.00
    	Y_A->table[21]	0.175	\
    	Y_A->table[22]	0.155	\	// 0.010
    	Y_A->table[23]	0.143	\
    	Y_A->table[24]	0.138	\	// 0.020
    	Y_A->table[25]	0.137	\ 	     
    	Y_A->table[26]	0.136	\	// 0.030
    	Y_A->table[27]	0.135	\ 	     
    	Y_A->table[28]	0.135	\	// 0.040
    	Y_A->table[29]	0.135	\ 	     
    	Y_A->table[30]	0.135		// 0.050

    settab2const K_mit_usb Y_B 0 11 0.0    //-0.1 thru -0.045 => 0.0
    setfield		K_mit_usb	\
		Y_B->table[12]	0.0 	\	// -0.040	Volts
		Y_B->table[13]	0.03	\	//
		Y_B->table[14]	0.06	\	// -0.030	Volts
    	Y_B->table[15]	0.12	\ 
    	Y_B->table[16]	0.25	\ 	// -0.020
    	Y_B->table[17]	0.39	\ 
    	Y_B->table[18]	0.57	\ 	// -0.010
    	Y_B->table[19]	0.695	\ 
    	Y_B->table[20]	0.78	\ 	// 0.00
    	Y_B->table[21]	0.825	\ 
    	Y_B->table[22]	0.845	\	// 0.010
    	Y_B->table[23]	0.857	\
    	Y_B->table[24]	0.862	\	// 0.020
    	Y_B->table[25]	0.863	\      
    	Y_B->table[26]	0.864	\	// 0.030
    	Y_B->table[27]	0.865	\
    	Y_B->table[28]	0.865	\	// 0.040
    	Y_B->table[29]	0.865	\      
    	Y_B->table[30]	0.865		// 0.050

		/* Setting the calc_mode to NO_INTERP for speed */
		setfield K_mit_usb Y_A->calc_mode 0 Y_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweakalpha K_mit_usb Y

		/* Filling the tables using B-SPLINE interpolation */
		call K_mit_usb TABFILL Y 3000 0

		setfield K_mit_usb X_A->sy 5.0 X_B->sy 5.0 Y_A->sy 5.0  \
		    Y_B->sy 5.0 Ek {EK}

end

function make_K2_mit_usb
	if (({exists K2_mit_usb}))
		return
	end
float EK = -0.07

	if (({exists K_mit_usb}))
		move K_mit_usb K2_mit_usb
		make_K_mit_usb
	else
		make_K_mit_usb
		move K_mit_usb K2_mit_usb
	end

	setfield K2_mit_usb X_A->sy 20.0 X_B->sy 20.0 Y_A->sy 20.0  \
	    Y_B->sy 20.0 Ek {EK}
end

function make_K_slow_usb
	if (({exists K_slow_usb}))
		return
	end
	float EK = -0.07

	if (({exists K_mit_usb}))
		move K_mit_usb K_slow_usb
		make_K_mit_usb
	else
		make_K_mit_usb
		move K_mit_usb K_slow_usb
	end
	setfield K_slow_usb X_A->sy 1.0 X_B->sy 1.0 Y_A->sy 1.0  \
	    Y_B->sy 1.0
end

//========================================================================
//			Tabchan Na Mitral cell channel 
//========================================================================

function make_Na_mit_usb
	if (({exists Na_mit_usb}))
		return
	end

	float THRESH = -0.055	/* offset both for erest and for thresh */
	float ENA = 0.045	/* Sodium reversal potl */

	create	tabchannel	Na_mit_usb
		setfield		^	\
		Ek		{ENA}	\		//	V
		Gbar		{ 1.2e3 * SOMA_A }	\	//	S
		Ik		0	\			//	A
		Gk		0	\			//	S
		Xpower	3	\
		Ypower	1	\
		Zpower	0


	setupalpha Na_mit_usb X {320e3*(0.013 + THRESH)} -320e3 -1.0  \
	    {-1.0*(0.013 + THRESH)} -0.004 {-280e3*(0.040 + THRESH)}  \
	    280e3 -1.0 {-1.0*(0.040 + THRESH)} 5.0e-3

	setupalpha Na_mit_usb Y 128.0 0.0 0.0 {-1.0*(0.017 + THRESH)} \
	     0.018 4.0e3 0.0 1.0 {-1.0*(0.040 + THRESH)} -5.0e-3
end

//========================================================================

function make_Na2_mit_usb
	if (({exists Na2_mit_usb}))
		return
	end
	float THRESH = -0.060	/* offset both for erest and for thresh */
	float ENA = 0.045	/* Sodium reversal potl */

	create tabchannel Na2_mit_usb
		setfield		^	\
		Ek		{ENA}	\		//	V
		Gbar		{ 1.2e3 * SOMA_A }	\	//	S
		Ik		0	\			//	A
		Gk		0	\			//	S
		Xpower	3	\
		Ypower	1	\
		Zpower	0

	setupalpha Na2_mit_usb X {320e3*(0.013 + THRESH)} -320e3 -1.0 \
	     {-1.0*(0.013 + THRESH)} -0.004 {-280e3*(0.040 + THRESH)}  \
	    280e3 -1.0 {-1.0*(0.040 + THRESH)} 5.0e-3

	setupalpha Na2_mit_usb Y 128.0 0.0 0.0  \
	    {-1.0*(0.017 + THRESH)} 0.018 4.0e3 0.0 1.0  \
	    {-1.0*(0.040 + THRESH)} -5.0e-3
end

//========================================================================
// CONSTANTS
float EGlu = 0.045
float EGABA_1 = -0.080
float EGABA_2 = -0.080
float SOMA_A = 1e-9
float GGlu = SOMA_A*50
float GGABA_1 = SOMA_A*50
float GGABA_2 = SOMA_A*50

//===================================================================
//                     SYNAPTIC CHANNELS   (Values guessed at)
//===================================================================


function make_glu_mit_usb
	if (({exists glu_mit_usb}))
		return
	end

	// for receptor input only
	create channelC2 glu_mit_usb
    	setfield	        glu_mit_usb \
		Ek			{EGlu} \
		tau1		{ 4.0e-3 } \	// sec
		tau2		{ 4.0e-3 } \ 	// sec
		gmax		{GGlu} // Siemens
end

function make_GABA_1_mit_usb
	if (({exists GABA_1_mit_usb}))
		return
	end

	create ddsyn GABA_1_mit_usb	// for both dd and ax inputs
	call GABA_1_mit_usb TABCREATE 10 -0.065 0.05
   	setfield	        GABA_1_mit_usb \
		Ek			{ EGABA_1 } \
		tau1		{ 10.0e-3 } \	// sec
		tau2		{ 10.0e-3 } \	// sec
		gmax		{GGABA_1} \		// Siemens
        transf->table[0] 0 \		// Setting up the table for 
        transf->table[1] 0.02 \		// transforming from presyn Vm to
        transf->table[2] 0.05 \		// activation.
        transf->table[3] 0.1 \
        transf->table[4] 0.2 \
        transf->table[5] 0.5 \
        transf->table[6] 0.8 \
        transf->table[7] 0.9 \
        transf->table[8] 0.95 \
        transf->table[9] 0.98 \
        transf->table[10] 1
    call GABA_1_mit_usb TABFILL 1000 0
end

function make_GABA_2_mit_usb
	if (({exists GABA_2_mit_usb}))
		return
	end

	create ddsyn GABA_2_mit_usb	// for both dd and ax inputs
	call GABA_2_mit_usb TABCREATE 10 -0.065 0.05
   	setfield	        GABA_2_mit_usb \
		Ek			{ EGABA_2 } \
		tau1		{ 10.0e-3 } \	// sec
		tau2		{ 10.0e-3 } \	// sec
		gmax		{GGABA_2} \		// Siemens
        transf->table[0] 0 \		// Setting up the table for 
        transf->table[1] 0.05 \		// transforming from presyn Vm to
        transf->table[2] 0.1 \		// activation.
        transf->table[3] 0.2 \
        transf->table[4] 0.5 \
        transf->table[5] 0.7 \
        transf->table[6] 0.8 \
        transf->table[7] 0.9 \
        transf->table[8] 0.95 \
        transf->table[9] 0.98 \
        transf->table[10] 1
    call GABA_2_mit_usb TABFILL 1000 0
end

function make_glu_gran_usb
	if (({exists glu_gran_usb}))
		return
	end

	create ddsyn glu_gran_usb	// for dd, ax, and centrif inputs
	call glu_gran_usb TABCREATE 10 -0.070 0.05

    	// sec
    	// sec
    	// Siemens
    	// Setting up the table for 
    	// transforming from presyn Vm to
    	// activation.
    	setfield glu_gran_usb Ek {EGlu} tau1 {4.0e-3} tau2 {6.0e-3}  \
    	    gmax {GGlu} transf->table[0] 0 transf->table[1] 0.05  \
    	    transf->table[2] 0.1 transf->table[3] 0.2  \
    	    transf->table[4] 0.5 transf->table[5] 0.7  \
    	    transf->table[6] 0.8 transf->table[7] 0.9  \
    	    transf->table[8] 0.95 transf->table[9] 0.98  \
    	    transf->table[10] 1
	// for dd, ax, and centrif inputs
    	setfield	        glu_gran_usb \
		Ek			{EGlu} \
		tau1		{ 4.0e-3 } \	// sec
		tau2		{ 6.0e-3 } \ 	// sec
		gmax		{GGlu} \ 		// Siemens
        transf->table[0] 0 \		// Setting up the table for 
        transf->table[1] 0.05 \		// transforming from presyn Vm to
        transf->table[2] 0.1 \		// activation.
        transf->table[3] 0.2 \
        transf->table[4] 0.5 \
        transf->table[5] 0.7 \
        transf->table[6] 0.8 \
        transf->table[7] 0.9 \
        transf->table[8] 0.95 \
        transf->table[9] 0.98 \
        transf->table[10] 1
    call glu_gran_usb TABFILL 1000 0
end

function make_glu_pg_usb
	if (({exists glu_pg_usb}))
		return
	end

	create ddsyn glu_pg_usb	// for dd, ax, and centrif inputs
	call glu_pg_usb TABCREATE 10 -0.070 0.05
    	setfield	        glu_pg_usb \
		Ek			{EGlu} \
		tau1		{ 4.0e-3 } \	// sec
		tau2		{ 6.0e-3 } \ 	// sec
		gmax		{GGlu} \ 		// Siemens
        transf->table[0] 0 \		// Setting up the table for 
        transf->table[1] 0.02 \		// transforming from presyn Vm to
        transf->table[2] 0.05 \		// activation.
        transf->table[3] 0.1 \
        transf->table[4] 0.2 \
        transf->table[5] 0.5 \
        transf->table[6] 0.8 \
        transf->table[7] 0.9 \
        transf->table[8] 0.95 \
        transf->table[9] 0.98 \
        transf->table[10] 1
    call glu_pg_usb TABFILL 1000 0
end

function make_olf_receptor
	if (({exists olf_receptor}))
		return
	end
float ENa = 0.045	// Volts

	create receptor2	olf_receptor
		setfield ^ \
			Ek	{ENa} \
			tau1	0.05	\	//sec
			tau2	0.1		\	//sec
			gmax	5e-8	\	//Siemens
			modulation	1		// unitless
end

function make_spike
	if (({exists spike}))
		return
	end

	create spikegen spike
	setfield spike \
		thresh	-0.00 \		// V
		abs_refract	10e-3 \	// sec
		output_amp	1		
end

//========================================================================

function make_Kca_mit_usb
	if (({exists Kca_mit_usb}))
		return
	end
	float EK = -0.08

	create vdep_channel Kca_mit_usb
		setfield		^	\
		Ek		{EK}	\			//	V
		gbar		{ 360.0 * SOMA_A }	\	//	S
		Ik		0	\			//	A
		Gk		0				//	S

	create table Kca_mit_usb/qv
	call Kca_mit_usb/qv TABCREATE 100 -0.1 0.1
	int i
	float x, dx, y
	x = -0.1
	dx = 0.2/100.0
	for (i = 0; i <= 100; i = i + 1)
		y = {exp {(x - {EREST_ACT})/0.027}}
		setfield Kca_mit_usb/qv table->table[{i}] {y}
		x = x + dx
	end

	create tabgate Kca_mit_usb/qca

	setupgate Kca_mit_usb/qca alpha  {5.0e5*0.015}  \
	    -5.0e5 -1.0 -0.015.0 -0.0013 -size 1000 -range 0.0 0.01

	call Kca_mit_usb/qca TABCREATE beta 1 -1 100
	setfield Kca_mit_usb/qca beta->table[0] 50
	setfield Kca_mit_usb/qca beta->table[1] 50

	addmsg Kca_mit_usb/qv Kca_mit_usb/qca PRD_ALPHA output
	addmsg Kca_mit_usb/qca Kca_mit_usb MULTGATE m 1
	addfield  Kca_mit_usb addmsg1
	addfield  Kca_mit_usb addmsg2
	setfield  Kca_mit_usb  \
	    addmsg1 "../Ca_mit_conc	qca	VOLTAGE		Ca" \
	    addmsg2 "..			qv	INPUT		Vm"
end

//========================================================================
//			Ca conc - mitral cell
//========================================================================

function make_Ca_mit_conc
	if (({exists Ca_mit_conc}))
		return
	end
	create Ca_concen Ca_mit_conc
	setfield Ca_mit_conc \
		tau		0.01	\			// sec
		B		5.2e-6	\	// Curr to conc
		Ca_base		0.00001
        addfield  Ca_mit_conc addmsg1
	setfield  Ca_mit_conc  \
	    addmsg1 "../LCa3_mit_usb	.		I_Ca	Ik"
end

// genesis

/* FILE INFORMATION - last update: Aug 17, 1993
** Molluscan Voltage dependent Na, K, KA, Ca, slow Na/Ca channels
** The channels given below are representative of those found in
** molluscan soma described in the review paper by
** David J. Adams, Stephen J. Smith and Stuart H. Thompson
** (Ann. Rev. Neurosci. 3, p.141 (1980)) and in other references below.
** Implemented by David Beeman - January 1991
** Converted to tabchans by Upinder S. Bhalla - Feb 1991
** Further additions and corrections by D. Beeman - Oct 92, Jan 93, Aug 93
** Converted to GENESIS 2 - March 1995
** All parameters are in SI (MKSA) units.
** The nominal resting membrane potential is -0.040 V.
**
**
** Typical equilibrium potentials taken from these references are:
**        ENa             =        0.050
**        EK              =       -0.060
**        EA              =       -0.063
**        ECa             =        0.064
**        EB              =        0.068  // +/- 0.059 V uncertainty!
**
** This file depends on functions and constants defined in defaults.g
*/

// CONSTANTS
float        ENa             =        0.050
float        EK              =       -0.060
float        EA              =       -0.063
float        ECa             =        0.064
float        EB              =        0.068  // +/- 0.059 V uncertainty!
float		 SOMA_A	     =        2e-7   // sq m


/***********************************************************************
                            Na-Current
 Adams and Gage, J. Physiol. 289, p. 143 (1979)  (Aplysia R15)

***********************************************************************/
/* The activation curve (X gate) has been shifted down by -5 mV to make it
   more like the Connor and Stevens I-current and provide more robust firing
   when used with the Late potassium K-Current in this file.  In order to
   restore it to the curve measured by Adams and Gage, add the command
   "scale_tabchan Na_aplysia_ag X minf 1.0 1.0 0.005 0.0" to the end of this
   function.
*/

function make_Na_aplysia_ag	// Na-current
    str chanpath = "Na_aplysia_ag"
    if ({exists {chanpath}})
        return
    end

    create  tabchannel  {chanpath}
    setfield  {chanpath}  \
         Ek     {ENa}             \
         Gbar   {137.5 * SOMA_A} \
         Ik     0                 \
         Gk     0					\
		 Xpower	3	\
		 Ypower	1	\
		 Zpower	0

    call    {chanpath}    TABCREATE X 30 -0.100 0.050
	setfield		{chanpath}	\    //  A = tau_m, B = m_inf
		X_A->table[0]	0.0122	X_B->table[0]	0.0   	\  // -0.1
		X_A->table[1]	0.0122	X_B->table[1]	0.0   	\
		X_A->table[2]	0.0122	X_B->table[2]	0.0   	\  // -0.09
		X_A->table[3]	0.0122	X_B->table[3]	0.0   	\
		X_A->table[4]	0.0122	X_B->table[4]	0.0   	\  // -0.08
		X_A->table[5]	0.0122	X_B->table[5]	0.0   	\
		X_A->table[6]	0.0122	X_B->table[6]	0.0   	\  // -0.07
		X_A->table[7]	0.0122	X_B->table[7]	0.0   	\
		X_A->table[8]	0.0122	X_B->table[8]	0.0   	\  // -0.06
		X_A->table[9]	0.0122	X_B->table[9]	0.0   	\
		X_A->table[10]	0.0122	X_B->table[10]	0.0   	\  // -0.05
		X_A->table[11]	0.0122	X_B->table[11]	0.0   	\
		X_A->table[12]	0.0122	X_B->table[12]	0.0   	\  // -0.04
		X_A->table[13]	0.0122	X_B->table[13]	0.0494	\
		X_A->table[14]	0.0116	X_B->table[14]	0.2414	\  // -0.030
		X_A->table[15]	0.00938	X_B->table[15]	0.4077 	\
		X_A->table[16]	0.00762	X_B->table[16]	0.5817 	\  // -0.020
		X_A->table[17]	0.00586	X_B->table[17]	0.7768 	\
		X_A->table[18]	0.00376	X_B->table[18]	0.9069 	\  // -0.010
		X_A->table[19]	0.00228	X_B->table[19]	0.9626 	\
		X_A->table[20]	0.00158	X_B->table[20]	0.9843 	\  // 0.00
		X_A->table[21]	0.00122	X_B->table[21]	0.9940 	\
		X_A->table[22]	0.00097	X_B->table[22]	0.9992 	\  // 0.010
		X_A->table[23]	0.00078	X_B->table[23]	1.0000 	\
		X_A->table[24]	0.00067	X_B->table[24]	1.0000 	\  // 0.020
		X_A->table[25]	0.00054	X_B->table[25]	1.0000 	\
		X_A->table[26]	0.00050	X_B->table[26]	1.0000 	\  // 0.030
		X_A->table[27]	0.00048	X_B->table[27]	1.0000 	\
		X_A->table[28]	0.00047	X_B->table[28]	1.0000 	\  // 0.040
		X_A->table[29]	0.00047	X_B->table[29]	1.0000 	\
		X_A->table[30]	0.00047	X_B->table[30]	1.0000 	   // 0.050

		/* Setting the calc_mode to NO_INTERP for speed */
		setfield {chanpath} X_A->calc_mode 0 X_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweaktau {chanpath} X

		/* Filling the tables using B-SPLINE interpolation */
		call {chanpath} TABFILL X 3000 0

    call    {chanpath}    TABCREATE Y 30 -0.100 0.050
	setfield  {chanpath}	\  //  A = tau_h, B = h_inf
		Y_A->table[0]	0.065	Y_B->table[0]	1.0   \ // -0.1
		Y_A->table[1]	0.065	Y_B->table[1]	1.0   \
		Y_A->table[2]	0.065	Y_B->table[2]	1.0   \	// -0.09
		Y_A->table[3]	0.065	Y_B->table[3]	1.0   \
		Y_A->table[4]	0.065	Y_B->table[4]	1.0   \ // -0.08
		Y_A->table[5]	0.065	Y_B->table[5]	1.0   \
		Y_A->table[6]	0.065	Y_B->table[6]	1.0   \	// -0.07
		Y_A->table[7]	0.065	Y_B->table[7]	1.0   \
		Y_A->table[8]	0.065	Y_B->table[8]	1.0   \	// -0.06
		Y_A->table[9]	0.065	Y_B->table[9]	1.0   \
		Y_A->table[10]	0.065	Y_B->table[10]	0.998 \ // -0.05
		Y_A->table[11]	0.0575	Y_B->table[11]	0.987	\
		Y_A->table[12]	0.050	Y_B->table[12]	0.935	\ // -0.04
		Y_A->table[13]	0.044	Y_B->table[13]	0.731	\
		Y_A->table[14]	0.038	Y_B->table[14]	0.339	\ // -0.030
		Y_A->table[15]	0.032	Y_B->table[15]	0.0884	\
		Y_A->table[16]	0.027	Y_B->table[16]	0.0180	\ // -0.020
		Y_A->table[17]	0.0232	Y_B->table[17]	0.0034	\
		Y_A->table[18]	0.0209	Y_B->table[18]	0.0     \ // -0.010
		Y_A->table[19]	0.0185	Y_B->table[19]	0.0  	\
		Y_A->table[20]	0.0168	Y_B->table[20]	0.0	\ // 0.00
		Y_A->table[21]	0.0153	Y_B->table[21]	0.0	\
		Y_A->table[22]	0.0135	Y_B->table[22]	0.0	\ // 0.010
		Y_A->table[23]	0.0129	Y_B->table[23]	0.0	\
		Y_A->table[24]	0.0118	Y_B->table[24]	0.0	\ // 0.020
		Y_A->table[25]	0.0111	Y_B->table[25]	0.0	\
		Y_A->table[26]	0.0103	Y_B->table[26]	0.0	\ // 0.030
		Y_A->table[27]	0.0100	Y_B->table[27]	0.0	\
		Y_A->table[28]	0.0096	Y_B->table[28]	0.0	\ // 0.040
		Y_A->table[29]	0.0094	Y_B->table[29]	0.0	\
		Y_A->table[30]	0.0088	Y_B->table[30]	0.0	  // 0.050

		/* Setting the calc_mode to NO_INTERP for speed */
		setfield {chanpath} Y_A->calc_mode 0 Y_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweaktau {chanpath} Y

		/* Filling the tables using B-SPLINE interpolation */
		call {chanpath} TABFILL Y 3000 0
end

/**********************************************************************
**                      Late potassium K-Current
**	K current activation from Thompson, J. Physiol 265: 465 (1977)
**	(Tritonia (LPl	2 and LPl 3 cells)
** Inactivation from RW Aldrich, PA Getting, and SH Thompson, 
** J. Physiol, 291: 507 (1979) (Anisidoris and Archidoris)
**
**********************************************************************/
function make_K_trit_agt  // K-current
    str chanpath = "K_trit_agt"
    if ({exists {chanpath}})
        return
    end

    create  tabchannel  {chanpath}
    setfield  {chanpath}  \
         Ek     {EK}              \
         Gbar   {65.7 * SOMA_A}   \
         Ik     0                 \
         Gk     0					\
		 Xpower	2					\
		 Ypower	1					\
		 Zpower	0

    call    {chanpath}    TABCREATE X 30 -0.100 0.050
    settab2const {chanpath}  X_A  0  12  0.0	//-0.1 thru -0.045=>0.0
    setfield		{chanpath} \
    	X_A->table[13] 	0.00 \       
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

    settab2const {chanpath} X_B  0  11  16.6	//-0.1 thru -0.045=>16.6
    setfield		{chanpath} \
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
		setfield {chanpath} X_A->calc_mode 0 X_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweakalpha {chanpath} X

		/* Filling the tables using B-SPLINE interpolation */
		call {chanpath} TABFILL X 3000 0
/* The alpha and beta values used here for the inactivation correspond to
   a constant value of tau_h = 1 sec.  This is a simplification of the
   AG&T results.  The "burster" tutorial sets Ypower to 0 (no inactivation).
*/
    call    {chanpath}    TABCREATE Y 30 -0.100 0.050
    settab2const {chanpath} Y_A  0  11  1.0  //-0.1 thru -0.035 => 1.0
    setfield		{chanpath} \
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

    settab2const  {chanpath}  Y_B  0  11  0.0  //-0.1 thru -0.045 => 0.0
    setfield		{chanpath}	\
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
		setfield {chanpath} Y_A->calc_mode 0 Y_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweakalpha {chanpath} Y

		/* Filling the tables using B-SPLINE interpolation */
		call {chanpath} TABFILL Y 3000 0

end

/**********************************************************************
**                 Transient potassium A-Current
**      J. A. Connor and C. F. Stevens, J. Physiol. (1971) 213, p. 31
**             data for Anisidoris gastropod neurons
**********************************************************************/

function make_KA_moll_cs  // A-current
    str chanpath = "A_moll_cs"
    if ({exists {chanpath}})
        return
    end

    create  tabchannel  {chanpath}
    setfield  {chanpath}  \
         Ek     {EA}              \
         Gbar   {65.7 * SOMA_A} \
         Ik     0                 \
         Gk     0		\
		 Xpower	4	\
		 Ypower	1	\
		 Zpower	0

    call    {chanpath}    TABCREATE X 30 -0.100 0.050
    settab2const {chanpath}  X_A  0  7  0.0 	//-0.1 thru -0.065 => 0.0
    setfield		{chanpath}	\
	X_A->table[8]		0.1 	\	// -0.060 Volts
    	X_A->table[9]		20.8	\
    	X_A->table[10]	38.4	\  // -0.050
    	X_A->table[11]	47.2	\
    	X_A->table[12]	54.3	\  // -0.040
    	X_A->table[13]	60.8	\
    	X_A->table[14]	65.7	\	// -0.030
    	X_A->table[15]	71.0	\
    	X_A->table[16]	74.8	\	// -0.020
    	X_A->table[17]	78.0	\	// a guess!
    	X_A->table[18]	80.0	\	// -0.010
    	X_A->table[19]	83.0
    settab2const  {chanpath}  X_A  20  30  83.3  //0 thru 50 => 83.3

    settab2const  {chanpath}  X_B  0  7  83.3    //-0.1 thru -0.065 => 83.3
    setfield		{chanpath}	\
	X_B->table[8]		83.3	\	// -0.060 Volts
    	X_B->table[9]		62.6	\
    	X_B->table[10]		44.9	\   // -0.050
    	X_B->table[11]		36.2	\
    	X_B->table[12]		29.1	\   // -0.040
    	X_B->table[13]		22.6	\
    	X_B->table[14]		17.7	\	// -0.030
    	X_B->table[15]		12.3	\
    	X_B->table[16]		8.5 	\	// -0.020
    	X_B->table[17]		5.0 	\	// a guess!
    	X_B->table[18]		3.1 	\	// -0.010
    	X_B->table[19]		1.2
    settab2const  {chanpath}  X_B  20  30  0.0 	//0 thru 0.050 => 0.0

		/* Setting the calc_mode to NO_INTERP for speed */
		setfield {chanpath} X_A->calc_mode 0 X_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweakalpha {chanpath} X

		/* Filling the tables using B-SPLINE interpolation */
		call {chanpath} TABFILL X 3000 0


    call    {chanpath}    TABCREATE Y 30 -0.100 0.050
    setfield		{chanpath}	\
	Y_A->table[0]		4.26	\ // -0.100 Volts
    	Y_A->table[1]		4.12	\
    	Y_A->table[2]		3.95	\ // -0.090
    	Y_A->table[3]		3.69	\
    	Y_A->table[4]		3.34	\ // -0.080
    	Y_A->table[5]		2.94	\
    	Y_A->table[6]		2.32	\ // -0.070
    	Y_A->table[7]		1.54	\
    	Y_A->table[8]		0.66	\	// -0.060
    	Y_A->table[9]		0.306	\
   	Y_A->table[10]		0.123	\ // -0.050
    	Y_A->table[11]		0.053
    settab2const {chanpath}  Y_A  12  30  0.0  // -0.04 thru 0.05 => 0.0

    setfield		{chanpath}	\
	Y_B->table[0]		0.00	\    // -0.1 Volts
    	Y_B->table[1]		0.13	\
    	Y_B->table[2]		0.31	\ // -0.090
    	Y_B->table[3]		0.57	\
    	Y_B->table[4]		0.92	\ // -0.080
    	Y_B->table[5]		1.32	\
    	Y_B->table[6]		1.93	\ // -0.070
    	Y_B->table[7]		2.72	\
    	Y_B->table[8]		3.60	\	// -0.060 mV
    	Y_B->table[9]		3.95	\
    	Y_B->table[10]		4.13	\ // -0.050 mV
    	Y_B->table[11]		4.20
    settab2const  {chanpath}  Y_B  12  30  4.26 // -0.04 thru 0.05=> 4.26

		/* Setting the calc_mode to NO_INTERP for speed */
		setfield {chanpath} Y_A->calc_mode 0 Y_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweakalpha  {chanpath}  Y

		/* Filling the tables using B-SPLINE interpolation */
		call {chanpath} TABFILL Y 3000 0
end

/**********************************************************************
**                High threshold Ca-current
**	Adams and Gage, J. Physiol 289 p143	(1979) (Aplysia R15)
**********************************************************************/

function make_Ca_aplysia_ag  // Ca-current
    str chanpath = "Ca_aplysia_ag"
    if ({exists {chanpath}})
        return
    end

    create  tabchannel  {chanpath}
    setfield  {chanpath}  \
         Ek     {ECa}              \
         Gbar   {65.2 * SOMA_A} \
         Ik     0                 \
         Gk     0		\
		 Xpower	2		\
		 Ypower	1		\
		 Zpower	0

    call    {chanpath}    TABCREATE X 30 -0.100 0.050
    settab2const  {chanpath}  X_A  0  18  0.0  //-0.1 thru -0.005 => 0.0
    setfield		{chanpath}	\
	X_A->table[19]	0.0 	\	// -0.005 Volts
    	X_A->table[20]	39.2	\	// 0.0
    	X_A->table[21]	46.7	\   // 0.005
    	X_A->table[22]	54.2	\	// 0.010
    	X_A->table[23]	85.3	\   // 0.015
    	X_A->table[24]	111.9	\	// 0.020
    	X_A->table[25]	128.2	\	// 0.025
    	X_A->table[26]	144.6	\	// 0.030
    	X_A->table[27]	156.7	\	// 0.035
    	X_A->table[28]	166.7	\	// 0.040
    	X_A->table[29]	166.7	\	// 0.045
    	X_A->table[30]	166.7		// 0.050

    settab2const  {chanpath}  X_B  0  17  250.0 //-0.1 thru -0.01 =>250
    setfield		{chanpath}	\
	X_B->table[18]	250.0 	\	// -0.010 Volts
	X_B->table[19]	217.0 	\	// -0.005 Volts
    	X_B->table[20]	147.0	\	// 0.0
    	X_B->table[21]	103.9	\   // 0.005
    	X_B->table[22]	64.4	\	// 0.010
    	X_B->table[23]	26.5	\   // 0.015
    	X_B->table[24]	8.55	\	// 0.020
    	X_B->table[25]	2.35	\	// 0.025
    	X_B->table[26]	0.58	\	// 0.030
    	X_B->table[27]	0.0		\	// 0.035
    	X_B->table[28]	0.0		\	// 0.040
    	X_B->table[29]	0.0		\	// 0.045
    	X_B->table[30]	0.0			// 0.050

		/* Setting the calc_mode to NO_INTERP for speed */
		setfield {chanpath} X_A->calc_mode 0 X_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweakalpha  {chanpath} X

		/* Filling the tables using B-SPLINE interpolation */
		call {chanpath} TABFILL X 3000 0

    call    {chanpath}    TABCREATE Y 30 -0.100 0.050
    settab2const  {chanpath}  Y_A  0  11  10.0  // -0.1 to -0.04=>10.0
    setfield		{chanpath}	\
	Y_A->table[12]		10.0	\ // -0.040 Volts
    	Y_A->table[13]		9.7		\
    	Y_A->table[14]		9.24	\ // -0.030
    	Y_A->table[15]		7.5		\
    	Y_A->table[16]		5.0		\ // -0.020
    	Y_A->table[17]		2.5		\
    	Y_A->table[18]		0.633	\ // -0.010
    	Y_A->table[19]		0.153	\
    	Y_A->table[20]		0.004	\	// 0.00
    	Y_A->table[21]		0.0	
    settab2const {chanpath}  Y_A  22  30  0.0  // 0.01 thru 0.05 => 0.0

    settab2const {chanpath}  Y_B  0  11  0.0  // -0.1 to -0.04=>00.0
    setfield		{chanpath}	\
	Y_B->table[12]		0.0		\ // -0.040 Volts
    	Y_B->table[13]		0.3		\
    	Y_B->table[14]		0.76	\ // -0.030
    	Y_B->table[15]		2.5		\
    	Y_B->table[16]		5.0		\ // -0.020
    	Y_B->table[17]		6.5		\
    	Y_B->table[18]		7.7		\ // -0.010
    	Y_B->table[19]		6.51	\
    	Y_B->table[20]		5.68	\	// 0.00
    	Y_B->table[21]		4.44	\
    	Y_B->table[22]		3.85	\	// 0.010
    	Y_B->table[23]		5.26	\
    	Y_B->table[24]		8.33	\	// 0.020
    	Y_B->table[25]		11.11	\
    	Y_B->table[26]		14.29	\	// 0.030
    	Y_B->table[27]		16.67	\
    	Y_B->table[28]		18.18	\	// 0.040
    	Y_B->table[29]		18.18	\
    	Y_B->table[30]		18.18		//0.05

		/* Setting the calc_mode to NO_INTERP for speed */
		setfield {chanpath} Y_A->calc_mode 0 Y_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweakalpha {chanpath} Y

		/* Filling the tables using B-SPLINE interpolation */
		call {chanpath} TABFILL Y 3000 0
end

/**********************************************************************
**          B-current : Slow inward Na and Ca bursting conductance
**  SJ Smith and SH Thompson  J.Physiol 382 p425	(1987) (Tritonia)
**********************************************************************/

function make_B_trit_st
    str chanpath = "B_trit_st"
    if ({exists {chanpath}})
        return
    end

    create  tabchannel  {chanpath}
    setfield  {chanpath}  \
         Ek     {EB}               \
         Gbar   {0.35*SOMA_A} \
         Ik     0		\
         Gk     0		\
                 Xpower 1               \
                 Ypower 0               \
                 Zpower 0

    call	{chanpath}	TABCREATE X 30 -0.100 0.050 // in volts
    settab2const {chanpath}  X_A  0  7  2.270 
    setfield	{chanpath} X_A->table[8]  2.270 \  //  -60 mV
	 X_A->table[9]	2.040  \ //  -55 mV
	 X_A->table[10]	1.800  \ //  -50 mV
	 X_A->table[11]	1.513  \ //  -45 mV
	 X_A->table[12]	1.220  \ //  -40 mV
	 X_A->table[13]	0.957  \ //  -35 mV
	 X_A->table[14]	0.690  \ //  -30 mV
	 X_A->table[15]	0.515  \ //  -25 mV
	 X_A->table[16]	0.390  \ //  -20 mV
	 X_A->table[17]	0.337  \ //  -15 mV
	 X_A->table[18]	0.310  \ //  -10 mV
	 X_A->table[19]	0.271  \ //   -5 mV
	 X_A->table[20]	0.240  \ //    0 mV
	 X_A->table[21]	0.216  \ //    5 mV
	 X_A->table[22]	0.209  \ //   10 mV
	 X_A->table[23]	0.205  \ //   15 mV
	 X_A->table[24]	0.202  \ //   20 mV
	 X_A->table[25]	0.191  \ //   25 mV
	 X_A->table[26]	0.174  \ //   30 mV
	 X_A->table[27]	0.157  \ //   35 mV
	 X_A->table[28]	0.139  \ //   40 mV
	 X_A->table[29]	0.115  \ //   45 mV
	 X_A->table[30]	0.104    //   50 mV

    settab2const {chanpath}  X_B  0  8  0.0 
    setfield	{chanpath} X_B->table[9]	0.0 \  //  -55 mV
	 X_B->table[10]	0.011 \  //  -50 mV
	 X_B->table[11]	0.0224\ //  -45 mV
	 X_B->table[12]	0.0370\ //  -40 mV
	 X_B->table[13]	0.0555\ //  -35 mV
	 X_B->table[14]	0.0815\ //  -30 mV
	 X_B->table[15]	0.1111\ //  -25 mV
	 X_B->table[16]	0.1333\ //  -20 mV
	 X_B->table[17]	0.155 \ //  -15 mV
	 X_B->table[18]	0.181 \ //  -10 mV
	 X_B->table[19]	0.200 \ //   -5 mV
	 X_B->table[20]	0.222 \ //    0 mV
	 X_B->table[21]	0.244 \ //    5 mV
	 X_B->table[22]	0.270 \ //   10 mV
	 X_B->table[23]	0.296 \ //   15 mV
	 X_B->table[24]	0.322 \ //   20 mV
	 X_B->table[25]	0.357 \ //   25 mV
	 X_B->table[26]	0.400 \ //   30 mV
	 X_B->table[27]	0.456 \ //   35 mV
	 X_B->table[28]	0.504 \ //   40 mV
	 X_B->table[29]	0.550 \ //   45 mV
	 X_B->table[30]	0.585  //   50 mV

    /* tweaking the tables for the tabchan calculation */
    tweaktau {chanpath} X

   /* Filling the tables using B-SPLINE interpolation */
   call {chanpath} TABFILL X 3000 0

   /* Setting the calc_mode to NO_INTERP for speed */
   setfield {chanpath} X_A->calc_mode 0 X_B->calc_mode 0
end

//========================================================================
//              Ca concentration (from Ca and B channels)
//========================================================================

//  Cell reader will set up messages from Ca_aplysia_ag and from B_trit_st
//  If these channel names are changed in the channel creation functions,
//  change them in the messages here also

function make_Ca_conc
        if ({exists Ca_conc})
                return
        end
        create Ca_concen Ca_conc
        setfield Ca_conc \
                tau     17.5   \      // sec
                B       1000 \      // Curr to conc for soma
                Ca_base 0
	addfield Ca_conc addmsg1
	addfield Ca_conc addmsg2
        setfield Ca_conc \
                addmsg1        "../Ca_aplysia_ag . I_Ca Ik" \
                addmsg2        "../B_trit_st . I_Ca Ik"
end

//========================================================================
//  Ca-dependent K Channel - K(C) - (vdep_channel with table and tabgate)
// Voltage and [Ca] dependence was taken from A. L. F. Gorman and M. V.
// Thomas, J.  Physiol. (London) 308: 287-313 (1980). (Aplysia R15)
//========================================================================

function make_Kc_aplysia_gt
	str chanpath = "K_C"
        if ({exists {chanpath}})
                return
        end

        create  vdep_channel    {chanpath}
                setfield             ^       \
                Ek              {EK}    \                       //      V
                gbar            { 124.0 * SOMA_A }      \       //      S
                Ik              0       \                       //      A
                Gk              0                               //      S

// Create a table for the function of concentration, allowing a
// concentration range of 0 to 2000e-6, with 50 divisions.  Note that the
// internal field for the table object is called "table".
        float   xmin = 0.0
        float   xmax = 0.002
        int     xdivs = 50
	float Ca_max = 187e-6	// nominal conc for expt results = 187 nM
        create table            {chanpath}/tab
        call {chanpath}/tab TABCREATE {xdivs} {xmin} {xmax}
        int i
        float x,dx,y
        dx = (xmax - xmin)/xdivs
        x = xmin
// The concentration-dependent factor is a linear function of [Ca]
// which is unity at [Ca] = Ca_max
        for (i = 0 ; i <= {xdivs} ; i = i + 1)
	    y = x/Ca_max
            setfield {chanpath}/tab table->table[{i}] {y}
            x = x + dx
        end
// Expand the table to 3000 entries to use without interpolation.  The
// TABFILL syntax is slightly different from that used with tabchannels.
// Here there is only one internal table, so the table name is not specified.

	setfield {chanpath}/tab table->calc_mode 0
	call {chanpath}/tab TABFILL 3000 0

// Now make a tabgate for the voltage-dependent activation parameter.
        float   xmin = -0.1
        float   xmax = 0.05
        int     xdivs = 49
        create  tabgate         {chanpath}/X
        call {chanpath}/X TABCREATE alpha {xdivs} {xmin} {xmax}
        call {chanpath}/X TABCREATE beta  {xdivs} {xmin} {xmax}
        int i
        float x,dx,alpha,beta, minf, tau
	tau = 3.8	// estimate from Thompson, Smith and Johnson 1986
        dx = (xmax - xmin)/xdivs
        x = xmin
        for (i = 0 ; i <= {xdivs} ; i = i + 1)
	    minf = 1/(1.0 + {exp {(0.0653 - x)/0.0253}})
	    alpha = minf/tau
	    beta = (1-minf)/tau
            setfield {chanpath}/X alpha->table[{i}] {alpha}
            setfield {chanpath}/X beta->table[{i}] {beta}
            x = x + dx
        end
	setfield {chanpath}/X alpha->calc_mode 0 beta->calc_mode 0
	call {chanpath}/X TABFILL alpha 3000 0
	call {chanpath}/X TABFILL beta  3000 0

        addmsg {chanpath}/tab  {chanpath} MULTGATE output 1
        addmsg {chanpath}/X  {chanpath}  MULTGATE m 1
	addfield {chanpath} addmsg1
	addfield {chanpath} addmsg2
        setfield {chanpath} \
                addmsg1        "../Ca_conc  tab INPUT Ca" \
                addmsg2        "..  X  VOLTAGE Vm"
end

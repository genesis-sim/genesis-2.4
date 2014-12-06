// genesis


/* FILE INFORMATION
** Some Voltage dependent Ca channels 
** Data from : Fox, Nowycky and Tsien, J Physiol 394 pp 149-172 (1987)
** and  Fox, Nowycky and Tsien, J Physiol 394 pp 173-200 (1987)
** 
** Implemented in NEUROKIT format by : Upinder S. Bhalla. 
** The voltage dependencies of activation and inactivation time
** constants are based on the voltage-step experiment curves,
** which appear to be inconsistent with the inactivation time constant
** curves presented in the papers.
**
** This file depends on functions and constants defined in defaults.g
*/

// CONSTANTS
float ECA = 0.09
float SOMA_A = 1e-9

/*****************************************************************
**			Ca 'N' channel
*****************************************************************/

function make_NCa_drg_fnt_tab
	if (({exists NCa_drg_fnt_tab}))
		return
	end

	create vdep_channel NCa_drg_fnt_tab
	setfield NCa_drg_fnt_tab Ek {ECA} gbar {1200.0*SOMA_A} Ik 0 Gk 0

	create tabgate NCa_drg_fnt_tab/m
	create tabgate NCa_drg_fnt_tab/h

	call NCa_drg_fnt_tab/m TABCREATE alpha 29 -0.10 0.045
	call NCa_drg_fnt_tab/m TABCREATE beta 29 -0.10 0.045
	// -0.1 V
	// -0.095 V
	// -0.09 V
	// -0.085 V
	// -0.08 V
	// -0.075 V
	// -0.07 V
	// -0.065 V
	// -0.06 V
	// -0.055 V
	// -0.05 V
	// -0.045 V
	// -0.04 V
	// -0.035 V
	// -0.03 V
	// -0.025 V
	// -0.02 V
	// -0.015 V
	// -0.01 V
	// -0.005 V
	// -0.0 V
	// 0.005 V
	// 0.01 V
	// 0.015 V
	// 0.02 V
	// 0.025 V
	// 0.03 V
	// 0.035 V
	// 0.04 V
	// 0.045 V
	setfield NCa_drg_fnt_tab/m alpha->table[0] 0.0  \
	    beta->table[0] 1316 alpha->table[1] 0.0 beta->table[1] 1282  \
	    alpha->table[2] 0.0 beta->table[2] 1250 alpha->table[3] 0.0  \
	    beta->table[3] 1219 alpha->table[4] 0.0 beta->table[4] 1176  \
	    alpha->table[5] 0.0 beta->table[5] 1111 alpha->table[6] 0.0  \
	    beta->table[6] 1000 alpha->table[7] 0.0 beta->table[7] 833  \
	    alpha->table[8] 0.0 beta->table[8] 556 alpha->table[9] 0.0  \
	    beta->table[9] 417 alpha->table[10] 0.0 beta->table[10] 333  \
	    alpha->table[11] 0.0 beta->table[11] 278  \
	    alpha->table[12] 2.38 beta->table[12] 236  \
	    alpha->table[13] 2.0 beta->table[13] 198  \
	    alpha->table[14] 5.17 beta->table[14] 167  \
	    alpha->table[15] 8.33 beta->table[15] 162  \
	    alpha->table[16] 19.6 beta->table[16] 158  \
	    alpha->table[17] 42.0 beta->table[17] 154  \
	    alpha->table[18] 88.1 beta->table[18] 150  \
	    alpha->table[19] 165 beta->table[19] 129  \
	    alpha->table[20] 281 beta->table[20] 104  \
	    alpha->table[21] 425 beta->table[21] 75.0  \
	    alpha->table[22] 620 beta->table[22] 46.7  \
	    alpha->table[23] 873 beta->table[23] 36.4  \
	    alpha->table[24] 1225 beta->table[24] 25.0  \
	    alpha->table[25] 1320 beta->table[25] 13.3  \
	    alpha->table[26] 1389 beta->table[26] 0.0  \
	    alpha->table[27] 1429 beta->table[27] 0.0  \
	    alpha->table[28] 1471 beta->table[28] 0.0  \
	    alpha->table[29] 1515 beta->table[29] 0.0
	/* Setting the calc_mode to NO_INTERP for speed */
	setfield NCa_drg_fnt_tab/m alpha->calc_mode 0 beta->calc_mode 0
	/* Filling the tables using B-SPLINE interpolation */
	call NCa_drg_fnt_tab/m TABFILL alpha 3000 0
	call NCa_drg_fnt_tab/m TABFILL beta 3000 0

	/* Creaating the tables
	call NCa_drg_fnt_tab/h TABCREATE alpha 29 -0.10 0.045
	call NCa_drg_fnt_tab/h TABCREATE beta 29 -0.10 0.045
	set NCa_drg_fnt_tab/h \
		alpha->table[0]  70.8	beta->table[0]	6.15	\	// -0.1 V
		alpha->table[1]  58.7	beta->table[1]	8.00	\	// -0.095 V
		alpha->table[2]  46.1	beta->table[2]	9.44	\	// -0.09 V
		alpha->table[3]  33.5	beta->table[3]	10.0	\	// -0.085 V
		alpha->table[4]  23.8	beta->table[4]	10.7	\	// -0.08 V
		alpha->table[5]  17.1	beta->table[5]	11.4	\	// -0.075 V
		alpha->table[6]  11.4	beta->table[6]	11.4	\	// -0.07 V
		alpha->table[7]  8.00	beta->table[7]	12.0	\	// -0.065 V
		alpha->table[8]  5.85	beta->table[8]	13.0	\	// -0.06 V
		alpha->table[9]  4.34	beta->table[9]	14.5	\	// -0.055 V
		alpha->table[10] 3.33	beta->table[10]	16.3	\	// -0.05 V
		alpha->table[11] 2.50	beta->table[11]	18.3	\	// -0.045 V
		alpha->table[12] 1.82	beta->table[12]	20.9	\	// -0.04 V
		alpha->table[13] 1.54	beta->table[13]	24.1	\	// -0.035 V
		alpha->table[14] 1.21	beta->table[14]	29.1	\	// -0.03 V
		alpha->table[15] 1.07	beta->table[15]	34.6	\	// -0.025 V
		alpha->table[16] 0.80	beta->table[16]	39.2	\	// -0.02 V
		alpha->table[17] 0.50	beta->table[17]	43.0	\	// -0.015 V
		alpha->table[18] 0.476	beta->table[18]	47.1	\	// -0.01 V
		alpha->table[19] 0.00	beta->table[19]	52.6	\	// -0.005 V
		alpha->table[20] 0.00	beta->table[20]	58.8	\	// -0.0 V
		alpha->table[21] 0.00	beta->table[21]	66.7	\	// 0.005 V
		alpha->table[22] 0.00	beta->table[22]	76.9	\	// 0.01 V
		alpha->table[23] 0.00	beta->table[23]	78.7	\	// 0.015 V
		alpha->table[24] 0.00	beta->table[24]	80.0	\	// 0.02 V
                alpha->table[25] 0.00	beta->table[25] 82.0	\	// 0.02 V
		alpha->table[26] 0.00	beta->table[26]	83.3	\	// 0.03 V
		alpha->table[27] 0.00	beta->table[27]	83.3	\	// 0.035 V
		alpha->table[28] 0.00	beta->table[28]	83.3	\	// 0.04 V
		alpha->table[29] 0.00	beta->table[29]	83.3		// 0.045 V
	/* Setting the calc_mode to NO_INTERP for speed */
	setfield NCa_drg_fnt_tab/h alpha->calc_mode 0 beta->calc_mode 0
	/* Filling the tables using B-SPLINE interpolation */
	call NCa_drg_fnt_tab/h TABFILL alpha 3000 0
	call NCa_drg_fnt_tab/h TABFILL beta 3000 0

	addmsg NCa_drg_fnt_tab/m NCa_drg_fnt_tab MULTGATE m 2
	addmsg NCa_drg_fnt_tab/h NCa_drg_fnt_tab MULTGATE m 1
        addfield  NCa_drg_fnt_tab addmsg1
        addfield  NCa_drg_fnt_tab addmsg2
	setfield  NCa_drg_fnt_tab addmsg1 ".. m	VOLTAGE	Vm"  \
	    addmsg2 ".. h	VOLTAGE	Vm"
end

/*****************************************************************
**			Ca 'N' channel : implemented as tabchan
*****************************************************************/

function make_NCa_drg_fnt
	if (({exists NCa_drg_fnt}))
		return
	end

	create tabchannel NCa_drg_fnt
	setfield NCa_drg_fnt Ek {ECA} Gbar {1200.0*SOMA_A} Ik 0 Gk 0  \
	    Xpower 2 Ypower 1 Zpower 0

	call NCa_drg_fnt TABCREATE X 29 -0.10 0.045
	// -0.1 V
	// -0.095 V
	// -0.09 V
	// -0.085 V
	// -0.08 V
	// -0.075 V
	// -0.07 V
	// -0.065 V
	// -0.06 V
	// -0.055 V
	// -0.05 V
	// -0.045 V
	// -0.04 V
	// -0.035 V
	// -0.03 V
	// -0.025 V
	// -0.02 V
	// -0.015 V
	// -0.01 V
	// -0.005 V
	// -0.0 V
	// 0.005 V
	// 0.01 V
	// 0.015 V
	// 0.02 V
	// 0.025 V
	// 0.03 V
	// 0.035 V
	// 0.04 V
	// 0.045 V
	setfield NCa_drg_fnt X_A->table[0] 0.0 X_B->table[0] 1316  \
	    X_A->table[1] 0.0 X_B->table[1] 1282 X_A->table[2] 0.0  \
	    X_B->table[2] 1250 X_A->table[3] 0.0 X_B->table[3] 1219  \
	    X_A->table[4] 0.0 X_B->table[4] 1176 X_A->table[5] 0.0  \
	    X_B->table[5] 1111 X_A->table[6] 0.0 X_B->table[6] 1000  \
	    X_A->table[7] 0.0 X_B->table[7] 833 X_A->table[8] 0.0  \
	    X_B->table[8] 556 X_A->table[9] 0.0 X_B->table[9] 417  \
	    X_A->table[10] 0.0 X_B->table[10] 333 X_A->table[11] 0.0  \
	    X_B->table[11] 278 X_A->table[12] 2.38 X_B->table[12] 236  \
	    X_A->table[13] 2.0 X_B->table[13] 198 X_A->table[14] 5.17  \
	    X_B->table[14] 167 X_A->table[15] 8.33 X_B->table[15] 162  \
	    X_A->table[16] 19.6 X_B->table[16] 158 X_A->table[17] 42.0  \
	    X_B->table[17] 154 X_A->table[18] 88.1 X_B->table[18] 150  \
	    X_A->table[19] 165 X_B->table[19] 129 X_A->table[20] 281  \
	    X_B->table[20] 104 X_A->table[21] 425 X_B->table[21] 75.0  \
	    X_A->table[22] 620 X_B->table[22] 46.7 X_A->table[23] 873  \
	    X_B->table[23] 36.4 X_A->table[24] 1225 X_B->table[24] 25.0  \
	    X_A->table[25] 1320 X_B->table[25] 13.3 X_A->table[26] 1389  \
	    X_B->table[26] 0.0 X_A->table[27] 1429 X_B->table[27] 0.0  \
	    X_A->table[28] 1471 X_B->table[28] 0.0 X_A->table[29] 1515  \
	    X_B->table[29] 0.0

	/* Setting the calc_mode to NO_INTERP for speed */
	setfield NCa_drg_fnt X_A->calc_mode 0 X_B->calc_mode 0

	/* tweaking the tables for the tabchan calculation */
	tweak_tabchan NCa_drg_fnt X

	/* Filling the tables using B-SPLINE interpolation */
	call NCa_drg_fnt TABFILL X 3000 0


	/* Creating the tables */
	call NCa_drg_fnt TABCREATE Y 29 -0.10 0.045
	// -0.1 V
	// -0.095 V
	// -0.09 V
	// -0.085 V
	// -0.08 V
	// -0.075 V
	// -0.07 V
	// -0.065 V
	// -0.06 V
	// -0.055 V
	// -0.05 V
	// -0.045 V
	// -0.04 V
	// -0.035 V
	// -0.03 V
	// -0.025 V
	// -0.02 V
	// -0.015 V
	// -0.01 V
	// -0.005 V
	// -0.0 V
	// 0.005 V
	// 0.01 V
	// 0.015 V
	// 0.02 V
	// 0.025 V
	// 0.03 V
	// 0.035 V
	// 0.04 V
	// 0.045 V
	setfield NCa_drg_fnt Y_A->table[0] 70.8 Y_B->table[0] 6.15  \
	    Y_A->table[1] 58.7 Y_B->table[1] 8.00 Y_A->table[2] 46.1  \
	    Y_B->table[2] 9.44 Y_A->table[3] 33.5 Y_B->table[3] 10.0  \
	    Y_A->table[4] 23.8 Y_B->table[4] 10.7 Y_A->table[5] 17.1  \
	    Y_B->table[5] 11.4 Y_A->table[6] 11.4 Y_B->table[6] 11.4  \
	    Y_A->table[7] 8.00 Y_B->table[7] 12.0 Y_A->table[8] 5.85  \
	    Y_B->table[8] 13.0 Y_A->table[9] 4.34 Y_B->table[9] 14.5  \
	    Y_A->table[10] 3.33 Y_B->table[10] 16.3 Y_A->table[11] 2.50  \
	    Y_B->table[11] 18.3 Y_A->table[12] 1.82 Y_B->table[12] 20.9  \
	    Y_A->table[13] 1.54 Y_B->table[13] 24.1 Y_A->table[14] 1.21  \
	    Y_B->table[14] 29.1 Y_A->table[15] 1.07 Y_B->table[15] 34.6  \
	    Y_A->table[16] 0.80 Y_B->table[16] 39.2 Y_A->table[17] 0.50  \
	    Y_B->table[17] 43.0 Y_A->table[18] 0.47 Y_B->table[18] 47.1  \
	    Y_A->table[19] 0.00 Y_B->table[19] 52.6 Y_A->table[20] 0.00  \
	    Y_B->table[20] 58.8 Y_A->table[21] 0.00 Y_B->table[21] 66.7  \
	    Y_A->table[22] 0.00 Y_B->table[22] 76.9 Y_A->table[23] 0.00  \
	    Y_B->table[23] 78.7 Y_A->table[24] 0.00 Y_B->table[24] 80.0  \
	    Y_A->table[25] 0.00 Y_B->table[25] 82.0 Y_A->table[26] 0.00  \
	    Y_B->table[26] 83.3 Y_A->table[27] 0.00 Y_B->table[27] 83.3  \
	    Y_A->table[28] 0.00 Y_B->table[28] 83.3 Y_A->table[29] 0.00  \
	    Y_B->table[29] 83.3

	/* Setting the calc_mode to NO_INTERP for speed */
	setfield NCa_drg_fnt Y_A->calc_mode 0 Y_B->calc_mode 0

	/* tweaking the tables for the tabchan calculation */
	tweak_tabchan NCa_drg_fnt Y

	/* Filling the tables using B-SPLINE interpolation */
	call NCa_drg_fnt TABFILL Y 3000 0
end

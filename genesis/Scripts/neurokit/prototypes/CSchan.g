// genesis

/* FILE INFORMATION
** Some Voltage Na and K channels 
** Data from :
** J. A. Connor and C. F. Stevens, J. Physiol. (1971) 213, p. 31
**  data for Ansidoris gastropod neurons
** 
** Implemented by David Beeman - January 1991
** slightly modified for NEUROKIT format by Upinder S. Bhalla. 
** Converted to tabchans by Upinder S. Bhalla - Feb 1991
** This file depends on functions and constants defined in defaults.g
*/

// CONSTANTS
// (I-current)
float ENa = 0.045
float EK = -0.060
float EA = -0.063
// sq m
float SOMA_A = 1e-9


/*********************************************************************
**                          I-Current (Na)
*********************************************************************/

function make_Na_moll_cs// I-current (Na)

    if (({exists Na_moll_cs}))
        return
    end

    create tabchannel Na_moll_cs
    setfield Na_moll_cs Ek {ENa} Gbar {1200.0*SOMA_A} Ik 0 Gk 0 Xpower 3 \
         Ypower 1 Zpower 0

    call Na_moll_cs TABCREATE X 30 -0.100 0.050
	// -0.1
	// -0.09
	// -0.08
	// -0.07
	// -0.06
	// -0.05
	// -0.04
	// -0.030
	// -0.020
	// -0.010
	// 0.00
	// 0.010
	// 0.020
	// 0.030
	// 0.040
	// 0.050
	setfield Na_moll_cs X_A->table[0] 0.0 X_B->table[0] 90.9  \
	    X_A->table[1] 0.0 X_B->table[1] 90.9 X_A->table[2] 0.0  \
	    X_B->table[2] 90.9 X_A->table[3] 0.0 X_B->table[3] 90.9  \
	    X_A->table[4] 0.0 X_B->table[4] 90.9 X_A->table[5] 0.0  \
	    X_B->table[5] 90.9 X_A->table[6] 0.0 X_B->table[6] 90.9  \
	    X_A->table[7] 0.0 X_B->table[7] 90.9 X_A->table[8] 0.0  \
	    X_B->table[8] 90.9 X_A->table[9] 0.0 X_B->table[9] 90.9  \
	    X_A->table[10] 0.0 X_B->table[10] 90.9 X_A->table[11] 0.0  \
	    X_B->table[11] 90.9 X_A->table[12] 0.0 X_B->table[12] 90.9  \
	    X_A->table[13] 0.0 X_B->table[13] 90.9 X_A->table[14] 21.3  \
	    X_B->table[14] 69.6 X_A->table[15] 34.5 X_B->table[15] 62.6  \
	    X_A->table[16] 60.2 X_B->table[16] 50.7 X_A->table[17] 124.0 \
	     X_B->table[17] 43.0 X_A->table[18] 284.0  \
	    X_B->table[18] 45.0 X_A->table[19] 513.0 X_B->table[19] 42.0 \
	     X_A->table[20] 788.0 X_B->table[20] 25.2  \
	    X_A->table[21] 928.0 X_B->table[21] 10.3  \
	    X_A->table[22] 1104.0 X_B->table[22] 4.4  \
	    X_A->table[23] 1352.0 X_B->table[23] 2.7  \
	    X_A->table[24] 2257.0 X_B->table[24] 0.0  \
	    X_A->table[25] 2257.0 X_B->table[25] 0.0  \
	    X_A->table[26] 2257.0 X_B->table[26] 0.0  \
	    X_A->table[27] 2257.0 X_B->table[27] 0.0  \
	    X_A->table[28] 2257.0 X_B->table[28] 0.0  \
	    X_A->table[29] 2257.0 X_B->table[29] 0.0  \
	    X_A->table[30] 2257.0 X_B->table[30] 0.0

		/* Setting the calc_mode to NO_INTERP for speed */
		setfield Na_moll_cs X_A->calc_mode 0 X_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweak_tabchan Na_moll_cs X

		/* Filling the tables using B-SPLINE interpolation */
		call Na_moll_cs TABFILL X 3000 0

    call Na_moll_cs TABCREATE Y 30 -0.100 0.050
	// -0.1
	// -0.09
	// -0.08
	// -0.07
	// -0.06
	// -0.05
	// -0.04
	// -0.030
	// -0.020
	// -0.010
	// 0.00
	// 0.010
	// 0.020
	// 0.030
	// 0.040
	// 0.050
	setfield Na_moll_cs Y_A->table[0] 12.5 Y_B->table[0] 0.0  \
	    Y_A->table[1] 12.5 Y_B->table[1] 0.0 Y_A->table[2] 12.5  \
	    Y_B->table[2] 0.0 Y_A->table[3] 12.5 Y_B->table[3] 0.0  \
	    Y_A->table[4] 12.5 Y_B->table[4] 0.0 Y_A->table[5] 12.5  \
	    Y_B->table[5] 0.0 Y_A->table[6] 12.5 Y_B->table[6] 0.0  \
	    Y_A->table[7] 12.5 Y_B->table[7] 0.0 Y_A->table[8] 12.5  \
	    Y_B->table[8] 0.0 Y_A->table[9] 12.5 Y_B->table[9] 0.0  \
	    Y_A->table[10] 12.5 Y_B->table[10] 0.0 Y_A->table[11] 12.3  \
	    Y_B->table[11] 0.19 Y_A->table[12] 11.6 Y_B->table[12] 0.89  \
	    Y_A->table[13] 10.0 Y_B->table[13] 2.49 Y_A->table[14] 7.94  \
	    Y_B->table[14] 5.10 Y_A->table[15] 4.9 Y_B->table[15] 12.9  \
	    Y_A->table[16] 0.58 Y_B->table[16] 30.0 Y_A->table[17] 0.0  \
	    Y_B->table[17] 99.3 Y_A->table[18] 0.0 Y_B->table[18] 204.1  \
	    Y_A->table[19] 0.0 Y_B->table[19] 245.1 Y_A->table[20] 0.0  \
	    Y_B->table[20] 278.6 Y_A->table[21] 0.0 Y_B->table[21] 285.7 \
	     Y_A->table[22] 0.0 Y_B->table[22] 285.7 Y_A->table[23] 0.0  \
	    Y_B->table[23] 285.7 Y_A->table[24] 0.0 Y_B->table[24] 285.7 \
	     Y_A->table[25] 0.0 Y_B->table[25] 285.7 Y_A->table[26] 0.0  \
	    Y_B->table[26] 285.7 Y_A->table[27] 0.0 Y_B->table[27] 285.7 \
	     Y_A->table[28] 0.0 Y_B->table[28] 285.7 Y_A->table[29] 0.0  \
	    Y_B->table[29] 285.7 Y_A->table[30] 0.0 Y_B->table[30] 285.7

		/* Setting the calc_mode to NO_INTERP for speed */
		setfield Na_moll_cs Y_A->calc_mode 0 Y_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweak_tabchan Na_moll_cs Y

		/* Filling the tables using B-SPLINE interpolation */
		call Na_moll_cs TABFILL Y 3000 0
end

/**********************************************************************
**                      Late potassium K-Current
**********************************************************************/
function make_K_moll_cs// K-current     

    if ({exists K_moll_cs})
        return
    end

    create tabchannel K_moll_cs
    setfield K_moll_cs Ek {EK} Gbar {1200*SOMA_A} Ik 0 Gk 0 Xpower 2  \
        Ypower 1 Zpower 0

    call K_moll_cs TABCREATE X 30 -0.100 0.050
    settab2const K_moll_cs X_A 0 11 0.0    //-0.1 thru -0.045=>0.0

    // -0.040 Volts
    // -0.030
    // -0.020
    // -0.010
    // 0.0
    // 0.010
    // 0.020
    // 0.030
    // 0.040
    // 0.050
    setfield K_moll_cs X_A->table[12] 0.78 X_A->table[13] 1.23  \
        X_A->table[14] 1.66 X_A->table[15] 2.33 X_A->table[16] 3.12  \
        X_A->table[17] 4.12 X_A->table[18] 5.36 X_A->table[19] 7.24  \
        X_A->table[20] 10.65 X_A->table[21] 16.1 X_A->table[22] 22.5  \
        X_A->table[23] 30.0 X_A->table[24] 34.7 X_A->table[25] 40.8  \
        X_A->table[26] 45.1 X_A->table[27] 47.3 X_A->table[28] 47.6  \
        X_A->table[29] 47.6 X_A->table[30] 47.6

    settab2const K_moll_cs X_B 0 11 3.33    //-0.1 thru -0.045=>33.3

    // -0.040 Volts
    // -0.030
    // -0.020
    // -0.010
    // 0.00
    // 0.010
    // 0.020
    // 0.030
    // 0.040
    // 0.050
    setfield K_moll_cs X_B->table[12] 2.55 X_B->table[13] 2.48  \
        X_B->table[14] 2.44 X_B->table[15] 2.26 X_B->table[16] 2.19  \
        X_B->table[17] 2.06 X_B->table[18] 1.88 X_B->table[19] 1.85  \
        X_B->table[20] 1.85 X_B->table[21] 1.77 X_B->table[22] 1.33  \
        X_B->table[23] 1.25 X_B->table[24] 1.00 X_B->table[25] 0.92  \
        X_B->table[26] 0.36 X_B->table[27] 0.29 X_B->table[28] 0.00  \
        X_B->table[29] 0.00 X_B->table[30] 0.00

		/* Setting the calc_mode to NO_INTERP for speed */
		setfield K_moll_cs X_A->calc_mode 0 X_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweak_tabchan K_moll_cs X

		/* Filling the tables using B-SPLINE interpolation */
		call K_moll_cs TABFILL X 3000 0


		// Note that the h gate is also activating
    call K_moll_cs TABCREATE Y 30 -0.100 0.050
    settab2const K_moll_cs Y_A 0 13 0.0    //-0.1 thru -0.035 => 0.0

    // -0.030	Volts
    // -0.020
    // -0.010
    // 0.00
    // 0.010
    // 0.020
    // 0.030
    // 0.040
    // 0.050
    setfield K_moll_cs Y_A->table[14] 1.11 Y_A->table[15] 2.96  \
        Y_A->table[16] 4.87 Y_A->table[17] 6.96 Y_A->table[18] 10.24  \
        Y_A->table[19] 21.2 Y_A->table[20] 36.4 Y_A->table[21] 58.1  \
        Y_A->table[22] 74.3 Y_A->table[23] 76.8 Y_A->table[24] 94.5  \
        Y_A->table[25] 106.2 Y_A->table[26] 109.3 Y_A->table[27] 110.6  \
        Y_A->table[28] 111.1 Y_A->table[29] 111.1 Y_A->table[30] 111.1

    settab2const K_moll_cs Y_B 0 13 22.2    //-0.1 thru -0.035 => 22.2

    // -0.030	Volts
    // -0.020
    // -0.010
    // 0.00
    // 0.010
    // 0.020
    // 0.030
    // 0.040
    // 0.050
    setfield K_moll_cs Y_B->table[14] 21.1 Y_B->table[15] 19.3  \
        Y_B->table[16] 17.4 Y_B->table[17] 15.3 Y_B->table[18] 12.0  \
        Y_B->table[19] 14.9 Y_B->table[20] 13.7 Y_B->table[21] 13.4  \
        Y_B->table[22] 9.08 Y_B->table[23] 6.50 Y_B->table[24] 5.50  \
        Y_B->table[25] 4.89 Y_B->table[26] 1.78 Y_B->table[27] 0.56  \
        Y_B->table[28] 0.00 Y_B->table[29] 0.00 Y_B->table[30] 0.00

		/* Setting the calc_mode to NO_INTERP for speed */
		setfield K_moll_cs Y_A->calc_mode 0 Y_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweak_tabchan K_moll_cs Y

		/* Filling the tables using B-SPLINE interpolation */
		call K_moll_cs TABFILL Y 3000 0

end

/**********************************************************************
**                 Transient potassium A-Current
**********************************************************************/

function make_KA_moll_cs// A-current

    if ({exists KA_moll_cs})
        return
    end

    create tabchannel KA_moll_cs
    setfield KA_moll_cs Ek {EA} Gbar {1200*SOMA_A} Ik 0 Gk 0 Xpower 4  \
        Ypower 1 Zpower 0

    call KA_moll_cs TABCREATE X 30 -0.100 0.050
    settab2const KA_moll_cs X_A 0 7 0.0    //-0.1 thru -0.065 => 0.0

    // -0.060 Volts
    // -0.050
    // -0.040
    // -0.030
    // -0.020
    // a guess!
    // -0.010
    setfield KA_moll_cs X_A->table[8] 0.1 X_A->table[9] 20.8  \
        X_A->table[10] 38.4 X_A->table[11] 47.2 X_A->table[12] 54.3  \
        X_A->table[13] 60.8 X_A->table[14] 65.7 X_A->table[15] 71.0  \
        X_A->table[16] 74.8 X_A->table[17] 78.0 X_A->table[18] 80.0  \
        X_A->table[19] 83.0
    settab2const KA_moll_cs X_A 20 30 83.3    //0 thru 50 => 83.3


    settab2const KA_moll_cs X_B 0 7 83.3    //-0.1 thru -0.065 => 83.3

    // -0.060 Volts
    // -0.050
    // -0.040
    // -0.030
    // -0.020
    // a guess!
    // -0.010
    setfield KA_moll_cs X_B->table[8] 83.3 X_B->table[9] 62.6  \
        X_B->table[10] 44.9 X_B->table[11] 36.2 X_B->table[12] 29.1  \
        X_B->table[13] 22.6 X_B->table[14] 17.7 X_B->table[15] 12.3  \
        X_B->table[16] 8.5 X_B->table[17] 5.0 X_B->table[18] 3.1  \
        X_B->table[19] 1.2
    settab2const KA_moll_cs X_B 20 30 0.0    //0 thru 0.050 => 0.0


		/* Setting the calc_mode to NO_INTERP for speed */
		setfield KA_moll_cs X_A->calc_mode 0 X_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweak_tabchan KA_moll_cs X

		/* Filling the tables using B-SPLINE interpolation */
		call KA_moll_cs TABFILL X 3000 0


    call KA_moll_cs TABCREATE Y 30 -0.100 0.050
    // -0.100 Volts
    // -0.090
    // -0.080
    // -0.070
    // -0.060
    // -0.050
    setfield KA_moll_cs Y_A->table[0] 4.26 Y_A->table[1] 4.12  \
        Y_A->table[2] 3.95 Y_A->table[3] 3.69 Y_A->table[4] 3.34  \
        Y_A->table[5] 2.94 Y_A->table[6] 2.32 Y_A->table[7] 1.54  \
        Y_A->table[8] 0.66 Y_A->table[9] 0.306 Y_A->table[10] 0.123  \
        Y_A->table[11] 0.053
    settab2const KA_moll_cs Y_A 12 30 0.0    // -0.04 thru 0.05 => 0.0


    // -0.1 Volts
    // -0.090
    // -0.080
    // -0.070
    // -0.060 mV
    // -0.050 mV
    setfield KA_moll_cs Y_B->table[0] 0.00 Y_B->table[1] 0.13  \
        Y_B->table[2] 0.31 Y_B->table[3] 0.57 Y_B->table[4] 0.92  \
        Y_B->table[5] 1.32 Y_B->table[6] 1.93 Y_B->table[7] 2.72  \
        Y_B->table[8] 3.60 Y_B->table[9] 3.95 Y_B->table[10] 4.13  \
        Y_B->table[11] 4.20
    settab2const KA_moll_cs Y_B 12 30 4.26    // -0.04 thru 0.05=> 4.26


		/* Setting the calc_mode to NO_INTERP for speed */
		setfield KA_moll_cs Y_A->calc_mode 0 Y_B->calc_mode 0

		/* tweaking the tables for the tabchan calculation */
		tweak_tabchan KA_moll_cs Y

		/* Filling the tables using B-SPLINE interpolation */
		call KA_moll_cs TABFILL Y 3000 0
end

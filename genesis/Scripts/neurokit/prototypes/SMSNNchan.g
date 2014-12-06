// genesis

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

// CONSTANTS
// (I-current)
float ENa = 0.045
// sq m
float SOMA_A = 1e-9


/*********************************************************************
**                          I-Current (Na)
*********************************************************************/

function make_Na_rat_smsnn// Na current

	// (I-current)
	float ENa = 0.045
	float x, y, dx
	int i
    if (({exists Na_rat_smsnn}))
        return
    end

    create tabchannel Na_rat_smsnn
    setfield Na_rat_smsnn Ek {ENa} Gbar {1200.0*SOMA_A} Ik 0 Gk 0  \
        Xpower 3 Ypower 1 Zpower 0

	call Na_rat_smsnn TABCREATE X 30 -0.1 0.05

    // -0.100 Volts
    // -0.095 Volts
    // -0.090 Volts
    // -0.085 Volts
    // -0.080 Volts
    // -0.075 Volts
    // -0.070 Volts
    // -0.065 Volts
    // -0.060 Volts
    // -0.055 Volts
    // -0.050 Volts
    // -0.045 Volts
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
    setfield Na_rat_smsnn X_A->table[0] 1.0e-4 X_A->table[1] 1.0e-4  \
        X_A->table[2] 1.2e-4 X_A->table[3] 1.45e-4 X_A->table[4] 1.67e-4 \
         X_A->table[5] 2.03e-4 X_A->table[6] 2.47e-4  \
        X_A->table[7] 3.20e-4 X_A->table[8] 3.63e-4  \
        X_A->table[9] 4.94e-4 X_A->table[10] 4.07e-4  \
        X_A->table[11] 4.00e-4 X_A->table[12] 3.56e-4  \
        X_A->table[13] 3.49e-4 X_A->table[14] 3.12e-4  \
        X_A->table[15] 2.83e-4 X_A->table[16] 2.62e-4  \
        X_A->table[17] 2.25e-4 X_A->table[18] 2.03e-4  \
        X_A->table[19] 1.74e-4 X_A->table[20] 1.67e-4  \
        X_A->table[21] 1.31e-4 X_A->table[22] 1.23e-4  \
        X_A->table[23] 1.16e-4 X_A->table[24] 1.02e-4  \
        X_A->table[25] 0.87e-4 X_A->table[26] 0.73e-4  \
        X_A->table[27] 0.80e-4 X_A->table[28] 0.80e-4  \
        X_A->table[29] 0.80e-4 X_A->table[30] 0.80e-4

	x = -0.1
	dx = 0.15/30.0

	for (i = 0; i <= 30; i = i + 1)
		y = 1.0/(1.0 + {exp {-(x + 0.041)/0.0086}})
		setfield Na_rat_smsnn X_B->table[{i}] {y}
		x = x + dx
	end
	tau_tweak_tabchan Na_rat_smsnn X
	setfield Na_rat_smsnn X_A->calc_mode 0 X_B->calc_mode 0
	call Na_rat_smsnn TABFILL X 3000 0


	call Na_rat_smsnn TABCREATE Y 30 -0.1 0.05
   // settab2const(Na_rat_smsnn,Y_A,0,10,6.4e-3)
	//-0.1 thru -0.05=>0.0

    // -0.100 Volts
    // -0.095 Volts
    // -0.090 Volts
    // -0.085 Volts
    // -0.080 Volts
    // -0.075 Volts
    // -0.070 Volts
    // -0.065 Volts
    // -0.060 Volts
    // -0.055 Volts
    // -0.050 Volts
    // -0.045 Volts
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
    setfield Na_rat_smsnn Y_A->table[0] 0.9e-3 Y_A->table[1] 1.0e-3  \
        Y_A->table[2] 1.2e-3 Y_A->table[3] 1.45e-3 Y_A->table[4] 1.7e-3  \
        Y_A->table[5] 2.05e-3 Y_A->table[6] 2.55e-3 Y_A->table[7] 3.2e-3 \
         Y_A->table[8] 4.0e-3 Y_A->table[9] 5.0e-3  \
        Y_A->table[10] 6.49e-3 Y_A->table[11] 6.88e-3  \
        Y_A->table[12] 4.07e-3 Y_A->table[13] 2.71e-3  \
        Y_A->table[14] 2.03e-3 Y_A->table[15] 1.55e-3  \
        Y_A->table[16] 1.26e-3 Y_A->table[17] 1.07e-3  \
        Y_A->table[18] 0.87e-3 Y_A->table[19] 0.78e-3  \
        Y_A->table[20] 0.68e-3 Y_A->table[21] 0.63e-3  \
        Y_A->table[22] 0.58e-3 Y_A->table[23] 0.53e-3  \
        Y_A->table[24] 0.48e-3 Y_A->table[25] 0.48e-3  \
        Y_A->table[26] 0.48e-3 Y_A->table[27] 0.48e-3  \
        Y_A->table[28] 0.48e-3 Y_A->table[29] 0.43e-3  \
        Y_A->table[30] 0.39e-3

	x = -0.1
	dx = 0.15/30.0
	for (i = 0; i <= 30; i = i + 1)
		y = 1.0/(1.0 + {exp {(x + 0.064)/0.0102}})
		setfield Na_rat_smsnn Y_B->table[{i}] {y}
		x = x + dx
	end
	tau_tweak_tabchan Na_rat_smsnn Y
	setfield Na_rat_smsnn Y_A->calc_mode 0 Y_B->calc_mode 0
	call Na_rat_smsnn TABFILL Y 3000 0
end

/*********************************************************************/

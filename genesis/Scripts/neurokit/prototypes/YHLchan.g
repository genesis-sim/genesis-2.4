// genesis

/* FILE INFORMATION
** Rat Na channel, cloned, in expression system.
** Data from :
** Xian-chang Yang, Hsiaolan Hsu and Henry A. Lester. Not yet published.
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

function make_Na_rat_yhl// Na current

	float x, y, dx
	int i
    if (({exists Na_rat_yhl}))
        return
    end

    create tabchannel Na_rat_yhl
    setfield Na_rat_yhl Ek {ENa} Gbar {1200.0*SOMA_A} Ik 0 Gk 0 Xpower 3 \
         Ypower 1 Zpower 0

	call Na_rat_yhl TABCREATE X 49 -0.1 0.1
	x = -0.1
	dx = 0.2/49.0

	for (i = 0; i <= 49; i = i + 1)
		y = 360e3*(x + 0.033)/(1.0 - {exp {-(x + 0.033)/0.003}}) - 400e3*(x + 0.042)/(1.0 - {exp {(x + 0.042)/0.020}})
		setfield Na_rat_yhl X_A->table[{i}] {2.0/y}

		y = 1.0/(1.0 + {exp {-(x + 0.0286)/0.0058}})
		setfield Na_rat_yhl X_B->table[{i}] {y}
		x = x + dx
	end
	tau_tweak_tabchan Na_rat_yhl X
	setfield Na_rat_yhl X_A->calc_mode 0 X_B->calc_mode 0
	call Na_rat_yhl TABFILL X 3000 0

	/*
	setup_tabchan_tau(Na_rat_yhl,X,1.38e-3,0.0,1.0,-0.001,1.0, \
		1.0,0.0,1.0,0.0286,-0.0059)
*/

	call Na_rat_yhl TABCREATE Y 49 -0.1 0.1
	x = -0.1
	dx = 0.2/49.0

	for (i = 0; i <= 49; i = i + 1)
		y = 1e-3*({exp {-(x + 0.0188)/0.0077}} + 0.47)
		setfield Na_rat_yhl Y_A->table[{i}] {y}

		y = 1.0/(1.0 + {exp {(x + 0.03938)/0.00615}})
		setfield Na_rat_yhl Y_B->table[{i}] {y}
		x = x + dx
	end
	tau_tweak_tabchan Na_rat_yhl Y
	setfield Na_rat_yhl Y_A->calc_mode 0 Y_B->calc_mode 0
	call Na_rat_yhl TABFILL Y 3000 0
end

/*********************************************************************/

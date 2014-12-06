//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
/* Parameters for simulations */
/*******************************************************************/
/*                          Generic parameters                     */
/*******************************************************************/
float PI = 3.14159
float RM = 1
float RA = 1
float CM = 1
float EREST_ACT = -60e-3
float	FConst = 96494
float DEFAULT_K1 = 0.1
float DEFAULT_K2 = 0.1
float DEFAULT_K3 = 0.1
float DEFAULT_VOL_CONVERSION = 1	// 1 #/cell = 1 uM
float NA = 6.0e23
float DEFAULT_VOL = DEFAULT_VOL_CONVERSION * 1e3 / NA

/*******************************************************************/
/*                        Sim control parameters                   */
/*******************************************************************/
float	MAXTIME	= 100		// sec
float	TRANSIENT_TIME = 2	// sec

float	SIMDT	= 1e-2		// sec
float	CONTROLDT = 5.0		// sec
float	PLOTDT 	= 1.0		// sec
float	TABLEDT = 0.2		// sec - not used any more, retained for BC
float	FASTDT = 1e-4		// sec: Now converted from a vestigial
				// problem in kkit6 to the dt used in
				// transient calculations.
int		VARIABLE_DT_FLAG = 0 // New in kkit7

int		SIMCLOCK = 0
int		DUMMYCLOCK = 1		// Used to be FASTCLOCK. Now not used,
					// but needed by subsequent clocks.
int		CONTROLCLOCK = 2
int		PLOTCLOCK = 3

int		USE_AUTOMATIC_DT = 0	// Flag for automatic selection of dt
int		INTEGMETHOD = 0		// simple Euler. 

int 	TABLESIZE = 500
int		FAKEPLOTSIZE = 6000

float	STIM_AMPL = 1e-9
float	STIM_DELAY = 5e-2
float	STIM_WIDTH = 1e-2

if (STIM_WIDTH < SIMDT)
	STIM_WIDTH = SIMDT
end

int DEFAULT_TIME_UNITS =	0	// sec
int DEFAULT_CONC_UNITS =	1	// uM

int NO_DIFF_GEOM = 0
int CYL_DIFF_GEOM = 1
int UDOMAIN_DIFF_GEOM = 2
int SPINE_CELL_DIFF_GEOM = 3
int SPINE_CYL_DIFF_GEOM = 4
int MANYSPINE_CYL_DIFF_GEOM = 5

int DIFFGEOM = NO_DIFF_GEOM	// No diffusion

int FIX_ENZ_PARENT_BUG = 0	// Determines whether or not simulator is
							// going to be picky about this bug.
							// Some cspace models use such systems.
/*******************************************************************/
/*                  Select Random Number Generator. Since the      */
/*  command does not even exist in earlier versions of GENESIS, it */
/* has to be commented in or out                                   */
/*******************************************************************/
setrand -sprng

/*******************************************************************/
/*                    Interface control parameters                   */
/*******************************************************************/

int		DO_X = 1	// Determines if we want to use the interface
if (DO_X)
	if ({xgetstat} == 0)
		DO_X = 0
	end
end

int		DO_PSEARCH = 0	// Determines if we want the psearcher

int		FIXCOORDS = 0 // Set only when upgrading from pre-version 8

float	GRID = 1		// Snap-to-grid on by default.

//
// Here we have some parameters for defining screen dimensions
//
// This set of X parms for a 1128x960 screen with title bars
//int WMH = 30 // height of window manager bar on windows
//int WINWID = 1152 // Root window width
//int WINHT = 900 // Root window height
//
// This set of X parms for a 1024x768 screen without title bars
int WMH = 40 // height of window manager bar on windows
int WINWID = 1024 // Root window width
int WINHT = 768 // Root window height

// This set of X parms for a 1280x1024 screen without title bars
// int WMH = 0 // height of window manager bar on windows
// int WINWID = 1280 // Root window width
// int WINHT = 1024 // Root window height

int BORDER = 4 // width of borders on windows


str BASEPATH = "/home2/bhalla/scripts/modules"

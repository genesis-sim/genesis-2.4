// genesis

/*
** $Id: olflib.g,v 1.3 2006/01/17 15:05:00 svitak Exp $
**
** $Log: olflib.g,v $
** Revision 1.3  2006/01/17 15:05:00  svitak
** Changes for voltage-dependent Z gate and new purkinje tutorial.
**
** Revision 1.2  2006/01/10 19:56:39  svitak
** Changes to allow voltage-dependent Z-gate.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.13  2003/03/28 21:17:42  gen-dbeeman
** Fix by Dave Beeman for an error that caused the SUM message for the
** table2D object to fail.
**
** Revision 1.12  1999/10/17 22:31:42  mhucka
** Dave Beeman said that hh_tabchan should be considered a failed experiment,
** and be removed, so I've removed the definition of hh_tabchannel.
**
** Revision 1.11  1998/06/30 23:28:37  dhb
** EDS 22d merge
**
** Revision 1.10  1998/06/30 21:21:58  dhb
** Added the hh_tabchannel object
**
** Revision 1.9.1.1  1998/06/30 23:25:43  dhb
** EDS 22d merge
**
 * EDS22d revison: EDS BBF-UIA 98/04/30-98/05/04
 * Added instantaneous gates to tab(2)channels
 *
** Revision 1.9  1997/07/18 20:34:49  dhb
** Put dddyn object back (was removed in merging in Antwerp changes)
**
** Revision 1.8  1997/06/04 18:01:38  dhb
** Changed name of tabchannel and tab2Dchannel SCALE_GBAR message to
** ADD_BGAR and the solt name from scaling_factor to offset
**
** Revision 1.7  1997/06/03 20:15:13  dhb
** Changed name of 2d table object from table2d to table2D
**
** Revision 1.6  1997/05/29 08:33:49  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS21e revison: EDS BBF-UIA 96/08/02-97/04/16
 * Added SCALE_GBAR msg, updated tabcurrent
 *
 * EDS20j revison: EDS BBF-UIA 95/07/27-95/08/03
 * Added DOMAINCONC msg to tabchannel, tab2Dchannel and tabcurrent
 * Added surface field to tab_current, table2d and tab2channel
 * Cleaned up message fields, added SHOW action to tab2Dchannel. 
 *
 * EDS20i revison: EDS BBF-UIA 95/06/02
 * Added table2d, tab2Dchannel and tabcurrent
 * Added TABSAVE and TABREAD actions and predefined values
 *
 * Revision 1.5  1995/05/13  01:35:12  dhb
 * Changes to support xfileview like functionality using disk_in
 * and xview elements.
 *
 * Revision 1.4  1994/10/04  23:01:01  dhb
 * Added TABDELETE action to tabchannel.
 *
 * Revision 1.3  1994/08/08  22:30:37  dhb
 * Changes from Upi.
 *
 * Revision 1.3  1994/06/13  22:53:56  bhalla
 * Added TAB2FIELDS, DUMP, SHOW and other actions
 *
 * Revision 1.2  1993/02/25  18:50:10  dhb
 * 1.4 to 2.0 command name changes.
 *
 * Revision 1.1  1992/11/14  00:38:47  dhb
 * Initial revision
 *
*/

/*
************************************************************************
**                       OLF SHELL COMMANDS                           **
************************************************************************
*/

// Functions for filling tabchannels and gates with HH-curves
addfunc	setupalpha SetupAlpha
addfunc setuptau SetupTau
addfunc setupgate SetupGate
addfunc setupghk SetupGHK
addfunc setupNaCa SetupNaCa
addfunc tweakalpha TweakAlpha
addfunc tweaktau TweakTau
addfunc scaletabchan ScaleTabchan
addfunc duplicatetable DuplicateTable
addfunc file2tab FileToTab
addfunc tab2file TabToFile
addfunc loadtab LoadTab

addaction TABCREATE 200
addaction TABFILL 201
addaction TABDELETE 202
addaction TABSAVE 203
addaction TABREAD 204
addaction TAB2FIELDS 207
addaction TABOP 208

// Values for the fill modes for the tables
int	B_SPLINE_FILL = 0
int	C_SPLINE_FILL = 1
int	LINEAR_FILL = 2
// Values for calc_mode
int NO_INTERP = 0
int LIN_INTERP = 1
int FIXED = 2
// Values for index fields of tab2Dchannel
int VOLT_INDEX = 0
int C1_INDEX = 1
int C2_INDEX = 2
int DOMAIN_INDEX = 3
int VOLT_C1_INDEX = -1
int VOLT_C2_INDEX = -2
int VOLT_DOMAIN_INDEX = -3
int C1_C2_INDEX = -4
int DOMAIN_C2_INDEX = -5
// Values for instant field of tab(2D)channel
int INSTANTX = 1
int INSTANTY = 2
int INSTANTZ = 4

/*
*************************************************************************
**                              OBJECT TABLE                           **
*************************************************************************
*/
//==========================================================================
//	name		data            function	class	
//==========================================================================

object	receptor	olf_receptor_type OlfReceptor 	segment \
	-author 	"U.S.Bhalla Caltech Jan/90" \
	-actions 	INIT PROCESS RESET CHECK RECALC SAVE2 RESTORE2 \
	-messages 	ODOR 0 	1 strength \		/* type 0: strength */
			GAIN 1	1 sensitivity \	/* type 1: sensitivity */
	-fields		"response[10]	:	weights for each odor" \
			"Vmin,Vmax	:	Range of membrane potentials" \
			"min_rate,max_rate :	range of firing rates" \
	-description	"Does weighted sum of odors to get activity." \
			"Generates both a memb voltage and a spike"

object	receptor2	olf_receptor2_type OlfReceptor2	segment channel \
	-author 	"U.S.Bhalla Caltech Jan/90" \
	-actions 	INIT PROCESS RESET SET CHECK RECALC SAVE2 RESTORE2 \
	-messages 	VOLTAGE		0 	1 Vm \
			ODOR 		1	2 activation odor_no \	
			MODULATION	2	1 modulation \
	-fields		"response[nodors] :	weights for each odor" \
			"nodors		:	Number of odors" \
			"tau1 tau2	:	Time constants of channel" \
			"gmax		:	peak conductance" \
			"Ik		:	channel current" \
			"modulation	:	gain control on responses" \
	-description	"Does weighted sum of odors to get activation" \
			"of a channel with usual conductance properties"

object	disk_in		disk_in_type	DiskIn	 	segment \
	-author 	"U.S.Bhalla Caltech Jan/90" \
	-actions 	INIT PROCESS RESET SET CHECK RECALC SHOW \
	-fields		"filename	:	name of data file" \
			"leave_open	:	leave file open between steps" \
			"nx,ny		:	dimensions of input array" \
			"loop		:	return to start of file on EOF"\
			"val		:	input array" \
			"xpts,ypts,zpts: Coords of points in FMT1 mode" \
	-hidden	tempdata	"temporary buffer for data" \
	-description	"Reads in a 2-d array of data from an ascii file."

/* 
** Modified Jul 1991 by Upi Bhalla to remove integ-method message
*/
object	tabgate		tab_gate_type TabGate	segment gate \
	-author 	"U.S.Bhalla Caltech Feb/90" \
	-actions 	RESET \
			PROCESS \
			TABCREATE \
			TABFILL \
			SET \
			SAVE2 \
			RESTORE2 \
			DUMP \
			*CALC_MINF TabGate_CALC_MINF \
			 *CALC_ALPHA TabGate_CALC_ALPHA \
			 *CALC_BETA TabGate_CALC_BETA \
	-messages 	VOLTAGE		0 	1 voltage \
			SUM_ALPHA	2	1 "sum onto alpha func" \
			PRD_ALPHA	3	1 "multiply onto alpha func" \
			SUM_BETA	4	1 "sum onto beta func" \
			PRD_BETA	5	1 "multiply onto beta func" \
			EREST		6	1 "resting potl for resets" \
	-fields		"m		:	state of gate" \
			"alpha		:	interpol struct for alpha" \
			"beta		:	interpol struct for beta" \
			"instantaneous	:	flag for looking at inst resp" \
	-description	"A gate with tabulated dependencies on any field for" \
			"opening and closing. Voltage and ion conc most" \
			"common fields for dependencies." \
			"The table for the object is created using tabcreate."

object	table		table_type TableFunc	segment \
	-author 	"U.S.Bhalla Caltech Feb/90" \
	-actions 	RESET \
			PROCESS \
			TABCREATE \
			TABFILL \
			TAB2FIELDS \
			TABOP \
			SET \
			SHOW \
			DUMP \
	-messages 	INPUT		0 	1 input \
			SUM		1	1 "sum onto table" \
			PRD		2	1 "multiply onto table" \
			INPUT2		3	2	input index \
	-fields		"input		:	input to table" \
			"output		:	output of table" \
			"table		:	interpol struct for table" \
	-description	"A table with a couple of message slots for adding" \
			"dependencies on other fields." \
			"The table for the object is created using tabcreate."

object  table2D     table2d_type Table2DFunc    segment \
    -author     "U.S.Bhalla MSSM May/93" \
    -actions    RESET \
            	PROCESS \
            	TABCREATE \
            	TABFILL \
            	SET \
    -messages   INPUT       0   2 inputx inputy \
                SUM         1   1 "sum onto table" \
                PRD         2   1 "multiply onto table" \
                X           3   1 "inputx" \
                Y           4   1 "inputy" \
    -fields     "input      :   x input to table" \
                "input2     :   y input to table" \
                "output     :   output of table" \
                "negoutput  :   -output of table" \
                "table      :   2d interpol struct for table" \
    -description    "A 2-d interpolation table " \
            "The table for the object is created using tabcreate." \
            "TABCREATE xdivs xmin xmax [ydivs ymin ymax]"

object	tabchannel		tab_channel_type TabChannel	segment channel \
	-author 	"E. De Schutter Caltech Jan 91" \
	-actions 	CREATE INIT RESET PROCESS CHECK \
			TABCREATE TABFILL TABDELETE \
                	*TABSAVE TabChannel_SAVE \
                	*TABREAD TabChannel_READ \
			SET SHOW \
			SAVE2 RESTORE2 \
			DUMP \
			*CALC_MINF TabChannel_CALC_MINF \
			 *CALC_ALPHA TabChannel_CALC_ALPHA \
			 *CALC_BETA TabChannel_CALC_BETA \
	-messages 	VOLTAGE		0 	1 Vm \
			CONCEN		1	1 C \
			DOMAINCONC	3	2 Ik scaling_factor \
			EK         	4   	1 Ek \
			ADD_GBAR	5	1 offset \
	-fields		"Gk, Ik, Ek, Gbar" \
			"X,Y,Z:	3 gates, X=m, Y=h and Z are optional," \
			" Z is concentration dependent if Z_conc is set (default)." \
			"Z_conc: Indicates Z gate is concentration dependent (default 1)" \
			"surface: surface area of compartment needed for DOMAINCONC" \
			"X_A,X_B,...:	interpol structures" \
			"instantaneous	:	flag for looking at inst resp" \
	-description "channel with tabulated equations" \
			"The table for the object is created using tabcreate," \
			"A-table contains alpha, B-table contains 1/tau," \
			"to minimize calculation time."

object  tab2Dchannel    tab2channel_type Tab2DChannel   segment channel \
    -author     "E. De Schutter, BBF-UIA Aug 94" \
    -actions    INIT RESET PROCESS CHECK \
                TABCREATE SET SHOW SAVE2 RESTORE2 \
                *TABSAVE TabChannel_SAVE \
                *TABREAD TabChannel_READ \
    -messages   VOLTAGE     0   1 Vm \
                CONCEN      1   1 C \
                CONCEN1     1   1 C \
                CONCEN2     2   1 C \
		DOMAINCONC  3	2 Ik scaling_factor \
                EK          4   1 Ek \
		ADD_GBAR  5	1 offset \
    -fields     "Gk, Ik, Ek, Gbar" \
                "X,Y,Z: 3 gates, X=m, Y=h and Z are optional," \
                "Xindex,... determines table type"\
                "Xpower,.. number of gates"\
				"surface: surface area of compartment needed for DOMAINCONC" \
                "X_A,X_B,...:   interpol2D structures" \
                "instantaneous  :   flag for looking at inst resp" \
    -description "channel with 2D tabulated equations" \
            "The table for the object is created using tabcreate," \
            "A-table contains alpha, B-table contains 1/tau," \
            "to minimize calculation time."

object  tabcurrent    tab_current_type TabCurrent   segment channel \
    -author     "E. De Schutter, BBF-UIA Feb 95" \
    -actions    RESET PROCESS CHECK \
                SHOW TABCREATE \
                *TABSAVE TabChannel_SAVE \
                *TABREAD TabChannel_READ \
    -messages   VOLTAGE     0   1 Vm \
                CONCEN      1   1 C \
                CONCEN1     1   1 C \
                CONCEN2     2   1 C \
		DOMAINCONC  3	2 Ik scaling_factor \
		ADD_GBAR  5	1 permeability \
    -fields     "Gk, Ik, Ek, Gbar" \
                "Gindex, Iindex determines table type"\
		"surface: surface area of compartment needed for DOMAINCONC" \
                "G_tab,I_tab:   interpol2D structure" \
    -description "tabulated ionic current." \
		  "This requires values for Ik and for the slope conductance" \
		  "Gk'.  Ek is computed assuming linearity over one timestep." \  
		  "Changes are instantenous.  Can also be used to compute" \
		  "the Goldman-Hodgkin-Katz equation using the ADD_GBAR" \
		  "message to change the calcium permeability." 


object	ddsyn		dd_syn_type DDSyn	segment channel \
	-author 	"Upinder S. Bhalla, Caltech May 91" \
	-actions 	INIT RESET PROCESS CHECK SET \
				TABCREATE TABFILL \
				SAVE2 RESTORE2 \
				DUMP \
	-messages 	VOLTAGE		0 	1 voltage \
				ACTIVATION	1	1 "activation" \
				V_PRESYN	2	1 "presyn voltage" \
				RAND_ACTIVATION	3	2 probability amplitude \
	-fields		"Gk, Ik, Ek, gmax" \
				"tau1 tau2 : time consts of channel activation" \
				"transf:	transform from Vm to activation" \
	-description "dendrodendritic synapse with tabulated transform" \
			"from presyn potential to postsyn activation." \
			"The table for the transform is created using tabcreate" \
			"and is shared with all copies of the element"

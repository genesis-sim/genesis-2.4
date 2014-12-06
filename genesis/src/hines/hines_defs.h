/* Version EDS22h 99/03/10, Erik De Schutter, Caltech & BBF-UIA 4/92-3/99 */

/*
** $Id: hines_defs.h,v 1.2 2006/01/09 16:28:50 svitak Exp $
**
** $Log: hines_defs.h,v $
** Revision 1.2  2006/01/09 16:28:50  svitak
** Increases in size of storage for comparment names. These can be very long
** when cvapp converts from Neurolucida format.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2000/07/03 18:22:25  mhucka
** This file does not need to redefine FARADAY, GAS_CONSTANT and ZERO_CELSIUS.
**
** Revision 1.3  1999/10/17 21:39:37  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
 * EDS22h revison: EDS BBF-UIA 99/03/10-99/03/10
 * Improved concpool code
 *
 * EDS22d revison: EDS BBF-UIA 98/05/04-98/05/05
 * Made instant gates work
 *
 * EDS22c revison: EDS BBF-UIA 97/11/28-98/03/12
 * Made symmetric compartments work
 *
 * EDS22b revison: EDS BBF-UIA 97/11/14-97/11/19
 * Added hillpump and pool defs
 *
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

/* action numbers */
#define DUPLICATE 202
#define HPROCESS 250
#define HSEVENT  251
#define HPUT 300
#define HGET 301
#define HRESTORE 302
#define HSAVE 303
#define HCHANGE 304

#define ERR -1
#define NOT -2

/* array sizes */
#define NAMELEN 1024
#define MAXELMS 4
#define MAXTABC 40	/* max number of conc conductance rate factor tables */
#define MAXTABV 80	/* max number of all conductance rate factor tables */
#define SYNSIZE 4
#define MAXSYN 60	/* max number of synaptic equation tables */
#define MAXNAMES 10000 /* max number of different elm names */
#define MAXPROTOS 100
#define MAXPROTOCOMPS 1000
#define MAXCOMPS 10000

/* msg numbers */
#define CHANNEL 0
#define CHANNEL1 1
#define CHANNEL2 2
#define RAXIAL 1
#define AXIAL 2
#define INJECT 3
#define CONNECTCROSS 5
#define EREST   6
#define INPUT 0
#define THRESH 1
#define SPIKE -1
#define VOLTAGE 0
#define CONCEN 1
#define CONCEN1 1
#define CONCEN2 2
#define DOMAINCONC 3
#define EK 4
#define I_Ca 0
#define fI_Ca 1
#define BASE 4
#define CIN 0
#define COUT 1
#define GVOLTAGE 3
#define PERMEABILITY 4
#define ADD_GBAR 5
#define ACTIVATION 1
#define INFLUX 0
#define OUTFLUX 1
#define FINFLUX 2
#define FOUTFLUX 3
#define STOREINFLUX 4
#define STOREOUTFLUX 5
#define DIFF_DOWN 6
#define DIFF_UP 7
#define BUFFER 10
#define TAUPUMP 11
#define EQTAUPUMP 12
#define MMPUMP 13
#define HILLPUMP 14
#define BCONCEN 0
#define BDIFF_DOWN 1
#define BDIFF_UP 2

/* hget_msg_elm codes */
#define MONE -1
#define NOERR -2

/* float comparisons */
#define TINY 1.0e-6
#define VTINY 1.0e-10
#define MTINY 1.0e-12
#define UTINY 1.0e-30

/* funcs codes */
#define FORWARD_ELIM 0
#define BACKWARD_ELIM 0
#define SET_DIAG 1
#define SKIP_DIAG 2
#define FASTSIBARRAY_ELIM 3
#define COPY_ARRAY 4
#define SIBARRAY_ELIM 5
#define CALC_RESULTS 6
#define FINISH 7

/* childtypes codes */
#define COMPT_T 1
#define NEUTRAL_T 10
#define SPIKEGEN_T 11
#define ELMMPUMP_T 15
#define ANYCHAN_T 2
#define TABCHAN_T 20
#define TAB2CHAN_T 21
#define TABCURR_T 26
#define TABFLUX_T 27
#define SYNCHAN_T 3
#define CHANC2_T 30
#define CHANC3_T 31
#define SYNCHANC_T 32
#define MGBLOCK_T 33
#define NERNST_T 40
#define GHK_T 41
#define TABGHK_T 42
#define CACONCEN_T 50
#define TAUPUMP_T 51
#define MMPUMP_T 52
#define HILLPUMP_T 53
#define FURA2_T 54
#define POOLBUFF_T 55
#define POOL_T 59
#define FIXBUFF_T 60
#define BUFSHELL_T 61
#define BUFSLAB_T 62
#define BUFUSER_T 63
#define BUF2SHELL_T 66
#define BUF2SLAB_T 67
#define BUF2USER_T 68
#define DIFSHELL_T 71
#define DIFSLAB_T 72
#define DIFUSER_T 73
/* childlink codes */
#define NODIFF 0
#define DIFFDOWN 1
#define DIFFUP 2
#define DIFFBOTH 3
#define FLUXOFFSET 10
/* childpos codes */
#define ANYPOOL  -2000
#define ANYSUBSHELL  -1000
#define SUBMEMBRANE  -1
#define SUPRAMEMBRANE 1
#define ANYSUPRASHELL 1000

/* op codes */
#define TABCURR_OP 0
#define POWER_OP 10
#define COMPT_OP 100
#define FCOMPT_OP 101
#define LCOMPT_OP 102
#define SPIKE_OP 200
#define ELMMPUMP_OP 500
#define CONC_VAL_OP 1000
#define NERNST_OP 2000
#define GHK_OP 2010
#define CHAN_OP 3000
#define CHAN_EK_OP 3001
#define SAME_GHK_OP 3002
#define DO_TABGHK_OP 3003
#define ADD_CURR_OP 3200
#define FLUX_OP 3300
#define SCALE_FLUX_OP 3301
#define STORE_ALL_OP 3500
#define STORE_ALLI_OP 3501
#define STORE_ALLC_OP 3502
#define IPOL1V_OP 4001
#define IPOL1C_OP 4002
#define IPOL2V_OP 4003
#define IPOL2C_OP 4004
#define SYN2_OP 4100
#define SYN3_OP 4101
#define MGBLOCK_OP 4200
#define NEWVOLT_OP 5100
#define NEWCONC1_OP 5110
#define NEWCONC2_OP 5111
#define DOMAIN_OP 5112
#define CONC_OP 10100
#define FCONC_OP 10101
#define LCONC_OP 10102
#define FLUXCONC_OP 10103
#define CACONC_OP 10200
#define LCACONC_OP 10202
#define TAUPUMP_OP 11000
#define CTAUPUMP_OP 11001
#define MMPUMP_OP 11010
#define HILLPUMP_OP 11011
#define PHILLPUMP_OP 11012
#define BUFFER_OP 12000
#define POOL_OP 12100

/* iinfo codes */
#define INJECT_OP 1
#define BASE_OP 2
#define THRESH_OP 3

/* readflag codes */
#define HPROTO_T -1		/* prototype hsolve created by readhines */
#define HSETUP_T  0		/* standard initialization by SETUP */
#define HREAD_T   1		/* created and initialized by readhines */
#define HDUPLICATE_T 2	/* copied and initialized by DUPLICATE */


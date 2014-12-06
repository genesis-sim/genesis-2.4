/* Version EDS22d 98/05/05, Erik De Schutter, BBF-UIA 8/94-5/98 */

/*
** $Id: conc_struct.h,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $
** $Log: conc_struct.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2000/07/09 23:37:33  mhucka
** Fix from Erik De Schutter to solve a problem in calculations for diffusion
** uncovered by Anand Chandrasekaran.  The bug involved the calculation of
** surface area of contacts of shells.
**
** EDS22k revison: EDS BBF-UIA 00/07/05
** Checks contact surfaces of shells
**
** Revision 1.4  1999/10/17 22:13:23  mhucka
** New version from Erik De Schutter, dated circa March 1999.
**
** EDS22d revison: EDS BBF-UIA 98/05/05-98/05/05
** Added support for single shells in hsolve
**
** EDS22b revison: EDS BBF-UIA 97/11/14
** Added hillpump_type
**
** Revision 1.2  1997/05/28 21:05:25  dhb
** Complete update from Antwerp GENESIS
**
** Revision 1.1  1992/12/11  19:02:44  dhb
** Initial revision
**
*/

/* Because of its idiosyncrasies difpool_type has been deleted.  
** difshell_type has same functionality and is much more general */

#include "struct_defs.h"

/*
*************************************
**         CONCENTRATION           **
*************************************
*/

typedef struct concpool_type Cpool;
typedef struct difshell_type Dshell;
typedef struct taupump_type Tpump;
typedef struct mmpump_type Mpump;
typedef struct hillpump_type Hpump;
typedef struct fixbuffer_type Fbuffer;
typedef struct difbuffer_type Dbuffer;
typedef struct dif2buffer_type D2buffer;
typedef struct fura2_type Fura2;

#define POOL_TYPE \
	double	C;	\
	double	prev_C;	\
	float	Ceq;	\
	float	val;	\
	float	leak;	\
	short	shape_mode;	\
	short	initialized;	\
	float	len;	\
	float   dia;	\
	float	thick;	\
	double	vol;

struct concpool_type {
	SEGMENT_TYPE
	POOL_TYPE
};

struct difshell_type {
	SEGMENT_TYPE
	POOL_TYPE
	float	D;
	double	surf_up;
	double	surf_down;
};

struct taupump_type {
	SEGMENT_TYPE
	double	kP;
	float	Ceq;
	float	T_A;
	float	T_B;
	float	T_V;
	float	T_C;
};

struct mmpump_type {
	SEGMENT_TYPE
	double  Ik;
	float	vmax;
	float	val;
	float	Kd;
	double	mmconst;
};

struct hillpump_type {
	SEGMENT_TYPE
	float	vmax;
	float	Kd;
	int	Hill;
};

#define FIXBUFFER_TYPE \
	double	Bfree;	\
	double	Bbound; \
	double	prev_free;	\
	double	prev_bound; \
	float	Btot;	\
	float	kBf;	\
	float	kBb;

struct fixbuffer_type {
	SEGMENT_TYPE
	/* computed variables */
	FIXBUFFER_TYPE
};

struct difbuffer_type {
	SEGMENT_TYPE
	/* computed variables */
	FIXBUFFER_TYPE
	/* user parameters */
	float	D;
	short	shape_mode;	/* SHELL, SLAB or USERDEF */
	short	initialized;
	float	len;
	float   dia;
	float	thick;
	/* semi-private parameters */
	double	vol;
	double	surf_up;
	double	surf_down;
};
struct dif2buffer_type {
	SEGMENT_TYPE
	/* computed variables */
	FIXBUFFER_TYPE
	/* user parameters */
	float	Dfree;
	float	Dbound;
	short	shape_mode;	/* SHELL, SLAB or USERDEF */
	short	initialized;
	float	len;
	float   dia;
	float	thick;
	/* semi-private parameters */
	double	vol;
	double	surf_up;
	double	surf_down;
};

struct fura2_type {
	SEGMENT_TYPE
	/* computed variables */
	double  F340;
	double  F380;
	double  ratio;
};


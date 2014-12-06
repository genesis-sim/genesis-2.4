/*
** $Id: pore_struct.h,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
** $Log: pore_struct.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:03:53  dhb
** Initial revision
**
*/

/*************************************************************************/
/*Adam Franklin Strassberg*/
/*March 15, 1992*/
/*Header file pore_ext.h*/
/*************************************************************************/

#include "struct_defs.h"

struct	K_pore_type {
	SEGMENT_TYPE
	float	Vk;
	float	Ek;
	float	Ik;
	double	Gk;
	float	Gmax;
	int	Nt;
	int 	No;
	int 	n0,n1,n2,n3,n4;
	float  alpha_A;
	float  alpha_B;
	float  alpha_C;
	float  alpha_D;
	float  alpha_F;
	float  beta_A;
	float  beta_B;
	float  beta_C;
	float  beta_D;
	float  beta_F;
};

struct	Na_pore_type {
	SEGMENT_TYPE
	float	Vk;
	float	Ek;
	float	Ik;
	double	Gk;
	float	Gmax;
	int	Nt;
	int 	No;
	int 	m0h0,m1h0,m2h0,m3h0;
	int 	m0h1,m1h1,m2h1,m3h1;
	float  malpha_A;
	float  malpha_B;
	float  malpha_C;
	float  malpha_D;
	float  malpha_F;
	float  mbeta_A;
	float  mbeta_B;
	float  mbeta_C;
	float  mbeta_D;
	float  mbeta_F;
	float  halpha_A;
	float  halpha_B;
	float  halpha_C;
	float  halpha_D;
	float  halpha_F;
	float  hbeta_A;
	float  hbeta_B;
	float  hbeta_C;
	float  hbeta_D;
	float  hbeta_F;
};

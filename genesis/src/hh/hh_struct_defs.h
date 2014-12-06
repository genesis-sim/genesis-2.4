/*
** $Id: hh_struct_defs.h,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
** $Log: hh_struct_defs.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1998/06/30 21:07:09  dhb
** Initial revision
**
*/

/*
** NOTE: this is only the HH specific parts of the struct.  The user of
** HH_CHAN_TYPE will need to include the CHAN_TYPE fields as well as
** Gbar, X, Y, Xpower and Ypower
*/

#define HH_CHAN_TYPE \
    short   	X_alpha_FORM; \
    float   	X_alpha_A; \
    float   	X_alpha_B; \
    float   	X_alpha_V0; \
    short   	X_beta_FORM; \
    float   	X_beta_A; \
    float   	X_beta_B; \
    float   	X_beta_V0; \
    short   	Y_alpha_FORM; \
    float   	Y_alpha_A; \
    float   	Y_alpha_B; \
    float   	Y_alpha_V0; \
    short   	Y_beta_FORM; \
    float   	Y_beta_A; \
    float   	Y_beta_B; \
    float   	Y_beta_V0;

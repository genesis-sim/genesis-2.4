/* ncx.c*/
#include "chem_ext.h"

/*
** Avrama Blackwell, GMU, Sept 2002
**
** Voltage and concentration dependent sodium calcium exchanger
** equations from Gall et al. Biophysiscal J. 1999
**
** I = K * Cai^H/(Cai^H + Km,ca^H) * (Vm - Vncx)
** where Vncx = RT/F(stoic*ln(Nao/Nai) - ln(Cao/Cai))
** first term represwents internal calciaum regulation
** second term represents voltage dependence
** this equation provides bestw fit to squid NCX data
**
** Alternative EQuations from Jafri (Luo and Rudy Circ Res 1994)
** I = K* (Cao*Nai^stoich*exp(gamma*FV/RT) - Cai*Nao^stoich*exp((1-gamma*FV/RT))
** divided by (Km,na^stoich+nao^stoich)*(Km,ca+cao)*(1+ksat*exp(1-gamma)FV/RT))
** The numerator is equivalent to DeSchutter formulation
** Jafri eqn still exchibits too much calcium dependence
*/

/* Inputs units:
   Concentration of substance. Must be consistent with Kmca and Kmna

   Output Units: current, depends on Gbar and Vm units
                  , must convert to concentration in rxnpool */

#define	Field(F) (ncx->F)
#define R 8.31 /* j/mol-K */
#define F 96485 /* C/mol */

ncx(ncx,action)
register struct exchange_type *ncx;
Action		*action;
{
MsgIn	*msg;
double	cahill;
double	naocai;
double expgamma,exp1gamma;
double DSterm, denom;
 int ncai, ncae, nnae, nnai, nvm;

 if(debug > 1){
   ActionHeader("NCX",ncx,action);
 }

 SELECT_ACTION(action){
 case INIT:
   ncx->I = 0;
   break;
		
 case PROCESS:
   MSGLOOP(ncx,msg) {
   case 0:	/* cytosolic Ca concentration */
     /* 0 = Conc */
     ncx->Ca_int = MSGVALUE(msg,0);
     break;
   case 1:	/* external Ca concentration */
     /* 0 = Conc */
     ncx->Ca_ext = MSGVALUE(msg,0);
     break;
   case 2:	/* cytosolic Na concentration */
     /* 0 = Conc */
     ncx->Na_int = MSGVALUE(msg,0);
     break;
   case 3:	/* external Na concentration */
     /* 0 = Conc */
     ncx->Na_ext = MSGVALUE(msg,0);
     break;
   case 4:	/* membrane potential */
     /* 0 = Conc */
     ncx->Vm = MSGVALUE(msg,0);
     break;
   }

   if (ncx->ncxtype == 0) {
     /* calcium regulatory term */
     cahill = pow(Field(Ca_int),Field(hill));
     ncx->caterm = cahill/(cahill+ncx->kmhill);
     
     /* effect of concentration on ncx eversal potential */
     if (Field(Na_msg)==1)
       ncx->naterm = ncx->stoich*log(ncx->Na_ext/ncx->Na_int);
     ncx->Vnaca=(ncx->naterm-log(ncx->Ca_ext/ncx->Ca_int))/Field(chi);

     /* total ncx current. Inward current is negative */
     Field(I) = Field(Gbar)*(Field(Vm)-Field(Vnaca))*Field(caterm);
     /* factor of 1/3 because 2/3 of charge compensated by NaKATPase */
     ncx->Gcurrent = ncx->Gbar*(1.0/3.0)*ncx->caterm;
   }
   if (ncx->ncxtype == 1) {
     if (Field(Na_msg) == 1) {
       Field(naterm)=pow(Field(Na_ext),Field(stoich));
       Field(naicao) = pow(Field(Na_int),Field(stoich))*Field(Ca_ext);
     }

     naocai = Field(naterm)*Field(Ca_int);

     expgamma = exp(Field(gamma)*Field(Vm)*Field(chi));
     exp1gamma=exp((Field(gamma)-1)*Field(Vm)*Field(chi));

     DSterm = Field(naicao)*expgamma - naocai*exp1gamma;
     denom = (ncx->naterm+ncx->kmhill)*ncx->caterm*(1+ncx->ksat*exp1gamma);
     Field(I) = Field(Gbar)*DSterm/denom;
   }

   break;

 case RESET:
   /* chi and kmhill are not recalculated during simulation */
   Field(chi)=Field(Vunits)*F/(R*Field(T));

   /* get values of parameters that may change */
   MSGLOOP(ncx,msg) {
   case 0:	/* cytosolic Ca concentration */
     /* 0 = Conc */
     ncx->Ca_int = MSGVALUE(msg,0);
     break;
   case 1:	/* external Ca concentration */
     /* 0 = Conc */
     ncx->Ca_ext = MSGVALUE(msg,0);
     break;
   case 2:	/* cytosolic Na concentration */
     /* 0 = Conc */
     ncx->Na_int = MSGVALUE(msg,0);
     break;
   case 3:	/* external Na concentration */
     /* 0 = Conc */
     ncx->Na_ext = MSGVALUE(msg,0);
     break;
   case 4:	/* membrane potential */
     /* 0 = Conc */
     ncx->Vm = MSGVALUE(msg,0);
     break;
   }

   if (ncx->ncxtype == 0) {
     /* calcium regulatory term */
     Field(kmhill)=pow(Field(Kmca),Field(hill));
     cahill = pow(Field(Ca_int),Field(hill));
     ncx->caterm = cahill/(cahill+ncx->kmhill);
     
     /* effect of concentration on exchange eversal potential */
     ncx->naterm = ncx->stoich*log(ncx->Na_ext/ncx->Na_int);
     ncx->Vnaca=(Field(naterm)-log(Field(Ca_ext)/Field(Ca_int)))/Field(chi);
     
     /* total exchange current. Inward current is negative */
     Field(I) = Field(Gbar)*(Field(Vm)-Field(Vnaca))*Field(caterm);
     /* factor of 1/3 because 2/3 of charge compensated by NaKATPase */
     ncx->Gcurrent = ncx->Gbar*(1.0/3.0)*ncx->caterm;
   }
   if (ncx->ncxtype == 1) {
     Field(caterm) = Field(Ca_ext)+Field(Kmca);
     Field(naterm)=pow(Field(Na_ext),Field(stoich));
     Field(kmhill) = pow(Field(Kmna),Field(stoich));
     naocai = Field(naterm)*Field(Ca_int);
     Field(naicao) = pow(Field(Na_int),Field(stoich))*Field(Ca_ext);

     expgamma = exp(Field(gamma)*Field(Vm)*Field(chi));
     exp1gamma = exp((Field(gamma)-1)*Field(Vm)*Field(chi));
     
     DSterm = Field(naicao)*expgamma - naocai*exp1gamma;
     denom = (ncx->naterm+ncx->kmhill)*ncx->caterm*(1+ncx->ksat*exp1gamma);
     Field(I) = Field(Gbar)*DSterm/denom;
   }

   /* valence needed to convert current ot molecules in rxnpool */
   Field(valence) = 2-Field(stoich); 
/* This is conversion from current to calcium pumped out. 
   If stoich = 3, then 1 ca is pumped out per electron. valence = -1
   If stoich = 4, then 1 ca is pumped out per two electrons. valence = -2
   To keep track of internal sodium concentration, use valence of +1:
   One sodium ion is pumped in per electron */
   break;

 case SET :
   return(0); /* do the normal set */
   break;

 case CHECK :
   ncai = ncae = nnae = nnai = nvm = 0;
   MSGLOOP (ncx,msg) {
   case 0:
     ncai+=1;
     break;
   case 1:
     ncae+=1;
     break;
   case 2:
     nnai+=1;
     break;
   case 3:
     nnae+=1;
     break;
   case 4:
     nvm+=1;
     break;
   }
   if (ncai !=1 || ncae != 1)
     ErrorMessage("ncx", "missing or too many Ca messages", ncx);
   if ((nnai!=1 || nnae !=1) && (Field(Na_ext)<= 0 || Field(Na_int) <=0))
     ErrorMessage ("ncx", "need Na messages or set Na values internally", ncx);
   if (nvm !=1)
     ErrorMessage("ncx", "need 1 and only 1 vm message", ncx);
   break;
 }
}

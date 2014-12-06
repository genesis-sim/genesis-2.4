/* enzyme.c */
#include "chem_ext.h"

/*
** Avrama Blackwell, ERIM 
**
** Modified in 1998, at GMU, to allow for inverted U shaped feedback without
** modeling details of feedback reactions. E.g. effect of Ca on PLC
**
** Modified December 1999 GMU
** 
** A Michaelis-Menton Enzymatic Rxn element. */

/*			   kf1	     kf2
** [Enzyme] + [Substrate] <-> [E-S] -> [Enzyme] + [Product]
**			   kb1
**
**** ASSUME that E-S steady state always,  E = Etot - ES
**
**   dp/dt =  kf1*Etot*S/(S + KM)
**
***** KM = (kb1+kf2)/kf1
**
** Modified March 2002: No longers assumes E-S in steady state
** computes new E-S concentration at each step.
** inputs are substrate and free enzyme concentration.
** outputs are changes to product, enzyme and substrate.
** allows input from rhodopsin object
** 
** A = kfsubs = kf*E*S (used for change in substrate and enzyme conc.)
** B = (kb+kcat)
** dES/dt = A-B*ES
** deltaenz = (kb+kcat)*ES (used for change in enzyme conc.)
** deltaprod = kcat*ES (used for change in product conc. )
** kbprods = kb*ES (used for change in substrate conc.)
**
*/

/* Inputs units:
   Substrate: concentration or quantity, must match kf units
   Half Conc: concentration
   Enzyme: concentration or Quantity, units must match enzyme reaction pool
   Kf: per conc or per quantity * per time
   kb: per time
   kcat: per time
  
   Output units:   quantity, convert to concentration in rxnpool */

#define HYPERBOLIC 0
#define SIGMOIDINC 1
#define SIGMOIDDEC 2
#define AVOGADRO 6.023e23         /* units are molecules per mole */

/* with feedback, decrease in enzyme activity is a function of concentration of
 * feedback substance:  
 *   hyperbolic form: fb propto 1 / conc(feedback substance) for neg feedback
 *                    fb propto conc (feeback substance) for positive feedback.
 *    Sigmoid form: fb = conc/conc + halfmax (for sigmoid increase feedback)
 *                  fb = halfmax / conc + halfmax (for sigmoid decrease feedback)
*/

enzyme(enz,action)
register struct enzyme_type *enz;
Action		*action;
{
  MsgIn	*msg;
  double dt;
  double temp;
  double fb;     /* concentration of feedback substance */
  double subs;       /* substrate concentration */
  double total_rhodopsin; /* total rhodopsin input, includes both free and complexed */
  double A,B;        /* integrate dcomplex/dt = A - B * complex */
  int which;         /* which rate constant is affected */
  double kfmod,kbmod,catmod; /* modification to rate constants */
  int i;
  int nrho, ne, ns, nvol;   /* number of rhodopsin, enzyme and substrate messages */

  if(debug > 1){
    ActionHeader("enz",enz,action);
  }

  SELECT_ACTION(action){
  case INIT:
    if (enz->type == 0)
      enz->previous_state = enz->complex_conc;
    else 
      enz->previous_state = enz->complex_quant;
    break;
    
  case PROCESS:
    dt = Clockrate(enz);
    kfmod=kbmod=catmod=1;
    
    MSGLOOP(enz,msg) 
      {
      case 0:	/* enzyme quantity */
	/* 0 = Concentration of enzyme*/
	enz->conc = MSGVALUE(msg,0);
	if (enz->type == 1) {
	  enz->quant=enz->conc;
	  if (enz->vol > 0)
	    enz->conc = enz->quant/(AVOGADRO*enz->units)/(enz->vol*enz->VolUnitConv);
	  else
	    enz->conc = 0;
	}
	break;

    /***** ignore the following if no feedback **********/

      case 1: /* feedback */
	/* 0 = Conc of feedback substrate */
	/* 1 = which rate const. 0 = kf, 1 = kb, 2 = kcat */
	enz->fbconc = MSGVALUE(msg,0);
	which=MSGVALUE(msg,1);
	enz->rateconst=which;
	 
  	if (debug > 0) 
   	printf (" enzyme: fbconc=%f rateconst=%d\n",enz->fbconc, which);
	
	if ((enz->sign == -1) && (enz->fbconc < enz->thresh)) /* modify rate const if conc < thresh */
	  {
	    if (enz->form == HYPERBOLIC)
	      temp = enz->fbconc/enz->thresh;
	    if (enz->form == SIGMOIDINC)
	      temp = enz->fbconc/(enz->fbconc+enz->halfmax);
	    if (enz->form == SIGMOIDDEC)
	      temp = enz->halfmax/(enz->fbconc+enz->halfmax);
	    enz->feedback=1;
	    for (i=1; i<=enz->pow; i++)
	      enz->feedback = enz->feedback*temp;
	  }
	else if ((enz->sign == 1) && (enz->fbconc > enz->thresh)) /* modify rate const if conc > thresh */
	  {
	    if (enz->form == HYPERBOLIC)
	      temp = enz->thresh/enz->fbconc;
	    if (enz->form == SIGMOIDINC)
	      temp = enz->fbconc/(enz->fbconc+enz->halfmax);
	    if (enz->form == SIGMOIDDEC)
	      temp = enz->halfmax / (enz->fbconc + enz->halfmax);
	    enz->feedback=1;
	    for (i=1; i<=enz->pow; i++)
	      enz->feedback = enz->feedback*temp;
	}
	else
	  enz->feedback = 1;

	
	if (which==0)
	  kfmod=enz->feedback;
	else if (which == 1)
	  kbmod=enz->feedback;
	else
	  catmod=enz->feedback;
  	if (debug > 0) 
   	printf (" enzyme: kfmod=%f kbmod=%f catmod=%f\n",kfmod, kbmod, catmod);
		   
	break;
    /***** end of feedback section  **********/


      case 2: /* substrate, units must match kf */
	/* 0 = Conc or quantity of enzyme substrate */
	subs = MSGVALUE(msg,0);
	break;

      case 3: /* quantity of rhodopsin, with variable volume */
	/* 0 = quantity */
	/* 1 = volume */
	/* 2 = SAin */
	total_rhodopsin = MSGVALUE(msg,0);
	enz->vol=MSGVALUE(msg,1);
	enz->SAin=MSGVALUE(msg,2);
	enz->SAout=MSGVALUE(msg,3);
	if (enz->vol == 0 && enz->conc > 0)
	    ErrorMessage("enzyme", "vol=0 with non zero enzyme conc", enz);
	enz->quant = total_rhodopsin - enz->complex_quant;
	if (enz->vol != 0)
	  enz->conc = enz->quant/(AVOGADRO*enz->units)/(enz->vol*enz->VolUnitConv);
	break;
      case 4:	/* variable volume and area */
	enz->vol= MSGVALUE(msg,0);
	enz->SAin= MSGVALUE(msg,1);
	enz->SAout=MSGVALUE(msg,2);
	break;
      }

/* after getting all messages, compute new concentrations. */
/* need to send messages to enzyme pool (kfsubs and deltaenz) 
   and product pool (deltaprod)  */

    /* A and B are factors for computing complex concentration */
    B=enz->kb*kbmod+enz->kcat*catmod;
    
    if (enz->vol > 0) {
      /* use either quantity or concentration for computation */
      if (enz->type == 0) {
	A=enz->kfsubs=enz->conc*subs*enz->kf*kfmod;
	enz->complex_conc = IntegrateMethod((B < 1.0e-10) ?
				 FEULER_INT : enz->object->method, 
				 enz,enz->complex_conc,A,B,dt,"complex_conc");
	enz->complex_quant = enz->complex_conc*(enz->vol*enz->VolUnitConv)*AVOGADRO*enz->units;
      }
      else {
	A=enz->kfsubs=enz->quant*subs*enz->kf*kfmod;
	enz->complex_quant = IntegrateMethod((B < 1.0e-10) ?
			       FEULER_INT : enz->object->method, 
			       enz,enz->complex_quant,A,B,dt,"complex_quant");
	enz->complex_conc = enz->complex_quant/(enz->vol*enz->VolUnitConv)/(AVOGADRO*enz->units);
      }
    }
    else if (enz->vol == 0) 
      enz->complex_conc=enz->complex_quant=0;
    else
      ErrorMessage("enzyme", "vol<0", enz);

    /* change in product must be in quantity due to different volumes */
    enz->deltacat=enz->kcat*catmod*enz->complex_quant;
    
    /* enzyme change (conc vs quant) must match input and type */
    if (enz->type == 0) {
      enz->kbprod=enz->kb*kbmod*enz->complex_conc;
      enz->deltaenz=B*enz->complex_conc;
    }
    else {
      enz->kbprod=enz->kb*kbmod*enz->complex_quant;
      enz->deltaenz=B*enz->complex_quant;
    }
    break;

  case RESET:
    MSGLOOP(enz,msg) 
      {
       case 3: /* quantity of rhodopsin, with variable volume */
	/* 0 = quantity */
	/* 1 = volume */
	/* 2 = SAin */
	total_rhodopsin = MSGVALUE(msg,0);
	enz->vol=MSGVALUE(msg,1);
	enz->SAin=MSGVALUE(msg,2);
	enz->SAout=MSGVALUE(msg,3);
	break;
      case 4:	/* variable volume and area */
	enz->vol= MSGVALUE(msg,0);
	enz->SAin= MSGVALUE(msg,1);
	enz->SAout=MSGVALUE(msg,2);
	break;
       }
    enz->VolUnitConv = enz->Dunits*enz->Dunits*enz->Dunits;

	  enz->deltacat = 0;
	  enz->deltaenz = 0;
	  enz->kbprod = 0;
	  enz->kfsubs = 0;
	  enz->conc=enz->quant=0;
	  if (enz->vol == 0)
	    enz->complex_conc = enz->complex_quant = 0;
	  else
	    if (enz->type == 0)
	      {
		enz->complex_conc = enz->Cinit;
		enz->complex_quant = enz->complex_conc*(enz->vol*enz->VolUnitConv)*AVOGADRO*enz->units;
	      }
	    else 
	      {
		enz->complex_quant = enz->Qinit;
		enz->complex_conc = enz->complex_quant/(AVOGADRO*enz->units)/(enz->vol*enz->VolUnitConv);
	      }
	  break;

    case CHECK:
      nrho=ne=ns=nvol=0;
      MSGLOOP(enz,msg) {
      case 0:
	ne += 1;
	break;
      case 2:
	ns += 1;
	break;
      case 3:
	nrho += 1;
	break;
      case 4:
	nvol+=1;
	break;
      }
      if (nrho > 1) 
	ErrorMessage("enzyme", "too many rhodopsin messages", enz);
      else if (nrho == 1) {
	if (enz->type == 0)
	  ErrorMessage("enzyme", "must use quantity with variable volume", enz);
	if (ne > 0 || ns != 1)
	  ErrorMessage("enzyme", "incorrect # of enz or subs messages", enz);
	if (nvol > 0)
	  ErrorMessage("enzyme", "can't mix rho and volume messages", enz);
      }
      else {
	if (ne !=1 || ns != 1)
	  ErrorMessage("enzyme", "incorrect # of enz or subs messages", enz);
	if (nvol>1)
	  ErrorMessage("enzyme", "more than 1 volume message", enz);
	else if (nvol==1 && enz->type == 0)
	  ErrorMessage("enzyme", "must use quantity with variable volume", enz);
      }
      break;

  case SET :
    return(0); /* do the normal set */
    break;
  }
}

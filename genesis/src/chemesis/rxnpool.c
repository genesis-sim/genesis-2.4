/* rxnpool.c */
#include "chem_ext.h"

/* Implementation of general 1D or 2D concentration pool. 
**  in-flows/outflows include
**		Vdep calcium currents (measured in amps)
**		IP3R calcium currents (dependent on concentration gradient)
**		pumps to cytosol and ER
**		1-D or 2D diffusion
**		general kinetic reactions
** flows between pools are assumed to be computed in terms of # moles and 
** divided by volume within the pool to get the effect on concentration.  
** flows within a single pool can be performed directly on concentration.
**
** This scheme works fine for single step integration methods,
**	multi-step methods will not be as accurate because changes in
**	variables outside this object (e.g. Bfree in poolbuffers) are
**	invisible to the integrator till the next message */

/*  Avrama Blackwell, GMU, Dec 1997 
    Modified Dec 1999 to  compute quantity
    Modified Mar 2002 to accept molecule quantities, and 
    simplify integration of diffusion and cicr inputs
    this forced a change in messages for mmpump, MM reaction, 
    cicrflux, diffusion; also specify units of concentration and current
    Modified Aug 2013 to fix length units 
*/

#define AVOGADRO 6.023e23         /* units are molecules per mole */
#define FARADAY 96485            /*Faraday = 96485 Coulombs/mole (C/mole)*/
static  double	savedata[2];

int rxnpool(molecule,action)
register struct rxnpool_type *molecule;
Action		*action;
{
double	A,B;		/* integrate dconc/dt = A - B*conc */
double  Aconc, Amoles; /*accumulates inputs in units of molecules or concentration */
double currentflux;
double	newvol, deltavol;  /* used to increment vol with variable volume */
double	dt;
MsgIn 	*msg;
int	n, nvol;   /* number of volume messages */

  if(debug > 1){
    ActionHeader("rxnpool",molecule,action);
  }

  SELECT_ACTION(action){
  case INIT:             /* can perform integration in terms of concentration or quantity */
    if (molecule->type == 0)
      molecule->previous_state = molecule->Conc;
    else 
      molecule->previous_state = molecule->quantity;
    break;

  case PROCESS:
    dt = Clockrate(molecule);
    A = Aconc = B = Amoles = 0;

    /* Read the msgs to get the rxnpool concentration.
       Cases 0 - 2 are in units of conc; no need to divide by vol */
    MSGLOOP(molecule,msg){
      case 0:	/* 0th order rxn */
	/* 0 = A, units must be concentration per millisecond */
	Aconc += MSGVALUE(msg,0);
	break;
      case 1:	/*1st order rxn, e.g. degradation */
	        /* 0 = B, units must be "per millisecond" */
	B += MSGVALUE(msg,0);
	break;
      case 2:	/* interaction with 2nd order reaction (e.g. buffer) */
	        /* Both A and B have units of concentration / second;  
		   Thus, concentration needs to be removed from B field */	
	/* 0 = A */
	/* 1 = B */
	Aconc += MSGVALUE(msg,0);
	if (molecule->Conc != 0)
	  B += MSGVALUE(msg,1)/molecule->Conc;
	break;
      case 3: 	/* interaction with 2nd order reaction (e.g. buffer) */ 
	        /* Differs from case 2 in that reaction in molecule units */
	       /* value of pool1 quantity must be removed from B field*/
	/* 0 = A (pool indep value) */
	/* 1 = B*quantity (pool dep value) */
	Amoles += MSGVALUE(msg,0);
	if (molecule->quantity != 0)
	  B += MSGVALUE(msg,1)/ (molecule->quantity);
	break;
      case 4:	/* current in/outflow */
	        /* current / e * Faraday yields moles; convert to 
		   concentration by dividing by volume */
	/* 0 = charge */
	/* 1 = I */
	/* dQ/dt = I/(e*Faraday) */
	/* Faraday converts I to Coulombs/time units.  If SI units, Iunits=1
	   If using nA (nC/sec) and msec, 
	   I * 1e-12 C-sec / nC-msec
	   convert from mol/sec to molecules/sec with AVOGADRO
	   Also, put minus sign in because neg currents increase Conc*/
	 currentflux = MSGVALUE(msg,1)*molecule->Iunits*AVOGADRO /(MSGVALUE(msg,0)*FARADAY);
	 Amoles -= currentflux;
	break;
      case 5:	/* interaction with mmpump, M-M rxn, diffusion or CICR ;
		   A units: molecules/msec; must divide by volume for conc*/
	/* 0 = A */
	Amoles += MSGVALUE(msg,0);
	break;
      case 6:	/* variable volume and area from rhodopsin object */
	newvol= MSGVALUE(msg,0);
	molecule->SAin= MSGVALUE(msg,1);
	molecule->SAout=MSGVALUE(msg,2);
	deltavol=newvol-molecule->vol;
	molecule->vol=newvol;
	break;
    }

    if (debug > 0)
      printf("%s: Aconc= %g, Amoles= %g, B=%g, currentflux=%g\n", molecule->name,Aconc, Amoles,B,currentflux);

/* After accumulating inflows and outflows, integrate to get new  conc. */
    if (molecule->vol == 0) {
      if (Amoles !=0 || Aconc != 0 )
	ErrorMessage("rxnpool", "vol=0 with non zero influx", molecule);
      else
	molecule->quantity=molecule->Conc=0;
    }
    else {
      if (molecule->type == 0)
	{
	  A = Amoles/(molecule->vol*molecule->VolUnitConv)/(AVOGADRO*molecule->units) + Aconc;
	  molecule->Conc = IntegrateMethod((B < 1.0e-10) ?
				FEULER_INT: molecule->object->method, 
				molecule,molecule->Conc,A,B,dt,"concentration");
	  /* Do not allow Co to get lower tham Cmin, note that this
	  ** limit is only felt indirectly by poolbuffers! */
	  if (molecule->Conc < molecule->Cmin) 
	    molecule->Conc = molecule->Cmin;
	  
	  molecule->quantity = molecule->Conc*(molecule->vol*molecule->VolUnitConv)*AVOGADRO*molecule->units;
	}

      if (molecule->type == 1)
	{
	  A = Amoles + Aconc*(molecule->vol*molecule->VolUnitConv)*AVOGADRO*molecule->units;
	  molecule->quantity = IntegrateMethod((B < 1.0e-10) ?
				     FEULER_INT: molecule->object->method, 
			       molecule,molecule->quantity,A,B,dt,"quantity");
	  if ((deltavol>0) && (molecule->Cinit > 0)) {
	    molecule->quantity += molecule->Cinit*(deltavol)*AVOGADRO*molecule->units;
	  }
	  molecule->Conc = molecule->quantity/(molecule->vol*molecule->VolUnitConv)/(AVOGADRO*molecule->units);

	  /* Do not allow Co to get lower tham Cmin, note that this
	  ** limit is only felt indirectly by poolbuffers! */
	  if (molecule->Conc < molecule->Cmin) 
	    {
	      molecule->Conc = molecule->Cmin;
	      molecule->quantity = molecule->Conc*(molecule->vol*molecule->VolUnitConv)*AVOGADRO*molecule->units;
	    }
	}
    }
    break;

  case RESET:
    MSGLOOP(molecule,msg){
      case 6:	/* variable volume and area from rhodopsin object */
	molecule->vol= MSGVALUE(msg,0);
	molecule->SAin= MSGVALUE(msg,1);
	molecule->SAout=MSGVALUE(msg,2);
	break;
    }
    molecule->VolUnitConv = molecule->Dunits*molecule->Dunits*molecule->Dunits;
    if (molecule->vol == 0)
      molecule->Conc = molecule->quantity = 0;
    else {
      if (molecule->type == 0)
	{
	  molecule->Conc = molecule->Cinit;
	  molecule->quantity = molecule->Conc*(molecule->vol*molecule->VolUnitConv)*AVOGADRO*molecule->units;
	}
      if (molecule->type == 1)
	{
	  if (molecule->Qinit > 0) {
	    molecule->quantity = molecule->Qinit;
	    molecule->Conc = molecule->quantity/(molecule->vol*molecule->VolUnitConv)/(AVOGADRO*molecule->units);
	  }
	  else {
	    molecule->Conc = molecule->Cinit;
	    molecule->quantity = molecule->Conc*(molecule->vol*molecule->VolUnitConv)*AVOGADRO*molecule->units;	 
	  }   
	}
    }
    break;
    
    case CHECK:
      nvol=0;
      MSGLOOP(molecule,msg) {
      case 6:
	nvol += 1;
	break;
      }
      if (nvol > 1)
	ErrorMessage("rxnpool", "too many volume messages", molecule);
      else if (nvol == 1 && molecule->type == 0)
	ErrorMessage("rxnpool", "must use quantity with variable volume", molecule);
      else if ((nvol == 0) && (molecule->vol <= 0.0))
	ErrorMessage("rxnpool", "Invalid Vol.", molecule);

      /* minimum concentration */
      if(molecule->Cmin < 0.0){
	ErrorMessage("rxnpool", "Invalid Cmin.", molecule);
      }
      if(molecule->Cinit < 0.0){
	ErrorMessage("rxnpool", "Invalid Cinit.", molecule);
      }
      if(molecule->Qinit < 0.0){
	ErrorMessage("rxnpool", "Invalid Qinit.", molecule);
      }
      /* length */
      if(molecule->len <= 0.0 || molecule->radius <= 0.0){
	ErrorMessage("rxnpool", "Invalid Length or radius", molecule);
      }
      break;


    case SAVE2:
	savedata[0] = molecule->Conc;
	savedata[1] = molecule->previous_state;
	/* action->data contains the file pointer */
	n=2;
	fwrite(&n,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),2,(FILE*)action->data);
	break;

    case RESTORE2:
	/* action->data contains the file pointer */
	fread(&n,sizeof(int),1,(FILE*)action->data);
	if (n != 2) {
	    ErrorMessage("rxnpool", "Invalid savedata length", molecule);
	    return n;
	}
	fread(savedata,sizeof(double),2,(FILE*)action->data);
	molecule->Conc = savedata[0];
	molecule->previous_state = savedata[1];
	break;

    }

    return(0);
    
}


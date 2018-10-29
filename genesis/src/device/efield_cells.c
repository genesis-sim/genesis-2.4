#include <math.h>
#include "dev_ext.h"
#include  "seg_struct.h"
#include "hines_struct.h"
#define RAXIAL          1
#define AXIAL           2

/*
 *
 * FILE: efield_cells.c
 *
 * This file contains the definition of the 'efield_cells' object and
 * auxilary functions.  This is a clocked object to correctly calculate
 * the Im field of hsolved compartments, and to put the value into the
 * original compartment element that was taken over by hsolve.
 *
 *
 * The object fields (defined in dev_struct.h) are:
 * char *cellpath;  full wildcard path to the group or network of cells,
 * e.g. '/Ex_layer4/pyrcell[]'
 * char *solvepath;  relative path to the hsolve for the cell. e.g. 'solver'
 * double field;
 * float  scale;
 * short debug_level; flag: 0 = no messages, 1 = some messages, 2 = more messages
 * These fields are set on RESET:
 * ElementList *celllist; GENESIS ElementList of cells
 *
 * Author: Dave Beeman, January 2018.
 *
 */

/* Function to return the index into a solver compts[] array that
   corresponds to a given compartment.
*/
int get_comptindex(compt, solver)
  Comp *compt;
  Hsolve *solver;
{
  int j, adj_i;
  for (j=0; j < solver->ncompts; j++) {
    if ((Comp *) (solver->compts[j])==compt) {
        adj_i = j;
        break;
    }
  }
  return(adj_i);
}

int get_symcomptindex(compt, solver)
  Ncomp *compt;
  Hsolve *solver;
{
  int j, adj_i;
  for (j=0; j < solver->ncompts; j++) {
    if ((Ncomp *) (solver->compts[j])==compt) {
        adj_i = j;
        break;
    }
  }
  return(adj_i);
}

/* do_efield_update(efieldelement, fullcellpath, fullsolvepath) is called by
   the efield_cells object PROCESS action.  It performs the actual update of
   a particular cell.
*/

int do_efield_update(efieldelement, fullcellpath, fullsolvepath)
    struct efield_cells_type *efieldelement;
    char *fullcellpath;
    char *fullsolvepath;
{
    short debug_level;
    double  Vm, adj_Vm, Ra, distance;
    double x, y, z;
    double dIm, Im; /* axial currents */
    int i, j, adj_i, ncompts, chanmode, comptindex;
    char *  oname;
    Hsolve *solver;
    MsgIn  *msgin;
    Element *element;

    debug_level = efieldelement->debug_level;
    if (debug_level > 1)
      {
	printf("Checking for existence of %s \n", fullsolvepath);
      }
    /* First, check to see if fullsolvepath points to a hsolve
       with chanmode > 1. If not, use the normal efield element
    */
    if ((element = GetElement(fullsolvepath)) == NULL)
      {
        Error();
        printf("The element %s does not exist.\n", fullsolvepath);
        return(0);
      }
    oname = BaseObject(GetElement(fullsolvepath))->name;
    if (strcmp(oname,"hsolve") != 0)
      {
        Error();
        printf("The solver path %s is not a hsolve element!\n", fullsolvepath);
        return(0);
      }
    
    /* now get the hsolve element for the cell */
    solver = (Hsolve *) GetElement(fullsolvepath);
    chanmode = solver->chanmode;
    if (chanmode < 2)
      {
        Error();
        printf("For hsolve chanmodes 0 or 1, use the normal efield object.\n");
        return(0);
      } 

    x =  efieldelement->x;
    y =  efieldelement->y;
    z =  efieldelement->z;
    
    ncompts = solver->ncompts;
    if (debug_level > 1) {
        printf("full cell path:  %s chanmode = %d\n", fullcellpath, chanmode);
        printf("%d compartments, electrode %s\n", ncompts, efieldelement->name);
    }

    /* Now do actual update of the field from the compartments of cell [n] */
    /* Loop over the compartments */
    for (i=0; i < ncompts; i++)
      {
	double compx, compy, compz;
	double coeff, coeff2;          
	double *compIm;
	const char *compname;

	comptindex = solver->elmnum[i];
	Vm = solver->vm[i];
	dIm = 0.0; /* initialize Im for this compartment */

	/* extract values/locations from the compartment or symcompartment */
	if (!solver->symflag)
	  {
	    Comp *comp = (Comp *) solver->compts[comptindex];
	    Ra = comp->Ra;
	    compx = comp->x;
	    compy = comp->y;
	    compz = comp->z;
	    compname = comp->name;
	    compIm = (double *) &(comp->Im);

	    if (debug_level > 1)
	      {
		printf("comptindex= %d i = %d  hnum[comptindex] =  %d\n",
		       comptindex, i, solver->hnum[comptindex]);
		printf("comp name = %s\n", compname);
		printf("Ra = %g Vm = %f\n", Ra, Vm);
	      }
	    
	    /* Find the adjacent compartments from their 
	       RAXIAL and AXIAL messages 
	    */
	    MSGLOOP(comp, msgin) {
	      case RAXIAL:  /* resistive axial */
		{
		  Comp * adj_compt = (Comp *)(msgin->src);
		  if (debug_level > 1) {
		    printf("RAXIAL adj_compt pathname =  %s\n",
			   Pathname(adj_compt));
		    printf("adj_compt name = %s adj_comp Ra = %g\n",
			   adj_compt->name, adj_compt->Ra);
		  }
		  adj_i = get_comptindex(adj_compt, solver);
		  adj_Vm =  solver->vm[solver->hnum[adj_i]];
		  dIm += (adj_Vm - Vm)/adj_compt->Ra;
		  if (debug_level > 1) {
		    printf("adj_compt i = %d  adj_compt Vm = %f dIm = %g\n",
			   adj_i, adj_Vm, dIm);
		  }
		  break;
		}
		case AXIAL:
		  {
		    Comp * adj_compt = (Comp *)(msgin->src);
		    if (debug_level > 1) {
		      printf("AXIAL adj_compt pathname =  %s\n",
			     Pathname(adj_compt));
		      printf("adj_compt name = %s\n", adj_compt->name);
		    }
		    adj_i =  get_comptindex(adj_compt, solver);
		    adj_Vm =  solver->vm[solver->hnum[adj_i]];
		    dIm += (adj_Vm - Vm)/Ra;
		    if (debug_level > 1) {
		      printf("adj_compt i = %d  adj_compt hnum[i] = %d  "
			     "adj_compt Vm = %f dIm = %g\n",
			     adj_i, solver->hnum[j], adj_Vm, dIm);
		    }
		    break;
		  }
	    } /* MSGLOOP */
            Im = dIm;
            /* store Im back into compartment */
            comp->Im = Im;
	  }
	else /* symcompartments */
	  {
	    Ncomp *comp = (Ncomp *) solver->compts[comptindex];
	    Ra = comp->Ra;
	    compx = comp->x;
	    compy = comp->y;
	    compz = comp->z;
	    compname = comp->name;
	    compIm = (double *) &(comp->Im);
	    coeff = comp->coeff;
	    coeff2 = comp->coeff2;

	    if (debug_level > 1)
	      {
		printf("comptindex= %d i = %d  hnum[comptindex] =  %d\n",
		       comptindex, i, solver->hnum[comptindex]);
		printf("comp name = %s\n", compname);
		printf("Ra = %g Vm = %f\n", Ra, Vm);
	      }

	    /* Find the adjacent compartments from their 
	       RAXIAL and AXIAL messages 
	    */
	    MSGLOOP(comp, msgin)
	      {
		case RAXIAL:  /* resistive axial */
		  {
		    Ncomp * adj_compt = (Ncomp *)(msgin->src);
		    if (debug_level > 1)
		      {
			printf("RAXIAL adj_compt pathname =  %s\n",
			       Pathname(adj_compt));
			printf("adj_compt name = %s adj_comp Ra = %g\n",
			       adj_compt->name, adj_compt->Ra);
		      }
		    adj_i = get_symcomptindex(adj_compt, solver);
		    adj_Vm =  solver->vm[solver->hnum[adj_i]];
		    dIm += (adj_Vm - Vm)/(adj_compt->Ra*coeff);
		    if (debug_level > 1)
		      {
			printf("adj_compt i = %d  adj_compt Vm = %f dIm = %g\n",
			       adj_i, adj_Vm, dIm);
		      }
		  } /* RAXIAL */
		  break;
		  case AXIAL: /* non-resistive axial */
		    {
		      Ncomp * adj_compt = (Ncomp *)(msgin->src);
		      if (debug_level > 1)
			{
			  printf("AXIAL adj_compt pathname =  %s\n",
				 Pathname(adj_compt));
			  printf("adj_compt name = %s\n", adj_compt->name);
			}
		      adj_i =  get_symcomptindex(adj_compt, solver);
		      adj_Vm =  solver->vm[solver->hnum[adj_i]];
		      dIm += (adj_Vm - Vm)/(adj_compt->Ra*coeff2);
		      if (debug_level > 1)
			{
			  printf("adj_compt i = %d  adj_compt hnum[i] = %d  "
				 "adj_compt Vm = %f dIm = %g\n",
				 adj_i, solver->hnum[j], adj_Vm, dIm);
			}
		    } /* AXIAL */
		    break;
		    /* leave out until known how used (symchannels only) */
	    /*  case CONNECTCROSS: */
	      } /* MSGLOOP */
	      Im = dIm;
	      /* store Im back into symcompartment */
	      comp->Im = Im;
	  } /* symcompartments */

        /* update the field potential */
	distance = sqrt(pow(compx - x, 2.0) + pow(compy - y, 2.0) +
			pow(compz - z, 2.0));
	efieldelement->field += Im/distance;
	if (debug_level > 0)
	  {
	    printf("cell %s compartment %s (x, y, z) = (%g  %g %g) \n",
		   fullcellpath, compname, compx, compy, compz);
	    printf("Im = %g  distance = %g Im/distance = %g\n",
		   Im, distance, Im/distance);
	    printf("Stored efieldelement field value = %g\n",
		   efieldelement->field);
	  }
      } /* loop over compartments */
    
    if (debug_level > 0)
      {
	printf("End loop over compartments for %s Field for this cell = %g\n",
	       fullcellpath, efieldelement->field);
      }
    return(1);
}


int EfieldCells(efieldelement, action)
    struct efield_cells_type *efieldelement;
    Action *action;
    {
    int    n;
    int    status;  /* Return value: 1 is OK, 0 means an error occurred. */
    short  debug_level;
    char *cellpath, *solvepath;
    ElementList *celllist;
    Hsolve      *solver;
   /* Instead of using calloc() assume 255 char max */
   char fullcellpath[256];
   char fullsolvepath[256];
   short output_len;

    SELECT_ACTION(action)
    {
    case CREATE:
        efieldelement->debug_level = 0;
        efieldelement->field = 0;
        efieldelement->scale = 0.32;
        break;

    case INIT:
        break;

    case RESET:
        efieldelement->field = 0;
        cellpath = efieldelement->cellpath;
        efieldelement->celllist = WildcardGetElement(cellpath, 1);
        break;

    case PROCESS:
        cellpath = efieldelement->cellpath;
        solvepath = efieldelement->solvepath;  /* relative path from cell */
        celllist = efieldelement->celllist;
        debug_level = efieldelement->debug_level;
        /* First, reset the field to zero at each step,
           then, loop over all cells */
        efieldelement->field = 0.0;
        if (debug_level > 0)
          {

          printf("Number of cells = %d\n", celllist->nelements);
          }
        for (n = 0; n <  celllist->nelements; n++)
        {

          /* Turn the element list into strings   */
          strncpy(fullcellpath, Pathname(celllist->element[n]), 255);
          output_len = snprintf(fullsolvepath, 255, "%s/%s", fullcellpath, solvepath);

          if (debug_level > 0) {
                printf("cell number = %d\n", n);
                printf("time = %f\n", SimulationTime());
                printf("full cell path = %s\n",fullcellpath );
                printf("full solver path = %s\n",fullsolvepath );
                printf("relative solver path = %s\n",solvepath );
            }
          do_efield_update(efieldelement, fullcellpath, fullsolvepath);

        } /* loop over cells */
        efieldelement->field *= efieldelement->scale;
        if (debug_level > 0) {
            printf("------------------------------\n");
            printf("time = %f  field = %g\n", SimulationTime(), efieldelement->field);
          }

    break;

    case CHECK:
        /* should warn if path fields are not properly initialized */
        break;

    }
    return (1);
    }


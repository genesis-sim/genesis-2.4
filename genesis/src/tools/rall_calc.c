static char rcsid[] = "$Id: rall_calc.c,v 1.2 2005/07/01 10:03:10 svitak Exp $";

/*
** $Log: rall_calc.c,v $
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2000/07/02 23:03:29  mhucka
** An "#endif RAXIAL" caused some compilers to complain it's non-ISO.
**
** Revision 1.6  2000/06/12 04:58:22  mhucka
** Added typecasts where appropriate.
**
** Revision 1.5  1997/07/18 03:02:36  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.4  1995/03/15 01:38:32  dhb
** calcCm, calcRm and rallcalcRm were all returning char* and would
** return a static string in error cases which resulted in script
** code trying to free the string.  Changed the return types for
** these commands to float, since they always returned a float value
** as a char string.
**
 * Revision 1.3  1993/02/24  16:54:48  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * Revision 1.2  1992/10/29  19:42:16  dhb
 * Replaced explicit msgin looping with MSGLOOP macro.
 *
 * Revision 1.1  1992/10/29  19:39:37  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <math.h>
#include "sim_ext.h"
#include "seg_struct.h"
#ifndef	RAXIAL
#define	RAXIAL 1
#endif
/*
**		rall_calc : By Upinder S. Bhalla May 1990
** 
** This file contains various routines for calculating cell parameters.
** The input resistance can be calculated directly from the parameters
** of the compartments. The value of Rm can be calculated form the 
** input resistance (experimental) of the cell assuming
** that all of the parameters in the compartments have been calculated
** using the same Rmest. 
** The values for input resistance and Rm are a few (generally < 5%)
** percent off the values you would get by dividing the Voltage change
** (found by running the simulation)
** by the injected current for the passive membrane. This is because
** this calculation uses Rall's analytic solution for the dendritic
** tree, whereas the simulation uses discrete compartments. The error
** increases if you use large compartments, and is around 3 % for 
** 10 compartments per length constant. The error may be reduced by
** terminating the dendritic branches with a 2x higher Ra, when using
** symmetric compartments.
**
** These calculations are based on the procedure outlined
** in 'Electric current flow in excitable cells' by Jack, Noble and
** Tsien, OUP, 1983, and Rall, Biophys J, Vol 9 1969 pp1483
**
** Formulae used :

---------------------------------------------------------------------

0 - Calculating Bj

Bj = SIGMAk[Bjk*( {(rm*ri)|(j-1)}/{(rm*ri)|(jk)} )^0.5]
where 

Bjk = ( B(j+1)k + tanh({(ri/rm)|jk}^0.5) ) /
		( 1 +  B(j+1)k * tanh({(ri/rm)|jk}^0.5) )

{(ri/rm)|jk} is ri/rm evaluated for compartment jk
B(j+1)k is B indexed at (j+1)k

---------------------------------------------------------------------

1 - input cond of cell

Gn = 1/rmsoma + SIGMAj(Boj * (1/(rmoj * rioj))^0.5)

Where
	Gn = input conductivity of cell : to be calculated.
	rmsoma = membrane res of cell
	SIGMA is the summation symbol, summation is over j th dend branch
	Boj is the B for the jth dend branch off the soma.
	rmoj is the memb res for the jth branch compt off the soma
	rioj is axial res for the jth branch compt off the soma

---------------------------------------------------------------------

2 - Value of Rm from input cond of cell.

Rm = Rmest/(Beta^2)

where 

Beta = {-A + ( A^2 + 4 * Gn/rmsoma)^0.5 } / {2 / rmsoma}
A = SIGMAj(Boj * (1/(rmoj * rioj))^0.5)
Gn = input cond of cell, measured
Rmest = Estimated value for Rm
rmsoma = membrane res of soma based on Rmest
all values of   rm = membrane res of compt, based on Rmest.

---------------------------------------------------------------------
*/

float calcBj();
float calcA();
float calcGn();
float gcompt();

float calcBj(elm,lastrmri)
	struct compartment_type *elm;
	float lastrmri;
{
	struct compartment_type *child;
	float Bj = 0.0;
	float Bjk,nextBj;
	float ri,rm,tr,rmri;
	MsgIn	*msg;

	ri = elm->Ra;
	rm = elm->Rm;

	rmri = ri * rm;

	MSGLOOP(elm, msg) {
	    case RAXIAL:
		{
			child = (struct compartment_type *)(msg->src);
			ri = child->Ra;
			rm = child->Rm;
			tr = tanh(sqrt(ri/rm));
			nextBj = calcBj(child,rm * ri);
			Bjk = (nextBj + tr) / (1 + nextBj * tr);
			Bj = Bj + Bjk * sqrt(((lastrmri) / (rm * ri)));
		}
		break;
	}

	return(Bj);
}

float calcA(soma)
	struct compartment_type *soma;
{
	float A = 0.0;
	float Boj;
	float somarmri,somarm;
	float rm,ri,tr;
	struct compartment_type *child;
	MsgIn	*msg;

	somarm = soma->Rm;
	somarmri = somarm * soma->Ra;

	MSGLOOP(soma, msg) {
	    case RAXIAL:
		{
			child = (struct compartment_type *)(msg->src);
			ri = child->Ra;
			rm = child->Rm;
			tr = tanh(sqrt(ri/rm));
			Boj = calcBj(child,rm * ri);
			Boj = (Boj + tr) / (1.0 + Boj * tr);
			A += Boj * (sqrt(1.0/(rm * ri)));
		}
		break;
	}
	return(A);
}

float calcGn(soma)
	struct compartment_type *soma;
{
	float Gn;

	Gn = calcA(soma) + 1.0 / soma->Rm;

	return(Gn);
}

/*
** calcRm assumes that the cell was set up with RM as memb resistivity
** I do not really understand what I was doing here.
*/
float calcRm(soma,Gn,RM)
	struct compartment_type *soma;
	float Gn;
	float RM;
{
	float Rm;
	float Beta,A;
	float rmsoma;

	rmsoma = soma->Rm;

	A = calcA(soma);
	Beta = (-A + sqrt( A * A + 4.0 * Gn/rmsoma)) / (2.0 / rmsoma);
	
	Rm = RM/(Beta * Beta);
	return(Rm);
}

float do_sumCm(argc,argv)
	int		argc;
	char	**argv;
{
	float cap = 0.0;
	ElementList	*elmlist, *WildcardGetElement();
	char	path[80];
	char	*parent;
	int		i;

	initopt(argc, argv, "[parent-element]");
	if (G_getopt(argc, argv) != 0 || optargc > 2)
	  {
	    printoptusage(argc, argv);
	    return 0.0;
	  }

	if (optargc == 2)
		parent = argv[1];
	else
		parent = ".";

	sprintf(path,"%s/##[TYPE=compartment]",parent);
	if (!(elmlist = WildcardGetElement(path,0))) {
		printf("no elements found on '%s'\n",path);
		return 0.0;
	}
	for (i = 0 ; i < elmlist->nelements ; i++)
		cap += ((struct compartment_type *)(elmlist->element[i]))->Cm;
	return cap;
}

float do_rall_calc_Rm(argc,argv)
	int		argc;
	char	**argv;
{
	float Gn;
	Element	*soma;

	initopt(argc, argv, "soma-element");
	if (G_getopt(argc, argv) != 0)
	  {
		printoptusage(argc, argv);
		return 0.0;
	  }

	if (!(soma = GetElement(optargv[1]))) {
		printf("element '%s' not found\n",optargv[1]);
		return 0.0;
	}
	Gn = calcGn((struct compartment_type *) soma);
	return Gn;
}


float gcompt(elm)
	struct compartment_type *elm;
{
	Element *child;
	float gc;
	MsgIn	*msg;

	gc = 1/(elm->Rm);

	MSGLOOP(elm, msg) {
	    case RAXIAL:
		{
			child = msg->src;
			gc += gcompt((struct compartment_type *) child);
		}
		break;
	}
	return(1.0/(elm->Ra+1.0/gc));
}

float gsoma(soma)
	struct compartment_type *soma;
{
	Element *child;
	float gc;
	MsgIn	*msg;

	gc = 1/(soma->Rm);

	MSGLOOP(soma, msg) {
	    case RAXIAL:
		{
			child = msg->src;
			gc += gcompt((struct compartment_type *) child);
		}
		break;
	}
	return(gc);
}


float do_calc_Rm(argc,argv)
	int		argc;
	char	**argv;
{
	float Gn;
	Element	*soma;

	initopt(argc, argv, "soma-element");
	if (G_getopt(argc, argv) != 0)
	  {
		printoptusage(argc, argv);
		return 0.0;
	  }

	if (!(soma = GetElement(optargv[1]))) {
		printf("element '%s' not found\n",optargv[1]);
		return 0.0;
	}
	Gn = gsoma((struct compartment_type *) soma);
	return Gn;
}



#undef	RAXIAL

/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
static char rcsid[] = "$Id: gsolve.c,v 1.5 2005/08/12 09:38:17 svitak Exp $";

/*
** $Log: gsolve.c,v $
** Revision 1.5  2005/08/12 09:38:17  svitak
** Switched comment style from slashslash to slashstar.
**
** Revision 1.4  2005/07/20 20:02:00  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.3  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/20 21:08:31  svitak
** Changed sprng() calls to G_RNG(). This is so the kinetics code can still
** be compiled even if sprng cannot.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2003/05/30 22:25:25  gen-dbeeman
** New version of kinetics library from Upi Bhalla, April 2003
**
*
*/

/*
 * Boilerplate-copied from ./pool.c, ideas from ./{pool,reac,ksolve}.c
 */

#include <float.h>
#include <math.h>
#include "kin_ext.h"
/* #define SIMPLE_SPRNG
#include "sprng.h" */
#include "string.h"

#define POOL_SUMTOTAL 0x01
#define POOL_CONSERVE 0x02
#define POOL_NSLAVE 0x01
#define POOL_CONCSLAVE 0x02
#define POOL_BUFFER 0x04

#ifdef __STDC__
static void gsolve_do_reset (struct gsolve_type *gsolve);
static void check_msg_type (struct gsolve_type *gsolve, MsgIn * msg);
void pq_insert(double, int);
void pq_swap(int, int);
void pq_update(int, double);
void pq_update_aux(int);
int add_dep_list(int, int);
/*
 * static void do_oldmsgmode (struct gsolve_type *gsolve, double *A,
 * double *B, double *conserve, double *sumtotal); 
 */
#else
static void gsolve_do_reset ();
static void check_msg_type ();
void pq_insert();
void pq_swap();
void pq_update();
void pq_update_aux();
int add_dep_list();
/*
 * static void do_oldmsgmode(); 
 */
#endif

#if 0
static long forward_reac_num = 0;
static long backward_reac_num = 0;
#endif

#if 0
typedef struct node_type {
	int index;
	struct node_type *next;
} node;
#endif

double propensity_trans[1900]; /* For Gibson-Bruck, stored in this
       flattened array instead of gsolve->prop*{reac,enz}. Not quite
       elegant. We still use gsolve->propensity_total though */

unsigned short dep_graph_adj[1900][1901];    /* adjacency list representation of
       dependency graph. [i][1900] store (in succession) indices of
       dependent transitions. [i][1901] stores the number of these. */
typedef struct priority_queue_node_type {
  double time;
  int transition_index;        /* priority queue -> transition index */
} pq_node;
pq_node pq_nodes[1900];   /* array representation of priority queue */
int pq_size;
int transition_pq_index[1900]; /* transition -> priority queue index */

double propensity_trans_zero[1900]; 
double time_zero[1900]; 
double trans_time_zero[1900];

void GsolveCalculateTotalPropensity(struct gsolve_type *gsolve) {
  int i, j;
  double **data = NULL;
  struct reac_type *reac = NULL;
  struct enz_type *enz = NULL;
	
  gsolve->propensity_total = 0;
	    
  if (gsolve->reac_list_populated)
  {
    for (i = 0; i < gsolve->reac_list->nelements; i++)
    {
      reac = (struct reac_type *) gsolve->reac_list->element[i];
      data = reac->msgdata; 
      gsolve->propensity_reac[0][i] = reac->kf; 
      for (j = 0; j < reac->msgcount[0]; j++, data++)
      	{
	gsolve->propensity_reac[0][i] *= **data;
      	}
/* Skip multiplication by dt as this is a relative choice algorithm */
      gsolve->propensity_reac[1][i] = reac->kb;
      for (j = 0; j < reac->msgcount[1]; j++, data++)
	{
	  gsolve->propensity_reac[1][i] *= **data;
	}
      gsolve->propensity_total +=
	gsolve->propensity_reac[0][i] + gsolve->propensity_reac[1][i];
    }
  }
  /* build up propensity_enz[][] and propensity_total */
  if (gsolve->enz_list_populated)
  {
    for (i = 0; i < gsolve->enz_list->nelements; i++)
    {
      enz = (struct enz_type *) gsolve->enz_list->element[i];
      data = enz->msgdata;
      for (j = 0; j < enz->msgcount[0]; j++, data++)
	gsolve->propensity_enz[0][i] = **data;
      if (enz->usecomplex)
	gsolve->propensity_enz[0][i] -= enz->nComplex;
      if (gsolve->propensity_enz[0][i] < 1)
	gsolve->propensity_enz[0][i] = 0;
      for (j = 0; j < enz->msgcount[1]; j++, data++)
	gsolve->propensity_enz[0][i] *= **data;
      gsolve->propensity_enz[0][i] *= enz->k1;

      gsolve->propensity_enz[1][i] = enz->k2 * enz->nComplex;
      if (enz->nComplex < 1)
	gsolve->propensity_enz[1][i] = 0;
		  
      gsolve->propensity_enz[2][i] = enz->k3 * enz->nComplex;
      if (enz->nComplex < 1)
	gsolve->propensity_enz[2][i] = 0;
		  
      gsolve->propensity_total +=
	gsolve->propensity_enz[0][i] + gsolve->propensity_enz[1][i] +
      gsolve->propensity_enz[2][i];
    }
  }
}

void GsolveUpdateReacMolecules(struct gsolve_type *gsolve,
		struct reac_type* reac_chosen, 
		int reac_chosen_direction
		)
{

	int j;
	double **data = reac_chosen->msgdata;
	int **se = reac_chosen->se;
	switch (reac_chosen_direction)
	  {
	  case 1:		/* forward */
	    for (j = 0; j < reac_chosen->msgcount[0]; j++, data++, se++)
	      if (**se == 0)
		**data -= 1;	/* TYPE = SUBSTRATE */
	    for (j = 0; j < reac_chosen->msgcount[1]; j++, data++, se++)
	      if (**se == 0)
		**data += 1;	/* TYPE = PRODUCT */
	    break;
	  case -1:		/* backward */
	    for (j = 0; j < reac_chosen->msgcount[0]; j++, data++, se++)
	      if (**se == 0)
		**data += 1;	/* TYPE = SUBSTRATE */
	    for (j = 0; j < reac_chosen->msgcount[1]; j++, data++, se++)
	      if (**se == 0)
		**data -= 1;	/* TYPE = PRODUCT */
	    break;
	  default:
	    break;
	  }
}

void GsolveUpdateEnzMolecules(struct gsolve_type *gsolve,
		struct enz_type* enz_chosen, 
		int enz_chosen_direction
		)
{
  double orig_nenz = 0.0;
  double **data = enz_chosen->msgdata;
  int** se = enz_chosen->se;
  int j;

  switch (enz_chosen_direction)
    {
    case 1:		/* E + S -----> E.S */
      enz_chosen->nComplex++;
      if (enz_chosen->usecomplex) {
        data += enz_chosen->msgcount[0];
        se += enz_chosen->msgcount[0];
      }
      else
        for (j = 0; j < enz_chosen->msgcount[0]; j++, data++, se++)
  	if (**se == 0)
  	  **data -= 1;
      for (j = 0; j < enz_chosen->msgcount[1]; j++, data++, se++)
        if (**se == 0)
  	**data -= 1;
      /*		    enz_chosen->pA = 0; */
      break;
      
    case 2:		/* E.S -----> E + S */
      enz_chosen->nComplex--;
      if (enz_chosen->usecomplex) {
        data += enz_chosen->msgcount[0];
        se += enz_chosen->msgcount[0];
      }
      else
        for (j = 0; j < enz_chosen->msgcount[0]; j++, data++, se++)
  	if (**se == 0)
  	  **data += 1;
      for (j = 0; j < enz_chosen->msgcount[1]; j++, data++, se++)
        if (**se == 0)
  	**data += 1;
      /*		    enz_chosen->pA = 0; */
      break;
      
    case 3:		/* E.S -----> E + P */
      enz_chosen->nComplex--;
      if (!enz_chosen->usecomplex) {
        for (j = 0; j < enz_chosen->msgcount[0]; j++, data++, se++)
  	if (**se == 0)
  	  **data += 1;
      } else {
        data += enz_chosen->msgcount[0];
        se += enz_chosen->msgcount[0];
      }
      data += enz_chosen->msgcount[1] + enz_chosen->msgcount[2] + enz_chosen->msgcount[3];
      se += enz_chosen->msgcount[1];
      for (j = 0; j < enz_chosen->msgcount[4]; j++, data++, se++)
        if (**se == 0)
  	**data += 1;
      /*		    enz_chosen->pA = 1 / dt; */
      break;
      
    default:
      break;
    }
  
  if (enz_chosen->nComplex < 0)
    enz_chosen->nComplex = 0;
  if (enz_chosen->usecomplex)
    {
      data = enz_chosen->msgdata;
      for (j = 0; j < enz_chosen->msgcount[0]; j++, data++)
        orig_nenz = **data;
      if (enz_chosen->nComplex > orig_nenz)
        enz_chosen->nComplex = orig_nenz;
    }
  enz_chosen->CoComplex = enz_chosen->nComplex / enz_chosen->vol;
}

void GsolveMethod1(struct gsolve_type *gsolve)
{
  double deviate_transition = 0.0;
  double transition_time = 0.0;
  struct reac_type *reac_chosen = NULL;
  struct enz_type *enz_chosen = NULL;
  int reac_chosen_direction = 0;	/* 1 or -1*/
  int enz_chosen_direction = 0;	/* 1, 2 or 3 */
  double prop_acc = 0.0;
  int i;

  deviate_transition = G_RNG()  * gsolve->propensity_total;
  /* scale deviate to compare against propensity coeffs */
		
  /*
  * choose reaction or enzyme transition, inelegantly 
  */
  for (i = 0; i < gsolve->reac_list->nelements; i++)
  {
    if (deviate_transition <=
	(prop_acc + gsolve->propensity_reac[0][i]))
    {
      reac_chosen = (struct reac_type *) gsolve->reac_list->element[i];
      reac_chosen_direction = 1;	/* forward */
	break;
    } else {
       prop_acc += gsolve->propensity_reac[0][i];
    }
		    
    if (deviate_transition <=
	(prop_acc + gsolve->propensity_reac[1][i]))
    {
      reac_chosen = (struct reac_type *) gsolve->reac_list->element[i];
      reac_chosen_direction = -1;	/* backward */
      break;
    } else {
      prop_acc += gsolve->propensity_reac[1][i];
    }
  }
  if (!reac_chosen)
  {
    for (i = 0; i < gsolve->enz_list->nelements; i++)
    {
      if (deviate_transition <=
		      (prop_acc + gsolve->propensity_enz[0][i]))
      {
        enz_chosen = (struct enz_type *) gsolve->enz_list->element[i];
	enz_chosen_direction = 1;
	break;
      } else {
        prop_acc += gsolve->propensity_enz[0][i];
      }

      if (deviate_transition <=
	  (prop_acc + gsolve->propensity_enz[1][i]))
      {
         enz_chosen = (struct enz_type *) gsolve->enz_list->element[i];
	 enz_chosen_direction = 2;
	 break;
      } else {
         prop_acc += gsolve->propensity_enz[1][i];
      }
		      
      if (deviate_transition <=
			  (prop_acc + gsolve->propensity_enz[2][i]))
      {
        enz_chosen = (struct enz_type *) gsolve->enz_list->element[i];
        enz_chosen_direction = 3;
        break;
      } else {
        prop_acc += gsolve->propensity_enz[2][i];
      }
    }
  }
  /* choose time and update current time */
  transition_time = -1 * log (G_RNG()) / gsolve->propensity_total;
  gsolve->internal_time += transition_time;

  if (reac_chosen)
    GsolveUpdateReacMolecules(
		    gsolve, reac_chosen, reac_chosen_direction);
  if (enz_chosen)
    GsolveUpdateEnzMolecules(
		    gsolve, enz_chosen, enz_chosen_direction);
}

void GsolveMethod2(struct gsolve_type *gsolve)
{
  double transition_time = 0.0;
  struct reac_type *reac_chosen = NULL;
  struct enz_type *enz_chosen = NULL;
  int reac_chosen_direction = 0;	/* 1 or -1*/
  int enz_chosen_direction = 0;	/* 1, 2 or 3 */
  int r_or_e_type = 0, r_or_e_index = 0, r_or_e_direction = 0;
  double minimum_time = DBL_MAX;
  double time;
  int i;

  for (i = 0; i < gsolve->reac_list->nelements; i++)
    {
      if (gsolve->propensity_reac[0][i]) {
        time = -1 * log (G_RNG()) / gsolve->propensity_reac[0][i];
        if (time < minimum_time)
  	{
  	  minimum_time = time;
  	  r_or_e_type = 1;	/* reac chosen */
  	  r_or_e_index = i;
  	  r_or_e_direction = 1;	/* forward */
  	}
      } else
        time = DBL_MAX;

      if (gsolve->propensity_reac[1][i]) {
        time = -1 * log (G_RNG()) / gsolve->propensity_reac[1][i];
        if (time < minimum_time)
  	{
  	  minimum_time = time;
  	  r_or_e_type = 1;	/* reac chosen */
  	  r_or_e_index = i;
  	  r_or_e_direction = -1;	/* backward */
  	}
      } else
        time = DBL_MAX;
    }
  
  for (i = 0; i < gsolve->enz_list->nelements; i++)
    {
      if (gsolve->propensity_enz[0][i]) {
        time = -1 * log (G_RNG()) / gsolve->propensity_enz[0][i];
        if (time < minimum_time)
  	{
  	  minimum_time = time;
  	  r_or_e_type = 2;	/* enz chosen */
  	  r_or_e_index = i;
  	r_or_e_direction = 1;
  	}
      } else
        time = DBL_MAX;

      if (gsolve->propensity_enz[1][i]) {
        time = -1 * log (G_RNG()) / gsolve->propensity_enz[1][i];
        if (time < minimum_time)
  	{
  	  minimum_time = time;
  	  r_or_e_type = 2;	/* enz chosen */
  	  r_or_e_index = i;
  	  r_or_e_direction = 2;
  	}
      } else
        time = DBL_MAX;

      if (gsolve->propensity_enz[2][i]) {
        time = -1 * log (G_RNG()) / gsolve->propensity_enz[2][i];
        if (time < minimum_time)
  	{
  	  minimum_time = time;
  	  r_or_e_type = 2;	/* enz chosen */
  	  r_or_e_index = i;
  	  r_or_e_direction = 3;
  	}
      } else
        time = DBL_MAX;
    }
  
  if (r_or_e_type == 1)
    {
      reac_chosen =
        (struct reac_type *) gsolve->reac_list->
        element[r_or_e_index];
      reac_chosen_direction = r_or_e_direction;
    }
  else if (r_or_e_type == 2)
    {
      enz_chosen =
        (struct enz_type *) gsolve->enz_list->element[r_or_e_index];
      enz_chosen_direction = r_or_e_direction;
    }
  
  transition_time = minimum_time;
  gsolve->internal_time += transition_time;
  if (reac_chosen)
    GsolveUpdateReacMolecules(
      gsolve, reac_chosen, reac_chosen_direction);
  if (enz_chosen)
    GsolveUpdateEnzMolecules(
      gsolve, enz_chosen, enz_chosen_direction);
}

void GsolveMethod3(struct gsolve_type *gsolve, double nextt)
{
  int i, j, k;
  double orig_nenz = 0.0;
  double **data = NULL;
  struct reac_type *reac = NULL;
  struct enz_type *enz = NULL;
  struct reac_type *reac_chosen = NULL;
  struct enz_type *enz_chosen = NULL;
  int reac_chosen_direction = 0;	/* 1 or -1*/
  int enz_chosen_direction = 0;	/* 1, 2 or 3 */
  int trans_chosen = 1900;
  double transition_time = 0.0;
  double propensity_trans_old;
  int **se;

  if (gsolve->enz_list_populated)
    for (i = 0; i < gsolve->enz_list->nelements; i++) {
		enz = (struct enz_type *) gsolve->enz_list->element[i];
		enz->pA = 0;
    }
  
  if (gsolve->propensity_total == 0) {
    gsolve->internal_time = nextt;
    return;
  }

  trans_chosen = pq_nodes[0].transition_index;
  transition_time = pq_nodes[0].time;
  
  if (trans_chosen < 1000) {     /* reac chosen */
    reac_chosen = (struct reac_type *) gsolve->reac_list->element[trans_chosen / 2];
    reac_chosen_direction = trans_chosen % 2;

    data = reac_chosen->msgdata;
    se = reac_chosen->se;
    switch (reac_chosen_direction) {
    case 0:   /* forward */
		for (j = 0; j < reac_chosen->msgcount[0]; j++, data++, se++)
	if (**se == 0)
	  **data -= 1;
		for (j = 0; j < reac_chosen->msgcount[1]; j++, data++, se++)
	if (**se == 0)
	  **data += 1;
		break;
    case 1:   /* backward */
		for (j = 0; j < reac_chosen->msgcount[0]; j++, data++, se++)
	if (**se == 0)
	  **data += 1;
		for (j = 0; j < reac_chosen->msgcount[1]; j++, data++, se++)
	 if (**se == 0)
	  **data -= 1;
		break;
    default:
		break;
    }
  }
  else {                         /* enz chosen */
    enz_chosen = (struct enz_type *) gsolve->enz_list->element[(trans_chosen - 1000) / 3];
    enz_chosen_direction = (trans_chosen - 1000) % 3;
    
    data = enz_chosen->msgdata;
    se = enz_chosen->se;
    /*	      dt = Clockrate (enz_chosen); */
    switch (enz_chosen_direction) {
    case 0:   /* E + S -----> E.S */
                enz_chosen->nComplex++;
                if (enz_chosen->usecomplex) {
                  data += enz_chosen->msgcount[0];
	 se += enz_chosen->msgcount[0];
		}
                else
                  for (j = 0; j < enz_chosen->msgcount[0]; j++, data++, se++)
	  if (**se == 0)
	    **data -= 1;
                for (j = 0; j < enz_chosen->msgcount[1]; j++, data++, se++)
	if (**se == 0)
	  **data -= 1;
		/*                enz_chosen->pA = 0; */
		break;
    case 1:   /* E.S -----> E + S */
                enz_chosen->nComplex--;
                if (enz_chosen->usecomplex) {
                  data += enz_chosen->msgcount[0];
	se += enz_chosen->msgcount[0];
		}
                else
                  for (j = 0; j < enz_chosen->msgcount[0]; j++, data++, se++)
	  if (**se == 0)
	    **data += 1;
                for (j = 0; j < enz_chosen->msgcount[1]; j++, data++, se++)
	if (**se == 0)
	  **data += 1;
		/*                enz_chosen->pA = 0; */
		break;
    case 2:   /* E.S -----> E + P */
                enz_chosen->nComplex--;
                if (!enz_chosen->usecomplex) {
                  for (j = 0; j < enz_chosen->msgcount[0]; j++, data++, se++)
	  if (**se == 0)
	    **data += 1;
		} else {
	data += enz_chosen->msgcount[0];
	se += enz_chosen->msgcount[0];
		}
		data += enz_chosen->msgcount[1] + enz_chosen->msgcount[2] + enz_chosen->msgcount[3];
		se += enz_chosen->msgcount[1];
		for (j = 0; j < enz_chosen->msgcount[4]; j++, data++, se++)
	if (**se == 0)
	  **data += 1;
		/*                enz_chosen->pA = 1 / dt; */
		break;
    default:
		break;
    }
    if (enz_chosen->nComplex < 0)
		enz_chosen->nComplex = 0;
    if (enz_chosen->usecomplex)
		{
	data = enz_chosen->msgdata;
	for (j = 0; j < enz_chosen->msgcount[0]; j++, data++)
	  orig_nenz = **data;
	if (enz_chosen->nComplex > orig_nenz)
	  enz_chosen->nComplex = orig_nenz;
		}
    enz_chosen->CoComplex = enz_chosen->nComplex / enz_chosen->vol;
  }

  for (i = 0; i < dep_graph_adj[trans_chosen][1900]; i++) {
    k = dep_graph_adj[trans_chosen][i];
    propensity_trans_old = propensity_trans[k];
    if (k < 1000) {
		gsolve->propensity_total -= propensity_trans[k];
		reac = (struct reac_type *) gsolve->reac_list->element[k / 2];
		data = reac->msgdata;
		
		if ((k % 2) == 0) {
	propensity_trans[k] = reac->kf;
	for (j = 0; j < reac->msgcount[0]; j++, data++)
	  propensity_trans[k] *= **data;
		}
		else {
	data += reac->msgcount[0];
	propensity_trans[k] = reac->kb;
	for (j = 0; j < reac->msgcount[1]; j++, data++)
	  propensity_trans[k] *= **data;
		}
		gsolve->propensity_total += propensity_trans[k];
    }
    else {
		gsolve->propensity_total -= propensity_trans[k];
		enz = (struct enz_type *) gsolve->enz_list->element[(k - 1000) / 3];
		data = enz->msgdata;
		
		if (((k - 1000) % 3) == 0) {
	for (j = 0; j < enz->msgcount[0]; j++, data++)
	  propensity_trans[k] = **data;
	if (enz->usecomplex)
	  propensity_trans[k] -= enz->nComplex;
	if (propensity_trans[k] < 1)
	  propensity_trans[k] = 0;
	for (j = 0; j < enz->msgcount[1]; j++, data++)
	  propensity_trans[k] *= **data;
	propensity_trans[k] *= enz->k1;
		}
		else if (((k - 1000) % 3) == 1) {
	propensity_trans[k] = enz->k2 * enz->nComplex;
	if (enz->nComplex < 1)
	  propensity_trans[k] = 0;
		}
		else {
	propensity_trans[k] = enz->k3 * enz->nComplex;
	if (enz->nComplex < 1)
	  propensity_trans[k] = 0;
		}
		
		gsolve->propensity_total += propensity_trans[k];
    }
    if (k != trans_chosen) {
		if (propensity_trans_old) {
	if (propensity_trans[k]) {
	  pq_update(transition_pq_index[k],  
		    ((propensity_trans_old / propensity_trans[k])  
		     * (pq_nodes[transition_pq_index[k]].time - transition_time))  
		    + transition_time);
	} else {
	  propensity_trans_zero[k] = propensity_trans_old;
	  time_zero[k] = transition_time;
	  trans_time_zero[k] = pq_nodes[transition_pq_index[k]].time;
	  pq_update(transition_pq_index[k], DBL_MAX);
	}
		} else {   /* propensity_trans_old == 0 */
	if (propensity_trans[k]) {
	  if (propensity_trans_zero[k])
	    pq_update(transition_pq_index[k],
				((propensity_trans_zero[k] / propensity_trans[k]) 
				 * (trans_time_zero[k] - time_zero[k])) 
				+ transition_time);
	  else
	    pq_update(transition_pq_index[k],
				-1 * log (G_RNG()) / propensity_trans[k]
				+ transition_time);
	} else {
	  /* Do nothing. propensity should already be DBL_MAX */
	}
		}
    }
    else {
		if (propensity_trans[k])
	pq_update(transition_pq_index[k],
		  -1 * log (G_RNG()) / propensity_trans[k]
		  + transition_time);
		else {
	pq_update(transition_pq_index[k], DBL_MAX);
	propensity_trans_zero[k] = 0;
		}
    }
  }
  gsolve->internal_time = transition_time;
}

int GsolveFunc (gsolve, action)
     struct gsolve_type *gsolve;
     Action *action;
{
  double dt = Clockrate(gsolve);
  double nextt = dt + SimulationTime();
  int i;

  if (debug > 1)
    {
      ActionHeader ("ReactionGsolve", gsolve, action);
    }
  
  SELECT_ACTION (action)
    {
    case PROCESS:
      if (!(gsolve->reac_list_populated || gsolve->enz_list_populated))
	      break;
      while (gsolve->internal_time < nextt)
	{
	  if ((gsolve->method == 1) || (gsolve->method == 2)) {
	    /* build up propensity_reac[][] and propensity_total */
	    GsolveCalculateTotalPropensity(gsolve);
	    
	    /* if prop total is zero, skip out now. */
	    if (gsolve->propensity_total == 0) {
		    gsolve->internal_time = nextt;
		    break;
	    }
	    
	    if (gsolve->method == 1)
	      GsolveMethod1(gsolve);
	    else if (gsolve->method == 2)
	      GsolveMethod2(gsolve);
	  } else { /* gsolve->method == 3; Gibson-Bruck method */
	      GsolveMethod3(gsolve, nextt);
	  }
          for (i = 0; i < gsolve->sumtotal_list->nelements; i++) {
	     GsolveUpdateSumtotal(gsolve->sumtotal_list->element[i]);
	  }
	}
        /* We only need to update the rest of the pools when the
	 * system clock ticks */
	if (gsolve->pool_list_populated) {
          for (i = 0; i < gsolve->pool_list->nelements; i++)
	    GsolveUpdatePool(gsolve->pool_list->element[i]);
	}
      break;
    case RESET:
      gsolve_do_reset (gsolve);
      break;
    case CREATE:
      /* gsolve_do_reset (gsolve); ** Don't do this! */
      gsolve->sumtotal_list = NULL;
      return(1);
      break;
    case SET:
      return (0);
      break;
    case DELETE:
	  if (gsolve->pool_list_populated) {
    	    for (i = 0; i < gsolve->pool_list->nelements; i++)
	      Enable(gsolve->pool_list->element[i]);
	  }
	  if (gsolve->reac_list_populated) {
    	    for (i = 0; i < gsolve->reac_list->nelements; i++)
	      Enable(gsolve->reac_list->element[i]);
	  }
	  if (gsolve->enz_list_populated) {
    	    for (i = 0; i < gsolve->enz_list->nelements; i++)
	      Enable(gsolve->enz_list->element[i]);
	  }
    break;
    }
	return (0);
}

static void
check_msg_type (gsolve, msg)
     struct gsolve_type *gsolve;
     MsgIn *msg;
{
  if (MSGSLOT (msg)[0].func != DoubleMessageData)
    {
      Warning ();
      /*
       * Complain 
       */
      printf ("Message source for REAC msg to %s is not a double\n",
	      Pathname (gsolve));
      printf ("Reverting to slow old messaging\n");
      gsolve->oldmsgmode = 1;
    }
}

static void
gsolve_do_reset (struct gsolve_type *gsolve)
{
  int i, j, j2, k, t;
  double **data;
  struct reac_type *reac;
  struct enz_type *enz;
  struct pool_type *pool, *sumtotal_src;
  
  int fwd_successors[200]; /* assuming a maximum of 200 successors of a pool */
  int fwd_reac_successors = 0;
  int bwd_successors[200];
  int bwd_reac_successors = 0;
  
  Msg *msgout, *msgin, *msgin2;
  unsigned int nmsgout, nmsgin, nmsgin2;
  MsgList *ml, *ml2;
  GenesisObject *object, *object2;

  /*  int **se; */

  double sumtotal = 0;

  gsolve->propensity_total = 0.0;
  gsolve->internal_time = 0.0;
	  
  
  gsolve->reac_list = WildcardGetElement ("##[TYPE=kreac]", 0);
  if ((!gsolve->reac_list) || (gsolve->reac_list->nelements == 0))
    gsolve->reac_list_populated = 0;
  else
    {
      gsolve->reac_list_populated = 1;
      ResetElements (gsolve->reac_list);
    }
  
  gsolve->enz_list = WildcardGetElement ("##[TYPE=kenz]", 0);
  if ((!gsolve->enz_list) || (gsolve->enz_list->nelements == 0))
    gsolve->enz_list_populated = 0;
  else
    {
      gsolve->enz_list_populated = 1;
      ResetElements (gsolve->enz_list);
    }
  
  gsolve->pool_list = WildcardGetElement ("##[TYPE=kpool]",0);
  if ((!gsolve->pool_list) || (gsolve->pool_list->nelements == 0))
    gsolve->pool_list_populated = 0;
  else {
    gsolve->pool_list_populated = 1;
    ResetElements (gsolve->pool_list);
  }


  if (gsolve->sumtotal_list)
    FreeElementList(gsolve->sumtotal_list);
  gsolve->sumtotal_list = CreateElementList(1);

  /* flooring all pool and enzyme complex 'n' fields once */
  if (gsolve->pool_list_populated) {
    for (i = 0; i < gsolve->pool_list->nelements; i++)
      {
        pool = (struct pool_type *) gsolve->pool_list->element[i];
	    Block(pool);
        pool->n = floor(pool->nInit) +
			(G_RNG()  < (pool->nInit - floor(pool->nInit)));
  
        data = pool->msgdata;
        data += pool->msgcount[0] + pool->msgcount[1] +
		pool->msgcount[2] + pool->msgcount[3] +
		pool->msgcount[4] + pool->msgcount[5];
        for (j = 0; j < pool->msgcount[6]; j++, data++)
  	  sumtotal += **data;
        if (!(pool->slave_enable & POOL_BUFFER))
  	if (pool->consv_flag & POOL_SUMTOTAL)
  	  pool->nTotal = pool->n = floor(sumtotal);
        /* put the rest of the circus in? or will this do? */
	/* Put in the sumtotal count */
	if (pool->msgcount[6] > 0) {
		AddElementToList(pool, gsolve->sumtotal_list);
	}
     }
  }
  
  if (gsolve->enz_list_populated) {
    for (i = 0; i < gsolve->enz_list->nelements; i++)
      {
        enz = (struct enz_type *) gsolve->enz_list->element[i];
	    Block(enz);
        enz->nComplex = floor(enz->nComplex) +
			(G_RNG()  < (enz->nComplex - floor(enz->nComplex)));
      }
  }
    
    
  /* Populating dep_graph_index */
  if (gsolve->reac_list_populated) {
    for (i = 0; i < gsolve->reac_list->nelements; i++)
      {
        reac = (struct reac_type *) gsolve->reac_list->element[i];
	    Block(reac);
        reac->A = reac->B = 0;
        reac->dep_graph_index = i*2; /* Two transitions per reac */
      }
  }
  
  if (gsolve->enz_list_populated) {
    for (i = 0; i < gsolve->enz_list->nelements; i++)
      {
        enz = (struct enz_type *) gsolve->enz_list->element[i];
        enz->sA = enz->eA = enz->B = 0;
        enz->dep_graph_index = 1000 + i*3; /* Three transitions per enz; enzymes begin at 1000 */
      }
  }
  
  if (gsolve->method == 3) { /* Gibson-Bruck */
    /* Build dependency graph */

    /* normal reacs, via pool_list */
    /* forward: pool incoming REACs are linked to pool outgoing SUBSTRATEs */
    /* backward: pool incoming REACs are linked to pool outgoing PRODUCTs */
    
    /* enzymes: (internal), via enz_list */
         
    /* TODO Profile use of adj matrix per timestep.. switch to adj list if too slow
	* 		This _is_ per PROCESS */
    
    /* TODO Pool to pool CONSERVE/SUMTOTAL handling - nothing special needed:? */

    
    for (i = 0; i < 1900; i++) {
      propensity_trans[i] = 0;
      propensity_trans_zero[i] = 0;
      time_zero[i] = 0;
      trans_time_zero[i] = DBL_MAX;
      transition_pq_index[i] = 1900;  /* never an actual point in pq */
      pq_nodes[i].time = DBL_MAX;
      pq_nodes[i].transition_index = 1900; /* never an actual transition */
    }

    for (i = 0; i < 1900; i++)
      for (j = 0; j < 1901; j++)
	dep_graph_adj[i][j] = 0;

    pq_size = 0;
    
    if (gsolve->pool_list_populated) {
      for (i = 0; i < gsolve->pool_list->nelements; i++) {
        pool = (struct pool_type *) gsolve->pool_list->element[i];
        msgout = pool->msgout;
        nmsgout = pool->nmsgout;
        /* TODO is pool->msgout TRUE? */
        for (j = 0; j < nmsgout; j++) {
  	if (VISIBLE(msgout->dst)) {
  	  ml = GetMsgListByType(msgout->dst->object, msgout->type);
  	  /* TODO optimize or replace slow string comparison */
  	  if ((ml) && (strcmp(ml->name, "SUBSTRATE") == 0)) {
  	    if (ElementIsA(msgout->dst, "reac")) {
  	      reac = (struct reac_type *) msgout->dst;
  	      fwd_successors[fwd_reac_successors] = reac->dep_graph_index;
  	      fwd_reac_successors++;
  	    } else if (ElementIsA(msgout->dst, "enz")) {
  	      enz = (struct enz_type *) msgout->dst;
  	      fwd_successors[fwd_reac_successors] = enz->dep_graph_index;
  	      fwd_reac_successors++;
  	    }
  	  }
  	  else if ((ml) && (strcmp(ml->name, "ENZYME") == 0)) {
  	    enz = (struct enz_type *) msgout->dst;
  	    fwd_successors[fwd_reac_successors] = enz->dep_graph_index;
  	    fwd_reac_successors++;
  	  }
  	  else if ((ml) && (strcmp(ml->name, "PRODUCT") == 0) && (ElementIsA(msgout->dst, "reac"))) {
  	    reac = (struct reac_type *) msgout->dst;
  	    bwd_successors[bwd_reac_successors] = reac->dep_graph_index + 1;
  	    bwd_reac_successors++;
  	  }
  	}
  	msgout = MSGOUTNEXT(msgout);
        }
        msgin = pool->msgin;
        object = pool->object;
        nmsgin = pool->nmsgin;
        /* TODO is pool->msgin TRUE? */
        for (j = 0; j < nmsgin; j++) {
  	if (VISIBLE(msgin->src)) {
  	  ml = GetMsgListByType(object, msgin->type);
  	  if ((ml) && (strcmp(ml->name, "REAC") == 0)) {
  	    if (ElementIsA(msgin->src, "reac")) {
  	      reac = (struct reac_type *) msgin->src;
  	      t = reac->dep_graph_index;
  	      for (k = 0; k < fwd_reac_successors; k++) {
  		add_dep_list(t, fwd_successors[k]);
  		add_dep_list(t+1, fwd_successors[k]);
  	      }
  	      for (k = 0; k < bwd_reac_successors; k++) {
  		add_dep_list(t, bwd_successors[k]);
  		add_dep_list(t+1, bwd_successors[k]);
  	      }
  	    } else if (ElementIsA(msgin->src, "enz")) {
  	      enz = (struct enz_type *) msgin->src;
  	      t = enz->dep_graph_index;
  	      /* TODO *_reac_* is a bad choice of name; change this appropriately */
  	      for (k = 0; k < fwd_reac_successors; k++) {
  		add_dep_list(t, fwd_successors[k]);
  		add_dep_list(t+1, fwd_successors[k]);
  	      }
  	      for (k = 0; k < bwd_reac_successors; k++) {
  		add_dep_list(t, bwd_successors[k]);
  		add_dep_list(t+1, bwd_successors[k]);
  	      }
  	    }
  	  }
  	  else if ((ml) && (strcmp(ml->name, "MM_PRD") == 0)) {
  	    enz = (struct enz_type *) msgin->src;
  	    t = enz->dep_graph_index;
  	    for (k = 0; k < fwd_reac_successors; k++)
  	      add_dep_list(t+2, fwd_successors[k]);
  	    for (k = 0; k < bwd_reac_successors; k++)
  	      add_dep_list(t+2, bwd_successors[k]);
  	  }
  	}
  	msgin = MSGINNEXT(msgin);
        }
  
        msgin = pool->msgin;
        object = pool->object;
        nmsgin = pool->nmsgin;
        for (j = 0; j < nmsgin; j++) {
  	if (VISIBLE(msgin->src)) {
  	  ml = GetMsgListByType(object, msgin->type);
  	  if ((ml) && (strcmp(ml->name, "SUMTOTAL") == 0)) {
  	    /* assuming only POOLs send out SUMTOTALs */
  	    sumtotal_src = (struct pool_type *) msgin->src;
  	    
  	    msgin2 = sumtotal_src->msgin;
  	    object2 = sumtotal_src->object;
  	    nmsgin2 = sumtotal_src->nmsgin;
  
  	    for (j2 = 0; j2 < nmsgin2; j2++) {
  	      if (VISIBLE(msgin2->src)) {
  		ml2 = GetMsgListByType(object2, msgin2->type);
  		if ((ml2) && (strcmp(ml2->name, "REAC") == 0)) {
  		  if (ElementIsA(msgin2->src, "reac")) {
  		    reac = (struct reac_type *) msgin2->src;
  		    t = reac->dep_graph_index;
  		    for (k = 0; k < fwd_reac_successors; k++) {
  		      add_dep_list(t, fwd_successors[k]);
  		      add_dep_list(t+1, fwd_successors[k]);
  		    }
  		    for (k = 0; k < bwd_reac_successors; k++) {
  		      add_dep_list(t, bwd_successors[k]);
  		      add_dep_list(t+1, bwd_successors[k]);
  		    }
  		  } else if (ElementIsA(msgin2->src, "enz")) {
  		    enz = (struct enz_type *) msgin2->src;
  		    t = enz->dep_graph_index;
  		    for (k = 0; k < fwd_reac_successors; k++) {
  		      add_dep_list(t, fwd_successors[k]);
  		      add_dep_list(t+1, fwd_successors[k]);
  		    }
  		    for (k = 0; k < bwd_reac_successors; k++) {
  		      add_dep_list(t, bwd_successors[k]);
  		      add_dep_list(t+1, bwd_successors[k]);
  		    }
  		  }
  		} else if ((ml2) && (strcmp(ml2->name, "MM_PRD") == 0)) {
  		  enz = (struct enz_type *) msgin2->src;
  		  t = enz->dep_graph_index;
  		  for (k = 0; k < fwd_reac_successors; k++)
  		    add_dep_list(t+2, fwd_successors[k]);
  		  for (k = 0; k < bwd_reac_successors; k++)
  		    add_dep_list(t+2, bwd_successors[k]);
  		}
  	      }
  	      msgin2 = MSGINNEXT(msgin2);
  	    }
  	  }
  	}
  	msgin = MSGINNEXT(msgin);
        }
        
        fwd_reac_successors = 0;
        bwd_reac_successors = 0;
      }
    }

    if (gsolve->enz_list_populated) {
      for (i = 0; i < gsolve->enz_list->nelements; i++) {
	enz = (struct enz_type *) gsolve->enz_list->element[i];
	t = enz->dep_graph_index;
	for (j = 0 ; j < 3; j++)
	  for (k = 0; k < 3; k++)
	    add_dep_list(t+j, t+k);
      }
    }

    if (gsolve->reac_list_populated) {
      for (i = 0; i < gsolve->reac_list->nelements; i++) {
	reac = (struct reac_type *) gsolve->reac_list->element[i];
	data = reac->msgdata;

	propensity_trans[i*2] = reac->kf;
	for (j = 0; j < reac->msgcount[0]; j++, data++)
	  propensity_trans[i*2] *= **data;
	
	propensity_trans[1+i*2] = reac->kb;
	for (j = 0; j < reac->msgcount[1]; j++, data++)
	  propensity_trans[1+i*2] *= **data;
	
	gsolve->propensity_total +=
	  propensity_trans[i*2] + propensity_trans[1+i*2];
      }
    }
    
    if (gsolve->enz_list_populated) {
      for (i = 0; i < gsolve->enz_list->nelements; i++) {
	enz = (struct enz_type *) gsolve->enz_list->element[i];
	data = enz->msgdata;
	
	for (j = 0; j < enz->msgcount[0]; j++, data++)
	  propensity_trans[1000 + i*3] = **data;
	if (enz->usecomplex)
	  propensity_trans[1000 + i*3] -= enz->nComplex;
	if (propensity_trans[1000 + i*3] < 1)
	  propensity_trans[1000 + i*3] = 0;
	for (j = 0; j < enz->msgcount[1]; j++, data++)
	  propensity_trans[1000 + i*3] *= **data;
	propensity_trans[1000 + i*3] *= enz->k1;
	
	propensity_trans[1001 + i*3] = enz->k2 * enz->nComplex;
	if (enz->nComplex < 1)
	  propensity_trans[1001 + i*3] = 0;
	
	propensity_trans[1002 + i*3] = enz->k3 * enz->nComplex;
	if (enz->nComplex < 1)
	  propensity_trans[1002 + i*3] = 0;
	
	gsolve->propensity_total +=
	  propensity_trans[1000 + i*3] + propensity_trans[1001 + i*3]
	  + propensity_trans[1002 + i*3];
	
      }
    }
    /* Populate priority queue storing (absolute) transition times */
    
    if (gsolve->reac_list_populated) {
      for (i = 0; i < gsolve->reac_list->nelements; i++) {
        if (propensity_trans[i*2])
	  pq_insert((-1 * log (G_RNG()) / propensity_trans[i*2]), i*2);
        else
	  pq_insert(DBL_MAX, i*2);
        if (propensity_trans[1+i*2])
	  pq_insert((-1 * log (G_RNG()) / propensity_trans[1+i*2]), 1+i*2);
        else
	  pq_insert(DBL_MAX, 1+i*2);
      }
    }
    if (gsolve->enz_list_populated) {
      for (i = 0; i < gsolve->enz_list->nelements; i++) {
        if (propensity_trans[1000+i*3])
	  pq_insert((-1 * log (G_RNG()) / propensity_trans[1000+i*3]), 1000+i*3);
        else
	  pq_insert(DBL_MAX, 1000+i*3);
        if (propensity_trans[1001+i*3])
	  pq_insert((-1 * log (G_RNG()) / propensity_trans[1001+i*3]), 1001+i*3);
        else
	  pq_insert(DBL_MAX, 1001+i*3);
        if (propensity_trans[1002+i*3])
	  pq_insert((-1 * log (G_RNG()) / propensity_trans[1002+i*3]), 1002+i*3);
        else
	  pq_insert(DBL_MAX, 1002+i*3);
      } 
    }
  }
}

/*
 * This function is declared as an extern void() 
 */
/*
 * void copyleft_kin () { printf ("The kinetics library is copylefted
 * under the LGPL, see kinetics/COPYRIGHT.\n\n"); }
 */

/* Priority Queue housekeeping */
/* INSERT */
void pq_insert(double t, int t_i) {
  int m = pq_size;

  pq_nodes[m].time = t;
  pq_nodes[m].transition_index = t_i;
  transition_pq_index[t_i] = m;

  while (m != 0) {    
    if (pq_nodes[m].time < pq_nodes[m/2].time) {
      pq_swap(m, m/2);
      m = m/2;
    }
    else
      break;
  }

  pq_size++;
  return;
}

/* SWAP (and update index) */
void pq_swap(int a, int b) {
  pq_node temp;

  temp = pq_nodes[a];
  pq_nodes[a] = pq_nodes[b];
  pq_nodes[b] = temp;

  transition_pq_index[pq_nodes[a].transition_index] = a;
  transition_pq_index[pq_nodes[b].transition_index] = b;

  return;
}


/* UPDATE */
void pq_update(int n, double new_t) {
  pq_nodes[n].time = new_t;
  pq_update_aux(n);

  return;
}

void pq_update_aux(int n) {
  double min_val_children = 0;
  int min_child = 0;

  if (pq_nodes[n].time < pq_nodes[n/2].time) {
    pq_swap(n, n/2);
    pq_update_aux(n/2);
  }
  else if ((2*n) < 1900) {
    if (pq_nodes[2*n].time < pq_nodes[2*n+1].time) {
      min_val_children = pq_nodes[2*n].time;
      min_child = 2*n;
    } else {
      min_val_children = pq_nodes[2*n+1].time;
      min_child = 2*n+1;
    }
    if (pq_nodes[n].time > min_val_children) {
      pq_swap(n, min_child);
      pq_update_aux(min_child);
    } 
  }

  return;
}

int add_dep_list(int trans /* ition */, int dep /* endent */ ) {
  int i, already_in = 0;

  for (i = 0; i < dep_graph_adj[trans][1900]; i++) {
    if (dep_graph_adj[trans][i] == dep)
      already_in = 1;
  }

  if (already_in)
    return -1;
  else {
    dep_graph_adj[trans][dep_graph_adj[trans][1900]++] = dep;
    return 0;
  }
}

/* TODO: copy by reference instead of bitwise (OPTI) */

#undef POOL_BUFFER
#undef POOL_SUMTOTAL
#undef POOL_CONSERVE
#undef POOL_NSLAVE
#undef POOL_CONCSLAVE

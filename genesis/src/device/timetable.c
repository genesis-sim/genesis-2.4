static char rcsid[] = "$Id: timetable.c,v 1.5 2005/07/20 20:01:58 svitak Exp $";

/*
** $Log: timetable.c,v $
** Revision 1.5  2005/07/20 20:01:58  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.4  2005/07/01 10:03:01  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.3  2005/06/27 19:00:37  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.2  2005/06/17 19:30:04  svitak
** Under certain circumstances, the code for the TUPDATE action of the
** timetable object would attempt to free an uninitialized pointer
** (tempti). This caused a segmentation fault under Cygwin. This was
** solved by initializing the pointer to NULL on declaration.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2002/01/11 04:49:15  mhucka
** Correction from Dieter for previous mods.
**
** Revision 1.3  2000/06/12 04:33:21  mhucka
** Added NOTREACHED comments where appropriate.
**
** Revision 1.2  2000/04/24 06:16:15  mhucka
** Added enhancements from Dieter Jaeger, 23 Apr 2000.  This adds a 4th
** mode to tupdate.
**
** Revision 1.1  1998/03/31 22:10:24  dhb
** Initial revision
**
*/

#include <math.h>
#include <stdlib.h>
#include "spike_ext.h"

#define BUFSIZE 100

/*
** Creates a table of ascending simulation time values. When
** SimulationTime matches the next time value the activation
** field is set for one time step. Designed to provide
** temporally precise sequences of inputs to other elements.
*/

int TimetableFunc(titab,action)
register struct timetable_type *titab;
Action		*action;
{
	void next_iti();
	int store_iti();
	void bubble_fsort();
	float currti, currval,intvl;
	float *tempti = NULL;
	float aisi, period, propp, isirange = 0.0;
	int usermode;
	float userval1, userval2 = 0.0, userval3 = 0.0, userval4 = 0.0;
	int avail,i,fstpos,lstpos,tail;
	FILE *fp = NULL;
	char line[128];

    if(debug > 1){
	ActionHeader("VDepGate",titab,action);
    }

    SELECT_ACTION(action){
	case SET:
		return(0);
    case PROCESS:
		if (titab->allocated == 0) return(0);
		titab->activation = 0;
		currti = SimulationTime();
		if(titab->timetable[titab->tabpos] <= currti && titab->tabpos < titab->maxpos) {
			titab->activation = titab->act_val;
			titab->tabpos++;
		}
		break;
    case RESET:
        titab->tabpos = 0;
        break;
    case TUPDATE: /* change table settings */
		if(!titab->allocated) return(0);
        if (action->argc < 2) {
			fprintf(stderr, "\nusage: TUPDATE mode, mode_desc1, [mode_desc2] [mode_desc3] [mode_desc4]\n");
			fprintf(stderr, "\nmode 1 (jitter times): TUPDATE 1 startti endti +-maxjitter\n");
			fprintf(stderr, "\nmode 2 (insert burst): TUPDATE 2 startti endti iti1 [iti2]\n");
			fprintf(stderr, "\nmode 3 (insert n spikes ): TUPDATE 3 startti endti n jitval\n");
            return(0);
		}
		usermode = atoi(action->argv[0]);
		userval1 = (float)strtod(action->argv[1], (char **)NULL);
		period = 0.0;
		if (action->argc > 2) userval2 = (float)strtod(action->argv[2], (char**)NULL);
		if (action->argc > 3) userval3 = (float)strtod(action->argv[3], (char**)NULL);
			aisi = userval3;
		if (action->argc > 4) {
			userval4 = (float)strtod(action->argv[4], (char**)NULL);
			period = userval2 - userval1;
			isirange = userval4 - userval3;
		}

		switch (usermode) {
		case 1:
			/* mode1 : jitter time values by random amount (max userval1) */
			if (action->argc < 4) {
				fprintf(stderr, "\nusage mode 1 (jitter times): TUPDATE 1 startti endti +-maxjitter\n");
			}
			if(titab->seed > 0) ran1(-(titab->seed));
			for(i=0; i<titab->maxpos; i++) {
				if(titab->timetable[i] >  userval1) break;
			}
			fstpos = i;
			for(i=titab->maxpos-1; i >=0; i--) {
				if(titab->timetable[i] < userval2) break;
			}
			lstpos = i + 1;
			for(i=fstpos; i<lstpos; i++) {
				intvl = (urandom() - 0.5) * userval3;
				titab->timetable[i] += intvl;
			}
			bubble_fsort(titab->timetable, titab->maxpos);
			break;
		case 2:
			/* insert period with different mean iti, typically a burst */
			if (action->argc < 4) {
				fprintf(stderr, "\nusage (insert burst): TUPDATE 2 startti endti iti1 [iti2]\n");
			}
			if(titab->seed > 0) ran1(-(titab->seed));
			for(i=0; i<titab->maxpos; i++) {
				if(titab->timetable[i] >  userval1) break;
			}
			fstpos = i;
			for(i=titab->maxpos-1; i >=0; i--) {
				if(titab->timetable[i] < userval2) break;
			}
			lstpos = i + 1;
			/* copy second part of timetable to temporary buffer */
			tail = titab->maxpos - lstpos;
			if(tail > 0) {
				tempti = (float *) malloc (tail * sizeof(float));
				if(tempti == NULL) {
					fprintf(stderr, "\nCOULD NOT ALLOC TEMP FLOAT ARRAY SIZE %d\n", tail);
					return(0);
				}
				for(i=0; i<tail; i++) {
					tempti[i] = titab->timetable[lstpos+i];
				}
			}
			/* insert new period */
			avail = titab->maxpos - 1;
			titab->maxpos = fstpos;

			/* to avoid edge effect need to go one interval backwards */
			currval = 0;
			next_iti(titab, &currval, userval3);
			currval = userval1 - currval;
			while(currval < userval1) next_iti(titab, &currval, userval3);

			while(currval < userval2) {
				store_iti(titab, &currval, &avail);
				/* allow for linearly increasing/decreasing mean spike isis */
				if(period > 0.0) {
					propp = (currval - userval1) / period;
					aisi =  userval3 + (propp * isirange);
				}
				next_iti(titab, &currval, aisi);
			}

			/* add second part back on end */
			titab->timetable = (float *) realloc (titab->timetable, (titab->maxpos + tail + 1) * sizeof(float));
			if(titab->timetable == NULL) {
				titab->allocated = 0;
				fprintf(stderr, "\nCANNOT REALLOC TIMETABLE TO %d\n", titab->maxpos + tail + 1);
			}
			for(i=0; i<tail; i++) {
				titab->timetable[titab->maxpos] = tempti[i];
				(titab->maxpos)++;
			}
			free (tempti);

			break; /* end usermode case 2 */
		case 3:
			/* insert n spikes with jitter from begti to endti */
			if (action->argc < 5) {
				fprintf(stderr, "\nusage (insert n spikes ): TUPDATE 3 startti endti n jitval\n");
			}

			if(userval3 < 0) {
				fprintf(stderr, "\nusage (insert n spikes ): TUPDATE 3 startti endti n jitval\n");
				fprintf(stderr, "\nERROR: Number of spikes to insert > 1 needs to be given as parameter\n");
				break;
			}

			avail = titab->maxpos - 1;
			currti = userval1;
			currval = currti;
			if(userval3 > 1) aisi = (userval2 - userval1) / (userval3 -1);

			for(i=0; i<userval3; i++) {
				if(userval4 > 0) {
					currval += ((urandom() - 0.5) * userval4);
				}
				store_iti(titab, &currval, &avail);
				currti += aisi;
				currval = currti;
			}
			titab->timetable = (float *) realloc (titab->timetable, (titab->maxpos + 1) * sizeof(float));
			bubble_fsort(titab->timetable, titab->maxpos);
			break;
		}
		break;
	case TABCREATE: /* create fixed size table : can be filled by user */
		if (action->argc < 1) {
			fprintf(stderr, "usage : %s tablesize \n","tabcreate");
			return(0);
		}
		titab->maxpos = atoi(action->argv[0]);
		if(titab->allocated) free (titab->timetable);
		titab->allocated = 0;
		titab->timetable = (float *) calloc (titab->maxpos, sizeof(float));
		if(titab->timetable == NULL) {
			titab->maxpos = 0;
			fprintf(stderr, "\nCANNOT ALLOC TIMETABLE\n");
			return(0);
		}
		titab->allocated = 1;
		break;
    case TABFILL: /* create and fill table */
		titab->maxpos = 0;
		if(titab->method < 1 || titab->method > 4) {
			fprintf(stderr, "timetable: method field is not valid\n");
			return(0);
		}
		/* seed random generator with seed value if desired */
		if(titab->seed > 0) ran1(-(titab->seed));

		/* allocate first segment of timetable */
		if(titab->allocated) free (titab->timetable);
		titab->allocated = 0;
		titab->timetable = (float *) malloc (BUFSIZE * sizeof(float));
		if(titab->timetable == NULL) {
			fprintf(stderr, "\nCANNOT MALLOC TIMETABLE\n");
			return(0);
		}
		avail = BUFSIZE;
		titab->allocated = 1;

		currval = 0;
		/*
			 to avoid edge effects by starting from 0 time
			 need to go one interval backwards first for
			 random interval distributions
		*/

		if(titab->method < 3) {
			next_iti(titab, &currval, titab->meth_desc1);
			currval = currval * -1.0;
			while(currval < 0.0) next_iti(titab, &currval, titab->meth_desc1);
			store_iti(titab, &currval, &avail);
		}

		/* open file if times read from file */
		if(titab->method == 4) {
			if ((fp = fopen(titab->fname, "r")) == NULL) {
   		    	fprintf(stderr, "\nError in timetable: unable to open file %s\n", titab->fname);
				if(titab->allocated) free (titab->timetable);
				titab->allocated = 0;
        		return (0);
			}

    	}


		/* fill timetable array up to maxtime */

		while(currval <= titab->maxtime) {
			/* find next inter-time interval */
			if(titab->method == 4) {
				if(fgets(line,128,fp) == NULL) {
					break;
				}
				if(sscanf(line, "%f", &currval) != 1) {
					fprintf(stderr, "\nError in file: No floating point value found on line\n");
					continue;
				}
			} else next_iti(titab, &currval, titab->meth_desc1);
			store_iti(titab, &currval, &avail);
		} /* end while < maxtime */

		if(titab->method == 4) {
			fclose(fp);
		}


		titab->timetable = (float *) realloc (titab->timetable, (titab->maxpos + 1) * sizeof(float));
		if(titab->timetable == NULL) {
			titab->allocated = 0;
			fprintf(stderr, "\nCANNOT REALLOC TIMETABLE TO %d\n", titab->maxpos + 1);
		}
       break;
    }
    return 1;
}

void next_iti(ttitab, currt, iti)
register struct timetable_type *ttitab;
float *currt;
float iti;
{
	float intvl = 0.0, calcval;
	int i;

	switch(ttitab->method) {
	case 1: /* exponential iti distribution */
		do
			intvl = urandom();
		while (intvl == 0.0);
		intvl = -log(intvl) * iti;
		break;
	case 2: /* gamma iti */
		if(ttitab->meth_desc3 < 1 || ttitab->meth_desc3 > 6) break;
		calcval = 1.0;
		for(i=0; i<ttitab->meth_desc3; i++) {
			calcval *= urandom();
		}
		calcval = -log(calcval) * (iti - ttitab->meth_desc2) / ttitab->meth_desc3;
		intvl = calcval + ttitab->meth_desc2;
		break;
	case 3: /* regular iti's */
		intvl = iti;
		break;
	}
	*currt += intvl;
}


int store_iti(ttitab, currt, tavail)
register struct timetable_type *ttitab;
float *currt;
int *tavail;
{
	ttitab->timetable[ttitab->maxpos] = *currt;

	/* check for buffer capacity */
	ttitab->maxpos++;
	if(ttitab->maxpos >= *tavail) {
		ttitab->timetable = (float *) realloc (ttitab->timetable, (*tavail + BUFSIZE) * sizeof(float));
		if(ttitab->timetable == NULL) {
			fprintf(stderr, "\nCANNOT REALLOC TIMETABLE FOR SIZE %d\n", *tavail + BUFSIZE);
			return(0);
		}
		*tavail += BUFSIZE;
	}
	return 1;
}

void bubble_fsort(farray,nterms)
        int     *farray;
        int nterms;
{
        float temp;
        int i;
        int     flag=1;

        while(flag) {
                flag=0;
                for(i=1;i<nterms;i++) {
                        if (farray[i-1]>farray[i]) {
                                temp = farray[i-1];
                                farray[i-1] = farray[i];
                                farray[i] = temp;
                                flag=1;
                        }
                }
        }
}

static char rcsid[] = "$Id: tabfiles.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: tabfiles.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2000/06/12 04:46:58  mhucka
** Fixed obvious case of "==" where "=" was meant in TabChannel_READ().
**
** Revision 1.1  1999/10/17 22:30:56  mhucka
** Added changes from Erik De Schutter dated circa 1999.  Several functions
** have been moved from tabchannel.c to tabfiles.c.
**
 * EDS22h revison: EDS BBF-UIA 99/03/17-99/02/26
 * Created this file
 * Made TabChannel_READ platform independent.
 *
*/

#include "olf_ext.h"
#include "olf_defs.h"
#include "olf_g@.h"
#include "result.h"
#include "symtab.h"

void TabChannel_ptrs(elm,i,ttype,itype,ntype,ipolA,ipolB,ipol2A,ipol2B)
	Element *elm;
	int		i,ttype,*ntype,*itype;
	Interpol	**ipolA,**ipolB;
	Interpol2D	**ipol2A,**ipol2B;
{
Tchan	*chan1;
T2chan	*chan2;
Tcurr	*curr;

	/* get table pointers */
	switch (ttype) {
		case TABCHAN_T:
			chan1=(Tchan *)elm;
			*itype=1;
			*ntype=2;
			if (chan1->X_alloced && (i==0)) {
				*ipolA=chan1->X_A;
				*ipolB=chan1->X_B;
			} else if (chan1->Y_alloced && ((i==0)||(i==1))) {
				*ipolA=chan1->Y_A;
				*ipolB=chan1->Y_B;
			} else if (chan1->Z_alloced) {
				*ipolA=chan1->Z_A;
				*ipolB=chan1->Z_B;
			}
			break;

		case TAB2CHAN_T:
			chan2=(T2chan *)elm;
			*itype=2;
			*ntype=2;
			if (chan2->X_alloced && (i==0)) {
				*ipol2A=chan2->X_A;
				*ipol2B=chan2->X_B;
			} else if (chan2->Y_alloced && (i<2)) {
				*ipol2A=chan2->Y_A;
				*ipol2B=chan2->Y_B;
			} else if (chan2->Z_alloced) {
				*ipol2A=chan2->Z_A;
				*ipol2B=chan2->Z_B;
			}
			break;

		case TABCURR_T:
			curr=(Tcurr *)elm;
			*itype=2;
			*ntype=2;
			if (i==0) {
				*ipol2A=curr->G_tab;
				*ipol2B=curr->I_tab;
			}
			break;
	}
}

/* E. De Schutter BBF-UIA 12/94-3/99 */
/*
** Routines to save/read all interpol tables to/from a file.
** Routines can be used by tabchannel, tab2Dchannel and tabcurrent.
** Includes extensive error checking.  Maybe part of these
**  routines should be relocated to Interpol(2D) code.
*/
void TabChannel_SAVE(elm,action)
Element *elm;
Action	*action;
{
int	i,j,k,n=0,ttype,ntype,itype;
int	dsize,isize;
char	version;
FILE	*fp,*fopen();
Tchan	*chan1;
Tcurr	*curr;
Interpol *ipolA,*ipolB;
Interpol2D *ipol2A,*ipol2B;
double	v,*A,*B = NULL;

	if (action->argc != 1){
	    Error();
	    printf(" usage: TABSAVE filename\n");
	    return;
	}
	dsize=sizeof(double);
	isize=sizeof(int);
	/* identify table type */
	if (strcmp(elm->object->name,"tabchannel")==0) {
	    ttype=TABCHAN_T;
	} else if (strcmp(elm->object->name,"tab2Dchannel")==0) {
	    ttype=TAB2CHAN_T;
	} else if (strcmp(elm->object->name,"tabcurrent")==0) {
        ttype=TABCURR_T;
	} else if (strcmp(elm->object->name,"tabflux")==0) {
	    ttype=TABFLUX_T;
	} else {
	    Error();
	    printf(" TABSAVE bug #1\n");
	    return;
	}
	/* count number of tables we need to save */
	switch (ttype) {
	    case TABCHAN_T:
	    case TAB2CHAN_T:
		chan1=(Tchan *)elm;
		if (chan1->X_alloced) n++;
		if (chan1->Y_alloced) n++;
		if (chan1->Z_alloced) n++;
		break;
	    
	    case TABCURR_T:
		curr=(Tcurr *)elm;
		if (curr->alloced) n++;
		break;

	    default:
		break;
	}
	if (n==0) return;	/* nothing to write */
	/* open the file */
	if (!(fp = fopen(action->argv[0],"w"))) {
	    Error();
	    printf(" can't open file '%s'\n",action->argv[0]);
	    return;
	}
	/* write header */
	version=4;	/* version number */
	fwrite(&version,1,1,fp);
	v=4.0;	/* version number */
	fwrite(&v,dsize,1,fp);
	fwrite(&ttype,isize,1,fp);
	fwrite(&n,isize,1,fp);
	/* loop through all the tables */
	for (i=0; i<n; i++) {
	    TabChannel_ptrs(elm,i,ttype,&itype,&ntype,&ipolA,&ipolB,&ipol2A,&ipol2B);
	    fwrite(&itype,isize,1,fp);
	    fwrite(&ntype,isize,1,fp);
	    if (itype==1) {
		/* write table sizes */
		fwrite(&(ipolA->xdivs),isize,1,fp);
		fwrite(&(ipolA->xmin),dsize,1,fp);
		fwrite(&(ipolA->xmax),dsize,1,fp);
		fwrite(&(ipolA->dx),dsize,1,fp);
		/* write the tables */
		A=ipolA->table;
		if (ntype>1) B=ipolB->table;
		for (j=0; j<=ipolA->xdivs; j++) {
		    fwrite(A++,dsize,1,fp);
		    if (ntype>1) fwrite(B++,dsize,1,fp);
		}
	    } else {	/* itype==2 */
		/* write table sizes */
		fwrite(&(ipol2A->xdivs),isize,1,fp);
		fwrite(&(ipol2A->xmin),dsize,1,fp);
		fwrite(&(ipol2A->xmax),dsize,1,fp);
		fwrite(&(ipol2A->dx),dsize,1,fp);
		fwrite(&(ipol2A->ydivs),isize,1,fp);
		fwrite(&(ipol2A->ymin),dsize,1,fp);
		fwrite(&(ipol2A->ymax),dsize,1,fp);
		fwrite(&(ipol2A->dy),dsize,1,fp);
		/* write the tables */
		for (j=0; j<=ipol2A->xdivs; j++) {
		    A=ipol2A->table[j];
		    if (ntype>1) B=ipol2B->table[j];
		    for (k=0; k<=ipol2A->ydivs; k++) {
			fwrite(A++,dsize,1,fp);
			if (ntype>1) fwrite(B++,dsize,1,fp);
		    }
		}
	    }
	}
	fclose(fp);
}

void TabChannel_READ(elm,action)
Element *elm;
Action	*action;
{
int	i,j,k,n=0,ttype = 0,itype,ntype,doflip;
int	dsize,isize;
char	version;
FILE	*fp,*fopen();
Tchan	*chan1;
Tcurr	*curr;
Interpol *ipolA,*ipolB;
Interpol2D *ipol2A,*ipol2B;
double	v,*A,*B = NULL;
int	tabiread();
double	tabfread();

	if (action->argc != 1){
	    Error();
	    printf(" usage: TABREAD filename\n");
	    return;
	}
	dsize=sizeof(double);
	isize=sizeof(int);
	/* identify table type */
	if (strcmp(elm->object->name,"tabchannel")==0) {
	    ttype=TABCHAN_T;
	} else if (strcmp(elm->object->name,"tab2Dchannel")==0) {
	    ttype=TAB2CHAN_T;
	} else if (strcmp(elm->object->name,"tabcurrent")==0) {
	    ttype=TABCURR_T;
	} else if (strcmp(elm->object->name,"tabflux")==0) {
	    ttype=TABFLUX_T;
	} else {
	    Error();
	    printf(" TABREAD bug #1\n");
	}
	/* count number of tables we need to save */
	switch (ttype) {
	    case TABCHAN_T:
	    case TAB2CHAN_T:
		chan1=(Tchan *)elm;
		if (chan1->X_alloced) n++;
		if (chan1->Y_alloced) n++;
		if (chan1->Z_alloced) n++;
		break;
	
	    case TABCURR_T:
		curr=(Tcurr *)elm;
		if (curr->alloced) n++;
		break;

	    default:
		break;
	}
	if (n==0) {	/* nothing to read */
	    Error();
	    printf(" no tables allocated in %s\n",Pathname(elm));
	    return;
	}
	/* open the file */
	if (!(fp = fopen(action->argv[0],"r"))) {
	    Error();
	    printf(" can't open file '%s'\n",action->argv[0]);
	    return;
	}
	/* read header: in versions 1-3 this was an integer, now it is a byte */
	for (i=0; i<4; i++) {
	    fread(&version,1,1,fp);
	    if (version>0) break;
	}
	if ((version<1)||((version==2)&&(ttype==TABCURR_T))||(version>4)) {
	    Error();
	    printf(" can't read file '%s': wrong version #%d\n",action->argv[0],version);
	    fclose(fp);
	    return;
	}
	if (version>=4) {
	/* test for binary type */
	    fread(&v,dsize,1,fp);
	    doflip=(v!=4.0);
	} else {
	    doflip=0;
	}
	i=tabiread(fp,doflip);
	if (i!=ttype) {
	    Error();
	    printf(" can't read file '%s': wrong object type (%d)\n",action->argv[0],i);
	    fclose(fp);
	    return;
	}
	i=tabiread(fp,doflip);
	if (i!=n) {
	    Error();
	    printf(" can't read file '%s': wrong number of tables (%d)\n",action->argv[0],i);
	    fclose(fp);
	    return;
	}
	/* loop through all the tables */
	for (i=0; i<n; i++) {
	    TabChannel_ptrs(elm,i,ttype,&itype,&ntype,&ipolA,&ipolB,&ipol2A,&ipol2B);
	    k=tabiread(fp,doflip);
	    if (k!=itype) {
		Error();
		printf("can't read file '%s': wrong dimension of table (%d: %d)\n",action->argv[0],itype,k);
		fclose(fp);
		return;
	    }
	    if (version>1) {
		k=tabiread(fp,doflip);
		if (k!=ntype) {
		    Error();
		    printf("can't read file '%s': wrong type of table (%d: %d)\n",action->argv[0],ntype,k);
		    fclose(fp);
		    return;
		}
	    } else {
		ntype=2;	/* two tables */
	    }
	    if (itype==1) {
		/* read table sizes */
		k=tabiread(fp,doflip);
		if ((k!=ipolA->xdivs) || 
		    ((ttype<TABCURR_T)&&(k!=ipolB->xdivs))) {
		    Error();
		    printf("can't read file '%s': wrong xdivs for table (%d: %d)\n",action->argv[0],k,ipolA->xdivs);
		    fclose(fp);
		    return;
		}
		ipolA->xmin=tabfread(fp,doflip);
		ipolA->xmax=tabfread(fp,doflip);
		ipolA->dx=tabfread(fp,doflip);
		ipolA->invdx=1.0/ipolA->dx;
		if (ntype>1) {
		    ipolB->xmin=ipolA->xmin;
		    ipolB->xmax=ipolA->xmax;
		    ipolB->dx=ipolA->dx;
		    ipolB->invdx=1.0/ipolA->dx;
		}
		/* read the tables */
		A=ipolA->table;
		if (ntype>1) B=ipolB->table;
		if (version==1) {
		    k=ipolA->xdivs-1;
		} else {	/* small bug corrected in version 2 */
		    k=ipolA->xdivs;
		}
		for (j=0; j<=k; j++) {
		    *A++=tabfread(fp,doflip);
		    if (ntype>1) *B++=tabfread(fp,doflip);
		}
	    } else {	/* itype==2 */
		/* read table sizes */
		k=tabiread(fp,doflip);
		if ((k!=ipol2A->xdivs) || ((n>1)&&(k!=ipol2B->xdivs))) {
		    Error();
		    printf(" can't read file '%s': wrong xdivs for table (%d: %d)\n",action->argv[0],k,ipol2A->xdivs);
		    fclose(fp);
		    return;
		}
		ipol2A->xmin=tabfread(fp,doflip);
		ipol2A->xmax=tabfread(fp,doflip);
		ipol2A->dx=tabfread(fp,doflip);
		ipol2A->invdx=1.0/ipol2A->dx;
		k=tabiread(fp,doflip);
		if ((k!=ipol2A->ydivs) || ((ntype>1)&&(k!=ipol2B->ydivs))) {
		    Error();
		    printf(" can't read file '%s': wrong ydivs for table (%d: %d)\n",action->argv[0],k,ipol2A->ydivs);
		    fclose(fp);
		    return;
		}
		ipol2A->ymin=tabfread(fp,doflip);
		ipol2A->ymax=tabfread(fp,doflip);
		ipol2A->dy=tabfread(fp,doflip);
		ipol2A->invdy=1.0/ipol2A->dy;
		if (ntype>1) {
		    ipol2B->xmin=ipol2A->xmin;
		    ipol2B->xmax=ipol2A->xmax;
		    ipol2B->dx=ipol2A->dx;
		    ipol2B->invdx=1.0/ipol2A->dx;
		    ipol2B->ymin=ipol2A->ymin;
		    ipol2B->ymax=ipol2A->ymax;
		    ipol2B->dy=ipol2A->dy;
		    ipol2B->invdy=1.0/ipol2A->dy;
		}
		/* read the tables */
		for (j=0; j<=ipol2A->xdivs; j++) {
		    A=ipol2A->table[j];
		    if (ntype>1) B=ipol2B->table[j];
		    for (k=0; k<=ipol2A->ydivs; k++) {
			*A++=tabfread(fp,doflip);
			if (ntype>1) *B++=tabfread(fp,doflip);
		    }
		}
	    }
	}
	fclose(fp);
}

/* reads integer value from file and flips it if needed */
int tabiread(fp,doflip)
FILE	*fp;
int	doflip;
{
int     n=sizeof(int);
int	val1,val2;
char  	*pval1,*pval2;

	if (doflip) {
	    fread(&(val1),n,1,fp);
	    pval1=(char *)(&(val1))+n-1;
	    pval2=(char *)(&(val2));
	    for (;n>0;n--) *pval2++=*pval1--;
	} else {
	    fread(&(val2),n,1,fp);
	}
	return(val2);
}

/* reads double value from file and flips it if needed */
double tabfread(fp,doflip)
FILE	*fp;
int	doflip;
{
int     n=sizeof(double);
double	val1,val2;
char  	*pval1,*pval2;

	if (doflip) {
	    fread(&(val1),n,1,fp);
	    pval1=(char *)(&(val1))+n-1;
	    pval2=(char *)(&(val2));
	    for (;n>0;n--) *pval2++=*pval1--;
	} else {
	    fread(&(val2),n,1,fp);
	}
	return(val2);
}


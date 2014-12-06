static char rcsid[] = "$Id: NIH_parser.c,v 1.2 2005/07/01 10:03:09 svitak Exp $";

/*
** $Log: NIH_parser.c,v $
** Revision 1.2  2005/07/01 10:03:09  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2001/06/29 21:32:22  mhucka
** You don't need 0's in some of the format strings.
**
** Revision 1.4  2000/06/19 06:11:09  mhucka
** The "end" variable in do_read_NIH() needs to be an int, not a short.
**
** Revision 1.3  2000/06/12 04:56:15  mhucka
** Fixed trivial errors in some printf format strings.
**
** Revision 1.2  2000/05/29 02:14:00  mhucka
** The #undef's for MAXORDER and MAXSEGMENT had trailing values, which is
** actually not conformant to the ISO standard.  I have no idea why it took
** this long before someone noticed the compiler warnings.
**
** Revision 1.1  1992/12/11 19:05:45  dhb
** Initial revision
**
*/

#include "copyright.h"
#include <stdio.h>
#include <math.h>
#include "sim_ext.h"

#define MAXORDER 30
#define MAXSEGMENT 9

/* Converts NIH/NINCDS-format morphology file into a GENESIS
**	.p file.  
** All data lines are of the format:
**		ORDER  BRANCH  SEGMENT  len  dia
**		(len and dia in um)
** File may contain comment lines, as long as they do not start with
**	a number.  The first line should be the soma, with ORDER=BRANCH=
**	SEGMENT=0 and 'len'='dia'=the radius of the corresponding sphere.
** The tree organization is preserved by making each order a
**	new level in the GENESIS element tree. */

void do_read_NIH(argc,argv)
	int argc;
	char	**argv;
{
	int		i,j,nc = 1,ns = 0,nargs = 1;
	char	rawline[201];
	char	*line;
	FILE	*finp,*foutp,*fopen();
	char	*endit;
	float	len,dia;
	float	x,y,z;
	double	g;
	/* the compartment id-numbers as they are used in the NIH-files */
	int		oldorder = 0,oldbranch = 0,oldsegment = 0,maxtree = 0;
	int		order,branch,segment,newid;
	int		lastbranch[MAXORDER],lastseg[MAXORDER],lastleft[MAXORDER];
	int     end;
	short	firstline = 1,left,oldleft = 1,longnames = 1;
	char	name[100],parname[100],ordname[20];

	if  (argc < 3) {
		fprintf(stderr,"usage : %s [-o] [-s] NIH_filename p_filename\n",argv[0]);
		fprintf(stderr,"        -o: copy dendritic tree hierarchy into element\n");
		fprintf(stderr,"            hierarchy up to this order (max=10)\n");
		fprintf(stderr,"        -s: output short compartment names\n");
		return;
	}
	/* search for option strings */
	if (strcmp(argv[1],"-o") == 0) {
		maxtree = atoi(argv[2]);
		nargs += 2;
	}
	if (strcmp(argv[1],"-s") == 0) {
		longnames = 0;
		maxtree = 0;
		nargs += 1;
	}
	
	if  (argc < nargs) 
		fprintf(stderr,"usage : %s [-o] [-s] NIH_filename p_filename\n",argv[0]);

	fprintf(stderr,"%d%d%d\n",maxtree,nargs,argc);
	if (!(finp = fopen(argv[nargs],"r"))) {
		fprintf(stderr,"can't open file '%s'\n",argv[nargs]);
		return;
	}
	if (!(foutp = fopen(argv[nargs+1],"w"))) {
		fprintf(stderr,"can't open file '%s' for writing\n",argv[nargs+1]);
		return;
	}

	fprintf(stderr,"reading '%s'.... \n",argv[nargs]);
	fprintf(stderr,"writing '%s'.... \n",argv[nargs+1]);
	fprintf(foutp,"// Original file: '%s'\n\n",argv[nargs]);
	fprintf(foutp,"*symmetric\n");
	fprintf(foutp,"*relative\n\n");

	/* read till end of file */
	for (i = 1, endit = fgets(rawline,100,finp); endit ; endit = fgets(rawline,100,finp), i++ ) {
		
		for (line = rawline; (*line == ' ' || *line == '	') ; line++);

	/* examine line */
		switch (*line) {
			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' : break;	/* numeric data, assume compartment line */
			default : continue; /* empty line or comment */
		}

	/* is a data line, scan contents */
		nargs = sscanf(line, "%d%d%d%f%f%i",
					&order,&branch,&segment,&len,&dia,&end);
		if (nargs < 5) {
		/* Not enough inputs matched. */
			fprintf(stderr,"error on line %d : missing fields\n",i);
			break;
		}
	
	/* check whether legal input values */
		if ((len <= 0.0) || (dia <= 0.0)) {
		/* Bad values */
			fprintf(stderr,"error on line %d : bad length or diameter\n",i);
			break;
		}
		if (order > MAXORDER) {
		/* Too big */
			fprintf(stderr,"error on line %d : order too big\n",i);
			break;
		}
		if (segment > MAXSEGMENT) {
		/* Too big */
			fprintf(stderr,"error on line %d : segment too big\n",i);
			break;
		}

	/* make a soma */
		if (firstline) {
		/* should be the soma */
			if ((order == 0) && (branch == 0)  && (segment == 0)) {
				/* is soma */
				fprintf(foutp,"soma  none 0  0 %f  %f\n",len,dia);
				firstline = 0;
				continue;
			}
			else {
				fprintf(stderr,"error on line %d : no soma\n",i);
				break;
			}
		}

	/* left or right branch? */
		left=(order+branch)%2;

	/* check whether consistent order of data */
		if (order > oldorder) {
			if (order > oldorder + 1) {
			/* We are missing a branching point  */
				fprintf(stderr,"error on line %d : skipped order\n",i);
				break;
			}
			if (longnames)
				lastbranch[oldorder] = oldbranch;
			else
				lastbranch[oldorder] = ns;
			lastseg[oldorder] = oldsegment;
			lastleft[oldorder] = oldleft;
			oldsegment=0;
			ns += 1;	/* counts continuous branches */
		}
		else if (order < oldorder) {
			oldsegment = 0;
			ns += 1;	/* counts continuous branches */
		}
		else {		/* order = oldorder */
			if (left != oldleft) {
				if (branch != oldbranch + 1) {
				/* BranchID not correct for this order! */
					fprintf(stderr,"error on line %d : bad branchID\n",i);
					break;
				}
				oldsegment = 0;
				ns += 1;	/* counts continuous branches */
			}
		}

		if (segment != oldsegment + 1) {
		/* We are missing or duplicating a segment */
			fprintf(stderr,"error on line %d : bad segment number\n",i);
			break;
		}
		if ((oldsegment > 0) && (branch != oldbranch)) {
		/* Changed branchID before finishing segments! */
			fprintf(stderr,"error on line %d : segment too big\n",i);
			break;
		}

	/* count compartments */
		nc += 1;

	/* prepare for output */
		/* make a name for the compartment and its parent */
		parname[0]=(char) 0;	/* clear old name */
		/* make an element tree for parent if requested */
		if (maxtree>0) {
			strcat(parname,"soma");
			for (j = 0; (j < order) && (j < maxtree) ; j++) {
				newid=lastleft[j]*10 + lastseg[j];
				sprintf(ordname,"/d%2.2d.%2.2d",j,newid);
				strcat(parname,ordname);
			}
		}
		/* the (empty) tree name is common to parent and comp */
		strcpy(name,parname);
		/* complete parname if it is needed */
		if (segment == 1) {
			/* add 'flat' name to parent if necessary */
			newid = order - 1;
			if ((order == 0)  && (maxtree == 0)) 
				strcat(parname,"soma");
			else if (longnames && (newid >= maxtree)) {
				sprintf(ordname,"den%2.2d.%d.%d",newid,
							lastbranch[newid],lastseg[newid]);
				strcat(parname,ordname);
			}
			/* make a short name */
			else 
				sprintf(parname,"d%2.2d.%d.%d",newid,
							lastbranch[newid],lastseg[newid]);
			
		}
		/* add the compartment name to the common name */
		if (order < maxtree) {
			newid=left*10 + segment;
			sprintf(ordname,"/d%2.2d.%2.2d",order,newid);
		}
		else if (longnames)
			sprintf(ordname,"den%2.2d.%d.%d",order,branch,segment);
		/* make a short name */
		else 
			sprintf(ordname,"d%2.2d.%d.%d",order,ns,segment);
		strcat(name,ordname);
	
	/* we create a fake morphology: let all branches go up by
	** 15+order*2.5 +/- 15 degrees in the XZ plane and vary Y by +/-2 */	
		if (len > 3.0)
			y = frandom(-2.0,2.0);
		else
			y = frandom (-len/3,len/3);
		len = sqrt(len*len - y*y);
		g = 0.2617 + (0.0436 * (float) order) + frandom(-0.2617,0.2617);
		x = len * cos(g);
		z = len * sin(g);
		if (left) x = -x;
		
	/* output result */
		if (segment == 1) {
			fprintf(foutp,"%s  %s  %f  %f  %f  %f\n",name,parname,
														x,y,z,dia);
		}
		else {
			fprintf(foutp,"%s  .  %f  %f  %f  %f\n",name,x,y,z,dia);
		}
	
	/* store old numbers */
		oldorder = order;
		oldbranch = branch;
		oldsegment = segment;
		oldleft = left;

	}	/* end of for loop */
	if (!endit) fprintf(stderr,"OK, %d compartments\n",nc); /* clean end of for loop */
	fclose(finp);
	fclose(foutp);
}

#undef MAXORDER
#undef MAXSEGMENT

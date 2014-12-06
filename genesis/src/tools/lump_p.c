static char rcsid[] = "$Id: lump_p.c,v 1.4 2006/01/09 16:28:50 svitak Exp $";

/*
** $Log: lump_p.c,v $
** Revision 1.4  2006/01/09 16:28:50  svitak
** Increases in size of storage for comparment names. These can be very long
** when cvapp converts from Neurolucida format.
**
** Revision 1.3  2005/07/20 20:02:04  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1996/08/06 21:05:05  dhb
** Replaced uses of {get|set}_script_float() with
** {Get|Set}ScriptDouble().
**
** Revision 1.1  1992/12/11  19:05:56  dhb
** Initial revision
**
*/

/* Erik De Schutter, Caltech 2/91 */

#include <math.h>
#include <stdio.h>
#include "sim_ext.h"
#include "tools.h"
#include "seg_struct.h"

#define NAMELEN 1024
#define NEW_CELL 0x01
#define RELATIVE 0x02
#define POLAR 0x04
#define SPHERICAL 0x20

#define DATA_MODE 1
#define FLAG_MODE 2
#define COMMENT_MODE 3
#define SCRIPT_MODE 4
#define ERR -1
#define INPUT 0
#define VOLTAGE 0
#define MAX_NCHANS 10
#define MAX_LINELEN 80
#define MAX_LINES 11
/* These fields are set from the cell definition file */
static float RM,CM,RA,MAX_FRACT,MIN_D,MAX_LAMBDA;
static char	*chomp_leading_spaces();
int	fill_arglist();

static void lump_script(line,lineno,flags)
	char	*line;
	int	lineno;
	int	*flags;
{
	char	command[NAMELEN];
	char	field[NAMELEN];
	float	value;
	int	nargs;

	nargs = sscanf(line,"%s %s %f",command,field,&value);
	if (nargs == 3 && strcmp(command,"*set_global") == 0) {
		/* setting a global */
		if (strcmp(field,"RM") == 0)
			RM = value;
		if (strcmp(field,"RA") == 0)
			RA = value;
		if (strcmp(field,"CM") == 0)
			CM = value;
	} else if (nargs == 1) /* just setting flags */ {
		if (strcmp(command,"*relative") == 0)
			*flags |= RELATIVE;
		else if (strcmp(command,"*absolute") == 0)
			*flags &= ~RELATIVE;
		else if (strcmp(command,"*polar") == 0)
			*flags |= POLAR;
		else if (strcmp(command,"*cartesian") == 0)
			*flags &= ~POLAR;
		else if (strncmp(command,"*spherical",6) == 0)
			*flags |= SPHERICAL;
		else if (strncmp(command,"*cylindrical",7) == 0)
			*flags &= ~SPHERICAL;
	}
}

void do_lump_cell(argc,argv)
	int argc;
	char	**argv;
{
	int	i,nc = 0,rowno = 1,no = 0;
	int	len;
	char	rawline[201];
	char	*line = NULL;
	FILE	*fp,*fn,*fopen();
	int	flags = 0;
	int	parse_mode = DATA_MODE;
	double	GetScriptDouble();
	char	*endit;

	if (argc < 6) {
	    fprintf(stderr,"usage : %s filename newfile max_fract min_d max_lambda\n",argv[0]);
	    fprintf(stderr,"Lumps together all unbranched segments with diameter\n");
	    fprintf(stderr,"smaller than min_d or a maximum difference in diameter\n");
	    fprintf(stderr,"smaller than max_fract (relative to smallest diameter)\n");
	    fprintf(stderr,"and the new lambda smaller than max_lambda.\n");
	    fprintf(stderr,"Assumes that unbranched segments have parent=='.', while ALL\n");
	    fprintf(stderr,"first comps after branch point should have a named parent.\n");
	    fprintf(stderr,"New diameter is average dia, new length conserves total membrane surface.\n");

	    return;
	}
	if (!(fp = fopen(argv[1],"r"))) {
	    fprintf(stderr,"can't open file '%s'\n",argv[1]);
	    return;
	}
	if (!(fn = fopen(argv[2],"w"))) {
	    fprintf(stderr,"can't open file '%s' for writing\n",argv[2]);
	    return;
	}

	MAX_FRACT=Atof(argv[3]);
	MIN_D=Atof(argv[4]);
	MAX_LAMBDA=Atof(argv[5]);
	/* getting values from script */
	CM = GetScriptDouble("CM");
	RM = GetScriptDouble("RM");
	RA = GetScriptDouble("RA");

	for (i = 1, endit = fgets(rawline,200,fp); endit ; endit = fgets(rawline,200,fp), i++ ) {
	    line = chomp_leading_spaces(rawline);
	    len = strlen(line);
	    while (line[len-2] == '\\' && endit) {
		line[len-2] = ' ';
		endit = fgets(rawline,200,fp);
		strcpy(line+len-1,rawline);
		i++;
	    }

	    switch (*line) {
		case '\n' :
		case '\r' :
		case '\0' :
		    fprintf(fn,line);
		    continue;
		case '/' :
		    if (line[1] == '/') {
			fprintf(fn,"%s",line);
			continue;
		    }
		    if (line[1] == '*')
			parse_mode = COMMENT_MODE;
		    else
			parse_mode = DATA_MODE;
		    break;
		case '*' :
		    if (line[1] == '/') {
			parse_mode = DATA_MODE;
			fprintf(fn,"%s",line);
			continue;
		    }
		    parse_mode = SCRIPT_MODE;
		    break;
		default :
		    break;
	    }

	    switch(parse_mode) {
		    case DATA_MODE :
		    if (lump_data(fn,line,i,flags,&rowno,&no) == ERR) {
			return;	
		    }
		    else
			nc += 1;
		    break;
		case SCRIPT_MODE :
		    lump_script(line,i,&flags);
		    fprintf(fn,"%s",line);
		    parse_mode = DATA_MODE;
		    break;
		case COMMENT_MODE :
		    len = strlen(line);
		    if (line[len-2] == '*' && line[len-1] == '/')
			parse_mode = DATA_MODE;
		    fprintf(fn,"%s",line);
		    break;
		default :
		    break;
	    }
	}
	i=0;		/* send end of file to lump_data */
	if (lump_data(fn,line,i,flags,&rowno,&no) == ERR) {
	}
	fclose(fp);
	fclose(fn);
	fprintf(stderr,"%s input: %d compartments.\n",argv[1],nc);
	fprintf(stderr,"%s output: %d compartments.\n",argv[2],no);
}


int lump_data(fn,line,lineno,flags,n,no)
	FILE	*fn;
	char	*line;
	int	lineno;
	int	flags;
	int	*n,*no;
{
	static float	x[MAX_LINES],y[MAX_LINES],z[MAX_LINES];
	static float	l[MAX_LINES],d[MAX_LINES];
	static char	ch[MAX_LINES][MAX_NCHANS][20];
	static float	dens[MAX_LINES][MAX_NCHANS];
	static char	name[MAX_LINES][NAMELEN];
	static char	parent[MAX_LINES][NAMELEN];
	static int	nargs[MAX_LINES];
	static float	max_d,min_d;
	float	lambda = 0.0,rel_d;
	int	i,j,f,k,m;
	short	output=0;

	if (lineno > 0) {
	    nargs[*n] = sscanf(line,
		"%s%s%f%f%f%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f",
		name[*n],parent[*n],&x[*n],&y[*n],&z[*n],&d[*n],
		ch[*n][0],&dens[*n][0],ch[*n][1],&dens[*n][1],ch[*n][2],
		&dens[*n][2],ch[*n][3],&dens[*n][3],ch[*n][4],
		&dens[*n][4],ch[*n][5],&dens[*n][5], ch[*n][6],
		&dens[*n][6],ch[*n][7],&dens[*n][7],ch[*n][8]
		,&dens[*n][8],ch[*n][9],&dens[*n][9]);
	
	    if (nargs[*n] < 6) {
	    /* Not enough inputs matched. */
		fprintf(stderr,"error on line %d :\n", lineno);
		fprintf(stderr,"	%s\nFields incorrect\n",line);
	        return(ERR);
	    }

	    if (flags & POLAR) {
		fprintf(stderr,"error: polar mode is not supported\n");
		return(ERR);
	    }
	    if (!(flags & RELATIVE)) {
		fprintf(stderr,"error: absolute mode is not supported\n");
		return(ERR);
	    }

	    if (flags & SPHERICAL) {
		output = 1;
	    }
	    if ((*n > 1) && (strcmp(parent[*n],".")) != 0) { 
		output = 1;  /* output preceding comps */
	    }
	    if (*n > 1) {
		/* compare diameters */
		if (d[*n] < min_d) min_d = d[*n];
		if (d[*n] > max_d) max_d = d[*n];
		rel_d = (max_d - min_d) / min_d;
		if ((max_d > MIN_D) && (rel_d > MAX_FRACT))
		    output = 1;
		if (*n == MAX_LINES)
		    output = 1;
	    }
	    if ((*n == 1) || output) {
		/* reset diameters */
		min_d = d[*n];
		max_d = d[*n];
	    }
	} else {	/* end of file */
	    output = 1;
	}

	/* compute the new comp in line zero */
	if (output == 1) {
	    f =  1;	/* lumped range is f to k */
	    k = *n - 1;
	    for (;;) {
	        if (k > f) {
		    d[0] = 0.0;
		    l[0] = 0.0;
		    for (j=0; j<=MAX_NCHANS; j+=1) 
			dens[0][j] = 0.0;
		    for (i=f; i<=k; i+=1) {
			d[0] += d[i];
			l[i] = sqrt(x[i]*x[i] + y[i]*y[i] + z[i]*z[i]);
			l[0] += d[i] * l[i] * PI;  /* surface */
			for (j=0; j<=(nargs[i]-8)/2; j+=1) 
			    dens[0][j] += dens[i][j];
		    }
		    d[0] = d[0] / (double)(k-f+1);	/* average */
		    l[0] = l[0] / (d[0] * PI);
		    for (j=0; j<=(nargs[f]-8)/2; j+=1) {
			dens[0][j] = dens[0][j] / (double)(k-f+1);
		    }
		    lambda = l[0] / (1000*sqrt(RM * d[0] * 0.25 / RA));
		    x[0] = sqrt(l[0]*l[0] * x[f]*x[f] / (l[f]*l[f]));
		    y[0] = sqrt(l[0]*l[0] * y[f]*y[f] / (l[f]*l[f]));
		    z[0] = sqrt(l[0]*l[0] * z[f]*z[f] / (l[f]*l[f]));
		    m = 0;	/* index of output values */
		} else {	/* only one line -> output it */
		    m = f;
		    k = f;
		}
		if ((k == f) || (lambda <= MAX_LAMBDA)) {	
		/* output it! */
	 	    /* orient in same direction as first compartment */
		    fprintf(fn,"%s  %s  %f  %f  %f  %f",name[k],parent[f],x[m],y[m],z[m],d[m]);
		    for (j=0; j<=(nargs[f]-8)/2; j+=1) {
		    	fprintf(fn,"  %s  %f",ch[f][j],dens[m][j]);
		    }
		    fprintf(fn,"\n");
		    *no += 1;		/* count output lines */
		    if (k >= (*n-1)) 
			break;		/* done, leave for loop*/
		    else {
			f = k+1;	/* do next part */
			k = *n-1;
		    }    
		} else {
		  k = k - 1;		/* average shorter part */
		}
	    }
	    if ((*n > 1) && (lineno > 0)) {
		/* not end of file, shift last entry */
	        nargs[1] = sscanf(line,
		"%s%s%f%f%f%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f",
		name[1],parent[1],&x[1],&y[1],&z[1],&d[1],ch[1][0],
		&dens[1][0],ch[1][1],&dens[1][1],ch[1][2],&dens[1][2],
		ch[1][3],&dens[1][3],ch[1][4],&dens[1][4],ch[1][5],
		&dens[1][5],ch[1][6],&dens[1][6],ch[1][7],&dens[1][7],
		ch[1][8],&dens[1][8],ch[1][9],&dens[1][9]);
		*n = 2;
	    } else {
		*n = 1;
	    }
	} else {
	    *n += 1;
	}
	return(1);
}

static char *chomp_leading_spaces(line)
	char	*line;
{
	char	*str;

	for(str = line; (*str == ' ' || *str == '	') ; str++);

	return(str);
}


static char rcsid[] = "$Id: lftab.c,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $";

/*
** $Log: lftab.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2001/06/29 21:32:50  mhucka
** Added extra parens inside conditionals and { } groupings to quiet certain
** compiler warnings.
**
** Revision 1.1  1994/08/08 22:17:27  dhb
** Initial revision
**
 * Revision 1.1  1994/06/13  22:45:21  bhalla
 * Initial revision
*/


/*
** lftab.c : load_file_or_tab.c. Utility function for several
** of the match commands.
** By Upinder S. Bhalla, 1994, Mt. Sinai.
*/

#include "sim_ext.h"

#define MAXPTS 5000

int load_file_or_table(name,px,py)
	char	*name;
	float	**px,**py;
{
	Element *elm;
	int i;
	float *x,*y;

	elm = GetElement(name);
	if (elm) {
		short datatype;
		Interpol **pip;

		if (strcmp(elm->object->type,"table_type") != 0) {
			Error();
			printf("Element %s is not a table object\n",name);
			return(0);
		}

		if ((pip  = (Interpol **)GetFieldAdr(elm,"table",&datatype))) {
			if (datatype == INVALID) {
				double dt = ((*pip)->xmax - (*pip)->xmin)/(*pip)->xdivs;
				double currt = ((*pip)->xmin);
				if (!(*pip && (*pip)->allocated)) {
				/* The interpol has not been allocated */
					Error();
				printf("table on element %s has not been allocated\n",
					name);
					return(0);
				}
				*px = x = (float *)calloc(MAXPTS,sizeof(float));
				*py = y = (float *)calloc(MAXPTS,sizeof(float));
				for (i = 0; i <= (*pip)->xdivs && i < MAXPTS; i++) {
					y[i] = (*pip)->table[i];
					x[i] = currt;
					currt += dt;
				}
				return(i);
			}
		}
	} else {
		FILE *fp;
		if (!(fp = fopen(name,"r"))) {
			printf("Error : could not open file or table %s\n",name);
			return(0);
		}
		*px = x = (float *)calloc(MAXPTS,sizeof(float));
		*py = y = (float *)calloc(MAXPTS,sizeof(float));
		for(i=0;i < MAXPTS &&
			EOF != fscanf(fp,"%f %f",&(x[i]),&y[i]) ; i++);

		fclose(fp);
		return(i);
	}
	return(0);
}

static char rcsid[] = "$Id: affdelay.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: affdelay.c,v $
** Revision 1.3  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:05  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:03:35  dhb
** Initial revision
**
*/

#include <math.h>
#include "per_ext.h"

/* 9/88 Matt Wilson */
int AffDelay(argc,argv)
int argc;
char **argv;
{
Projection *projection;
Connection *connection;
float	x_dst,y_dst;
float	vx = 0.0;
float	vy = 0.0;
float	tx;
float	ty;
float	tan_theta;
float	d;
float	angle;
int	i;
int	vxmode,vymode;
float	vxmean = 0.0,vxvar = 0.0;
float	vymean = 0.0,vyvar = 0.0;
float	vxlower = 0.0,vxupper = 0.0;
float	vylower = 0.0,vyupper = 0.0;
float	sd;
ElementList	*list;
char 	*path;
int	nxtarg;

    if(argc < 4){
	Error();
	printf("usage: %s path angle vx vy\n",argv[0]);
	printf("usage: %s path [-vx vx][-uniformvx low high][-gaussianvx mean sd low high][-expvx low high]\n",argv[0]);
	printf("[-vy vy][-uniformvy low high][-gaussianvy mean sd low high][-expvy low high]\n");
	return(0);
    }
    path = argv[1];
    angle = Atof(argv[2]);
    tan_theta = tan(2*M_PI*angle/360);
    vxmode = -1;
    vymode = -1;
    nxtarg = 2;
    while(++nxtarg < argc){
	/* 
	** get the velocities along the main tract 
	*/
	if(strcmp(argv[nxtarg],"-vx") == 0){
	    vxmode = 0;
	    if((vx = Atof(argv[++nxtarg])) <= 0){
		Error();
		printf("illegal velocity vx = %e\n",vx);
		return(0);
	    }
	    continue;
	} 
	if(strcmp(argv[nxtarg],"-uniformvx") == 0){
	    vxmode = 1;
	    vxlower = Atof(argv[++nxtarg]);
	    vxupper = Atof(argv[++nxtarg]);
	    if(vxlower <= 0 || vxupper <= 0){
		Error();
		printf("illegal velocity range = %e to %e\n",vxlower,vxupper);
		return(0);
	    }
	    continue;
	} 
	if(strcmp(argv[nxtarg],"-gaussianvx") == 0){
	    vxmode = 2;
	    vxmean = Atof(argv[++nxtarg]);
	    sd = Atof(argv[++nxtarg]);
	    vxlower = Atof(argv[++nxtarg]);
	    vxupper = Atof(argv[++nxtarg]);
	    vxvar = sd*sd;
	    if(vxlower <= 0 || vxupper <= 0){
		Error();
		printf("illegal velocity range = %e to %e\n",vxlower,vxupper);
		return(0);
	    }
	    continue;
	} 
	if(strcmp(argv[nxtarg],"-expvx") == 0){
	    vxmode = 3;
	    vxlower = Atof(argv[++nxtarg]);
	    vxupper = Atof(argv[++nxtarg]);
	    if(vxlower <= 0 || vxupper <= 0){
		Error();
		printf("illegal velocity range = %e to %e\n",vxlower,vxupper);
		return(0);
	    }
	    continue;
	}
	/* 
	** get the velocities along the collaterals 
	*/
	if(strcmp(argv[nxtarg],"-vy") == 0){
	    vymode = 0;
	    if((vy = Atof(argv[++nxtarg])) <= 0){
		Error();
		printf("illegal velocity vy = %e\n",vy);
		return(0);
	    }
	    continue;
	} 
	if(strcmp(argv[nxtarg],"-uniformvy") == 0){
	    vymode = 1;
	    vylower = Atof(argv[++nxtarg]);
	    vyupper = Atof(argv[++nxtarg]);
	    if(vylower <= 0 || vyupper <= 0){
		Error();
		printf("illegal velocity range = %e to %e\n",vylower,vyupper);
		return(0);
	    }
	    continue;
	} 
	if(strcmp(argv[nxtarg],"-gaussianvy") == 0){
	    vymode = 2;
	    vymean = Atof(argv[++nxtarg]);
	    sd = Atof(argv[++nxtarg]);
	    vyvar = sd*sd;
	    vylower = Atof(argv[++nxtarg]);
	    vyupper = Atof(argv[++nxtarg]);
	    if(vylower <= 0 || vyupper <= 0){
		Error();
		printf("illegal velocity range = %e to %e\n",vylower,vyupper);
		return(0);
	    }
	    continue;
	} 
	if(strcmp(argv[nxtarg],"-expvy") == 0){
	    vymode = 3;
	    vylower = Atof(argv[++nxtarg]);
	    vyupper = Atof(argv[++nxtarg]);
	    if(vylower <= 0 || vyupper <= 0){
		Error();
		printf("illegal velocity range = %e to %e\n",vylower,vyupper);
		return(0);
	    }
	    continue;
	} 
	Error();
	printf("%s : invalid option '%s'\n",argv[0],argv[nxtarg]);
	return 0;
    }
    if(vxmode == -1 || vymode == -1){
	Error();
	printf("incomplete specification of afferent velocity parameters\n");
	return(0);
    }
    list = WildcardGetElement(path,1);
    for(i=0;i<list->nelements;i++){
	projection = (Projection *)list->element[i];
	switch(vxmode){
	case 0:			/* constant */
	    break;
	case 1:			/* uniform */
	    vx = frandom(vxlower,vxupper);
	    break;
	case 2:			/* gaussian */
	    do {
		vx = rangauss(vxmean,vxvar);
	    } while(vx < vxlower || vx > vxupper);
	    break;
	case 3:			/* exponential */
	    vx = -log(frandom(vxlower,vxupper));
	    break;
	}
	switch(vymode){
	case 0:			/* constant */
	    break;
	case 1:			/* uniform */
	    vy = frandom(vylower,vyupper);
	    break;
	case 2:			/* gaussian */
	    do {
		vy = rangauss(vymean,vyvar);
	    } while(vy < vylower || vy > vyupper);
	    break;
	case 3:			/* exponential */
	    vy = -log(frandom(vylower,vyupper));
	    break;
	}
	if(vx <= 0) vx = 0;
	if(vy <= 0) vy = 0;
	for(connection=projection->connection;connection;
	connection=connection->next){
	    x_dst = connection->target->x;
	    y_dst = connection->target->y;
	    d = MIN(x_dst, y_dst/tan_theta);
	    /*
	    ** calculate time delay along the main fiber tract
	    */
	    tx = (x_dst - d)/vx;
	    /*
	    ** calculate time delay along the angled collateral
	    */
	    ty = sqrt(y_dst*y_dst + d*d)/vy;
	    connection->delay = tx + ty;
	}
	printf(".");
	fflush(stdout);
    }
    printf("\n");
    FreeElementList(list);
    OK();
    return 1;
}


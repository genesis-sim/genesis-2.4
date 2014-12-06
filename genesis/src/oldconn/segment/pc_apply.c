static char rcsid[] = "$Id: pc_apply.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";

/*
** $Log: pc_apply.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:04:07  dhb
** Initial revision
**
*/

#include "seg_ext.h"

/* 5/88 Matt Wilson */
do_rand(argc,argv,projection,connection)
int argc;
char **argv;
Projection *projection;
Connection *connection;
{
float vel;

    if(connection == NULL) return(0);
    if(argc > 2){
	connection->weight =
	frandom(Atof(argv[1]),Atof(argv[2]));
    }
}

/* 5/88 Matt Wilson */
do_set_weight(argc,argv,projection,connection)
int argc;
char **argv;
Projection *projection;
Connection *connection;
{
float vel;

    if(connection == NULL) return(0);
    if(argc > 1){
	connection->weight = atoi(argv[1]);
    }
}


/* 8/88 Matt Wilson */
ExpConnectionField(argc,argv,projection,connection)
int argc;
char **argv;
Projection *projection;
Connection *connection;
{
float scale;
float rate;
char val[80];
float minval = 0;
char *field;

    if(projection == NULL || connection == NULL) return(0);
    if(argc < 4){
	printf("usage: %s field rate scale [minval]\n",argv[0]);
	return(0);
    }
    field = argv[1];
    scale = Atof(argv[2]);
    rate = Atof(argv[3]);
    if(argc > 4){
	minval = Atof(argv[4]);
    }
    /*
    ** set the delay according to the radial distance between
    ** the source and dst and the velocity
    */
    sprintf(val,"%f",scale*exp(rate*sqrt(
	pow(projection->parent->x - connection->target->parent->x,2.0) +
	pow(projection->parent->y - connection->target->parent->y,2.0)
	)) + minval);
    return(SetConnection(projection,connection,field,val));
}


/* 8/88 Matt Wilson */
InvRadialConnectionField(argc,argv,projection,connection)
int argc;
char **argv;
Projection *projection;
Connection *connection;
{
float scale;
char val[80];
char *field;

    if(projection == NULL || connection == NULL) return(0);
    if(argc < 3){
	printf("usage: %s field scale\n",argv[0]);
	return(0);
    }
    field = argv[1];
    scale = Atof(argv[2]);
    /*
    ** set the delay according to the radial distance between
    ** the source and dst and the velocity
    */
    sprintf(val,"%f",scale/sqrt(
	pow(projection->parent->x - connection->target->parent->x,2.0) +
	pow(projection->parent->y - connection->target->parent->y,2.0)
	));
    return(SetConnection(projection,connection,field,val));
}

/* 8/88 Matt Wilson */
RadialConnectionField(argc,argv,projection,connection)
int argc;
char **argv;
Projection *projection;
Connection *connection;
{
float scale;
char val[80];
char *field;

    if(projection == NULL || connection == NULL) return(0);
    if(argc < 3){
	printf("usage: %s field scale\n",argv[0]);
	return(0);
    }
    field = argv[1];
    scale = Atof(argv[2]);
    /*
    ** set the field according to the radial distance between
    ** the source and dst 
    */
    sprintf(val,"%f",sqrt(
	pow(projection->parent->x - connection->target->parent->x,2.0) +
	pow(projection->parent->y - connection->target->parent->y,2.0)
	) * scale);
    return(SetConnection(projection,connection,field,val));
}

/* 4/88 Matt Wilson */
do_manhattan_delay(argc,argv,projection,connection)
int argc;
char **argv;
Projection *projection;
Connection *connection;
{
float vel;

    if(projection == NULL || connection == NULL) return(0);
    if(argc > 1)
	vel = Atof(argv[1]);
    else
	vel = 1;
    if(vel == 0) vel = 1;
    /*
    ** set the delay according to the manhattan distance between
    ** the source and dst and the velocity
    */
    connection->delay = 
	(fabs(projection->parent->x - connection->target->parent->x) +
	fabs(projection->parent->y - connection->target->parent->y)) / vel;
}

/* 9/88 Matt Wilson */
XExpConnectionField(argc,argv,projection,connection)
int argc;
char **argv;
Projection *projection;
Connection *connection;
{
float scale;
float rate;
char val[80];
float minval = 0;
char *field;

    if(projection == NULL || connection == NULL) return(0);
    if(argc < 4){
	printf("usage: %s field rate scale [min]\n",argv[0]);
	return(0);
    }
    field = argv[1];
    scale = Atof(argv[2]);
    rate = Atof(argv[3]);
    if(argc > 4){
	minval = Atof(argv[4]);
    }
    /*
    ** set the delay according to the x distance between
    ** the source and dst and the scale
    */
    sprintf(val,"%f",scale*exp(rate*
	fabs(projection->parent->x - connection->target->parent->x)
	) + minval);
    return(SetConnection(projection,connection,field,val));
}




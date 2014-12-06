static char rcsid[] = "$Id: old_cell_parser.c,v 1.2 2006/01/09 16:28:50 svitak Exp $";

/*
** $Log: old_cell_parser.c,v $
** Revision 1.2  2006/01/09 16:28:50  svitak
** Increases in size of storage for comparment names. These can be very long
** when cvapp converts from Neurolucida format.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:06:02  dhb
** Initial revision
**
*/

/* Upi Bhalla, Caltech 11/90 */
/* Modified by Erik De Schutter, Caltech 11/90, 1/91
**  implemented symmetric compartments and axial diffusion pools */
/* Modified by Upi Bhalla, Caltech 4/91
**  implemented channel counting and in-line prototyping of compartments */
/* Modified by Erik De Schutter, Caltech  6/91
**  implemented dia field */
/* Modified by Erik De Schutter, Caltech  9/91
**  corrected membrane scaling for PASSIVE spines */

#include <stdio.h>
#include "sim_ext.h"
#include "tools.h"
#include "seg_struct.h"
#include "hh_struct.h"
#include "olf_struct.h"
#include "buf_struct.h"
#include "conc_struct.h"

#define NAMELEN 1024
#define NEW_CELL 0x01
#define RELATIVE 0x02
#define POLAR 0x04
#define LAMBDA_WARNING 0x08
#define SYN_DENS 0x10
#define SPHERICAL 0x20
#define SPINES 0x40
#define HSOLVE 0x80
#define SYMMETRIC 0x100
#define MEMB_FLAG 0x400

#define DATA_MODE 1
#define FLAG_MODE 2
#define COMMENT_MODE 3
#define SCRIPT_MODE 4
#define ERR -1
#define INPUT 0
#define VOLTAGE 0
#define MAX_NCHANS 10
#define MAX_LINELEN 80
#define INPUT_LINELEN 300
#define ARRAY_LINELEN 301

/* These fields are set from the cell definition file */
static float RM,CM,RA,EREST_ACT,DENDR_DIAM,SPINE_SURF,SPINE_DENS;
static float MEMB_FACTOR;
static int TAILWEIGHT=1;
static char	*chomp_leading_spaces();
static char comptname[50];
float	calc_dia(),calc_len();
int		fill_arglist();
static char *cellname;

float calc_surf(flags,len,dia)
	int		flags;
	float len,dia;
{
	float surface;

	if (len == 0.0) {		/* SPHERICAL */
	    surface = dia * dia * PI;
	} else {			/* CYLINDRICAL */
	    surface = len * dia * PI;
	}
	return(surface);
}

float calc_vol(len,dia)
	float len,dia;
{
	float	volume; 

	if (len == 0.0)		/* SPHERICAL */
	    volume =  dia * dia * dia * PI / 6.0;
	else 			/* CYLINDRICAL */
	    volume = len * dia * dia * PI / 4.0;
	return(volume);
}

Element *add_compartment(flags,name,link,len,dia,surface,volume)
	int		flags;
	char	*name;
	char	*link;
	float	len;
	float	dia;
	float	*surface;
	float	*volume;
{
	int 	i,argc;
	char	*argv[10];
	struct symcompartment_type  *compt,*lcompt;
	char	src[MAX_LINELEN],dest[MAX_LINELEN];
	MsgIn	*msgin;
	char	*cross;
	Element	*elm,*lelm;
	float	tsurface,tvolume,tlen,tdia,val,val2;

	compt = (struct symcompartment_type *)(GetElement(name));
	if (compt) {
	    fprintf(stderr,"double definition of (sym)compartment '%s'\n",name);
	    return(NULL);
	}
	/* copy the predefined prototype compartment with all its 
	** subelements */
	if (flags & NEW_CELL) {
	    argv[0] = "c_do_copy";
	    argv[1] = comptname;
	    argv[2] = name;
	    do_copy(3,argv);
	}

	compt = (struct symcompartment_type *)(GetElement(name));
	if (!compt) {
	    fprintf(stderr,"could not find (sym)compartment '%s'\n",name);
	    return(NULL);
	}

	*surface = calc_surf(flags,len,dia);
	*volume = calc_vol(len,dia);

	/* Rescaling all kids of copied element to its dimensions
	**  copied element is assumed to be cylindrical */
	if (elm=compt->child) {
	    tlen = calc_len(compt,RM,RA);
		tdia = calc_dia(compt,RM,RA);
	    tsurface = calc_surf(flags,tlen,tdia);
	    tvolume = calc_vol(tlen,tdia);
	    for(;elm;elm=elm->next){
			unscale_kids(elm,&val,&val2,tdia,tlen,tsurface,tvolume,flags);
			scale_kids(elm,val,val2,dia,len,surface,volume,flags);
	    }
	}

	/* make messages to connect with parent compartment */
	if (flags & NEW_CELL) {
	    if (strcmp(link,"none") != 0) {
		/* for the messages we need to distinguish between 
		**	asymmetric or symmetric compartments */
		if (strcmp(compt->object->name,"compartment") == 0) {
		    argv[0] = "c_do_add_msg";
		    argv[1] = link;
		    argv[2] = name;
		    argv[3] = "AXIAL";
		    argv[4] = "Vm";
		    do_add_msg(5,argv);
	    
		    argv[1] = name;
		    argv[2] = link;
		    argv[3] = "RAXIAL";
		    argv[4] = "Ra";
		    argv[5] = "Vm";
		    for (i=1; i<=TAILWEIGHT; i+=1) {
			do_add_msg(6,argv);
		    }
		} else if (strcmp(compt->object->name,"symcompartment")==0) {
		/* Check shape of parent compartment, if not found will default 
		**  to cylinder */
		    /* get pointer to parent compartment */
		    lcompt=(struct symcompartment_type *)(GetElement(link));
		    if (!lcompt) {
				fprintf(stderr,"could not find symcompartment '%s'\n",link);
				return(NULL);
		    }
		    for (i = 0 ; i < lcompt->object->envc-1; i+=2) {
				if (strncmp(lcompt->object->env[i],"Shape",5) == 0)
					break;
		    }
			if ((i < lcompt->object->envc) &&
				(strncmp(lcompt->object->env[i+1],"sphere",6) == 0)) {
		    	/* parent is SPHERICAL */
				/* setup axial current incoming to name_compt */
				argv[0] = "c_do_add_msg";
				argv[1] = link;
				argv[2] = name;
				argv[3] = "CONNECTSPHERE";
				argv[4] = "Ra";
				argv[5] = "Vm";
				do_add_msg(6,argv);
				/* setup axial current outgoing to link_compt */
				argv[1] = name;
				argv[2] = link;
				for (i=1; i<=TAILWEIGHT; i+=1) {
					do_add_msg(6,argv);
				}
		    } else {		/*parent is CYLINDRICAL or default*/
				/* setup axial current into head of name_compt */ 
				argv[0] = "c_do_add_msg";
				argv[1] = link;
				argv[2] = name;
				argv[3] = "CONNECTHEAD";
				argv[4] = "Ra";
				argv[5] = "Vm";
				do_add_msg(6,argv);

				/* if another compartment is already linked to 
				** tail of link_compt, cross link with it */
				for (msgin=lcompt->msg_in;msgin;msgin=msgin->next) {
					if (msgin->type == 1) {  /* CONNECTTAIL */
					/* another tail compartment exists! */
					cross = msgin->src->name;
					/* crossing axial R to head of name_compt */
					argv[1] = cross;
					argv[2] = name;
					argv[3] = "CONNECTCROSS";
					do_add_msg(6,argv);
					/* crossing axial R to head cross_compt */
					argv[1] = name;
					argv[2] = cross;
					do_add_msg(6,argv);
			    }
			}
				
			/* setup axial current out to tail of link_compt */
			argv[1] = name;
			argv[2] = link;
			argv[3] = "CONNECTTAIL";
			for (i=1; i<=TAILWEIGHT; i+=1) {
			    do_add_msg(6,argv);
			}
		    }
		} else {  /* neither compartment or symcompartment */
		    fprintf(stderr,"'%s' is not a (sym)compartment!\n",comptname);
		    return(NULL);
		}
		/* This handles the case where we want to send messages
		**  to the compartment proximal to current compartment,
		**  using the -env variables. The less exotic cases are
		**  handled later on.
		** These messages may be present in any subelement
		**  of the prototype and should be of the form:
		**	sendmsg#	"-/...	./...	MSGTYPE	MSGVARS"
		**  or
		**	sendmsg#	"./...	-/...	MSGTYPE	MSGVARS"
		**  where the - symbol refers to the parent element and
		**  the (first) . symbol to the new compartment.  The
		**  rest of the subpath has to be specified completely*/
		if (elm=compt->child) {
		    for(;elm;elm=elm->next){
				for(i = 0 ; i < elm->object->envc-1; i+=2) {
					if(strncmp(elm->object->env[i],"sendmsg",7)==0){
						/* since the zeroth argv is already filled*/
						argc=fill_arglist(argv+1,elm->object->env[i+1])+1;
						if (strncmp(argv[1],"-/",2)==0) {
							/* source is parent sub-element */
							argv[1] +=1;  /* skip the '-' */
							sprintf(src,"%s%s",link,argv[1]);
							argv[2] +=1;  /* skip the '.' */
							sprintf(dest,"%s%s",name,argv[2]);
							argv[1] = src;
							argv[2] = dest;
							lelm=(GetElement(src));
							if (lelm) {
								/* only if parent element exists */
								do_add_msg(argc,argv);
							}
						} else if (strncmp(argv[2],"-/",2)==0) {
							/* destination is parent sub-element */
							argv[1] +=1;  /* skip the '.' */
							sprintf(src,"%s%s",name,argv[1]);
							argv[2] +=1;  /* skip the '-' */
							sprintf(dest,"%s%s",link,argv[2]);
							argv[1] = src;
							argv[2] = dest;
							lelm=(GetElement(dest));
							if (lelm) {
								/* only if parent element exists */
								do_add_msg(argc,argv);
							}
						}
					}
				}
		    }
		}
	    }
	}

	/* compute membrane surface and RA and set Shape field */
	if (flags & SPHERICAL) {
	    compt->Ra = 8.0 * RA / (dia * PI);
		/* Assumes default is cylinder, resets to sphere if appropriate */
		argv[0] = "c_do_set";
		argv[1] = "-env";
		argv[2] = name;
		argv[3] = "Shape";
		argv[4] = "sphere";
		do_set(5,argv);
	}
	else {		/* CYLINDRICAL */
	    compt->Ra = 4.0 * RA * len /(dia * dia * PI);
	}
    tsurface = *surface;
	if (len > 0.0) { /* CYLINDRICAL */
	    if ((flags & SPINES) && (dia <= DENDR_DIAM)) 
			tsurface = tsurface + len * SPINE_DENS * SPINE_SURF;
	}
	if (flags & MEMB_FLAG)
		tsurface *= MEMB_FACTOR;

	compt->Cm = CM * tsurface;
	compt->Rm = RM / tsurface;
	compt->Em = EREST_ACT;
	compt->dia = dia;
	return((Element *)compt);
}


Element *add_channel(name,parent)
	char	*name;
	char	*parent;
{
	int argc;
	char	*argv[10];
	Element	*elm,*kid;
	MsgIn	*msg;
	struct ch	*compt;
	static char	dest[NAMELEN];
	char	*oname;

	if (!(elm = GetElement(name)))
		return(NULL);
	
	sprintf (dest,"%s/%s[%d]",parent,elm->name,elm->index);
	argv[0] = "c_do_copy";
	argv[1] = name;
	argv[2] = dest;
	do_copy(3,argv);

	if (!(elm = GetElement(dest)))
		return(NULL);

	abs_position(elm,elm->parent->x,elm->parent->y,elm->parent->z);

	oname = elm->object->name;

	argv[0] = "c_do_add_msg";
	if (strcmp(oname,"hh_channel") == 0 ||
		strcmp(oname,"tabchannel") == 0 ||
		strcmp(oname,"channelC2") == 0 ||
		strcmp(oname,"channelA") == 0 ||
		strcmp(oname,"channelB") == 0 ||
		strcmp(oname,"channelC") == 0 ||
		strcmp(oname,"ddsyn") == 0 ||
		strcmp(oname,"receptor2") == 0) {
		argv[1] = dest;
		argv[2] = parent;
		argv[3] = "CHANNEL";
		argv[4] = "Gk";
		argv[5] = "Ek";
		do_add_msg(6,argv);

		argv[1] = parent;
		argv[2] = dest;
		argv[3] = "VOLTAGE";
		argv[4] = "Vm";
		do_add_msg(5,argv);
	} else if (strcmp(oname,"vdep_channel") == 0) {
		argv[1] = dest;
		argv[2] = parent;
		argv[3] = "CHANNEL";
		argv[4] = "Gk";
		argv[5] = "Ek";
		do_add_msg(6,argv);

		argv[1] = parent;
		argv[2] = dest;
		argv[3] = "VOLTAGE";
		argv[4] = "Vm";
		do_add_msg(5,argv);
	} else if (strcmp(oname,"graded") == 0 ||
		strcmp(oname,"spike") == 0) {
		argv[1] = parent;
		argv[2] = dest;
		argv[3] = "INPUT";
		argv[4] = "Vm";
		do_add_msg(5,argv);
	}
	return(elm);
}


parse_compartment(flags,name,parent,x,y,z,d,nargs,ch,dens)
	int	flags;
	char	*name;
	char	*parent;
	float	x,y,z,d;
	int		nargs;
	char	ch[MAX_NCHANS][20];
	float	*dens;
{
	float nlambda;
	char	*ch_name;
	char	*ch_type;
	float	tx,ty,tz;
	int 	i,j,k;
	float	len,surf,vol,val2;
	Element	*elm, *compt, *parent_compt, *kid;
	Element *chanlist[MAX_NCHANS];
	char	msgname[MAX_LINELEN];
	char	src[MAX_LINELEN];
	char	dest[MAX_LINELEN];
	char	*argv[10];
	int		argc;

	if (!(flags & SPHERICAL)) {
		if (strcmp(parent,"none") == 0) {
			len = sqrt(x * x + y * y + z * z);
			x = y = z = 0.0;
		} else {
			parent_compt = GetElement(parent);
			if (!parent_compt) {
				fprintf(stderr,"could not find parent compt %s\n",parent);
				return;
			}
			if (flags & RELATIVE) {
				len = sqrt(x * x + y * y + z * z);
				x = x + parent_compt->x;
				y = y + parent_compt->y;
				z = z + parent_compt->z;
			} else {
				tx = x - parent_compt->x;
				ty = y - parent_compt->y;
				tz = z - parent_compt->z;
				len = sqrt(tx * tx + ty * ty + tz * tz);
			}
		}
	} else {
		len = 0;
	}
	if (!(compt = add_compartment(flags,name,parent,len,d,&surf,&vol)))
		return;
	abs_position(compt,x,y,z);
	/*
	We might also want to put in options for random rotating here
	*/

	if (!(flags & SPHERICAL)) {
		if (len == 0.0) {
			printf("ERROR : compartment '%s' has zero length!\n",name);
			return;
		}
	}
	if ((flags & LAMBDA_WARNING) && !(flags & SPHERICAL)) {
		nlambda = len / sqrt(RM * d * 0.25 / RA);
		if (nlambda < 0.01)
			printf("WARNING : compartment '%s' is only %f lambdas\n",
				name,nlambda);
		else if (nlambda > 0.20)
			printf("WARNING : compartment '%s' too long: %f lambdas\n",
				name,nlambda);
	}

	for (j = 7,k=0 ; j < nargs ; j += 2,k++) {
		sprintf(src,"/library/%s",ch[k]);
		chanlist[k] = NULL;
		if (!(elm = add_channel(src,name))) {
			set_compt_field(compt,ch[k],dens[k],len,d,flags);
			continue;
		}
		chanlist[k] = elm;
		scale_kids(elm,dens[k],val2,d,len,&surf,&vol,flags);
	}
	argv[0] = "c_do_add_msg";
	for (j = 7,k=0 ; j < nargs ; j += 2,k++) {
		if (!(elm = chanlist[k]))
			continue;
		ch_name = ch[k];
		for(i = 0 ; i < elm->object->envc-1; i+=2) {
			if (strncmp(elm->object->env[i],"sendmsg",7) == 0) {
				/* since the zeroth argv is already filled */
				argc = fill_arglist(argv + 1,elm->object->env[i+1])+1;
				sprintf(src,"%s/%s/%s",name,ch_name,argv[1]);
				sprintf(dest,"%s/%s/%s",name,ch_name,argv[2]);
				argv[1] = src;
				argv[2] = dest;
				do_add_msg(argc,argv);
			}
		}
	}
}

append_to_cell(name,flags)
	char	*name;
	int		*flags;
{
	char	*argvar[5];
	Element	*elm;

	if ((elm = GetElement(name)) == NULL) {
		*flags |= NEW_CELL;
		argvar[0] = "c_do_create";
		argvar[1] = "neutral";
		argvar[2] = name;
		do_create(3,argvar);
	}
	ChangeWorkingElement(name);
}

start_cell(name,flags)
	char	*name;
	int		*flags;
{
	char	*argvar[5];
	Element	*elm;

	if ((elm = GetElement(name)) != NULL) {
		if (elm->child || strcmp(elm->object->name,"neutral")!=0) {
			printf("Warning : cell '%s' already exists. Deleting previous version\n",name);
			DeleteElement(elm);
		} else {
			ChangeWorkingElement(name);
			return;
		}
	}

	*flags |= NEW_CELL;
	argvar[0] = "c_do_create";
	argvar[1] = "neutral";
	argvar[2] = name;
	do_create(3,argvar);
	ChangeWorkingElement(name);
}


do_read_cell(argc,argv)
	int argc;
	char	**argv;
{
	int		i,j;
	int		ncompts=0,nchans=0,nothers=0;
	int		len;
	char	rawline[ARRAY_LINELEN];
	char	*line;
	FILE	*fp,*fopen();
	Element	*elm;
	int		flags = SYN_DENS;
	int		parse_mode = DATA_MODE;
	float	get_script_float();
	char	*endit;
	char	*argvar[5];

	if (argc < 3) {
		fprintf(stderr,"usage : %s filename cellname\n",argv[0]);
		return;
	}
	if (!(fp = fopen(argv[1],"r"))) {
		fprintf(stderr,"can't open file '%s'\n",argv[1]);
		return;
	}


	/* getting values from script */
	CM = get_script_float("CM");
	RM = get_script_float("RM");
	RA = get_script_float("RA");
	EREST_ACT = get_script_float("EREST_ACT");

	/* setting the default compartment name */
	strcpy(comptname,"/library/compartment");

	/* setting the default memb_factor */
	MEMB_FACTOR = 1.0;

	fprintf(stderr,"reading '%s'.... \n",argv[1]);

	cellname = argv[2];
	start_cell(argv[2],&flags);


	for (i = 1, endit = fgets(rawline,INPUT_LINELEN,fp); endit ; endit = fgets(rawline,INPUT_LINELEN,fp), i++ ) {
		line = chomp_leading_spaces(rawline);
		len = strlen(line);
		while (line[len-2] == '\\' && endit) {
			line[len-2] = ' ';
			endit = fgets(rawline,INPUT_LINELEN,fp);
			strcpy(line+len-1,rawline);
			i++;
		}

		switch (*line) {
			case '\n' :
			case '\r' :
			case '\0' : continue;
			case '/' :
				if (line[1] == '/')
					continue;
				if (line[1] == '*')
					parse_mode = COMMENT_MODE;
				else
					parse_mode = DATA_MODE;
				break;
			case '*' :
				if (line[1] == '/') {
					parse_mode = DATA_MODE;
					continue;
				}
				parse_mode = SCRIPT_MODE;
				break;
			default :
				break;
		}

		switch(parse_mode) {
			case DATA_MODE :
				if (read_data(line,i,flags) == ERR) {
				}
				break;
			/*
			case FLAG_MODE :
				if (read_flag(line,i,flags) == ERR) {
				}
				break;
			*/
			case SCRIPT_MODE :
				if (read_script(line,i,&flags) == ERR) {
				}
				parse_mode = DATA_MODE;
				break;
			case COMMENT_MODE :
				len = strlen(line);
				if (line[len-2] == '*' && line[len-1] == '/')
					parse_mode = DATA_MODE;
				break;
			default :
				break;
		}
	}
	elm = (Element *)WorkingElement();
	if (!elm) {
		printf ("No current element for traverse cell\n");
	} else {
		traverse_cell(elm, &ncompts,&nchans,&nothers);
		if (flags & HSOLVE) {
			/* creating the hsolve element */
			argvar[0] = "c_do_create";
			argvar[1] = "hsolve";
			argvar[2] = "solve";
			do_create(3,argvar);
			/* Setting its path to the current cell */
			argvar[0] = "c_do_set";
			argvar[1] = "solve";
			argvar[2] = "path";
			if (flags & SYMMETRIC)
				argvar[3] = "./##[][TYPE=symcompartment]";
			else
				argvar[3] = "./##[][TYPE=compartment]";
			do_set(4,argvar);
			/* invoking its setup routine */
			CallElement(GetElement("solve"),GetAction("SETUP"));
		}
	}
	fprintf(stderr,"%s read: %d compartments, %d channels, %d others\n",
		argv[1],ncompts,nchans,nothers);
	fclose(fp);
}


read_data(line,lineno,flags)
	char	*line;
	int		lineno;
	int		flags;
{
	float	x,y,z,l,d;
	float	r,theta,phi;
	static	char	ch[MAX_NCHANS][20];
	float	dens[MAX_NCHANS];
/* mds3 changes */
/*
**	char	name[NAMELEN],lastname[NAMELEN],parent[NAMELEN];
*/
	static  char	lastname[NAMELEN];
	char	name[NAMELEN],parent[NAMELEN];
/* end of mds3 changes */
	int		nargs;

	nargs = sscanf(line,
		"%s%s%f%f%f%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f",
		name,parent,&x,&y,&z,&d,
		ch[0],&dens[0],ch[1],&dens[1],ch[2],&dens[2],
		ch[3],&dens[3],ch[4],&dens[4],ch[5],&dens[5],
		ch[6],&dens[6],ch[7],&dens[7],ch[8],&dens[8],ch[9],&dens[9]);
	
	if (nargs < 6) {
	/* Not enough inputs matched. */
		fprintf(stderr,"error on line %d :\n", lineno);
		fprintf(stderr,"	%s\nFields incorrect\n",line);
		return(ERR);
	}
	/* A shortcut for branches : use '.' instead of full name if
	** the parent is the previous element */
	if (strcmp(parent,".") == 0)
		strcpy(parent,lastname);
	strcpy(lastname,name);
/*
** Cartesian coord mode
**	name parent		x		y		z		dia 	ch dens...
**
** polar coord mode
**	name parent		r		theta	phi		dia		ch dens...
**
*/
	if (flags & POLAR) {
		r = x * 1.0e-6;
		theta = y * PI/180.0;
		phi = z * PI/180.0;
		x = r * sin(phi) * cos(theta);
		y = r * sin(phi) * sin(theta);
		z = r * cos(phi);
	} else {
		x *= 1.0e-6;
		y *= 1.0e-6;
		z *= 1.0e-6;
	}
	d *= 1.0e-6;
	parse_compartment(flags,name,parent,x,y,z,d,nargs,ch,dens);
	return(1);
}

read_script(line,lineno,flags)
	char	*line;
	int		lineno;
	int		*flags;
{
	char	command[NAMELEN];
	char	field[NAMELEN];
	float	value,len;
	int		i,nargs;
    struct symcompartment_type  *compt;

    nargs = sscanf(line,"%s %s %f",command,field,&value);
	if (strcmp(command,"*add_spines") == 0) {
		/* extra membrane surface will be added to all dendritic
		** compartments with d<=DENDR_DIAM, "collapses" spines
		** DENDR_DIAM in um, SPINE_DENS in 1/um, SPINE_SURF in um^2 */
		nargs = sscanf(line,"%s %f %f %f",command,&DENDR_DIAM,
						&SPINE_DENS,&SPINE_SURF);
		if (nargs == 4) {
			*flags |= SPINES;
			DENDR_DIAM *= 1.0e-6;
			SPINE_DENS *= 1.0e6;
			SPINE_SURF *= 1.0e-12;
		}
	} else if (nargs == 3 && strcmp(command,"*set_global") == 0) {
		/* setting a global */
		if (strcmp(field,"CONNECTTAIL") == 0) {
		    /* implements Nodus weight function */
		    TAILWEIGHT = value;
		} else {
		    set_script_float(field,value);
		    if (strcmp(field,"RM") == 0)
			RM = value;
		    if (strcmp(field,"RA") == 0)
			RA = value;
		    if (strcmp(field,"CM") == 0)
			CM = value;
		    if (strcmp(field,"EREST_ACT") == 0)
			EREST_ACT = value;
		}
	} else if (nargs == 2 && strcmp(command,"*memb_factor") == 0) {
		*flags |= MEMB_FLAG;
		sscanf(line,"%s %f",command,&MEMB_FACTOR);
		if (MEMB_FACTOR > 10.0 || MEMB_FACTOR < 1.0e-10)
			printf("Warning : memb_factor = '%f'; probably out of range\n",MEMB_FACTOR);
	} else if (strcmp(command,"*start_cell") == 0) {
		/* making a new cell or a prototype set of compts */
		if (nargs == 2)
			start_cell(field,flags);
		else
			start_cell(cellname,flags);
	} else if (strcmp(command,"*append_to_cell") == 0) {
		/* appending onto an existing cell */
		if (nargs == 2)
			append_to_cell(field,flags);
		else
			append_to_cell(cellname,flags);
	} else if (strcmp(command,"*makeproto") == 0) {
		if (nargs != 2) {
			printf("One compartment must be defined to be prototype\n");
		} else {
			makeproto_func(field);
		}
	} else if (strcmp(command,"*hsolve") == 0) {
			*flags |= HSOLVE;
	} else if (nargs == 1) /* just setting flags */ {
		if (strcmp(command,"*relative") == 0)
			*flags |= RELATIVE;
		else if (strcmp(command,"*absolute") == 0)
			*flags &= ~RELATIVE;
		else if (strcmp(command,"*polar") == 0)
			*flags |= POLAR;
		else if (strcmp(command,"*cartesian") == 0)
			*flags &= ~POLAR;
		else if (strcmp(command,"*lambda_warn") == 0)
			*flags |= LAMBDA_WARNING;
		else if (strcmp(command,"*lambda_unwarn") == 0)
			*flags &= ~LAMBDA_WARNING;
		else if (strcmp(command,"*syn_dens") == 0)
			*flags |= SYN_DENS;
		else if (strcmp(command,"*syn_cond") == 0)
			*flags &= ~SYN_DENS;
		else if (strncmp(command,"*spherical",6) == 0)
			*flags |= SPHERICAL;
		else if (strncmp(command,"*cylindrical",7) == 0)
			*flags &= ~SPHERICAL;
		else if (strcmp(command,"*symmetric") == 0) {
			if (strcmp(comptname,"/library/compartment") == 0)
				strcpy(comptname,"/library/symcompartment");
			*flags |= SYMMETRIC;
		} else if (strcmp(command,"*asymmetric") == 0) {
			if (strcmp(comptname,"/library/symcompartment") == 0)
				strcpy(comptname,"/library/compartment");
			*flags &= ~SYMMETRIC;
		}
	} else if (nargs == 2) { /* setting various other values */
		if (strcmp(command,"*compt") == 0) {
			/* setting the prototype compt */
			strcpy(comptname,field);
			/* set SPHERICAL flag correctly */
			compt = (struct symcompartment_type *)(GetElement(comptname));
			if (!compt) {
				fprintf(stderr,"could not find prototype compartment '%s'\n",comptname);
			} else {
				for (i = 0 ; i < compt->object->envc-1; i+=2) {
					if (strncmp(compt->object->env[i],"Shape",5) == 0)
						break;
				}
				if ((i < compt->object->envc) &&
					(strncmp(compt->object->env[i+1],"sphere",6) == 0)) {
					*flags |= SPHERICAL;
				} else {
                /* For compatibility with scripts that use the -env Len
				**  field (older neurokit version) */
					len = 1.0;    /* default is cylinder */
					for (i = 0 ; i < compt->object->envc-1; i+=2) {
						if (strncmp(compt->object->env[i],"Len",3) == 0) {
							len=Atof(compt->object->env[i+1]);
						}
					}
					if (len == 0.0) {  /* is SPHERICAL */
						*flags |= SPHERICAL;
					} else {
						*flags &= ~SPHERICAL;
					}
				}
            }
        }
	}
}

static char *chomp_leading_spaces(line)
	char	*line;
{
	char	*str;

	for(str = line; (*str == ' ' || *str == '	') ; str++);

	return(str);
}

/* WARNING: this output routine does not handle the spherical/
**	cylindrical and asymmetric/symmetric options correctly! It
**  cannot handle changing global values either (EDS) */
do_write_cell(argc,argv)
	int argc;
	char	**argv;
{
	FILE	*fp,*fopen();
	Element	*neuron;
	Element	*elm,*lastelm;
	Element	*chan;
	Element	*parent,*find_parent_dend();
	ElementList	el;
	Element *elms[MAX_EL_SIZE];
	float	calc_dia();
	float	calc_len();
	float	cond,calc_cond();
	float	get_script_float();
	int		i;
	float	x,y,z,r,theta,phi;
	int		relative_flag = 1, cartesian_flag = 1, syn_dens_flag = 1;
	char	*author;
	long	clock;

	el.nelements = 0;
	el.element = elms;

	if (argc < 3) {
		fprintf(stderr,"usage : %s filename cellname [cartesian/polar] [relative/absolute] \n",argv[0]);
		fprintf(stderr,"[syn_dens/syn_cond] [-author author]\n");
		fprintf(stderr,"Default is cartesian relative syn_dens\n");
		return;
	}
	author = NULL;
	for (i = 3 ; i < argc ; i++) {
		if (strcmp(argv[i],"polar") == 0)
			cartesian_flag = 0;
		if (strcmp(argv[i],"absolute") == 0)
			relative_flag = 0;
		if (strcmp(argv[i],"syn_cond") == 0)
			syn_dens_flag = 0;
		if (strcmp(argv[i],"-author") == 0) {
			i++;
			author = argv[i];
		}
	}

	if (!(fp = fopen(argv[1],"w"))) {
		fprintf(stderr,"can't open file '%s' for writing\n",argv[1]);
		return;
	}

	if ((neuron = GetElement(argv[2])) == NULL) {
		fprintf(stderr,"Can't find cell '%s' for writing\n",argv[2]);
		return;
	}

	/*
	** Finding all dendrites, but not their channels
	*/
	find_all_dends(neuron,0,&el);

	elm = elms[0];
	if (!elm || el.nelements == 0) {
		fprintf(stderr,"Incorrect neuron specification\n");
		return;
	}

	fprintf(stderr,"writing '%s'.... \n",argv[1]);
	/*
	** Writing out the header info
	*/
	fprintf(fp,"//	PARAMETER FILE FOR NEURON '%s'\n",neuron->name);
	if (author)
		fprintf(fp,"//	Author : %s \n",author);
	clock = time(0);
	fprintf(fp,"//	%s\n",ctime(&clock));
	fprintf(fp,"\n//	Format of file :\n");
	fprintf(fp,"// x,y,z,dia are in microns, all other units are SI (Meter Kilogram Second Amp)\n");
	fprintf(fp,"// In polar mode 'r' is in microns, theta and phi in degrees \n");
	fprintf(fp,"// Control line options start with a '*'\n");
	fprintf(fp,"// The format for each compartment parameter line is :\n");

	fprintf(fp,"//name	parent	r	theta	phi	d	ch	dens ...\n");
	fprintf(fp,"//in polar mode, and in cartesian mode :\n");
	fprintf(fp,"//name	parent	x	y	z	d	ch	dens ...\n\n\n");
	/*
	** Specifying coordinate modes
	*/
	fprintf(fp,"//		Coordinate mode\n");
	if (cartesian_flag)
		fprintf(fp,"*cartesian\n");
	else
		fprintf(fp,"*polar\n");
	if (relative_flag)
		fprintf(fp,"*relative\n");
	else
		fprintf(fp,"*absolute\n");

	/*
	** Writing out the constants
	*/
	fprintf(fp,"\n//		Specifying constants\n");
	CM = get_script_float("CM");
	RM = get_script_float("RM");
	RA = get_script_float("RA");
	fprintf(fp,"*set_global	RM	%g\n",RM);
	fprintf(fp,"*set_global	RA	%g\n",RA);
	fprintf(fp,"*set_global	CM	%g\n\n",CM);

	/*
	** Writing out soma
	*/
	fprintf(fp,"%s	none	%g	0	0	%g",elm->name,
		1e6 * calc_len(elm,RM,RA),1e6 * calc_dia(elm,RM,RA));
	for (chan = elm->child ; chan ; chan = chan->next) {
		if ((cond = calc_cond(elm,chan,RM,RA,syn_dens_flag)) > 0) {
			fprintf(fp,"	%s	%g",chan->name,cond);
		}
	}
	fprintf(fp,"\n\n");

	/*
	** loop for writing out each element
	*/
	for (i = 1 ; i < el.nelements ; i++) {
	/* Starting up */
		elm = elms[i];
		parent = find_parent_dend(elm);

	/* Printing name of elm */
		if (elm->index == 0)
			fprintf(fp,"%s	",elm->name);
		else
			fprintf(fp,"%s[%d]	",elm->name,elm->index);

	/* Printing parent of elm */
		if (parent == lastelm)
			fprintf(fp,".	");
		else if (parent->index == 0)
			fprintf(fp,"%s	",parent->name);
		else
			fprintf(fp,"%s[%d]	",parent->name,parent->index);
		lastelm = elm;

	/* printing coords of elm */
		if (relative_flag) {
			x = elm->x - parent->x;
			y = elm->y - parent->y;
			z = elm->z - parent->z;
		} else {		/* absolute coordinates */
			x = elm->x;
			y = elm->y;
			z = elm->z;
		}
		x *= 1e6;
		y *= 1e6;
		z *= 1e6;

		if (cartesian_flag) {
			fprintf(fp,"%g	%g	%g	",x,y,z);
		} else { 		/* polar coords : r,theta,phi */
			r = sqrt(x * x + y * y);
			if (x < r) /* To compensate for rounding errors */
				theta = acos(x/r);
			else
				theta = 0;

			if (y < 0)
				theta = 2 * PI - theta;
			theta *= 180.0/PI;
			
			r = sqrt(x * x + y * y + z * z);

			if (z < r) /* To compensate for rounding errors */
				phi = (acos(z/r)) * 180.0 / PI;
			else
				phi = 0;

			fprintf(fp,"%g	%g	%g	",r,theta,phi);
		}

	/* printing diameter of elm */
		fprintf(fp,"%g",1e6 * calc_dia(elm,RM,RA));

	/* Loop for printing the channels and their densities */
		for (chan = elm->child ; chan ; chan = chan->next) {
			if ((cond = calc_cond(elm,chan,RM,RA,syn_dens_flag)) > 0) {
				fprintf(fp,"	%s	%g",chan->name,cond);
			}
		}

	/* Finish off the line. Extra line every five, for readability */
		fprintf(fp,"\n");
		if ((i%5) == 0)
		fprintf(fp,"\n");
	}
	fclose(fp);
	fprintf(stderr,"Writing done\n");
}

unscale_kids(elm,dens,dens2,dia,length,surface,volume,flags)
	Element	*elm;
	float	*dens,*dens2;
	float	dia,length,surface,volume;
	int		flags;
{
	float	d;

	if (strcmp(elm->object->name,"hh_channel") == 0) {
		*dens = ((struct hh_channel_type *)elm)->Gbar;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(elm->object->name,"tabchannel") == 0) {
		*dens = ((struct tab_channel_type *)elm)->Gbar;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(elm->object->name,"vdep_channel") == 0) {
		*dens = ((struct vdep_channel_type *)elm)->gbar;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(elm->object->name,"channelC2") == 0) {
		*dens = ((struct channelC2_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(elm->object->name,"receptor2") == 0) {
		*dens = ((struct olf_receptor2_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(elm->object->name,"channelC") == 0) {
		*dens = ((struct channelC_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(elm->object->name,"manuelconduct") == 0) {
		*dens = ((struct manuelconduct_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(elm->object->name,"ddsyn") == 0) {
		*dens = ((struct dd_syn_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	/* Shortcut for specifying spike thresholds */
	} else if (strcmp(elm->object->name,"spike") == 0) {
		*dens = ((struct spike_type *)elm)->thresh;
	} else if (strcmp(elm->object->name,"Ca_concen") == 0) {
		*dens = ((struct Ca_concen_type *)elm)->B;
		if (*dens > 0.0)
			*dens *= volume;
	/* Pools may have an effective volume/length smaller than comp */
	} else if (strcmp(elm->object->name,"difpool") == 0) {
		*dens = ((struct difpool_type *)elm)->Vol;
		*dens2 = ((struct difpool_type *)elm)->Len;
		if (*dens > 0.0)
			*dens /= volume;
		if (*dens2 > 0.0)
			*dens /= length;
	}
}

scale_kids(elm,dens,dens2,dia,length,surface,volume,flags)
	Element	*elm;
	float	dens,dens2;
	float	dia,length,*surface,*volume;
	int		flags;
{
	float	d;

	if (strcmp(elm->object->name,"hh_channel") == 0) {
		if (dens < 0.0)
			((struct hh_channel_type *)elm)->Gbar = -1.0 * dens;
		else
			((struct hh_channel_type *)elm)->Gbar = dens * *surface;
	} else if (strcmp(elm->object->name,"tabchannel") == 0) {
		if (dens < 0.0)
			((struct tab_channel_type *)elm)->Gbar = -1.0 * dens;
		else
			((struct tab_channel_type *)elm)->Gbar = dens * *surface;
	} else if (strcmp(elm->object->name,"vdep_channel") == 0) {
		if (dens < 0.0)
			((struct vdep_channel_type *)elm)->gbar = -1.0 * dens;
		else
			((struct vdep_channel_type *)elm)->gbar=dens * *surface;
	} else if (strcmp(elm->object->name,"channelC2") == 0) {
		if (dens < 0.0)
			((struct channelC2_type *)elm)->gmax = -1.0 * dens;
		else
			((struct channelC2_type *)elm)->gmax = dens * *surface;
	} else if (strcmp(elm->object->name,"manuelconduct") == 0) {
		if (dens < 0.0)
			((struct manuelconduct_type *)elm)->gmax = -1.0 * dens;
		else
			((struct manuelconduct_type *)elm)->gmax = dens * *surface;
	} else if (strcmp(elm->object->name,"receptor2") == 0) {
		if (dens < 0.0)
			((struct olf_receptor2_type *)elm)->gmax = -1.0 * dens;
		else
			((struct olf_receptor2_type *)elm)->gmax=dens * *surface;
	} else if (strcmp(elm->object->name,"channelC") == 0) {
		if (dens < 0.0)
			((struct channelC_type *)elm)->gmax = -1.0 * dens;
		else
			((struct channelC_type *)elm)->gmax = dens * *surface; 
	} else if (strcmp(elm->object->name,"ddsyn") == 0) {
		if (dens < 0.0)
			((struct dd_syn_type *)elm)->gmax = -1.0 * dens;
		else
			((struct dd_syn_type *)elm)->gmax = dens * *surface;
	/* Shortcut for specifying spike thresholds */
	} else if (strcmp(elm->object->name,"spike") == 0) {
		((struct spike_type *)elm)->thresh = dens;
	} else if (strcmp(elm->object->name,"Ca_concen") == 0) {
		if (dens < 0.0)
			((struct Ca_concen_type *)elm)->B = -1.0 * dens;
		else
			((struct Ca_concen_type *)elm)->B = dens / *volume;
	} else if (strcmp(elm->object->name,"difpool") == 0) {
		((struct difpool_type *)elm)->Vol = dens * *volume;
		((struct difpool_type *)elm)->Len = dens2 * length;
	}
}

int fill_arglist(argv,line)
/* parses a message of the form "src dst type var1 var2 ..."
** into separate arguments */
	char	**argv;
	char	*line;
{
	int nargs = 0;
	static char	str[INPUT_LINELEN];
	char	*s;
	int		new=1;

	strcpy(str,line);

	for (s=str;*s;s++) {
		if (*s == ' ' || *s == '\t') {
			*s = '\0';
			new = 1;
		} else if (new) {
			argv[nargs] = s;
			nargs++;
			new = 0;
		}
	}
	return(nargs);
}

do_paste_channel(argc,argv)
	int argc;
	char	**argv;
{
	Element *elm;
	Element *add_channel();
	int		i;
	char	*argtab[20];
	char	*name,*ch_name;
	char	src[MAX_LINELEN],dest[MAX_LINELEN];
	
	if (argc < 3) {
		fprintf(stderr,"usage : %s source_channel dest_compartment\n",
			argv[0]);
		return;
	}

	ch_name = argv[1];
	name = argv[2];

	if ((elm = GetElement(ch_name)) == NULL) {
		printf("Could not find channel '%s'\n",argv[2]);
		return;
	}

	if ((elm = GetElement(name)) == NULL) {
		printf("Could not find compartment '%s'\n",argv[2]);
		return;
	}

	if ((strcmp(elm->object->name,"compartment") != 0) &&
		(strcmp(elm->object->name,"symcompartment") != 0)) {
		printf("Element '%s' is not a (sym)compartment.\n",argv[2]);
		return;
	}

	elm = add_channel(ch_name,argv[2]);

	if (!elm) {
		printf("Could not add channel '%s'\n",argv[2]);
		return;
	}

	ch_name=Pathname(elm);

	/* Filling in the setenv messages */
	argtab[0] = "c_do_add_msg";
	for(i = 0 ; i < elm->object->envc-1; i+=2) {
		if (strncmp(elm->object->env[i],"sendmsg",7) == 0) {
			/* since the zeroth argtab is already filled */
			argc = fill_arglist(argtab + 1,elm->object->env[i+1])+1;
			sprintf(src,"%s/%s",ch_name,argtab[1]);
			sprintf(dest,"%s/%s",ch_name,argtab[2]);
			argtab[1] = src;
			argtab[2] = dest;
			do_add_msg(argc,argtab);
		}
	}
}

traverse_cell(elm,ncompts,nchans,nothers)
	Element	*elm;
	int	*ncompts,*nchans,*nothers;
{

	for (;elm ; elm = elm->next) {
		if (strcmp(elm->object->name,"compartment") == 0 ||
			strcmp(elm->object->name,"symcompartment") == 0)
			*ncompts += 1;
		else if (strcmp(elm->object->name,"hh_channel") == 0 ||
			strcmp(elm->object->name,"vdep_channel") == 0 || 
			strcmp(elm->object->name,"tabchannel") == 0 || 
			strcmp(elm->object->name,"channelC2") == 0 ||
			strcmp(elm->object->name,"channelA") == 0 ||
			strcmp(elm->object->name,"channelB") == 0 ||
			strcmp(elm->object->name,"channelC") == 0 ||
			strcmp(elm->object->name,"ddsyn") == 0 ||
			strcmp(elm->object->name,"receptor2") == 0) 
			*nchans += 1;
		else
			*nothers += 1;

		if (elm->child)
			traverse_cell(elm->child,ncompts,nchans,nothers);
	}
}


/*
** Converting a new cell to the prototype configuration. This is
** done by changing the 'flat' element heirarchy to one where the
** parent is the specified compartment.
** We need to do some tricky element juggling here. First,
** remove the proto from the childlist of elm. Then,
** add the childlist of elm to the childlist of proto. Then,
** replace elm by proto
*/
makeproto_func(field)
	char	*field;
{
	Element	*elm,*child,*proto;

	elm = (Element *)GetElement(field);
	proto = NULL;
	if (!elm) {
		printf("could not find cell %s\n",field);
		return;
	}

	/* Removing proto from elm's childlist */
	if (strcmp(elm->child->name,elm->name) == 0) {
		proto=elm->child;
		elm->child=proto->next;
	} else {
		for (child=elm->child;child;child=child->next){
			if (child->next) {
				if(strcmp(elm->name,child->next->name)==0) {
					proto=child->next;
					child->next = proto->next;
				}
			}
		}
	}
	if (!proto) {
		printf("could not find compartment %s\n",field);
		return;
	}

	/* Add childlist of elm to childlist of proto */
	if (!(proto->child)) {
		proto->child = elm->child;
	} else {
		for (child=proto->child;child;child=child->next) {
			if (!(child->next)) {
				child->next = elm->child;
				break;
			}
		}
	}

	/* Replace elm by proto */
	if (elm == elm->parent->child) {
		elm->parent->child = proto;
		proto->next = elm->next;
		proto->parent = elm->parent;
	} else {
		for(child=elm->parent->child;child;child=child->next) {
			if (elm == child->next) {
				child->next = proto;
				proto->next = elm->next;
				proto->parent = elm->parent;
				break;
			}
		}
	}
	elm->next = NULL;
	elm->parent = NULL;
	elm->child = NULL;
	sfree(elm);
}

set_compt_field(compt,field,value,len,dia,flags)
	struct symcompartment_type	*compt;
	char	*field;
	float	value,len,dia;
	int		flags;
{
	float calc_surf();

	if (strcmp(field,"RA") == 0){
		if (flags & SPHERICAL) {
	    	compt->Ra = 8.0 * RA / (dia * PI);
		}
		else {		/* CYLINDRICAL */
	    	compt->Ra = 4.0 * RA * len /(dia * dia * PI);
		}
	} else if (strcmp(field,"RM") == 0){
		compt->Rm = RM / calc_surf(flags,len,dia);
	} else if (strcmp(field,"CM") == 0){
		compt->Cm = CM * calc_surf(flags,len,dia);
	} else {
		SetElement(compt,field,ftoa(value));
	}
}

#undef INPUT
#undef VOLTAGE

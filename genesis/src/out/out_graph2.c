static char rcsid[] = "$Id: out_graph2.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: out_graph2.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1997/05/29 08:48:21  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS21a revison: EDS BBF-UIA 96/04/12
 * Corrected CallElement syntax
 *
** Revision 1.1  1992/12/11  19:03:24  dhb
** Initial revision
**
*/

#include "sim_ext.h"
#include "out_struct.h"

int do_reset_graph(argc,argv)
int argc;
char **argv;
{
    if(argc < 2){
	printf("usage: %s path\n",argv[0]);
	return(0);
    }
    CallElement(GetElement(argv[1]),GetAction("RESET"),0);
    return(1);
}

int do_graph_file(argc,argv)
int argc;
char **argv;
{
int 	nxtarg;
int 	filecnt;
int 	i;
char 	*graph;
float 	x,y;
int 	notdone;
char 	line[100];
FILE 	*fp[100];
char 	*fpname[100];
char	plotname[100];
int 	start_plot = 0;
int 	overlay = 0;
static	char	*colorname = "Blue";

    if(argc < 3){
	printf("usage: %s graphname [-color color][-overlay #] file [file ...]\n",
	   argv[0]);
	return(0);
    }
    graph = argv[1];
    nxtarg = 2;
    filecnt = 0;
    while(nxtarg < argc){
	if(strcmp(argv[nxtarg],"-overlay") ==0){
	    start_plot = atoi(argv[++nxtarg]);
	    overlay = 1;
	} else
  if(strcmp(argv[nxtarg],"-color") ==0) {
	  colorname = argv[++nxtarg];
	   } else
	if((fp[filecnt] = fopen(argv[nxtarg],"r")) == NULL){
	    printf("cant find file %s\n",argv[nxtarg]);
	} else {
	    fpname[filecnt] = argv[nxtarg];
	    filecnt++;
	}
	nxtarg++;
    }
    if(!overlay){
	/*
	** clear the graph
	*/
	CallElement(GetElement(graph),GetAction("RESET"),0);
    }
    while(notdone){
	/*
	** keep going while any file still has data
	*/
	notdone = 0;
	for(i=0;i<filecnt;i++){
	    /*
	    ** for each file read in a line of data
	    */
	    fgets(line,100,fp[i]);
	    if(!feof(fp[i])) {
		/*
		** extract coordinate pairs
		*/
		sscanf(line,"%f%f",&x,&y);
		notdone = 1;
		/*
		** and plot them
		*/
		sprintf(plotname,"%d %s",start_plot+i,fpname[i]);
		axis_graph_add_pts(graph,plotname,colorname,x,y);
	    }
	}
    }
    for(i=0;i<filecnt;i++){
	/*
	** close all files
	*/
	fclose(fp[i]);
    }
    return(1);
}

int do_graph_pts(argc,argv)
int argc;
char **argv;
{
int 	nxtarg;
char 	*graph;
float 	x,y;
char	*plotname;
char	*colorname;

    if(argc < 3){
	printf("usage: %s graphname [-plotname name] x y [x y ...]\n",argv[0]);
	return(0);
    }
    nxtarg = 1;
    graph = argv[nxtarg++];
    plotname = "none";
    colorname = NULL;
	while (*argv[nxtarg] == '-' && nxtarg < argc) {
		if(strcmp(argv[nxtarg],"-plotname") ==0) {
			plotname = argv[nxtarg+1];
			nxtarg += 2;
			continue;
		}
		if(strcmp(argv[nxtarg],"-color") ==0) {
			colorname = argv[nxtarg+1];
			nxtarg += 2;
			continue;
		}
		if((argv[nxtarg][1] >= '0' &&
		argv[nxtarg][1] <= '9') ||
		argv[nxtarg][1] == '.'){
		    break;
		}
		printf("%s; unknown option '%s'\n", argv[0], argv[nxtarg]);
		return(0);
	}
    while(nxtarg < argc -1){
	x = Atof(argv[nxtarg]);
	y = Atof(argv[++nxtarg]);
	nxtarg++;
	axis_graph_add_pts(graph,plotname,colorname,x,y);
    }
    return(1);
}

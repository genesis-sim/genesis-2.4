static char rcsid[] = "$Id: parinit.c,v 1.2 2005/09/29 23:19:38 ghood Exp $";
/* version.c contains the most recent rcs version number */
#include "version.c"

/*
 * $Log: parinit.c,v $
 * Revision 1.2  2005/09/29 23:19:38  ghood
 * updated for PGENESIS 2.3
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  1999/12/19 04:01:21  mhucka
 * Fix for compiling under egcs under Red Hat 6.x
 *
 * Revision 1.11  1997/07/22 21:30:24  ngoddard
 * support for user extensions
 *
 * Revision 1.10  1997/02/20 21:13:37  ngoddard
 * support for xpvm; using pvm_trecv for timeout ops; fixed bugs related to futures; provide user warnings when appears stalled; barrer and barrierall take a third argument: how long to wait before stall is assumed; beginnings of performance stats package
 *
 * Revision 1.9  1997/01/21 22:20:23  ngoddard
 * paragon port
 *
 * Revision 1.7  1996/06/21 16:10:44  ghood
 * General debugging
 *
 * Revision 1.6  1996/05/03 19:48:10  ghood
 * moving to biomed area
 *
 * Revision 1.5  1995/05/24 15:34:46  ngoddard
 * improved threads but still basic design flaw, also fixed comment leader in barrier tests
 *
 * Revision 1.4  1995/05/10  18:56:47  ngoddard
 * before making changes to allow ids on barriers
 *
 * Revision 1.3  1995/05/02  22:07:04  ngoddard
 * changed to use -execdir and -altsimrc
 *
 * Revision 1.2  1995/04/18  21:28:12  ngoddard
 * *** empty log message ***
 *
 * Revision 1.1  1995/04/03  22:04:16  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 * Revision 1.5  1993/12/30  00:06:12  ngoddard
 * Release 12/14/93
 *
 * Revision 1.2  1993/10/15  22:51:51  ngoddard
 * added Id and Log keywords
 *
 *
*/

#include <stdio.h>
#include "par_ext.h"
#if USE_MPI
#include <mpi.h>
#endif

static Postmaster * pm;
static FILE * Dbgfile;


void InitPar (pargc, pargv, penvp)
     int *pargc;
     char ***pargv;
     char ***penvp;
{
  char **newargs;
  int i;
  int rank;
  
#if USE_MPI
  if (MPI_Init(pargc, pargv) != MPI_SUCCESS)
    {
      fprintf(stderr, "MPI_Init() was unsuccessful\n");
      exit(1);
    }

  if (MPI_Comm_rank(MPI_COMM_WORLD, &rank) != MPI_SUCCESS)
    {
      fprintf(stderr, "MPI_Comm_rank failed\n");
      exit(1);
    }
#endif

  if (getenv("TERM") == NULL)
    {
#if USE_MPI
      printf("In process %d TERM is undefined.\n", rank);
#endif
      newargs = (char **) malloc(((*pargc) + 1) * sizeof(char*));
      for (i = 0; i < *pargc; ++i)
        newargs[i] = (*pargv)[i];
      newargs[i++] = "-notty";
      *pargv = newargs;
      *pargc = i;
    }
  else
    {
#if USE_MPI
      printf("In process %d TERM is set to %s\n", rank, getenv("TERM"));
#endif
    }
}

void FinalizePar ()
{
#if USE_MPI
  (void) MPI_Finalize();
#endif
}

void Dbgmsg(str)
     char * str;
{
  fprintf(Dbgfile, "%s", str);
  fflush(Dbgfile);
  fflush(stdout);
  fflush(stderr);
}

void ParEventLoop();

/* Event loop for handling parallel messages */
void ParEventLoop()
{
  ClearSetupMessages(pm, 1, 0.);
}

/*

   syntax is:
   paron [-n[odes] <nodecount>] [-startup <filename>] [-stdout <filename>] [-execdir <dirname>] [-nice <priority-level>] [-batch] [-p[arallel] | -f[arm] | -m[ixed] <mixed-spec>]
   
   master: if no startup file is given, substitutes the top level script
   file if there is one or else the default which simply does paron with no
   args, for what is sent to worker.

   worker: if no args then use the ones sent by the master, otherwise
   verify that master and workers agree on zoning.

*/

int do_paron(argc,argv)
     int argc;
     char	**argv;
{
#define NPMARGS 13
  char *pmarg[NPMARGS+1];
  char *zarg[512];
  int	i, j, action, status, nnodes, nzargs = 0, npmargs = 0;
  char version[20];
  char * startf = NULL;
  char * nullarg = "";
  char tidbuf[32];		/* to hold a PVM tid number in ascii */

  Dbgfile = stdout;

#if defined(BGL)
  SetBatchMode(1);
#endif
  action = -1;
  initopt(argc, argv, "-executable filename -nodes nodecount -execdir dirname -nice level -startup filename -altsimrc filename -silent level -debug flags -output filename -dbgout filename -batch -parallel -farm -mixed nnodes nzones ...");

  /* call to CREATE has these args, note these are 1 more than in pvm-master */
  /* [0] - "create" */
  /* [1] - "post" */
  /* [2] - "/post" */
  /* [3] - start-file-name (could be NULL) */
  /* [4] - num nodes, NULL implies get from environment or master */
  /* [5] - .simrc file to use, NULL implies .nxsimrc */
  /* [6] - silence level, NULL imples 3 */
  /* [7] - spawn value, 4 implies debug, 8 implies trace (see PVM) */
  /* [8] - worker executable name, NULL implies "genesis" */
  /* [9] - file for worker output on control node, NULL implies local output */
  /* [10] - working directory for workers, default is masters cwd */
  /* [11] - nice level for workers, default is the master's nice level */
  /* [12] - batch mode */

  pmarg[0]="create";
  pmarg[1]="post";
  pmarg[2]="/post";
  pmarg[3]=nullarg;
  pmarg[4]=nullarg;
  pmarg[5]=nullarg;
  pmarg[6]=nullarg;
  pmarg[7]=nullarg;
  pmarg[8]=nullarg;
  pmarg[9]=nullarg;
  pmarg[10]=nullarg;
  pmarg[11]=nullarg;
  pmarg[12]=nullarg;
  npmargs = NPMARGS+1;

  while ((status = G_getopt(argc, argv)) == 1)
    {
      if (strcmp(G_optopt, "-startup") == 0)
	{ pmarg[3] = optargv[1]; }
      else if (strcmp(G_optopt, "-nodes") == 0)
	{ pmarg[4] = optargv[1]; }
      else if (strcmp(G_optopt, "-altsimrc") == 0)
	{ pmarg[5] = optargv[1]; }
      else if (strcmp(G_optopt, "-silent") == 0)
	{ pmarg[6] = optargv[1]; }
      else if (strcmp(G_optopt, "-debug") == 0)
	{ pmarg[7] = optargv[1]; }
      else if (strcmp(G_optopt, "-executable") == 0)
	{ pmarg[8] = optargv[1]; }
      else if (strcmp(G_optopt, "-output") == 0)
	{ pmarg[9] = optargv[1]; }
      else if (strcmp(G_optopt, "-execdir") == 0)
	{ pmarg[10] = optargv[1]; }
      else if (strcmp(G_optopt, "-nice") == 0)
	{ pmarg[11] = optargv[1]; }
      else if (strcmp(G_optopt, "-batch") == 0)
	{
	  pmarg[12] = "-batch";
	  SetBatchMode(1);
	  SetTtyMode(0);
	}
      else if (strcmp(G_optopt, "-dbgout") == 0)
	{ char fname[256];
	  long getpid();
	  sprintf(fname, "%s.%ld", optargv[1], getpid());
	  Dbgfile = fopen(fname, "w");
	}
      else if (strcmp(G_optopt, "-parallel") == 0) {
	if (nzargs == 0)
	  { zarg[nzargs++] = optargv[0]; }
	else
	  printf("Warning: zoning already defined as `%s`, %s ignored\n",
		 zarg[0], optargv[0]);
      } else if (strcmp(G_optopt, "-farm") == 0)
	if (nzargs == 0)
	  { zarg[nzargs++] = optargv[0]; }
	else
	  printf("Warning: zoning already defined as `%s`, %s ignored\n",
		 zarg[0], optargv[0]);
      else if (strcmp(G_optopt, "-mixed") == 0)
	if (nzargs == 0) {
	  if (optargc%2 != 1)
	    { printoptusage(argc, argv); return 0; }
	  for (i = 0; i < optargc; i++)
	    zarg[nzargs++] = optargv[i];
	}
	else
	  printf("Warning: zoning already defined as `%s`, %s ignored\n",
		 zarg[0], optargv[0]);
    }

  if(status != 0){
    TraceScript();
    printoptusage(argc, argv);
    return 0;
  }

  /* do nothing if the postmaster already exists */
  if (GetElement("/post") != NULL) {
    printf("parallel library already enabled\n");
    return(1);
  }
  
  if (IsSilent() <2)
    {	/* print the version number */
      for (i = 0; rcsversion[i] != ','; i++);
      for (i = i+3, j = 0; rcsversion[i] != ' '; i++, j++)
	version[j] = rcsversion[i];
      version[j] = '\0';
      printf("Initializing parallel library version %s\n", version);
    }

#if USE_PVM
  /* enroll in PVM, abort if not found */
  if ((i = EnrollInPvm()) < 0)
    return 0;
#endif

  sprintf(tidbuf,"%d",i);
  pmarg[NPMARGS] = tidbuf;

  do_enable_parallel(0,0);
  /* Create the postmaster object*/
  DBGMSG("creating postmaster...\n");
  do_create(npmargs,pmarg);
  DBGMSG("created postmaster...\n");
  
  /* set up the machine partition */
  pm=(Postmaster *)GetElement("/post");
  if (pm == NULL)		/* postmaster initialization failed */
    {
      fprintf(stdout, "paron command failed\n");
      return 0;
    }

  CreateZones(pm,nzargs, zarg);
  
  /* Schedule a job which handles setup messages */
  /* Use a medium priority. Setup messages are rare, and expensive */
  AddJob(ParEventLoop,1);
  
  return(1);
}








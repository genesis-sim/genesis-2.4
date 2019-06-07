/* $Id: libacct.c,v 1.2 2005/09/29 23:11:12 ghood Exp $ */

/*
 * $Log: libacct.c,v $
**
** Revision 1.3  2019/01/16 09:00:0  jcrone
** Added gcmalloc header file
**
 * Revision 1.2  2005/09/29 23:11:12  ghood
 * removed T3D code
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:41  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.4  1997/07/07 05:39:29  ngoddard
 * function prototpyes for sunos
 *
 * Revision 1.3  1997/02/20 21:21:54  ngoddard
 * fixed rcs keywords
 *
 * Revision 1.2  1997/02/20 21:20:08  ngoddard
 * added rcs keywords
 * */

#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include "acct.h"
#include "gcmalloc_ext.h"

#define MAX_ACCTS	80

char *accts[MAX_ACCTS] = {0};
double acct_bins[MAX_ACCTS] = {0.0};

int n_accts = 0;

NewAcct(id, name)
     int id;
     char *name;
{
  accts[id] = (char *) malloc(strlen(name) + 1);
  strcpy(accts[id], name);
}

void
Acct (n)
     int n;
{
  static int lastAcct = -1;
  static struct timeval last_time;
  struct timeval this_time;
  double elapsedMicroseconds;

  gettimeofday(&this_time, NULL);
  if (lastAcct >= 0)
    {
      elapsedMicroseconds = ((double) (this_time.tv_sec - last_time.tv_sec)) +
	0.000001 * (this_time.tv_usec - last_time.tv_usec);
      acct_bins[lastAcct] += elapsedMicroseconds;
    }
  lastAcct = n;
  last_time = this_time;
}


void
PrintAcct (name, tid)
     char *name;
     int tid;
{
  FILE *f;
  char fn[256];
  char hn[128];
  int i;
  double total;
  
  Acct(-1);		/* this makes sure the times are updated to
			   reflect everything up to this routine */
  if (name == NULL)
    f = stdout;
  else
    {
      sprintf(fn, "/tmp/%s.%d.%d.acct", name, getpid(), tid);
      f = fopen(fn, "w");
    }
  gethostname(hn, 128);
  fprintf(f, "\nHOST: %s\n", hn);
  total = 0;
  for (i = 0; i < MAX_ACCTS; ++i)
    total += acct_bins[i];
  for (i = 0; i < MAX_ACCTS; ++i)
    if (accts[i] != NULL)
      fprintf(f, "%20s: %12.6f seconds; %6.2f%% of total\n",
	      accts[i],
	      acct_bins[i],
	      100.0 * acct_bins[i] / total);
  if (name != NULL)
    fclose(f);
}

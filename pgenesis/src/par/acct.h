/* $Id: acct.h,v 1.1.1.1 2005/06/14 04:38:39 svitak Exp $ */

/*
 * $Log: acct.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:41  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.4  1997/07/07 05:39:28  ngoddard
 * function prototpyes for sunos
 *
 * Revision 1.3  1997/02/20 21:21:54  ngoddard
 * fixed rcs keywords
 *
 * Revision 1.2  1997/02/20 21:20:08  ngoddard
 * added rcs keywords
 * */

#ifdef FUNC_PROTO
extern void Acct(int n);
extern void PrintAcct (char *name, int tid);
#else
extern void Acct();
extern void PrintAcct();
#endif

#define PGENESIS 1
#define PGENESIS_EVENT 2
#define PGENESIS_PROCESS 3
#define PGENESIS_PROCESS_SNDREC 4
#define PGENESIS_PROCESS_SNDREC_SND 5
#define PGENESIS_PROCESS_SNDREC_REC 6
#define PGENESIS_PROCESS_SNDREC_GETFIELD 7
#define PGENESIS_PROCESS_SYNC 8

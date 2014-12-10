#ifndef PVM_DEFS
#define PVM_DEFS

#if USE_MPI && USE_PVM
#error Only one of USE_MPI or USE_PVM should be set to 1 in the top-level Makefile.
#endif

#if !(USE_MPI) && !(USE_PVM)
#error Either USE_MPI or USE_PVM should be set to 1 in the top-level Makefile.
#endif

#if USE_PVM
/* we simply include the standard PVM header file */
#include "pvm3.h"
#endif

#if USE_MPI
/* we define PVM calls in terms of MPI calls */
#include <sys/time.h>

#define PvmDataDefault	0
#define PvmDataRaw	1
#define PvmDataInPlace	2

#define PvmDebugMask    2
#define PvmOutputTid    4

typedef struct pvmhostinfo {
  int hi_tid;
  char *hi_name;
  char *hi_arch;
  int hi_speed;
} pvmhostinfo; 

int pvm_initsend ();
int pvm_pkint ();
int pvm_pkuint ();
int pvm_pkshort ();
int pvm_pkushort ();
int pvm_pkfloat ();
int pvm_pkbyte ();
int pvm_pkstr ();
int pvm_pkdouble ();
int pvm_upkint ();
int pvm_upkuint ();
int pvm_upkshort ();
int pvm_upkushort ();
int pvm_upkfloat ();
int pvm_upkbyte ();
int pvm_upkstr ();
int pvm_upkdouble ();
int pvm_send ();
int pvm_recv ();
int pvm_exit ();
void exit ();
int pvm_mytid ();
int pvm_nrecv ();
int pvm_trecv ();
int pvm_bufinfo ();
int pvm_mcast ();
int pvm_setopt ();
int pvm_config ();
int pvm_get_PE ();
#endif

#endif

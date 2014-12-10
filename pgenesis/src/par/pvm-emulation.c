/* This set of routines was generated to emulate the action
   of pvm using the Message Passing Interface MPI equivalent calls */
#if USE_MPI

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#ifdef _POSIX_PRIORITY_SCHEDULING
#include <sched.h>
#endif
#include "par_ext.h"
#include "pvm-defs.h"

#include <mpi.h>

#define BUF_SIZE 65536
static char SendBuffer[BUF_SIZE];
static char RecvBuffer[BUF_SIZE];
static int Sendsize;
static int Recvsize;
static int SourceId;
static int MessageTag;
static int MessageLength;

static int TotalMessages;

Postmaster * pm;

int pvm_initsend(encoding)
    int encoding;
  {
    /* Initialize the send buffer pointer-index to zero */
    Sendsize=0;
    return 0;
  }

/* pack integers into send buffer */
int pvm_pkint(a,num,stride)
     int *a;
     int num;
     int stride;

  {
    int  nb;

#ifdef DEBUG
    printf("%f (%d): pack an integer %d at location %d\n",
	   MPI_Wtime(), pvm_mytid(),
	   *a,Sendsize);
   fflush(NULL);
#endif

    nb=sizeof(int)*num;
    memcpy(&SendBuffer[Sendsize],a,nb);
    Sendsize+=nb;
    if(Sendsize<BUF_SIZE) 
      return 0;
    return -1;
  }

/* pack unsigned integers into send buffer */ 
int pvm_pkuint(a,num,stride)
     unsigned *a;
     int num;
     int stride;
 { 
    int  nb;

    nb=sizeof(unsigned)*num;
    memcpy(&SendBuffer[Sendsize],a,nb);
    Sendsize+=nb;
    if(Sendsize<BUF_SIZE) 
      return 0;
    return -1;
  }

/* pack short integers into send buffer */
int pvm_pkshort(a,num,stride)
     short *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(short)*num;
    memcpy(&SendBuffer[Sendsize],a,nb);
    Sendsize+=nb;
    if(Sendsize<BUF_SIZE) 
      return 0;
    return -1;
  }

/* pack unsigned short integers into send buffer */
int pvm_pkushort(a,num,stride)
     unsigned short *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(short)*num;
    memcpy(&SendBuffer[Sendsize],a,nb);
    Sendsize+=nb;
    if(Sendsize<BUF_SIZE) 
      return 0;
    return -1;
  }

/* pack floats into send buffer */
int pvm_pkfloat(a,num,stride)
     float *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(float)*num;
    memcpy(&SendBuffer[Sendsize],a,nb);
    Sendsize+=nb;
    if(Sendsize<BUF_SIZE) 
      return 0;
    return -1;
  }

/* pack bytes into send buffer */
int pvm_pkbyte(a,num,stride)
     char *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(char)*num;
    memcpy(&SendBuffer[Sendsize],a,nb);
    Sendsize+=nb;
    if(Sendsize<BUF_SIZE) 
      return 0;
    return -1;
  }

/* Pack a string into the send buffer */
int pvm_pkstr(string)
     char *string;

  {
    int  len, nb;

/* save the length of the string for packing */
    len=strlen(string);
    nb=sizeof(int);
    memcpy(&SendBuffer[Sendsize],&len,nb);
    Sendsize+=nb;
    memcpy(&SendBuffer[Sendsize],string,len+1);
    Sendsize+=len+1;

#ifdef DEBUG
    printf("%f (%d): pack a string of length %6d\n",
	   MPI_Wtime(), pvm_mytid(),
	   len);
   fflush(NULL);
#endif

    if(Sendsize<BUF_SIZE) 
      return 0;
    return -1;
  }

/* pack boubles into the send buffer */
int pvm_pkdouble(a,num,stride)
     double *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(double)*num;
    memcpy(&SendBuffer[Sendsize],a,nb);
    Sendsize+=nb;
    if(Sendsize<BUF_SIZE) 
      return 0;
    return -1;
  }

/* unpack integers from the receive buffer */
int pvm_upkint(a,num,stride)
     int *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(int)*num;
    memcpy(a,&RecvBuffer[Recvsize],nb);

#ifdef DEBUG
    printf("%f (%d): unpack an integer %d from location %d\n",
	   MPI_Wtime(), pvm_mytid(),
	   *a,Recvsize);
   fflush(NULL);
#endif

    Recvsize+=nb;
    return 0;
  }

/* unpack unsigned integers from the receive buffer */
int pvm_upkuint(a,num,stride)
     unsigned int *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(unsigned)*num;
    memcpy(a,&RecvBuffer[Recvsize],nb);
    Recvsize+=nb;
    return 0;
  }

/* unpack shorts from the receive buffer */
int pvm_upkshort(a,num,stride)
     short *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(short)*num;
    memcpy(a,&RecvBuffer[Recvsize],nb);
    Recvsize+=nb;
    return 0;
  }

/* unpack unsigned shorts from the receive buffer */
int pvm_upkushort(a,num,stride)
     unsigned short *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(short)*num;
    memcpy(a,&RecvBuffer[Recvsize],nb);
    Recvsize+=nb;
    return 0;
  }

/* unpack floats from the receive buffer */
int pvm_upkfloat(a,num,stride)
     float *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(float)*num;
    memcpy(a,&RecvBuffer[Recvsize],nb);
    Recvsize+=nb;
    return 0;
  }

/* unpack bytes from the receive buffer */
int pvm_upkbyte(a,num,stride)
     char *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(char)*num;
    memcpy(a,&RecvBuffer[Recvsize],nb);
    Recvsize+=nb;
    return 0;
  }

/* unpack string from the receive buffer */
int pvm_upkstr(string)
     char *string;

  {
    int  len, nb;

/* length of the string for unpacking */
    nb=sizeof(int);
    memcpy(&len,&RecvBuffer[Recvsize],nb);
    Recvsize+=nb;
/* now get the string */
    memcpy(string,&RecvBuffer[Recvsize],len+1);
    Recvsize+=len+1;
    return 0;
  }

/* unpack doubles from the receive buffer */
int pvm_upkdouble(a,num,stride)
     double *a;
     int num;
     int stride;

  {
    int  nb;

    nb=sizeof(double)*num;
    memcpy(a,&RecvBuffer[Recvsize],nb);
    Recvsize+=nb;
    return 0;
  }

/* send buffer to destination with message tag */
int pvm_send(dstid, msgtag)
  int dstid;
  int msgtag;
 {

#ifdef DEBUG
   printf("%f (%d): Send to %d with tag %d length %d\n",
	  MPI_Wtime(), pvm_mytid(),
	  dstid,msgtag,Sendsize);
   fflush(NULL);
#endif

   return MPI_Send(&SendBuffer,Sendsize,MPI_BYTE,dstid,msgtag,MPI_COMM_WORLD);
 }

/* get mesage from source with message tag */
/* if srcid = -1 any source will do */
/* if msgtag= -1 any message tag will do */
int pvm_recv(srcid, msgtag)

/* blocking receive; hang around unitl a message is received then
   return bufid =1 */
  int srcid;
  int msgtag;
{
  int info;
  int length;
  MPI_Status status;

  if(srcid==-1)
    srcid=MPI_ANY_SOURCE;
  if(msgtag==-1)
    msgtag=MPI_ANY_TAG;

#ifdef DEBUG
  printf("%f (%d): blocking recv called with tag %d and source id %d\n",
	 MPI_Wtime(), pvm_mytid(),
	 msgtag,srcid);
  fflush(NULL);
#endif

  info=MPI_Recv(&RecvBuffer,BUF_SIZE,MPI_BYTE,srcid,msgtag,MPI_COMM_WORLD,&status);

  MPI_Get_count( &status,MPI_BYTE,&length); 
  SourceId=status.MPI_SOURCE;
  MessageTag=status.MPI_TAG;
  MessageLength=length;
  Recvsize=0;
  
  
#ifdef DEBUG
  printf("%f (%d): blocking recv returning msg from src %d tag %d length %d\n",
	 MPI_Wtime(), pvm_mytid(),
	 srcid, msgtag, length);
  fflush(NULL);
#endif

  return 1; /* return the bufid; always 1 */
}

/* Terminate the parallel region of the code and exit */
pvm_exit()
{
}

#ifdef DEBUG
/* Create an exit routine which will force MPI to quit so processors
   will not hang and tie up the machine until the wall clock limit is 
   reached  
*/

   void exit(flag)
   int flag;
{
   static int errorcode = 0;
   if (errorcode != 0)
     _exit(errorcode);

   printf("%f (%d): Modified Exit routine was called\n",
	  MPI_Wtime(), pvm_mytid());
   fflush(NULL);

   errorcode = -1;
   MPI_Abort(MPI_COMM_WORLD,errorcode);
}
#endif


/* return the processor id for calling processor */
pvm_mytid()
{
  int myid;
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  return myid;
}


/* non blocking message receive; Return if message not ready */
int  pvm_nrecv(srcid,msgtag)                                    
/* pvm nonblocking receive;  if message is not ready
   just return with a status of 0 otherwise return bufid=1 */

int srcid,msgtag;                                          
                                                           
{
  int bufid,count,flag;                                           
  MPI_Status status;                                         

  if(srcid==-1)
    srcid=MPI_ANY_SOURCE;
  if(msgtag==-1)
    msgtag=MPI_ANY_TAG;
#ifdef DEBUG
  printf("%f (%d): nonblocking recv called with tag %d and source id %d\n",
	 MPI_Wtime(), pvm_mytid(),
	 msgtag,srcid);
  fflush(NULL);
#endif

  bufid=MPI_Iprobe(srcid,msgtag,MPI_COMM_WORLD,&flag,&status);     
  if(flag==0)                                               
    {
      /* there is no message pending so we just return */                     
#ifdef DEBUG
      printf("%f (%d): nonblocking recv returning without msg\n",
	     MPI_Wtime(), pvm_mytid());
      fflush(NULL);
#endif
      return 0;                                                
    }
  else                                                       
    {
      /* message pending so receive the message */                 
      count=BUF_SIZE;
      bufid=MPI_Recv(&RecvBuffer,count,MPI_BYTE,srcid,msgtag,MPI_COMM_WORLD,&status);
      MPI_Get_count(&status,MPI_BYTE,&count);
      SourceId=status.MPI_SOURCE;
      MessageTag=status.MPI_TAG;
      MessageLength=count;
      Recvsize=0;
      bufid=1;

#ifdef DEBUG
      printf("%f (%d): nonblocking recv returning msg from src %d tag %d length %d\n",
	     MPI_Wtime(), pvm_mytid(),
	     srcid, msgtag, count);
      fflush(NULL);
#endif

      return bufid;
    }
}

/* message receive with time out */
int  pvm_trecv(srcid,msgtag,tim)                                    

int srcid,msgtag;                                          
struct timeval *tim;
                                                           
{
  int bufid,count,flag;                                           
  MPI_Status status;                                         
  double pgen_clock();
  double StartTime, EndTime;
  double MPI_CurrentTime, MPI_StartTime, MPI_MessageTime;
   

/* use the correct MPI values for any source and any tag */
   if(srcid==-1)
     srcid=MPI_ANY_SOURCE;

   if(msgtag==-1)
     msgtag=MPI_ANY_TAG;
   
  StartTime=pgen_clock();

  EndTime=StartTime+(*tim).tv_sec;

  
#ifdef DEBUG
    printf("%f (%d): We have started a message probe (timeout %f)\n",
	   MPI_Wtime(), pvm_mytid(), EndTime - StartTime);
    fflush(NULL);
    MPI_StartTime=MPI_Wtime();
#endif

  TotalMessages+= 1;
  while (pgen_clock()<=EndTime)
   {
     bufid=MPI_Iprobe(srcid,msgtag,MPI_COMM_WORLD,&flag,&status);     

     if(flag!=0)                                               
       goto GetMessage;  /* we have a message */

#ifdef _POSIX_PRIORITY_SCHEDULING
     sched_yield();
#endif
   }

    
#ifdef DEBUG
    printf("%f (%d): We have a time out in trecv\n",
	   MPI_Wtime(), pvm_mytid());
   fflush(NULL);
#endif

    return 0; /* we have a timeout */
    
GetMessage:

    count=BUF_SIZE;
    bufid=MPI_Recv(&RecvBuffer,count,MPI_BYTE,srcid,msgtag,MPI_COMM_WORLD,&status);     
    MPI_Get_count(&status,MPI_BYTE,&count);                  
    SourceId=status.MPI_SOURCE;
    MessageTag=status.MPI_TAG;
    MessageLength=count;

#ifdef DEBUG
     printf("%f (%d): we rcvd the message from %d with tag %d and length %d\n",
	    MPI_Wtime(), pvm_mytid(),
	    SourceId, MessageTag, MessageLength);
     MPI_CurrentTime = MPI_Wtime();
     MPI_MessageTime = MPI_CurrentTime - MPI_StartTime;
     printf("%f (%d): waited %f seconds for message\n",
	    MPI_CurrentTime, pvm_mytid(), MPI_MessageTime);
     fflush(NULL);
#endif

    Recvsize=0;   /* reset the buffer pointer */
    bufid=1;
    return bufid; /* return the buffer id */
}

/* get information about the message in the current receive buffer */
 int pvm_bufinfo(bufid,bytes,msgtag,tid)
 int bufid;
 int *bytes,*msgtag,*tid;
{
    if(bufid!=1)
       {printf("Bad buffer id on %d \n",pvm_mytid());
       fflush(NULL);
    }

   /* return the source, message type and length for the current buffer*/
   *tid=SourceId;
   *msgtag=MessageTag;
   *bytes=MessageLength;
 return 0;
}

/* Broadcast message to list of processors */
int pvm_mcast(tids, ntask, msgtag)
  int *tids;
  int ntask,msgtag;
 {
   int i,dstid;
   for (i=0;i<ntask;i++)
     {
       dstid=tids[i];
#ifdef DEBUG
   printf("%f (%d): sending mcast message to %d with msgtag of %d\n",
	  MPI_Wtime(), pvm_mytid(),
	  dstid,msgtag);
   fflush(NULL);
#endif
       MPI_Send(&SendBuffer,Sendsize,MPI_BYTE,dstid,msgtag,MPI_COMM_WORLD);
     }
 }


int pvm_setopt(what,val)
  int what,val;
{
  return 0;
}

int pvm_config(nhost,narch,hostp)
int *nhost,narch;
pvmhostinfo **hostp;
{
 *nhost=1;
#ifdef DEBUG
 printf ("%f (%d): pvm_config called\n",
	 MPI_Wtime(), pvm_mytid());
 fflush(NULL);
#endif
 return;
}

int pvm_get_PE(tid)
 int tid;
{
  /* this looks broken --ghood */
 if(tid==0) 
   return 0;
 else
   return 1;
}
#endif

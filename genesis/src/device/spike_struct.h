/* $Id: spike_struct.h,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
**
** $Log: spike_struct.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1998/03/31 22:10:24  dhb
** Initial revision
**
*/

#include "struct_defs.h"
 
struct doxc_type  {
  ELEMENT_TYPE
	int num_bins;
	float binwidth;
	int reset_mode;
	float threshold;
	int no1spks;
	int no2spks;
  float *sp1times;
  float *sp2times;
	int *xcarray;
	int curr1idx;
	int curr2idx;
	int calc1idx;
	int calc2idx;
	int maxspikes;
	float wintime;
	int dontnow;
	int allocated;
};
 
struct doac_type  {
	ELEMENT_TYPE
	int num_bins;
	float binwidth;
	int reset_mode;
	float threshold;
	int nospks;
	float *sptimes;
	int *acarray;
	int curridx;
	int calcidx;
	float wintime;
	int dont;
	int maxspikes;
	int allocated;
};
 
struct dointvl_type  {
	ELEMENT_TYPE
	int num_bins;
	float binwidth;
	int reset_mode;
	float threshold;
	int nospks;
	float fstspk;
	int *intvlarray;
	int allocated;
};

struct dosum_type  {
	ELEMENT_TYPE
	double insum;
};

#define NO_CLEAR_BINS 0
#define CLEAR_BINS 1
#define NEW_TABLE 2
#define FREE_RUN 0
#define TRIGGERED 1
#define SPIKES 2

struct new_peristim_type {
    ELEMENT_TYPE
    int     output;
    float   threshold;
    float   binwidth;
    int     num_bins;
    short   trigger_mode; /* FREE_RUN or TRIGGERED or SPIKES*/
    short   reset_mode;  /* NO_CLEAR_BINS or CLEAR_BINS */
    int     *table;
    int     bin_index;
    int     bin_steps;
    short   spiking;
    short   allocated;
};

struct new_freq_mon_type {
    BUFFER_TYPE
    float   exponent;
    float   frequency;
};

#define TABCREATE 200
#define TABFILL 201
#define TUPDATE 250
#define OPEN 300
#define CLOSE 301
#define FLUSH 302

struct timetable_type {
    ELEMENT_TYPE
    float activation;
    float act_val;
    int seed;
    int method;
    float meth_desc1;
    float meth_desc2;
    int meth_desc3;
    float maxtime;
    int tabpos;
    int maxpos;
	char *fname;
    float *timetable;
    short allocated;
};

struct event_tofile_type {
    ELEMENT_TYPE
    char *fname;
    FILE *fp;
    short open;
    float threshold;
};


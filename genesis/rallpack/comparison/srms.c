/*
** spikerms.c
** By Upinder S. Bhalla, May 1992, Caltech.
** Modified July 1992 to avoid multiple minima if waveform is unstable.
**
** This program calculates the rms difference between two
** spike trains. It does linear interpolation to handle differing
** timescales.
*/

#include <stdio.h>
#include <math.h>

#define MAXPTS 10000
#define MAXSPK 100

typedef struct spk_type {
	int ipeak,ivalley;
	float peak,valley;
	float tpeak,tvalley;
	float isi,ptp;
} SPK;

main(argc,argv)
	int	argc;
	char	**argv;
{
	FILE	*f1,*f2,*fopen();
	float 	t1[MAXPTS],t2[MAXPTS],v1[MAXPTS],v2[MAXPTS];
	float	shape[MAXPTS];
	SPK		spk1[MAXSPK],spk2[MAXSPK];
	float	tsumsq=0.0,psumsq=0.0,ssumsq=0.0,nsumsq=0.0;
	float	ftemp,foffset,interp,scale;
	int		itemp;
	int		nspk,npts1,npts2,nspk1=0,nspk2=0;
	double	sqrt();
	int		i,j,k;
	float	t,st,interp1,interp2;
	FILE	*out1,*out2;
	float	dt;

	if (argc<3) {
		printf("usage : %s file1 file2 [-out]\n",argv[0]);
		return;
	}

	f1=fopen(argv[1],"r");
	if (!f1) {
		printf("Could not open file %s\n",argv[1]);
		return;
	}
	f2=fopen(argv[2],"r");
	if (!f2) {
		printf("Could not open file %s\n",argv[2]);
		return;
	}

	if (argc==4 && strcmp(argv[3],"-out")==0) {
		out1=fopen("out1","w");
		out2=fopen("out2","w");
		if (!out1 || !out2) {
			printf("Could not open files out1 and out2\n",argv[1]);
			return;
		}
	} else {
		out1=out2=NULL;
	}

	for(i=0;fscanf(f1,"%f %f",&t1[i],&v1[i]) == 2;i++);
	npts1=i;
	dt=(t1[npts1-1]-t1[0])/(float)(npts1-1);
	for(i=0;fscanf(f2,"%f %f",&t2[i],&v2[i]) == 2;i++);
	npts2=i;

	/* Finding the spikes */
	for(i=3;i<npts1-2;i++) {
		if (v1[i-2]<v1[i] && v1[i-1]<=v1[i] &&
			v1[i]>v1[i+1] && v1[i]>v1[i+2]) {
			spk1[nspk1].ipeak=i;
			spk1[nspk1].peak=v1[i];
			spk1[nspk1].tpeak=t1[i];
			spk1[nspk1].valley=100000;
		}
		if (spk1[nspk1].valley>v1[i] && spk1[nspk1].tpeak>0 &&
			v1[i-3]>v1[i] && v1[i-2]>v1[i] && v1[i-1]>=v1[i] &&
			v1[i]<v1[i+1] && v1[i]<v1[i+2]) {
			spk1[nspk1].ivalley=i;
			spk1[nspk1].valley=v1[i];
			spk1[nspk1].tvalley=t1[i];
			nspk1++;
		}
	}

	for(i=3;i<npts2-2;i++) {
		if (v2[i-2]<v2[i] && v2[i-1]<=v2[i] &&
			v2[i]>v2[i+1] && v2[i]>v2[i+2]) {
			spk2[nspk2].ipeak=i;
			spk2[nspk2].peak=v2[i];
			spk2[nspk2].tpeak=t2[i];
			spk2[nspk2].valley=100000;
		}
		if (spk2[nspk2].valley>v2[i] && spk2[nspk2].tpeak>0 &&
			v2[i-3]>v2[i] && v2[i-2]>v2[i] && v2[i-1]>=v2[i] &&
			v2[i]<v2[i+1] && v2[i]<v2[i+2]) {
			spk2[nspk2].ivalley=i;
			spk2[nspk2].valley=v2[i];
			spk2[nspk2].tvalley=t2[i];
			nspk2++;
		}
	}

	nspk=(nspk1>nspk2) ? nspk2 : nspk1;

	tsumsq=0.0;
	/* comparing the spike (peak) intervals */
	for(i=1;i<nspk;i++) {
		spk1[i].isi = spk1[i].tpeak-spk1[i-1].tpeak;
		spk2[i].isi = spk2[i].tpeak-spk2[i-1].tpeak;
		/* Find the fractional difference */
		ftemp=2.0*(spk1[i].isi-spk2[i].isi)/(spk1[i].isi+spk2[i].isi);
		tsumsq+=ftemp*ftemp;
	}

	psumsq=0.0;
	/* comparing the spike (peak to peak) amplitudes */
	for(i=0;i<nspk;i++) {
		spk1[i].ptp = spk1[i].peak-spk1[i].valley;
		spk2[i].ptp = spk2[i].peak-spk2[i].valley;
		/* Find the fractional difference */
		ftemp=2.0*(spk1[i].ptp-spk2[i].ptp)/(spk1[i].ptp+spk2[i].ptp);
		psumsq+=ftemp*ftemp;
	}

	/* Comparing the spike shape. We need to scale each spike in 
	** time to do this. We will use linear interpolation. */
	j=1;
	k=1;
	t=spk1[0].tpeak;
	for(i=1;i<nspk;i++) {
		scale=spk2[i].isi/spk1[i].isi;
		for(;t<spk1[i].tpeak;t+=dt) {
			for(;t>t1[j] && j < npts1;j++);
			interp1=v1[j-1]+(v1[j]-v1[j-1])*
				(t-t1[j-1])/(t1[j]-t1[j-1]);

			st=spk2[i-1].tpeak+(t-spk1[i-1].tpeak)*scale;
			for(;st>t2[k] && k < npts2;k++);
			interp2=v2[k-1]+(v2[k]-v2[k-1])*
				(st-t2[k-1])/(t2[k]-t2[k-1]);
			if (out1) {
				fprintf(out1,"%f	%f\n",t,interp1);
				fprintf(out2,"%f	%f\n",t,interp2);
			}
			/* Get the fractional difference */
			ftemp=2.0*(interp1-interp2)/(spk1[i].ptp+spk2[i].ptp);
			ssumsq+=ftemp*ftemp;
			nsumsq+=1.0;
		}
	}
	printf("rms isi=%e,rms ptp=%e,rms shape=%e,\n normalized rms total=%e npts=%d nspk=%d\n",
		sqrt(tsumsq/(float)(nspk-1)),
		sqrt(psumsq/(float)nspk),
		sqrt(ssumsq/nsumsq),
		sqrt(tsumsq/(float)(nspk-1))+
		sqrt(psumsq/(float)nspk)+
		sqrt(ssumsq/nsumsq),
		(int)nsumsq,
		nspk);
	fclose(f1);
	fclose(f2);
	if (out1) {
		fclose(out1);
		fclose(out2);
	}

	exit(0);
}

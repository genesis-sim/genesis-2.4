#include <stdio.h>
#include <math.h>
#define PI 3.1415926535
#define DT 200e-6
#define RUNTIME 1.0
#define TAU0 0.04
#define VREST -0.07
#define VCHARGE 1.042
#define DATATYPE 4

main()
{
	float sum=0.0;
	int i;
	float Cn[100];
	float tau[100];
	float t;
	float Vm0,Vm999;
	FILE *v0,*v999,*fopen();
	FILE *v0_fmt1,*v999_fmt1;
	int temp;
	float ftemp;

	for(i=1;i<100;i++) {
		Cn[i]=2.0/(PI*PI*i*i+1.0);
		tau[i]=TAU0/(1.0+PI*PI*i*i); /* 40 millisec */
	}
	Cn[0]=0.687012;
	tau[0]=TAU0;

	v0=fopen("v0","w");
	v999=fopen("v999","w");
	v0_fmt1=fopen("v0_fmt1","w");
	v999_fmt1=fopen("v999_fmt1","w");

	temp=0;
	fwrite(&temp,sizeof(int),1,v0_fmt1);
	fwrite(&temp,sizeof(int),1,v0_fmt1);
	ftemp=DT;
	fwrite(&ftemp,sizeof(float),1,v0_fmt1);
	temp=DATATYPE;
	fwrite(&temp,sizeof(int),1,v0_fmt1);

	fwrite(&temp,sizeof(int),1,v999_fmt1);
	fwrite(&temp,sizeof(int),1,v999_fmt1);
	ftemp=DT;
	fwrite(&ftemp,sizeof(float),1,v999_fmt1);
	temp=DATATYPE;
	fwrite(&temp,sizeof(int),1,v999_fmt1);

	for (t=0.0;t<=RUNTIME;t+=DT) {
		Vm0=Vm999=0.0;
		for(i=0;i<100;i++) {
			Vm0+=Cn[i]*exp(-t/tau[i]);
			if (i%2)
				Vm999-=Cn[i]*exp(-t/tau[i]);
			else
				Vm999+=Cn[i]*exp(-t/tau[i]);
		}
		fprintf(v0,"%f	%f\n",t,VREST + VCHARGE * (1.0 - Vm0));
		fprintf(v999,"%f	%f\n",t,VREST + VCHARGE * (1.0 - Vm999));

		sum=VREST + VCHARGE * (1.0 - Vm0);
		fwrite(&sum,sizeof(float),1,v0_fmt1);
		sum=VREST + VCHARGE * (1.0 - Vm999);
		fwrite(&sum,sizeof(float),1,v999_fmt1);
	}
	fclose(v0);
	fclose(v999);
	fclose(v0_fmt1);
	fclose(v999_fmt1);
}

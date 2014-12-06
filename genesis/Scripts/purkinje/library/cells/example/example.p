
// This is an artificial cell that I created with some creativity,
// based on the example granule cell (that Xodus does not draw
// properly for some reason).  This cell is intented to be a second
// example of how to plugin different cells into the tutorial.

*asymmetric
*relative

*set_global	RM	10.0
*set_global	RA	0.2
*set_global	CM	0.01
*set_global	EREST_ACT	-0.060


*polar
*start_cell /library/notfakespine4
notfakespine4	none		10	0	0	2
neck		.		1	0	80	0.2
head		.		3	0	90	1
neck2		notfakespine4	1	90	90	0.2
head2		.		3	90	80	1
neck3		notfakespine4	1	180	100	0.2
head3		.		3	180	90	1
neck4		notfakespine4	1	270	100	0.2
head4		.		3	270	90	1

*makeproto /library/notfakespine4

*cartesian

*start_cell
soma		none		0	0	8	6

*polar

// trunk[0]	soma		40	20	10	2
// trunk[1]	.		40	80	10	1.5	
// trunk[2]	.		20	110	20	1.4	
// trunk[3]	.		20	140	10	1.3	
// trunk[4]	.		20	160	10	1.3	
// trunk[5]	.		20	160	20	1.3	
// trunk[6]	.		20	180	10	1.3	


*compt /library/notfakespine4

// periph1[0]	trunk[6]	10	0	10	1.25	
periph1[0]	soma	10	0	10	1.25	
periph1[1]	.		10	0	20	1.25	
periph1[2]	.		10	0	30	1.25	
periph1[3]	.		10	0	10	1.25	
periph1[4]	.		10	0	20	1.25	

periph11[0]	.		10	5	20	1.2	
periph11[1]	.		10	20	30	1.2	
periph11[2]	.		10	40	30	1.18
periph11[3]	.		10	30	20	1.17
periph11[4]	.		10	50	10	1.16
periph11[5]	.		10	40	30	1.15
periph11[6]	.		10	30	30	1.14
periph11[7]	.		10	20	20	1.13
periph11[8]	.		10	10	10	1.12
periph11[9]	.		10	10	10	1.11
periph11[10]	.		10	20	20	1.10
periph11[11]	.		10	30	25	1.09
periph11[12]	.		10	50	20	1.08
periph11[13]	.		10	50	15	1.07
periph11[14]	.		10	30	20	1.06
periph11[15]	.		10	40	10	1.05
periph11[16]	.		10	20	30	1.04
periph11[17]	.		10	20	20	1.03
periph11[18]	.		10	10	30	1.02
periph11[19]	.		10	40	10	1.01
periph11[20]	.		10	30	20	1.00

periph12[0]	periph1[4]	10	-80	20	1.2	
periph12[1]	.		10	-70	30	1.2	
periph12[2]	.		10	-90	10	1.18
periph12[3]	.		10	-80	30	1.16
periph12[4]	.		10	-90	50	1.15
periph12[5]	.		10	-100	30	1.14
periph12[6]	.		10	-90	30	1.13
periph12[7]	.		10	-100	50	1.12
periph12[8]	.		10	-90	30	1.11
periph12[9]	.		10	-80	40	1.10
periph12[10]	.		10	-80	20	1.09
periph12[11]	.		10	-90	30	1.08
periph12[12]	.		10	-70	30	1.07
periph12[13]	.		10	-80	40	1.06
periph12[14]	.		10	-90	10	1.05
periph12[15]	.		10	-100	30	1.04
periph12[16]	.		10	-90	10	1.03
periph12[17]	.		10	-80	30	1.02

// periph2[0]	trunk[6]	10	170	20	1.2
periph2[0]	soma	10	170	20	1.2
periph2[1]	.		10	180	30	1.2
periph2[2]	.		10	160	40	1.19
periph2[3]	.		10	150	20	1.18
periph2[4]	.		10	170	10	1.17
periph2[5]	.		10	190	10	1.16
periph2[6]	.		10	180	20	1.15
periph2[7]	.		10	180	30	1.14
periph2[8]	.		10	170	30	1.13
periph2[9]	.		10	160	10	1.12
periph2[10]	.		10	180	20	1.11
periph2[11]	.		10	160	10	1.1
periph2[12]	.		10	170	20	1.1

periph21[0]	.		10	170	20	1.03
periph21[1]	.		10	160	20	1.02
periph21[2]	.		10	150	30	1.01
periph21[3]	.		10	150	10	1.0
periph21[4]	.		10	140	30	1.0
periph21[5]	.		10	150	20	1.0

periph22[0]	periph2[12]	10	185	20	1.05
periph22[1]	.		10	190	20	1.03
periph22[2]	.		10	200	30	1.02
periph22[3]	.		10	210	30	1.01
periph22[4]	.		10	200	20	1.0
periph22[5]	.		10	220	30	1.0
periph22[6]	.		10	220	20	1.0
periph22[7]	.		10	210	10	1.0
periph22[8]	.		10	200	30	1.0

deep0[0]	soma		10	55	150	1.0
deep0[1]	.		10	45	160	0.8
deep0[2]	.		10	65	150	0.8
deep0[3]	.		10	35	140	0.7
deep0[4]	.		10	45	130	0.6
deep0[5]	.		10	55	150	0.5
deep0[6]	.		10	45	160	0.5

deep1[0]	soma		10	125	140	1.0
deep1[1]	.		10	115	150	0.8
deep1[2]	.		10	125	140	0.8
deep1[3]	.		10	135	160	0.8
deep1[4]	.		10	125	170	0.7
deep1[5]	.		10	135	150	0.7
deep1[6]	.		10	145	140	0.6
deep1[7]	.		10	155	150	0.6
deep1[8]	.		10	135	150	0.5

deep2[0]	soma		10	-25	150	1.0
deep2[1]	.		10	-35	160	0.8
deep2[2]	.		10	-45	150	0.8
deep2[3]	.		10	-35	140	0.7
deep2[4]	.		10	-65	150	0.6
deep2[5]	.		10	-55	130	0.6
deep2[6]	.		10	-35	150	0.5
deep2[7]	.		10	-45	140	0.5

deep3[0]	soma		10	-145	130	1.0
deep3[1]	.		10	-155	140	0.8
deep3[2]	.		10	-145	130	0.8
deep3[3]	.		10	-125	150	0.7
deep3[4]	.		10	-135	160	0.6
deep3[5]	.		10	-145	170	0.5
deep3[6]	.		10	-155	150	0.5
deep3[7]	.		10	-135	160	0.5


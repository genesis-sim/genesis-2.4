/*
** $Id: profile.h,v 1.3 2005/07/01 10:16:10 svitak Exp $
** $Log: profile.h,v $
** Revision 1.3  2005/07/01 10:16:10  svitak
** Restore of accidentally deleted file.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:05:41  dhb
** Initial revision
**
*/

#define	MAXPROFILE	100

static	double	ProfileStart[MAXPROFILE];
static	double	ProfileVal[MAXPROFILE];
static	char	ProfileName[MAXPROFILE][80];
extern	double	rtd_t();

#define InitProfile() { int	index; for(index=0;index<MAXPROFILE;index++) ProfileVal[index] = 0; }
#define	StartProfile(index) ProfileStart[index] = rtd_t()
#define	EndProfile(index) ProfileVal[index] += rtd_t() - ProfileStart[index]
#define	GetProfile(index) ProfileVal[index]
#define	SetPname(index,s) strcpy(&ProfileName[index][0],s)
#define	GetPname(index)	&ProfileName[index][0]

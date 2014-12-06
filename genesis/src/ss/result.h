/*
** $Id: result.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $
** $Log: result.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/05/09 14:55:29  mhucka
** Portability fix for AIX.
**
** Revision 1.2  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.1  1992/12/11 19:05:10  dhb
** Initial revision
**
*/

#ifndef RESULT_H
#define RESULT_H

#ifdef aix
/* AIX defines this in /usr/include/reloc.h.  Bad AIX, bad.  No biscuit.  */
#undef r_type
#endif

/*
** Result structures used in script expression evaluations
*/

typedef struct _localval
  {
    short	l_type;
    short	l_offs;
  } LocalVal;

typedef union _resultvalue
  {
    int		r_int;
    double	r_float;
    char	*r_str;
    LocalVal	r_loc;
  }ResultValue;

typedef struct _result
  {
    int		r_type;
    ResultValue	r;
  } Result;



#endif

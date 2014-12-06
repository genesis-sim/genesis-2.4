/*
** $Id: result.h,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
** $Log: result.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2001/04/25 17:16:58  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.1  1995/01/13 01:09:48  dhb
** Initial revision
**
**
** This file is from GENESIS 1.4.1
**
** Revision 1.1  1992/12/11  19:05:10  dhb
** Initial revision
**
*/

#ifndef RESULT_H
#define RESULT_H

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

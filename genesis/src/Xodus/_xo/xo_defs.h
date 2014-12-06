/* $Id */
/*
 * $Log: xo_defs.h,v $
 * Revision 1.1  2005/06/17 21:23:52  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:22  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1994/03/22 15:41:13  bhalla
 * Added extern definition for xoFullName
 *
 * Revision 1.3  1994/02/02  20:26:05  bhalla
 * *** empty log message ***
 * */

#ifndef _xo_defs_h
#define	_xo_defs_h

  /* Add the xodus actions */
#define B1DOWN 1000001
#define B2DOWN 1000002
#define B3DOWN 1000003
#define ANYBDOWN 1000004

#define B1UP 1000005
#define B2UP 1000006
#define B3UP 1000007
#define ANYBUP 1000008

#define B1DOUBLE 1000009
#define B2DOUBLE 1000010
#define B3DOUBLE 1000011
#define ANYBDOUBLE 1000012

#define NOBMOVE 1000013
#define B1MOVE 1000014
#define B2MOVE 1000015
#define B3MOVE 1000016
#define ANYBMOVE 1000017

#define NOBENTER 1000018
#define B1ENTER 1000018
#define B2ENTER 1000020
#define B3ENTER 1000021
#define ANYBENTER 1000022

#define NOBLEAVE 1000023
#define B1LEAVE 1000024
#define B2LEAVE 1000025
#define B3LEAVE 1000026
#define ANYBLEAVE 1000027

#define KEYPRESS 1000028

#define UPDATEFIELDS 1000029
#define XUPDATE UPDATEFIELDS

#define XODRAG 1000030
#define XODROP 1000031
#define XOWASDROPPED 1000032

#define XOCOMMAND 1000033
#endif

extern char *xoFullName();

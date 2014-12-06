/*
 *
 * FILE: fac_debug.h
 *       Various macros to help in debugging genesis libraries.
 *
 * Author: Mike Vanier
 *
 * $Log: fac_debug.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  1999/12/29 10:27:32  mhucka
 * Removed (admittedly in an ugly way) depencies on GCC.
 *
 */

/*
 *
 * Copyright (c) 1999 Michael Christopher Vanier
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without license
 * or royalty fees, to use, copy, modify, and distribute this software and its
 * documentation for any purpose, provided that the above copyright notice and
 * the following two paragraphs appear in all copies of this software.
 *
 * In no event shall Michael Christopher Vanier (hereinafter referred to as
 * "the author") or the Genesis Developer's Group be liable to any party for
 * direct, indirect, special, incidental, or consequential damages arising out
 * of the use of this software and its documentation, even if the author
 * and/or the Genesis Developer's Group have been advised of the possibility
 * of such damage.
 *
 * The author and the Genesis Developer's Group specifically disclaim any
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose.  The software
 * provided hereunder is on an "as is" basis, and the author and the Genesis
 * Developer's Group have no obligation to provide maintenance, support,
 * updates, enhancements, or modifications.
 *  
 */

#ifndef FAC_DEBUG_H
#define FAC_DEBUG_H

/*
 * Definition of macros to make error-handling code more concise and less
 * gross.
 */

/*
 * 1999-12-12 <mhucka@bbb.caltech.edu>
 * The use of ERROR2 and ERROR3 is a grungy hack to handle variable
 * arguments.  Mike V.'s original code assumed gcc, which has a vararg
 * mechanism for macros, but for portability, I modified all these ERROR
 * macros to use a more portable but less elegant method.
 */

#define  ERROR2(format, arg1, arg2)           \
       Error();                               \
       printf(format , arg1, arg2);           \
       return 0;

#define  ERROR3(format, arg1, arg2, arg3)     \
       Error();                               \
       printf(format , arg1, arg2, arg3);     \
       return 0;

#define  ERROR_IF(test, format, arg1, arg2)   \
       if (test)                              \
       {                                      \
           Error();                           \
           printf(format, arg1, arg2);        \
           return 0;                          \
       }

/* This is the same as ERROR_IF, but returns NULL instead. */

#define  ERRORN_IF(test, format, arg1, arg2)  \
       if (test)                              \
       {                                      \
           Error();                           \
           printf(format, arg1, arg2);        \
           return NULL;                       \
       }

#define MYPATH     Pathname((Element *)element)
#define PARENTPATH Pathname((Element *)parent)

/* Function inlining; not all c compilers support this. */
#if (!defined __GNUC__)
#  define inline
#endif

#endif  /* FAC_DEBUG_H */


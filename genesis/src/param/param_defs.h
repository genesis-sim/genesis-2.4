/*
 *
 * Copyright (c) 1997, 1998, 1999 Michael Christopher Vanier
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * In no event shall Michael Vanier or the Genesis Developer's Group
 * be liable to any party for direct, indirect, special, incidental, or
 * consequential damages arising out of the use of this software and its
 * documentation, even if Michael Vanier and the Genesis Developer's
 * Group have been advised of the possibility of such damage.
 *
 * Michael Vanier and the Genesis Developer's Group specifically
 * disclaim any warranties, including, but not limited to, the implied
 * warranties of merchantability and fitness for a particular purpose.
 * The software provided hereunder is on an "as is" basis, and Michael
 * Vanier and the Genesis Developer's Group have no obligation to
 * provide maintenance, support, updates, enhancements, or modifications.
 *
 */

#ifndef PARAM_DEFS_H
#define PARAM_DEFS_H

#define TABCREATE       200   /* also used by tabchannels */
#define TABDELETE       202   /* also used by tabchannels */
#define RANDOMIZE       1003
#define REPRODUCE       1004
#define CROSSOVER       1005
#define INVERT          1006
#define MUTATE          1007
#define FITSTATS        1008
#define RECENTER        1009
#define UPDATE_PARAMS   1010
#define ACCEPT          1011
#define DISPLAY         1012
#define SAVE3           1013
#define RESTORE3        1014
#define INITSEARCH      1015
#define RESTART         1016
#define EVALUATE        1017
#define DISPLAY2        1018
#define RESTART2        1019
#define LOADBEST        1020
#define UPDATE_PARAMS2  1021
#define LOADTRACE       1022
#define NEXTTRACE       1023
#define RESETALL        1024
#define SAVEBEST        1025
#define RESTOREBEST     1026

#define INPUT           10002
#define INPUT2          10003

#ifdef  TINY
#undef  TINY
#endif

#define TINY       1.0e-8

#ifdef  STINY
#undef  STINY
#endif

#define STINY 1.0e-20 /* "supertiny" */

/* Some c compilers define HUGE in math.h */

#ifdef  HUGE
#undef  HUGE
#endif

#define HUGE 1.0e30

#define BADMATCH       HUGE
#define BIGNUM         9999999
#define LABEL_SIZE     80
#define MAX_LINELEN    150

/*
 * If there's an error, return a massive penalty;
 * used by match routines.
 */

#define ERROR_PENALTY 1.0e6

/*
 * Macro to print something if the PARAM_DEBUG flag is on;
 * note that this will only work with gcc since ansi C
 * does not allow macros with variable-length argument
 * lists.
 */

#if defined (__GNUC__)
#if defined (PARAM_DEBUG)
#define DPRINTF(format, args...)  fprintf (stderr, format , ## args)
#else
#define DPRINTF(format, args...)
#endif
#else
/* 2000-05-29 <mhucka@bbb.caltech.edu>
   Don't indent the following #define, or you will screw up the
   IRIX 6.5.3 C preprocessor.  (No, I'm not kidding.)
*/
#define DPRINTF(stuff) {}
#endif  /* __GNUC__ */

#endif  /* PARAM_DEFS_H */


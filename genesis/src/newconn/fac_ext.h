/*
 *
 * FILE: fac_ext.h
 *
 *      Externally-required definitions for the fac library.
 *
 *
 * AUTHOR: Mike Vanier
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

#ifndef FAC_EXT_H
#define FAC_EXT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sim_ext.h"
#include "shell_func_ext.h"
#include "fac_debug.h"
#include "fac_defs.h"
#include "newconn_struct.h"
#include "fac_struct.h"
#include "synaptic_event.h"
#include "printoptusage.h"

/*
 * External functions used by this library.  ANSI-style declarations
 * don't seem to work due to type conversion weirdnesses.  Oh well.
 */

/*
extern int       Error();
extern int       Warning();
extern int       InvalidAction();
extern int       CallEventAction();
extern char     *Pathname();
extern float     ran1();
extern int       initopt();
extern int       G_getopt();
extern char*     G_optopt;
*/
extern int       Cellreader_read_channel();
extern void      RemovePendingSynapticEvents();
extern void      RemapMsgData();
extern void      SaveSynapticEvents();
extern int       RestoreSynapticEvents();
extern int       SaveSizeSynapticEvents();
extern int       Synchan_SET();
extern int       Synchan_SHOW();


#endif /* FAC_EXT_H */

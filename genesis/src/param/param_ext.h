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

#ifndef PARAM_EXT_H
#define PARAM_EXT_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "sim_ext.h"
#include "shell_func_ext.h"
#include "param_defs.h"
#include "param_struct.h"

extern float  rangauss();

/* version of free that checks for NULL pointers: */

extern void paramFree();

/*
 * version of fwrite that checks to make sure it isn't writing
 * from a NULL pointer:
 */

extern size_t paramFwrite();

extern void   CopyParamLabel();
extern double GetparamGA();

#endif  /* PARAM_EXT_H */


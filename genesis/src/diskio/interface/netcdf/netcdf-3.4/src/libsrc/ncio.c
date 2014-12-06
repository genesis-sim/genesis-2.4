/*
 * $Id: ncio.c,v 1.1.1.1 2005/06/14 04:38:31 svitak Exp $
 */

#if defined(_CRAY)
#   include "ffio.c"
#else
#   include "posixio.c"
#endif

/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /cvsroot/genesis-sim/genesis2/genesis/src/diskio/interface/netcdf/netcdf-3.4/src/ncgen/generic.h,v 1.1.1.1 2005/06/14 04:38:30 svitak Exp $
 *********************************************************************/

#ifndef UD_GENERIC_H
#define UD_GENERIC_H

union generic {			/* used to hold any kind of fill_value */
    float floatv;
    double doublev;
    int intv;
    short shortv;
    char charv;
};

#endif

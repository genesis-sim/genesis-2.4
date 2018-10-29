genesis-2.4
===========

Repository for continued development of the GENESIS 2.4 neural simulator
------------------------------------------------------------------------

The files here are based on the November 11, 2014 public release of
GENESIS 2.4, available from http://genesis-sim.org/GENESIS. The directory
'genesis' contains these, with any changes made since that time.

The 'pgenesis' directory contains development versions of parallel
GENESIS (PGENESIS) version 2.4, updated from version 2.3.1. Not all
features of GENESIS 2.4 have yet been incorporated into PGENESIS 2.4.

The December 6, 2014 update contains new documentation and compiled binaries for
using GENESIS under MS Windows with Cygwin.

The December 11, 2016 update contains source code (in genesis/src/newconn) for
'facdep_rules', a new hsolveable short term facilitation/depression synaptic
plasticity object. Documentation is in genesis/Doc/facdep_rules.txt.

The July 8, 2017 update contains source code and documentation for
'facdep_rules2', an improved version of 'facdep_rules'.

A new version of genesis/src/sys/system.c fixes compilation problems with
recent versions of glibc.

The October 29, 2018 update contains two new objects and documentation
for calculating extracellular field potentials when the sources are cells
that have been taken over by the fast hsolve solver object.
See Doc/efield2.txt and Doc/efield_cells.txt for details.

The 'genesis-binaries' directory contains pre-compilied binary
distributions, such as 'genesis-binaries/genesis-2.4-Cygwin-bin.tar.gz'.

Upgrading GENESIS 2.4
---------------------

If you have an installed GENESIS 2.4 distribution, you can simply download
the new files into the appropriate 'genesis' directories. Then, do the usual
sequence of 'make clean; make; make install' in genesis/src.

If you would like to download the entire package, click on 'Clone or
Download' for genesis-sim/genesis-2.4 then "Download Zip" this will
give genesis-2.4-master.zip with:

genesis  genesis-binaries  pgenesis  README.md

Unless you want to install pgenesis also, you can delete it and the
genesis-binaries tree.

Alternatively, if you are familiar with using git and want to clone your
own copy of the repository, you may do that.

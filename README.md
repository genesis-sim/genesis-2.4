genesis-2.4
===========

Repository for continued development of the GENESIS 2.4 neural simulator
------------------------------------------------------------------------

The files here have been updated from the November 11, 2014 public release of
GENESIS 2.4, from http://genesis-sim.org/GENESIS. The directory
'genesis' contains these, with any changes made since that time.

The 'pgenesis' directory contains the official (May 2019) release of
parallel GENESIS (PGENESIS) version 2.4, updated from the December
2014 preliminary release.

The most recent update of this repository contains the May 2019 GENESIS 2.4
update and the official PGENESIS 2.4 release. This is a significant update
that has now been packaged as 'genesis-pgenesis-2.4-05-2019.tar.gz' on the
GENESIS web site http://genesis-sim.org/GENESIS. Further updates to GENESIS
and PGENESIS will continue to appear here.

The May 2019 Update
-------------------

Recent updates to GENESIS and a merge of the GENESIS/PGENESIS
development branch from the U.S. Army Research Laboratory have
addressed a number of issues in order to increase its scalability for modeling
very large networks of multicompartmental neurons on supercomputers.

Optional installation of a garbage collector library drastically
reduces memory usage, allowing for models of millions of
neurons. Various run-time errors and integer overflow issues were
eliminated. Changes in PGENESIS insured the repeatability of random
connections between simulation runs. Updates of GENESIS allow the fast
"hsolve" solver to be used with new synaptic plasticity models and
measurements of cortical field potentials. The efficient delivery of
spike events when hsolve is used with large network models results in
increases in simulation speed by factors of 10 to 20.

Previous updates to GENESIS 2.4 are:

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

Python analysis scripts in Scripts/gpython-tools are updated to use
either Python 2 or 3. 

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
own copy of the repository, you may do that

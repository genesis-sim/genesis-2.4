

                           G E N E S I S
		   The GEneral NEural Simulation System
                     Version 2.4 May 2019 Update
           Last edited: Tue May 28 17:26:35 MDT 2019

Introduction
------------

This directory contains the 'genesis' portion of the combined May 2019
GENESIS 2.4 Update and the PGENESIS 2.4 final release. You can view an
index to this distribution by pointing your browser to index.html. The
HTML version of this document README.html contains links to the documentation
and web sites that are mentioned below.

The November 2014 release of GENESIS 2.4 was intended to be the last
packaged GENESIS distribution for download from the GENESIS web site at
http://www.genesis-sim.org/GENESIS. Updates to GENESIS and PGENESIS
are now obtained from the Repository for Continued Development of the
GENESIS 2.4 Neural Simulator (https://github.com/genesis-sim/genesis-2.4).

Although the version number has not changed, recent user-contributed
updates are significant enough to warrant this new packaged
distribution "genesis-pgenesis-2.4-05-2019.tar.gz" on the GENESIS web
site.  Recent updates to GENESIS and a merge of the GENESIS/PGENESIS
development branch from the U.S. Army Research Laboratory have
addressed a number of issues that limited its scalability for modeling
very large networks on high performance computing resources. Optional
installation of a garbage collector library drastically reduces memory
usage, allowing for models of millions of neurons. Various run-time
errors and integer overflow issues were eliminated. Changes in
PGENESIS insured the repeatability of random connections between
simulation runs. Updates of GENESIS since the 2014 release allow the
fast "hsolve" solver to be used with new synaptic plasticity models
and electric field measurements.

If you would like to run large-scale GENESIS models on supercomputers
such as those at the Neuroscience Gateway Portal (https://www.nsgportal.org/),
be sure to read the documentation in the genesis-2.4/pgenesis directory.

A detailed list of source code changes from previous versions is in
ChangeLog.txt. New features of version 2.3 and 2.4 are described in
Doc/Changes.txt and Hyperdoc/Manual-2.html.

Directions for installing binary distributions may be found in README.bindist
in this directory. Directions for building and installing source distributions
are in src/README. Be sure to read the directions
concerning the installation of the ".simrc" file.  Directions for printing
and using the documentation may be found in Doc/README. The
Scripts/README file describes the demonstration and tutorial simulations
that are included with this distribution.

Description of GENESIS
----------------------

GENESIS is a general purpose simulation platform which was developed
to support the simulation of neural systems ranging from complex
models of single neurons to simulations of large networks made up of
more abstract neuronal components. Most GENESIS applications involve
realistic simulations of biological neural systems. Currently GENESIS
and the parallel version PGENESIS are used on parallel supercomputers
to simulate cortical networks that contain millions of
mult-compartmental neurons. Although the software can also model more
abstract networks, there exist other simulation packages that are more
suitable for networks of point integrate-and-fire neurons, or
connectionist modeling.


A detailed guide to the GENESIS neuroscience tutorials and the construction
of GENESIS simulations is given in:

   The Book of GENESIS: Exploring Realistic Neural Models with the GEneral
   NEural SImulation System, Second Edition, by James M. Bower and David
   Beeman, Springer-Verlag (1998).  The free Internet Edition is available
   at http://www.genesis-sim.org/GENESIS/bog/bog.html

Additional hypertext documentation, including a beginners guide to
UNIX/Linux commands, can be found in the Tutorials directory. You can
view or download (for installation in Tutorials) a set of tutorials
on creating models in GENESIS at http://genesis-sim.org/GENESIS/UGTD.html.

Source code contributed by GENESIS users which must be compiled is found in
src/contrib; other contributions, such as scripts and utilities are in contrib.

Obtaining GENESIS
------------------

GENESIS source and binary distributions can be obtained from:
http://genesis-sim.org/GENESIS, or from the Repository for Continued
Development of the GENESIS 2.4 Neural Simulator:
https://github.com/genesis-sim/genesis-2.4

Machine dependence
------------------

GENESIS and its graphical front-end XODUS are written in C and are
known to run under many UNIX-based systems with the X Window System,
as well as Mac OSX and Windows with Cygwin. It has also been used
under various MS Windows virtual environments for running Linux. Most
recently, this update with PGENESIS has been used to run large cortical
network simulations on supercomputer clusters at the San Diego
Supercomputer Center and the US Army Research Laboratory.

The current GENESIS 2.4 version has been tested on various recent
Linux distributions including Debian, Ubuntu, CentOS, RHEL, and
Fedora.

In the past GENESIS has been successfully installed on Cygwin (32 bit
version, which works with 64 bit Windows). The file src/Makefile.dist
has definition for other older UNIX systems tested for version 2.3.
This includes

  * x86-based FreeBSD 4.1
  * IBM SP2 systems running AIX 4.2 and 4.3
  * PPC-based Linux systems running LinuxPPC, SUSE 7.0, and OSX
  * SGI MIPS-based systems running IRIX 5.x and 6.5.x
  * Sun SPARC and x86 systems running Solaris 2.6, and 2.7
  * Sun SPARC-based systems running SunOS 4.1.x
  * Sun SPARC-based running Solaris 2.5/2.5.1
  * HP-UX systems
  * DECStation running Ultrix 4.x or 3.x
  * Cray T3E and T3D

We welcome feedback on experiences with installation of GENESIS and
PGENESIS. If you have problems, please post a description to the
GENESIS Users Mailing List.  As always, we are grateful for updates
and fixes submitted to the Repository for Continued Development of the
GENESIS 2.4 Neural Simulator at https://github.com/genesis-sim/genesis-2.4.

GENESIS Users Mailing List
--------------------------

Serious users of GENESIS are advised to join the GENESIS
genesis-sim-users mailing list at
https://lists.sourceforge.net/lists/listinfo/genesis-sim-users.

Disclaimer
----------

It should be understood that this software system is being provided for
general distribution as a public service to the neural network and
computational neuroscience communities.  We make no claims as to the
quality or functionality of this software for any purpose whatsoever and
its release does not constitute a commitment on our part to provide support
of any kind.

----------------
Copyright Notice
----------------

  Copyright 1988 - 2001 by the California Institute of Technology

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation.  Portions of this program are in library form.  The
  libraries are also free software; you can redistribute them and/or modify
  them under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2.1 of the License, or (at
  your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License and the
  GNU Lesser General Public License along with this program; if not, write to
  the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
  Boston, MA 02110-1301  USA.

  Recent additions to the GENESIS libraries are copyrighted by other
  institutions or authors, and are covered by the GNU General Public
  License (GPL), GNU Lesser General Public License (LGPL), or by other
  licenses that are compatible with the GPL and that do not restrict the
  free distribution of GENESIS.  These licenses appear with these modules.

  For a statement of the GPL, see the file "GPLicense".
  For a statement of the LGPL, see the file "LGPLicense".

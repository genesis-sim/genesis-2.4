               PGENESIS 2.4 Installation and Use
               --------------------------------

        	   Nigel Goddard & Greg Hood
		Pittsburgh Supercomputing Center

	     (Updated May 28, 2019 for PGENESIS 2.4 final release)
	     (Updated December 9, 2014 for PGENESIS 2.4 preliminary release)
	     (Updated December 18, 2008 for PGENESIS 2.3.1)
             (Updated March 16, 2006 for PGENESIS 2.3)
             (Updated March 21, 2003 for PGENESIS 2.2.1)

             Official PGENESIS 2.4 Release - May 31, 2019

This version of PGENESIS is intended to be installed in the
'genesis-2.4' directory along with the 'genesis' directory from the
combined May 2019 GENESIS 2.4 update and the PGENESIS 2.4 final
release. It replaces the preliminary PGENESIS 2.4 release and contains
updates, bug fixes, and links to new objects and commands from the May
2019 update of GENESIS 2.4. In particular, it incorporates a number of
modifications to both GENESIS and PGENESIS provided by the U. S. Army
Research Laboratory. These resolve a number of issues that limited its
scalability to model very large networks on high performance computing
resources. Optional installation of a garbage collector library
drastically reduces memory usage for models of over 10,000
neurons. Various run-time errors and integer overflow issues were
eliminated. Changes in PGENESIS insured the repeatability of random
connections between simulation runs.

NOTE: Before installing this software, please check that you have the latest
      versions of GENESIS and PGENESIS 2.4 by checking the GENESIS web site:

      http://www.genesis-sim.org/GENESIS/

      or the Repository for Continued Development of the GENESIS 2.4
      Neural Simulator (https://github.com/genesis-sim/genesis-2.4).
      
      We *strongly* advise you to download the latest version if you
      don't already have it. The GENESIS web page also gives contact
      information and information about the sourceforge
      genesis-sim-users mailing list.

INTRODUCTION
-------------

PGENESIS (Parallel GENESIS) is designed to allow researchers to run
simulations faster on parallel hardware; it supports both running
simulations of large networks on multiple processors, and running many
simulations concurrently (e.g. for parameter searching). PGENESIS
will also run on a single workstation or multi-core PCs, and this can
be useful for simulation development. Currently, it supports network
simulations that contain millions of neurons on supercomputer
clusters.

If you have a network model and want to use PGENESIS, you will have to
partition your model so that specific neurons are created on each of
the PGENESIS processes, and then connect them up using the PGENESIS
raddmsg or rvolumeconnect script functions.  Unfortunately, with
PGENESIS this is not automatic, although the process is well documented
with tutorials and example scripts.

PGENESIS will run on almost any parallel cluster, SMP, supercomputer,
or network of workstations where MPI and/or PVM is supported, and on
which serial GENESIS itself is runnable.  It is much easier to run on
a network of workstations if they have a shared file system so that
any workstation can access any file.  If this is not the case, you
will need to install PGENESIS independently on each workstation and
you will need to have a copy of the files required by your simulation
(data files, script files, etc) on each workstation, preferably in an
identically named directory path.


COMPATIBILITY WITH REGULAR GENESIS
----------------------------------

PGENESIS 2.4 is compatible with GENESIS version 2.4, and contains a few
changes in 'Makefile.dist' for modern operating systems.

Versions of PGENESIS older than 2.4 will not work with GENESIS 2.4 or later.


HOW IT WORKS (IN A NUTSHELL)
----------------------------

PGENESIS adds a library (parlib) to an installed version of GENESIS,
creating an executable "pgenesis" that has the usual GENESIS objects and
commands, plus a "postmaster" object (post) and some other new objects and
commands.  The "postmaster" objects communicate with each other through
a standard interprocess communication library such as MPI or PVM.

For more complete information on how to use PGENESIS, see Chapter 21,
"Large-Scale Simulation using Parallel GENESIS" in The Book of GENESIS
(http://genesis-sim.org/GENESIS/iBoG/index.htm). For a tutorial on
converting large network simulations to PGENESIS, see
http://genesis-sim.org/GENESIS/UGTD/Tutorials/genprog/par-net-tut.html.

INSTALLING PGENESIS
-------------------

1. Download, compile, and install serial GENESIS version 2.4.

   If you have not obtained GENESIS already, download
   genesis-pgenesis-2.4-05-2019.tar.gz from the GENESIS web site, and
   pick a directory where it will be installed. This is a gzipped tar
   file which can be extracted with:

	tar xvzf genesis-pgenesis-2.4-05-2019.tar.gz

   This will produce a directory genesis-2.4/genesis and the one
   genesis-2.4/pgenesis, which should be the same as the one with this
   file.  Follow the instructions in genesis-2.4/genesis/src/README.txt.
   (Assuming that you have installed 'genesis' in the directory above, see
    ../genesis/src/README.txt or README.html)
    
   Make sure that after doing a "make" or "make nxall" you also do a
   "make install" or "make nxinstall".

2. Prepare to install PGENESIS

  Now that GENESIS and PGENESIS are distributed in a combined release,
  all that remains is to study the "README" files in the genesis and
  pgenesis directories.

  The file ../genesis/src/README.html describes the use of the
  optional Boehm-Demers-Weiser conservative garbage collector. Its use
  with GENESIS and PGENESIS has allowed simulations of over one
  million cells, with little increase in memory usage with model size.
  However, it shouldn't be necessary for most users unless there are
  memory issues with very large models having greater than 10,000
  cells. It is enabled with the GCMALLOC definitions in the Makefile.
  Most GENESIS and PGENESIS users will want to leave these definitions
  commented out. You can read the instructions relating to flags for
  the garbage collector in ../genesis/src/README-GC.txt.

3. Ensure either MPI or PVM is installed.

   Most clusters or parallel systems already have an installed MPI
   implementation, so our recommendation is to use that.  (You can
   check if MPI is already there by typing "man MPI", "which mpirun",
   or looking in typical locations where community software is
   installed, e.g., /usr/lib64/, /usr/local, /usr/local/packages/,
   /usr/share/, /opt, etc.)  Any MPI implementation that supports MPI
   version 1 functionality should work. In particular the MPICH and
   MPICH2 implementations are known to be compatible. In the past
   openmpi has been used successfully, although there have been
   problems with some recent Linux releases.  If MPI does not appear
   to be installed, ask your system administrator to install a version
   appropriate for your system, or install one of these and the development
   libraries with your package manager.

   For recent Linux versions, MPICH will be the easiest MPI
   implementation to use. Once you have insured that you have the MPI
   executables and libraries, you will need to set the PATH
   environment to the place where MPI keeps its executables, such as
   mpirun.  For example:

   bashrc:  PATH=$PATH:/usr/lib64/mpich/bin
   csh or .tcsh:  set path = ($path  /usr/lib64/mpich/bin)

   It may also be necessary to set the LD_LIBRARY_PATH environment variable
   to point to the MPI libraries, for example:

       bash: export LD_LIBRARY_PATH=/usr/lib64/mpich/lib
       csh or tcsh: setenv LD_LIBRARY_PATH /usr/lib64/mpich/lib

   If, instead of MPI, you want to use an existing PVM installation,
   you should have version 3.3.4 or later.  If pvm is already on your
   PATH you can find out the version number with:

	echo version | pvm

   Make sure that your environment includes the proper settings for $PVM_ROOT
   and $PVM_ARCH, and that the PVM object libraries are in
   $PVM_ROOT/lib/$PVM_ARCH.

   If you want to install PVM, you can get the latest version, as well as
   more information about PVM from the PVM home page:

	http://www.epm.ornl.gov/pvm/pvm_home.html.

   You do not need the root password to install PVM.  After untarring the
   distribution, you will find installation instructions in pvm3/Readme.  A
   PVM manual is is also available from the PVM home page.  A PVM distribution
   and manual is also available from the GENESIS ftp site, but it is not
   guaranteed to be the most recent version. Most modern supercomputers
   are more likely to use MPI.

4. Modify the Makefile

   The Makefile.dist file in the pgenesis directory should be copied
   to Makefile, which should then be edited for your platform type as
   described by the comments in the Makefile.  Note that this is NOT
   pgenesis/src/Makefile, which should be left as is.  For PVM, you
   must set PVM_ROOT and PVM_ARCH in the Makefile, unless you have set
   them in your .cshrc as described in the PVM manual.  The default
   setting for PVM_ARCH should be correct in almost all cases.  You
   will have to uncomment one of the machine/compiler dependent
   options for CC, CPP, CFLAGS, LIBS, SYSLIBS, etc. For an initial
   installation attempt, we recommend that you leave the GCMALLOC
   definitions commented out, unless you plan to run very large
   network models. The GENESIS and INST_DIR macros should
   be set to, respectively, the full pathnames of the serial GENESIS
   installation and the directory in which parallel GENESIS should be
   installed (usually the PGENESIS root directory).  The PARSRC_DIR
   should be set to the full pathname of pgenesis. Finally, uncomment
   the EDITED=yes assignment; the Makefile will not function until this
   is done.

5. Compile the parallel libraries and executables

   There are several Makefile targets available for compilation:

     'make nxinstall'      - to compile and install pgenesis without Xodus
     'make install'        - to compile and install pgenesis with Xodus
     'make extended'       - to install extended pgenesis with Xodus
     'make nxextended'     - to install extended pgenesis without Xodus
     'make env'            - to show some of the configuration settings
     'make clean'          - to remove this machine type's object files
     'make spotless'       - to remove all machine types' object files

   Normally, you will use the 'make install' command, which compiles and
   installs PGENESIS with Xodus.  Once the Makefile has been customized,
   execute:

     make install

   If you do not use Xodus, first make and install nxgenesis for
   serial GENESIS, and then execute "make nxinstall". If you wish to
   install both pgenesis and nxpgenesis, follow this by making and
   installing serial genesis.  Then make and install ("make install")
   pgenesis. This order insures that the proper serial libraries are
   linked with the parallel version.

   If there are errors that are not ignored, then redo this command
   piping the output into a file, and examine it for the source of errors.

     make install > & ! make.out &
     (or for the Bourne shell: make install > make.out 2>&1 & )

   Note that, unlike the case with serial GENESIS, "make install" or "make
   nxinstall" is used to both compile and install PGENESIS.

   The compile process can be viewed by typing:

                tail -f make.out        [ hit ^C to exit the tail ]

   Note that this is done in the pgenesis directory, NOT pgenesis/src.
   Also note that, unlike the case with serial GENESIS, 'make install'
   or 'make nxinstall' is used to both compile and install PGENESIS.

   This will create a shell script bin/pgenesis, which can be executed
   in order to start MPI or PVM and run the proper pgenesis binary
   file. The pgenesis binary is created in bin/<arch>, where <arch> is
   one of the architectures supported by PVM (e.g. Linux, Alpha, SGI5,
   SUN4, etc.) It will also create a startup/.psimrc file that should be
   copied to your home directory.

If you find that you need to make changes in the Makefile that are not
described in the Makefile.dist comments, or need to make other corrections
in order to install and run pgenesis, please report them through the
genesis-sim-users mailing list.

TESTING PGENESIS
----------------

After copying genesis/.simrc and pgenesis/startup/.psimrc
(or the nx versions) to your home directory, set your path to include
pgenesis/bin, in order to be able to execute the pgenesis script from any
directory.  For example, if the Makefile setting for PARSRC_DIR is
/usr/local/genesis-2.4/pgenesis and your shell is tcsh or csh, use:

       set path = ($path /usr/local/genesis-2.4/pgenesis/bin)

If you are using bash as your shell, use

       PATH=$PATH:/usr/local/genesis-2.4/pgenesis/bin

You can do a simple test of PGENESIS by running the hello.g simulation
script in the pgenesis Scripts directory.  After changing to this
directory, start it with "pgenesis -nodes 3 hello.g", or "pgenesis
-nox -nodes 3 hello.g" to use nxpgenesis. If you are using PVM, the
command is "pgenesis demo.g". 

If this runs with no errors, you can try a more complex network
simulation that uses graphics by running the demo.g simulation script
in the pgenesis/Scripts/orient1 directory.  After changing to this
directory, start it with "pgenesis -nodes 3 demo.g" with MPI or
"pgenesis demo.g" with PVM.

This is a 3-way parallel decomposition of the genesis orient_tut
simulation over 3 nodes.  The default is to create these nodes on a
single processor.  (Of course, to achieve any advantage of using
PGENESIS, you will want to use networked workstations or a multiple
processor machine. Details for doing this are given in the
documentation.)

The display will be somewhat like the serial GENESIS orient_tut simulation,
and clicking on "sweep_horiz" should produce a similar pattern of activity
and plots.  A log file is created in o.out, so you will need to have write
permission in the directory that contains this simulation.

RUNNING PGENESIS
-----------------

Each user should be make sure that the PATH is set to include
pgenesis/bin, where pgenesis is the root of the PGENESIS installation,
i.e., the INST_DIR variable in the top level Makefile.  If the script files
you wish to use are not in your home directory, the examples directory, or
the directory in which you run parallel GENESIS, you will need to add their
directory path to the SIMPATH variable in the .simrc file you use.  By
default this is .psimrc for PGENESIS worker nodes.

You can run PGENESIS by typing "pgenesis scriptfile".

There are several available options, which may be listed by
typing "pgenesis -help".  The most commonly used of these are:
        -nodes <num>  = specifies number of nodes to use (with MPI)
	-config <file>      where <file> is a file containing a list
			      of machines to be used (1 per line)
        -nox		    run without Xodus
	-debug tty	    create a window for each pgenesis node

RUNNING PGENESIS ON MULTIPROCESSOR MACHINES
-------------------------------------------

PGENESIS is increasingly being used on supercomputer clusters, that have
multiple processors, each having multiple cores. The processors are grouped
into "nodes". Here we call this a "supercomputer node", to distinguish it
from a "PGENESIS node", which is an MPI process. If suffient computer cores
are available, each PGENESIS node in a simulation should be assigned to a
separate core.

The installation of the garbage collector (Section 2) may be
beneficial if running very large simulations or if memory resources are
becoming an issue.

This version of PGENESIS is now available for running large-scale
network simulations on supercomputers at the Neuroscience Gateway
Portal (NSG). For further information, see the
https://www.nsgportal.org/ web page. When running PGENESIS on machines
such as these, it may be necessary to use a resource manager such as
the Simple Linux Utility for Resource Management (SLURM) batch
environment. This allows you to allocate the number of supercomputer
nodes to be used, as well as the number of tasks (MPI processes or
PGENESIS nodes) to be assigned to each supercomputer node. For a
simulation that uses 26 PGENESIS nodes, on a machine that has only 24
cores per supercomputer nodes, it could be split into 13 cores on each
of two supercomputer nodes with SLURM commands:

    #SBATCH --nodes=2
    #SBATCH --ntasks-per-node=13

In some installations, the resource manage will take care of the assigment
of cores to nodes, and it will be sufficent to use a command like:

    genesis -nox -batch -notty -nodes 26 par-ACnet23.g

In other cases, you may need to pass this information directly to
mpirun (or the ibrun wrapper) instead of the C shell script
pgenesis/bin/pgenesis. On such a machine, you will likely be using
nxpgenesis compiled without the overhead of the XODUS libraries, or
other libraries that are not needed in this environment. You can use
a command such as:

    mpirun -v --ppn 13 $PGENESIS/bin/Linux/nxpgenesis \
       -altsimrc $PGENESIS/startup/.psimrc -notty -batch par-ACnet23.g

Here "$PGENESIS" is the full path to genesis-2.4/pgenesis and the option
"--ppn" gives the number of processes per supercomputer node. With ibrun,
the option is "--npernode".


FURTHER DOCUMENTATION
---------------------

For any use of PGENESIS beyond the simple test described below, see the
documentation in the pgenesis/Hyperdoc directory.  You can access the HTML
documentation by pointing your browser at Hyperdoc/index.html. There is
a very detailed descrip.gtion of the use of PGENESIS in the chapter:

"Large-Scale Simulation using Parallel GENESIS", Nigel H. Goddard and
Greg Hood, Chapter 21 in Bower, J.M., and Beeman, D. (1998). The Book
of GENESIS: Exploring Realistic Neural Models with the GEneral NEural
SImulation System (2nd Ed.). Springer-Verlag, NY.
(Free internet edition at http://genesis-sim.org/GENESIS/bog/bog.html)

The documentation also describes the use of PGENESIS on a network of
workstations.  Please report any inaccuracies to the mailing list.

DEBUGGING PGENESIS SCRIPTS
---------------------------

Normally the master node sends its output to your screen and
the worker nodes send their output to the file /tmp/pvml.????
on the machine they are running on, where ???? is your uid on
that machine.  Many script bugs can be tracked down by examining
those /tmp/pvml.???? files.  Note that PVM appends to these
files on every run, until PVM is halted and restarted.  Also
note that if a worker node exits without executing "paroff",
some of its output to stdout/stderr which is buffered may
not appear in the /tmp/pvml.???? file.

Output from the workers can be redirected to a file by giving
the paron command the -output <filename> option.  <filename>
can be /dev/tty, in which case the worker output appears on
your screen mixed with the master's output.

If you are running X-Windows, you can have each worker create
its own window and send its output there by using the "-debug tty"
flag when invoking PGENESIS.  This requires that your DISPLAY
environment variable is set to "hostname:0" when you invoke
the pgenesis script.

By default PGENESIS starts the workers using -silent 3 which
minimizes the messages printed by the workers (e.g., the banner).
To see more worker status messages, give the paron command
the -silent 0 option.

Common errors include:

	0. User impatience leads to interruption of PGENESIS
	   before it has completed the initialization of the workers.
	   The timeout is set to 120 seconds, for maximum information
	   about the problem, let PGENESIS timeout if it fails
	   to initialize workers properly (i.e., wait!).

	1. PVM can't find the worker executable.  This is specified
	   with the -executable <filename> option of the paron
	   command, the default is "pnxgenesis".  This executable,
	   or a link to it, should be in ~/pvm3/bin/$PVM_ARCH.
	   A symptom of this problem is that PGENESIS reports it
	   failed to spawn workers and the /tmp/pvml.??? files are
	   empty apart from the header.

	2. PVM aborts worker startup due to I/O in your .cshrc
	   Symptoms of this are that PGENESIS reports workers
	   failed to spawn and the /tmp/pvml.??? files contain
	   error messages about TTY or other strange things, and
	   no GENESIS startup banner.  Modify your .cshrc as
	   described in pvm-cshrc.stub.

	3. The workers can't find the script containing the paron
	   command.  Symptoms of this are that PGENESIS reports
	   workers failed to spawn and the /tmp/pvml.??? files
	   show the GENESIS prompt with no attempt to execute
	   paron, or the startup banners show that the script file
	   could not be found.  Edit your ~/.psimrc and ~/.nxpsimrc
	   so that SIMPATH lists the directories containing the
	   scripts.

	4. Incorrect use of barriers.  Symptoms are that the master
	   or some worker(s) hang and eventually timeout.  Examine
	   your use of barriers.  Note that some commands contain
	   implicit barriers, it is possible there is a bug in
	   the source.

For more information on the finer points of using PVM, refer
to the PVM user guide.


USER-EXTENDED PGENESIS
-----------------------

This section is intended for GENESIS users who have written or are using
custom GENESIS libraries written in C that need to get compiled and linked
into the GENESIS executable.  For PGENESIS libraries are constructed
just as for serial GENESIS, each in its own subdirectory in a canonical
form.  See the GENESIS documentation for descriptions of this form.  In
this section we describe how to link these libraries with PGENESIS
components to create an extended PGENESIS.

Assuming your libraries are subdirectories of a root directory I'll call
MYPGEN, these are the steps you follow to create and run extended PGENESIS.

   A. Copy pgenesis/lib/Usermake to MYPGEN/Makefile.  Modify the Makefile for
      your platform type as described in the Makefile.  You must set PVM_ROOT
      and PVM_ARCH in the Makefile, unless you have set them in your .cshrc as
      described in the PVM manual.  The default setting for PVM_ARCH should be
      correct in almost all cases.  You will have to uncomment one of the
      machine/compiler dependent options for CC, CPP, CFLAGS, LIBS, SYSLIBS,
      etc.  The GENESIS and INST_DIR macros should be set to, respectively,
      the full pathnames of the serial GENESIS installation and the directory
      in which parallel GENESIS should be installed (usually the PGENESIS root
      directory).

   B. Now add the names of your libraries and their subdirectories to USERLIB
      and USERDIR and set the installation root for your extended PGENESIS
      in USERINST.  Execute

	'make nxextended'	to make extended PGENESIS without Xodus
	'make extended'		to make extended PGENESIS with Xodus.

   C. This should create USERINST/bin and USERINST/lib containing various
      files.  The only one you need to be aware of is USERINST/bin/pgenesis.
      To run your extended PGENESIS, set your path to include USERINST/bin
      early.  Then invoke extended PGENESIS with

	'pgenesis'

      just as you would for vanilla PGENESIS.


SYSTEM-SPECIFIC IDIOSYNCRACIES
------------------------------

When using PVM-based PGENESIS on various older SMPs:

   On symmetric multiprocessors (e.g, Solaris multiprocessor, SGI Challenge,
   DEC AXP/8k series) you may need to increase the number of shared memory
   segments and perhaps other parameters of the shared memory system to
   run PVM effectively.  For example, on Solaris multiprocessor, you need
   to add the following to /etc/system:

     set shmsys:shminfo_shmseg = 100
     set shmsys:shminfo_shmmni = 100

   On ALPHA and HP SMPs, we have found that PVM does not always release
   the shared memory segments and semaphores it uses.  Unreleased items
   can use up all available shared memory/semaphore resource.  The script
   pgenesis/bin/ipclean attempts to release these resources.  You can use
   the command "ipcs" to see the status of the shared memory system.  In
   extreme cases a reboot is necessary to clean it out.

When using PVM-based PGENESIS on SGI/Cray Origin 2000:

   There is a problem with PVM 3.4.beta3, PVM 3.3.11, and probably earlier
   versions, in which the pvm_trecv call consumes processor time while it
   is waiting.  The effect that this has is to cause all PGENESIS processes
   on the Origin 2000 to continuously consume cycles, even if they are waiting
   for messages to arrive from elsewhere.  So, if you have more processes than
   processors, this will cause a significant deterioration in performance.

When using PVM-based PGENESIS on DEC 8400:

   PVM appears to have problems in occasionally generating unkillable
   processes.  Machine reboot has been the only recourse at times.

On HPUX SMPs:

   On HP-UX B.10.20 on an HP 9000/879 SMP, we found that serial GENESIS
   must be compiled with bison/flex:

     YACC = bison -y
     PARSER = bison
     LEX = flex -l
     LEXLIB = -lfl -lPW
     LIBS = $(LEXLIB) -lm
     XINCLUDE=/usr/include/X11R6
     XLIB = /usr/lib/X11R6


On Blue Gene/L (BGL) machines:

   You may have to download, configure, and make flex-2.5.4 (or later).
   The LEXLIB line should be modified to point to where it resides.

   The "NETCDFOBJ = $(GENESIS)/lib/netcdflib.o \" and following line
   will probably have to be commented out since we have had trouble
   getting the netcdf package to build on BGL.

   Also remember to uncomment the "USE_MPI = 1" and following lines
   since PGENESIS needs to use MPI on BGL.

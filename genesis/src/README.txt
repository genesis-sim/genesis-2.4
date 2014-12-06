GENESIS Version 2.4 README
Building and Installing GENESIS
Last edited: $Date: 2014/11/10 22:01:19 MST $

CONTENTS

    1  GETTING STARTED
    2  INTRODUCTION
    3  UNPACKING THE DISTRIBUTION
    4  PREREQUISITES FOR BUILDING THE DISTRIBUTION
        4.1  X11 Development Libraries
        4.2  yacc and lex
        4.3  Required Makefile Modifications -- "the old way"
        4.4  Custom Makefile Modifications
        4.5  Compiling
        4.6  Installation
        4.7 Specifying an Alternate Installation Directory
    5  RUNNING GENESIS
        5.1  Testing the Distribution
        5.2  The GENESIS Neural Modeling Tutorials
    6  KNOWN PLATFORMS
    7  PROBLEMS COMPILING GENESIS
        7.1 Compiling with Mac OS X
    8  PROBLEMS RUNNING GENESIS
        8.1  Floating Point Variation

1  GETTING STARTED

    GENESIS 2.4 features a new autoconf-based build system, meaning
    that now a simple "./configure" command will create the Makefile
    that is appropriate for your system. The old "Makefile.dist" and
    system-dependent Makefiles are still available to build GENESIS
    the old way (See Sec. 4.3). This may be necessary in case of
    problems with operating systems that are not supported by autoconf.

    Pick a place to unpack the distribution, e.g. "/usr/local" or "/opt".
    If you want a personal installation, rather than system-wide, create
    a place within your home directory.

    QUICK START

    0) cd /usr/local
    1) tar xzf genesis-2.4-src.tar.gz
    2) cd genesis-2.4/genesis/src
    3) ./configure --prefix=/usr/local/genesis-2.4/genesis
    4) make >& make.out
    5) make install >& install.out
    6) Add INSTALLDIR to your PATH (See Sec. 4.6)
    7) genesis

    If you'd rather make a version not requiring X11, substitute these steps:

    4) make nxgenesis >& nxmake.out
    5) make nxinstall >& nxinstall.out
    6) Add INSTALLDIR to your PATH (See Sec. 4.6)
    7) nxgenesis

    If it didn't work, you'll have to read on ...
    
    In particular, see Section 4.5 on directing error messages to a
    file, and Section 7. Mac users, be sure to read Section 7.1.

    Some Further Details:

    The GNU autconf utility assumes a default installation directory
    (INSTALLDIR) of /usr/local/genesis if a simple "./configure" is used.

    Whether or not you install the package in /usr/local, or in your home
    directory, it is best to have "genesis" lie below "genesis-2.4".
    This will make it possible to install parallel GENESIS (PGENESIS) as
    a sibling directory "genesis-2.4/pgenesis". This is required for the
    installation of PGENESIS.

    The recommended way to create the Makefile is with:

       ./configure --prefix=/usr/local/genesis-2.4/genesis

    Alternatively, to put it a "bin" directory of your own:

       ./configure --prefix=$HOME/bin/genesis-2.4/genesis

2  INTRODUCTION

    This file contains information necessary to successfully build, install
    and run GENESIS.  Please read the entire document carefully.

    GENESIS is known to compile on a wide range of Unix-based operating systems
    including SunOs and Sun Solaris, FreeBSD, OpenBSD, SGI Irix, HPUX, IBM SP2
    AIX, a gamut of Linuxes, Mac OSX and Windows with Cygwin.  We welcome
    feedback on experiences with these and other platforms. Most recently,
    version 2.4 has been successfully tested under Linux, Mac OS, and Cygwin.


3  UNPACKING AND INSTALLING THE DISTRIBUTION

    The GENESIS distribution may be installed in whatever directory is most
    convenient.  The distributed compressed tar file unpacks to produce the
    directory named "genesis-2.4" and its subdirectories. This is a slight
    departure from early releases which always unpacked to a directory
    named genesis. The new structure accommodates parallel genesis and
    facilitates users running multiple versions.

    In the instructions that follow, we refer to the genesis directory as
    the "installation directory".  For example, if you unpack the distribution
    to /usr/local (as is often done to make GENESIS accessible to multiple
    users on a single computer), the installation directory will be
    /usr/local/genesis-2.4/genesis. Directory and file names are given relative
    to the installation directory.  Thus, the file you are reading is
    src/README.

4  PREREQUISITES FOR BUILDING THE DISTRIBUTION

    This section describes some prerequisites to building as well as the
    actual build process.

4.1  X11 Development Libraries

    X11 development files are needed if you intend to compile the graphical
    version of GENESIS. The location of the X11 files varies among
    different operating systems so there's no standard way to determine if
    they are installed. You can try 'locate Xlib.h' at a command prompt or
    look in src/Makefile.dist for comments relevant to your operating
    system.

    You must have the X Window System X.org (or X11R5, X11R6) in order
    to successfully compile and run graphical GENESIS. The runtime
    files associated with this software package will be installed on
    most computers, but the development files may not be. If you
    have determined that these files are not installed on your system,
    install them from the original installation media; look for
    some variation on 'X11 development'. Check the web site for your
    OS or use your package manager to download them.

    In general, it is best to install most of the C software development
    libraries that are available for your system.
	
4.2  yacc and lex

    GENESIS uses the yacc and lex programs to generate a parser for SLI, the
    script language of GENESIS. If GENESIS does not compile properly due to
    problems with the code generated by yacc or lex, or your system does not
    have yacc (or bison) or lex (or flex), your best bet is to obtain
    the GNU versions of these programs (bison and flex).

4.3  Required Makefile Modifications -- "The old way"

    If for some reason you are not able to create a Makefile by the
    method described in Section 1, you may create it by using
    Makefile.dist as a template. Make a copy of src/Makefile.dist to
    src/Makefile and edit src/Makefile to uncomment the appropriate
    Makefile variables for your system.  Do NOT edit the files called
    'Makefile.machine-name' or rename them to Makefile.  It is
    critical that parameters be set correctly for a clean compilation
    and installation to occur.

    Instructions about what must be modified in src/Makefile are included at
    the beginning of the file.

4.4  Custom Makefile Modifications

    If for some reason your X11 include and library files are located in
    non-standard directories (e.g. not in /usr/include and /usr/lib) you
    can set the following parameters in src/Makefile:

        XLIB=        <dir>
                Specify the directory in which the X11 libraries
                reside.

        XINCLUDE=    <dir>
                Specify the directory in which the X11 include
                hierarchy resides.  Note that the directory
                specified must have a subdirectory named "X11"
                in which the include files reside.

    For example, if you are using a Sun Microsystems workstation and have
    fully installed the OpenWindows package (which is the vendor-supplied
    implementation of the X Window System on this platform), you can use
    the following parameter settings:

        XLIB =        /usr/openwin/lib
        XINCLUDE =    /usr/openwin/include

    to compile using openwin includes and libraries.  (Note the comments in
    src/Makefile about using openwin.)

4.5  Compiling

    From the src directory type:
        (if you are using MacOS X/Darwin may have to: limit stacksize 4096)
        make clean
        make > make.out 2>&1 &
        (or for csh: make > & ! make.out &)

    This will take a minute or two on modern architectures. The
    compile process can be viewed by typing:

        tail -f make.out     [ hit ^C to exit the tail ]

    It should end with something like:

        Full GENESIS Compiled -- All Done

    Unfortunately, the GENESIS compilation may display a message
    indicating a successful build even when it's clearly not. It is
    best to check the last 30-50 lines of make.out to be sure no
    errors have been reported. If you suspect an error, please read
    the 'PROBLEMS COMPILING GENESIS' section below.

    If you want to build a version of GENESIS to be run without XODUS
    (nxgenesis), type "make nxall".  To compile a minimum version of
    GENESIS with none of the object libraries included (mingenesis), type
    "make minall".
    
4.6  Installation
    
    After a successful compile, the distribution can be installed by
    typing:

        make install > install.out 2>&1
        (or for csh: make install > & ! install.out &)
        (or "make nxinstall" for nxgenesis)
        (or "make mininstall" for mingenesis)

    This will copy all important files and binaries to the installation
    directory Check the output in install.out and report any errors
    during installation.

    The installation will fail if a 'test' executable cannot be found.
    This program is built in to most modern shells, but may also be found in
    some directory on your system. The most likely path to a test executable
    is /usr/bin. The best solution is to add a directory to your execution
    path which contains a 'test' executable. See the instructions below
    for adding a directory to your execution path.

    After a successful installation, type:
        make clean

    to remove temporary files which were created during the compilation.
    For a more thorough cleaning, use 'make cleandist', but note that this will
    move Makefile to Makefile.bak.

    Next, copy the .simrc file to your home directory.  .simrc files
    from previous versions of GENESIS should be replaced by the one in
    the installation directory.  If you are using GENESIS without XODUS
    (nxgenesis) or a minimal version of GENESIS (mingenesis), copy
    .nxsimrc or .minsimrc to .simrc in your home directory or use the
    '-altsimrc' option when starting GENESIS.

    The .simrc file created after the "make install" step should have the
    correct path names to scripts needed by GENESIS.  If you should move
    the genesis directory tree from where it was originally installed
    to someplace else, you will have to edit this file.  Note that it
    requires full pathnames, e.g "/home/joeuser/genesis-2.4/genesis/startup",
    NOT "~joeuser/genesis-2.4/genesis/startup" or "`pwd`/genesis/startup".

    You may move, copy, or create links to the GENESIS executables so that
    they appear in some directory which is already on your shell's command
    search path, but be sure that GENESIS program names do not conflict
    with existing programs. Alternatively, you can add the installation
    directory to your path.  For example, if the installation directory is
    /usr/local/genesis-2.4/genesis and your shell is tcsh or csh, use:

        set path=($path /usr/local/genesis-2.4/genesis)

    If you are using sh or bash, use

        PATH=$PATH:/usr/local/genesis-2.4/genesis
        export PATH

    These lines should be added to your .cshrc or .tcshrc file (for
    csh/tcsh) or your .bashrc (for bash) in your home directory.

4.7 Specifying an Alternate Installation Directory

    Whether you created the Makefile by using "./configure" or by editing a
    copy of Makefile.dist, you may want to install the compiled version of
    GENESIS and its libraries in a different place than was specified in
    the Makefile.  This can be changed by editing the Makefile line
    "INSTALLDIR ?= " to give the full path to where your want it to be.

    Another way is to invoke "make install" with the value of INSTALLDIR,

    as in:

        make install INSTALLDIR=/usr/local/genesis-2.4/genesis
        
5  RUNNING GENESIS

    You must have an X11 server running in order to run graphical GENESIS. If
    the installation steps were successful, you should just be able to
    type genesis at the command prompt, or nxgenesis for a non-graphical
    version.

5.1  Testing the Distribution

    If you wish to test specifics, cd to Scripts/{neuron, squid, neurokit,
    or orient_tut} and follow the directions in the README files found in those
    directories.  Make sure that you've added the installation
    directory to your path and copied .simrc to your home directory, as
    described above, before running any of the demos.

    Also, see the discussion on floating point computation below.

5.2  The GENESIS Neural Modeling Tutorials

    The genesis/Tutorials directory in the GENESIS distribution is provided as
    a place holder for supplemental hypertext GENESIS modeling tutorials and
    documentation, including the GENESIS Neural Modeling Tutorials package.
    These are an evolving set of HTML tutorials intended to teach the process
    of constructing biologically realistic neural models with the GENESIS
    simulator. The latest version of this package is offered as a separate
    download from the GENESIS web site. The downloaded files from
    http://genesis-sim.org/GENESIS/UGTD.html should be unpacked in the
    genesis/Tutorials directory in order to augment the files that are
    provided there.  The "bare bones" version that is provided with the
    GENESIS distribution includes a beginner's guide to UNIX/Linux commands
    that may be useful for those who are new to the command line environment
    used by GENESIS.

6  KNOWN PLATFORMS

    See the top-level README for a list of systems on which GENESIS is
    known to compile and run.  Note that the list is for very specific
    configurations of software. In general, you should expect no
    problems on recent versions of Limux and Mac OSX > 10.8.  While
    GENESIS has been successfully compiled and run on a wide variety
    of systems, different versions of an operating system, compiler,
    linker, or other auxiliary program may cause problems. Read on for
    some known problems and solutions.


7  PROBLEMS COMPILING GENESIS

    GENESIS will not compile on versions of X11 before X11R5.  All of our code
    has been built using X11 from MIT.  In testing, GENESIS 2.x compiled and
    ran under the vendor supplied X11 products.  Should you encounter problems
    with a vendor's X11 product, we suggest you obtain the MIT distribution and
    install the libraries (it is free).

    Errors will appear differently based on the vendor and version of the
    compiler and linker. Usually compile and link errors will be prefixed by
    either 'Error' or 'ERROR', though some systems may use some arcane system
    of numbers. In any case, there will be some consistent string to search for
    to determine the root cause of compilation and linkage errors.

    It's a fairly safe bet that if there are errors somewhere in the compilation
    process, some error will appear at the end of the "make.out" file. Often,
    the root cause of the error at the end of the file occurs somewhat earlier
    in the process. For this reason, you should start searching for errors
    from the beginning of the make.out file. Fixing the earliest occurring error
    may resolve later errors. If the only error occurs at the end of make.out,
    the issue will most likely be a missing library and you may have to talk to
    your sysadmin in order to get the correct library installed.

    Most problems in compiling GENESIS arise from these sources:

    --> You have not properly set the various options in the Makefile.
        Make sure you have removed the comment marker (#) from all variables
        relevant to your operating system. If you've used the Makefile before,
        make sure there are no variables uncommented under other operating
        systems. Also, be sure there is not a different section which more
        closely resembles the system you are using; there are separate sections
        for Linux on a Power PC and Linux on an x86 for example.

    --> Some necessary library or directory of include files (e.g. the X11
        libraries) has been installed in a non-standard place on your system,
        or were not installed.  For example, be sure that all the C and X11
        development libraries and tools have been installed.  If a file or
        library is not being found during the make, look through the make.out
        file to find out which library is not being found.  After you have
        determined where it actually is, or have installed it, make any needed
        changes to the path given in XLIB, XINCLUDE, or LIBS.  You may have to
        specify an additional library search path with the "-L" option, for
        example "LIBS= $(LEXLIB) -lm -L/usr/pubsw/lib"

    --> M4 errors. Some architectures may require modification of:
        src/diskio/interface/netcdf/netcdf-3.4/src/macros.make
        The file contains a flag (-B10000) to the m4 processor which most
        modern m4 implementations ignore. The m4 processor distributed with
        FreeBSD is known to reject this flag, as well as some Solaris versions.
        Comment out or eliminate the flag (7 characters), or install GNU m4
        and make sure it is first in your PATH.

    --> GCC 2.6.3 users should probably upgrade to a newer version. If
        __OPTIMIZE__ is defined when including stdlib.h, it will causes
        incompatible redefinition of some functions.

    --> Sun Openwin: you may need to add /usr/openwin/lib to the
        LD_LIBRARY_PATH environment variable when compiling and/or
        running GENESIS.  If, when running GENESIS, you get a complaint
        that libX11.so.4.2 can't be found (or something similar),
        LD_LIBRARY_PATH is likely to be the problem.

    --> Sun Solaris with the GCC compiler: There have been reports
        that GCC versions 3.x produce errors when the code_g program
        is compiled using optimization level O2.  If you encounter this
        problem, change the CFLAGS line to read
        'CFLAGS=-O -DBIGENDIAN'.

    --> The SGI version of lex allocates a relatively small
        input buffer of 200 characters.  If this buffer overflows, GENESIS
        will exit.  This can happen with very long arguments to commands.
        We have made changes elsewhere in the distribution to avoid this
        situation.  However, should you encounter this, you might consider
        using GNU flex instead of the SGI lex.

    --> Different versions of an operating system may require changes
in the GENESIS source code or Makefile options.  If you do not find a
fix on the GENESIS web site, please give us as much information as
possible about the error messages and your operating system in an
email to the GENESIS Users Mailing List at
https://lists.sourceforge.net/lists/listinfo/genesis-sim-users and use
the bug reporting features at:
http://sourceforge.net/tracker/?func=add&group_id=141069&atid=748364.

    --> SunOS 4.1 may have some peculiarities which prevent SPRNG and NETCDF
        from compiling. You may need to comment out these sections in your
        Makefile.

    --> When compiling under Cygwin, all source files should use Unix-like
        line terminators. This should be the default in the distribution.
        However, if you've edited some source file or are compiling your
        own objects into GENESIS, your source file(s) may have some Windows/DOS
        line terminators. This may manifest itself during compilation as
        "syntax error"s when 'make' runs the code_sys or code_g auxiliary
        programs. Use the dos2unix command to fix the offending file(s).

7.1 Compiling with Mac OS X

    The current GENESIS 2.4 version has been verified to compile correctly only
    with gcc-4.8 on OSX (where the default compiler is clang). gcc can be
    installed via homebrew:

      brew install https://raw.github.com/Homebrew/homebrew-versions/gcc48.rb

    You will need X11 in order to compile Xodus (otherwise, you can
    always make nxgenesis). You can get it from XQuartz:

      http://xquartz.macosforge.org/landing/

    Flex and Bison are required, and can be installed via

      brew install flex bison

    For most Mac users, the biggest problem will be installing the necessary X
    window system libraries, and locating where they are installed, in case
    "configure" cannot locate them. Future versions of Mac OS may provide X11
    without the need to install via Quartz.

    It is recommended that you first try "make nxgenesis" to see if there are
    any problems other than with the installation of XODUS.  XODUS makes use
    of the legacy Athena Widget set (Xaw) within the X.org libraries. When
    building GENESIS with XODUS, you may encounter compilation errors
    involving the failure to find header files for Xlib.h and Intrinsic.h,
    which are expected to be in /usr/X11/include/.

    The paths shown for XLIB and XINCLUDE in Makefile.dist may not be valid, but
    might not cause problems if the compiler is aware of the actual location of
    the needed X libraries and include files.

    If compilation fails because of errors in netcdf, see the earlier comments
    on changing the definitions for the netCDF file format, in order to
    disable it. Recent changes to the src/diskio library have likely solved
    this problem.

8  PROBLEMS RUNNING GENESIS

    Please see README.bindist in the top-level directory.

8.1  Floating Point Variation

    The default compiler options will differ for different compilers (say Intel
    versus GNU) and may differ for different versions of the same compiler;
    these default options will probably dictate how floating point operations
    are carried out on a given system.

    It may be possible to make different processors produce very similar
    results by using appropriate compiler options. For example, computations
    can be made more consistent among x86 systems by providing the
    '-mfpmath=387 -ffloat-store' flags to the gcc compiler. However, this is
    a least common denominator approach and may result in significant
    performance degradation. Compiler options which break ISO or IEEE floating
    point conformance will break GENESIS! In particular, the gcc option
    -funsafe-math-optimizations (implied by -ffast-math) is known to cause
    some GENESIS operations to fail.

Installing gcmalloc
-------------------

The Boehm-Demers-Weiser conservative garbage collector is used as a
garbage collecting replacement for C malloc. It allows memory to be
allocated as normal, but without explicitly deallocating memory that
is no longer useful. The collector automatically recycles memory when
it determines that it can no longer be otherwise accessed.

The installation of this library is optional and shouldn't be
necessary for most users unless there are memory issues with very
large models having greater than 10,000 cells. However, its use with
PGENESIS has allowed simulations of over one million cells, with
little increase in memory usage with model size. Its installation is
recommended when GENESIS 2.4 is used with PGENESIS 2.4 for running
large network models on supercomputer clusters. Most GENESIS
and PGENESIS users will want to leave the "GCMALLOC" definitions in
the Makefile commented out.

In order to avoid the need to install gcmalloc in the system libraries,
and to avoid conflict with any existing versions, it is to be installed in
the directory 'genesis/locallibs/gcmalloc' using the Makefile definitions:

 GCMALLOC_FLAG=-DGCMALLOC
 GCMALLOCLIB=-L$(INSTALLDIR)/locallibs/gcmalloc/lib -lgc
 GCMALLOCINCLUDE=-I$(INSTALLDIR)/locallibs/gcmalloc/include

The updated GENESIS 2.4 distribution provides empty directories in
genesis-2.4/genesis/locallibs for gcmalloc. In order to compile
GENESIS and PGENESIS with gcmalloc, you need to compile the
relocateable statically-linked library libgc.a and copy it to 
locallibs/gcmalloc/lib. This and the resulting include directory
should be copied to locallibs/gcmalloc.

The main web page for the BDW Garbage Collector is
http://www.hboehm.info/gc. This describes the library and gives links to
the development repository at https://github.com/ivmai/bdwgc.
This is where you can download the latest version.

The installation will require that you have C and C++ toolchains, git,
automake, autoconf, and libtool already installed.

To build a working version of the collector, you will need to do
something like the following:

(1) Pick a place to build the garbage collector. The resulting libgc.a
    and include directory will be copied to your GENESIS installation
    directory. In this example, the software is installed in the
    user's home directory '~/Software' and a GENESIS installation
    directory '/usr/local/genesis-2.4'.

(2) Download the necessary files:
    cd ~/Software
    git clone git://github.com/ivmai/libatomic_ops.git
    git clone git://github.com/ivmai/bdwgc.git

(3) You will now have directories libatomic_ops and bdwgc in
    ~/Software.  The bdwgc directory has useful information in
    README.md and README.QUICK, as well as the doc subdirectory.

(4) For installation, libatomic_ops needs to be a subdirectory of bdwgc:

    mv libatomic_ops bdwgc
    
(5) Next, build  bdwgc:

    cd bdwgc 
    autoreconf -vif
    automake --add-missing
    ./configure --prefix=$HOME/Software/gcmalloc --enable-static \
       --enable-gc-debug --enable-rdirect-malloc --disable-threads
    make
    make check
    make install

(6) if these are successful, then copy only libgc.a and the include
     directory to the genesis/loclalibs/gcmalloc cirectories:

    cp -p lib/libgc.a /usr/local/genesis-2.4/genesis/loclalibs/gcmalloc/lib
    cp -rp include  /usr/local/genesis-2.4/genesis/loclalibs/gcmalloc

At this point, you should be ready to compile genesis and pgenesis with
support for gcmalloc. For genesis or nxgenesis, follow the instructions
in genesis/SRC/README Section 4.3 for compiling "The old way" without
using 'configure'. Likewise, pgenesis/README describes how to compile
pgenesis or nxpgenesis.

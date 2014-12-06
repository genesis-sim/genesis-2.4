README file for GENESIS Documentation
--------------------------------------

The genesis/Doc directory contains the on-line help files (*.txt) to be
used with the GENESIS "help <topic>" command.  These files comprise the
plain text version of the GENESIS Reference Manual.  Note that the
extensions of these files has been changed to ".txt", from the ".doc"
extension used in previous GENESIS versions.  An older postscript version
of the manual (for GENESIS 2.2), formatted for printing, is available from
the GENESIS web site.

README.changes lists the documentation changes from
previous releases.  Changes.txt describes new GENESIS features.

The HTML hypertext documentation for GENESIS is in a separate directory,
genesis/Hyperdoc.  The main file is Manual.html, and there is a README
file giving further information.

When using the on-line help from within GENESIS, begin with CONTENTS and
Intro.  As most of these files are longer than a single screen, it is best
to pipe the output into "more".  For example,

	help CONTENTS | more
	help Intro | more

For information concerning a Users Guide to the construction of neural
simulations with GENESIS, please see the description of "The Book of
GENESIS" in Intro.txt and BoG.txt.

The genesis/Tutorials directory in the GENESIS distribution is
provided as a "place holder" for supplemental hypertext GENESIS
modeling tutorials and documentation, including the GENESIS Neural
Modeling Tutorials package.  These are an evolving set of HTML
tutorials intended to teach the process of constructing biologically
realistic neural models with the GENESIS simulator.  The latest
version of these tutorials, documentation and example scripts is
offered within the "Ultimate GENESIS Tutorial Distribution" as a
separate download from the GENESIS web site,
http://genesis-sim.org/GENESIS. After unpacking the downloaded file
(UGTD.tar.gz or UGTD.zip) the files may be placed in the
genesis/Tutorials directory in order to augment the files that are
provided there.

The "bare bones" version that is provided with the GENESIS distribution
includes a beginner's guide to UNIX/Linux commands that may be useful
for those who are new to the command line environment used by GENESIS,
and links to HTML documentation.

----------------------------------------------------------------------------

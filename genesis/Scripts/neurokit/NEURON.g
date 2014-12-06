//genesis
// This file is the same as Neurokit.g.  It is provided to maintain
// compatibility with the older name "NEURON".
echo " "
echo " "
echo " "
echo "     **************************************************************"
echo "     **                                                          **"
echo "     **                 NEURON SIMULATION UTILITY                **"
echo "     **                                                          **"
echo "     **  By Upinder S. Bhalla, Caltech, May 1990, Jan,Jul 1991   **"
echo "     **        Embellishments by Erik De Schutter, Dec 1990      **"
echo "     **                                                          **"
echo "     **************************************************************"
echo " "
echo " "
echo " "

/****************************************************************************/

/* Setting the SIMPATH to include the neurokit prototype directories.
**
** Serious users of neurokit should put the neurokit and
** neurokit/prototype directories in SIMPATH in their ~/.simrc files,
** so that they can run it from any directory. The line below should
** then be commented out
*/
setenv SIMPATH {getenv SIMPATH} ./prototypes


/* always include these default definitions! */
include defaults 

/* userprefs is for loading the preferred set of prototypes into
** the library and assigning new values to the defaults.
** A customised copy of userprefs.g usually lives in  the local
** directory where the simulation is going to be run from */
include userprefs 

/* create the neurokit objects and display the title bar */
include xall 


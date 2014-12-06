
The Scripts/tutorials directory contains the GENESIS scripts which are
listed in Appendix B of "The Book of GENESIS".  These scripts are used in
Part II of the book with tutorials that guide the user through the process
of constructing GENESIS simulations.  Tutorials 1-6 correspond to Chapters
11-16 of the first edition, and 12-17 of the second.  We have given brief
descriptions of the tutorials below, so that it will be possible (although
not recommended!) to use the scripts without the book.

----------------------------------------------------------------------

tutorial1 - "A Basic Tutorial Under GENESIS" guides the user through a brief
    session under Genesis, using the basic features of the simulator to
    create and run a simple simulation of a passive compartment.  Other
    tutorials build upon this to create simple and more complex cells and
    networks of neurons.  (Uses tutorial1.g)

tutorial2 - "Making a More Realistic Soma Compartment" introduces the user
    to more features of the GENESIS script language and modifies the
    previous simulation to construct a cell body which will be used in
    future tutorials.  (Uses tutorial2.g)

tutorial3 - "Adding Hodgkin-Huxley Channels to the Soma" discusses the types
    of voltage activated ionic channel elements which are available in
    GENESIS and makes use of a script (hhchan.g) from the neurokit
    prototypes library to add Hodgkin-Huxley sodium and potassium channels
    to the soma constructed in the previous tutorial.  The user is also
    introduced to some additional features of XODUS.  Uses
    tutorial3.g, hhchan.g)

tutorial4 - "Building a multi-compartmental neuron" adds a dendrite
    compartment with an excitatory synaptically activated channel.  This
    version uses the new randomspike, spikegen and synchan objects.  A
    randomspike object randomly stimulates the synchan with a SPIKE message,
    and the soma feeds a spikegen to provide feedback to the synchan with a
    second SPIKE message.  This connection may be toggled on and off.


tutorial5 - "Automating cell construction with the cell reader" uses the
    "readcell" function to build the same neuron from a data file.  This is
    the preferred method for the construction of complex cells and the
    exchange of cell models with other GENESIS users.  This tutorial also
    provides an introduction to the "neurokit" cell builder and its library
    of prototype cell components.  (Uses tutorial5.g, cell.p, protodefs.g,
    hhchan.g) 

tutorial6 - "Building a Cell with Neurokit" is a guide to the use of
    neurokit for constructing one's own cell models.  The emphasis is on
    customizing the 'userprefs' file, using the model of Tutorials 4 and 5
    as an example.  It also goes into more detail describing ways that
    neurokit can provide synaptic activation to a cell.  Although it may be
    used without having worked through the programming of Tutorials 1-5, it
    assumes some minimal familiarity with GENESIS and the model which is
    implemented in these tutorials.  Therefore, one should at least read
    through the description of the model neuron in these tutorials.
    It also assumes that one has worked with neuorokit, using the default
    "camit" model example to try out some of the features described in the
    "neurokit/README" file.

    If you wish to see the results which are obtained by constructing a
    "userprefs" file like the one given here, you may go ahead and run
    neurokit from this directory.  (Run "genesis" and then type "Neurokit"
    to the genesis prompt.)  When you click on "help", the
    "neuro_tut.text" file will be displayed.  Skip ahead to the section
    "Some experiments using neurokit" and try some of the experients with
    the cell which has been loaded into neurokit.  (Uses cell.p,
    userprefs.g, hhchan.g, neuro_tut.text, and Neurokit.  It also assumes
    that neurokit/prototypes has been included in the .simrc SIMPATH.)

hhchan_K.g - example script used in Chapter 13 to demonstrate the use
of an extended object for a Hodgkin-Huxley ``squid-like'' potassium
channel with extended SET and CREATE actions.

setcolors.g - a little script to change the colors of all exisiting widgets
to something a little less drab than the defaults.  I would welcome
suggestions for a nice set of defaults to go into genesis/Xdefaults.
To use it, load a simulation, "include setcolors.g", and type "colorize".
If you miss the colorful XODUS 1.0 widgets, type "upicolors".

New addition in GENESIS 2.2:

newtutorial3.g - This is a modification of tutorial3.g that uses tabchannels
rather than hh_channels for the voltage activated channels.  For serious
modeling, we recommend the use of the tabchannel, rather than the older
hh_channel object.  Not only are tabchannels faster, but they allow the
use of more accurate (and faster) implicit numerical integration methods.
The use of these methods, with the hsolve object, is *strongly* recommended
for multicompartment models with more than just a few compartments.
Further details are given in Chapters 19 and 20 of the Book of GENESIS
(second edition), and in the documentation for tabchannel and hsolve.

Dave Beeman - dbeeman@dogstar.colorado.edu


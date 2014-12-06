README file for Scripts/chemesis
================================

This directory contains tutorial scripts to teach how to develop
models of second messengers and calcium dynamics.  A step-by-step
explanation of the Calx.g scripts is provided in CalTut.txt.

mglu-ip3-chemesis.g is a chemesis script that builds the three
biochemical reactions leading from activation of the metabotropic
glutamate receptor, through production of inositol triphosphate. It
uses parameters in mglu-param.g, and graphs for illustration are
created in mglu-graphs.g.  The same reactions are also programmed for
xppaut, in the file mglu-ip3.ode.  This file has optional G_alpha and
IP3 degradation. mglu-ip3-kkit.txt contains instructions for
developing this model in kinetikit.

CALx.g are a series of scripts that build up calcium dynamics in a
step by step manner. The calcium mechanisms and the chemesis objects
are further described in Blackwell, 2005 (K. T. Blackwell, Modeling
Calcium Concentration and Biochemical Reactions, Brains, Minds and
Media, urn:nbn:de:0009-3-2245, available from
http://www.brains-minds-media.org). The CalTut.txt also explains these
in more detail.

Cal1-SI.g creates a single compartment with interaction between
calcium and buffer

Cal2-SI.g creates a two compartment model: dendrite and soma.  The
calcium - buffer reaction is set up in each compartment in a function
called cabuf, which implements the pools and reaction introduced in
cal1.g.  The function simplies the code, so it doesn't have to be
repeated once for each compartment. One additional object/element is
required to allow for diffusion between compartments.

Cal2difshell.g implements calcium the same calcium buffering and
diffusion using the genesis difshell object.

Cal3-SI.g creates the two compartment model of cal2.g, and then adds
ip3 induced calcium release into the soma.  Within cal3.g, two of the
release elements are created, for illustrative purposes.  The
remaining elements are created in a function that is called.

Cal4-SI.g adds a SERCA pump to the model in Cal3-SI.g in order to more
quickly re-establish calcium equilibrium.

(cal5.g and cal6.g used an obsolete set of chemesis objects to create
voltage dependent channels.  These have been replaced by Cal7.g and
Cal8.g and are no longer provided)

Cal7.g adds a voltage dependent calcium channel.  The output is
connected both to a calcium compartment and to an electrical
compartment, which in turn sends a voltage message to the calcium
channel.ts.

Cal7difshell.g implements the same model as Cal7.g using genesis objects.

Cal8.g uses the function cabufdiff.g, which expands cabufSI.g, to
create multiple sections in both the soma and dendrite, to improve the
diffusion.  It also shows how you can set up diffusion in two
dimensions.


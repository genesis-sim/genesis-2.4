/*
Section 18.7.3 of the BoG (Utility Functions for Synapses) describes and
gives a listing for the synapse_info function.

Usage:   synapse_info path_to_synchan
Example: synapse_info /network/cell[5]/apical1/Ex_channel

This is used to return a list of synaptic connections to the channel, their
sources, weights, and delays.  It uses some of the built-in GENESIS commands:

getsyncount [presynaptic-element] [postsynaptic-element]
getsynindex <presynptic-element> <postpostsynaptic-element> [-number n]
getsynsrc <postsynaptic-element> <index>
getsyndest  <presynptic-element> <n> [-index] //n is the no of spike message

*/

function synapse_info(path)
   str path,src
   int i
   float weight,delay
   floatformat %.3g
   for(i=0;i<{getsyncount {path}};i=i+1)
   src={getsynsrc {path} {i}}
   weight={getfield {path} synapse[{i}].weight}
   delay={getfield {path} synapse[{i}].delay }
   echo synapse[{i}] :\
        src = {src} weight ={weight} delay ={delay}
   end
end

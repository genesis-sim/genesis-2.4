//genesis - tools.g for CPG 2.0 version

// ===========================================
// links together two asymmetric compartments
// ===========================================
function link_compartment(comp1,comp2)
    addmsg   {comp1}   {comp2}  RAXIAL Ra previous_state
    addmsg   {comp2}   {comp1}  AXIAL  previous_state
end

// ===========================================
// links a channel which computes channel current
// (e.g. ChannelC2 or synchan) with a compartment
// ===========================================
function link_channel2(channel,compartment)
    addmsg   {channel}   {compartment} CHANNEL	Gk Ek 
    addmsg   {compartment}   {channel} VOLTAGE 	Vm 
end

/* Functions for getting and setting synchan weights given the source
   element and the number of the SPIKE message
*/

function getweight(src_element, spikemsg)
    str src_element, dest_element
    int spikemsg, synindex
    dest_element = {getsyndest {src_element} {spikemsg}}
    synindex = {getsyndest {src_element} {spikemsg} -index}
    return {getfield {dest_element} synapse[{synindex}].weight}
end

function setweight(src_element, spikemsg, weight)
    str src_element, dest_element
    int spikemsg, synindex
    float weight
    dest_element = {getsyndest {src_element} {spikemsg}}
    synindex = {getsyndest {src_element} {spikemsg} -index}
    setfield {dest_element} synapse[{synindex}].weight {weight}
end

// Not used in this simulation, but is available:
// ===========================================
// sets up data file output of either binary or ascii type
// ===========================================
function disk_output(path,type)
    str path,type
    if(type == "binary")
	create 		disk_out		{path}
    end
    if(type == "ascii")
	create 		asc_file		{path}
    end
    setfield	 {path}		flush			1 \
			leave_open		1
    useclock {path} 1
end

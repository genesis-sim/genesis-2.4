//genesis

// $Id: tools.g,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
// $Log: tools.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:34  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.2  1994/09/23 16:44:13  dhb
// Converted to 2.0 syntax
// ,
//
// Revision 1.1  1992/12/11  19:03:48  dhb
// Initial revision


//genesis

// ===========================================
//      BASIC SIMULATOR UTILITY FUNCTIONS
// ===========================================

// ===========================================
// links together two asymetric compartments
// ===========================================
function link_compartment(comp1, comp2)
    addmsg {comp1} {comp2} RAXIAL Ra previous_state
    addmsg {comp2} {comp1} AXIAL previous_state
end

// ===========================================
// links a channel which computes channel current
// (e.g. ChannelC2) with a compartment
// ===========================================
function link_channel2(channel, compartment)
    addmsg {channel} {compartment} CHANNEL Gk Ek
    addmsg {compartment} {channel} VOLTAGE Vm
end

// ===========================================
// links a channel which does not compute channel current
// (e.g. ChannelC) with a compartment
// ===========================================
function link_channel(channel, compartment)
    addmsg {channel} {compartment} CHANNEL Gk Ek
end

// ===========================================
// sets up data file output of either binary or ascii type
// ===========================================
function disk_output(path, type)
    str path, type
    if (type == "binary")
	create disk_out {path}
    end
    if (type == "ascii")
	create asc_file {path}
    end
    setfield {path} flush 1 leave_open 1
    useclock {path} 1
end

// =================================
//   modify spike output based on integration step to give constant 
//   impulse area
// =================================
function adjustspike
    setfield /##[TYPE=spike] output_amp {1.0/{getclock 0}}
end


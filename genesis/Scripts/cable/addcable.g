//genesis
// addcable.g - routines to add a passive cable to the soma

// From the standpoint of the user, cable sections are labeled from
// 1 to NCableSects.  (Zero means none exist.)  However, the actual
// cable elements are labeled cable[0] through cable[{NCableSects-1}].

function link_compartment(comp1, comp2)
    str comp1, comp2
    addmsg {comp1} {comp2} AXIAL previous_state
    addmsg {comp2} {comp1} RAXIAL Ra previous_state
end

function make_cable(nsects)
    int nsects, i
    makecompartment /cell/cable[{0}] {dend_l} {dend_d} {EREST}
     // give it an x-coord for view
     setfield /cell/cable[0] x 1.0
    if (nsects > 0)
      for (i = 1; i < nsects; i = i + 1)
        makecompartment /cell/cable[{i}] {dend_l} {dend_d} {EREST}
	// give it an x-coord for view
	setfield /cell/cable[{i}] x {i + 1}
	link_compartment /cell/cable[{i - 1}] /cell/cable[{i}]
      end
    end
end

/* \
     NOTE: If a cable exists, it is deleted and a new one is created.  This
   means that messages for current injection, synaptic input and plotting
   must be reestablished.  (The latter has not yet been done.)  It may be
   better to retain any sections (with their messages) which will be used
   in the new cable.
*/

function add_cable
// global variables used:
//	int NCableSects = number of existing cable sections
//	int IntegMethod = the current integration method
    int i, nsects    // number of cable sections after function call
    int comp_no    // compartment for injection or synaptic input 
    int SaveMethod    // temporary place to save IntegMethod
    nsects = {getfield /output/control/nsections value}
    if (nsects < 0)
        nsects = 0
    end
    if (nsects == {NCableSects})    // quit if no change
	return
    end
    if ({exists /cell/solve})
        delete /cell/solve
    end
    // hack to prevent add_injection, add_syn_input from making solve too soon
    SaveMethod = {IntegMethod}
    IntegMethod = 0
    if ({NCableSects} > 0)    // delete the cable and all its messages
        for (i = 0; i < {NCableSects}; i = i + 1)
	    delete /cell/cable[{i}]
	end
    end
    if (nsects > 0)
	make_cable {nsects}
	link_compartment /cell/soma /cell/cable[0]
    end
    // new number of cable sections
    NCableSects=nsects
    // re-establish the current injection and synaptic input to cable
    comp_no = ({getfield /output/inject_menu/comp_no value})
    if ((comp_no) > 0)    // if injection was to cable 

	if ((comp_no) > (nsects))	// if compartment no longer exists

	    // then inject soma
	    comp_no = 0
	    setfield /output/inject_menu/comp_no value 0
	end
	add_injection
    end
    comp_no = ({getfield /output/synapse_menu/comp_no value})
    if ((comp_no) > 0)    // if input was to cable 

	if ((comp_no) > (nsects))	// if compartment no longer exists

	    // then input to soma
	    comp_no = 0
	    setfield /output/synapse_menu/comp_no value 0
	end
	add_syn_input
     end
     // cable plots will have been lost
     setfield /output/plot_menu/add value 0
     // simulator must be reset after altering connections
     reset
//     if ({exists /viewform/draw})     // delete the draw and view
//	delete_draw
//     end
//     if ({getfield /output/control/propagation state} == 1)
// 	make_draw {nsects + 1} 	//If propagation visible, create widgets
//     end
    echo {nsects}" passive cable compartments added to soma"
    // now, set_method can make hsolve if needed
    IntegMethod = SaveMethod
    set_method {IntegMethod}    // use the current integration method
end

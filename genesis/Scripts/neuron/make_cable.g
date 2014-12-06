//genesis
// make_cable.g - routines to unlink compartments and insert a passive cable

function unlink_comp(comp1, comp2)
    str comp1, comp2
    int i, n, nmsg
    // Delete the incoming message to comp1 from comp2
    //loop over the number of incoming messages
    nmsg = {getmsg {comp1} -incoming -count}
    for (i = 0; i < nmsg; i = i + 1)
        if ({getmsg {comp1} -incoming -source {i}} == (comp2))
	    n = i
	    break
	else
	    n = -1
	end
    end
    if (n >= 0)
        deletemsg {comp1}  {n} -incoming
    end
    // Now delete the outgoing message from comp1 to comp2
    nmsg = {getmsg {comp1} -outgoing -count}
    for (i = 0; i < nmsg; i = i + 1)
        if ({getmsg {comp1} -outgoing -destination {i}} == (comp2))
	    n = i
	    break
	else
	    n = -1
	end
    end
    if (n >= 0)
        deletemsg {comp1}  {n} -outgoing
    end
end

function make_cable(nsects)
    int nsects, i
    makecompartment /cell/cable[{0}] {dend_l} {dend_d} {EREST}
    if (nsects > 0)
      for (i = 1; i < nsects; i = i + 1)
        makecompartment /cell/cable[{i}] {dend_l} {dend_d} {EREST}
	link_compartment /cell/cable[{i - 1}] /cell/cable[{i}]
      end
    end
end

function add_cable
    // number of cable sections after function call
    int i, nsects
    // global variable:    int NCableSects = number of existing cable sections
    // alternatively, use "return({NCableSects})" to pass it back via func name
    nsects = {getfield /output/control/nsections value}
    if (nsects < 0)
        nsects = 0
    end
    if (nsects != {NCableSects})    // otherwise, do nothing

        if ({NCableSects} > 0)        // delete the cable

	    for (i = 0; i < {NCableSects}; i = i + 1)
	        // also deletes messages to dend1,2
	        delete /cell/cable[{i}]
	    end
	else	// if no cable, unlink dend2 and dend1

	    unlink_comp /cell/dend2 /cell/dend1
	end

	if (nsects > 0)
	    make_cable {nsects}
	    link_compartment /cell/dend2 /cell/cable[0]
	    link_compartment /cell/cable[{nsects - 1}] /cell/dend1
	else	// link dend2 and dend1 without intervening cable

	    link_compartment /cell/dend2 /cell/dend1
        end
        // simulator must be reset after altering connections
        reset_kludge
    end
    NCableSects=nsects
    echo {nsects}" passive sections added between dendrite #2 and #1"
end

//genesis

// synactivator object
//
// This is an extended object built from scratch to provide
// for synaptic activation equivalent to a single spike of
// a given weight.  The activation is delivered by calling
// an action.  This is something that would be nice to have
// in the various synaptic channel objects.

int __SYN_ACTIVATOR_OBJECT_DEFINED__

if (__SYN_ACTIVATOR_OBJECT_DEFINED__)
    return
end


// a synactivator is a neutral with the following actions and fields:
//
// FIELDS
//
//	weight	(rw)	: spike weight (alias for z)
//	act	(ro)	: calculated activation to deliver (alias for y)
//	path	(ro)	: channel to deliver to
//
// ACTIONS
//
//	SEND_SPIKE path	: send a spike to the given channel
//	PROCESS		: called from schedule to terminate activation
//
// The object is defined to be in the device class.  This is important
// as the order in which the synactivator PROCESS action is called from
// the schedule determines when the activation is terminated.  Since
// devices follow the channels in the schedule, we can terminate the
// activation on the next call to PROCESS.

create neutral /synactivator
pushe /synactivator > /dev/null

addfield . weight -indirect . z -description "Spike weight"
addfield . act -indirect . y -description "Calculated activation"
addfield . path -description "Destination channel"

setfieldprot . -readonly path act

addaction . SEND_SPIKE __synactivatorSEND_SPIKE
addaction . PROCESS __synactivatorPROCESS

addclass . device

pope > /dev/null

addobject synactivator /synactivator


// ACTION functions

// SEND_SPIKE action
//
// calculate the activation for the channel and send an ACTIVATION
// message causing the activation to be added to the channel when
// it is next processed.

function __synactivatorSEND_SPIKE(action, chanpath)

    if ({getfield act} != 0.0)
	echo synactivator: already sending a spike to {getfield path}
    end

    str chantype
    foreach chantype (manualconduct channelC channelC2 channelC3 \
	    channelA channelB synchan synchan2 hebbsynchan hebbsynchan2)
	if ({isa {chantype} {chanpath}})
	    chantype = "Ok"
	    break
	end
    end

    if (chantype != "Ok")
	echo synactivator: '{chanpath}' not a synaptic channel
	return
    end

    setfield act { {getfield weight} / {getclock 0} }
    setfield path {chanpath}

    addmsg . {chanpath} ACTIVATION act

end

// PROCESS action
//
// Remove the ACTIVATION message if there is one and zero the act
// field which indicates no spike being sent

function __synactivatorPROCESS

    if ({getfield act} != 0.0)
	setfield act 0
	deletemsg {getfield path} 0 -find . ACTIVATION
    end

end

__SYN_ACTIVATOR_OBJECT_DEFINED__ = 1

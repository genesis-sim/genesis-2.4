//genesis

function make_dosercontrol
    create xform /dosercontrol [80,{TB},500,600]
    addfield /dosercontrol is_visible -description "flag for visibility"
    setfield /dosercontrol is_visible 0
    ce /dosercontrol
	create neutral tab
	int i
	float base = 0.0001
	for (i = 0; i < 22; i = i + 1)
		addfield tab conc[{i}]
		if ((i % 3) == 0)
			setfield tab conc[{i}] {base}
		end
		if ((i % 3) == 1)
			setfield tab conc[{i}] {base * 2}
		end
		if ((i % 3) == 2)
			setfield tab conc[{i}] {base * 5}
			base = base * 10
		end
	end

	create neutral finetab
	base = 0.0001
	for (i = 0; i < 72; i = i + 1)
		addfield finetab conc[{i}]
		if ((i % 10) == 0)
			setfield finetab conc[{i}] {base}
		end
		if ((i % 10) == 1)
			setfield finetab conc[{i}] {base * 1.2}
		end
		if ((i % 10) == 2)
			setfield finetab conc[{i}] {base * 1.5}
		end
		if ((i % 10) == 3)
			setfield finetab conc[{i}] {base * 2}
		end
		if ((i % 10) == 4)
			setfield finetab conc[{i}] {base * 2.5}
		end
		if ((i % 10) == 5)
			setfield finetab conc[{i}] {base * 3}
		end
		if ((i % 10) == 6)
			setfield finetab conc[{i}] {base * 4}
		end
		if ((i % 10) == 7)
			setfield finetab conc[{i}] {base * 5}
		end
		if ((i % 10) == 8)
			setfield finetab conc[{i}] {base * 6}
		end
		if ((i % 10) == 9)
			setfield finetab conc[{i}] {base * 8}
			base = base * 10
		end
	end

	create xtext instr_text -hgeom 200 -bg white
	xtextload instr_text \
	"	Instructions:" \
	"Double-click on the pool you want to vary, then click on "\
	"the 'Select input pool' button. Or, enter the pool path"\
	"in the appropriate dialog."\
	"Now Double-click on the pool you want to monitor, then click on "\
	"the 'Select output pool' button. Or, enter the pool path"\
	"in the appropriate dialog."\
	"Select the desired start and end concentration value."\
	"Ensure that the runtime is sufficient for the run to settle."\
	"Check that the clocks are set up for the desired accuracy."\
	"Select Buffered or Incremented options to decide how the "\
	"variable pool will be changed. Buffered ensures that the" \
	"free pool concentrations are known, but the total pool" \
	"concentration will be unknown and larger. Incremented option"\
	"controls the total pool concentration but the free concentration"\
	"will be smaller."\
	"When all is done, click the 'start' button."

	create xbutton selectin -label "Select variable pool" \
		-script "do_select_inpool"
    create xdialog inpool -label "Input pool name:" 
	create xbutton selectout -label "Select monitored pool" \
		-script "do_select_outpool"
    create xdialog outpool -label "Output pool name:" 

	create xlabel range -bg cyan -label "Concentration range"
	create xtoggle s0 -label "0.1nM" -wgeom 50%
	create xtoggle s1 [0:last,0:range,50%,] -label "1nM"
	create xtoggle s2 -label "10nM" -wgeom 50%
	create xtoggle s3 [0:last,0:s0,50%,] -label "100nM"
	create xtoggle s4 -label "1uM" -wgeom 50%
	create xtoggle s5  [0:last,0:s2,50%,] -label "10uM"
	create xtoggle s6 -label "100uM" -wgeom 50%
	create xtoggle s7  [0:last,0:s4,50%,] -label "1mM"

	create xlabel separator -bg cyan -label ""
	create xtoggle buftog -onlabel "Buffered" -offlabel "Incremented"
	create xtoggle finetog -onlabel "Fine spacing: 10/log" -offlabel "Coarse spacing: 3/log"
	create xbutton Start -offbg green -script do_doser
	create xbutton Halt -offbg red -script do_halt_doser
	addfield Halt halt_flag
	setfield /dosercontrol/Halt halt_flag 0

    create xbutton Cancel -script "xhide /dosercontrol"
    disable /dosercontrol
	ce /
end

function do_select_inpool
	setfield /dosercontrol/inpool value {getfield /parmedit/pool elmpath}
end

function do_select_outpool
	setfield /dosercontrol/outpool value {getfield /parmedit/pool elmpath}
end

function find_doser_min
	int i
	for (i = 0; i <= 7; i = i + 1)
		if ({getfield /dosercontrol/s{i} state} == 1)
			return {i}
		end
	end
	do_inform 0 "Warning: No minimum value specified"
	return -1
end

function find_doser_max
	int i
	for (i = 7; i >= 0; i = i - 1)
		if ({getfield /dosercontrol/s{i} state} == 1)
			return {i}
		end
	end
	do_inform 0 "Warning: No maximum value specified"
	return -1
end

function do_halt_doser
	setfield /dosercontrol/Halt halt_flag 1
	stop
end

function do_doser
	int min = {find_doser_min}
	int max = {find_doser_max}

	if ((min == -1) || (max == -1))
		return
	end

	float conc
	float lastconc = 0
	str inpool = {getfield /dosercontrol/inpool value}
	str outpool = {getfield /dosercontrol/outpool value}
	int isbuf = {getfield /dosercontrol/buftog state}
	float temp
	int i
	int orig_slave = {getfield {inpool} slave_enable}
	float orig_CoInit = {getfield {inpool} CoInit}
	setfield {inpool} CoInit 0 slave_enable 0
	int usefine = {getfield /dosercontrol/finetog state}
	str tab
	do_reset
	if (usefine)
		min = min * 10
		max = max * 10
		tab = "/dosercontrol/finetab"
	else
		min = min * 3
		max = max * 3
		tab = "/dosercontrol/tab"
	end

	for (i = min; i <= max; i = i + 1)
		conc = {getfield {tab} conc[{i}]}
		if (isbuf)
			setfield {inpool} CoInit {conc} slave_enable 4
		else
			temp = {getfield {inpool} Co}
			setfield {inpool} Co {temp + conc - lastconc}
		end
		do_run
		if ({getfield /dosercontrol/Halt halt_flag} == 1)
			do_inform "Dose-response run terminated"
			setfield /dosercontrol/Halt halt_flag 0
			break;
		end
		echo {conc}"	"{getfield {outpool} Co}
		lastconc = conc
	end
	setfield {inpool} CoInit {orig_CoInit} slave_enable {orig_slave}

end

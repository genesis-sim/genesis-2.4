// genesis


function activate

	enable /output
	enable /field
	enable /pyr
	enable /ff
	enable /fb
	enable /bulb

	setclock 0 {getfield /x_interface/simulate/step_size value}
	disable /x_interface/playback/clock
	reset
end


function deactivate

	enable /playback
	enable /x_interface/playback/clock

	disable /field
	disable /output
	disable /pyr
	disable /ff
	disable /fb
	disable /bulb
	reset
end



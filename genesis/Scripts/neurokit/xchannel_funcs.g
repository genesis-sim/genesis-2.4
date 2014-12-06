//genesis

int kincolor = 15

function do_xchannel_funcs
	create x1form /channel_params  \
	    [480,440,{user_screenwidth - 480},450]
	disable /channel_params
	pushe /channel_params
	create x1graph alpha [0,0,50%,42%] -range [-0.1,0,0.1,1]
	setfield alpha ymin 0 overlay 0 XUnits Volts YUnits "1/sec"
	create x1graph beta [50%,0,50%,42%] -range [-0.1,0,0.1,1]
	setfield beta ymin 0 overlay 0 XUnits Volts YUnits "1/sec"
	create x1graph taus [0,42%,50%,42%] -range [-0.1,0,0.1,1]
	setfield taus ymin 0 overlay 0 XUnits Volts YUnits "sec"
	create x1graph minf [50%,42%,50%,42%] -range [-0.1,0,0.1,1]
	setfield minf ymin 0 overlay 0 XUnits Volts YUnits "minf"
	create x1dialog gate [4,4:minf.bottom,19%,30] -value "X" -script  \
	    "do_plot_kinetics "<widget>
	create x1toggle overlay [20%,4:minf.bottom,19%,30]
	setfield ^ label0 "overlay off" label1 "overlay on"
	create x1label MODIFY [40%,4:minf.bottom,11%,30] -title "MODIFY:"
	create x1button alphabutton [52%,4:minf.bottom,11%,30] -title  \
	    alpha -script "scale_gate alpha"
	create x1button betabutton [64%,4:minf.bottom,11%,30] -title beta \
	     -script "scale_gate beta"
	create x1button taubutton [76%,4:minf.bottom,11%,30] -title tau  \
	    -script "scale_gate tau"
	create x1button minfbutton [88%,4:minf.bottom,11%,30] -title  \
	    m_inf -script "scale_gate minf"

	create x1button duplicate [0,4:overlay.bottom,14%,30] -title  \
	    "Dup Gate" -script dup_gate
	create x1button restore [14%,4:overlay.bottom,14%,30] -title  \
	    "Restore" -script "echo Not yet implemented"
	create x1dialog sx [28%,4:overlay.bottom,18%,30] -value 1.0
	create x1dialog sy [46%,4:overlay.bottom,18%,30] -value 1.0
	create x1dialog ox [64%,4:overlay.bottom,18%,30] -value 0.0
	create x1dialog oy [82%,4:overlay.bottom,18%,30] -value 0.0
	pope
end

function calc_cond(name)
	str name
	float len, dia, gmax
	float area = {getfield {comptpath} Cm}

	/*
	len = calc_len({comptpath})
	dia = calc_dia({comptpath})
	*/
	area = area/CM
	if (({strcmp {getfield {channelpath} object->name} hh_channel} == 0) \
 || ({strcmp {getfield {channelpath} object->name} tabchannel} == 0) \
 || ({strcmp {getfield {channelpath} object->name} tab2Dchannel} == 0))
		gmax = {getfield {name} Gbar}
	else
		if ( \
		    {strcmp {getfield {channelpath} object->name} vdep_channel} == 0 \
		    )
			gmax = {getfield {name} gbar}
		else
			gmax = {getfield {name} gmax}
		end
	end

	/*
	return({gmax/(len * dia * PI)})

	As a general rule, CM is less likely to be tweaked than RM and RA
	*/
	return ((gmax/area))
end

function calc_reversal(name)
	str name
	float rev
	if ( \
	    {strcmp {getfield {channelpath} object->name} hh_channel} == 0 \
	    )
		rev = {getfield {name} Ek}
	else
		rev = {getfield {name} Ek}
	end

	return ((rev))
end

function set_cond(widget)
	str widget

	/*
	float len,dia
	*/
	float gmax = {getfield {widget} value}
	float area
	str name, chan
	float temp

	chan = {getfield /compt_display/draw/{getfield /compt_display/draw pixchoose} text}


	if (({getfield /compt_select/scope state}) == 1)	// global

		if ( \
		    ({strcmp {getfield {channelpath} object->name} hh_channel} == 0) || ({strcmp {getfield {channelpath} object->name} tabchannel} == 0) \
		    )
			foreach name ( \
			    {el {cellpath}/#[][activation>0.99]})
				area = {getfield {name} Cm}
				area = area/CM
				temp = gmax*area
				setfield {name}/{chan} Gbar {temp}
			end
		else
			if ( \
			    {strcmp {getfield {channelpath} object->name} vdep_channel} == 0 \
			    )
				foreach name ( \
				    {el {cellpath}/#[][activation>0.99]} \
				    )
					area = {getfield {name} Cm}
					area = area/CM
					temp = gmax*area
					setfield {name}/{chan}  \
					    gbar {temp}
				end
			else
				foreach name ( \
				    {el {cellpath}/#[][activation>0.99]} \
				    )
					area = {getfield {name} Cm}
					area = area/CM
					temp = gmax*area
					setfield {name}/{chan}  \
					    gmax {temp}
				end
			end
		end
	else	// local

		area = {getfield {comptpath} Cm}
		area = area/CM

		gmax = gmax*area

		if ( \
		    ({strcmp {getfield {channelpath} object->name} hh_channel} == 0) || ({strcmp {getfield {channelpath} object->name} tabchannel} == 0) \
		    )
			setfield {channelpath} Gbar {gmax}
		else
			if ( \
			    {strcmp {getfield {channelpath} object->name} vdep_channel} == 0 \
			    )
				setfield {channelpath} gbar {gmax}
			else
				setfield {channelpath} gmax {gmax}
			end
		end
	end
end

function set_revpotl(widget)
	str widget

	float rev = {getfield {widget} value}
	float area
	str name, chan
	float temp

	chan = {getfield /compt_display/draw/{getfield /compt_display/draw pixchoose} text}
	if (({getfield /compt_select/scope state}) == 1)	// global

		foreach name ({el {cellpath}/#[][activation>0.99]})
			setfield {name}/{chan} Ek {rev}
		end
	else	// local

		setfield {channelpath} Ek {rev}
	end
end

function update_channel_params
	/*
	echo updating channel parameters
	*/
end

function hilight_channel
	float x, y
	str pix

	pushe /compt_display/draw
	pix = {getfield . pixchoose}
	setfield hilight tx {getfield {pix} tx} ty {getfield {pix} ty}
	xupdate .
	pope
end

function deleteelms(path)

	str	elm

	foreach elm ({el {path}})
	    delete {elm}
	end
end

function do_plot_kinetics(widget)
	str widget
	str gate = {getfield {widget} value}
	str channeltype
	str gatepath
	float x, y, a, b, tau
	float xmin = -0.1, xmax = 0.07, dx = 0.002
	float amax = -1.0, bmax = -1.0, mmax = -1.0, tmax = -1.0
	str kinname
	int i

	ce /channel_params
	dx = 0.005

	if (!({exists {channelpath}}))
		return
	end

	kinname = {getfield {channelpath} name} @ "->" @ (gate)

	channeltype = {getfield {channelpath} object->name}
	if (({getfield /channel_params/overlay state}) == 0)
		//DHB call alpha RESET
		//DHB call beta RESET
		//DHB call taus RESET
		//DHB call minf RESET
		deleteelms alpha/##[ISA=xplot]
		deleteelms beta/##[ISA=xplot]
		deleteelms taus/##[ISA=xplot]
		deleteelms minf/##[ISA=xplot]
		setfield alpha ymax 1e6
		setfield beta ymax 1e6
		setfield taus ymax 1e6
		setfield taus ymax 1e6
		kincolor = 15
	else
		kincolor = kincolor + 15
		if (kincolor > 63)
			kincolor = kincolor - 60
		end
		amax = {getfield alpha ymax}
		bmax = {getfield beta ymax}
		mmax = {getfield minf ymax}
		tmax = {getfield taus ymax}
	end

	if ({strcmp {channeltype} hh_channel} == 0)
		if ( \
		    !(({strcmp {gate} "X"} == 0) || ({strcmp {gate} "Y"} == 0)) \
		    )
			echo For hh channels, gate must be X or Y
			return
		end
	end

	if ({strcmp {channeltype} tabchannel} == 0)
		if ( \
		    !(({strcmp {gate} "X"} == 0) || ({strcmp {gate} "Y"} == 0) || ({strcmp {gate} "Z"} == 0)) \
		    )
			echo For vdep channels, gate must be X or Y or Z
			return
		end
		if ({getfield {channelpath} {gate}_alloced} == 1)
			xmin = {getfield {channelpath} {gate}_A->xmin}
			xmax = {getfield {channelpath} {gate}_A->xmax}
			dx = {getfield {channelpath} {gate}_A->dx}
			if (dx < ((xmax - xmin)/100.0))
				dx = (xmax - xmin)/100.0
			end
		else
			echo gate {gate} not allocated
			return
		end
	end

	if ( \
	    ({strcmp {channeltype} hh_channel} == 0) || ({strcmp {channeltype} tabchannel} == 0) \
	    )

		if (!{exists alpha/{kinname}})
		    create xplot alpha/{kinname}
		end

		if (!{exists beta/{kinname}})
		    create xplot beta/{kinname}
		end

		if (!{exists taus/{kinname}})
		    create xplot taus/{kinname}
		end

		for (x = xmin; x <= xmax; x = x + dx)

			a = {call {channelpath} CALC_ALPHA {gate} {x}}
			if (amax < a)
				amax = a
			end

			b = {call {channelpath} CALC_BETA {gate} {x}}
			if (bmax < b)
				bmax = b
			end
			tau = 1/(a + b)
			if (tau > tmax)
				tmax = tau
			end

			call alpha/{kinname} ADDPTS {x} {a}
			setfield alpha/{kinname} fg {kincolor}

			call beta/{kinname} ADDPTS {x} {b}
			setfield beta/{kinname} fg {kincolor}

			call taus/{kinname} ADDPTS {x} {tau}
			setfield taus/{kinname} fg {kincolor}
		end

		if (!{exists minf/{kinname}})
		    create xplot minf/{kinname}
		end

		for (x = xmin; x <= xmax; x = x + dx)
			y = {call {channelpath} CALC_MINF {gate} {x}}
			if (y > mmax)
				mmax = y
			end

			call minf/{kinname} ADDPTS {x} {y}
			setfield minf/{kinname} fg {kincolor}
		end

		setfield alpha ymax {amax} xmin {xmin} xmax {xmax}
		setfield beta ymax {bmax} xmin {xmin} xmax {xmax}
		setfield taus ymax {tmax} xmin {xmin} xmax {xmax}
		setfield minf ymax {mmax} xmin {xmin} xmax {xmax}
	end

	if ({strcmp {channeltype} vdep_channel} == 0)
		gatepath = (channelpath) @ "/" @ (gate)
		if (!({exists {gatepath}}))
			echo Gate {gatepath} does not exist.
			return
		end
		if ( \
		    {strcmp {getfield {gatepath} object->name} vdep_gate} != 0 \
		    )
			xmin = {getfield {gatepath} alpha->xmin}
			xmax = {getfield {gatepath} alpha->xmax}
			dx = {getfield {gatepath} alpha->dx}
			if (dx < ((xmax - xmin)/100.0))
				dx = (xmax - xmin)/100.0
			end
		end

		if (!{exists alpha/{kinname}})
		    create xplot alpha/{kinname}
		end

		if (!{exists beta/{kinname}})
		    create xplot beta/{kinname}
		end

		if (!{exists taus/{kinname}})
		    create xplot taus/{kinname}
		end

		for (x = xmin; x <= xmax; x = x + dx)

			a = {call {gatepath} CALC_ALPHA {x}}
			if (amax < a)
				amax = a
			end

			b = {call {gatepath} CALC_BETA {x}}
			if (bmax < b)
				bmax = b
			end
			tau = 1/(a + b)
			if (tau > tmax)
				tmax = tau
			end

			call alpha/{kinname} ADDPTS {x} {a}
			setfield alpha/{kinname} fg {kincolor}

			call beta/{kinname} ADDPTS {x} {b}
			setfield beta/{kinname} fg {kincolor}

			call taus/{kinname} ADDPTS {x} {tau}
			setfield taus/{kinname} fg {kincolor}
		end

		if (!{exists minf/{kinname}})
		    create xplot minf/{kinname}
		end
		for (x = xmin; x <= xmax; x = x + dx)
			y = {call {gatepath} CALC_MINF {x}}
			if (y > mmax)
				mmax = y
			end

			call minf/{kinname} ADDPTS {x} {y}
			setfield minf/{kinname} fg {kincolor}
		end

		setfield alpha ymax {amax} xmin {xmin} xmax {xmax}
		setfield beta ymax {bmax} xmin {xmin} xmax {xmax}
		setfield taus ymax {tmax} xmin {xmin} xmax {xmax}
		setfield minf ymax {mmax} xmin {xmin} xmax {xmax}
	end
	if ({strcmp {channeltype} channelC2} == 0)
	end
	if ({strcmp {channeltype} table} == 0)
	end

	ce /
end// do_plot_kinetics


function scale_gate(mode)
	str mode
	str gate
	float sx = {getfield /channel_params/sx value}
	float sy = {getfield /channel_params/sy value}
	float ox = {getfield /channel_params/ox value}
	float oy = {getfield /channel_params/oy value}
	str channeltype = {getfield {channelpath} object->name}

	if ({strcmp {channeltype} tabchannel} == 0)
		gate = {getfield /channel_params/gate value}
		scaletabchan {channelpath} {gate} {mode} {sx} {sy} {ox}  \
		    {oy}
		do_plot_kinetics /channel_params/gate
		return
	end
	if ({strcmp {channeltype} vdep_channel} == 0)
		gate = (channelpath) @ "/" @ ({getfield /channel_params/gate value})
		if (!{exists {gate}})
			echo gate '{gate}' does not exist
			return
		end
		if ({strcmp {getfield {gate} object->name} tabgate} != 0 \
		    )
			echo gate '{gate}' is not a tabgate
			return
		end
		// For tabgates, argv[1] is the tabgate path and argv[2] is ignored
		scaletabchan {gate} {gate} {mode} {sx} {sy} {ox} {oy}
		do_plot_kinetics /channel_params/gate
		return
	end
	echo channel type {channeltype} cannot be manipulated this way
end

function dup_gate
	str gate
	str channeltype = {getfield {channelpath} object->name}
	if ({strcmp {channeltype} tabchannel} == 0)
		gate = {getfield /channel_params/gate value}
		duplicatetable {channelpath} {gate}
		return
	end
	if ({strcmp {channeltype} vdep_channel} == 0)
		gate = (channelpath) @ "/" @ ({getfield /channel_params/gate value})
		if (!{exists {gate}})
			echo gate '{gate}' does not exist
			return
		end
		if ({strcmp {getfield {gate} object->name} tabgate} != 0 \
		    )
			echo gate '{gate}' is not a tabgate
			return
		end
		// For tabgates, argv[1] is the tabgate path and argv[2] is ignored
		duplicatetable {gate} {gate}
		return
	end
	echo channel type {channeltype} cannot be manipulated this way
end

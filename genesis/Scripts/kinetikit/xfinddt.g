//genesis

function find_forward_frac_propensity(name)
	int nmsg
	int i
	float pf = {getfield {name} kf}
	str mtype
	str msrc
	float mval
	float min = 1e10

	pushe {name}
	nmsg = {getmsg . -in -count}
	for (i = 0 ; i < nmsg; i = i + 1)
		mtype = {getmsg . -in -type {i}}
		if ({strcmp {mtype} "SUBSTRATE"} == 0)
			msrc = {getmsg . -in -source {i}}
			mval = {getfield {msrc} n}
			if (min > mval)
				min = mval
			end
			pf = pf * mval
		end
	end
	pope
	if (min > 0)
		return {pf / min}
	else
		return 0
	end
end

function find_backward_frac_propensity(name)
	int nmsg
	int i
	float pb = {getfield {name} kb}
	str mtype
	str msrc
	float mval
	float min = 1e10

	pushe {name}
	nmsg = {getmsg . -in -count}
	for (i = 0 ; i < nmsg; i = i + 1)
		mtype = {getmsg . -in -type {i}}
		if ({strcmp {mtype} "PRODUCT"} == 0)
			msrc = {getmsg . -in -source {i}}
			mval = {getfield {msrc} n}
			if (min > mval)
				min = mval
			end
			pb = pb * mval
		end
	end
	pope
	if (min > 0)
		return {pb / min}
	else
		return 0
	end
end

function find_enz_sub_frac_propensity(name)
	int nmsg
	int i
	float pf = {getfield {name} k1}
	str mtype
	str msrc
	float mval
	float min = 1e10

	pushe {name}
	nmsg = {getmsg . -in -count}
	for (i = 0 ; i < nmsg; i = i + 1)
		mtype = {getmsg . -in -type {i}}
		if (({strcmp {mtype} "SUBSTRATE"} == 0) || ({strcmp {mtype} "ENZYME"} == 0))
			msrc = {getmsg . -in -source {i}}
			mval = {getfield {msrc} n}
			if (min > mval)
				min = mval
			end
			pf = pf * mval
		end
		if ({strcmp {mtype} "INTRAMOL"} == 0)
			mval = {getfield {msrc} n}
			if (mval > 0)
				pf = pf / mval
			end
		end
	end
	pope
	if (min > 0)
		return {pf / min}
	else
		return 0
	end
end

function find_enz_prd_frac_propensity(name)
	return {{getfield {name} k2}  + {getfield {name} k3}}
end

function find_dt(err, do_display)
	float err
	int do_display

	str name
	float pf
	float max_pf = 0
	str max_pf_name

	foreach name ({el /kinetics/##[ISA=reac]})
		pf = {find_forward_frac_propensity {name}}
		if (max_pf < pf)
			max_pf = pf
			max_pf_name = name @ " kf"
		end
	end

	foreach name ({el /kinetics/##[ISA=reac]})
		pf = {find_backward_frac_propensity {name}}
		if (max_pf < pf)
			max_pf = pf
			max_pf_name = name @ " kb"
		end
	end

	foreach name ({el /kinetics/##[ISA=enz]})
		pf = {find_enz_sub_frac_propensity {name}}
		if (max_pf < pf)
			max_pf = pf
			max_pf_name = name @ " k1"
		end
	end

	foreach name ({el /kinetics/##[ISA=enz]})
		pf = {find_enz_prd_frac_propensity {name}}
		if (max_pf < pf)
			max_pf = pf
			max_pf_name = name @ " k2, k3"
		end
	end

	if (do_display)
		do_inform "dt = "{{sqrt {err}} / max_pf}" on "{max_pf_name}
	end
	return {{sqrt {err}} / max_pf}
end

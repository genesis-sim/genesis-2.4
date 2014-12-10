// genesis

function create_display
	xcolorscale rainbow2
	create xform /xout [40,400,900,400]
	ce /xout

	// Creating a view to look at the input pattern
	float ret_xscale = 0.7
	float ret_yscale = 0.8
	float ret_xoffset = REC_SEPX/2
	float ret_yoffset = REC_SEPY/2

	create xlabel /xout/"Input Pattern" [0%,2%,30%,30] 
	create xdraw /xout/drawr [0%,10%,30%,300]
	setfield /xout/drawr \
	    xmin {ret_xmin / ret_xscale} \
	    xmax {ret_xmax / ret_xscale} \
	    ymin {ret_ymin / ret_yscale} \
	    ymax {ret_ymax / ret_yscale}
	create xview /xout/drawr/inputs 
	setfield     /xout/drawr/inputs \
		     field "msgin" \
		     tx {ret_xoffset}   \
		     ty {ret_yoffset} \
		     sizescale {REC_SEPX * 0.8}

	create xdraw /xout/drawh [33%,10%,30%,300] 
	setfield     /xout/drawh \
	    xmin     {V1_xmin / V1_xscale} \
	    xmax     {V1_xmax / V1_xscale} \
	    ymin     {V1_ymin / V1_yscale} \
	    ymax     {V1_ymax / V1_yscale}
	create xlabel /xout/"V1 horizontal cells" [35%,2%,30%,30]

	create xview /xout/drawh/inputs 
	setfield     /xout/drawh/inputs \
		     tx {V1_xoffset}   \
		     ty {V1_yoffset}   \
		     field "msgin" \
		     value_min[0] -0.090 \
		     value_max[0] 0.060 \
		     sizescale {V1_SEPX * 30} \
		     morph_val 0

	create xlabel /xout/"Activity Pattern" [69%,2%,30%,30]
	create xdraw /xout/drawv [67%,10%,30%,300] 
	setfield     /xout/drawv \
	    xmin     {V1_xmin / V1_xscale} \
	    xmax     {V1_xmax / V1_xscale} \
	    ymin     {V1_ymin / V1_yscale} \
	    ymax     {V1_ymax / V1_yscale}
	create xlabel /xout/"V1 vertical cells" [68%,2%,30%,30]

	create xview /xout/drawv/inputs 
	setfield     /xout/drawv/inputs \
		     field "msgin" \
		     tx {V1_xoffset}   \
		     ty {V1_yoffset}   \
		     value_min[0] -0.090 \
		     value_max[0] 0.060 \
		     sizescale {V1_SEPX * 30} \
		     morph_val 0

	xshow /xout
end

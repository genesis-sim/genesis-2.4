// genesis


function adjust_wts(RA, CA, fb_pyr, fb_fb, ff_pyr, bulb_pyr, bulb_ff,  \
    bulb_fb, pyr_fb, pyr_ff, pyr_local)
float  \
    RA, CA, fb_pyr, fb_fb, ff_pyr, bulb_pyr, bulb_ff, bulb_fb, pyr_fb, pyr_ff, pyr_local

	scaleweight /pyr/pyramidal[]/RAaxon {{weight_RA}*RA}
	scaleweight /pyr/pyramidal[]/CAaxon {{weight_CA}*CA}
	scaleweight /fb/interneuron[]/axon {{weight_fb_pyr}*fb_pyr}
	scaleweight /fb/interneuron[]/fbaxon {{weight_fb_fb}*fb_fb}
	scaleweight /ff/interneuron[]/axon {{weight_ff_pyr}*ff_pyr}
	scaleweight /bulb/mitral[]/axon {{weight_bulb_pyr}*bulb_pyr}
	scaleweight /bulb/mitral[]/axon2 {{weight_bulb_ff}*bulb_ff}
	scaleweight /bulb/mitral[]/axon3 {{weight_bulb_fb}*bulb_fb}
	scaleweight /pyr/pyramidal[]/FBaxon {{weight_pyr_fb}*pyr_fb}
	scaleweight /pyr/pyramidal[]/FFaxon {{weight_pyr_ff}*pyr_ff}
	scaleweight /pyr/pyramidal[]/LRAaxon  \
	    {{weight_pyr_local}*pyr_local}
	scaleweight /pyr/pyramidal[]/LCAaxon  \
	    {{weight_pyr_local}*pyr_local}

	echo Weights Adjusted

end

adjust_wts 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0





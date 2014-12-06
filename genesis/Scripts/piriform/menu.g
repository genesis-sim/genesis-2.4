// genesis

create xform /menu [0,0,600,600]
create xdraw /menu/draw [0,0,100%,100%]  -bg antiquewhite -wx 1000.0 -wy 1000.0
xshow /menu

ce /menu/draw


// pyramidal cell minus axons

create xshape Ia -fg black -coords [-40,62.5,0][40,62.5,0][-40,62.5,0][-40,-62.5,0][-40,-62.5,0][40,-62.5,0][40,-62.5,0][40,65,0] \
        -tx 0 -ty 332.5 \
        -linewidth 5 -pixflags c
create xshape supIb -fg black -coords [-40,66,0][-40,-66,0][-40,-66,0][40,-66,0][40,-66,0][40,66,0] \
        -tx 0 -ty 199.5 \
        -linewidth 5 -pixflags c
create xshape deepIb -fg black -coords [-40,66,0][-40,-63,0][-40,-63,0][40,-63,0][40,-63,0][40,66,0] \
        -tx 0 -ty 63 \
        -linewidth 5 -pixflags c
create xshape soma -fg black -coords [-20,-50,0][-80,-50,0][-80,-50,0][-40,50,0][40,50,0][80,-50,0][80,-50,0][20,-50,0] \
        -tx 0 -ty -50 \
        -linewidth 5 -pixflags c
create xshape III -fg black -coords [-80,85,0][-180,-85,0][-180,-85,0][-110,-85,0][-110,-85,0][-20,77,0][-20,77,0][0,40,0][0,40,0][20,77,0][20,77,0][110,-85,0][110,-85,0][180,-85,0][180,-85,0][80,85,0] \
        -tx 0 -ty -185 -textcolor brown  \
        -linewidth 5 -pixflags c



// inhibitory cells minus axons

create xsphere ff -fg blue  -r 30 -tx -220 -ty 360 -pixflags c
create xsphere fb -fg blue  -r 30 -tx 250 -ty -200 -pixflags c



// axons emanating from pyramidal cell

create xshape pyr_axon1 -fg black -coords [0,-150,0][0,-350,0][-50,-230,0][50,-230,0][0,-350,0][-351,-350,0][-350,-350,0][-350,-200,0][0,-350,0][352,-350,0][350,-350,0][350,-320,0] \
        -tx 0 -ty 0 -linewidth 3 -drawmode DrawSegments -pixflags c
create xshape pyr_axon_left1 -fg black -coords [-350,-180,0][-350,190,0][-350,70,0][-400,70,0] \
        -tx 0 -ty 0 -linewidth 3 -drawmode DrawSegs -pixflags c
create xshape pyr_axon_left2 -fg black -coords [-350,210,0][-350,290,0] \
        -tx 0 -ty 0 -linewidth 3 -drawmode DrawSegs -pixflags c
create xshape pyr_axon_left3 -fg black -coords [-350,310,0][-350,340,0][-351,340,0][-295,340,0] \
        -tx 0 -ty 0 -linewidth 3 -drawmode DrawSegs -pixflags c 
create xshape pyr_axon_right1 -fg black -coords [350,-300,0][350,60,0][350,-200,0][330,-200,0] \
        -tx 0 -ty 0 -linewidth 3 -drawmode DrawSegs -pixflags c
create xshape pyr_axon_right2 -fg black -coords [350,80,0][350,190,0][349,190,0][400,190,0] \
        -tx 0 -ty 0 -linewidth 3 -drawmode DrawSegs -pixflags c 



// afferent axons

create xshape Aff -fg black -coords [-400,450,0][400,450,0][-350,450,0][-350,380,0][-351,380,0][-295,380,0][-130,450,0][-130,370,0][-131,370,0][-80,370,0][250,450,0][250,110,0][250,30,0][250,-124,0] \
        -tx 0 -ty 0 -linewidth 3 -drawmode DrawSegs -pixflags c 



// association axons

create xshape CA_axon -fg black -coords [-180,-30,0][140,-30,0] \
        -tx -220 -ty 230 -linewidth 3 -drawmode DrawSegs -pixflags c 
create xshape RA_axon -fg black -coords [-170,-30,0][140,-30,0] \
        -tx 255 -ty 100 -linewidth 3 -drawmode DrawSegs -pixflags c 
create xshape LOCAL_axon -fg black -coords [-180,0,0][32,0,0] \
        -tx -220 -ty -190 -linewidth 3 -drawmode DrawSegs -pixflags c 



// axons emanating from inhibitory cells

create xshape fb_axon -fg black -coords [0,0,0][0,-70,0][-180,-70,0][140,-70,0][-55,-70,0][-55,200,0][-51,200,0][-147,200,0] \
        -tx 252 -ty -240 -linewidth 3 -drawmode DrawSegs -pixflags c 
create xshape ff_axon -fg black -coords [0,0,0][0,-20,0][-180,-20,0][140,-20,0] \
        -tx -220 -ty 320 -linewidth 3 -drawmode DrawSegs -pixflags c 



// excitatory synapses (left facing)

str pix_name

create xshape Aff_syn -coords \
        [0,0,0][35,23,0][35,-23,0] \
        -drawmode FillPoly  \
        -linewidth 3  -fg red -tx -82 -ty 370 \
	-script "xshow /x_interface/Aff_syn"
foreach pix_name (Aff_ff_syn fb_ff_syn CA_syn LOCAL_syn)
	copy Aff_syn {pix_name} 
end

setfield Aff_ff_syn tx -298 ty 380 text "" script "xshow /x_interface/Aff_ff_syn"
setfield fb_ff_syn tx -298 ty 340 text "" script "xshow /x_interface/fb_ff_syn"
setfield CA_syn tx -82 ty 200 text "" script "xshow /x_interface/CA_syn"
setfield LOCAL_syn tx -190 ty -190 text "" script "xshow /x_interface/LOCAL_syn"



// excitatory synapses (right facing)

create xshape RA_syn -coords \
        [0,0,0][-35,23,0][-35,-23,0] \
        -drawmode FillPoly   \
        -linewidth 3 -fg red -tx 85 -ty 70 -script "xshow /x_interface/RA_syn"
copy RA_syn pyr_fb_syn
setfield pyr_fb_syn tx 330 ty -200 text "" script "xshow /x_interface/pyr_fb_syn"



// excitatory feedforward synapse onto fb

create xshape Aff_fb_syn -coords [0,0,0][-23,-35,0][23,-35,0] \
        -drawmode FillPoly  -tx 250 -ty -121 \
        -linewidth 3 -fg red -script "xshow /x_interface/Aff_fb_syn"



// inhibitory synapses (both sides)

create xshape ff_syn -coords \
        [0,0,0][35,23,0][35,-23,0] \
        -drawmode FillPoly \
        -linewidth 3 -fg blue -tx -82 -ty 300 \
	-script "xshow /x_interface/ff_syn"
create xshape fb_syn -coords \
        [0,0,0][-35,23,0][-35,-23,0] \
        -drawmode FillPoly  \
        -tx 110 -ty -40 -linewidth 3 -fg blue \
	-script "xshow /x_interface/fb_syn"


// single arrows pointed towards the right

create xshape ff_arrow1 -coords [0,0,0][-10,10,0][-10,-10,0] \
        -drawmode FillPoly -linewidth 1 -pixflags c -fg black -tx -85 -ty 300

foreach pix_name (pyr_ff_arrow LOCAL_arrow1 LOCAL_arrow2 LOCAL_pyr_arrow1 fb_arrow)
	copy ff_arrow1 {pix_name}
end

setfield pyr_ff_arrow tx -305 ty 340 text ""
setfield LOCAL_arrow1 tx -195 ty -190 text ""
setfield LOCAL_arrow2 tx -390 ty -190 text ""
setfield LOCAL_pyr_arrow1 tx 53 ty -230 text ""
setfield fb_arrow tx 400 ty -310 text ""



// single arrows pointed towards the left

create xshape ff_arrow2 -coords [0,0,0][10,10,0][10,-10,0] \
        -drawmode FillPoly -linewidth 1 -pixflags c -fg black -tx -405 -ty 300

foreach pix_name (LOCAL_pyr_arrow2 fb_left_arrow1 fb_left_arrow2 pyr_fb_arrow)
	copy ff_arrow2 {pix_name}
end

setfield LOCAL_pyr_arrow2 tx -53 ty -230 text ""
setfield fb_left_arrow1 tx 67 ty -310 text ""
setfield fb_left_arrow2 tx 115 ty -40 text ""
setfield pyr_fb_arrow tx 334 ty -200 text ""



// double arrows pointed towards the right

create xshape Aff_right -coords [-20,10,0][-20,-10,0][-10,0,0][-10,-10,0][0,0,0][-10,10,0][-10,0,0] \
        -drawmode FillPoly -linewidth 1 -pixflags c -fg black -tx 403 -ty 450 

foreach pix_name (Aff_left Aff_ff Aff_Ia CA1 CA2 pyr_right2_arrow)
	copy Aff_right {pix_name}
end

setfield Aff_left tx -380 ty 450 text ""
setfield Aff_ff tx -305 ty 380 text ""  
setfield Aff_Ia tx -85 ty 370 text "" 
setfield CA1 tx -380 ty 200 text "" 
setfield CA2 tx -85 ty 200 text "" 
setfield pyr_right2_arrow tx 403 ty 190 text ""  



// double arrows pointed towards the left

create xshape RA1_arrows -coords [20,10,0][20,-10,0][10,0,0][10,-10,0][0,0,0][10,10,0][10,0,0] \
        -drawmode FillPoly -linewidth 1 -pixflags c -fg black -tx 94 -ty 70

foreach pix_name (RA2_arrows pyr_RA_arrows)
	copy RA1_arrows {pix_name}
end

setfield RA2_arrows tx 380 ty 70 text ""  
setfield pyr_RA_arrows tx -403 ty 70 text ""  



// double arrows pointed down

create xshape Aff_fb_arrows1 -coords [10,20,0][-10,20,0][0,10,0][-10,10,0][0,0,0][10,10,0][0,10,0] \
        -drawmode FillPoly -linewidth 1 -pixflags c -fg black -tx 251 -ty 251 
copy Aff_fb_arrows1 Aff_fb_arrows2
setfield Aff_fb_arrows2 tx 251 ty -115 text ""



// labels for varoius components of circuit

create xshape Aff_label -tx 0 -ty 470 -text "afferent LOT"
create xshape CA_label1 -tx -230 -ty 225 -text "caudally directed"
create xshape CA_label2 -tx -230 -ty 188 -text "association fibers"
create xshape LOCAL_label1 -tx -280 -ty -165 -text "local"
create xshape LOCAL_label2 -tx -270 -ty -205 -text "association"
create xshape RA_pyr_label -tx -200 -ty -370 -text "rostrally directed assoc."
create xshape CA_pyr_label -tx 200 -ty -370 -text "caudally directed assoc."
create xshape pyr_LOCAL_label -tx 0 -ty -250 -text "local   assoc."
create xshape RA_label1 -tx 220 -ty 95 -text "rostrally directed"
create xshape RA_label2 -tx 220 -ty 55 -text "association fibers"



// xbuttons

create xtoggle mode [1%,95%,18%,4%] -script "mode"
        setfield ^ label0 "SIMULATE" label1 "PLAYBACK"
create xbutton simulate [21%,95%,18%,4%] -title "simulate" -script "xshow /x_interface/simulate"
create xbutton playback [41%,95%,18%,4%] -title "playback" -script "xshow /x_interface/playback"
create xbutton help [61%,95%,18%,4%] -title "help" -script "xshow /x_interface/help"
create xbutton quit [81%,95%,18%,4%] -title "quit" -script "xshow /x_interface/quit"

create xbutton field_potential [77%,10%,21%,4%] -title "field potential" \
        -script "xshow /x_interface/field_pot"
create xbutton spike_count [77%,15%,21%,4%] -title "spike count"        \
        -script "xshow /x_interface/spike"
create xbutton side_view [77%,20%,21%,4%] -title "full cell view"       \
        -script "xshow /x_interface/full_cell_view"

create xshape Ia_menu -fg LightSteelBlue -coords [-25,20,0][25,20,0][25,20,0][25,-20,0][25,-20,0][-25,-20,0][-25,-20,0][-25,20,0] \
        -drawmode FillPoly  \
        -linewidth 3 -tx 0 -ty 330 -text " Ia " \
	-script "xshow /x_interface/Ia_options" 
create xshape supIb_menu -fg LightSteelBlue -coords [-30,20,0][30,20,0][30,20,0][30,-20,0][30,-20,0][-30,-20,0][-30,-20,0][-30,20,0] \
        -drawmode FillPoly  \
        -linewidth 3 -tx 0 -ty 200 -text " supIb " \
	-script "xshow /x_interface/supIb_options" 
create xshape deepIb_menu -fg LightSteelBlue -coords [-35,20,0][35,20,0][35,20,0][35,-20,0][35,-20,0][-35,-20,0][-35,-20,0][-35,20,0] \
        -drawmode FillPoly  \
        -linewidth 3 -tx 0 -ty 70 -text " deepIb " \
	-script "xshow /x_interface/deepIb_options" 
create xshape soma_menu -fg LightSteelBlue -coords [-50,20,0][50,20,0][50,20,0][50,-20,0][50,-20,0][-50,-20,0][-50,-20,0][-50,20,0] \
        -drawmode FillPoly  \
        -linewidth 3 -tx 0 -ty -65 -text " soma(II) " \
	-script "xshow /x_interface/soma_options" 
create xshape III_menu -fg LightSteelBlue -coords [-25,20,0][25,20,0][25,20,0][25,-20,0][25,-20,0][-25,-20,0][-25,-20,0][-25,20,0] \
        -drawmode FillPoly  \
        -linewidth 3 -tx 0 -ty -170 -text " III " \
	-script "xshow /x_interface/III_options" 


create xshape fb_menu -fg LightSteelBlue -coords [-25,20,0][25,20,0][25,20,0][25,-20,0][25,-20,0][-25,-20,0][-25,-20,0][-25,20,0] \
        -drawmode FillPoly  \
        -linewidth 3 -tx 250 -ty -202 -text " fb " -script "xshow /x_interface/fb_network"

create xshape ff_menu -fg LightSteelBlue -coords [-25,20,0][25,20,0][25,20,0][25,-20,0][25,-20,0][-25,-20,0][-25,-20,0][-25,20,0] \
        -drawmode FillPoly  \
        -linewidth 3 -tx -220 -ty 360 -text " ff " -script "xshow /x_interface/ff_network"



// create legend form

create xform /menu/legend [540,50,600,350]
create xdraw /menu/legend/draw [0,0,100%,90%] -wx 1000 -wy 600 -bg antiquewhite
ce /menu/legend/draw
create xlabel /menu/legend/label -label "LEGEND" [0,0,100%,10%] -bg pink
create xshape ex_synapse -coords \
        [0,0,0][35,23,0][35,-23,0] \
        -drawmode FillPoly  -tx -17 -ty 125 \
        -linewidth 3  -fg red -pixflags c
create xshape excitatory_synapse -tx 0 -ty 75 -text "excitatory synapse" -pixflags c
create xshape inhib_synapse -coords \
        [0,0,0][35,23,0][35,-23,0] \
        -drawmode FillPoly  -tx -17 -ty -75 \
        -linewidth 3  -fg blue -pixflags c
create xshape inhibitory_synapse -tx 0 -ty -125 -text "inhibitory synapse" -pixflags c
create xshape one_right_arrow -coords [0,0,0][-10,10,0][-10,-10,0] \
        -drawmode FillPoly -tx 300 -ty 125 \
        -linewidth 1 -fg black -pixflags c
create xshape local_fiber -tx 300 -ty 75 -text "local fibers" -pixflags c
create xshape two_right_arrow -coords [-20,10,0][-20,-10,0][-10,0,0][-10,-10,0][0,0,0][-10,10,0][-10,0,0] \
        -drawmode FillPoly -tx 300 -ty -75 \
        -linewidth 1 -fg black -pixflags c
create xshape distant_fiber -tx 300 -ty -125 -text "distant fibers" -pixflags c
create xshape inhibitory_cell -tx -300 -ty 75 -text "inhibitory cell" -pixflags c
create xshape pyramidal -coords [-5,60,0][5,60,0][5,0,0][30,-30,0][20,-30,0][0,-10,0][-20,-30,0][-30,-30,0][-5,0,0][-5,60,0] \
       -tx -300 -ty -65 -pixflags c
create xsphere ff -fg blue  -r 20 -tx -300 -ty 140 -pixflags c
create xshape pyramidal_cell -tx -300 -ty -125 -text "pyramidal cell" -pixflags c
create xbutton cancel [1%,90%,98%,10%] -title "cancel" -script "xhide /menu/legend"


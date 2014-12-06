//genesis
//

// here are some of the icons
create xshape  /form/draw/tree/shape[1] -coords \
	[-0.5,-0.5,0][0,0.5,0][0.5,-0.5,0][-0.5,-0.5,0] \
 -drawmode FillPoly -pixcolor maroon -value neutral -pixflags v

create xshape  /form/draw/tree/shape[2] -coords \
"[0.6,0.8,0][0.9,0.6,0][1,0.4,0][1,0.2,0][0.8,-0.2,0][-0.6,-0.4,0][-0.4,0,0] [-0.4,0.2,0][-0.5,0.4,0][-0.8,0.6,0][0.6,0.8,0][-0.8,0.6,0][-1,0.2,0][-1,0,0][-0.9,-0.2,0][-0.6,-0.4,0]"	\
 -drawmode DrawLines -pixcolor maroon -value compartment -pixflags v

 
create xshape /form/draw/tree/shape[3] -coords \
"[-0.2,-1,0][-0.2,-0.6,0][-1,-0.6,0][-0.2,-0.2,0][-0.8,-0.2,0] [-0.2,0.2,0][-0.6 ,0.2,0][-0.2,0.6,0][-0.4,0.6,0][0,1,0][0.4,0.6,0][0.2,0.6,0][0.6,0.2,0][0.2,0.2,0][0.8,-0.2,0][0.2,-0.2,0][-1,-0.6,0][0.2,-0.6,0][0.2,-1,0]" \
-drawmode FillPoly -pixcolor green -value xtree -pixflags v

 
create xshape  /form/draw/tree/shape[4] -coords  \
"[0.2,0.6,0][0.8,0.6,0][1,0.4,0][1,-1,0][0.4,-1,0][0.2,-0.8,0][0.2,0.6,0][0.4,0.7,0][0.4,0.9,0][0.2,1,0][-0.6,1,0][-1,0.6,0][-1,-1,0][-0.2,-1,0][-0.2,0.5,0][0.2,0.6,0]" \
 -drawmode DrawLines -pixcolor violet -value hh_channel -pixflags v
 

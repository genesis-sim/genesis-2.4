// genesis

// assigning the retinal parameters:

float	REC_SEPX = 40e-6   // separation of cells in x direction in meters
float	REC_SEPY = 40e-6   // separation of cells in y direction in meters
float	REC_NX = 10        // number of cells in x direction
float	REC_NY = 10        // number of cells in y direction
float   ret_xmin = {-REC_NX * REC_SEPX / 2}
float   ret_ymin = {-REC_NY * REC_SEPY / 2}
float   ret_xmax = {REC_NX * REC_SEPX / 2}
float   ret_ymax = {REC_NY * REC_SEPY / 2}

// creating the retina

create neutral /retina

// creating a receptor cell in the library 
// which is just a randomspike input module; rate is in spikes/sec 

if(!{exists /library})
  create neutral /library 
  disable /library 
end

create neutral     /library/rec
create randomspike /library/rec/input  
setfield ^ rate 1.0 abs_refract 0.001 

// creating a plane (map) of receptor cells on the retina
// based on the /library/rec prototype 
// Usage :
// createmap prototype destination 
// number_in_x_direction number_in_y_direction
// -delta separation_in_x_direction separation_in_y_direction
// -origin offset_in_x_direction offset_in_y_direction


createmap /library/rec /retina/recplane \
	        {REC_NX}   {REC_NY} \
		-delta  {REC_SEPX} {REC_SEPY}   \
		-origin	{-REC_NX * REC_SEPX / 2} {-REC_NY * REC_SEPY / 2}


useclock /retina/## 1
 







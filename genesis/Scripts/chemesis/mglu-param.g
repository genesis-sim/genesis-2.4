//Chemesis2.0
//units are microMolar, meter and sec, unless indicated

/* ######## volume assumed 1e-12 Liters (1e-15 m^3) corresponds to 10 um cubed ######### */
float Volume = 1e-15 
float lenUnits = 1 //length units are meters
float uM=1e-6

float K_mGluR = 4*{Volume}*{uM}*6e23 /*molecules*/
float K_G = 25.0
float K_PLC = 5.0
float K_PIP2 = 160 

// Kinetics of G protein activation and degradation
float D5f =15, D5b =7.2
float D6f =1.8
//Vmax2 = D6f Km2 = (D6f + D5B)/D5f, used with mmenz object
float Vmax2 =1.8, Km2 =0.6
float D7f = 9  /* aG decay, from Biddlecomb and Ross */

//kinetics of G_alpha binding to PLC
float G2f =100, G2b =100

//kinetics of IP3 production and degradation
/* values from Mitchell et al., Smrcka et al., Rack et al. */
float Vmax1 =0.58, kfplc=0.83, kbplc=0.1 





































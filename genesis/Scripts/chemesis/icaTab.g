//genesis
//icaTab.g
//The only units in this file are voltage, in Volts (SI)

function exp_form (rate, slope, V)
	float rate,slope,V
	//equation is ({rate} *(exp ({-V}/{slope}) ))
	float numx ={{-V}/{slope}}
	float expx = {exp {numx}}
	float entry = ({rate}*{expx})
	return {entry}
end

function sig_form (rate, vhalf, slope, V)
	float rate, vhalf, slope, V
	//equation is ({rate}/(exp ({{V}-{vhalf}}/{slope})+1))
	//rate/(EXP((v-vhalf)/slope)+1)
	float numx = {{{V}-{vhalf}}/{slope}}
	float expx = {exp {numx}}
	float entry = ({rate}/{{expx}+1})
	return {entry}
end

function lin_form (rate, vhalf, slope, V)

	float rate, vhalf, slope, V
	//equation is (({rate}*({V}-{vhalf}))/{exp ({v}-{vhalf}/{slope})-1)})
	float expx = {exp {{{V}-{vhalf}}/{slope}}} -1
	float numerator = {{rate}*{{V}-{vhalf}}}
	float entry = {{numerator}/{expx}}
	return {entry}
	// put in check for if v=vhalf then add 0.0001 or something. or... dtop/dbottom is L'Hopital.  
	
end

function make_CaChan (path, Actvhalf, Actslope, Acttau, Inactvhalf, Inactslope, Inacttau)

    str path  
    float Actvhalf
    float Actslope
    float Acttau
    float Inactvhalf
    float Inactslope
    float Inacttau

//Units are Volts
    float Erev = 0.060  /* reversal potential of calcium */
    float xmin = -0.100   /* minimum voltage we will see in the simulation */
    float xmax = 0.050   /* maximum voltage we will see in the simulation */
    int xdivs = 3000    /* the number of divisions between -0.1 and 0.05 */
    float step=({xmax}-{xmin})/{xdivs}
    int c = 0
    float vmemb

    create tabchannel {path}

    /* make the table for the activation with a range of -100mV - +50mV
     * with an entry for ever 5mV
     */
    call {path} TABCREATE X {xdivs} {xmin} {xmax}
    call {path} TABCREATE Y {xdivs} {xmin} {xmax}

    for(c = 0; c <= {xdivs}; c = c + 1)
        vmemb={{c * {step}} + xmin}
        setfield {path} X_A->table[{c}] {Acttau}
        setfield {path} X_B->table[{c}] {sig_form 1 {Actvhalf} {Actslope} {vmemb}}
        setfield {path} Y_A->table[{c}] {Inacttau}
        setfield {path} Y_B->table[{c}] {sig_form 1 {Inactvhalf} {Inactslope} {vmemb}}
    end

    tweaktau {path} X
    tweaktau {path} Y

    setfield {path} Ek {Erev} Xpower 2 Ypower 1

end

/******************************************************************/

function make_ghk(CaPath,channame)
    str CaPath, channame
//NOTE: ghk expects temperaturein celsius
    create ghk {CaPath}/{channame}_ghk
    setfield {CaPath}/{channame}_ghk valency 2.0 T 20 
end

   /*******************************************************************/

function make_ica_ghk (vpath, capath, caextpath, channame, gmax, Actvhalf, Actslope, Acttau, Inactvhalf, Inactslope, Inacttau)
str vpath, capath, caextpath, channame
float gmax 
    float Actvhalf
    float Actslope
    float Acttau
    float Inactvhalf
    float Inactslope
    float Inacttau

echo "ca chan params" {capath} {Actvhalf} {Actslope} {Acttau} {Inactvhalf} {Inactslope} {Inacttau}	
   make_CaChan {capath}/{channame} {Actvhalf} {Actslope} {Acttau} {Inactvhalf} {Inactslope} {Inacttau}	
   make_ghk {capath} {channame}	/* make GHK Ica */

    float area
    if ({CHEMESIS})
       area = {getfield {capath} SAout}
    else
       //area = {getfield {capath} surf_up}
       //surf up is the diffusion surface, not the outer membrane surface
       area = {PI}*{getfield {capath} dia} * {getfield {capath} thick}
    end
    echo "area" {area} 
    setfield {capath}/{channame} Gbar {gmax*area}

/* voltage messages to Ica and GHK-Ica */
   addmsg {vpath} {capath}/{channame} VOLTAGE Vm
   addmsg {vpath} {capath}/{channame}_ghk VOLTAGE Vm

/* ext & int calcium concentration to CHK */
if ({CHEMESIS})
   addmsg {caextpath} {capath}/{channame}_ghk Cout Conc
   addmsg {capath} {capath}/{channame}_ghk Cin Conc
else
   addmsg {caextpath} {capath}/{channame}_ghk Cout z
   addmsg {capath} {capath}/{channame}_ghk Cin C
end

/* open fraction from ica to GHK ica = Gk  */
   addmsg {capath}/{channame} {capath}/{channame}_ghk PERMEABILITY Gk

/* send psuedoG and Vr to voltage compartment */
   addmsg {capath}/{channame}_ghk {vpath} CHANNEL Gk Ek

/* send current and charge to concentration pool */
if ({CHEMESIS})
   addmsg {capath}/{channame}_ghk {capath} CURRENT valency Ik
else
   addmsg {capath}/{channame}_ghk {capath} INFLUX Ik
end

end




//genesis - hhchan_K.g - creates an extended object for a H-H K channel

// Define some constants to define the form of the rate constant equation
int EXPONENTIAL =   1
int SIGMOID     =   2
int LINOID      =   3

//  Original Hodgkin-Huxley squid parameters
float EREST_ACT = -0.070
float EK = -0.082
	
create		hh_channel	K_squid_hh
setfield K_squid_hh \
        Ek              {EK} \                          // V
        Gbar            0.0  \
        Xpower          4.0 \
        Ypower          0.0 \
        X_alpha_FORM    {LINOID} \
        X_alpha_A       -10.0e3 \                       // 1/V-sec
        X_alpha_B       -10.0e-3 \                      // V
        X_alpha_V0      {10.0e-3+EREST_ACT} \           // V
        X_beta_FORM     {EXPONENTIAL} \
        X_beta_A        125.0 \                         // 1/sec
        X_beta_B        -80.0e-3 \                      // V
        X_beta_V0       {0.0+EREST_ACT}                 // V

addfield 	K_squid_hh 	gdens		// conductance density
setfield 	K_squid_hh 	gdens		360.0   // S/m^2

setfieldprot K_squid_hh -hidden  Xpower Ypower X_alpha_FORM \
    X_alpha_A X_alpha_B X_alpha_V0 X_beta_FORM X_beta_A \
    X_beta_B X_beta_V0 Y_alpha_FORM Y_alpha_A Y_alpha_B \
    Y_alpha_V0 Y_beta_FORM Y_beta_A Y_beta_B Y_beta_V0 
setfieldprot K_squid_hh -readonly Gbar 

function K_squid_hh_SET(action, field, newvalue)
    float newvalue
    float PI = 3.14159
    if (field == "gdens")
        setfield . Gbar {PI*{getfield .. dia}*{getfield .. len}*newvalue}
    end
    return 0	// indicate that SET action isn't yet complete
end
addaction K_squid_hh SET K_squid_hh_SET

function K_squid_hh_CREATE(action, parent, object, elm)
    float PI = 3.14159
    if (!{isa compartment ..})
        echo K_squid_hh must be the child of a compartment
        return 0
    end
    setfield . Gbar {PI*{getfield .. dia}*{getfield .. len}*{getfield . gdens}}
    addmsg . .. CHANNEL Gk Ek
    addmsg .. . VOLTAGE Vm
    return 1
end
addaction K_squid_hh CREATE K_squid_hh_CREATE

addobject K_squid_hh K_squid_hh -author "J. R. Hacker" \
    -description "Hodgkin-Huxley Active K Squid Channel - SI units"

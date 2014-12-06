//genesis
/*
******************************
**         STARTUP          **
******************************
*/

// set the prompt
setprompt "piriform !"
// set the basic simulation step size
setclock 0 0.1
// set the output step size
setclock 1 1.0
// set the alternate output step size
setclock 2 2.0
randseed 234567

int ACTIVE = 0
int MULTI = 1
int CONNECT = 1
int INTERNEURON = 1
int SPIKE = 1 && !ACTIVE
int BULB = 1
int EXTRA = 0
int FIELD = 1
int T_NOISE = 0
int LOCAL_FF = 1
int DISINHIB = 1

echo PIRIFORM CORTEX SIMULATION CONFIGURATION
echo ----------------------------------------
if (ACTIVE)
    echo Hodgkin-Huxley
else
    echo integrate and fire
end

if (MULTI)
    echo multi-compartmental pyramidal cells (soma + dendrite)
else
    echo single compartment pyramidal cells (soma)
end

if (SPIKE)
    echo using spike conductances
end

if (!SPIKE && !ACTIVE)
    echo using simple spikes
end


if (INTERNEURON)
    echo with interneurons
else
    echo without interneurons
end

if (CONNECT)
    echo with synaptic connections
else
    echo without synaptic connection
end

echo ----------------------------------------


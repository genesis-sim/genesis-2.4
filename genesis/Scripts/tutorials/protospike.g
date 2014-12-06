// genesis

/* FILE INFORMATION
**
**  Function to create prototype "spikegen" element "spike"
**  with unit amplitude and 0.010 second refractory period
**
**  GENESIS 2.0
*/

function make_spike
        if ({exists spike})
                return
        end

        create spikegen spike
        setfield spike \
                thresh  0.00 \         // V
                abs_refract     10e-3 \ // sec
                output_amp      1
end

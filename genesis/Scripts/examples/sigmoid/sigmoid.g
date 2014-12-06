//genesis - example of the sigmoid object

// create a function generator to generate input to the sigmoid
create funcgen /funky
setfield /funky amplitude 100 dc_offset 0 frequency 0.005 \
	mode 2 //triangle wave

// create and display a graph inside a form
create xform /data
create xgraph /data/voltage [0,0,100%,70%] 
xshow /data

// send a message (PLOT Vm) to the graph
addmsg /funky /data/voltage PLOT output *input *red

// now make a sigmoid element to receive input from the compartment
create sigmoid /sigmoid

/*
The sigmoid object has the fields:

        input        state        amplitude        thresh        gain

The equation being applied is

        state = amplitude*(tanh(gain*(input - thresh)) + 1)/2.0;

The valid messages to the the sigmoid are:

	INPUT	input
	THRESH	threshold
	GAIN	gain
	AMP	amplitude

The first of these is used to provide input to the sigmoid, and the other
three provide a way to set the parameter fields with messages.

In this example, we want to choose parameters which will give the tanh
function a moderate rate of increase over the range of the input and
a non-linear output which covers the same range as the input.
*/

setfield sigmoid amplitude 100 gain 0.05 thresh 50
addmsg /funky /sigmoid INPUT output
addmsg /sigmoid /data/voltage PLOT state *sigmoid *black

function setfreq
   setfield /sigmoid gain {getfield /data/GAIN value}
end

// make some buttons to execute simulation commands
create xbutton /data/RESET -script reset
create xbutton /data/RUN -script "step 100"
create xbutton /data/QUIT -script quit
create xdialog /data/GAIN -value 0.05 -script setfreq

reset

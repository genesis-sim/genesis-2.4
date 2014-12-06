//genesis - testexample.g - tests the new object "example_object"

create example_object /sum_inputs
create neutral /input           // source of inputs
setfield /input x 5 y 3
addmsg /input /sum_inputs ADD x
addmsg /input /sum_inputs SUBTRACT y
addmsg /input /sum_inputs TWOARGS x y

reset
echo {getfield /sum_inputs output}
step
echo {getfield /sum_inputs output}
step
echo {getfield /sum_inputs output}
step
echo {getfield /sum_inputs output}

call /sum_inputs NEWACTION

//genesis

/*
The table object is also a very good function generator.  Here is an example
script, submitted by Upi Bhalla, which shows how to use the "step_mode"
option to read out table values with each clock tick, permitting the table
to act as a generator for any arbitrary function.     
*/


float PI = 3.14159
float dx = PI/50.0
float x = 0.0
float y
int i = 0


create table /tab
setfield /tab step_mode 1 stepsize 1

call /tab TABCREATE 100 0 100

for (i = 0; i < 100; i = i + 1)
        y = {sin {x}}
        setfield /tab table->table[{i}] {y}
        x = x + dx
end

create xform /form
create xgraph /form/graph
        setfield /form/graph xmax 200 ymax 1.0 ymin -1.0 bg ivory
create xbutton /form/RESET -script reset
create xbutton /form/RUN -script "step 200"
create xbutton /form/QUIT -script quit

xshow /form
addmsg /tab /form/graph PLOT output *sine *red

reset

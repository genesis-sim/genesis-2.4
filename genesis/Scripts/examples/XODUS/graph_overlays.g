//genesis
// graph_overlays example
// By Upinder S. Bhalla Mt. Sinai School of Medicine 1995.
// This example tests the overlays and the labels on the axes and graph
// Note how I create one plot (foo) explicitly, and the others
// implicitly by sending msgs.
create xform /form [1,1,500,800] -title "OUTPUT" -fg black
ce /form
function do_runit
    step 1000
end
create xbutton runit
    setfield runit script do_runit
create xbutton reset -wgeom 50% -script reset
create xbutton quit -ygeom 0:runit -xgeom 50% -wgeom 50% \
    -script do_quit
function do_quit
    quit
end
create xgraph graph
ce graph
    setfield hgeom 50%
    setfield script "echo in graph" xmin 0 xmax 100 ymin -1 ymax 10
    setfield yoffset 2
    setfield xmax 2500
    setfield overlay 1

int i
create xplot foo -script "echo hello you clicked on <w>"
    ce foo
    for(i = 0; i < 200 ; i = i + 1)
        setfield xpts->table[{i}] {i}
        setfield ypts->table[{i}] {sin {i/10.0} }
    end
    setfield npts 200 fg blue wx 1 wy 1
    //setfield xmin 0 xmax 100 ymin -1 ymax 1 wx 1 wy 1
    ce ..
create table /tab
    call /tab TABCREATE 250 0 250
    setfield /tab step_mode 1 stepsize 1
    for(i = 0; i < 200 ; i = i + 1)
        setfield /tab table->table[{i}] {cos {i/10.0} }
    end
create table /tab2
    call /tab2 TABCREATE 250 0 250
    setfield /tab2 step_mode 1 stepsize 1
    for(i = 0; i < 200 ; i = i + 1)
        setfield /tab2 table->table[{i}] {sin {i/10.0} }
    end
addmsg /tab /form/graph PLOT output *output *yellow
addmsg /tab2 /form/graph PLOT output *output *green
create xtext /form/text -hgeom 30%
xtextload /form/text \
"This example tests the overlays and the labels on the" \
"axes and graph. Hit the 'runit' button."\
"Then hit 'reset' and note how the old plots move up." \
"If you hit 'runit' again a new plot will be generated."\
"Click on the 'foo' label to invoke the script function"\
"associated with it."\
"Click and drag the ends of the axes to change the scale."\
"Typing 'a' (for 'all') in the graph will scale to fit."

reset
xshow /form

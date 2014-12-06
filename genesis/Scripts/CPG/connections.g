//genesis connections.g

/* 
	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
 	connection_form		(graphicspath,path,num,x,y)
        makeconnections         (path)
=============================================================================
*/


// ==========================================================================
//            create a connection information display
// ==========================================================================
function connection_form(formpath, cellpath, excsyn, inhsyn, x, y)
    str formpath, cellpath, excsyn, inhsyn, doodle
    float x, y, temp
    str subformpath = (formpath) @ "/conn" @ (excsyn)
    float inhweight = {getweight {cellpath}/spike {inhsyn}}  // in tools.g
    float excweight = {getweight {cellpath}/spike {excsyn}}
    str type, temp1, temp2

    temp1 = {substring {cellpath} 1 1}
    temp2 = {substring {cellpath} 2 2}
    if ((excsyn) == "5")
        if ((temp1) == "L")
           temp1 = "R"
        else
           temp1 = "L"
        end
        if ((temp2) == "1")
           temp2 = "2"
        else
           temp2 = "1"
        end
    else
        if ((excsyn) == "3")
            if ((temp1) == "L")
                temp1 = "R"
            else
                temp1 = "L"
            end
         else
            if ((temp2) == "1")
                 temp2 = "2"
            else
                 temp2 = "1"
            end
        end
    end
    doodle = (cellpath) @ "to" @ (temp1) @ (temp2) @ "conn"
    type = "connection to " @ (temp1) @ (temp2)
    create xlabel {subformpath} [{x + 5},{y},95%,25] -title {type}
    if ({inhweight} == 0)
       temp = 0
    else
       temp = 1
    end
    create xtoggle {subformpath}/syntype [{x},{y + 30},95%,25] -script  \
        "synapsetoggle <widget> "{cellpath}" "{inhsyn}" "{doodle}
    setfield {subformpath}/syntype state {temp}
    setfield {subformpath}/syntype label0 "EXCITATORY"
    setfield {subformpath}/syntype label1 "INHIBITORY"
    create xdialog {subformpath}/weight [{x},{y + 60},95%,30] -script  \
        "createconn "{cellpath}" "{inhsyn}" <widget> "{subformpath}"/syntype" \
         -title "weight (syn)" -value {max {excweight} {inhweight}}
 end


 // ==========================================================================
 //    place the connection information displays in the connection form
 // ==========================================================================
function makeconnections(cellpath)
    str cellpath, temp1, temp2
    str  \
        togglepath = "/output/connections/" @ ({getpath {cellpath} -tail})
    str connectionspath = (cellpath) @ "/connections"

    // add a toggle for this cell in the main cell control panel
    maketoggle {cellpath} {togglepath} {connectionspath}

    // =================================================
    //          create connections form
    // =================================================
    create xform {connectionspath} [855,85,210,380] -title  \
        {connectionspath} 
    pushe {connectionspath}
    create xtoggle done [5,20,95%,25] -script  \
        "donetoggle <widget> "{togglepath}" "{connectionspath}
    setfield done label1 "VISIBLE"
    setfield done label0 "HIDDEN"

    // =================================================
    //       create buttons for changing parameters 
    // ================================================
        connection_form {connectionspath} {cellpath} 1 0 5 55
        connection_form {connectionspath} {cellpath} 3 2 5 160
        connection_form {connectionspath} {cellpath} 5 4 5 265
     pope
end

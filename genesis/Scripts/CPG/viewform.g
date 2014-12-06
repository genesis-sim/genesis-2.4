//genesis viewform.g

/*
             functions defined in this script
=============================================================================
      FUNCTION                  ARGUMENTS
============================================================================
    showem                   (path)
    connection_doodle        (name,x_coord1,y_coord1,x_coord2,y_coord2,synapse)
    soma_doodle              (path,x_coord,y_coord)
    connupdate
    createconn               (excconn,inhconn,widget)
    makeviewform
============================================================================
*/

// =========================================================================
//        show the information associated with the cell clicked on
// =========================================================================
function showem(value)
     str graphicspath, settingspath, inputspath
     str connectionspath
     str value, temp

     if (({substring {value} 4 7}) == "soma")
         temp = {substring {value} 1 2}
         graphicspath = "/" @ (temp) @ "/graphics"
         settingspath = "/" @ (temp) @ "/settings"
         inputspath = "/" @ (temp) @ "/inputs"

         setfield {graphicspath}/done state 1
         setfield /output/graphics/{temp} state 1
         xshow {graphicspath}

         setfield {settingspath}/done state 1
         setfield /output/settings/{temp} state 1
         xshow {settingspath}

         setfield {inputspath}/done state 1
         setfield /output/inputs/{temp} state 1
         xshow {inputspath}
 
     else     if (({substring {value} 6 9}) == "conn")
               temp = {substring {value} 0 1}
               connectionspath = "/" @ (temp) @ "/connections"
               setfield {connectionspath}/done state 1
               setfield /output/connections/{temp} state 1
               xshow {connectionspath}
          end
     end
  end

  // =========================================================================
  //                create connection symbol in the viewform
  // =========================================================================
function connection_doodle(path, x1, y1, x2, y2, excsyn, inhsyn)
     str path, excsyn, inhsyn, color
     float x1, y1, x2, y2, x3, y3, x4, y4, dx, dy
     str src_elem =  "/" @ {substring {path} 0 1} @ "/spike"
     float inhweight = {getweight {src_elem} {inhsyn}}  // in tools.g
     float excweight = {getweight {src_elem} {excsyn}}

   if ({exists /viewform/draw/{path}})
        if (({excweight} == 0) && ({inhweight} == 0))
              delete /viewform/draw/{path}
        end
   else
        if (({excweight} != 0) || ({inhweight} != 0))
             dx = (x2) - (x1)
             dy = (y2) - (y1)
             if ((dy) == 0)
                  x3 = (x2)
                  x4 = (x2)
                  y3 = (y2) + 0.1
                  y4 = (y2) - 0.1
             else
                  if ((dx) == 0)
                       x3 = (x2) + 0.1
                       x4 = (x2) - 0.1
                       y3 = (y2)
                       y4 = (y2)
                  else
                       if ((dx) < 0)
                            dx = -1
                       else
                            dx = 1
                       end
                       if ((dy) < 0)
                            dy = -1
                       else
                            dy = 1
                       end
                       x3 = (dx)*0.73
                       x4 = (dx)*0.59
                       y3 = (dy)*0.44
                       y4 = (dy)*0.58
                  end
            end
            if ({excweight} == 0)
                  color = "orange"
            else
                  color = "green"
            end
            create xshape /viewform/draw/{path} -linewidth 9 -fg {color} \
               -linestyle LineSolid \
	       -joinstyle JoinBevel \
               -script "showem.d1 <v>" -value {path} \
               -coords [{x1},{y1},0][{x2},{y2},0][{x3},{y3},0][{x4},{y4},0]
        end
    end
 end

 // =========================================================================
 //                   create cell symbol in the viewform
 // =========================================================================
function soma_doodle(path, x_coord, y_coord)
     str path
     float x_coord, y_coord
     float radius, dtheta, x, y
     int i
     create xvar /viewform/draw/{path}  \
         -value_min -90 -value_max 70   \
         -varmode shape -color_val 1 -morph_val 0 -script "showem.d1 <v>"

     pushe /viewform/draw/{path}

     setfield . tx {getfield /{path}/soma x}  ty {getfield /{path}/soma y}
     addmsg /{path}/soma .  VAL1 Vm

     // create circle for icon
     dtheta = 3.14159/10
     radius = 0.5
     for(i=0;i<=20;i=i+1)
         x = radius*{cos {i*dtheta}}
         y = radius*{sin {i*dtheta}}
         setfield shape[0] xpts->table[{i}] {x}
         setfield shape[0] ypts->table[{i}] {y}
         setfield shape[1] xpts->table[{i}] {x}
         setfield shape[1] ypts->table[{i}] {y}
     end
     setfield shape[0] npts 20
     setfield shape[1] npts 20
     pope

//     echo {getfield /viewform/draw/{path} path}
     useclock /viewform/draw/{path} 2
     create xshape /viewform/draw/{path}_toggle -text {path} -tx  \
         {x_coord} -ty {y_coord}  -textfont 9x15bold
     end

 //===========================================================================
 //      updates the connection doodles in the viewform 
 //===========================================================================
function connupdate
     connection_doodle R1toL1conn .53 .9 -.4 .9 3 2
     connection_doodle R1toR2conn 1.1 .53 1.1 -.4 1 0
     connection_doodle R2toR1conn .9 -.53 .9 .4 1 0
     connection_doodle R2toL2conn .53 -1.1 -.4 -1.1 3 2
     connection_doodle L2toR2conn -.53 -.9 .4 -.9 3 2
     connection_doodle L2toL1conn -1.1 -.53 -1.1 .4 1 0
     connection_doodle L1toL2conn -.9 .53 -.9 -.4 1 0
     connection_doodle L1toR2conn -.6 .75 .65 -.5 5 4
     connection_doodle R2toL1conn .6 -.75 -.65 .5 5 4
     connection_doodle R1toL2conn .6 .75 -.65 -.5 5 4
     connection_doodle L2toR1conn -.6 -.75 .65 .5 5 4
     connection_doodle L1toR1conn -.53 1.1 .4 1.1 3 2
end

//==========================================================================
// creates connection doodle if necessary and changes appropriate weight
//==========================================================================
function createconn(cellpath, inhsyn, widget, type)
   str cellpath, inhsyn, widget, type
   if (({getfield {type} state}) == 0) // excitatory
       setweight  {cellpath}/spike {inhsyn + 1}  {getfield {widget} value}
   else
       setweight  {cellpath}/spike {inhsyn}  {getfield {widget} value}
   end
   connupdate
   xupdate /viewform/draw
end

// ==========================================================================
//      create viewform, put in graphics, and show buttons
// ==========================================================================
function makeviewform
     //use slow clock for updates
     setclock 2 1
     create xform /viewform [5,180,455,490] -nolabel
     create xdraw /viewform/draw [2%,2%,96%,88%] -wx 4.5 -wy 4.5 \
	-cx 0 -cy 0 -bg white 
     create xlabel /viewform/label  [2%,90%,96%,10%] -title  \
         "propagation of network activation"
     useclock /viewform/draw 2

     soma_doodle L1 -1.75 1
     soma_doodle R1 1.75 1
     soma_doodle L2 -1.75 -1
     soma_doodle R2 1.75 -1
     connupdate
     xshow /viewform
end

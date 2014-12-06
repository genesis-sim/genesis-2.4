// genesis defaults.g

/*

==========================================================================
               FUNCTION                 ARGUEMENTS
==========================================================================
          makesystem
          setdefaults
          resetchannels                 (cellpath) 
          resetdefaults
===========================================================================
*/


//========================================================================
//                  create the cells and connections
//========================================================================
function makesystem
  //========================================================================
  //                            create L1 cell
  //========================================================================
  makeneuron /L1 30e-4 30e-4 100e-4 2e-4
  position /L1 -1 1 I

  //========================================================================
  //                            create L2 cell
  //========================================================================
  makeneuron /L2 30e-4 30e-4 100e-4 2e-4
  position /L2 -1 -1 I

  //========================================================================
  //                            create R1 cell
  //========================================================================
  makeneuron /R1 30e-4 30e-4 100e-4 2e-4
  position /R1 1 1 I

  //========================================================================
  //                            create R2 cell
  //========================================================================
  makeneuron /R2 30e-4 30e-4 100e-4 2e-4
  position /R2 1 -1 I

  //========================================================================
  //                  create connections from L1 and set values
  //========================================================================
  //descend  inh 0
  addmsg /L1/spike /L2/soma/dend/K_channel SPIKE

  //descend  exc 1
  addmsg /L1/spike /L2/soma/dend/Na_channel SPIKE

  //lateral  inh 2
  addmsg /L1/spike /R1/soma/dend/K_channel SPIKE

  //lateral  exc 3
  addmsg /L1/spike /R1/soma/dend/Na_channel SPIKE

  //diagonal inh 4
  addmsg /L1/spike /R2/soma/dend/K_channel SPIKE

  //diagonal exc 5
  addmsg /L1/spike /R2/soma/dend/Na_channel SPIKE

  //=========================================================================
  //                 create connections from R1 and set values
  //=========================================================================

  //descend  inh 0
  addmsg /R1/spike /R2/soma/dend/K_channel SPIKE

  //descend  exc 1
  addmsg /R1/spike /R2/soma/dend/Na_channel SPIKE

  //lateral  inh 2
  addmsg /R1/spike /L1/soma/dend/K_channel SPIKE

  //lateral  exc 3
  addmsg /R1/spike /L1/soma/dend/Na_channel SPIKE

  //diagonal inh 4
  addmsg /R1/spike /L2/soma/dend/K_channel SPIKE

  //diagonal exc 5
  addmsg /R1/spike /L2/soma/dend/Na_channel SPIKE

  //=========================================================================
  //                   create connections from L2 and set values
  //=========================================================================
  //ascend   inh 0
  addmsg /L2/spike /L1/soma/dend/K_channel SPIKE

  //ascend   exc 1
  addmsg /L2/spike /L1/soma/dend/Na_channel SPIKE

  //lateral  inh 2
  addmsg /L2/spike /R2/soma/dend/K_channel SPIKE

  //lateral  exc 3 
  addmsg /L2/spike /R2/soma/dend/Na_channel SPIKE

  //diagonal inh 4
  addmsg /L2/spike /R1/soma/dend/K_channel SPIKE

  //diagonal exc 5
  addmsg /L2/spike /R1/soma/dend/Na_channel SPIKE

  //=========================================================================
  //                  create connections from R2 and set values
  //=========================================================================
  //ascend   inh 0
  addmsg /R2/spike /R1/soma/dend/K_channel SPIKE

  //ascend   exc 1
  addmsg /R2/spike /R1/soma/dend/Na_channel SPIKE

  //lateral  inh 2
  addmsg /R2/spike /L2/soma/dend/K_channel SPIKE

  //lateral  exc 3
  addmsg /R2/spike /L2/soma/dend/Na_channel SPIKE

  //diagonal inh 4
  addmsg /R2/spike /L1/soma/dend/K_channel SPIKE

  //diagonal exc 5
  addmsg /R2/spike /L1/soma/dend/Na_channel SPIKE

end


//===========================================================================
//                       set the default parameter values
//===========================================================================
function setdefaults

  // set all delays to 15 msec
  planardelay /L1/spike -fixed 15
  planardelay /L2/spike -fixed 15
  planardelay /R1/spike -fixed 15
  planardelay /R2/spike -fixed 15

  //diagonal exc R2
  setweight /L1/spike 5  100
  //diagonal inh R2
  setweight /L1/spike 4  0
  //lateral  exc R1
  setweight /L1/spike 3  0
  //lateral  inh R1
  setweight /L1/spike 2  100
  //descend  exc L2
  setweight /L1/spike 1  0
  //descend  inh L2
  setweight /L1/spike 0  100

  //diagonal exc L2
  setweight /R1/spike 5  100
  //diagonal inh L2
  setweight /R1/spike 4  0
  //lateral  exc L1
  setweight /R1/spike 3  0
  //lateral  inh L1
  setweight /R1/spike 2  100
  //descend  exc R2
  setweight /R1/spike 1  0
  //descend  inh R2
  setweight /R1/spike 0  100

  //diagonal exc R1
  setweight /L2/spike 5  0
  //diagonal inh R1
  setweight /L2/spike 4  100
  //lateral  exc R2
  setweight /L2/spike 3  0
  //lateral  inh R2
  setweight /L2/spike 2  100
  //ascend   exc L1
  setweight /L2/spike 1  100
  //ascend   inh L1
  setweight /L2/spike 0  0

  //diagonal exc L1
  setweight /R2/spike 5  0
  //diagonal inh L1
  setweight /R2/spike 4  100
  //lateral  exc L2
  setweight /R2/spike 3  0
  //lateral  inh L2
  setweight /R2/spike 2  100
  //ascend   exc R1
  setweight /R2/spike 1  100
  //ascend   inh R1
  setweight /R2/spike 0  0

end


//==========================================================================
//                reset dendrite channel defaults                          
//==========================================================================
function resetchannels(path)
   //cellpath
   str path
   setfield {path}/Na_channel Ek 45
   setfield {path}/Na_channel gmax 80e-9
   setfield {path}/Na_channel tau1 3
   setfield {path}/Na_channel tau2 3

   setfield {path}/K_channel Ek -82
   setfield {path}/K_channel gmax 40e-9
   setfield {path}/K_channel tau1 10
   setfield {path}/K_channel tau2 10
end

//=========================================================================
//  reset cell parameters to default values, delete old windows, make new ones
//=========================================================================
function resetdefaults

   delete /L1/settings
   delete /L2/settings
   delete /R1/settings
   delete /R2/settings
   delete /L1/connections
   delete /L2/connections
   delete /R1/connections
   delete /R2/connections
   delete /L1/inputs
   delete /L2/inputs
   delete /R1/inputs
   delete /R2/inputs
   delete /output/connections
   delete /output/settings
   delete /output/inputs
   delete /viewform
   setdefaults
   resetchannels /L1/soma/dend
   resetchannels /L2/soma/dend
   resetchannels /R1/soma/dend
   resetchannels /R2/soma/dend

   create xform /output/settings [125,5,100,145] -title Settings
   xshow /output/settings
   create xform /output/connections [245,5,100,145] -title Connections
   xshow /output/connections
   create xform /output/inputs [365,5,100,145] -title Inputs
   xshow /output/inputs
   makesettings /L1
   makesettings /L2
   makesettings /R1
   makesettings /R2
   makeconnections /L1
   makeconnections /L2
   makeconnections /R1
   makeconnections /R2
   makeinputs /L1 0
   makeinputs /L2 5
   makeinputs /R1 10
   makeinputs /R2 15
   makeviewform
\\   check
   colorize
   reset
   echo Simulation reset to default values.
end

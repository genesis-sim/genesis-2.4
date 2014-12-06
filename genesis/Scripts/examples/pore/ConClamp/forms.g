//genesis
//*************************************************************************
//Adam Fraklin Strassberg
//March 15, 1992

//Simulation Interface and Control
//*************************************************************************



//*************************************************************************

//Miscellaneous Functions

//Sets the field within the path to the value of the dialog
function dset(path, field, dialog)
    float val
    val = {getfield {dialog} value}
    echo Setting {path} {field} {val}
    setfield {path} {field} {val}
end

//Sets the Pulse Interval of the Current Pulse
function set_pulse_interval(path, dialog)
    float interval, delay1, delay2
    interval = {getfield {dialog} value}
    delay1 = {getfield {path} delay1}
    delay2 = interval - delay1
    echo Setting {path} delay2 {delay2}
    setfield {path} delay2 {delay2}
end

//Dummy Function - needed to satisfy certain script fields
function void
end
//*************************************************************************



//*************************************************************************
//Reset Function - intializes the simulation

function sim_reset

//Set the output data file

float area
float numNa
float numK
float KGmax
float NaGmax
float Cm_sq
float Gleak_squid
float Vleak_squid
str Name

Name = {getfield /forms/control/fname value}
echo
echo Ouput File is {Name}

//Set the output data file
if (!{exists {Name}})
   create asc_file {Name}
   setfield {Name} flush 1  leave_open 1
   addmsg /patch {Name} SAVE Vm
end

area = {getfield /data/Area value}
numK = {getfield /data/Kdense value}*area
numNa = {getfield /data/Nadense value}*area
KGmax = {getfield /data/KGopen value}
NaGmax = {getfield /data/NaGopen value}
Cm_sq = {getfield /data/Cm_squid value}
Gleak_squid = {getfield /data/Gleak_squid value}
Vleak_squid = {getfield /data/Vleak_squid value}

//Initialize the Patch
setfield /patch Cm {Cm_sq*area}
setfield /patch Rm {1/(Gleak_squid*area)}
setfield /patch Em {Vleak_squid}
setfield /patch Vm {Vrest_squid}

//Initialize the K+ (N) channels 
setfield /patch/K gbar {KGmax*numK}
setfield /patch/Na gbar {NaGmax*numNa}


//Reset the system

reset

//Hack to accomplish proper reset scheduling (could use initVm field instead)
call /pulse RESET

call /patch RESET
setfield /patch Vm {Vrest_squid}

call /patch/Na/m RESET
call /patch/Na/h RESET
call /patch/Na RESET

call /patch/K/n RESET
call /patch/K RESET


end
//*************************************************************************



//*************************************************************************
//Run Functions - run the simulation

//Set the simulation clock to dt
function set_dt
    float dt
    dt = {getfield /forms/control/dt value}
    echo dt = {dt} msec
    //dt must be less than the graphic clock time step 
    //or else we get a core dump
    if (dt > 0.5)
	setfield /forms/control/dt value 0.5
        dt = 0.5
        echo You can't go faster than the graphics clock!
    end
    setclock 0 {dt}
end

//Run the simulation for totaltime/dt steps
function sim_step
    int nstep
    float dt, tstep
    set_dt
    tstep = {getfield /forms/control/tstep value}
    dt = {getfield /forms/control/dt value}
    nstep = tstep/dt
    step {nstep}
   //Delete the output file link when finished
   delete {getfield /forms/control/fname value}
end
//*************************************************************************



//*************************************************************************
//Create Simulation Control Interface

create neutral /forms

create xform /forms/control [400,600,290,190] -title  \
    "Simulation control"
ce /forms/control

create xbutton reset [10,45,60,25] -title "RESET" -script "sim_reset"
create xbutton run [75,45,60,25] -title "RUN" -script "sim_step"
create xbutton stop [140,45,60,25] -title "STOP" -script "stop"
create xbutton quit [205,45,60,25] -title "QUIT" -script "quit"

create xdialog tstep [10,80,255,25] -value 100.0 -title "time (msec)"  \
    -script void
create xdialog dt [10,115,255,25] -value 0.05 -title "dt (msec)" -script \
     "set_dt"
create xdialog fname [10,150,255,25] -value Vout -title "Output File"  \
    -script "void"

xshow /forms/control
//*************************************************************************



//*************************************************************************
//Create Current Pulse Control Interface

create xform /forms/iclamp [700,600,290,190] -title "Current Clamp"
ce /forms/iclamp

create xdialog level [10,45,255,25] -value 0.0 -title  \
    "Current Level (pA)" -script "dset /pulse level1 "<widget>
create xdialog delay [10,80,255,25] -value 0 -title "Onset Delay (msec)" \
     -script "dset /pulse delay1 "<widget>
create xdialog width [10,115,255,25] -value 100 -title  \
    "Pulse Width (msec)" -script "dset /pulse width1 "<widget>
create xdialog interval [10,150,255,25] -value 100 -title  \
    "Pulse Interval (msec)" -script "set_pulse_interval /pulse "<widget>

xshow /forms/iclamp
//*************************************************************************



//*************************************************************************
//Create Channel Control Interface

create xform /data [10,600,350,270] -title "K+ and Na+ (N) Channels"

create xdialog /data/Area -script void -value 1.0 -title "Area(um2)"
create xdialog /data/KGopen -script void -value 20.0 -title "KGopen(pS)"
create xdialog /data/Kdense -script void -value 18.0 -title  \
    "Density of K Channels(/um2)"
create xdialog /data/NaGopen -script void -value 20.0 -title  \
    "NaGopen(pS)"
create xdialog /data/Nadense -script void -value 60.0 -title  \
    "Density of Na Channels(/um2)"
create xdialog /data/Cm_squid -script void -value 0.01 -title  \
    "Cm_squid(pF/um2)"
create xdialog /data/Gleak_squid -script void -value 3.0 -title  \
    "Gleak_squid(pS/um2)"
create xdialog /data/Vleak_squid -script void -value 10.613 -title  \
    "Vleak_squid(mV)"

xshow /data
//*************************************************************************

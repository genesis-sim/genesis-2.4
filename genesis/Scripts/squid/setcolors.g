function colorize
    setfield /##[ISA=xlabel] fg white bg blue3
    setfield /##[ISA=xbutton] offbg rosybrown1 onbg rosybrown1
    setfield /##[ISA=xtoggle] onfg red offbg cadetblue1 onbg cadetblue1
    setfield /##[ISA=xdialog] bg palegoldenrod
    setfield /##[ISA=xgraph] bg ivory
end

function upicolors
    setfield /##[ISA=xlabel] fg black bg green
    setfield /##[ISA=xbutton] offbg DeepSkyBlue onbg grey onfg red
    setfield /##[ISA=xtoggle] offfg white offbg blue onbg red onfg white
    setfield /##[ISA=xdialog] bg yellow
    setfield /##[ISA=xgraph] bg white
    setfield /##[ISA=xform] bg lightblue
end

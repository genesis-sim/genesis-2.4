//genesis

function pcsave(file)
str file
    save /##[TYPE=compartment] {file}
    save /##[TYPE=channelC] {file} -append
    save /##[TYPE=channelC2] {file} -append
end


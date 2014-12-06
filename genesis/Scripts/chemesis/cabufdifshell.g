//genesis
//cabufdifshell.g
function cabufdifshell (pool, Cainit, buftot, rad, len, CaDiff, bufDiff)
float rad, len
float Cainit, buftot, CaDiff, bufDiff
str pool

    /* create pools of molecules */
    create difshell {pool}Ca
    setfield {pool}Ca Ceq {Cainit} shape_mode {SLAB} dia {2*rad} thick {len} val 2 D {CaDiff}
    
    create difbuffer {pool}Cabuf
    setfield {pool}Cabuf Btot {buftot} shape_mode {SLICE} dia {2*rad} thick {len} kBf 0.028e6 kBb 19.6 D {bufDiff}

    /* messages between buffer and calcium */
    addmsg {pool}Ca {pool}Cabuf CONCEN C
    addmsg {pool}Cabuf {pool}Ca BUFFER kBf kBb Bfree Bbound

end


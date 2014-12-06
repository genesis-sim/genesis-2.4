/* non-inactivating BK-type Ca-dependent K current
** Moczydlowski and Latorre 1983, J. Gen. Physiol. 82:511-542.
** Implemented by Erik De Schutter BBF-UIA,
** with original parameters scaled for units: V, sec, mM. 
** Assumes tab2Dchannel "Moczyd_KC" has a sibling Ca_concen "Ca_conc". */
float EK = -0.085   // volts
function make_Moczyd_KC
    int xdivs = 100
    int ydivs = {xdivs}
    float xmin, xmax, ymin, ymax
    xmin = -0.1; xmax = 0.05; ymin = 0.0; ymax = 0.0030 // x = Vm, y = [Ca]
    int i, j
    float x, dx, y, dy, a, b
    float Temp = 37
    float ZFbyRT = 23210/(273.15 + Temp)
    if (!({exists Moczyd_KC}))
        create tab2Dchannel Moczyd_KC
        setfield Moczyd_KC Ek {EK} Gbar 0.0  \
            Xindex {VOLT_C1_INDEX} Xpower 1 Ypower 0 Zpower 0
        call Moczyd_KC TABCREATE X {xdivs} {xmin} {xmax} \
            {ydivs} {ymin} {ymax}
    end
    dx = (xmax - xmin)/xdivs
    dy = (ymax - ymin)/ydivs
    x = xmin
    for (i = 0; i <= xdivs; i = i + 1)
        y = ymin
        for (j = 0; j <= ydivs; j = j + 1)
            a = 480*y/(y + 0.180*{exp {-0.84*ZFbyRT*x}})
            b = 280/(1 + y/(0.011*{exp {-1.00*ZFbyRT*x}}))
            setfield Moczyd_KC X_A->table[{i}][{j}] {a}
            setfield Moczyd_KC X_B->table[{i}][{j}] {a + b}
            y = y + dy
        end
        x = x + dx
    end
    setfield Moczyd_KC X_A->calc_mode {LIN_INTERP}
    setfield Moczyd_KC X_B->calc_mode {LIN_INTERP}
    addfield Moczyd_KC addmsg1
    setfield Moczyd_KC addmsg1 "../Ca_conc . CONCEN1 Ca"
end

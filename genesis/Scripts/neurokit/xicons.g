//genesis

create x1shape /xproto/draw/Trode -linewidth 2 -hlhistmode none  \
    -hldispmode none /* -pixflags pc */ -transfmode 1 -textmode nodraw -coords  \
    [-40,90,0][0,0,0][-20,100,0][-30,95,0][-40,150,0][-140,100,0]

create x1shape /xproto/draw/Vclamp -hlhistmode none -hldispmode none  \
    /* -pixflags pc */ -transfmode 1 -textmode nodraw -coords  \
    [-30,-100,0][0,0,0][-10,-100,0][-20,-100,0][-40,-200,0][-70,-200,]

create x1shape /xproto/draw/Inj -linewidth 2 -hlhistmode none -hldispmode \
     none /* -pixflags pc */ -transfmode 1 -textmode nodraw -coords  \
    [25,90,0][0,0,0][10,100,0][20,100,0][40,200,0][70,200,0][80,210,0][70,200,0][80,190,0]

create x1shape /xproto/draw/randGABA -linewidth 2 -hlhistmode none  \
    -hldispmode none -joinstyle JoinBevel -textmode nodraw -coords  \
    [0,0,0.0001][0,0,0.00004][0.00004,0.00003,0][-0.00004,0.00003,0] \
    [0,-0.00005,0][0.00004,0.00003,0][-0.00004,0.00003,0][0,0,0.00004] \
    [0,-0.00005,0]

create x1shape /xproto/draw/randglu -linewidth 2 -hlhistmode none  \
    -hldispmode none -joinstyle JoinBevel -pixcolor red -textmode nodraw \
    -coords  \
    [0,0,0.0001][0,0,0.00004][0.00004,0.00003,0][-0.00004,0.00003,0] \
    [0,-0.00005,0][0.00004,0.00003,0][-0.00004,0.00003,0][0,0,0.00004] \
    [0,-0.00005,0]

create x1shape /xproto/draw/randACh -linewidth 2 -hlhistmode none  \
    -hldispmode none -joinstyle JoinBevel -pixcolor green -textmode nodraw \
    -coords  \
    [0,0,0.0001][0,0,0.00004][0.00004,0.00003,0][-0.00004,0.00003,0] \
    [0,-0.00005,0][0.00004,0.00003,0][-0.00004,0.00003,0][0,0,0.00004] \
    [0,-0.00005,0]

create x1shape /xproto/draw/actGABA -linewidth 2 -hlhistmode none  \
    -hldispmode none -joinstyle JoinBevel -textmode nodraw -coords  \
    [0.00002,0,0.0001][0,0,0][0.00004,0,0][-0.00004,0,0]

create x1shape /xproto/draw/actglu -linewidth 2 -hlhistmode none  \
    -hldispmode none -pixcolor red -joinstyle JoinBevel -textmode nodraw -coords  \
    [0.00002,0,0.0001][0,0,0][0.00004,0,0][-0.00004,0,0]

create x1shape /xproto/draw/actACh -linewidth 2 -hlhistmode none  \
    -hldispmode none -pixcolor green -joinstyle JoinBevel -textmode nodraw -coords  \
    [0.00002,0,0.0001][0,0,0][0.00004,0,0][-0.00004,0,0]
create x1shape /xproto/draw/comptshape1 -linewidth 3 -hlhistmode none  \
    -hldispmode none -pixcolor orchid -textmode nodraw -coords  \
    [20,40,0][18,38,0][17,36,0][16,32,0][16,28,0][17,24,0] \
    [18,22,0][20,20,0][22,22,0][23,24,0][24,28,0][24,32,0] \
    [23,36,0][22,38,0][20,40,0][60,60,0][62,58,0][63,56,0] \
    [64,52,0][64,48,0][63,44,0][62,42,0][60,40,0][20,20,0]

create x1shape /xproto/draw/comptshape -linewidth 3 -hlhistmode none  \
    -hldispmode none -pixcolor orchid -textmode nodraw -coords  \
    [5,10,0][4.5,9.5,0][4.2,9,0][4,8,0][4,7,0][4.2,6,0]  \
    [4.5,5.5,0][5,5,0][5.5,5.5,0][5.8,6,0][6,7,0][6,8,0] \
    [5.8,9,0][5.5,9.5,0][5,10,0][15,15,0][15.5,14.5,0][15.8,14,0] \
    [16,13,0][16,12,0][15.8,11,0][15.5,10.5,0][15,10,0][5,5,0]

create x1shape /xproto/draw/hilight -linewidth 4 -hlhistmode none  \
    -hldispmode none -pixcolor red -textmode nodraw -coords  \
    [-1.2,1.2,0][1.2,1.2,0][1.2,-1.2,0][-1.2,-1.2,0][-1.2,1.2,0]

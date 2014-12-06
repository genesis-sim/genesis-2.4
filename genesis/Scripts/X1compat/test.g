//genesis

create x1form /f
setfield /f wgeom 300 hgeom 300

create x1draw /f/d xmin -300 xmax 300 ymin -300 ymax 300

ce /f/d
create x1shape s1 text s1 value s1 pts [0,0,0][100,0,0][0,100,0][0,0,0]
setfield s1 hldispmode star

copy s1 s2
setfield s2 text s2 value s2 tx -200

copy s1 s3
setfield s3 text s3 value s3 tx -200 ty -200

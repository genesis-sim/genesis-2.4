//genesis
// kkit Version 7 dumpfile
 
// Saved on Sun Sep  3 11:57:55 2000
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.01
CONTROLDT = 5
PLOTDT = 1
MAXTIME = 100
float TRANSIENT_TIME = 2
int VARIABLE_DT_FLAG = 0
float DEFAULT_VOL = 1.6667e-21
kparms
 
//genesis

initdump -version 3 -ignoreorphans 1
simobjdump table input output alloced step_mode stepsize
simobjdump xtree path script namemode sizescale
simobjdump xcoredraw xmin xmax ymin ymax
simobjdump xtext editable
simobjdump xgraph xmin xmax ymin ymax overlay
simobjdump xplot pixflags script fg ysquish do_slope wy
simobjdump kpool CoTotal CoInit Co CoRemaining n nInit nTotal nMin vol \
  slave_enable notes xtree_fg_req xtree_textfg_req
simobjdump xtab input output alloced step_mode stepsize notes editfunc \
  xtree_fg_req xtree_textfg_req plotfield manageclass baselevel last_x last_y \
  is_running
simobjdump group notes editfunc xtree_fg_req xtree_textfg_req plotfield \
  manageclass expanded movealone
simobjdump kenz CoComplexInit CoComplex nComplexInit nComplex vol sA pA eA B \
  k1 k2 k3 keepconc usecomplex ksum oldmsgmode notes editfunc xtree_fg_req \
  xtree_textfg_req plotfield manageclass link_to_manage
simobjdump kreac A B kf kb is_active oldmsgmode notes editfunc xtree_fg_req \
  xtree_textfg_req plotfield manageclass
simundump group /kinetics/epi 0 "" edit_group blue black x group 0 0
simundump xtab /kinetics/epi/xtab 0 0 0 1 1 0 "" edit_xtab white blue output \
  group 0 100 0.92955 1
loadtab /kinetics/epi/xtab table 1 100 0 100 \
 0 0.0092955 0.018591 0.027887 0.037182 0.046477 0.055773 0.065069 0.074364 \
 0.083659 0.092955 0.10225 0.11155 0.12084 0.13014 0.13943 0.14873 0.15802 \
 0.16732 0.17661 0.18591 0.19521 0.2045 0.2138 0.22309 0.23239 0.24168 \
 0.25098 0.26027 0.26957 0.27887 0.28816 0.29746 0.30675 0.31605 0.32534 \
 0.33464 0.34393 0.35323 0.36252 0.37182 0.38112 0.39041 0.39971 0.409 0.4183 \
 0.42759 0.43689 0.44618 0.45548 0.46478 0.47407 0.48337 0.49266 0.50196 \
 0.51125 0.52055 0.52984 0.53914 0.54843 0.55773 0.56703 0.57632 0.58562 \
 0.59491 0.60421 0.6135 0.6228 0.63209 0.64139 0.65069 0.65998 0.66928 \
 0.67857 0.68787 0.69716 0.70646 0.71575 0.72505 0.73434 0.74364 0.75294 \
 0.76223 0.77153 0.78082 0.79012 0.79941 0.80871 0.818 0.8273 0.8366 0.84589 \
 0.85519 0.86448 0.87378 0.88307 0.89237 0.90166 0.91096 0.92025 1.0001
simundump kpool /kinetics/epi/sub 0 1 1 1 0.0006985 1 1 1 0 1 1 "" 27 blue
simundump kpool /kinetics/epi/prd 0 0 0 0 0 0 0 0 0 1 0 "" 0 blue
simundump kpool /kinetics/epi/enz 0 1 1 1 0.55932 1 1 1 0 1 0 "" 61 blue
simundump kenz /kinetics/epi/enz/kenz 0 0 0 0 0 1 0.03348 0.00837 0.04185 \
  0.044009 0.1 0.4 0.1 0 0 0.5 0 "" edit_enz red 61 CoComplex kpool \
  enzproto_to_pool_link
simundump kreac /kinetics/epi/inhib 0 0.47414 0.47947 10 1 0 0 "" edit_reac \
  white blue A group
simundump kpool /kinetics/epi/inhib-enz 0 0 0 0 0 0 0 0 0 1 0 "" 47 blue
simundump xcoredraw /edit/draw 0 -5 5 -2.5 7.5
simundump xtree /edit/draw/tree 0 /kinetics/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
call /edit/draw/tree ADDMSGARROW \
             select                 all       all       all    none -1000 0 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
     /kinetics/##[] /kinetics/##[TYPE=group]       all       all    none -1001 0 \
  "echo.p dragging <S> onto group <D>; thing_to_group_add.p <S> <D>" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kpool]  SUMTOTAL     kpool darkgreen -1002 0 \
  "echo.p dragging <S> to <D> for sumtotal" "pool_to_pool_add_sumtot.p <S> <D>" "pool_to_pool_drop_sumtot.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kpool]  CONSERVE     kpool    blue -1003 0 \
  "echo.p dragging <S> to <D> for conserve" "pool_to_pool_add_consv.p <S> <D>" "pool_to_pool_drop_consv.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kreac] /kinetics/##[TYPE=kpool]      REAC     kpool    none -1004 0 \
  "" "reac_to_pool_add.p <S> <D>" "reac_to_pool_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kreac] SUBSTRATE     kreac   green -1005 0 \
  "" "pool_to_reac_add.p <S> <D>" "pool_to_reac_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kreac]        KF     kreac  purple -1006 0 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kreac]        KB     kreac hotpink -1007 0 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kreac]   PRODUCT     kreac   green -1008 1 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kenz] /kinetics/##[TYPE=kpool]    MM_PRD     kpool     red -1009 0 \
  "echo.p dragging <S> to <D> for enz product" "enz_to_pool_add.p <S> <D>" "enz_to_pool_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kenz] SUBSTRATE      kenz     red -1010 0 \
  "echo.p dragging <S> to <D> for enz substrate" "pool_to_enz_add.p <S> <D>" "pool_to_enz_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kenz] /kinetics/##[TYPE=kpool]  SUMTOTAL     kpool darkgreen -1011 0 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kenz] /kinetics/##[TYPE=kpool]  CONSERVE     kpool    blue -1012 0 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kenz]  INTRAMOL      kenz   white -1013 0 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=stim] /kinetics/##[TYPE=kpool]     SLAVE     kpool  orange -1014 0 \
  "echo.p dragging <S> to <D> for stimulus" "stim_to_pool_add.p <S> <D>" "stim_to_pool_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=stim] /kinetics/##[TYPE=stim]     INPUT      stim  orange -1015 0 \
  "echo.p dragging <S> to <D> for stim" "stim_to_stim_add.p <S> <D>" "stim_to_stim_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=xtab] /kinetics/##[TYPE=kpool]     SLAVE     kpool  orange -1016 0 \
  "echo.p dragging <S> to <D> for xtab" "xtab_to_pool_add.p <S> <D>" "xtab_to_pool_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=xtab] /kinetics/##[TYPE=xtab]     INPUT      xtab  orange -1017 0 \
  "echo.p dragging <S> to <D> for xtab" "xtab_to_xtab_add.p <S> <D>" "xtab_to_xtab_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=xtab]     INPUT      xtab hotpink -1018 0 \
  "echo.p dragging <S> to <D> for xtab" "pool_to_xtab_add.p <S> <D>" "pool_to_xtab_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kchan] /kinetics/##[TYPE=kpool]      REAC     kpool    none -1019 0 \
  "echo.p dragging <S> to <D> for chan product" "chan_to_pool_add.p <S> <D>" "chan_to_pool_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kchan]   PRODUCT     kchan  orange -1020 1 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kchan] SUBSTRATE     kchan  orange -1021 0 \
  "echo.p dragging <S> to <D> for chan substrate" "pool_to_chan_add.p <S> <D>" "pool_to_chan_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=transport] /kinetics/##[TYPE=kpool]    MM_PRD     kpool  yellow -1022 0 \
  "" "transport_to_pool_add.p <S> <D>" "transport_to_pool_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=transport] SUBSTRATE transport  yellow -1023 0 \
  "" "pool_to_transport_add.p <S> <D>" "pool_to_transport_drop.p <S> <D>"
call /edit/draw/tree UNDUMP 8 \
 "/kinetics/epi" 0.73771 5.6893 0 \
 "/kinetics/epi/xtab" -1.7623 6.3477 0 \
 "/kinetics/epi/sub" -4.2418 4.2387 0 \
 "/kinetics/epi/prd" -1.0246 4.2181 0 \
 "/kinetics/epi/enz" -2.6844 2.3251 0 \
 "/kinetics/epi/enz/kenz" -2.7254 3.2428 0 \
 "/kinetics/epi/inhib" 1.0861 3.107 0 \
 "/kinetics/epi/inhib-enz" 2.9713 4.3827 0
simundump xtext /file/notes 0 1
xtextload /file/notes \
"Some useful comments here." \
"Kkit tutorial file."
addmsg /kinetics/epi/enz/kenz /kinetics/epi/sub REAC sA B 
addmsg /kinetics/epi/xtab /kinetics/epi/sub SLAVE output 
addmsg /kinetics/epi/enz/kenz /kinetics/epi/prd MM_PRD pA 
addmsg /kinetics/epi/inhib /kinetics/epi/prd REAC A B 
addmsg /kinetics/epi/enz/kenz /kinetics/epi/enz REAC eA B 
addmsg /kinetics/epi/inhib /kinetics/epi/enz REAC A B 
addmsg /kinetics/epi/enz /kinetics/epi/enz/kenz ENZYME n 
addmsg /kinetics/epi/sub /kinetics/epi/enz/kenz SUBSTRATE n 
addmsg /kinetics/epi/prd /kinetics/epi/inhib SUBSTRATE n 
addmsg /kinetics/epi/enz /kinetics/epi/inhib SUBSTRATE n 
addmsg /kinetics/epi/inhib-enz /kinetics/epi/inhib PRODUCT n 
addmsg /kinetics/epi/inhib /kinetics/epi/inhib-enz REAC B A 
enddump
// End of dump

call /edit/draw/tree RESET
fix_transp_msgs
reset

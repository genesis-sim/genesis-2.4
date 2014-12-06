//genesis
// kkit Version 8 dumpfile
 
// Saved on Sun Jan 21 01:10:24 2001
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.01
CONTROLDT = 5
PLOTDT = 0.1
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
simobjdump modout nmsgr nreac nenz notes xtree_fg_req xtree_textfg_req
simobjdump modin nmsgr nreac nenz nenzsub nenzprd nreacsub nreacprd notes \
  xtree_fg_req xtree_textfg_req
simobjdump kreac A B kf kb stoch_flag is_active oldmsgmode notes editfunc \
  xtree_fg_req xtree_textfg_req plotfield manageclass
simundump kpool /kinetics/B 0 0 0 0 0 0 0 0 0 1 0 "" blue black
simundump kpool /kinetics/C 0 0 0 1 -1 1 0 0 0 1 0 "" 28 black
simundump kpool /kinetics/A 0 1 1 0 1 0 1 1 0 1 0 "" 1 black
simundump kreac /kinetics/DA 0 0 0 0.1 0.1 1 0 0 "" edit_reac 1 black A group
simundump kpool /kinetics/D 0 0 0 0 0 0 0 0 0 1 0 "" 61 black
simundump kreac /kinetics/CD 0 0 0 0.1 0.1 1 0 0 "" edit_reac white black A \
  group
simundump kreac /kinetics/BC 0 0 0 0.1 0.1 1 0 0 "" edit_reac white black A \
  group
simundump kreac /kinetics/AB 0 0 0 0.1 0.1 1 0 0 "" edit_reac white black A \
  group
simundump xgraph /graphs/conc1 0 0 100 0 1.1454 0
simundump xgraph /graphs/conc2 0 0 100 0 1 0
simundump xplot /graphs/conc1/A.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 1 0 0 1
simundump xplot /graphs/conc1/B.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc1/C.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 28 0 0 1
simundump xplot /graphs/conc1/D.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 61 0 0 1
simundump xgraph /moregraphs/conc3 0 0 100 0 1 0
simundump xgraph /moregraphs/conc4 0 0 100 0 1 0
simundump xcoredraw /edit/draw 0 -5 5 -5 5
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
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=modout] /kinetics/##[TYPE=modin]     INPUT     modin     red -1024 0 \
  "echo.p dragging <S> to <D> for interacting modules" "modout_to_modin_add.p <S> <D>" "modout_to_modin_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=modout]     DUMMY    modout    none -1025 0 \
  "msgr_to_modout.p <S> <D>" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kenz] /kinetics/##[TYPE=modout]     DUMMY    modout    none -1026 0 \
  "enz_to_modout.p <S> <D>" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kreac] /kinetics/##[TYPE=modout]     DUMMY    modout    none -1027 0 \
  "reac_to_modout.p <S> <D>" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=modin]     DUMMY     modin    none -1028 0 \
  "msgr_to_modin.p <S> <D>" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kenz] /kinetics/##[TYPE=modin]     DUMMY     modin    none -1029 0 \
  "enz_to_modin.p <S> <D>" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kreac] /kinetics/##[TYPE=modin]     DUMMY     modin    none -1030 0 \
  "reac_to_modin.p <S> <D>" "" ""
call /edit/draw/tree UNDUMP 8 \
 "/kinetics/B" 2.1926 2.5514 0 \
 "/kinetics/C" 2.0492 -1.5638 0 \
 "/kinetics/A" -2.459 2.5514 0 \
 "/kinetics/DA" -3.9754 0.84362 0 \
 "/kinetics/D" -2.2541 -1.7078 0 \
 "/kinetics/CD" -0.32787 -3.0247 0 \
 "/kinetics/BC" 3.8525 0.26749 0 \
 "/kinetics/AB" -0.14344 3.6831 0
simundump xtext /file/notes 0 1
xtextload /file/notes \
"Example of stochastic transitions" \
"among a molecule which can exist" \
"in 4 states."
addmsg /kinetics/AB /kinetics/B REAC B A 
addmsg /kinetics/BC /kinetics/B REAC A B 
addmsg /kinetics/BC /kinetics/C REAC B A 
addmsg /kinetics/CD /kinetics/C REAC A B 
addmsg /kinetics/DA /kinetics/A REAC B A 
addmsg /kinetics/AB /kinetics/A REAC A B 
addmsg /kinetics/A /kinetics/DA PRODUCT n 
addmsg /kinetics/D /kinetics/DA SUBSTRATE n 
addmsg /kinetics/CD /kinetics/D REAC B A 
addmsg /kinetics/DA /kinetics/D REAC A B 
addmsg /kinetics/C /kinetics/CD SUBSTRATE n 
addmsg /kinetics/D /kinetics/CD PRODUCT n 
addmsg /kinetics/B /kinetics/BC SUBSTRATE n 
addmsg /kinetics/C /kinetics/BC PRODUCT n 
addmsg /kinetics/A /kinetics/AB SUBSTRATE n 
addmsg /kinetics/B /kinetics/AB PRODUCT n 
addmsg /kinetics/A /graphs/conc1/A.Co PLOT Co *A.Co *1 
addmsg /kinetics/B /graphs/conc1/B.Co PLOT Co *B.Co *blue 
addmsg /kinetics/C /graphs/conc1/C.Co PLOT Co *C.Co *28 
addmsg /kinetics/D /graphs/conc1/D.Co PLOT Co *D.Co *61 
enddump
// End of dump

call /edit/draw/tree RESET
fix_transp_msgs
reset

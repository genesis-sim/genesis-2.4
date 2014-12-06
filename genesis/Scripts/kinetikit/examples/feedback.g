//genesis
// kkit Version 9 flat dumpfile
 
// Saved on Sun Jun 29 09:46:07 2003
 
include kkit {argv 1}
 
FASTDT = 0.01
SIMDT = 0.001
CONTROLDT = 5
PLOTDT = 1
MAXTIME = 2000
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 0
DEFAULT_VOL = 1.6667e-21
VERSION = 9.0
setfield /file/modpath value /home2/bhalla/scripts/modules
kparms
 
//genesis

initdump -version 3 -ignoreorphans 1
simobjdump table input output alloced step_mode stepsize x y z
simobjdump xtree path script namemode sizescale
simobjdump xcoredraw xmin xmax ymin ymax
simobjdump xtext editable
simobjdump xgraph xmin xmax ymin ymax overlay
simobjdump xplot pixflags script fg ysquish do_slope wy
simobjdump group xtree_fg_req xtree_textfg_req plotfield expanded movealone \
  link savename file version md5sum mod_save_flag x y z
simobjdump kpool CoTotal CoInit Co n nInit nTotal nMin vol slave_enable notes \
  xtree_fg_req xtree_textfg_req x y z
simobjdump kreac kf kb notes xtree_fg_req xtree_textfg_req x y z
simobjdump kenz CoComplexInit CoComplex nComplexInit nComplex vol k1 k2 k3 \
  keepconc usecomplex notes xtree_fg_req xtree_textfg_req link x y z
simobjdump stim level1 width1 delay1 level2 width2 delay2 baselevel trig_time \
  trig_mode notes xtree_fg_req xtree_textfg_req is_running x y z
simobjdump xtab input output alloced step_mode stepsize notes editfunc \
  xtree_fg_req xtree_textfg_req baselevel last_x last_y is_running x y z
simobjdump kchan perm gmax Vm is_active use_nernst notes xtree_fg_req \
  xtree_textfg_req x y z
simobjdump transport input output alloced step_mode stepsize dt delay clock \
  kf xtree_fg_req xtree_textfg_req x y z
simobjdump proto x y z
simobjdump linkinfo xtree_fg_req xtree_textfg_req uplink downlink x y z
simobjdump uplink xtree_fg_req xtree_textfg_req x y z
simobjdump downlink xtree_fg_req xtree_textfg_req x y z
simobjdump mirror notes xtree_fg_req x y z
simundump kpool /kinetics/X_prd 1 1 0 0 0 0 1 0 1 0 "" blue black 10 4 0
simundump kpool /kinetics/X_sub 1 1 1 1 1 1 1 0 1 4 "" blue black 8 4 0
simundump kreac /kinetics/degrade_X 1 0.1 0 "" white black 9 5 0
simundump kpool /kinetics/X 1 1 0 0 0 0 1 0 1 0 "" blue black 6 4 0
simundump kenz /kinetics/X/X_enz 1 0 0 0 0 1 1 1 1 0 0 "" red blue "" 7 3 0
simundump kpool /kinetics/X_inactive 1 1 1 1 1 1 1 0 1 0 "" blue black 2 4 0
simundump kpool /kinetics/Y 1 1 0 0 0 0 1 0 1 0 "" red black 6 0 0
simundump kenz /kinetics/Y/Y_enz 1 0 0 0 0 1 1 1 1 0 0 "" red red "" 5 1 0
simundump kpool /kinetics/Y_sub 1 1 1 1 1 1 1 0 1 4 "" red black 4 0 0
simundump kpool /kinetics/Y_prd 1 20 0 0 0 0 20 0 1 0 "" red black 2 0 0
simundump kreac /kinetics/degrade_Y 1 0.1 0 "" white black 3 -1 0
simundump kpool /kinetics/Y_inactive 1 1 1 1 1 1 1 0 1 0 "" red black 10 0 0
simundump kreac /kinetics/activate_X 1 1 1 "" white black 5 3 0
simundump kreac /kinetics/activate_Y 1 1 1 "" white black 7 1 0
simundump kreac /kinetics/activate_X[1] 1 1 1 "" white black 3 3 0
simundump kpool /kinetics/x.yprd 0 0 0 0 0 0 0 0 1 0 "" blue black 4 4 0
simundump kpool /kinetics/y.xprd 1 1 0 0 0 0 1 0 1 0 "" red black 8 0 0
simundump kreac /kinetics/activate_Y[1] 1 1 1 "" white black 9 1 0
simundump xgraph /graphs/conc1 0 0 2103 0 4 0
simundump xgraph /graphs/conc2 0 0 2103 0 4 0
simundump xplot /graphs/conc1/X.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0.0064722 0 1
simundump xplot /graphs/conc1/Y_prd.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0.2014 0 1
simundump xplot /graphs/conc2/Y.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0.0064482 0 1
simundump xplot /graphs/conc2/X_prd.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0.20215 0 1
simundump xgraph /moregraphs/conc3 0 0 2103 0 4 0
simundump xgraph /moregraphs/conc4 0 0 2103 0 4 0
simundump xcoredraw /edit/draw 0 0.58703 12.328 -4.6962 7.0444
simundump xtree /edit/draw/tree 0 /kinetics/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
addmsg /kinetics/X/X_enz /kinetics/X_prd MM_PRD pA 
addmsg /kinetics/degrade_X /kinetics/X_prd REAC A B 
addmsg /kinetics/activate_Y /kinetics/X_prd REAC A B 
addmsg /kinetics/activate_Y[1] /kinetics/X_prd REAC A B 
addmsg /kinetics/X/X_enz /kinetics/X_sub REAC sA B 
addmsg /kinetics/degrade_X /kinetics/X_sub REAC B A 
addmsg /kinetics/X_prd /kinetics/degrade_X SUBSTRATE n 
addmsg /kinetics/X_sub /kinetics/degrade_X PRODUCT n 
addmsg /kinetics/X/X_enz /kinetics/X REAC eA B 
addmsg /kinetics/activate_X /kinetics/X REAC B A 
addmsg /kinetics/X /kinetics/X/X_enz ENZYME n 
addmsg /kinetics/X_sub /kinetics/X/X_enz SUBSTRATE n 
addmsg /kinetics/activate_X[1] /kinetics/X_inactive REAC A B 
addmsg /kinetics/Y/Y_enz /kinetics/Y REAC eA B 
addmsg /kinetics/activate_Y /kinetics/Y REAC B A 
addmsg /kinetics/Y /kinetics/Y/Y_enz ENZYME n 
addmsg /kinetics/Y_sub /kinetics/Y/Y_enz SUBSTRATE n 
addmsg /kinetics/Y/Y_enz /kinetics/Y_sub REAC sA B 
addmsg /kinetics/degrade_Y /kinetics/Y_sub REAC B A 
addmsg /kinetics/Y/Y_enz /kinetics/Y_prd MM_PRD pA 
addmsg /kinetics/degrade_Y /kinetics/Y_prd REAC A B 
addmsg /kinetics/activate_X /kinetics/Y_prd REAC A B 
addmsg /kinetics/activate_X[1] /kinetics/Y_prd REAC A B 
addmsg /kinetics/Y_prd /kinetics/degrade_Y SUBSTRATE n 
addmsg /kinetics/Y_sub /kinetics/degrade_Y PRODUCT n 
addmsg /kinetics/activate_Y[1] /kinetics/Y_inactive REAC A B 
addmsg /kinetics/X /kinetics/activate_X PRODUCT n 
addmsg /kinetics/Y_prd /kinetics/activate_X SUBSTRATE n 
addmsg /kinetics/x.yprd /kinetics/activate_X SUBSTRATE n 
addmsg /kinetics/X_prd /kinetics/activate_Y SUBSTRATE n 
addmsg /kinetics/Y /kinetics/activate_Y PRODUCT n 
addmsg /kinetics/y.xprd /kinetics/activate_Y SUBSTRATE n 
addmsg /kinetics/X_inactive /kinetics/activate_X[1] SUBSTRATE n 
addmsg /kinetics/Y_prd /kinetics/activate_X[1] SUBSTRATE n 
addmsg /kinetics/x.yprd /kinetics/activate_X[1] PRODUCT n 
addmsg /kinetics/activate_X[1] /kinetics/x.yprd REAC B A 
addmsg /kinetics/activate_X /kinetics/x.yprd REAC A B 
addmsg /kinetics/activate_Y[1] /kinetics/y.xprd REAC B A 
addmsg /kinetics/activate_Y /kinetics/y.xprd REAC A B 
addmsg /kinetics/Y_inactive /kinetics/activate_Y[1] SUBSTRATE n 
addmsg /kinetics/X_prd /kinetics/activate_Y[1] SUBSTRATE n 
addmsg /kinetics/y.xprd /kinetics/activate_Y[1] PRODUCT n 
addmsg /kinetics/X /graphs/conc1/X.Co PLOT Co *X.Co *blue 
addmsg /kinetics/Y_prd /graphs/conc1/Y_prd.Co PLOT Co *Y_prd.Co *red 
addmsg /kinetics/Y /graphs/conc2/Y.Co PLOT Co *Y.Co *red 
addmsg /kinetics/X_prd /graphs/conc2/X_prd.Co PLOT Co *X_prd.Co *blue 
enddump
// End of dump

complete_loading

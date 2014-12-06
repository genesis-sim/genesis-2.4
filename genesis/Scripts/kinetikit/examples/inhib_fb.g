//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Sun Nov 13 12:48:53 2005
 
include kkit {argv 1}
 
FASTDT = 0.01
SIMDT = 0.01
CONTROLDT = 5
PLOTDT = 1
MAXTIME = 4000
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 0
DEFAULT_VOL = 1.6667e-21
VERSION = 11.0
setfield /file/modpath value /home2/bhalla/scripts/modules
kparms
 
//genesis

initdump -version 3 -ignoreorphans 1
simobjdump doqcsinfo filename accessname accesstype transcriber developer \
  citation species tissue cellcompartment methodology sources \
  model_implementation model_validation x y z
simobjdump table input output alloced step_mode stepsize x y z
simobjdump xtree path script namemode sizescale
simobjdump xcoredraw xmin xmax ymin ymax
simobjdump xtext editable
simobjdump xgraph xmin xmax ymin ymax overlay
simobjdump xplot pixflags script fg ysquish do_slope wy
simobjdump group xtree_fg_req xtree_textfg_req plotfield expanded movealone \
  link savename file version md5sum mod_save_flag x y z
simobjdump geometry size dim shape outside xtree_fg_req xtree_textfg_req x y \
  z
simobjdump kpool DiffConst CoInit Co n nInit mwt nMin vol slave_enable \
  geomname xtree_fg_req xtree_textfg_req x y z
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
simobjdump text str
simundump geometry /kinetics/geometry 0 1.6667e-21 3 sphere "" white black 0 \
  0 0
simundump kpool /kinetics/X_prd 1 0 0 0 0 0 0 0 1 0 /kinetics/geometry blue \
  black 2.2541 3.1157 0
simundump kpool /kinetics/X_sub 1 0 1 1 1 1 0 0 1 4 /kinetics/geometry blue \
  black -2.9303 3.097 0
simundump kreac /kinetics/degrade_X 1 0.05 0 "" white black -0.30738 3.8806 0
simundump kpool /kinetics/X 1 0 1 1 1 1 0 0 1 0 /kinetics/geometry blue black \
  -0.51229 0.87687 0
simundump kenz /kinetics/X/X_enz 1 0 0 0 0 1 1 1 1 0 0 "" red blue "" \
  -0.59426 1.9515 0
simundump kpool /kinetics/X_inactive 1 0 0 0 0 0 0 0 1 0 /kinetics/geometry \
  blue black -4.0574 0.22388 0
simundump kpool /kinetics/Y 1 0 1 1 1 1 0 0 1 0 /kinetics/geometry red black \
  -0.51229 -0.6903 0
simundump kenz /kinetics/Y/Y_enz 1 0 0 0 0 1 1 1 1 0 0 "" red red "" -0.47131 \
  -2.0522 0
simundump kpool /kinetics/Y_sub 1 0 1 1 1 1 0 0 1 4 /kinetics/geometry red \
  black 2.1926 -2.7612 0
simundump kpool /kinetics/Y_prd 1 0 0 0 0 0 0 0 1 0 /kinetics/geometry red \
  black -2.9303 -2.8545 0
simundump kreac /kinetics/degrade_Y 1 0.05 0 "" white black -0.59426 -3.7687 \
  0
simundump kpool /kinetics/Y_inactive 1 0 0 0 0 0 0 0 1 0 /kinetics/geometry \
  red black 3.738 0.018103 0
simundump kreac /kinetics/inactivate_Y 1 1 1 "" white black 2.0679 -1.3758 0
simundump kreac /kinetics/inactivate_X 1 1 1 "" white black -2.7638 1.6111 0
simundump xtab /kinetics/xtab 1 0 0 1 1 0 "" edit_xtab white green 0 1 \
  0.022727 2 1.5111 1.9551 0
loadtab /kinetics/xtab table 1 100 0 2000 \
 0 0 0 0 0 0 0 0 0 0 0 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.2 0.2 0.2 \
 0.2 0.2 0.2 0.2 0.2 0.2 0.2 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.4 0.4 \
 0.4 0.4 0.4 0.4 0.4 0.4 0.4 0.4 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.6 \
 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.6 0.7 0.7 0.7 0.7 0.7 0.7 0.7 0.7 0.7 0.7 \
 0.8 0.8 0.8 0.8 0.8 0.8 0.8 0.8 0.8 0.8 1 1 1 1 1 1 1 1 1 1
simundump xgraph /graphs/conc1 0 0 5328.8 0 8 0
simundump xgraph /graphs/conc2 0 0 5328.8 0 8 0
simundump xplot /graphs/conc1/X.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc1/Y_prd.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /graphs/conc2/Y.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /graphs/conc2/X_prd.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xgraph /moregraphs/conc3 0 0 5328.8 0 8 0
simundump xgraph /moregraphs/conc4 0 0 5328.8 0 8 0
simundump xcoredraw /edit/draw 0 -4.8515 4.8515 -4.8515 4.8515
simundump xtree /edit/draw/tree 0 /kinetics/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"Here we do get bistability. It is" \
"just a matter of changing the rates" \
"of degradation of the products, so" \
"that the conc-eff curves are somewhat" \
"steeper."
addmsg /kinetics/X/X_enz /kinetics/X_prd MM_PRD pA 
addmsg /kinetics/degrade_X /kinetics/X_prd REAC A B 
addmsg /kinetics/inactivate_Y /kinetics/X_prd REAC A B 
addmsg /kinetics/inactivate_Y /kinetics/X_prd REAC A B 
addmsg /kinetics/X/X_enz /kinetics/X_sub REAC sA B 
addmsg /kinetics/degrade_X /kinetics/X_sub REAC B A 
addmsg /kinetics/X_prd /kinetics/degrade_X SUBSTRATE n 
addmsg /kinetics/X_sub /kinetics/degrade_X PRODUCT n 
addmsg /kinetics/X/X_enz /kinetics/X REAC eA B 
addmsg /kinetics/inactivate_X /kinetics/X REAC A B 
addmsg /kinetics/xtab /kinetics/X SLAVE output 
addmsg /kinetics/X /kinetics/X/X_enz ENZYME n 
addmsg /kinetics/X_sub /kinetics/X/X_enz SUBSTRATE n 
addmsg /kinetics/inactivate_X /kinetics/X_inactive REAC B A 
addmsg /kinetics/Y/Y_enz /kinetics/Y REAC eA B 
addmsg /kinetics/inactivate_Y /kinetics/Y REAC A B 
addmsg /kinetics/Y /kinetics/Y/Y_enz ENZYME n 
addmsg /kinetics/Y_sub /kinetics/Y/Y_enz SUBSTRATE n 
addmsg /kinetics/Y/Y_enz /kinetics/Y_sub REAC sA B 
addmsg /kinetics/degrade_Y /kinetics/Y_sub REAC B A 
addmsg /kinetics/Y/Y_enz /kinetics/Y_prd MM_PRD pA 
addmsg /kinetics/degrade_Y /kinetics/Y_prd REAC A B 
addmsg /kinetics/inactivate_X /kinetics/Y_prd REAC A B 
addmsg /kinetics/inactivate_X /kinetics/Y_prd REAC A B 
addmsg /kinetics/Y_prd /kinetics/degrade_Y SUBSTRATE n 
addmsg /kinetics/Y_sub /kinetics/degrade_Y PRODUCT n 
addmsg /kinetics/inactivate_Y /kinetics/Y_inactive REAC B A 
addmsg /kinetics/X_prd /kinetics/inactivate_Y SUBSTRATE n 
addmsg /kinetics/X_prd /kinetics/inactivate_Y SUBSTRATE n 
addmsg /kinetics/Y /kinetics/inactivate_Y SUBSTRATE n 
addmsg /kinetics/Y_inactive /kinetics/inactivate_Y PRODUCT n 
addmsg /kinetics/Y_prd /kinetics/inactivate_X SUBSTRATE n 
addmsg /kinetics/Y_prd /kinetics/inactivate_X SUBSTRATE n 
addmsg /kinetics/X /kinetics/inactivate_X SUBSTRATE n 
addmsg /kinetics/X_inactive /kinetics/inactivate_X PRODUCT n 
addmsg /kinetics/X /graphs/conc1/X.Co PLOT Co *X.Co *blue 
addmsg /kinetics/Y_prd /graphs/conc1/Y_prd.Co PLOT Co *Y_prd.Co *red 
addmsg /kinetics/Y /graphs/conc2/Y.Co PLOT Co *Y.Co *red 
addmsg /kinetics/X_prd /graphs/conc2/X_prd.Co PLOT Co *X_prd.Co *blue 
enddump
// End of dump

setfield /kinetics/xtab table->dx 20
setfield /kinetics/xtab table->invdx 0.05
complete_loading

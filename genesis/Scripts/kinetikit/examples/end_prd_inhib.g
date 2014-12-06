//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Sun Nov 13 12:51:34 2005
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.01
CONTROLDT = 5
PLOTDT = 1
MAXTIME = 100
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
simundump kpool /kinetics/enz 0 0 1 1 1 1 0 0 1 0 /kinetics/geometry 61 black \
  -2.6844 2.3251 0
simundump kenz /kinetics/enz/kenz 0 0 0 0 0 1 0.1 0.4 0.1 0 0 "" red 61 "" \
  -2.7254 3.2428 0
simundump kpool /kinetics/sub 0 0 1 1 1 1 0 0 1 1 /kinetics/geometry 27 black \
  -4.2418 4.2387 0
simundump kpool /kinetics/prd 0 0 0 0 0 0 0 0 1 0 /kinetics/geometry 0 black \
  -1.0246 4.2181 0
simundump kreac /kinetics/inhib 0 10 1 "" white black 1.0861 3.107 0
simundump kpool /kinetics/inhib-enz 0 0 0 0 0 0 0 0 1 0 /kinetics/geometry 47 \
  black 2.9713 4.3827 0
simundump xtab /kinetics/xtab 0 0 0 1 1 0 "" edit_xtab white red 0 100 \
  0.92955 1 -1.7623 6.3477 0
loadtab /kinetics/xtab table 1 100 0 100 \
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
simundump xgraph /graphs/conc1 0 0 100 0 1 0
simundump xgraph /graphs/conc2 0 0 100 0 1 0
simundump xplot /graphs/conc1/sub.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 27 0 0 1
simundump xplot /graphs/conc1/prd.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 0 0 0 1
simundump xplot /graphs/conc1/enz.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 61 0 0 1
simundump xplot /graphs/conc1/inhib-enz.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 47 0 0 1
simundump xgraph /moregraphs/conc3 0 0 100 0 1 0
simundump xgraph /moregraphs/conc4 0 0 100 0 1 0
simundump xcoredraw /edit/draw 0 -5 5 -2.5 7.5
simundump xtree /edit/draw/tree 0 /kinetics/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"" \
"Kkit tutorial file for end-product inhibition."
addmsg /kinetics/enz/kenz /kinetics/enz REAC eA B 
addmsg /kinetics/inhib /kinetics/enz REAC A B 
addmsg /kinetics/enz /kinetics/enz/kenz ENZYME n 
addmsg /kinetics/sub /kinetics/enz/kenz SUBSTRATE n 
addmsg /kinetics/enz/kenz /kinetics/sub REAC sA B 
addmsg /kinetics/xtab /kinetics/sub SLAVE output 
addmsg /kinetics/enz/kenz /kinetics/prd MM_PRD pA 
addmsg /kinetics/inhib /kinetics/prd REAC A B 
addmsg /kinetics/prd /kinetics/inhib SUBSTRATE n 
addmsg /kinetics/enz /kinetics/inhib SUBSTRATE n 
addmsg /kinetics/inhib-enz /kinetics/inhib PRODUCT n 
addmsg /kinetics/inhib /kinetics/inhib-enz REAC B A 
addmsg /kinetics/sub /graphs/conc1/sub.Co PLOT Co *sub.Co *27 
addmsg /kinetics/prd /graphs/conc1/prd.Co PLOT Co *prd.Co *0 
addmsg /kinetics/enz /graphs/conc1/enz.Co PLOT Co *enz.Co *61 
addmsg /kinetics/inhib-enz /graphs/conc1/inhib-enz.Co PLOT Co *inhib-enz.Co *47 
enddump
// End of dump

setfield /kinetics/xtab table->dx 1
setfield /kinetics/xtab table->invdx 1
complete_loading

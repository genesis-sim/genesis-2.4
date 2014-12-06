//genesis
// kkit Version 9 flat dumpfile
 
// Saved on Sun Jul 27 02:11:46 2003
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.01
CONTROLDT = 5
PLOTDT = 1
MAXTIME = 100
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
simundump kpool /kinetics/state 0 0 0 0 0 0 0 0 1 0 "" 61 black 0 2 0
simundump kenz /kinetics/state/Make_ligand 0 0 0 0 0 1 0.1 0.4 0.1 0 0 "" red \
  61 "" 0 0 0
simundump kpool /kinetics/basal 0 1 1 1 1 1 1 0 1 0 "" blue black 3 0 0
simundump kpool /kinetics/substrate 0 1 1 1 1 1 1 0 1 4 "" 24 black -4 2 0
simundump kpool /kinetics/Receptor 0 0.1 0.1 0.1 0.1 0.1 0.1 0 1 0 "" 28 \
  black -4 0 0
simundump kpool /kinetics/inact_rec 0 0 0 0 0 0 0 0 1 0 "" 8 black -4 1 0
simundump kreac /kinetics/fast_inact 0 1 1 "" white black -1 3 0
simundump kreac /kinetics/rec_ligand_binding 0 0.1 0.1 "" white black -2 2 0
simundump kpool /kinetics/Ligand.Receptor 0 0 0 0 0 0 0 0 1 0 "" 0 black 3 2 \
  0
simundump kenz /kinetics/Ligand.Receptor/turn_off 0 0 0 0 0 1 0.1 0.4 0.1 0 0 \
  "" red 0 "" 0 1 0
simundump kpool /kinetics/Ligand 0 0 0 0 0 0 0 0 1 0 "" 48 black 4 1 0
simundump kreac /kinetics/basal_turnon 0 0.1 0 "" white black 2 1 0
simundump kreac /kinetics/Degrade_ligand 0 0.1 0 "" white black -2 1 0
simundump xgraph /graphs/conc1 0 0 99 0 0.79702 0
simundump xgraph /graphs/conc2 0 0 99 0 0.15921 0
simundump xplot /graphs/conc1/state.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 61 0 0 1
simundump xplot /graphs/conc1/Ligand.Receptor.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 0 0 0 1
simundump xplot /graphs/conc2/Ligand.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 48 0 0 1
simundump xplot /graphs/conc2/Receptor.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 28 0 0 1
simundump xgraph /moregraphs/conc3 0 0 100 0 1 0
simundump xgraph /moregraphs/conc4 0 0 100 0 1 0
simundump xcoredraw /edit/draw 0 -5 5 -4 6
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
addmsg /kinetics/basal_turnon /kinetics/state REAC B A 
addmsg /kinetics/state/Make_ligand /kinetics/state REAC eA B 
addmsg /kinetics/fast_inact /kinetics/state REAC A B 
addmsg /kinetics/Ligand.Receptor/turn_off /kinetics/state REAC sA B 
addmsg /kinetics/state /kinetics/state/Make_ligand ENZYME n 
addmsg /kinetics/substrate /kinetics/state/Make_ligand SUBSTRATE n 
addmsg /kinetics/basal_turnon /kinetics/basal REAC A B 
addmsg /kinetics/Ligand.Receptor/turn_off /kinetics/basal MM_PRD pA 
addmsg /kinetics/state/Make_ligand /kinetics/substrate REAC sA B 
addmsg /kinetics/Degrade_ligand /kinetics/substrate REAC B A 
addmsg /kinetics/fast_inact /kinetics/Receptor REAC A B 
addmsg /kinetics/rec_ligand_binding /kinetics/Receptor REAC A B 
addmsg /kinetics/fast_inact /kinetics/inact_rec REAC B A 
addmsg /kinetics/Receptor /kinetics/fast_inact SUBSTRATE n 
addmsg /kinetics/state /kinetics/fast_inact SUBSTRATE n 
addmsg /kinetics/inact_rec /kinetics/fast_inact PRODUCT n 
addmsg /kinetics/Ligand /kinetics/rec_ligand_binding SUBSTRATE n 
addmsg /kinetics/Receptor /kinetics/rec_ligand_binding SUBSTRATE n 
addmsg /kinetics/Ligand.Receptor /kinetics/rec_ligand_binding PRODUCT n 
addmsg /kinetics/rec_ligand_binding /kinetics/Ligand.Receptor REAC B A 
addmsg /kinetics/Ligand.Receptor/turn_off /kinetics/Ligand.Receptor REAC eA B 
addmsg /kinetics/Ligand.Receptor /kinetics/Ligand.Receptor/turn_off ENZYME n 
addmsg /kinetics/state /kinetics/Ligand.Receptor/turn_off SUBSTRATE n 
addmsg /kinetics/state/Make_ligand /kinetics/Ligand MM_PRD pA 
addmsg /kinetics/rec_ligand_binding /kinetics/Ligand REAC A B 
addmsg /kinetics/Degrade_ligand /kinetics/Ligand REAC A B 
addmsg /kinetics/basal /kinetics/basal_turnon SUBSTRATE n 
addmsg /kinetics/state /kinetics/basal_turnon PRODUCT n 
addmsg /kinetics/Ligand /kinetics/Degrade_ligand SUBSTRATE n 
addmsg /kinetics/substrate /kinetics/Degrade_ligand PRODUCT n 
addmsg /kinetics/state /graphs/conc1/state.Co PLOT Co *state.Co *61 
addmsg /kinetics/Ligand.Receptor /graphs/conc1/Ligand.Receptor.Co PLOT Co *Ligand.Receptor.Co *0 
addmsg /kinetics/Ligand /graphs/conc2/Ligand.Co PLOT Co *Ligand.Co *48 
addmsg /kinetics/Receptor /graphs/conc2/Receptor.Co PLOT Co *Receptor.Co *28 
enddump
// End of dump

call /kinetics/Ligand/notes LOAD \
"This molecule is freely diffusible."
complete_loading

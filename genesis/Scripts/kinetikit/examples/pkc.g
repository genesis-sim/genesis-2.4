//genesis
// kkit Version 2 dumpfile
 
// Saved on Thu Jun 27 11:31:08 1996
 
include kkit
 
FASTDT = 0.005
SIMDT = 0.005
CONTROLDT = 1
PLOTDT = 1
MAXTIME = 2000
kparms
 
//genesis

initdump -version 3 -ignoreorphans 1
simobjdump table input output alloced step_mode stepsize
simobjdump xtree path script namemode sizescale
simobjdump xcoredraw xmin xmax ymin ymax
simobjdump xtext editable
simobjdump xgraph xmin xmax ymin ymax overlay
simobjdump xplot pixflags flags script fg ysquish do_slope wy
simobjdump kpool CoTotal CoInit Co CoRemaining n nInit nTotal nMin vol \
  slave_enable keepconc notes xtree_fg_req xtree_textfg_req
simobjdump xtab input output alloced step_mode stepsize notes editfunc \
  xtree_fg_req xtree_textfg_req plotfield manageclass baselevel last_x last_y \
  is_running
simobjdump group notes editfunc xtree_fg_req xtree_textfg_req plotfield \
  manageclass expanded movealone
simobjdump kreac A B kf kb is_active notes editfunc xtree_fg_req \
  xtree_textfg_req plotfield manageclass
simobjdump kenz CoComplexInit CoComplex nComplexInit nComplex vol sA pA eA B \
  k1 k2 k3 keepconc usecomplex ksum notes editfunc xtree_fg_req \
  xtree_textfg_req plotfield manageclass link_to_manage
simundump group /kinetics/PKC 0 "" edit_group blue black x group 0 0
simundump kpool /kinetics/PKC/PKC-Ca 0 0.28282 3.7208e-17 0.11562 0.1672 \
  69374 2.2325e-11 1.6969e+05 0 6e+05 0 1 "" red black
simundump kreac /kinetics/PKC/PKC-act-by-Ca 0 34687 34687 1e-06 0.5 1 \
  "Need est of rate of assoc of Ca and PKC. Assume it is fast\nThe original parameter-searched kf of 439.4 has been\nscaled by 1/6e8 to account for change of units to n. Kf now 8.16e-7, kb=.6085\nRaised kf to 1e-6 to match Ca curve, kb to .5\n" \
  edit_reac white blue A group
simundump kreac /kinetics/PKC/PKC-act-by-DAG 0 83248 83248 1.3333e-08 8.6348 \
  1 \
  "Need est of rate. Assume it is fast\nObtained from param search\nkf raised 10 X : see Shinomura et al PNAS 88 5149-5153 1991.\nkf changed from 3.865e-7 to 2.0e-7 in line with closer analysis of\nShinomura data.\n26 June 1996: Corrected DAG data: reduce kf 15x from \n2e-7 to 1.333e-8" \
  edit_reac white blue A group
simundump kreac /kinetics/PKC/PKC-Ca-to-memb 0 88137 88137 1.2705 3.5026 1 "" \
  edit_reac white blue A group
simundump kreac /kinetics/PKC/PKC-DAG-to-memb 0 9641 9641 1 0.1 1 \
  "Raise kb from .087 to 0.1 to match data from Shinomura et al.\nLower kf from 1.155 to 1.0 to match data from Shinomura et al." \
  edit_reac white blue A group
simundump kreac /kinetics/PKC/PKC-act-by-Ca-AA 0 4162.4 4162.4 2e-09 0.1 1 \
  "Schaechter and Benowitz We have to increase Kf for conc scaling\nChanged kf to 2e-9 on Sept 19, 94. This gives better match.\n" \
  edit_reac white blue A group
simundump kreac /kinetics/PKC/PKC-act-by-DAG-AA 0 46827 46827 2 0.2 1 \
  "Assume slowish too. Schaechter and Benowitz" edit_reac white blue A group
simundump kpool /kinetics/PKC/PKC-DAG-AA* 0 0.12061 4.9137e-18 0.39023 \
  -0.26962 2.3414e+05 2.9482e-12 72366 0 6e+05 0 1 "" cyan blue
simundump kpool /kinetics/PKC/PKC-Ca-AA* 0 0.16961 1.75e-16 0.069374 0.10024 \
  41624 1.05e-10 1.0177e+05 0 6e+05 0 1 "" orange blue
simundump kpool /kinetics/PKC/PKC-Ca-memb* 0 0.10258 1.3896e-17 0.041939 \
  0.060644 25163 8.3376e-12 61549 0 6e+05 0 1 "" pink blue
simundump kpool /kinetics/PKC/PKC-DAG-memb* 0 0.023754 9.4352e-21 0.16068 \
  -0.13693 96410 5.6611e-15 14252 0 6e+05 0 1 "" yellow blue
simundump kpool /kinetics/PKC/PKC-basal* 0 0.0432 0.02 0.0019271 0.041273 \
  1156.2 12000 25920 0 6e+05 0 1 "" pink blue
simundump kreac /kinetics/PKC/PKC-basal-act 0 57812 57812 1 50 1 \
  "Initial basal levels were set by kf = 1, kb = 20. In model, though, the\nbasal levels of PKC activity are higher." \
  edit_reac white blue A group
simundump kpool /kinetics/PKC/PKC-AA* 0 1 1.8133e-17 0.0057812 0.99422 3468.7 \
  1.088e-11 6e+05 0 6e+05 0 1 "" cyan blue
simundump kreac /kinetics/PKC/PKC-act-by-AA 0 346.87 346.87 2e-10 0.1 1 \
  "Raise kf from 1.667e-10 to 2e-10 to get better match to data." edit_reac \
  white blue A group
simundump kpool /kinetics/PKC/PKC-Ca-DAG 0 0.0017993 8.4632e-23 0.016068 \
  -0.014269 9641 5.0779e-17 1079.6 0 6e+05 0 1 "" white blue
simundump kreac /kinetics/PKC/PKC-n-DAG 0 5203 5203 1e-09 0.1 1 \
  "kf raised 10 X based on Shinomura et al PNAS 88 5149-5153 1991\ncloser analysis of Shinomura et al: kf now 1e-8 (was 1.66e-8).\nFurther tweak. To get sufficient AA synergy, increase kf to 1.5e-8\n26 June 1996: Corrected DAG levels: reduce kf by 15x from\n1.5e-8 to 1e-9" \
  edit_reac white blue A group
simundump kpool /kinetics/PKC/PKC-DAG 0 0.08533 1.161e-16 0.086717 -0.0013868 \
  52030 6.9661e-11 51198 0 6e+05 0 1 "CoInit was .0624\n" white blue
simundump kreac /kinetics/PKC/PKC-n-DAG-AA 0 46827 46827 3e-08 2 1 \
  "Reduced kf to 0.66X to match Shinomura et al data.\nInitial: kf = 3.3333e-9\nNew: 2e-9\nNewer: 2e-8\nkb was 0.2\nnow 2." \
  edit_reac white blue A group
simundump kpool /kinetics/PKC/PKC-DAG-AA 0 0.012092 2.5188e-19 0.039023 \
  -0.02693 23414 1.5113e-13 7255.5 0 6e+05 0 1 "" white blue
simundump kpool /kinetics/PKC/PKC-cytosolic 0 1.5489 1 0.096353 1.4526 57812 \
  6e+05 9.2935e+05 0 6e+05 0 1 \
  "Marquez et al J. Immun 149,2560(92) est 1e6/cell for chromaffin cells\nWe will use 1 uM as our initial concen\n" \
  white blue
simundump kpool /kinetics/DAG 1 150 150 150 -111 9e+07 9e+07 9e+07 0 6e+05 6 \
  0 \
  "The conc of this has been a problem. Schaecter and Benowitz use 50 uM,\nbut Shinomura et al have < 5. So I have altered the DAG-dependent \nrates in the PKC model to reflect this." \
  green black
simundump kpool /kinetics/Ca 1 1 1 1 -9 6e+05 6e+05 6e+05 0 6e+05 6 0 "" red \
  black
simundump kpool /kinetics/AA 0 50 50 50 0 3e+07 3e+07 3e+07 0 6e+05 6 1 "" \
  darkgreen black
simundump kpool /kinetics/PKC-active 0 0.02 0 0.66993 5.5604e-08 4.0196e+05 \
  12000 4.0196e+05 0 6e+05 2 1 "Conc of PKC in brain is about 1 uM (?)" \
  yellow black
simundump kenz /kinetics/PKC-active/PKC-act-raf 1 0 5.5604e-08 0 0.033363 \
  6e+05 0.5338 0.13345 0.66725 0.66725 1.66e-06 16 4 0 0 20 \
  "Rate consts from Chen et al Biochem 32, 1032 (1993)\nk3 = k2 = 4\nk1 = 9e-5\nrecalculated gives 1.666e-5, which is not very different.\nLooks like k3 is rate-limiting in this case: there is a huge amount\nof craf locked up in the enz complex. Let us assume a 10x\nhigher Km, ie, lower affinity.  k1 drops by 10x.\nAlso changed k2 to 4x k3.\nLowerd k1 to 1e-6 to balance 10X DAG sensitivity of PKC" \
  edit_enz red yellow CoComplex kpool enzproto_to_pool_link
simundump xtab /kinetics/AA-conc-series 0 0 50 1 1 0 "" edit_xtab white red \
  output group 0 100 0.009902649474 3
loadtab /kinetics/AA-conc-series table 1 100 0 400 \
 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 10 10 10 10 10 10 10 10 10 10 10 \
 10 10 10 10 10 10 10 10 10 25 25 25 25 25 25 25 25 25 25 25 25 25 25 25 25 \
 25 25 25 25 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 100 \
 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100
simundump xtab /kinetics/Ca-conc-series 0 0 1 1 1 0 "" edit_xtab white red \
  output group 1e-6 0 0 3
loadtab /kinetics/Ca-conc-series table 1 100 0 400 \
 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 \
 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.001 0.01 0.01 0.01 0.01 \
 0.01 0.01 0.01 0.01 0.01 0.01 0.01 0.01 0.01 0.01 0.01 0.01 0.01 0.01 0.01 \
 0.01 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 \
 0.1 0.1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 10 10 10 10 10 10 10 10 10 \
 10 10 10 10 10 10 10 10 10 10 10
simundump xtab /kinetics/Ca-conc-step 0 99.755 1 1 1 0.005 "" edit_xtab white \
  red output group 1e-4 0 0 3
loadtab /kinetics/Ca-conc-step table 1 100 0 100 \
 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 \
 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 \
 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 1 1 1 1 1 1 1 1 1 1 1 1 \
 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
simundump xtab /kinetics/DAG-conc-series 1 0 18 1 1 0 \
  "A remarkably small conc range." edit_xtab white red output group 0 0 0 3
loadtab /kinetics/DAG-conc-series table 1 100 0 2000 \
 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 18 18 18 18 18 18 18 18 18 18 18 \
 18 18 18 18 18 18 18 18 18 37.5 37.5 37.5 37.5 37.5 37.5 37.5 37.5 37.5 37.5 \
 37.5 37.5 37.5 37.5 37.5 37.5 37.5 37.5 37.5 37.5 75 75 75 75 75 75 75 75 75 \
 75 75 75 75 75 75 75 75 75 75 75 112.5 112.5 112.5 112.5 112.5 112.5 112.5 \
 112.5 112.5 112.5 112.5 112.5 112.5 112.5 112.5 112.5 112.5 112.5 112.5 \
 112.5
simundump xgraph /graphs/conc1 0 0 2000 0 130.24 0
simundump xgraph /graphs/conc2 0 0 2000 0 0.595 0
simundump xplot /graphs/conc1/AA.Co 3 524288 3 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" darkgreen 0.55 0 1
simundump xplot /graphs/conc1/DAG.Co 3 524288 3 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" green 0.55 0 1
simundump xplot /graphs/conc2/Ca.Co 3 524288 3 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0.005 0 1
simundump xplot /graphs/conc2/PKC-active.Co 3 524288 3 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" yellow 0.005 0 1
simundump xplot /graphs/conc2/PKC-DAG.Co 3 524288 3 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" white 0.005 0 1
simundump xgraph /moregraphs/conc3 0 0 2000 0 1 0
simundump xgraph /moregraphs/conc4 0 0 2000 0 1 0
simundump xcoredraw /edit/draw 0 -12.316 7.6873 -10.818 9.1849
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
  "echo.p dragging <S> to <D> for product" "reac_to_pool_add.p <S> <D>" "reac_to_pool_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kreac] SUBSTRATE     kreac   green -1005 0 \
  "echo.p dragging <S> to <D> for substrate" "pool_to_reac_add.p <S> <D>" "pool_to_reac_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kreac]   PRODUCT     kreac   green -1006 1 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kenz] /kinetics/##[TYPE=kpool]    MM_PRD     kpool     red -1007 0 \
  "echo.p dragging <S> to <D> for enz product" "enz_to_pool_add.p <S> <D>" "enz_to_pool_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kenz] SUBSTRATE      kenz     red -1008 0 \
  "echo.p dragging <S> to <D> for enz substrate" "pool_to_enz_add.p <S> <D>" "pool_to_enz_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kenz] /kinetics/##[TYPE=kpool]  SUMTOTAL     kpool darkgreen -1009 0 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kenz] /kinetics/##[TYPE=kpool]  CONSERVE     kpool    blue -1010 0 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kenz]  INTRAMOL      kenz   white -1011 0 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=stim] /kinetics/##[TYPE=kpool]     SLAVE     kpool  orange -1012 0 \
  "echo.p dragging <S> to <D> for stimulus" "stim_to_pool_add.p <S> <D>" "stim_to_pool_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=stim] /kinetics/##[TYPE=stim]     INPUT      stim  orange -1013 0 \
  "echo.p dragging <S> to <D> for stim" "stim_to_stim_add.p <S> <D>" "stim_to_stim_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=xtab] /kinetics/##[TYPE=kpool]     SLAVE     kpool  orange -1014 0 \
  "echo.p dragging <S> to <D> for xtab" "xtab_to_pool_add.p <S> <D>" "xtab_to_pool_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=xtab] /kinetics/##[TYPE=xtab]     INPUT      xtab  orange -1015 0 \
  "echo.p dragging <S> to <D> for xtab" "xtab_to_xtab_add.p <S> <D>" "xtab_to_xtab_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=xtab]     INPUT      xtab hotpink -1016 0 \
  "echo.p dragging <S> to <D> for xtab" "pool_to_xtab_add.p <S> <D>" "pool_to_xtab_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kchan] /kinetics/##[TYPE=kpool]      REAC     kpool    none -1017 0 \
  "echo.p dragging <S> to <D> for chan product" "chan_to_pool_add.p <S> <D>" "chan_to_pool_drop.p <S> <D>"
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kchan]   PRODUCT     kchan  orange -1018 1 \
  "" "" ""
call /edit/draw/tree ADDMSGARROW \
/kinetics/##[TYPE=kpool] /kinetics/##[TYPE=kchan] SUBSTRATE     kchan  orange -1019 0 \
  "echo.p dragging <S> to <D> for chan substrate" "pool_to_chan_add.p <S> <D>" "pool_to_chan_drop.p <S> <D>"
call /edit/draw/tree UNDUMP 31 \
 "/kinetics/PKC" -3.04929 8.21631 0 \
 "/kinetics/PKC/PKC-Ca" -4.07517 1.51085 0 \
 "/kinetics/PKC/PKC-act-by-Ca" -4.07517 -0.122947 0 \
 "/kinetics/PKC/PKC-act-by-DAG" -2.06118 0.693953 0 \
 "/kinetics/PKC/PKC-Ca-to-memb" -3.79738 4.25331 0 \
 "/kinetics/PKC/PKC-DAG-to-memb" -2.61677 2.73621 0 \
 "/kinetics/PKC/PKC-act-by-Ca-AA" -0.78797 3.81568 0 \
 "/kinetics/PKC/PKC-act-by-DAG-AA" 1.24916 3.23218 0 \
 "/kinetics/PKC/PKC-DAG-AA*" 0.600985 5.53701 0 \
 "/kinetics/PKC/PKC-Ca-AA*" -0.602776 6.29557 0 \
 "/kinetics/PKC/PKC-Ca-memb*" -2.77881 6.52897 0 \
 "/kinetics/PKC/PKC-DAG-memb*" -1.82969 5.50784 0 \
 "/kinetics/PKC/PKC-basal*" -4.7465 5.56619 0 \
 "/kinetics/PKC/PKC-basal-act" -4.97799 3.05713 0 \
 "/kinetics/PKC/PKC-AA*" 1.78159 6.82072 0 \
 "/kinetics/PKC/PKC-act-by-AA" -4.99245 -1.86537 0 \
 "/kinetics/PKC/PKC-Ca-DAG" 0.230597 1.8026 0 \
 "/kinetics/PKC/PKC-n-DAG" -3.0103 -1.99016 0 \
 "/kinetics/PKC/PKC-DAG" -0.996314 -1.08573 0 \
 "/kinetics/PKC/PKC-n-DAG-AA" -1.22781 -2.95294 0 \
 "/kinetics/PKC/PKC-DAG-AA" 0.624134 0.227153 0 \
 "/kinetics/PKC/PKC-cytosolic" -6.13155 0.597108 0 \
 "/kinetics/DAG" -3.20515 -4.71675 0 \
 "/kinetics/Ca" -8.3874 -2.76345 0 \
 "/kinetics/AA" -3.28976 -9.33758 0 \
 "/kinetics/PKC-active" 2.13247 8.47704 0 \
 "/kinetics/PKC-active/PKC-act-raf" 6.26075 9.14727 0 \
 "/kinetics/AA-conc-series" -1.20937 -11.7853 0 \
 "/kinetics/Ca-conc-series" -8.86992 -6.55545 0 \
 "/kinetics/Ca-conc-step" -7.27763 -4.96314 0 \
 "/kinetics/DAG-conc-series" 1.83714 -5.91609 0
simundump xtext /file/notes 0 1
xtextload /file/notes \
"Corrected DAG levels. 15x scale factor" \
"applied. Going through dose-R " \
"curves to check that all is still well."
addmsg /kinetics/PKC/PKC-act-by-Ca /kinetics/PKC/PKC-Ca REAC B A 
addmsg /kinetics/PKC/PKC-act-by-DAG /kinetics/PKC/PKC-Ca REAC A B 
addmsg /kinetics/PKC/PKC-Ca-to-memb /kinetics/PKC/PKC-Ca REAC A B 
addmsg /kinetics/PKC/PKC-act-by-Ca-AA /kinetics/PKC/PKC-Ca REAC A B 
addmsg /kinetics/PKC/PKC-cytosolic /kinetics/PKC/PKC-act-by-Ca SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/PKC/PKC-act-by-Ca SUBSTRATE n 
addmsg /kinetics/PKC/PKC-Ca /kinetics/PKC/PKC-act-by-Ca PRODUCT n 
addmsg /kinetics/DAG /kinetics/PKC/PKC-act-by-DAG SUBSTRATE n 
addmsg /kinetics/PKC/PKC-Ca /kinetics/PKC/PKC-act-by-DAG SUBSTRATE n 
addmsg /kinetics/PKC/PKC-Ca-DAG /kinetics/PKC/PKC-act-by-DAG PRODUCT n 
addmsg /kinetics/PKC/PKC-Ca /kinetics/PKC/PKC-Ca-to-memb SUBSTRATE n 
addmsg /kinetics/PKC/PKC-Ca-memb* /kinetics/PKC/PKC-Ca-to-memb PRODUCT n 
addmsg /kinetics/PKC/PKC-Ca-DAG /kinetics/PKC/PKC-DAG-to-memb SUBSTRATE n 
addmsg /kinetics/PKC/PKC-DAG-memb* /kinetics/PKC/PKC-DAG-to-memb PRODUCT n 
addmsg /kinetics/PKC/PKC-Ca /kinetics/PKC/PKC-act-by-Ca-AA SUBSTRATE n 
addmsg /kinetics/AA /kinetics/PKC/PKC-act-by-Ca-AA SUBSTRATE n 
addmsg /kinetics/PKC/PKC-Ca-AA* /kinetics/PKC/PKC-act-by-Ca-AA PRODUCT n 
addmsg /kinetics/PKC/PKC-DAG-AA* /kinetics/PKC/PKC-act-by-DAG-AA PRODUCT n 
addmsg /kinetics/PKC/PKC-DAG-AA /kinetics/PKC/PKC-act-by-DAG-AA SUBSTRATE n 
addmsg /kinetics/PKC/PKC-act-by-DAG-AA /kinetics/PKC/PKC-DAG-AA* REAC B A 
addmsg /kinetics/PKC/PKC-act-by-Ca-AA /kinetics/PKC/PKC-Ca-AA* REAC B A 
addmsg /kinetics/PKC/PKC-Ca-to-memb /kinetics/PKC/PKC-Ca-memb* REAC B A 
addmsg /kinetics/PKC/PKC-DAG-to-memb /kinetics/PKC/PKC-DAG-memb* REAC B A 
addmsg /kinetics/PKC/PKC-basal-act /kinetics/PKC/PKC-basal* REAC B A 
addmsg /kinetics/PKC/PKC-cytosolic /kinetics/PKC/PKC-basal-act SUBSTRATE n 
addmsg /kinetics/PKC/PKC-basal* /kinetics/PKC/PKC-basal-act PRODUCT n 
addmsg /kinetics/PKC/PKC-act-by-AA /kinetics/PKC/PKC-AA* REAC B A 
addmsg /kinetics/AA /kinetics/PKC/PKC-act-by-AA SUBSTRATE n 
addmsg /kinetics/PKC/PKC-AA* /kinetics/PKC/PKC-act-by-AA PRODUCT n 
addmsg /kinetics/PKC/PKC-cytosolic /kinetics/PKC/PKC-act-by-AA SUBSTRATE n 
addmsg /kinetics/PKC/PKC-act-by-DAG /kinetics/PKC/PKC-Ca-DAG REAC B A 
addmsg /kinetics/PKC/PKC-DAG-to-memb /kinetics/PKC/PKC-Ca-DAG REAC A B 
addmsg /kinetics/PKC/PKC-cytosolic /kinetics/PKC/PKC-n-DAG SUBSTRATE n 
addmsg /kinetics/DAG /kinetics/PKC/PKC-n-DAG SUBSTRATE n 
addmsg /kinetics/PKC/PKC-DAG /kinetics/PKC/PKC-n-DAG PRODUCT n 
addmsg /kinetics/PKC/PKC-n-DAG /kinetics/PKC/PKC-DAG REAC B A 
addmsg /kinetics/PKC/PKC-n-DAG-AA /kinetics/PKC/PKC-DAG REAC A B 
addmsg /kinetics/PKC/PKC-DAG /kinetics/PKC/PKC-n-DAG-AA SUBSTRATE n 
addmsg /kinetics/AA /kinetics/PKC/PKC-n-DAG-AA SUBSTRATE n 
addmsg /kinetics/PKC/PKC-DAG-AA /kinetics/PKC/PKC-n-DAG-AA PRODUCT n 
addmsg /kinetics/PKC/PKC-n-DAG-AA /kinetics/PKC/PKC-DAG-AA REAC B A 
addmsg /kinetics/PKC/PKC-act-by-DAG-AA /kinetics/PKC/PKC-DAG-AA REAC A B 
addmsg /kinetics/PKC/PKC-act-by-Ca /kinetics/PKC/PKC-cytosolic REAC A B 
addmsg /kinetics/PKC/PKC-basal-act /kinetics/PKC/PKC-cytosolic REAC A B 
addmsg /kinetics/PKC/PKC-act-by-AA /kinetics/PKC/PKC-cytosolic REAC A B 
addmsg /kinetics/PKC/PKC-n-DAG /kinetics/PKC/PKC-cytosolic REAC A B 
addmsg /kinetics/PKC/PKC-act-by-DAG /kinetics/DAG REAC A B 
addmsg /kinetics/PKC/PKC-n-DAG /kinetics/DAG REAC A B 
addmsg /kinetics/DAG-conc-series /kinetics/DAG SLAVE output 
addmsg /kinetics/PKC/PKC-act-by-Ca /kinetics/Ca REAC A B 
addmsg /kinetics/Ca-conc-series /kinetics/Ca SLAVE output 
addmsg /kinetics/PKC/PKC-act-by-Ca-AA /kinetics/AA REAC A B 
addmsg /kinetics/PKC/PKC-act-by-AA /kinetics/AA REAC A B 
addmsg /kinetics/PKC/PKC-n-DAG-AA /kinetics/AA REAC A B 
addmsg /kinetics/AA-conc-series /kinetics/AA SLAVE output 
addmsg /kinetics/PKC/PKC-DAG-AA* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC/PKC-Ca-memb* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC/PKC-Ca-AA* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC/PKC-DAG-memb* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC/PKC-basal* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC/PKC-AA* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC-active/PKC-act-raf /kinetics/PKC-active REAC sA B 
addmsg /kinetics/PKC-active/PKC-act-raf /kinetics/PKC-active CONSERVE nComplex nComplexInit 
addmsg /kinetics/PKC-active /kinetics/PKC-active/PKC-act-raf ENZYME n 
addmsg /kinetics/AA /graphs/conc1/AA.Co PLOT Co *AA.Co *darkgreen 
addmsg /kinetics/DAG /graphs/conc1/DAG.Co PLOT Co *DAG.Co *green 
addmsg /kinetics/Ca /graphs/conc2/Ca.Co PLOT Co *Ca.Co *red 
addmsg /kinetics/PKC-active /graphs/conc2/PKC-active.Co PLOT Co *PKC-active.Co *yellow 
addmsg /kinetics/PKC/PKC-DAG /graphs/conc2/PKC-DAG.Co PLOT Co *PKC-DAG.Co *white 
enddump
// End of dump

call /edit/draw/tree RESET
reset

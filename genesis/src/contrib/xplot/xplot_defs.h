/*
 * FILE: xplot_defs.h
 *
 */

#ifndef XPLOT_DEFS_H
#define XPLOT_DEFS_H


#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

#define FROM_FILE       0
#define FROM_OTHER      1

#define NORMAL_GRAPH    0
#define BAR_GRAPH       1
#define SCATTER_GRAPH   2
#define STEP_GRAPH      3

#define XMODE           0
#define YMODE           1

#define BOX_PT          0
#define FBOX_PT         1
#define CIRCLE_PT       2
#define FCIRCLE_PT      3
#define TRIANGLE_PT     4
#define FTRIANGLE_PT    5
#define X_PT            6
#define CROSS_PT        7
#define DOT             8

#define LINEAR_AXIS     0
#define LOG10_AXIS      1

#define FRAME_WINDOW    1
#define GRAPH_WINDOW    2
#define TEXT_WINDOW     3

#define SOLID_LINE      0
#define MAX_TEXT_LINE   100

#define WORLD_LBL       0
#define SCREEN_LBL      1
#define WINDOW_LBL      2

#define TEMPORARY_LBL   0
#define PERMANENT_LBL   1

#define STRING_LBL      0
#define BOX_LBL         1
#define LINE_LBL        2

/*
 * define button modes
 */

#define DATAMODE        0
#define DRAWMODE        1
#define TEXTMODE        2
#define ZAPMODE         3
#define PINCHMODE       4

/*
 * define item types
 */

#define BUTTON          0
#define TOGGLE          1

#endif  /* XPLOT_DEFS_H */




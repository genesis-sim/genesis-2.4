/*
 * FILE: xplot_struct.h
 *
 */

#ifndef XPLOT_STRUCT_H
#define XPLOT_STRUCT_H


#include "xplot_defs.h"

/*
 * basic window stuctures
 */

#define WINDOW_STRUCT               \
    char           *windowname;     \
    char           *display_name;   \
    Display        *display;        \
    GC              context;        \
    Drawable        drawable;       \
    Window          window;         \
    Visual         *visual;         \
    int             screen_number;  \
    Font            font;           \
    XFontStruct    *fontinfo;       \
    int             wwidth;         \
    int             wheight;        \
    char           *fontname;       \
    int             fontwidth;      \
    int             fontheight;     \
    int             linewidth;      \
    unsigned long   background;     \
    unsigned long   foreground;     \
    unsigned long   color;          \
    int             color_mode;     \
    int             inverse;        \
    Label          *label;          \
    int             mapped;


struct stringlabel
{
    float wx, wy;   /* text start */
    short sx, sy;
    char line[MAX_TEXT_LINE];
};


struct boxlabel
{
    float wx, wy, wr;   /* box center/size */
    short sx, sy, sr;
};


struct linelabel
{
    float wx, wy;   /* line start */
    float wx2, wy2; /* line end */
    short sx, sy;
    short sx2, sy2;
};


typedef struct labeltype
{
    short coord_mode;       /* 0=world coord; 1=screen coord */
    short color;
    struct labeltype *next;
    short priority;         /* 0 = temporary; 1 = permanent  */
    short type;             /* 0 = string; 1 = box; 2 = line */
    union
    {
        struct stringlabel string;
        struct boxlabel    box;
        struct linelabel   line;
    }
    u;
}
Label;


typedef struct
{
    WINDOW_STRUCT
}
BasicWindow;


typedef struct
{
    short x, y;
}
Coord;


typedef struct
{
    float x, y;
}
FCoord;


typedef struct
{
    float value;
}
ErrorData;


typedef struct source_type
{
    char *filename;
    int   type;
    BasicWindow *parent;
    struct source_type *next;
}
DataSource;


typedef struct plot_type
{
    int         datasource;     /* 0 = file; 1 = stdin; 2 = other */
    char       *filename;       /* data file information */
    char       *title;          /* line title */
    int         linestyle;      /* line style */
    float       xmax;           /* scale parameters */
    float       xmin;
    float       ymax;
    float       ymin;
    int         xhi;            /* data limiting bounds */
    int         xlo;
    int         npoints;        /* number of data points */
    int         arraysize;
    FCoord     *data;           /* original data */
    Coord      *coord;          /* plot data */
    FCoord     *fcoord;         /* precise plot data */
    ErrorData  *errorbar;       /* error bar data */
    struct plot_type  *next;
    struct graph_type *graph;   /* parent graph */
    int         selected;       /* flag */
    int         visible;        /* flag */
    float       scale;          /* data scale factor */
    float       xscale;         /* x scale factor */
    float       escale;         /* error scale factor */
    float       yoffset;        /* data y offset */
    float       xaxisoffset;    /* x axis offset */
    float       yaxisoffset;    /* y axis offset */
    float       xoffset;        /* data x offset */
    int         plot_type;      /* scatter/bar/normal */
    float       bar_width;      /* width in world coords */
    int         bar_filled;     /* flag to fill the bars */
    int         show_error;     /* flag to display error bars */
    int         point_symbol;   /* icon to use to mark points */
    int         showpoints;     /* flag to show point icons */
    int         pointfreq;      /* how often to mark points */
    float       auto_x;         /* auto x index */
    float       auto_xstart;    /* start for auto x index */
    int         xaxis;          /* flag */
    int         yaxis;          /* flag */
}
Plot;


typedef struct graph_type
{
    WINDOW_STRUCT
    struct _Frame      *frame;
    Plot               *plot;
    DataSource         *source;
    struct graph_type  *next;
    float   dragwx1, dragwy1;
    int     dragx1, dragy1, dragx2, dragy2;
    char   *title;                  /* plot title */
    int     autoscale_x,            /* autoscale flags */
            autoscale_y;
    int     xaxis_type,             /* linear/log */
            yaxis_type;
    int     show_xaxis,             /* axis display flags */
            show_yaxis;
    int     xaxis_exponent,         /* axis label exponent */
            yaxis_exponent;
    int     xaxis_leftdp,           /* axis decimal precision */
            yaxis_leftdp;
    int     xaxis_rightdp,          /* axis decimal precision */
            yaxis_rightdp;
    char   *xaxis_title;            /* axis labels */
    char   *yaxis_title;
    int     xaxis_style,            /* axis style */
            yaxis_style;
    float   xaxis_yintcpt,          /* axis intercepts */
            yaxis_xintcpt;
    float   xaxis_tickmin,          /* tick information */
            yaxis_tickmin;
    int     xaxis_nticks,           /* actual number of ticks */
            yaxis_nticks;
    int     xaxis_nsubticks,        /* number of subticks */
            yaxis_nsubticks;
    int     xaxis_desired_nticks,   /* desired number of ticks */
            yaxis_desired_nticks;
    float   xaxis_tickinc,          /* tick spacing */
            yaxis_tickinc;
    float   xaxis_desired_tickinc,  /* desired tick spacing */
            yaxis_desired_tickinc;
    float   xaxis_tickstart,        /* starting tick */
            yaxis_tickstart;
    int     ticksize;               /* size of ticks in pixels */
    float   wxmin, wxmax;           /* world screen coordinates */
    float   wymin, wymax;
    float   wxscale, wyscale;       /* screen coord scale factor */
    float   xmin, xmax;             /* min/max data coordinates */
    float   ymin, ymax;
    int     show_xlabels;           /* flag to show labels */
    int     show_ylabels;           /* flag to show labels */
    int     show_legend;            /* flag to show legend */
    int     show_xgrid;             /* flag to show grid   */
    int     show_ygrid;
    int     show_title;             /* flag to show title  */
    int     auto_axes;              /* flag for auto axes  */
    int     xaxistitle_offset;
    int     yaxistitle_offset;
    int     xticklabel_offset;
    int     yticklabel_offset;
    int     resolution;             /* postscript resolution 0/1 */
    int     quadrants;              /* axis quadrants to display */
    int     gridcolor;
    int     truncate_tick_values;   /* flag */
}
Graph;


typedef struct
{
    int id;
    PFI func;
    BasicWindow *parent;
}
WindowData;


typedef struct
{
    WINDOW_STRUCT
    struct _Frame *frame;
    char line[MAX_TEXT_LINE];
    int index;
}
TextWindow;


typedef struct _Frame
{
    WINDOW_STRUCT
    Graph *graph;
    TextWindow *text;
    int x, y;
    int width, height;
}
Frame;


#endif  /* XPLOT_STRUCT_H */


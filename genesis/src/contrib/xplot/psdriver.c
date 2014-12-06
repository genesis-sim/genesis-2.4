/*
 * FILE: psdriver.c
 *
 */

/*
 *           PSdriver
 *           X11 -> postscript library routines
 *           Copyright 1989, 1990  Matt Wilson
 *           California Institute of Technology
 *           wilson@smaug.cns.caltech.edu
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation, and that the name of the California Institute
 * of Technology not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  Neither the author nor California Institute of Technology
 * make any representations about the suitability of this software for
 * any purpose. It is provided "as is" without express or implied warranty.
 *
 * To use these functions
 * replace the X11 calls in your code with calls to the
 * comparable driver functions.
 * e.g. instead of
 *   XDrawLine (display, drawable, context, x1, y1, x2, y2)
 * use
 *   XPSDrawLine (display, drawable, context, x1, y1, x2, y2)
 *
 * output can be directed to a file or directly to a printer using
 *   SetPSFileOutput(state)          1=file 0=printer
 *   SetPSFilename(name)
 *
 * to produce postscript, output add this sequence of calls to your code
 *
 * PreparePS(display, window, scale, box, header);
 * refresh_window(window);           your routine to refresh the screen
 * FinishPS();
 *
 */

#include <stdio.h>
#include <math.h>

/* CHECKME: is this still necessary?  If so, FIXME! */
#define XLIB_ILLEGAL_ACCESS
#define __EMX__
#include <X11/Xlib.h>
#undef XLIB_ILLEGAL_ACCESS

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*
 * This is lame: popen() and pclose() are defined in stdio.h, but
 * only under certain conditions.
 */

extern FILE *popen(const char *command, const char *type);
extern int   pclose(FILE *stream);



/* CHECKME: is this still true? */
/* stdlib.h conflicts with gcc on Red Hat Linux, so we don't include it. */
extern char *getenv(const char *name);

typedef struct
{
    short   x, y;
}
Coord;

typedef struct
{
    float   x, y;
}
FCoord;

#define XOUT    0
#define PSOUT   1

/* max points in a single multiple line vector call in X windows */
#define MAXCHUNK 10000

/* max strokes in a single postscript path */
#define MAXSTROKES 200

#define DPI 300
#define PAGEHEIGHT  11.0
#define PAGEWIDTH   8.5
/* default user space coords are 1/72 of an inch each */
#define DEFAULTRES  72

static int output_flag = XOUT;
static FILE *PSfp;
static int global_window_height;
static int global_window_width;
static float pix_scale;
static float page_scale;
static short file_output = 0;
static char *ps_filename;
static char *ps_filemode = "w";
static int current_gray = -1;
static int current_line = -1;
static int max_gray = 256;
static int inverse = 1;


void
SetPSInverse(int state)
{
    inverse = state;
}


void
SetPSFilename(char *name)
{
    ps_filename = name;
}


void
SetPSFilemode(char *mode)
{
    ps_filemode = mode;
}


void
SetPSFileOutput(int state)
{
    file_output = state;
}


void
SetMaxGray(int val)
{
    max_gray = val;
}


float
ComputeGray(int pixel)
{
    return (1.0 - (float)pixel / max_gray);
}


void
PSSetPixel(Display *display, int pixel)
{
    if (pixel != current_gray)
    {
        current_gray = pixel;

        if (pixel == XBlackPixel(display, XDefaultScreen(display)))
        {
            fprintf(PSfp, "%d G\n", inverse);
        }
        else if (pixel == XWhitePixel(display, XDefaultScreen(display)))
        {
            fprintf(PSfp, "%d G\n", !inverse);
        }
        else
        {
            fprintf(PSfp, "%5.3f G\n", ComputeGray(pixel));
        }
    }
}


void
PSSetLineWidth(int width)
{
    if (width == 0)
        width = 1;

    if (current_line != width)
    {
        current_line = width;
        fprintf(PSfp, "%f setlinewidth\n", current_line / pix_scale);
    }
}


void
PSClosefill(void)
{
    fprintf(PSfp, "C\n");
}


void
PSNewpath(void)
{
    fprintf(PSfp, "N\n");
}


void
PSStroke(void)
{
    fprintf(PSfp, "S\n");
}


void
PSMoveto(int x, int y)
{
    fprintf(PSfp, "%d %d M\n", x, global_window_height - y);
}


void
PSLineto(int x, int y)
{
    fprintf(PSfp, "%d %d L\n", x, global_window_height - y);
}


void
PSMovetoFloat(float x, float y)
{
    fprintf(PSfp, "%g %g M\n", x, global_window_height - y);
}


void
PSLinetoFloat(float x, float y)
{
    fprintf(PSfp, "%g %g L\n", x, global_window_height - y);
}


void
PSArc(float x, float y, float r, float a1, float a2)
{
    if ((a1 < 0) || (a2 < 0))
    {
        a1 = fabs(a1);
        a2 = fabs(a2);
        fprintf(PSfp, "%g %g %g %g %g AN\n",
                x, global_window_height - y, r,
                a1, a2);
    }
    else
    {
        fprintf(PSfp, "%g %g %g %g %g A\n",
                x, global_window_height - y, r,
                a1, a2);
    }
}


void
PSShow(char *s)
{
    fprintf(PSfp, "(%s) show\n", s);
}


void
PSFont(int height)
{
    fprintf(PSfp, "/Helvetica findfont %d scalefont setfont\n", height);
}


void
XPSDrawText(Display *display, Drawable drawable, GC context, int x, int y,
            char *s)
{
    XFontStruct *finfo;
    int height;

    if (s == NULL)
        return;

    if (output_flag == XOUT)
    {
        XDrawImageString(display, drawable, context, x, y, s, strlen(s));
    }
    else
    {
        finfo = XQueryFont(display, context->gid);
        height = finfo->ascent + finfo->descent;
        PSFont(height);
        PSMoveto(x, y);
        PSShow(s);
    }
}


void
XPSDrawDot(Display *display, Drawable drawable, GC context, int x1, int y1)
{
    if (output_flag == XOUT)
    {
        XDrawLine(display, drawable, context, x1, y1, x1, y1 + 1);
    }
    else
    {
        /* Hack: this isn't a good way to draw PS dots */
        XGCValues values;

        XGetGCValues(display, context, GCForeground | GCLineWidth, &values);
        PSSetPixel(display, values.foreground);
        PSSetLineWidth(values.line_width);
        PSNewpath();
        PSMoveto(x1, y1);
        PSLineto(x1, y1 + 1);
        PSStroke();
    }
}


void
XPSDrawLine(Display *display, Drawable drawable, GC context,
            int x1, int y1, int x2, int y2)
{
    if (output_flag == XOUT)
    {
        XDrawLine(display, drawable, context, x1, y1, x2, y2);
    }
    else
    {
        /*
         * get the current foreground color from the graphics context
         */

        XGCValues values;

        XGetGCValues(display, context, GCForeground | GCLineWidth, &values);
        PSSetPixel(display, values.foreground);
        PSSetLineWidth(values.line_width);
        PSNewpath();
        PSMoveto(x1, y1);
        PSLineto(x2, y2);
        PSStroke();
    }
}


void
XPSDrawLines(Display *display, Drawable drawable,
             GC context, Coord *coord, int ncoords, int mode)
{
    int i;
    int pcount;
    int nchunks;
    int chunksize;

    if (output_flag == XOUT)
    {
        /*
         * avoid the limit on the length of a multiple line vector
         * by doing it in multiple calls
         */

        nchunks = (ncoords - 1) / MAXCHUNK + 1;

        for (i = 0; i < nchunks; i++)
        {
            if ((chunksize = (ncoords - i * MAXCHUNK)) > MAXCHUNK)
            {
                chunksize = MAXCHUNK;
            }

            /*
             * Draw one point past to connect this chunk with the
             * next.  Don't do it for the last chunk.
             */

            if (i < nchunks - 1)
                chunksize++;

            XDrawLines(display, drawable, context,
                       (XPoint *)(coord + i * MAXCHUNK), chunksize, mode);
        }
    }
    else
    {
        XGCValues values;

        XGetGCValues(display, context, GCForeground | GCLineWidth, &values);

        if (ncoords <= 0)
        {
            return;
        }

        PSNewpath();
        PSSetPixel(display, values.foreground);
        PSSetLineWidth(values.line_width);
        PSMoveto(coord[0].x, coord[0].y);
        pcount = 0;

        for (i = 1; i < ncoords; i++)
        {
            if ((coord[i].x == coord[i - 1].x)
                && (coord[i].y == coord[i - 1].y) && i < ncoords - 1)
            {
                continue;
            }

            PSLineto(coord[i].x, coord[i].y);

            /*
             * break it up into managable cuhnks
             */

            if (pcount > MAXSTROKES)
            {
                PSStroke();
                PSNewpath();
                PSMoveto(coord[i].x, coord[i].y);
                pcount = 0;
            }

            pcount++;
        }

        PSStroke();
    }
}


void
XPSDrawLinesFloat(Display *display, Drawable drawable, GC context,
                  Coord *coord, int ncoords, int mode, FCoord *fcoord)
{
    int i;
    int pcount;
    int nchunks;
    int chunksize;

    if (output_flag == XOUT)
    {
        /*
         * avoid the limit on the length of a multiple line vector
         * by doing it in multiple calls
         */

        nchunks = (ncoords - 1) / MAXCHUNK + 1;

        for (i = 0; i < nchunks; i++)
        {
            if ((chunksize = ncoords - i * MAXCHUNK) > MAXCHUNK)
            {
                chunksize = MAXCHUNK;
            }

            /*
             * Draw one point past to connect this chunk with the
             * next. Don't do it for the last chunk.
             */

            if (i < nchunks - 1)
                chunksize++;

            XDrawLines(display, drawable, context,
                       (XPoint *)(coord + i * MAXCHUNK), chunksize, mode);
        }
    }
    else
    {
        XGCValues values;

        XGetGCValues(display, context, GCForeground | GCLineWidth, &values);

        if (ncoords <= 0)
        {
            return;
        }

        PSNewpath();
        PSSetPixel(display, values.foreground);
        PSSetLineWidth(values.line_width);
        PSMovetoFloat(fcoord[0].x, fcoord[0].y);
        pcount = 0;

        for (i = 1; i < ncoords; i++)
        {
            if ((fcoord[i].x == fcoord[i - 1].x) &&
                (fcoord[i].y == fcoord[i - 1].y) && i < ncoords - 1)
            {
                continue;
            }

            PSLinetoFloat(fcoord[i].x, fcoord[i].y);

            /*
             * break it up into managable cuhnks
             */

            if (pcount > MAXSTROKES)
            {
                PSStroke();
                PSNewpath();
                PSMovetoFloat(fcoord[i].x, fcoord[i].y);
                pcount = 0;
            }

            pcount++;
        }

        PSStroke();
    }
}


void
XPSDrawRectangle(Display *display, Drawable drawable, GC context, int x,
                 int y, int w, int h)
{
    if (output_flag == XOUT)
    {
        XDrawRectangle(display, drawable, context, x, y, w, h);
    }
    else
    {
        XGCValues values;

        XGetGCValues(display, context, GCForeground | GCLineWidth, &values);
        PSSetPixel(display, values.foreground);
        PSSetLineWidth(values.line_width);
        PSNewpath();
        PSMoveto(x, y);
        PSLineto(x + w, y);
        PSLineto(x + w, y + h);
        PSLineto(x, y + h);
        PSLineto(x, y);
        PSStroke();
    }
}


void
XPSFillRectangle(Display *display, Drawable drawable, GC context,
                 int x, int y, int w, int h)
{
    if (output_flag == XOUT)
    {
        XFillRectangle(display, drawable, context, x, y, w, h);
    }
    else
    {
        XGCValues values;

        XGetGCValues(display, context, GCForeground | GCLineWidth, &values);
        PSSetPixel(display, values.foreground);
        PSNewpath();
        PSMoveto(x, y);
        PSLineto(x + w, y);
        PSLineto(x + w, y + h);
        PSLineto(x, y + h);
        PSClosefill();
    }
}


void
XPSFillPolygon(Display *display, Drawable drawable, GC context,
               Coord *coord, int ncoords, int shape, int mode)
{
    int i;
    int pcount;

    if (output_flag == XOUT)
    {
        XFillPolygon(display, drawable, context,
                     (XPoint *)coord, ncoords, shape, mode);
    }
    else
    {
        XGCValues values;

        XGetGCValues(display, context, GCForeground | GCLineWidth, &values);

        if (ncoords <= 0)
        {
            return;
        }

        PSNewpath();
        PSSetPixel(display, values.foreground);
        PSMoveto(coord[0].x, coord[0].y);
        pcount = 0;

        for (i = 1; i < ncoords; i++)
        {
            PSLineto(coord[i].x, coord[i].y);

            /*
             * break it up into managable cuhnks
             */

            if (pcount > MAXSTROKES)
            {
                PSClosefill();
                PSMoveto(coord[i].x, coord[i].y);
                pcount = 0;
            }

            pcount++;
        }

        PSClosefill();
    }
}

/*
 * NOTE: XPSDrawArc and XPSFillArc don't handle elliptical arcs correctly
 * (e.g. w != h).
 */

void
XPSDrawArc(Display *display, Drawable drawable, GC context,
           int x, int y, int w, int h, int angle1, int angle2)
{
    if (output_flag == XOUT)
    {
        XDrawArc(display, drawable, context, x, y, w, h, angle1, angle2);
    }
    else
    {
        XGCValues values;

        XGetGCValues(display, context, GCForeground | GCLineWidth, &values);
        PSNewpath();
        PSSetPixel(display, values.foreground);
        PSSetLineWidth(values.line_width);
        PSArc(x + w / 2.0, y + h / 2.0, w / 2.0, angle1 / 64.0,
              angle2 / 64.0);
        PSStroke();
    }
}


void
XPSFillArc(Display *display, Drawable drawable, GC context,
           int x, int y, int w, int h, int angle1, int angle2)
{
    if (output_flag == XOUT)
    {
        XFillArc(display, drawable, context, x, y, w, h, angle1, angle2);
    }
    else
    {
        XGCValues values;

        XGetGCValues(display, context, GCForeground | GCLineWidth
                     | GCArcMode, &values);
        PSNewpath();
        PSSetPixel(display, values.foreground);
        PSSetLineWidth(values.line_width);

        if (values.arc_mode == ArcPieSlice)
            PSMoveto(x + w / 2, y + h / 2);

        PSArc(x + w / 2.0, y + h / 2.0, w / 2.0, angle1 / 64.0,
              angle2 / 64.0);
        PSClosefill();
    }
}


void
PSHeader(XWindowAttributes *info, float requested_scale, int box, int header)
{
    float   scale, scalex, scaley;
    float   tx, ty;
    int     bbxmin, bbxmax, bbymin, bbymax;
    long    clock;
    char    headerstr[200];
    char   *namestr;
    char   *ptr;
    char    clockstr[100];

    /*
     * --------------------------------------------
     *  calculate translation, scales, bounding box
     * --------------------------------------------
     */

    global_window_width = info->width;
    global_window_height = info->height;

    /*
     * calculate the scale factors in inches/screenpixel
     */

    scalex = PAGEWIDTH / global_window_width;
    scaley = PAGEHEIGHT / global_window_height;

    /*
     * use the smaller scale factor so everything will fit on the page
     */

    scale = requested_scale * ((scalex < scaley) ? scalex : scaley);

    /*
     * calculate the dots/screenpixel scale factor
     */

    pix_scale = scale * DPI;

    /*
     * convert to postscriptpoints/screenpixel
     */

    page_scale = scale * DEFAULTRES;

    /*
     * center it on the page for printer output
     * leave it at (0, 0) for file_output
     */

    if (file_output)
    {
        tx = 0.0;
        ty = 0.0;
    }
    else
    {
        tx = DEFAULTRES * (PAGEWIDTH - scale * global_window_width) / 2.0;
        ty = DEFAULTRES * (PAGEHEIGHT - scale * global_window_height) / 2.0;
    }

    /*
     * bounding box
     */

    bbxmin = tx;
    bbymin = ty;
    bbxmax = tx + page_scale * global_window_width + 1;
    bbymax = ty + page_scale * global_window_height + 1;

    /*
     * -----------
     *  COMMENTS
     * -----------
     */

    fprintf(PSfp, "%%!\n");
    fprintf(PSfp, "%%%%BoundingBox: %d %d %d %d\n",
            bbxmin, bbymin, bbxmax, bbymax);
    fprintf(PSfp, "%%%%EndComments\n");

    fprintf(PSfp, "initgraphics\n");
    fprintf(PSfp, "/M { moveto } def\n");
    fprintf(PSfp, "/L { lineto } def\n");
    fprintf(PSfp, "/A { arc } def\n");
    fprintf(PSfp, "/AN { arcn } def\n");
    fprintf(PSfp, "/S { stroke } def\n");
    fprintf(PSfp, "/N { newpath } def\n");
    fprintf(PSfp, "/C { closepath fill } def\n");
    fprintf(PSfp, "/G { setgray } def\n");

    if (box)
    {
        fprintf(PSfp, "0.5 G\n");
        PSNewpath();
        PSMoveto(bbxmin, global_window_height - bbymin);
        PSLineto(bbxmin, global_window_height - bbymax);
        PSLineto(bbxmax, global_window_height - bbymax);
        PSLineto(bbxmax, global_window_height - bbymin);
        PSLineto(bbxmin, global_window_height - bbymin);
        PSStroke();
        fprintf(PSfp, "0 G\n");
    }

    fprintf(PSfp, "%f %f translate\n", tx, ty);
    fprintf(PSfp, "%f %f scale\n", page_scale, page_scale);

    if (header)
    {
        if ((namestr = getenv("NAME")) == ((char *)NULL))
        {
            namestr = "";
        }

        time(&clock);
        strcpy(clockstr, ctime(&clock));

        if ((ptr = strchr(clockstr, '\n')))
        {
            *ptr = '\0';
        }

        sprintf(headerstr, "%s ::  %s ::  %s",
                clockstr, getcwd(NULL, 200), namestr);
        PSNewpath();
        PSFont(9);
        fprintf(PSfp, "15 15 M\n");
        PSShow(headerstr);
        PSStroke();
    }

    PSNewpath();
    PSMoveto(0, 0);
    PSLineto(0, global_window_height);
    PSLineto(global_window_width, global_window_height);
    PSLineto(global_window_width, 0);
    PSLineto(0, 0);
    fprintf(PSfp, "eoclip\n");
}


void
PSTrailer(void)
{
    fprintf(PSfp, "showpage ");
}


void
PreparePS(Display *display, Window window, float scale, int box, int header)
{
    char    command[80];
    XWindowAttributes info;
    char   *printer;
    char   *pcom;

    current_gray = -1;
    current_line = -1;
    XGetWindowAttributes(display, window, &info);

    if (file_output)
    {
        if ((PSfp = fopen(ps_filename, ps_filemode)) == NULL)
        {
            fprintf(stderr, "unable to open '%s'\n", ps_filename);
            return;
        }
    }
    else
    {
        if ((printer = (char *)getenv("PRINTER")) == NULL)
        {
            printer = "lw";
        }

        if ((pcom = (char *)getenv("PRINTCOMMAND")) == NULL)
        {
            sprintf(command, "lpr -h -P%s", printer);
            pcom = command;
        }

        if ((PSfp = popen(pcom, "w")) == NULL)
        {
            fprintf(stderr, "unable to execute '%s'\n", command);
            return;
        }
    }

    PSHeader(&info, scale, box, header);
    output_flag = PSOUT;
}


void
FinishPS(void)
{
    PSTrailer();
    output_flag = XOUT;

    if (file_output)
    {
        if (PSfp)
            fclose(PSfp);
    }
    else
    {
        if (PSfp)
            pclose(PSfp);
    }
}

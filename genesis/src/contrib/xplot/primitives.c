/*
 * FILE: primitives.c
 *
 */

#include "xplot_ext.h"

void
Beep(void)
{
    fprintf(stderr, "\007");
}


void
DrawSuperLine(BasicWindow *basic, int sx1, int sy1, int sx2, int sy2)
{
    SetColor(basic->foreground ^ basic->background);
    DrawMode(basic, GXxor);
    DrawLine(sx1, sy1, sx2, sy2);
    DrawMode(basic, GXcopy);
}


void
EraseSuperLine(BasicWindow *basic, int sx1, int sy1, int sx2, int sy2)
{
    SetColor(basic->foreground ^ basic->background);
    DrawMode(basic, GXxor);
    DrawLine(sx1, sy1, sx2, sy2);
    DrawMode(basic, GXcopy);
}


void
DrawSuperBox(BasicWindow *basic, int sx1, int sy1, int sx2, int sy2)
{
    SetColor(basic->foreground ^ basic->background);
    DrawMode(basic, GXxor);
    Box(sx1, sy1, sx2, sy2);
    DrawMode(basic, GXcopy);
}


void
EraseSuperBox(BasicWindow *basic, int sx1, int sy1, int sx2, int sy2)
{
    SetColor(basic->foreground ^ basic->background);
    DrawMode(basic, GXxor);
    Box(sx1, sy1, sx2, sy2);
    DrawMode(basic, GXcopy);
}


void
ZapExpose(Display *display)
{
    XEvent e;

    while (XCheckMaskEvent(display, ExposureMask, &e))
        ;
}


void
DrawMode(BasicWindow *basic, int mode)
{
    XSetFunction(basic->display, basic->context, mode);
}


char *
CopyString(char *s)
{
    char *tmp;

    if (s == NULL)
    {
        return NULL;
    }

    tmp = (char *)malloc(strlen(s) + 1);
    strcpy(tmp, s);
    return tmp;
}


void
SetLinestyle(BasicWindow *basic, int style)
{
    XGCValues values;

    if (style == 0)
    {
        values.line_style = LineSolid;
        XChangeGC(basic->display, basic->context, GCLineStyle, &values);
    }
    else
    {
        values.line_style = LineDoubleDash;
        XChangeGC(basic->display, basic->context, GCLineStyle, &values);
    }
}


void
_SetColor(BasicWindow *basic, int pixel)
{
    basic->color = pixel;
    XSetForeground(basic->display, basic->context, (unsigned long)pixel);
}


void
Invert(BasicWindow *basic)
{
    XSetForeground(basic->display, basic->context, basic->background);
    XSetBackground(basic->display, basic->context, basic->foreground);
}


void
Normal(BasicWindow *basic)
{
    XSetForeground(basic->display, basic->context, basic->foreground);
    XSetBackground(basic->display, basic->context, basic->background);
}


void
SetColor(int pixel)
{
    G->color = pixel;
    XSetForeground(G->display, G->context, (unsigned long)pixel);
}


void
SetForeground(BasicWindow *basic, int pixel)
{
    basic->foreground = pixel;
    XSetForeground(basic->display, basic->context, (unsigned long)pixel);
}


void
SetBackground(BasicWindow *basic, int pixel)
{
    basic->background = pixel;
    XSetBackground(basic->display, basic->context, (unsigned long)pixel);
}


void
SetPlaneMask(int mask)
{
    XSetPlaneMask(G->display, G->context, (unsigned long)mask);
}


void
SetFont(BasicWindow *frame, char *name)
{
    frame->font = XLoadFont(frame->display, name);

    if (frame->font == 0)
    {
        fprintf(stderr, "unable to open text font");
    }

    frame->fontinfo = XQueryFont(frame->display, frame->font);
    frame->fontheight = frame->fontinfo->ascent + frame->fontinfo->descent;
    frame->fontwidth = frame->fontinfo->max_bounds.rbearing -
        frame->fontinfo->min_bounds.lbearing;
    XSetFont(frame->display, frame->context, frame->font);
}


void
SetLineWidth(int width)
{
    XGCValues values;

    G->linewidth = width;
    values.line_width = width;
    XChangeGC(G->display, G->context, GCLineWidth, &values);
}


void
TextExtent(char *s, int *width, int *height)
{
    int ascent;
    int descent;
    int direction;

    XCharStruct overall;

    XTextExtents(G->fontinfo, s, strlen(s), &direction, &ascent, &descent,
                 &overall);
    *height = overall.ascent + overall.descent;
    *height = G->fontheight;
    *width = overall.width;
}


void
Text(BasicWindow *basic, int x, int y, char *s)
{
    XPSDrawText(basic->display, basic->drawable, basic->context, x, y, s);
}


void
_FilledBox(BasicWindow *basic, int xl, int yb, int xr, int yt)
{
    if (yb > yt)
    {
        XPSFillRectangle(basic->display, basic->drawable, basic->context, xl,
                         yt, xr - xl, yb - yt);
    }
    else
    {
        XPSFillRectangle(basic->display, basic->drawable, basic->context, xl,
                         yb, xr - xl, yt - yb);
    }
}


void
FilledBox(int xl, int yb, int xr, int yt)
{
    if (yb > yt)
    {
        XPSFillRectangle(G->display, G->drawable, G->context, xl, yt, xr - xl,
                         yb - yt);
    }
    else
    {
        XPSFillRectangle(G->display, G->drawable, G->context, xl, yb, xr - xl,
                         yt - yb);
    }
}


void
_Box(BasicWindow *basic, int xl, int yb, int xr, int yt)
{
    XPSDrawRectangle(basic->display, basic->drawable, basic->context, xl, yb,
                     xr - xl, yt - yb);
}


void
Box(int xl, int yb, int xr, int yt)
{
    XPSDrawRectangle(G->display, G->drawable, G->context, xl, yb, xr - xl,
                     yt - yb);
}


void
FilledPoly(Coord *coord, int ncoords)
{
    XPSFillPolygon(G->display, G->drawable, G->context, coord, ncoords,
                   Convex, CoordModeOrigin);
}


void
_DrawLine(BasicWindow *b, int x1, int y1, int x2, int y2)
{
    XPSDrawLine(b->display, b->drawable, b->context, x1, y1, x2, y2);
}


void
DrawLine(int x1, int y1, int x2, int y2)
{
    XPSDrawLine(G->display, G->drawable, G->context, x1, y1, x2, y2);
}


void
DrawDot(int x1, int y1)
{
    XPSDrawDot(G->display, G->drawable, G->context, x1, y1);
}


void
MultipleLines(Coord *coord, int ncoords)
{
    XPSDrawLines(G->display, G->drawable, G->context, coord, ncoords,
                 CoordModeOrigin);
}


void
FloatMultipleLines(Coord *coord, int ncoords, FCoord *fcoord)
{
    XPSDrawLinesFloat(G->display, G->drawable, G->context, coord, ncoords,
                      CoordModeOrigin, fcoord);
}


void
ClearWindow(BasicWindow *basic)
{
    XClearWindow(basic->display, basic->window);
}


void
Bell(int vol)
{
    XBell(G->display, 0);
}


void
DebugX(char *s)
{
    XEvent  E;

    fprintf(stderr, "%s\n", s);
    XNextEvent(G->display, &E);
    fprintf(stderr, "OK\n");
}


void
Circle(int cx, int cy, int d)
{
    XDrawArc(G->display, G->drawable, G->context, cx - d / 2, cy - d / 2, d,
             d, 0, 360 * 64);
}


void
FilledCircle(int cx, int cy, int d)
{
    XFillArc(G->display, G->drawable, G->context, cx - d / 2, cy - d / 2, d,
             d, 0, 360 * 64);
}


void
Triangle(int cx, int cy, int d)
{
    Coord coord[4];
    int   ncoords = 4;

    coord[0].x = cx - d / 2;
    coord[0].y = cy + d / 2;
    coord[1].x = cx;
    coord[1].y = cy - d / 2;
    coord[2].x = cx + d / 2;
    coord[2].y = cy + d / 2;
    coord[3].x = coord[0].x;
    coord[3].y = coord[0].y;

    XPSDrawLines(G->display, G->drawable, G->context, coord, ncoords,
                 CoordModeOrigin);
}


void
FilledTriangle(int cx, int cy, int d)
{
    Coord coord[4];
    int   ncoords = 4;

    coord[0].x = cx - d / 2;
    coord[0].y = cy + d / 2;
    coord[1].x = cx;
    coord[1].y = cy - d / 2;
    coord[2].x = cx + d / 2;
    coord[2].y = cy + d / 2;
    coord[3].x = coord[0].x;
    coord[3].y = coord[0].y;

    XPSFillPolygon(G->display, G->drawable, G->context, coord, ncoords,
                   Convex, CoordModeOrigin);
}

/*
 * FILE: scale.c
 *
 */

#include "xplot_ext.h"

/* Forward declarations: */

void FloatScreenTransform(Graph *graph,
                          float wx, float wy, float *sx, float *sy);
void PositionSubwindows(Frame *frame);
void RescalePlot(Plot *plot);

static int calcwxmin = 1;
static int calcwymin = 1;
static int calcwxmax = 1;
static int calcwymax = 1;


void
SuppressWxmin(void)
{
    calcwxmin = 0;
}


void
SuppressWymin(void)
{
    calcwymin = 0;
}


void
SuppressWxmax(void)
{
    calcwxmax = 0;
}


void
SuppressWymax(void)
{
    calcwymax = 0;
}


void
RescaleGraph(Graph *graph)
{
    Plot   *plot;
    double  val;

    if ((val = graph->wxmax - graph->wxmin) == 0)
    {
        val = 1;
    }

    graph->wxscale = graph->wwidth / val;

    if ((val = graph->wymax - graph->wymin) == 0)
    {
        val = 1;
    }

    graph->wyscale = graph->wheight / val;

    for (plot = graph->plot; plot; plot = plot->next)
    {
        RescalePlot(plot);
    }
}


int
RescaleFrame(Frame *frame)
{
    XWindowAttributes winfo;
    int w, h;

    /*
     * get the size of the frame
     */

    XGetWindowAttributes(frame->display, frame->window, &winfo);

    /*
     * check to see whether the window size has changed
     */

    w = winfo.width;
    h = winfo.height;

    /*
     * If it has, then update the window parameters.
     */

    if (w != frame->wwidth || h != frame->wheight)
    {
        frame->wheight = winfo.height;
        frame->wwidth = winfo.width;

        /*
         * Position the subwindows.
         */

        PositionSubwindows(frame);
        RescaleGraph(frame->graph);
        return 1;
    }

    return 0;
}


void
PositionSubwindows(Frame *frame)
{
    TextWindow *text;
    Graph      *graph;
    int         text_height;

    graph = frame->graph;
    text  = frame->text;

    if (text->mapped)
    {
        text_height = 2 * frame->fontheight;
    }
    else
    {
        text_height = 0;
    }

    if (graph->mapped)
    {
        /*
         * adjust the graph window accordingly
         */
        graph->wwidth = frame->wwidth;
        graph->wheight = frame->wheight - text_height;
        XMoveResizeWindow(graph->display, graph->window, -1, -1,
                          graph->wwidth, graph->wheight);
    }

    if (text->mapped)
    {
        /*
         * adjust the text window
         */

        text->wwidth = frame->wwidth;
        text->wheight = text_height;
        XMoveResizeWindow(text->display, text->window, -1, graph->wheight,
                          graph->wwidth, text_height);
    }
}


void
GetDataPoint(Plot *plot, FCoord *data, float *wx, float *wy)
{
    switch (plot->graph->xaxis_type)
    {
    case LINEAR_AXIS:
        *wx = data->x * plot->xscale + plot->xoffset + plot->yaxisoffset;
        break;

    case LOG10_AXIS:
        *wx = log10((double)data->x * plot->xscale + plot->xoffset)
            + plot->yaxisoffset;
        break;
    }

    switch (plot->graph->yaxis_type)
    {
    case LINEAR_AXIS:
        *wy = data->y * plot->scale + plot->yoffset + plot->xaxisoffset;
        break;

    case LOG10_AXIS:
        *wy = log10((double)(data->y * plot->scale) + plot->yoffset)
            + plot->xaxisoffset;
        break;
    }
}


void
RescalePlot(Plot *plot)
{
    int     i;
    float   sx, sy;
    float   wx, wy;
    FCoord *data;
    Coord  *coord;
    FCoord *fcoord;

    /*
     * transform each data coordinate into a screen coordinate
     */

    data   = plot->data;
    coord  = plot->coord;
    fcoord = plot->fcoord;

    if ((data == NULL)
        || (coord == NULL)
        || (fcoord == NULL))
    {
        return;
    }

    data   += plot->xlo;
    coord  += plot->xlo;
    fcoord += plot->xlo;

    for (i = plot->xlo; i <= plot->xhi; i++)
    {
        GetDataPoint(plot, data, &wx, &wy);

        if (i == plot->xlo)
        {
            plot->xmin = plot->xmax = wx;
            plot->ymin = plot->ymax = wy;
        }
        else
        {
            if (wx < plot->xmin)
                plot->xmin = wx;

            if (wx > plot->xmax)
                plot->xmax = wx;

            if (wy < plot->ymin)
                plot->ymin = wy;

            if (wy > plot->ymax)
                plot->ymax = wy;
        }

        FloatScreenTransform(plot->graph, wx, wy, &sx, &sy);
        coord->x  = sx;
        coord->y  = sy;
        fcoord->x = sx;
        fcoord->y = sy;
        data++;
        coord++;
        fcoord++;
    }
}


/*
 * Transform world coordinates (x, y) into screen coordinates (sx, sy).
 */

void
ScreenTransform(Graph *graph, float wx, float wy, int *sx, int *sy)
{
    *sx = (wx - graph->wxmin) * graph->wxscale;

    if (*sx > 10000)
        *sx = 10000;

    if (*sx < -10000)
        *sx = -10000;

    /*
     * Invert the screen coordinates so that the top of the window is up.
     */

    *sy = graph->wheight - (wy - graph->wymin) * graph->wyscale;

    if (*sy > 10000)
        *sy = 10000;

    if (*sy < -10000)
        *sy = -10000;
}


/*
 * Transform world coordinates (x, y) into screen coordinate (sx, sy).
 */

void
FloatScreenTransform(Graph *graph, float wx, float wy, float *sx, float *sy)
{
    *sx = (wx - graph->wxmin) * graph->wxscale;
    /*
     * try to keep X from losing its shorts over large integers
     */

    if (*sx > 10000)
        *sx = 10000;

    if (*sx < -10000)
        *sx = -10000;
    /*
     * invert the screen coordinates so that the top of the window is up
     */
    *sy = graph->wheight - (wy - graph->wymin) * graph->wyscale;

    if (*sy > 10000)
        *sy = 10000;

    if (*sy < -10000)
        *sy = -10000;
}


/*
 * Transform screen coordinates (sx, sy) into world coordinate (wx, wy).
 */

void
WorldTransform(Graph *graph, int sx, int sy, float *wx, float *wy)
{
    if ((graph->wxscale == 0) || (graph->wyscale == 0))
    {
        *wx = 0;
        *wy = 0;
    }

    /*
     * Do the inverse transform from screen coords to world coords.
     */

    *wx = sx / graph->wxscale + graph->wxmin;

    /*
     * Invert the y screen coord before transforming.
     */

    *wy = (graph->wheight - sy) / graph->wyscale + graph->wymin;
}


void
AutoScale(Graph *graph, int autoscale_x, int autoscale_y)
{
    int     cnt;
    float   val;
    Plot   *plot;

    if (autoscale_x)
    {
        cnt = 0;

        for (plot = graph->plot; plot; plot = plot->next)
        {
            if (plot->visible)
            {
                if (cnt++ < 1)
                {
                    graph->xmax = plot->xmax;
                    graph->xmin = plot->xmin;
                }
                else
                {
                    if (plot->xmin < graph->xmin)
                        graph->xmin = plot->xmin;

                    if (plot->xmax > graph->xmax)
                        graph->xmax = plot->xmax;
                }
            }
        }
    }

    if (autoscale_y)
    {
        cnt = 0;

        for (plot = graph->plot; plot; plot = plot->next)
        {
            if (plot->visible)
            {
                if (cnt++ < 1)
                {
                    graph->ymax = plot->ymax;
                    graph->ymin = plot->ymin;
                }
                else
                {
                    if (plot->ymin < graph->ymin)
                        graph->ymin = plot->ymin;

                    if (plot->ymax > graph->ymax)
                        graph->ymax = plot->ymax;
                }
            }
        }
    }

    /*
     * Determine the world coordinate bounds of the window.
     * Assume that the data area will take up a fraction of the window.
     */

    val = graph->xmax - graph->xmin;

    if (val == 0)
        val = 1;

    if (calcwxmin)
    {
        graph->wxmin = graph->xmin - 0.15 * val;
    }

    if (calcwxmax)
    {
        graph->wxmax = graph->xmax + 0.1 * val;
    }

    val = graph->ymax - graph->ymin;

    if (val == 0)
        val = 1;

    if (calcwymin)
    {
        graph->wymin = graph->ymin - 0.15 * val;
    }

    if (calcwymax)
    {
        graph->wymax = graph->ymax + 0.1 * val;
    }

    calcwxmin = 1;
    calcwymin = 1;
    calcwxmax = 1;
    calcwymax = 1;
}


void
AutoOrigin(Graph *graph)
{
    int   sw, sh;
    int   tw, th;
    float ww, wh;
    char  label[80];
    int   i;

    /*
     * Autolocate the origin within the current window.
     * Make enough room to fit the labels.
     */

    label[0] = '\0';

    for (i = 0; i < graph->yaxis_rightdp + graph->yaxis_leftdp + 2; i++)
    {
        strcat(label, "0");
    }

    TextExtent(label, &tw, &th);
    sh = graph->wheight - th * 3.5;
    sw = tw;

    WorldTransform(graph, sw, sh, &ww, &wh);
    graph->yaxis_xintcpt = ww;
    graph->xaxis_yintcpt = wh;
}

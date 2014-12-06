/*
 * FILE: axis.c
 *
 */

#include "xplot_ext.h"

/* Forward declarations: */

void
CalculateAxisScale(float wmin, float wmax, int left, int right,
                   int *exponent, int nticks);
void DrawXAxis(Graph *graph);
void DrawYAxis(Graph *graph);


void
DrawAxes(Graph *graph)
{
    /*
     * label the axes
     */

    if (graph->show_ylabels && (graph->xaxis_title != NULL))
    {
        DrawYAxisLabel(graph);
    }

    if (graph->show_xlabels && (graph->xaxis_title != NULL))
    {
        DrawXAxisLabel(graph);
    }

    if (graph->show_yaxis)
    {
        DrawYTicks(graph);
    }

    if (graph->show_xaxis)
    {
        DrawXTicks(graph);
    }

    /*
     * draw the axis labels
     */

    if (graph->show_yaxis)
    {
        DrawYAxis(graph);
    }

    if (graph->show_xaxis)
    {
        DrawXAxis(graph);
    }
}


void
CalculateAxisParameters(Graph *graph)
{
    if (graph->auto_axes)
    {
        AutoOrigin(graph);
    }

    CalculateYTicks(graph);
    CalculateXTicks(graph);

    /*
     * Calculate the axis scale factor.
     */

    CalculateAxisScale(graph->wxmin, graph->wxmax,
                       graph->xaxis_leftdp, graph->xaxis_rightdp,
                       &(graph->xaxis_exponent), graph->xaxis_nticks);

    CalculateAxisScale(graph->wymin, graph->wymax,
                       graph->yaxis_leftdp, graph->yaxis_rightdp,
                       &(graph->yaxis_exponent), graph->yaxis_nticks);
}


void
CalculateAxisScale(float wmin, float wmax, int left, int right,
                   int *exponent, int nticks)
{
    int scale_factor;

    /*
     * determine the scale factor which will allow
     * for unique values for each tick with the given precision
     */

    scale_factor = log10((double)((wmax - wmin) / nticks));

    /*
     * if log is negative then check against right hand size
     */

    if (scale_factor < 0)
    {
        /*
         * only scale if the exponent exceeds the available field size
         */

        /*
         * check for auto field formatting
         */

        if (right < 0)
        {
            if (-scale_factor > -right)
            {
                *exponent = scale_factor;
            }
            else
            {
                *exponent = 0;
            }
        }
        else
        {
            if (-scale_factor > right)
            {
                *exponent = scale_factor;
            }
            else
            {
                *exponent = 0;
            }
        }
    }
    else
    {
        /*
         * if log is positive then check against left hand size
         */

        /*
         * only scale if the exponent exceeds the available field size
         */

        if (scale_factor > left)
        {
            *exponent = scale_factor;
        }
        else
        {
            *exponent = 0;
        }
    }
}


void
DrawXAxis(Graph *graph)
{
    int   sy;
    int   tmp;
    int   sx1;
    int   sx2;
    int   axisx;
    Plot *plot;

    /*
     * get the screen coordinates of the axis
     * should probably make sure they are reasonable screen
     * coords since X has such problems if they are not
     */

    SetColor(graph->foreground);
    ScreenTransform(graph, graph->wxmax, graph->xaxis_yintcpt, &sx2, &sy);
    ScreenTransform(graph, graph->yaxis_xintcpt, 0.0, &axisx, &tmp);
    ScreenTransform(graph, graph->wxmin, 0.0, &sx1, &tmp);

    /*
     * draw the axis line
     */

    /*
     * check for quadrant I
     */

    if (graph->quadrants & 0x1)
    {
        DrawLine(axisx, sy, sx2, sy);
    }

    /*
     * check for quadrant III
     */

    if (graph->quadrants & 0x4)
    {
        DrawLine(sx1, sy, axisx, sy);
    }

    /*
     * check each plot to see whether it has its own axis
     */

    for (plot = graph->plot; plot; plot = plot->next)
    {
        if (plot->xaxis && plot->visible)
        {
            ScreenTransform(graph, graph->wxmax,
                            graph->xaxis_yintcpt + plot->xaxisoffset,
                            &tmp, &sy);

            if (graph->quadrants & 0x1)
            {
                DrawLine(axisx, sy, sx2, sy);
            }

            if (graph->quadrants & 0x4)
            {
                DrawLine(sx1, sy, axisx, sy);
            }
        }
    }
}


void
DrawYAxis(Graph *graph)
{
    int   sx;
    int   sy1;
    int   sy2;
    int   tmp;
    int   axisy;
    Plot *plot;

    /*
     * draw the axis through the desired intercept
     */

    SetColor(graph->foreground);
    ScreenTransform(graph, graph->yaxis_xintcpt, graph->wymin, &sx, &sy1);
    ScreenTransform(graph, 0.0, graph->xaxis_yintcpt, &tmp, &axisy);
    ScreenTransform(graph, 0.0, graph->wymax, &tmp, &sy2);

    /*
     * draw the axis line
     */

    /*
     * check for quadrant II
     */

    if (graph->quadrants & 0x2)
    {
        DrawLine(sx, axisy, sx, sy2);
    }

    /*
     * check for quadrant IV
     */

    if (graph->quadrants & 0x8)
    {
        DrawLine(sx, sy1, sx, axisy);
    }

    /*
     * check each plot to see whether it has its own axis
     */

    for (plot = graph->plot; plot; plot = plot->next)
    {
        if (plot->yaxis && plot->visible)
        {
            ScreenTransform(graph, graph->yaxis_xintcpt + plot->yaxisoffset,
                            0.0, &sx, &tmp);

            if (graph->quadrants & 0x2)
            {
                DrawLine(sx, axisy, sx, sy2);
            }

            if (graph->quadrants & 0x8)
            {
                DrawLine(sx, sy1, sx, axisy);
            }
        }
    }
}

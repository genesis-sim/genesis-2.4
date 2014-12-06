/*
 * FILE: tick.c
 *
 */

#include "xplot_ext.h"

void
CalculateTickCount(float interval, float origin, float wmin, float wmax,
                   float *start, int *nticks)
{
    /*
     * find the number of intervals which will fit in wmax-wmin
     */

    *nticks = (wmax - wmin) / (interval) + 1.5;

    /*
     * the starting tick is the first multiple of the interval
     * from the origin that appears after wmin
     */

    if (wmin - origin > 0)
    {
        *start = origin
            + (int)(1 + (wmin - origin) / (interval)) * (interval);
    }
    else
    {
        *start = origin
            + (int)((wmin - origin) / (interval)) * (interval);
    }
}


float
CalculateTickInterval(int desired_nticks,
                      float origin, float wmin, float wmax)
{
    float base_interval;
    float interval_hint;
    int   total_ticks;
    int   tickscale;
    float logscale;
    float interval;

    /*
     * find an even spacing of ticks over the interval wmin to wmax
     */

    interval_hint = (wmax - wmin) / desired_nticks;

    /*
     * find the base interval
     */

    logscale = log10((double) interval_hint);

    /*
     * always go to the finer resolution
     */

    if (logscale < 0)
    {
        base_interval = pow((double) 10.0, (double) ((int) (logscale - .5)));
    }
    else
    {
        base_interval = pow((double) 10.0, (double) ((int) logscale));
    }

    if (base_interval == 0)
    {
        base_interval = 1;
    }

    /*
     * calculate the integer number of base intervals that will fit in the
     * total_interval
     */

    total_ticks = (wmax - wmin) / base_interval;

    if (total_ticks > desired_nticks)
    {
        /*
         * find the multiple of the total_ticks which is closest to the
         * desired ticks
         */

        tickscale = (float) total_ticks / desired_nticks + .5;
        interval = base_interval * tickscale;
    }
    else
    {
        /*
         * find the multiple of the total_ticks which is closest to the
         * desired ticks
         */

        tickscale = (float) desired_nticks / total_ticks + .5;
        interval = base_interval / tickscale;
    }

    return interval;
}


void
DrawXTickLabel(Graph *graph, int sx, int sy, float val, float scale,
               int left, int right)
{
    char format[80];
    char label[80];
    int  tw, th;

    if (right < 0)
    {
        sprintf(format, "%%%dg", -right);
    }
    else
    {
        sprintf(format, "%%%d.%df", left, right);
    }

    /*
     * Change tick values that are almost zero to zero.
     * Otherwise adjust them to the scale value.
     */

    if (graph->truncate_tick_values)
    {
        if (fabs(val / graph->yaxis_tickinc) < 0.001)
            val = 0.0;
        else
            val *= scale;
    }

    sprintf(label, format, val);
    TextExtent(label, &tw, &th);
    Text((BasicWindow *)graph,
         sx - tw / 2,
         (int)(sy + 2 * graph->ticksize + th + graph->xticklabel_offset),
         label);
}


void
DrawYTickLabel(Graph *graph, int sx, int sy, float val, float scale,
               int left, int right)
{
    char format[80];
    char label[80];
    int  tw, th;

    if (right < 0)
    {
        sprintf(format, "%%%dg", -right);
    }
    else
    {
        sprintf(format, "%%%d.%df", left, right);
    }

    /*
     * Change tick values that are almost zero to zero.
     * Otherwise adjust them to the scale value.
     */

    if (fabs(val / graph->yaxis_tickinc) < 0.001)
        val = 0.0;
    else
        val *= scale;

    sprintf(label, format, val);
    TextExtent(label, &tw, &th);
    Text((BasicWindow *)graph,
         sx - tw - 2 * graph->ticksize + graph->yticklabel_offset,
         sy + th / 2, label);
}


void
CalculateYTicks(Graph *graph)
{
    int desired_nticks;

    /*
     * If the desired tick count is less than 0 then
     * use the tick count as a screen coordinate spacing
     * between ticks.
     */

    if (graph->yaxis_desired_nticks < 0)
    {
        desired_nticks = -((float) (graph->wheight)
                           / graph->yaxis_desired_nticks) + 0.5;
    }
    else
    {
        desired_nticks = graph->yaxis_desired_nticks;
    }

    /*
     * Get reasonable tick intervals for the given number of ticks.
     */

    if (graph->yaxis_desired_tickinc > 0)
    {
        graph->yaxis_tickinc = graph->yaxis_desired_tickinc;
    }
    else
    {
        /*
         * Calculate tick spacing.
         */
        graph->yaxis_tickinc =
            CalculateTickInterval(desired_nticks, graph->xaxis_yintcpt,
                                  graph->wymin, graph->wymax);
    }

    CalculateTickCount(graph->yaxis_tickinc, graph->xaxis_yintcpt,
                       graph->wymin, graph->wymax,
                       &(graph->yaxis_tickstart), &(graph->yaxis_nticks));
}


void
CalculateXTicks(Graph *graph)
{
    int desired_nticks;

    /*
     * If the desired tick count is less than 0 then
     * use the tick count as a screen coordinate spacing
     * between ticks.
     */

    if (graph->xaxis_desired_nticks < 0)
    {
        desired_nticks = -((float) (graph->wwidth)
                           / graph->xaxis_desired_nticks) + 0.5;
    }
    else
    {
        desired_nticks = graph->xaxis_desired_nticks;
    }

    if (graph->xaxis_desired_tickinc > 0)
    {
        graph->xaxis_tickinc = graph->xaxis_desired_tickinc;
    }
    else
    {
        /*
         * calculate tick spacing
         */
        graph->xaxis_tickinc =
            CalculateTickInterval(desired_nticks, graph->yaxis_xintcpt,
                                  graph->wxmin, graph->wxmax);
    }

    CalculateTickCount(graph->xaxis_tickinc, graph->yaxis_xintcpt,
                       graph->wxmin, graph->wxmax,
                       &(graph->xaxis_tickstart), &(graph->xaxis_nticks));
}


void
DrawYTicks(Graph *graph)
{
    int   sx1, sy1;
    int   sx2, sy2;
    int   i;
    float xint;
    float val;
    float scale;
    int   tmp;
    int   axisy;
    int   j;

    if (graph->yaxis_desired_nticks == 0)
    {
        graph->yaxis_nticks = 1;
        return;
    }

    /*
     * Get the screen coordinates of the axis.
     */

    SetColor(graph->foreground);
    xint = graph->yaxis_xintcpt;
    ScreenTransform(graph, xint, graph->wymin, &sx1, &sy1);
    ScreenTransform(graph, xint, graph->wymax, &sx2, &sy2);

    /*
     * Get the axis exponent.
     */

    scale = 1.0 / pow((double) 10.0, (double) graph->yaxis_exponent);
    ScreenTransform(graph, 0.0, graph->xaxis_yintcpt, &tmp, &axisy);

    /*
     * Loop over all the tick marks.
     */

    for (i = 0; i < graph->yaxis_nticks; i++)
    {
        for (j = 0; j < graph->yaxis_nsubticks + 1; j++)
        {
            /*
             * Get the actual value of the tick.
             */

            val = i * graph->yaxis_tickinc +
                graph->yaxis_tickstart +
                j * graph->yaxis_tickinc / (graph->yaxis_nsubticks + 1);

            /*
             * Locate its screen position in the window.
             */

            ScreenTransform(graph, graph->yaxis_xintcpt, val, &sx1, &sy1);

            /*
             * Either draw a grid or just tick marks at the tick intervals.
             */

            if (graph->show_ygrid)
            {
                SetColor(graph->gridcolor);
                SetLinestyle((BasicWindow *)graph, 1);
                DrawLine(0, sy1, graph->wwidth, sy1);
                SetLinestyle((BasicWindow *)graph, 0);
                SetColor(graph->foreground);
            }

            if (!(graph->quadrants & 0x2) && !(graph->quadrants & 0x8))
                continue;

            /*
             * Check for quadrants II.
             */

            if (!(graph->quadrants & 0x2) && sy1 < axisy)
                continue;

            /*
             * Check for quadrants IV.
             */

            if (!(graph->quadrants & 0x8) && sy1 > axisy)
                continue;

            if (j == 0)
            {
                DrawLine(sx1 - graph->ticksize, sy1,
                         sx2 + graph->ticksize, sy1);

                /*
                 * Label the ticks.
                 */

                if (graph->show_ylabels)
                {
                    DrawYTickLabel(graph, sx1, sy1,
                                   val, scale,
                                   graph->yaxis_leftdp, graph->yaxis_rightdp);
                }
            }
            else
            {
                DrawLine(sx1 - (int) (0.5 * graph->ticksize), sy1,
                         sx2 + (int) (0.5 * graph->ticksize), sy1);
            }
        }
    }
}


void
DrawXTicks(Graph *graph)
{
    int   sx1, sy1;
    int   sx2, sy2;
    int   i;
    float yint;
    float val;
    float scale;
    int   tmp;
    int   axisx;
    int   j;


    if (graph->xaxis_desired_nticks == 0)
    {
        graph->xaxis_nticks = 1;
        return;
    }

    /*
     * Get the screen coordinates of the axis.
     */

    SetColor(graph->foreground);
    yint = graph->xaxis_yintcpt;
    ScreenTransform(graph, graph->wxmin, yint, &sx1, &sy1);
    ScreenTransform(graph, graph->wxmax, yint, &sx2, &sy2);

    scale = 1.0 / pow((double) 10.0, (double) graph->xaxis_exponent);
    ScreenTransform(graph, graph->yaxis_xintcpt, 0.0, &axisx, &tmp);

    for (i = 0; i < graph->xaxis_nticks; i++)
    {
        for (j = 0; j < graph->xaxis_nsubticks + 1; j++)
        {
            val = i * graph->xaxis_tickinc + graph->xaxis_tickstart +
                j * graph->xaxis_tickinc / (graph->xaxis_nsubticks + 1);
            ScreenTransform(graph, val, graph->xaxis_yintcpt, &sx1, &sy1);

            /*
             * Either draw a grid or just tick marks at the tick intervals.
             */

            if (graph->show_xgrid)
            {
                SetColor(graph->gridcolor);
                SetLinestyle((BasicWindow *)graph, 1);
                DrawLine(sx1, 0, sx1, graph->wheight);
                SetLinestyle((BasicWindow *)graph, 0);
                SetColor(graph->foreground);
            }

            if (!(graph->quadrants & 0x1) && !(graph->quadrants & 0x4))
                continue;

            /*
             * Check for quadrants I.
             */

            if (!(graph->quadrants & 0x1) && sx1 > axisx)
                continue;

            /*
             * Check for quadrants III.
             */

            if (!(graph->quadrants & 0x4) && sx1 < axisx)
                continue;

            if (j == 0)
            {
                DrawLine(sx1, sy1 - graph->ticksize, sx1,
                         sy2 + graph->ticksize);

                /*
                 * label the ticks
                 */

                if (graph->show_xlabels)
                {
                    DrawXTickLabel(graph, sx1, sy1,
                                   val, scale,
                                   graph->xaxis_leftdp, graph->xaxis_rightdp);
                }
            }
            else
            {
                DrawLine(sx1, sy1 - (int) (0.5 * graph->ticksize), sx1,
                         sy2 + (int) (0.5 * graph->ticksize));
            }
        }
    }
}

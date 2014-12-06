/*
 * FILE: label.c
 *
 */

#include "xplot_ext.h"

int verbose_legend = 1;

void
TextWindowLabel(char *label)
{
    SetColor(F->text->foreground);
    FreeLabels((BasicWindow *)F->text);
    AddLabelString((BasicWindow *)F->text, label,
                   0, F->text->fontheight, 0.0, 0.0, SCREEN_LBL,
                   TEMPORARY_LBL);
    Text((BasicWindow *)F->text, 0, F->text->fontheight, label);
}


void
DrawGraphTitle(Graph *graph)
{
    int center;
    int tw, th;

    if (graph->title)
    {
        SetColor(graph->foreground);
        TextExtent(graph->title, &tw, &th);
        center = (graph->wwidth - tw) / 2;
        Text((BasicWindow *)graph, center, th * 2, graph->title);
    }
}


void
DrawPlotLegend(Plot *plot, int count)
{
    int  sx, sy;
    char legend[200];
    char tmp[80];
    int  tw, th;


    if (plot->visible)
    {
        SetColor(plot->linestyle);
    }
    else
    {
        SetColor(plot->graph->background);
    }

    legend[0] = '\0';

    if (plot->title)
    {
        strcpy(legend, plot->title);
    }

    if (verbose_legend)
    {
        if (plot->scale != 1)
        {
            sprintf(tmp, "[ys=%g]", plot->scale);
            strcat(legend, tmp);
        }

        if (plot->xscale != 1)
        {
            sprintf(tmp, "[xs=%g]", plot->xscale);
            strcat(legend, tmp);
        }

        if (plot->escale != 1)
        {
            sprintf(tmp, "[es=%g]", plot->escale);
            strcat(legend, tmp);
        }

        if (plot->xoffset != 0)
        {
            sprintf(tmp, "[x=%g]", plot->xoffset);
            strcat(legend, tmp);
        }

        if (plot->yoffset != 0)
        {
            sprintf(tmp, "[y=%g]", plot->yoffset);
            strcat(legend, tmp);
        }

        if (plot->xaxisoffset != 0)
        {
            sprintf(tmp, "[xa=%g]", plot->xaxisoffset);
            strcat(legend, tmp);
        }

        if (plot->yaxisoffset != 0)
        {
            sprintf(tmp, "[ya=%g]", plot->yaxisoffset);
            strcat(legend, tmp);
        }
    }

    TextExtent(legend, &tw, &th);
    sx = plot->graph->wwidth - tw - 25;
    sy = th * (count + 3);

    /*
     * Show the linestyle of the plot.
     */

    switch (plot->plot_type)
    {
    case NORMAL_GRAPH:
        DrawLine(plot->graph->wwidth - 20, sy, plot->graph->wwidth - 5, sy);

        if (plot->showpoints)
        {
            DrawPointIcon(plot, plot->graph->wwidth - 13, sy);
        }

        break;

    case BAR_GRAPH:
        DrawLine(plot->graph->wwidth - 20, sy, plot->graph->wwidth - 5, sy);
        break;

    case SCATTER_GRAPH:
        DrawPointIcon(plot, plot->graph->wwidth - 13, sy);
        break;
    }

    SetColor(plot->graph->foreground);

    /*
     * Show the name of the plot.
     */

    Text((BasicWindow *)plot->graph, sx, sy, legend);

    /*
     * Indicate whether it is selected.
     */

    if (plot->selected)
    {
        Box(sx - 8, sy - th / 2 - 2, sx - 4, sy - th / 2 + 2);
    }
    else
    {
        /*
         * Erase any previous selection.
         */

        SetColor(plot->graph->background);
        Box(sx - 8, sy - th / 2 - 2, sx - 4, sy - th / 2 + 2);
    }
}


void
DrawXAxisLabel(Graph *graph)
{
    char label[80];
    char tmp[80];
    int  tw, th;
    int  sx, sy;

    SetColor(graph->foreground);
    ScreenTransform(graph, graph->wxmax, graph->xaxis_yintcpt, &sx, &sy);

    /*
     * Calculate the axis scale factor.
     CalculateAxisScale(graph->wxmin, graph->wxmax,
     graph->xaxis_leftdp, graph->xaxis_rightdp, &(graph->xaxis_exponent),
     graph->xaxis_nticks);
    */

    /*
     * Print the axis label.
     */

    TextExtent(graph->xaxis_title, &tw, &th);

    /*
     * Center the axis title.
     */

    Text((BasicWindow *)graph, (graph->wwidth - tw) / 2,
         sy + th * 3 + graph->xaxistitle_offset, graph->xaxis_title);
    label[0] = '\0';

    /*
     * Identify log axis.
     */

    if (graph->xaxis_type == LOG10_AXIS)
    {
        strcpy(label, "LOG ");
    }

    /*
     * Label the scale factor if any.
     */

    if (graph->xaxis_exponent != 0)
    {
        sprintf(tmp, "x10^%d", graph->xaxis_exponent);
        strcat(label, tmp);
    }

    if (strlen(label) > 0)
    {
        TextExtent(label, &tw, &th);
        Text((BasicWindow *)graph, sx - tw - 10, sy + th * 3, label);
    }
}


void
DrawYAxisLabel(Graph *graph)
{
    char label[80];
    char tmp[80];
    int  tw, th;
    int  sx, sy;

    SetColor(graph->foreground);
    ScreenTransform(graph, graph->yaxis_xintcpt, graph->wymin, &sx, &sy);

    /*
     * calculate the axis scale factor
     CalculateAxisScale(graph->wymin, graph->wymax,
     graph->yaxis_leftdp, graph->yaxis_rightdp, &(graph->yaxis_exponent),
     graph->yaxis_nticks);
    */

    /*
     * Print the axis label.
     */

    TextExtent(graph->yaxis_title, &tw, &th);
    Text((BasicWindow *)graph,
         sx + 5 + graph->yaxistitle_offset, th, graph->yaxis_title);
    label[0] = '\0';

    /*
     * Identify log axis.
     */

    if (graph->yaxis_type == LOG10_AXIS)
    {
        strcpy(label, "LOG ");
    }

    /*
     * Label the scale factor if any.
     */

    if (graph->yaxis_exponent != 0)
    {
        sprintf(tmp, "x10^%d", graph->yaxis_exponent);
        strcat(label, tmp);
    }

    if (strlen(label) > 0)
    {
        TextExtent(label, &tw, &th);
        Text((BasicWindow *)graph, sx + 5, th * 2, label);
    }
}

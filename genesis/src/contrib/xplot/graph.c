/*
 * FILE: graph.c
 *
 */

#include "xplot_ext.h"

void
ScaleAndRefreshGraph(Graph *graph)
{
    RescaleGraph(graph);
    RefreshGraph(graph);
}

void
RefreshGraph(Graph *graph)
{
    ResetGlobalOperation();
    ClearWindow((BasicWindow *)graph);
    DisplayLabels((BasicWindow *)graph);
    DrawGraph(graph);
}

void
DrawGraph(Graph *graph)
{
    Plot *plot;
    int   count;

    CalculateAxisParameters(graph);
    DrawAxes(graph);

    /*
     * draw each plot of the graph
     */

    count = 0;

    for (plot = graph->plot; plot; plot = plot->next)
    {
        if (plot->graph->show_legend)
        {
            DrawPlotLegend(plot, count);
        }

        if (plot->visible)
        {
            DrawPlot(plot, count);
        }

        count++;
    }

    if (graph->show_title)
    {
        DrawGraphTitle(graph);
    }
}


void
DrawPointIcon(Plot *plot, int x, int y)
{
    switch (plot->point_symbol)
    {
    case BOX_PT:
        /*
         * place a 5x5 pixel box around each point
         */

        Box(x - 2, y - 2, x + 2, y + 2);
        break;

    case FBOX_PT:
        /*
         * place a solid 5x5 pixel box around each point
         */

        FilledBox(x - 3, y - 3, x + 2, y + 3);
        break;

    case CIRCLE_PT:
        /*
         * place a 5 pixel circle around each point
         */

        Circle(x, y, 5);
        break;

    case FCIRCLE_PT:
        /*
         * place a solid 5 pixel circle around each point
         */

        FilledCircle(x, y, 7);
        break;

    case TRIANGLE_PT:
        /*
         * place a 5 pixel triangle around each point
         */

        Triangle(x, y, 5);
        break;

    case FTRIANGLE_PT:
        /*
         * place a solid 5 pixel triangle around each point
         */

        FilledTriangle(x, y, 7);
        break;

    case X_PT:
        /*
         * place a 5 pixel X at each point
         */

        DrawLine(x - 2, y - 2, x + 2, y + 2);
        DrawLine(x - 2, y + 2, x + 2, y - 2);
        break;

    case CROSS_PT:
        /*
         * place a 5 pixel X at each point
         */

        DrawLine(x - 2, y, x + 2, y);
        DrawLine(x, y + 2, x, y - 2);
        break;

    case DOT:
        /*
         * place a single pixel dot at each point
         */

        DrawDot(x, y);
        break;
    }
}


void
DrawPoints(Plot *plot)
{
    int i;

    /*
     * draw an icon for each point
     */

    for (i = plot->xlo; i <= plot->xhi; i++)
    {
        /*
         * optimize for points out of view
         */

        if ((plot->coord[i].x < -10)
            || (plot->coord[i].x > plot->graph->wwidth + 10))
        {
            continue;
        }

        if (i % plot->pointfreq == 0)
        {
            DrawPointIcon(plot, plot->coord[i].x, plot->coord[i].y);
        }
    }
}


void
DrawSteps(Plot *plot)
{
    int i;

    /*
     * draw an step between each point
     */

    for (i = plot->xlo; i < plot->xhi; i++)
    {
        /*
         * optimize for points out of view
         */

        if ((plot->coord[i].x < -10)
            || (plot->coord[i].x > plot->graph->wwidth + 10))
        {
            continue;
        }

        DrawLine(plot->coord[i].x, plot->coord[i].y,
                 plot->coord[i].x, plot->coord[i + 1].y);

        DrawLine(plot->coord[i].x, plot->coord[i + 1].y,
                 plot->coord[i + 1].x, plot->coord[i + 1].y);
    }
}


void
DrawBars(Plot *plot)
{
    int i;
    int width;
    int sx1, sx2;
    int sy, sy2;

    /*
     * calculate the screen coordinates of the x axis
     * and the bar width
     */

    if (plot->xaxis)
    {
        ScreenTransform(plot->graph, plot->bar_width,
                        plot->graph->xaxis_yintcpt + plot->xaxisoffset, &sx1,
                        &sy);
    }
    else
    {
        ScreenTransform(plot->graph, plot->bar_width,
                        plot->graph->xaxis_yintcpt, &sx1, &sy);
    }

    ScreenTransform(plot->graph, 0.0, 0.0, &sx2, &sy2);
    width = abs(sx1 - sx2);

    /*
     * draw a box from the data point to the  x axis
     */

    for (i = plot->xlo; i <= plot->xhi; i++)
    {
        /*
         * optimize for points out of view
         */

        if ((plot->coord[i].x < -10)
            || (plot->coord[i].x > plot->graph->wwidth + 10))
        {
            continue;
        }

        if (plot->bar_filled)
        {
            FilledBox((int)(plot->coord[i].x - width / 2.0 + .5),
                      plot->coord[i].y,
                      (int)(plot->coord[i].x + width / 2.0 + .5), sy);
        }
        else
        {
            Box((int)(plot->coord[i].x - width / 2.0 + .5), plot->coord[i].y,
                (int)(plot->coord[i].x + width / 2.0 + .5), sy);
        }
    }
}


void
DrawErrorBars(Plot *plot)
{
    int i;
    int sx;
    int tmp;
    int sy1, sy2;

    if (plot->errorbar == NULL)
    {
        return;
    }

    for (i = plot->xlo; i <= plot->xhi; i++)
    {
        if (i % plot->pointfreq == 0)
        {
            sx = plot->coord[i].x;

            /*
             * get the screen coords of the error bar endpoints
             */

            ScreenTransform(plot->graph,
                            0.0,
                            plot->data[i].y -
                            plot->escale * plot->errorbar[i].value +
                            plot->yoffset + plot->xaxisoffset, &tmp, &sy1);

            ScreenTransform(plot->graph, 0.0,
                            plot->data[i].y +
                            plot->escale * plot->errorbar[i].value +
                            plot->yoffset + plot->xaxisoffset, &tmp, &sy2);

            DrawLine(sx, sy1, sx, sy2);          /* error bar */
            DrawLine(sx - 2, sy1, sx + 2, sy1);  /* ticks at the endpoints */
            DrawLine(sx - 2, sy2, sx + 2, sy2);
        }
    }
}


void
DrawPlot(Plot *plot, int count)
{

    SetColor(plot->linestyle);
    /* SetLinestyle(plot->graph, plot->linestyle); */

    switch (plot->plot_type)
    {
    case NORMAL_GRAPH:
        if (plot->graph->resolution == 1)
        {
            FloatMultipleLines(plot->coord + plot->xlo,
                               plot->xhi - plot->xlo + 1,
                               plot->fcoord + plot->xlo);
        }
        else
        {
            MultipleLines(plot->coord + plot->xlo,
                          plot->xhi - plot->xlo + 1);
        }

        if (plot->showpoints)
        {
            DrawPoints(plot);
        }

        if (plot->show_error)
        {
            DrawErrorBars(plot);
        }

        break;

    case BAR_GRAPH:
        DrawBars(plot);

        if (plot->show_error)
        {
            DrawErrorBars(plot);
        }

        break;

    case SCATTER_GRAPH:
        DrawPoints(plot);

        if (plot->show_error)
        {
            DrawErrorBars(plot);
        }

        break;

    case STEP_GRAPH:
        DrawSteps(plot);

        if (plot->show_error)
        {
            DrawErrorBars(plot);
        }

        break;
    }

    /* SetLinestyle(plot->graph, SOLID_LINE); */
}

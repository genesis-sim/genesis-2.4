/*
 * FILE: buttons.c
 *
 */

#include "xplot_ext.h"

int snapmode = 1;
static int buttonmode = DATAMODE;

int
ButtonMode(void)
{
    return buttonmode;
}

void
SetButtonMode(int mode)
{
    buttonmode = mode;
}

void
ToggleSnapMode(void)
{
    snapmode = !snapmode;
}

void
SnapMode(int mode)
{
    snapmode = mode;
}


/*
 * takes the screen coordinates given in sx, sy and locates the
 * data point with the closest x screen coordinate
 * then returns those data values in wx, wy
 */

void
Snap(Graph *graph, int sx, int sy, float *wx, float *wy)
{
    float   x, y;
    float   wx2, wy2;
    int     i;
    int     npoints;
    Plot   *p;
    Plot   *plot;
    FCoord *data;

    /*
     * get the world coordinates of the cursor
     */

    WorldTransform(graph, sx, sy, &x, &y);

    /*
     * get the currently selected plot
     * use the first plot as the default
     */

    plot = graph->plot;

    for (p = graph->plot; p; p = p->next)
    {
        if (p->selected)
        {
            plot = p;
        }
    }

    /*
     * locate the data point with the closest x coord
     * from the currently selected plot
     */

    data = plot->data;
    npoints = plot->npoints;

    /*
     * check the endpoints
     */

    if (npoints > 0)
    {
        /*
         * get the first data point
         */

        GetDataPoint(plot, data, wx, wy);

        /*
         * if there is only one point then just return
         * return with wx, wy set to the data point
         */

        if (npoints < 2)
            return;

        /*
         * otherwise get the last data point
         */

        GetDataPoint(plot, data + npoints - 1, &wx2, &wy2);

        /*
         * is it beyond the most positive data point?
         */

        if (x >= *wx && x >= wx2)
        {
            /*
             * return with wx, wy set to the most positive data point
             */

            if (wx2 > *wx)
            {
                *wx = wx2;
                *wy = wy2;
            }

            return;
        }

        /*
         * is it beyond the most negative data point?
         */

        if (x <= *wx && x <= wx2)
        {
            /*
             * return with wx, wy set to the most positive data point
             */

            if (wx2 < *wx)
            {
                *wx = wx2;
                *wy = wy2;
            }

            return;
        }

        GetDataPoint(plot, data, wx, wy);

        for (i = 1; i < plot->npoints; i++)
        {
            GetDataPoint(plot, data + i, &wx2, &wy2);

            /*
             * is x between this data point and the last?
             */

            if (wx2 >= *wx)
            {
                if (x < wx2 && x >= *wx)
                {
                    return;
                }
            }
            else
            {
                if (x < *wx && x >= wx2)
                {
                    *wx = wx2;
                    *wy = wy2;
                    return;
                }
            }

            *wx = wx2;
            *wy = wy2;
        }
    }
}


void
NBShowCoords(BasicWindow *basic, int sx, int sy, float x, float y)
{
    char    label[80];

    sprintf(label, "x=%-8.4g y=%-8.4g", x, y);
    SetColor(basic->foreground);
    Text(basic, sx + 3, sy, label);
    FreeLabels(basic);
    AddLabelString(basic, label, sx + 3, sy, 0.0, 0.0,
                   SCREEN_LBL, TEMPORARY_LBL);
}


void
ShowCoords(BasicWindow *basic, int sx, int sy, float x, float y)
{
    char    label[80];

    Box(sx - 2, sy - 2, sx + 2, sy + 2);
    AddLabelBox(basic, 0, 0, 0, x, y, 2.0, WORLD_LBL, TEMPORARY_LBL);
    sprintf(label, "x=%-8.4g y=%-8.4g", x, y);
    SetColor(basic->foreground);
    Text(basic, sx + 3, sy, label);
    AddLabelString(basic, label, sx + 3, sy, x, y, WORLD_LBL, TEMPORARY_LBL);
}


void
ShowSlope(BasicWindow *basic, int sx, int sy,
          float x1, float y1, float x2, float y2)
{
    char label1[200];
    char label[200];

    SetColor(basic->foreground);
    sprintf(label1, "dx= %-7.4g dy= %-7.4g l= %-7.4g                ",
            x2 - x1,
            y2 - y1,
            sqrt(pow((double)(x2 - x1), (double)2.0)
                 + pow((double)(y2 - y1), (double)2.0)));

    if (x2 - x1 == 0)
    {
        sprintf(label, "%s s= Infinity    ", label1);
    }
    else
    {
        sprintf(label, "%s s= %-7.4g         ", label1,
                (y2 - y1) / (x2 - x1));
    }

    Text(basic, sx + 3, sy, label);
    FreeLabels(basic);
    AddLabelString(basic, label, sx + 3, sy, 0.0, 0.0,
                   SCREEN_LBL, TEMPORARY_LBL);
}



void
ShowSlopeCoords(BasicWindow *basic, int sx, int sy,
                float x1, float y1, float x2, float y2)
{
    char    label[200];
    char    label1[200];

    SetColor(basic->foreground);
    sprintf(label1, "x= %-8.4g y= %-8.4g dx= %-7.4g dy= %-7.4g l= %-7.4g",
            x2,
            y2,
            x2 - x1,
            y2 - y1,
            sqrt(pow((double)(x2 - x1), (double)2.0)
                 + pow((double)(y2 - y1), (double)2.0)));

    if (x2 - x1 == 0)
    {
        sprintf(label, "%s s= Infinity    ", label1);
    }
    else
    {
        sprintf(label, "%s s= %-7.4g         ", label1,
                (y2 - y1) / (x2 - x1));
    }

    Text(basic, sx + 3, sy, label);
    FreeLabels(basic);
    AddLabelString(basic, label, sx + 3, sy, 0.0, 0.0,
                   SCREEN_LBL, TEMPORARY_LBL);
}


void
ShowCursorCoords(BasicWindow *basic, int sx, int sy, int x, int y)
{
    char    label[200];

    SetColor(basic->foreground);
    sprintf(label, "%5d %5d         ", x, y);
    Text(basic, sx + 3, sy, label);
    FreeLabels(basic);
    AddLabelString(basic, label, sx + 3, sy, 0.0, 0.0,
                   SCREEN_LBL, TEMPORARY_LBL);
}



void
ButtonPressAction(Graph *graph, XButtonPressedEvent *buttonevent)
{
    float   x, y;
    int     sx1, sy1;
    int     sx2, sy2;
    int     button;

    sx1 = buttonevent->x;
    sy1 = buttonevent->y;
    button = buttonevent->button;

    switch (ButtonMode())
    {
    case DATAMODE:
        switch (button)
        {
        case 1:
            /*
             * mark the start of the drag location
             */
            graph->dragx1 = sx1;
            graph->dragy1 = sy1;
            break;

        case 2:
            ClearWindow((BasicWindow *)graph->frame->text);

            if (snapmode)
            {
                /*
                 * world coord xy readout of data
                 */

                Snap(graph, sx1, sy1, &x, &y);

                /*
                 * get the screen coords of the data point
                 */

                ScreenTransform(graph, x, y, &sx2, &sy2);
                ShowCoords((BasicWindow *)graph, sx2, sy2, x, y);

                /*
                 * mark the start of the drag location
                 */

                graph->dragx1 = sx2;
                graph->dragy1 = sy2;
            }
            else
            {
                /*
                 * world coord xy readout of cursor location
                 */

                WorldTransform(graph, sx1, sy1, &x, &y);
                ShowCoords((BasicWindow *)graph, sx1, sy1, x, y);

                /*
                 * mark the start of the drag location
                 */

                graph->dragx1 = sx1;
                graph->dragy1 = sy1;
            }

            graph->dragwx1 = x;
            graph->dragwy1 = y;
            break;

        case 3:
            /*
             * world coord xy readout closest above point
             */

            Snap(graph, sx1, sy1, &x, &y);

            /*
             * draw a line to the point on the plot (sx2, sy2)
             */

            ScreenTransform(graph, x, y, &sx2, &sy2);

            /* avoid writing on the last pixel */

            if (sy2 > sy1)
            {
                sy2 -= 1;
            }
            else
            {
                sy2 += 1;
            }

            graph->dragx1 = sx1;
            graph->dragy1 = sy1;
            graph->dragx2 = sx2;
            graph->dragy2 = sy2;
            DrawSuperLine((BasicWindow *)graph, sx2, sy1, sx2, sy2);
            ClearWindow((BasicWindow *)graph->frame->text);
            NBShowCoords((BasicWindow *)graph->frame->text,
                         0, graph->fontheight, x, y);
        }

        break;

    case DRAWMODE:
        switch (button)
        {
        case 1:
            /*
             * mark the start of the line
             */

            graph->dragx1 = sx1;
            graph->dragy1 = sy1;
            WorldTransform(graph, sx1, sy1, &x, &y);
            graph->dragwx1 = x;
            graph->dragwy1 = y;
            break;
        }

        break;

    case ZAPMODE:
        switch (button)
        {
        case 1:
            /*
             * delete the label
             */

            ZapLabel(graph, (XKeyEvent *)buttonevent);
            break;
        }

        break;

    case PINCHMODE:
        switch (button)
        {
        case 1:
            /*
             * offset the plots
             */

            OffsetPlotGraphically(graph, buttonevent->x, buttonevent->y, 0);
            break;
        }

        break;
    }
}


void
ButtonReleaseAction(Graph *graph, XButtonReleasedEvent *buttonevent)
{
    int     button;
    int     sx2, sy2;
    int     sx1, sy1;
    float   x1, y1, x2, y2;

    button = buttonevent->button;
    sx2 = buttonevent->x;
    sy2 = buttonevent->y;
    switch (ButtonMode())
    {
    case DATAMODE:
        switch (button)
        {
        case 1:     /* zoom box */
            /*
             * mark the end of the drag and zoom
             */

            graph->dragx2 = sx2;
            graph->dragy2 = sy2;
            WorldTransform(graph, graph->dragx1, graph->dragy1, &x1, &y1);
            WorldTransform(graph, graph->dragx2, graph->dragy2, &x2, &y2);

            /*
             * Don't perform a zero scale action.
             */

            if (x2 == x1 || y2 == y1)
                return;

            if (x2 > x1)
            {
                graph->wxmin = x1;
                graph->wxmax = x2;
            }
            else
            {
                graph->wxmin = x2;
                graph->wxmax = x1;
            }

            if (y2 > y1)
            {
                graph->wymin = y1;
                graph->wymax = y2;
            }
            else
            {
                graph->wymin = y2;
                graph->wymax = y1;
            }

            ScaleAndRefreshGraph(graph);
            /*
             * set the drag coords back to the invalid state
             */
            graph->dragx1 = -1;
            graph->dragx2 = -1;
            break;

        case 2:     /* ruler line */
            /*
             * get the world coordinates of the starting point
             */

            x1 = graph->dragwx1;
            y1 = graph->dragwy1;

            if (sx2 != graph->dragx1 || sy2 != graph->dragy1)
            {
                if (snapmode)
                {
                    /*
                     * display the final coords
                     */

                    Snap(graph, sx2, sy2, &x2, &y2);

                    /*
                     * compute the screen coordinate of the data point
                     */

                    ScreenTransform(graph, x2, y2, &sx1, &sy1);
                    ShowCoords((BasicWindow *)graph, sx1, sy1, x2, y2);
                }
                else
                {
                    /*
                     * display the final coords
                     */

                    WorldTransform(graph, sx2, sy2, &x2, &y2);
                    ShowCoords((BasicWindow *)graph, sx2, sy2, x2, y2);
                }

                ClearWindow((BasicWindow *)graph->frame->text);
                ShowSlope((BasicWindow *)graph->frame->text,
                          0, graph->fontheight,
                          x1, y1, x2, y2);
                AddLabelLine((BasicWindow *)graph, 0, 0, 0, 0,
                             x1, y1, x2, y2, WORLD_LBL, TEMPORARY_LBL);
            }

            /*
             * set the drag coords back to the invalid state
             */

            graph->dragx1 = -1;
            graph->dragx2 = -1;
            break;

        case 3:     /* drag line */
            /*
             * erase the old line
             */

            EraseSuperLine((BasicWindow *)graph, graph->dragx2,
                           graph->dragy1, graph->dragx2, graph->dragy2);

            /*
             * get the coords
             */

            Snap(graph, sx2, sy2, &x2, &y2);
            ShowCoords((BasicWindow *)graph,
                       graph->dragx2, graph->dragy2, x2, y2);
            graph->dragx1 = -1;
            graph->dragx2 = -1;
            break;
        }

        break;

    case DRAWMODE:
        switch (button)
        {
        case 1:     /* draw line */
            /*
             * mark the end of the drag and zoom
             */

            graph->dragx2 = sx2;
            graph->dragy2 = sy2;
            WorldTransform(graph, graph->dragx1, graph->dragy1, &x1, &y1);
            WorldTransform(graph, graph->dragx2, graph->dragy2, &x2, &y2);

            /*
             * draw a line between the points
             */

            AddLabelLine((BasicWindow *)graph,
                         0, 0, 0, 0,
                         x1, y1, x2, y2, WORLD_LBL, TEMPORARY_LBL);

            /*
             * set the drag coords back to the invalid state
             */

            graph->dragx1 = -1;
            graph->dragx2 = -1;
            break;
        }

        break;
    }
}


void
PointerMotionAction(Graph *graph, XPointerMovedEvent *motionevent)
{
    int     sx2, sy2;
    int     sx1, sy1;
    int     button;
    float   x1, y1;
    float   x2, y2;
    float   x, y;


    /*
     * get the location of the cursor
     */

    sx2 = motionevent->x;
    sy2 = motionevent->y;

    /*
     * get the button pressed
     */

    button = (motionevent->state & 0xFF00) >> 8;

    switch (ButtonMode())
    {
    case PINCHMODE:
        /*
         * pinch at the cursor location
         */

        WorldTransform(graph, sx2, sy2, &x2, &y2);
        NBShowCoords((BasicWindow *)graph->frame->text,
                     0, graph->fontheight, x2, y2);
        break;

    case TEXTMODE:
        /*
         * if its in text mode then read out the cursor location
         */

        ShowCursorCoords((BasicWindow *)graph->frame->text,
                         0, graph->fontheight, sx2, sy2);
        break;

    case DRAWMODE:
        /*
         * If it's in screen coord mode then read out the cursor location.
         */

        switch (button)
        {
        case 1:     /* draw line */
            if (graph->dragx1 != -1)
            {
                if (graph->dragx2 != -1)
                {
                    /*
                     * erase the old drag line from the starting drag
                     * coordinates drag->x1, y1 to the previous end
                     * coordinates drag->x2, y2.
                     */

                    EraseSuperLine((BasicWindow *)graph,
                                   graph->dragx1, graph->dragy1,
                                   graph->dragx2, graph->dragy2);
                }

                /*
                 * get the world coordinates of the initial drag point
                 */

                x1 = graph->dragwx1;
                y1 = graph->dragwy1;

                /*
                 * get the world coordinates of the current cursor
                 * location
                 */

                WorldTransform(graph, sx2, sy2, &x2, &y2);

                /*
                 * update the drag coordinates to the current location
                 */

                graph->dragx2 = sx2;
                graph->dragy2 = sy2;

                /*
                 * display data about the line between the points in the
                 * text window
                 */

                ShowSlopeCoords((BasicWindow *)graph->frame->text,
                                0, graph->fontheight, x1,
                                y1, x2, y2);

                /*
                 * draw the new drag line
                 */

                DrawSuperLine((BasicWindow *)graph,
                              graph->dragx1, graph->dragy1, graph->dragx2,
                              graph->dragy2);
            }

            break;
        }

        break;

    case DATAMODE:
        switch (button)
        {
        case 1:     /* zoom box */
            if (graph->dragx1 != -1)
            {
                if (graph->dragx2 != -1)
                {
                    /*
                     * erase the old drag rectangle
                     */

                    EraseSuperBox((BasicWindow *)graph,
                                  graph->dragx1, graph->dragy1,
                                  graph->dragx2, graph->dragy2);
                }

                graph->dragx2 = sx2;
                graph->dragy2 = sy2;

                /*
                 * draw the new drag rectangle
                 */

                DrawSuperBox((BasicWindow *)graph,
                             graph->dragx1, graph->dragy1,
                             graph->dragx2, graph->dragy2);
            }

            break;

        case 2:     /* ruler line */
            if (graph->dragx1 != -1)
            {
                if (graph->dragx2 != -1)
                {
                    /*
                     * erase the old drag line from the starting drag
                     * coordinates drag->x1, y1 to the previous end
                     * coordinates drag->x2, y2.
                     */

                    EraseSuperLine((BasicWindow *)graph,
                                   graph->dragx1, graph->dragy1,
                                   graph->dragx2, graph->dragy2);
                }

                /*
                 * get the world coordinates of the initial drag point
                 */

                x1 = graph->dragwx1;
                y1 = graph->dragwy1;

                if (snapmode)
                {
                    /*
                     * get the data coordinates closest to the current cursor
                     * location
                     */

                    Snap(graph, sx2, sy2, &x2, &y2);

                    /*
                     * compute the screen coordinate of the data point
                     */

                    ScreenTransform(graph, x2, y2, &sx1, &sy1);

                    /*
                     * update the drag coordinates to the current location
                     */

                    graph->dragx2 = sx1;
                    graph->dragy2 = sy1;
                }
                else
                {
                    /*
                     * get the world coordinates of the current cursor
                     * location
                     */

                    WorldTransform(graph, sx2, sy2, &x2, &y2);

                    /*
                     * update the drag coordinates to the current location
                     */

                    graph->dragx2 = sx2;
                    graph->dragy2 = sy2;
                }

                /*
                 * display data about the line between the points in the
                 * text window
                 */

                ShowSlopeCoords((BasicWindow *)graph->frame->text,
                                0, graph->fontheight, x1,
                                y1, x2, y2);
                /*
                 * draw the new drag line
                 */

                DrawSuperLine((BasicWindow *)graph,
                              graph->dragx1, graph->dragy1, graph->dragx2,
                              graph->dragy2);
            }

            break;

        case 4:
            /*
             * get the data point whose screen x coordinate is closest to the
             * current cursor location
             */

            Snap(graph, sx2, sy2, &x, &y);

            /*
             * compute the screen coordinate of the data point
             */

            ScreenTransform(graph, x, y, &sx1, &sy1);

            /*
             * erase the previous line
             */

            EraseSuperLine((BasicWindow *)graph,
                           graph->dragx2, graph->dragy2, graph->dragx2,
                           graph->dragy1);

            /*
             * and draw a new line using the x coordinate of the data point
             * and the y coordinates of the cursor and the data point
             */

            graph->dragx1 = sx2;
            graph->dragy1 = sy2;
            graph->dragx2 = sx1;
            graph->dragy2 = sy1;
            DrawSuperLine((BasicWindow *)graph, sx1, sy1, sx1, sy2);

            /*
             * display the data coordinates in the text window
             */

            NBShowCoords((BasicWindow *)graph->frame->text,
                         0, graph->fontheight, x, y);
            break;
        }

        break;
    }
}

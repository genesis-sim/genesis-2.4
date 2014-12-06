/*
 * FILE: keys.c
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include "xplot_ext.h"

char    numstr[200];
char    labelstr[200];
char    tmpstr[200];
int     plot_select             = 0;
int     command_entry           = 0;
int     first_char              = 1;
int     labelsx, labelsy, labelwx, labelwy;
short   labelcoord_mode         = WINDOW_LBL;
static  float plot_scale        = 0.75;
static  int box                 = FALSE;
static  int header              = FALSE;
int     line_width              = 1;
static  int escape_mode         = 0;
static  int axis_mode           = XMODE;
static  int global_operation    = 0;
static  float shiftpercent      = 0.05;
int     jump;  /* sets a number of plots to skip to select the next */


void
SetHeader(int mode)
{
    header = mode;
}


void
AutoBarWidth(Plot *plot, float percent)
{
    /*
     * set bar width to a fraction of the distance between the first
     * two points
     */

    if (plot->npoints > 2)
    {
        plot->bar_width = percent * fabs(plot->data[0].x - plot->data[1].x);
    }
    else
    {
        plot->bar_width = 0;
    }
}


void
ZapLabel(Graph *graph, XKeyEvent *event)
{
    Label  *label;

    label = FindLabel((BasicWindow *)graph, event->x, event->y);
    DeleteLabel((BasicWindow *)graph, label);
    RefreshGraph(graph);
}


void
RightShift(void)
{
    float val;

    /*
     * right shift plot by a percentage of the window bounds
     */

    val = (G->wxmax - G->wxmin) * shiftpercent;
    G->wxmax += val;
    G->wxmin += val;
    ScaleAndRefreshGraph(G);
}


void
LeftShift(void)
{
    float val;

    /*
     * left shift plot by a percentage of the window bounds
     */

    val = (G->wxmax - G->wxmin) * shiftpercent;
    G->wxmax -= val;
    G->wxmin -= val;
    ScaleAndRefreshGraph(G);
}


void
SetAxisMode(int mode)
{
    axis_mode = mode;
}


void
TogglePinchmode(void)
{
    if (ButtonMode() == PINCHMODE)
    {
        NormalSelectInputs(G);
        SetButtonMode(DATAMODE);
        NormalCursor(F);
    }
    else
    {
        ClickSelectInputs(G);
        SetButtonMode(PINCHMODE);
        HandCursor(F);
    }
}


void
ToggleDrawmode(void)
{
    if (ButtonMode() == DRAWMODE)
    {
        SetButtonMode(DATAMODE);
        NormalCursor(F);
    }
    else
    {
        SetButtonMode(DRAWMODE);
        CrosshairCursor(F);
    }
}


void
ToggleZapmode(void)
{
    if (ButtonMode() == ZAPMODE)
    {
        SetButtonMode(DATAMODE);
        NormalCursor(F);
    }
    else
    {
        SetButtonMode(ZAPMODE);
        TargetCursor(F);
    }
}


void
ShiftPlotList(Graph *graph)
{
    Plot   *nextplot;
    Plot   *headplot;
    Plot   *plot;

    headplot = NULL;

    for (plot = graph->plot; plot; plot = plot->next)
    {
        /*
         * swap
         */

        if (plot->selected)
        {
            nextplot = plot->next;

            if (nextplot == NULL)
            {
                /*
                 * end of the list
                 */
                if (headplot == NULL)
                    return; /* only entry in the list */
                nextplot = graph->plot;
                graph->plot = plot;
                plot->next = nextplot;
                headplot->next = NULL;
            }
            else
            {
                if (headplot == NULL)
                {
                    /*
                     * beginning of list
                     */
                    graph->plot = nextplot;
                }
                else
                {
                    headplot->next = nextplot;
                }

                plot->next = nextplot->next;
                nextplot->next = plot;
            }

            return;
        }

        headplot = plot;
    }
}


void
SetShiftPercent(float val)
{
    shiftpercent = val;
}


void
PrintToFile(Graph *graph, char *fname, char *mode)
{
    /*
     * create a postscript file
     */

    SetPSFileOutput(1);
    SetPSFilename(fname);
    SetPSFilemode(mode);
    PreparePS(graph->display, graph->window, plot_scale, box, header);
    RefreshGraph(graph);
    FinishPS();
    SetPSFileOutput(0);
}

void
PrintOut(Graph *graph)
{
    PreparePS(graph->display, graph->window, plot_scale, box, header);
    RefreshGraph(graph);
    FinishPS();
}

void
SetBox(int mode)
{
    box = mode;
}


void
SelectPlot(Graph *graph, int ival)
{
    int   cnt;
    Plot *plot;
    int   numplots;

    numplots = 0;

    /*
     * clear previous selections
     */

    for (plot = graph->plot; plot; plot = plot->next)
    {
        plot->selected = 0;
        numplots++;
    }

    /*
     * mark the new selection and wrap around if necessary
     */

    if (ival < 0)
        ival = numplots - 1;

    if (ival >= numplots)
        ival = 0;

    cnt = 0;

    for (plot = graph->plot; plot; plot = plot->next)
    {
        if (cnt == ival)
        {
            plot->selected = 1;
            break;
        }

        cnt++;
    }

    /*
     * default to the first plot if an invalid selection
     * was made. This also gives wraparound if autoselection
     * is used.
     */

    if (plot == NULL)
    {
        graph->plot->selected = 1;
    }

    /*
     * update the plot legends
     */

    cnt = 0;

    for (plot = graph->plot; plot; plot = plot->next)
    {
        DrawPlotLegend(plot, cnt);
        cnt++;
    }
}


void
OffsetPlotGraphically(Graph *graph, int cx, int cy, float val)
{
    Plot  *plot;
    int    closest;
    float  distance;
    int    i;
    float  d;

    /*
     * find the points closest to the current cursor
     */

    for (plot = graph->plot; plot; plot = plot->next)
    {
        distance = 0;
        closest = 0;

        for (i = 0; i < plot->npoints; i++)
        {
            d = abs(cx - plot->coord[i].x);

            if (i == 0 || d < distance)
            {
                distance = d;
                closest = i;
            }
        }

        /*
         * offset the plot by the data value
         */

        plot->xaxisoffset = val - plot->data[closest].y;
    }

    ScaleAndRefreshGraph(graph);
}


void
SelectPlotGraphically(Graph *graph, int cx, int cy)
{
    int    cnt;
    Plot  *plot;
    Plot  *closest;
    float  distance = 0.0;
    int    i;
    float  d;

    /*
     * clear previous selections
     */

    for (plot = graph->plot; plot; plot = plot->next)
    {
        plot->selected = 0;
    }

    /*
     * find the point closest to the current cursor
     */

    closest = NULL;

    for (plot = graph->plot; plot; plot = plot->next)
    {
        for (i = 0; i < plot->npoints; i++)
        {
            d = SQR(cx - plot->coord[i].x) + SQR(cy - plot->coord[i].y);

            if (closest == NULL || d < distance)
            {
                distance = d;
                closest = plot;
            }
        }
    }

    /*
     * mark the new selection
     */

    for (plot = graph->plot; plot; plot = plot->next)
    {
        if (plot == closest)
        {
            plot->selected = 1;
            break;
        }
    }

    /*
     * default to the first plot if an invalid selection
     * was made. This also gives wraparound if autoselection
     * is used.
     */

    if (plot == NULL)
    {
        fprintf(stderr, "");
    }

    /*
     * update the plot legends
     */

    cnt = 0;

    for (plot = graph->plot; plot; plot = plot->next)
    {
        DrawPlotLegend(plot, cnt);
        cnt++;
    }
}


Plot *
SelectedPlot(Graph *graph)
{
    Plot *plot;

    for (plot = graph->plot; plot; plot = plot->next)
    {
        if (plot->selected)
        {
            return plot;
        }
    }

    return NULL;
}


int
GlobalOperation(void)
{
    return global_operation;
}


void
ResetGlobalOperation(void)
{
    global_operation = 0;
}


void
SetGlobalOperation(void)
{
    global_operation = 1;
}


void
ToggleVisibility(Graph *graph, Plot *plot)
{
    if ((plot = SelectedPlot(graph)))
    {
        plot->visible = !plot->visible;
    }

    numstr[0] = '\0';
    RefreshGraph(graph);
}


void
GotoPlot(Graph *graph, Plot *plot, int *ival, int num)
{
    int cnt;

    /*
     * select the plot
     */

    if (strlen(numstr) > 0)
    {
        /*
         * get the number of the plot
         */

        *ival = atoi(numstr);
    }
    else
    {
        /*
         * select the next plot
         */

        cnt = 0;

        for (plot = graph->plot; plot; plot = plot->next)
        {
            if (plot->selected)
            {
                *ival = cnt + num;
                break;
            }

            cnt++;
        }
    }

    SelectPlot(graph, *ival);
    numstr[0] = '\0';
}


void
KeyAction(Graph *graph, XKeyEvent *event)
{
    char    buffer[100];
    char    c;
    KeySym  key;
    float   val;
    int     ival;
    int     cnt;
    Plot   *plot;
    float   wx, wy;
    int     i;

    /* MCV: the following are for my additions/hacks. */
    int     plottarget1, plottarget2, plotcount, plotselected = 0;

    buffer[0] = '\0';

    /*
     * do key mapping to determine the actual key pressed
     */

    XLookupString(event, buffer, 100, &key, NULL);

    if ((c = *buffer) == '\0')
    {
        /*
         * check the key code for special keys
         */

        if (debug)
        {
            printf("%s\n", XKeysymToString(key));
        }

        return;
    }

    if (command_entry)
    {
        if (c == '\015')    /* was control-M */
        {
            ClearWindow((BasicWindow *)graph->frame->text);

            if (global_operation)
            {
                for (plot = graph->plot; plot; plot = plot->next)
                {
                    InterpretCommand(graph, &plot, numstr);
                }
            }
            else
                plot = SelectedPlot(graph);

            if (InterpretCommand(graph, &plot, numstr) == 0)
            {
                RefreshGraph(graph);
            }

            command_entry = 0;
            numstr[0] = '\0';
        }
        else if (c == '\010')   /* backspace */
        {
            if (strlen(numstr) > 0)
            {
                numstr[strlen(numstr) - 1] = '\0';
                SetColor(graph->frame->text->foreground);
                sprintf(tmpstr, "%s\037 ", numstr);
                Text((BasicWindow *)graph->frame->text, 0,
                     graph->frame->text->fontheight, tmpstr);
            }
        }
        else
        {
            SetColor(graph->frame->text->foreground);
            /* \037 is control-? */
            sprintf(tmpstr, "%s%c\037 ", numstr, c);
            Text((BasicWindow *)graph->frame->text,
                 0, graph->frame->text->fontheight,
                 tmpstr);
            sprintf(numstr, "%s%c", numstr, c);
        }

        return;
    }

    /*
     * see if in raw text entry mode
     */

    if (ButtonMode() == TEXTMODE)
    {
        if (c == '\015')    /* control-M */
        {
            NormalCursor(F);

            if (labelcoord_mode == WORLD_LBL)
            {
                WorldTransform(graph, labelsx, labelsy, &wx, &wy);
            }

            AddLabelString((BasicWindow *)graph,
                           labelstr, labelsx, labelsy,
                           wx, wy, labelcoord_mode, PERMANENT_LBL);
            sprintf(tmpstr, "%s ", labelstr);
            SetColor(graph->foreground);
            Text((BasicWindow *)graph, labelsx, labelsy, tmpstr);
            SetButtonMode(DATAMODE);
            first_char = 1;
            labelstr[0] = '\0';
            /* RefreshGraph(graph); */
            NormalSelectInputs(graph);
        }
        else if (c == '\016')   /* control-N */
        {
            if (labelcoord_mode == WORLD_LBL)
            {
                WorldTransform(graph, labelsx, labelsy, &wx, &wy);
            }

            AddLabelString((BasicWindow *)graph, labelstr, labelsx, labelsy,
                           wx, wy, labelcoord_mode, PERMANENT_LBL);
            SetColor(graph->foreground);
            sprintf(tmpstr, "%s ", labelstr);
            Text((BasicWindow *)graph, labelsx, labelsy, tmpstr);

            labelsy += graph->fontheight;
            labelstr[0] = '\0';
            SetColor(graph->foreground);
            sprintf(tmpstr, "%s\037", labelstr);
            Text((BasicWindow *)graph, labelsx, labelsy, tmpstr);
        }
        else if (c == '\037')   /* control-? */
        {
            if (strlen(labelstr) > 0)
            {
                labelstr[strlen(labelstr) - 1] = '\0';
                SetColor(graph->foreground);
                sprintf(tmpstr, "%s\037", labelstr);
                Text((BasicWindow *)graph, labelsx, labelsy, tmpstr);
            }
        }
        else
        {
            if (first_char)
            {
                labelsx = event->x;
                labelsy = event->y;
                first_char = 0;
                labelstr[0] = '\0';
            }

            sprintf(labelstr, "%s%c", labelstr, c);
            SetColor(graph->foreground);
            sprintf(tmpstr, "%s\037", labelstr);
            Text((BasicWindow *)graph, labelsx, labelsy, tmpstr);
        }

        return;
    }

    if (c == '\037')        /* control-? */
    {
        if (strlen(numstr) > 0)
        {
            numstr[strlen(numstr) - 1] = '\0';
            SetColor(graph->frame->text->foreground);
            sprintf(tmpstr, "%s\037 ", numstr);
            Text((BasicWindow *)graph->frame->text,
                 0, graph->frame->text->fontheight,
                 tmpstr);
        }
    }
    else
    {
        SetColor(graph->frame->text->foreground);
        sprintf(tmpstr, "%s%c\037 ", numstr, c);
        Text((BasicWindow *)graph->frame->text,
             0, graph->frame->text->fontheight, tmpstr);
    }

    /*
     * check for numeric strings
     */

    if ((c >= '0' && c <= '9')
        || (c == '.')
        || (c == '-')
        || (c == 'e'))
    {
        sprintf(numstr, "%s%c", numstr, c);
    }
    else
    {
        if (escape_mode)
        {
            switch (c)
            {
            case '\014':    /* control-L */
                FreeAllLabels((BasicWindow *)graph);
                RefreshGraph(graph);
                break;

            case '\t':
                /*
                 * toggle snapmode in the ruler line
                 */
                ToggleSnapMode();
                break;

            case '=':
                /*
                 * select the offset of the currently selected plot
                 */

                if (strlen(numstr) > 0)
                {
                    if (global_operation)
                    {
                        for (plot = graph->plot; plot; plot = plot->next)
                        {
                            if (axis_mode == XMODE)
                                plot->xaxisoffset = atof(numstr);
                            else if (axis_mode == YMODE)
                                plot->yaxisoffset = atof(numstr);
                        }
                    }
                    else if ((plot = SelectedPlot(graph)))
                    {
                        if (axis_mode == XMODE)
                        {
                            plot->xaxisoffset = atof(numstr);
                        }
                        else if (axis_mode == YMODE)
                        {
                            plot->yaxisoffset = atof(numstr);
                        }
                    }
                }

                numstr[0] = '\0';
                ScaleAndRefreshGraph(graph);
                break;

            case 'A':
                if (strlen(numstr) > 0)
                {
                    if (axis_mode == XMODE)
                    {
                        graph->xaxistitle_offset = atoi(numstr);
                    }
                    else if (axis_mode == YMODE)
                    {
                        graph->yaxistitle_offset = atoi(numstr);
                    }
                }

                numstr[0] = '\0';
                break;

            case 'B':
                /*
                 * toggle bar_graph fill mode
                 */

                if (global_operation)
                {
                    for (plot = graph->plot; plot; plot = plot->next)
                    {
                        plot->plot_type = BAR_GRAPH;
                        plot->bar_filled = !plot->bar_filled;
                    }
                }
                else if ((plot = SelectedPlot(graph)))
                {
                    plot->plot_type = BAR_GRAPH;
                    plot->bar_filled = !plot->bar_filled;
                }

                RefreshGraph(graph);
                break;

            case 'D':
                ToggleDrawmode();
                break;

            case 'L':
                /*
                 * set the line_width
                 */

                if (strlen(numstr) > 0)
                    line_width = atoi(numstr);
                else
                    line_width = 1;

                SetLineWidth(line_width);
                RefreshGraph(graph);
                numstr[0] = '\0';
                break;

            case 'R':
                /*
                 * fast raster specification
                 */

                if (strlen(numstr) > 0)
                    val = atof(numstr);
                else
                    val = 0;
                numstr[0] = '\0';
                ival = 0;

                for (plot = graph->plot; plot; plot = plot->next)
                {
                    if (axis_mode == XMODE)
                    {
                        plot->xaxisoffset = val * ival;
                    }
                    else if (axis_mode == YMODE)
                    {
                        plot->yaxisoffset = val * ival;
                    }

                    ival++;
                }

                numstr[0] = '\0';
                ScaleAndRefreshGraph(graph);
                break;

            case 'T':
                if (strlen(numstr) > 0)
                {
                    if (axis_mode == XMODE)
                    {
                        graph->xticklabel_offset = atoi(numstr);
                    }
                    else if (axis_mode == YMODE)
                    {
                        graph->yticklabel_offset = atoi(numstr);
                    }
                }

                numstr[0] = '\0';
                break;

            case 'a':
                /*
                 * toggle axis display
                 */
                graph->show_xaxis = !graph->show_xaxis;
                graph->show_yaxis = !graph->show_yaxis;
                RefreshGraph(graph);
                break;

            case 'b':
                if (strlen(numstr) > 0)
                {
                    /*
                     * set bar_graph display mode and bar width
                     */

                    if (global_operation)
                    {
                        for (plot = graph->plot; plot; plot = plot->next)
                        {
                            plot->plot_type = BAR_GRAPH;
                            plot->bar_width = atof(numstr);
                        }
                    }
                    else if ((plot = SelectedPlot(graph)))
                    {
                        plot->plot_type = BAR_GRAPH;
                        plot->bar_width = atof(numstr);
                    }
                }
                else
                {
                    /*
                     * autosize the bar to the maximum
                     */

                    if (global_operation)
                    {
                        for (plot = graph->plot; plot; plot = plot->next)
                        {
                            plot->plot_type = BAR_GRAPH;
                            AutoBarWidth(plot, 0.9);
                        }
                    }
                    else if ((plot = SelectedPlot(graph)))
                    {
                        plot->plot_type = BAR_GRAPH;
                        AutoBarWidth(plot, 0.9);
                    }
                }

                numstr[0] = '\0';
                RefreshGraph(graph);
                break;

            case 'c':
                /*
                 * select the linestyle/color of the currently selected plot
                 */

                if (strlen(numstr) > 0)
                {
                    if ((plot = SelectedPlot(graph)))
                    {
                        plot->linestyle = atoi(numstr);
                    }
                }

                numstr[0] = '\0';
                RefreshGraph(graph);
                break;

            case 'd':
                /*
                 * toggle error bars
                 */

                if (global_operation)
                {
                    for (plot = graph->plot; plot; plot = plot->next)
                    {
                        plot->show_error = !plot->show_error;
                    }
                }
                else if ((plot = SelectedPlot(graph)))
                {
                    plot->show_error = !plot->show_error;
                }

                RefreshGraph(graph);
                break;

            case 'f':
                /*
                 * set the tick label precision
                 */

                if (strlen(numstr) > 0)
                {
                    if (axis_mode == XMODE)
                    {
                        graph->xaxis_rightdp = atoi(numstr);
                    }
                    else if (axis_mode == YMODE)
                    {
                        graph->yaxis_rightdp = atoi(numstr);
                    }
                }

                numstr[0] = '\0';
                RefreshGraph(graph);
                break;

            case 'i':
                /*
                 * toggle inverse video
                 */
                graph->inverse = !graph->inverse;
                SetInverse((BasicWindow *)graph);
                RefreshGraph(graph);
                break;

            case 'l':
                /*
                 * set the axis style to log/linear
                 */

                if (strlen(numstr) > 0)
                {
                    if (axis_mode == XMODE)
                    {
                        graph->xaxis_type = atoi(numstr);
                    }
                    else if (axis_mode == YMODE)
                    {
                        graph->yaxis_type = atoi(numstr);
                    }
                }

                numstr[0] = '\0';
                ScaleAndRefreshGraph(graph);
                break;

            case 'm':
                /*
                 * set the plot type
                 */

                if (strlen(numstr) > 0)
                {
                    if (global_operation)
                    {
                        for (plot = graph->plot; plot; plot = plot->next)
                        {
                            plot->plot_type = atoi(numstr);
                        }
                    }
                    else if ((plot = SelectedPlot(graph)))
                    {
                        plot->plot_type = atoi(numstr);
                    }
                }
                else
                {
                    if ((plot = SelectedPlot(graph)))
                    {
                        plot->plot_type = NORMAL_GRAPH;
                    }
                }
                numstr[0] = '\0';
                RefreshGraph(graph);
                break;

            case 'o':
                /*
                 * toggle automatic axis placement mode
                 */
                graph->auto_axes = !graph->auto_axes;
                RefreshGraph(graph);
                break;

            case 'p':
                /*
                 * set the point style
                 */

                if (strlen(numstr) > 0)
                {
                    if ((plot = SelectedPlot(graph)))
                    {
                        plot->point_symbol = atoi(numstr);
                    }
                }
                else
                {
                    if ((plot = SelectedPlot(graph)))
                    {
                        plot->point_symbol = BOX_PT;
                    }
                }

                numstr[0] = '\0';
                RefreshGraph(graph);
                break;

            case 'r':
                /*
                 * toggle the showing of points
                 */

                if (global_operation)
                {
                    for (plot = graph->plot; plot; plot = plot->next)
                    {
                        plot->showpoints = !plot->showpoints;
                    }
                }
                else if ((plot = SelectedPlot(graph)))
                {
                    plot->showpoints = !plot->showpoints;
                }

                RefreshGraph(graph);
                break;

            case 's':
                /*
                 * select the scale of the currently selected plot
                 */

                if (strlen(numstr) > 0)
                {
                    if (global_operation)
                    {
                        for (plot = graph->plot; plot; plot = plot->next)
                        {
                            if (axis_mode == YMODE)
                            {
                                plot->scale = atof(numstr);
                            }
                            else if (axis_mode == XMODE)
                            {
                                plot->xscale = atof(numstr);
                            }
                        }
                    }
                    else if ((plot = SelectedPlot(graph)))
                    {
                        if (axis_mode == YMODE)
                        {
                            plot->scale = atof(numstr);
                        }
                        else if (axis_mode == XMODE)
                        {
                            plot->xscale = atof(numstr);
                        }
                    }
                }

                numstr[0] = '\0';
                ScaleAndRefreshGraph(graph);
                break;

            case 't':
                /*
                 * set the number of ticks
                 */

                if (strlen(numstr) > 0)
                {
                    if (axis_mode == XMODE)
                    {
                        graph->xaxis_desired_nticks = atoi(numstr);
                    }
                    else if (axis_mode == YMODE)
                    {
                        graph->yaxis_desired_nticks = atoi(numstr);
                    }
                }

                numstr[0] = '\0';
                RefreshGraph(graph);
                break;

            case 'x':
                /*
                 * toggle axis placement mode
                 */

                if (global_operation)
                {
                    for (plot = graph->plot; plot; plot = plot->next)
                    {
                        if (axis_mode == XMODE)
                        {
                            plot->xaxis = !plot->xaxis;
                        }
                        else if (axis_mode == YMODE)
                        {
                            plot->yaxis = !plot->yaxis;
                        }
                    }
                }
                else if ((plot = SelectedPlot(graph)))
                {
                    if (axis_mode == XMODE)
                    {
                        plot->xaxis = !plot->xaxis;
                    }
                    else if (axis_mode == YMODE)
                    {
                        plot->yaxis = !plot->yaxis;
                    }
                }

                RefreshGraph(graph);
                break;
            }

            escape_mode = 0;
        }
        else
        {
            switch (c)
            {
            case '?':
                Help();
                break;

            case '\033':  /* escape key */
                escape_mode = 1;
                break;

            case '\001':  /* control-a */
                PrintToFile(graph, "xplot.ps", "a");
                RefreshGraph(graph);
                break;

            case '\004':  /* control-d */
                Quit();
                break;

            case '\005':  /* control-e */
                ZapLabel(graph, event);
                break;

            case '\012':  /* control-l */
                FreeLabels((BasicWindow *)graph);
                RefreshGraph(graph);
                break;

            case '\023':  /* control-s */
                HandCursor(F);
                SelectPlotGraphically(graph, event->x, event->y);
                NormalCursor(F);
                break;

            case '\017':  /* control-o */
                if (strlen(numstr) > 0)
                {
                    val = atof(numstr);
                }
                else
                {
                    val = 0;
                }

                HandCursor(F);
                OffsetPlotGraphically(graph, event->x, event->y, val);
                NormalCursor(F);
                numstr[0] = '\0';
                break;

            case '\024':  /* control-t */
                labelcoord_mode = WINDOW_LBL;   /* enter at window coords */
                TextCursor(F);
                TextSelectInputs(graph);
                SetButtonMode(TEXTMODE);
                break;

            case '\025':  /* control-u */
                /*
                 * erase the current command string
                 */
                numstr[0] = '\0';
                break;

            case '\027':  /* control-w */
                labelcoord_mode = WORLD_LBL;    /* enter at world coords */
                TextCursor(F);
                TextSelectInputs(graph);
                SetButtonMode(TEXTMODE);
                break;

            case 'A':
                if (axis_mode == XMODE)
                {
                    AutoScale(graph, 1, 0);
                }
                else if (axis_mode == YMODE)
                {
                    AutoScale(graph, 0, 1);
                }

                ScaleAndRefreshGraph(graph);
                break;

            case 'D':
                /*
                 * shift the selected plot down in the list
                 */

                ival = 1;

                if (strlen(numstr) > 0)
                {
                    ival = atoi(numstr);
                }

                numstr[0] = '\0';

                for (i = 0; i < ival; i++)
                {
                    ShiftPlotList(graph);
                }

                /*
                 * update the plot legends
                 */

                cnt = 0;

                for (plot = graph->plot; plot; plot = plot->next)
                {
                    DrawPlotLegend(plot, cnt);
                    cnt++;
                }

                break;

            case 'F':
                PrintToFile(graph, "xplot.ps", "w");
                RefreshGraph(graph);
                break;

            case 'I':
                /*
                 * set the shift percentage
                 */

                if (strlen(numstr) > 0)
                {
                    shiftpercent = atof(numstr);
                }

                numstr[0] = '\0';
                /*
                 * up shift plot by a percentage of the window bounds
                 */
                val = (graph->wymax - graph->wymin) * shiftpercent;

                if ((plot = SelectedPlot(graph)))
                {
                    plot->xaxisoffset += val;
                    ScaleAndRefreshGraph(graph);
                }

                break;

            case 'J':
                /*
                 * set the shift percentage
                 */

                if (strlen(numstr) > 0)
                {
                    shiftpercent = atof(numstr);
                }

                numstr[0] = '\0';

                /*
                 * left shift plot by a percentage of the window bounds
                 */

                val = (graph->wxmax - graph->wxmin) * shiftpercent;

                if ((plot = SelectedPlot(graph)))
                {
                    plot->yaxisoffset -= val;
                    ScaleAndRefreshGraph(graph);
                }

                break;

            case 'K':
                /*
                 * set the shift percentage
                 */

                if (strlen(numstr) > 0)
                {
                    shiftpercent = atof(numstr);
                }

                numstr[0] = '\0';

                /*
                 * right shift plot by a percentage of the window bounds
                 */

                val = (graph->wxmax - graph->wxmin) * shiftpercent;

                if ((plot = SelectedPlot(graph)))
                {
                    plot->yaxisoffset += val;
                    ScaleAndRefreshGraph(graph);
                }

                break;

            case 'L':
                /*
                 * toggle legends
                 */

                graph->show_legend = !graph->show_legend;
                RefreshGraph(graph);
                break;

            case 'M':
                /*
                 * set the shift percentage
                 */

                if (strlen(numstr) > 0)
                {
                    shiftpercent = atof(numstr);
                }

                numstr[0] = '\0';

                /*
                 * down shift plot by a percentage of the window bounds
                 */

                val = (graph->wymax - graph->wymin) * shiftpercent;

                if ((plot = SelectedPlot(graph)))
                {
                    plot->xaxisoffset -= val;
                    ScaleAndRefreshGraph(graph);
                }

                break;

            case 'P':
                /*
                 * do the postscript output at the desired scale
                 * which blanks the screen
                 */

                PrintOut(graph);

                /*
                 * redisplay the screen
                 */

                RefreshGraph(graph);
                break;

            case 'T':
                /*
                 * toggle title
                 */

                graph->show_title = !graph->show_title;
                RefreshGraph(graph);
                break;

            case 'V':
                /*
                 * make all plots visible
                 */

                for (plot = graph->plot; plot; plot = plot->next)
                {
                    plot->visible = 1;
                }

            case '/':
                ClearWindow((BasicWindow *)graph->frame->text);
                command_entry = 1;
                strcpy(numstr, "/");
                break;

            case '[':
                /*
                 * set the lower bound
                 */

                if (strlen(numstr) > 0)
                {
                    if (axis_mode == XMODE)
                    {
                        graph->wxmin = atof(numstr);
                    }
                    else if (axis_mode == YMODE)
                    {
                        graph->wymin = atof(numstr);
                    }
                }

                numstr[0] = '\0';
                ScaleAndRefreshGraph(graph);
                break;

            case ']':
                /*
                 * set the upper bound
                 */

                if (strlen(numstr) > 0)
                {
                    if (axis_mode == XMODE)
                    {
                        graph->wxmax = atof(numstr);
                    }
                    else if (axis_mode == YMODE)
                    {
                        graph->wymax = atof(numstr);
                    }
                }

                numstr[0] = '\0';
                ScaleAndRefreshGraph(graph);
                break;

            case '<':
                /*
                 * zoom out by a percentage of the window bounds
                 */
                val = (graph->wymax - graph->wymin) * .05;
                graph->wymax += val;
                graph->wymin -= val;
                val = (graph->wxmax - graph->wxmin) * .05;
                graph->wxmax += val;
                graph->wxmin -= val;
                ScaleAndRefreshGraph(graph);
                break;

            case '>':
                /*
                 * zoom in by a percentage of the window bounds
                 */
                val = (graph->wymax - graph->wymin) * .05;
                graph->wymax -= val;
                graph->wymin += val;
                val = (graph->wxmax - graph->wxmin) * .05;
                graph->wxmax -= val;
                graph->wxmin += val;
                ScaleAndRefreshGraph(graph);
                break;

            case 'a':
                AutoScale(graph, graph->autoscale_x, graph->autoscale_y);
                ScaleAndRefreshGraph(graph);
                break;

            case 'b':
                /*
                 * make the selected plot invisible and the next one visible;
                 * equivalent to v-s-v
                 */
                numstr[0] = '\0';
                ToggleVisibility(graph, plot);
                GotoPlot(graph, plot, &ival, 1);
                ToggleVisibility(graph, plot);
                break;

            case 'c':
                /*
                 * make the selected plot invisible and the next one visible;
                 * equivalent to v-p-v
                 */
                /*
                 * set number of plots to jump if any
                 */
                numstr[0] = '\0';
                ToggleVisibility(graph, plot);
                GotoPlot(graph, plot, &ival, -1);
                ToggleVisibility(graph, plot);
                break;

            case 'd':
                /*
                 * make the selected plot invisible and the next one visible;
                 * equivalent to v-s-v; does this for two sets of plots
                 * separated by a jump
                 */

                /*
                 * Calculate the two target plots from the currently selected
                 * ones.  This will do stupid things if you have selected
                 * more than two plots.
                 */

                plotcount = 0;
                plottarget1 = plottarget2 = -1;

                for (plot = graph->plot; plot; plot = plot->next)
                {
                    if (plot->visible)
                    {
                        if (plottarget1 < 0)
                            plottarget1 = plotcount + 1;
                        else
                            plottarget2 = plotcount + 1;

                        plot->visible = 0;
                    }

                    if (plot->selected)
                    {
                        plotselected = plotcount + 1;
                    }

                    plotcount++;
                }

                if (plottarget1 == plotcount)
                    plottarget1 = 0;

                if (plottarget2 == plotcount)
                    plottarget2 = 0;

                if (plotselected == plotcount)
                    plotselected = 0;

                SelectPlot(graph, plotselected);

                plotcount = 0;

                for (plot = graph->plot; plot; plot = plot->next)
                {
                    if (plotcount == plottarget1 || plotcount == plottarget2)
                        plot->visible = 1;
                    plotcount++;
                }

                RefreshGraph(graph);
                break;


            case 'f':
                /*
                 * make the selected plot invisible and the next one visible;
                 * equivalent to v-p-v; does this for two sets of plots
                 * separated by a jump
                 */

                /*
                 * Calculate the two target plots from the currently selected
                 * ones.  This will do stupid things if you have selected
                 * more than two plots.
                 */

                plotcount = 0;
                plottarget1 = plottarget2 = -1;

                for (plot = graph->plot; plot; plot = plot->next)
                {
                    if (plot->visible)
                    {
                        if (plottarget1 < 0)
                            plottarget1 = plotcount - 1;
                        else
                            plottarget2 = plotcount - 1;
                        plot->visible = 0;
                    }

                    if (plot->selected)
                    {
                        plotselected = plotcount - 1;
                    }

                    plotcount++;
                }

                if (plottarget1 < 0)
                    plottarget1 = plotcount - 1;

                if (plottarget2 < 0)
                    plottarget2 = plotcount - 1;

                if (plotselected < 0)
                    plotselected = plotcount - 1;

                SelectPlot(graph, plotselected);

                plotcount = 0;

                for (plot = graph->plot; plot; plot = plot->next)
                {
                    if (plotcount == plottarget1 || plotcount == plottarget2)
                        plot->visible = 1;
                    plotcount++;
                }

                RefreshGraph(graph);
                break;

            case 'g':
                /*
                 * toggle grid
                 */

                if (axis_mode == XMODE)
                {
                    graph->show_xgrid = !graph->show_xgrid;
                }

                if (axis_mode == YMODE)
                {
                    graph->show_ygrid = !graph->show_ygrid;
                }

                RefreshGraph(graph);
                break;

            case 'i':
                /*
                 * set the shift percentage
                 */

                if (strlen(numstr) > 0)
                {
                    shiftpercent = atof(numstr);
                }

                numstr[0] = '\0';
                /*
                 * up shift plot by a percentage of the window bounds
                 */
                val = (graph->wymax - graph->wymin) * shiftpercent;
                graph->wymax += val;
                graph->wymin += val;
                ScaleAndRefreshGraph(graph);
                break;

            case 'j':
                /*
                 * set the shift percentage
                 */

                if (strlen(numstr) > 0)
                {
                    shiftpercent = atof(numstr);
                }

                numstr[0] = '\0';
                LeftShift();
                break;

            case 'k':
                /*
                 * set the shift percentage
                 */

                if (strlen(numstr) > 0)
                {
                    shiftpercent = atof(numstr);
                }

                numstr[0] = '\0';
                RightShift();
                break;

            case 'l':
                /*
                 * toggle labels
                 */

                if (axis_mode == XMODE)
                {
                    graph->show_xlabels = !graph->show_xlabels;
                }

                if (axis_mode == YMODE)
                {
                    graph->show_ylabels = !graph->show_ylabels;
                }

                RefreshGraph(graph);
                break;

            case 'm':
                /*
                 * set the shift percentage
                 */

                if (strlen(numstr) > 0)
                {
                    shiftpercent = atof(numstr);
                }

                numstr[0] = '\0';
                /*
                 * down shift plot by a percentage of the window bounds
                 */
                val = (graph->wymax - graph->wymin) * shiftpercent;
                graph->wymax -= val;
                graph->wymin -= val;
                ScaleAndRefreshGraph(graph);
                break;

            case 'n':
                /*
                 * set number of plots to jump
                 */

                if (strlen(numstr) > 0)
                {
                    jump = atoi(numstr);
                }
                else
                    jump = 0;
                numstr[0] = '\0';
                printf("jump = %d\n", jump);
                break;

            case 'o':
                AutoOrigin(graph);
                RefreshGraph(graph);
                break;

            case 'p':
                GotoPlot(graph, plot, &ival, -1);
                break;

            case 'q':
                Quit();
                break;

            case 'r':
                RefreshGraph(graph);
                break;

            case 's':
                GotoPlot(graph, plot, &ival, 1);
                break;

            case 't':
                /* make all plots invisible */
                for (plot = graph->plot; plot; plot = plot->next)
                {
                    plot->visible = 0;
                }

                RefreshGraph(graph);
                break;

            case 'v':
                /*
                 * toggle visibility
                 */
                if (global_operation)
                {
                    for (plot = graph->plot; plot; plot = plot->next)
                    {
                        plot->visible = !plot->visible;
                    }

                    RefreshGraph(graph);
                }
                else
                    ToggleVisibility(graph, plot);
                break;

            case 'x':
                axis_mode = XMODE;
                break;

            case 'y':
                axis_mode = YMODE;
                break;

            case 'z':
                /* Toggle `truncate_tick_values'. */
                graph->truncate_tick_values = 1 - graph->truncate_tick_values;
                RefreshGraph(graph);

            case '|':
                /*
                 * set the axis intercept
                 */
                if (strlen(numstr) > 0)
                {
                    if (axis_mode == XMODE)
                    {
                        graph->xaxis_yintcpt = atof(numstr);
                    }
                    else if (axis_mode == YMODE)
                    {
                        graph->yaxis_xintcpt = atof(numstr);
                    }
                }

                numstr[0] = '\0';
                RefreshGraph(graph);
                break;

            case '=':
                /*
                 * select the offset of the currently selected plot
                 */

                if (strlen(numstr) > 0)
                {
                    if (global_operation)
                    {
                        for (plot = graph->plot; plot; plot = plot->next)
                        {
                            if (axis_mode == XMODE)
                            {
                                plot->xoffset = atof(numstr);
                            }
                            else if (axis_mode == YMODE)
                            {
                                plot->yoffset = atof(numstr);
                            }
                        }
                    }
                    else if ((plot = SelectedPlot(graph)))
                    {
                        if (axis_mode == XMODE)
                        {
                            plot->xoffset = atof(numstr);
                        }
                        else if (axis_mode == YMODE)
                        {
                            plot->yoffset = atof(numstr);
                        }
                    }
                }

                numstr[0] = '\0';
                ScaleAndRefreshGraph(graph);
                break;

            case '*':
                /*
                 * select global operation
                 */

                global_operation = 1;
                break;
            }
        }
    }
}

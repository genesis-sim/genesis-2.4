/*
 * FILE: arg.c
 *
 */

#include "xplot_ext.h"
#include <signal.h>
#include <X11/Xutil.h>

static char *commandstr;
static int commandsource;


char *
CommandStr(void)
{
    return commandstr;
}


int
CommandSource(void)
{
    return commandsource;
}


DataSource *
AddSource(Graph *graph)
{
    DataSource *source;

    /*
     * allocate it
     */

    source = (DataSource *) calloc(1, sizeof(DataSource));

    /*
     * insert it into the global list
     */

    source->next = graph->source;
    graph->source = source;
    source->parent = (BasicWindow *) graph;
    return source;
}


void
ParseArgList(Frame *frame, int argc, char **argv)
{
    int     nxtarg;
    int     filecnt;
    DataSource *source;
    Graph  *graph;
    int     x, y, width, height;
    int     status;

    frame->color_mode = 1;
    frame->fontname = NULL;
    frame->inverse = -1;
    frame->display_name = NULL; /* use the environment's DISPLAY value */
    frame->x = -1;              /* default geometry */
    frame->y = -1;
    frame->width = 800;
    frame->height = 700;

    graph = frame->graph;
    graph->dragx1 = -1;
    graph->dragx2 = -1;
    graph->resolution = 1;  /* hi res postscript output */
    nxtarg = 0;

    graph->autoscale_x = TRUE;
    graph->autoscale_y = TRUE;
    graph->yaxistitle_offset = 0;
    graph->xaxistitle_offset = 0;
    graph->xticklabel_offset = 0;
    graph->yticklabel_offset = 0;

    graph->yaxis_tickinc = 0;   /* automatic tick interval calc */
    graph->xaxis_tickinc = 0;

    graph->yaxis_nsubticks = 0;
    graph->xaxis_nsubticks = 0;

    graph->show_xaxis = TRUE;
    graph->show_yaxis = TRUE;

    graph->xaxis_desired_nticks = -60;  /* 60 pixel spacing between xticks */
    graph->yaxis_desired_nticks = -40;  /* 40 pixel spacing between yticks */

    graph->ticksize = 3;    /* 3 pixel ticks */

    graph->show_xlabels = 1;    /* show all labels initially */
    graph->show_ylabels = 1;
    graph->show_legend = TRUE;
    graph->show_title = TRUE;
    graph->quadrants = 0xf; /* display all quadrants */
    graph->gridcolor = 90;

    filecnt = 0;
    graph->xaxis_title = "";
    graph->yaxis_title = "";

    graph->xaxis_leftdp = 4;
    graph->yaxis_leftdp = 4;

    graph->xaxis_rightdp = -1;  /* use the %g formatting */
    graph->yaxis_rightdp = -1;

    graph->title = NULL;
    commandstr = NULL;

    while (argc > ++nxtarg)
    {
        if (arg_is("-usage") || arg_is("-help"))
        {
            fprintf(stderr,
                    "usage: %s file [file ..][-title name][-display name]\n",
                    argv[0]);
            fprintf(stderr,
                    "         [-bw][-inverse][-font name][-geometry str]\n");
            fprintf(stderr, "         [-xlh # #][-ylh # #][-debug]\n");
            fprintf(stderr, "         [-xtitle name][-ytitle name]\n");
            fprintf(stderr, "         [-command str][-commandfile name]\n");
            fprintf(stderr, "         [-restore graphfile]\n");
            exit(0);
        }
        else if (arg_is("-display"))
        {
            frame->display_name = argv[++nxtarg];
        }
        else if (arg_is("-restore"))
        {
            SetRestoreFile(argv[++nxtarg]);
        }
        else if (arg_is("-commandfile"))
        {
            commandstr = argv[++nxtarg];
            commandsource = FROM_FILE;
        }
        else if (arg_is("-command"))
        {
            commandstr = argv[++nxtarg];
            commandsource = FROM_OTHER;
        }
        else if (arg_is("-bw"))
        {
            frame->color_mode = 0;
        }
        else if (arg_is("-inverse"))
        {
            frame->inverse = 1;
        }
        else if (arg_is("-title"))
        {
            graph->title = argv[++nxtarg];
        }
        else if (arg_is("-font"))
        {
            frame->fontname = argv[++nxtarg];
        }
        else if (arg_is("-xtitle"))
        {
            graph->xaxis_title = argv[++nxtarg];
        }
        else if (arg_is("-ytitle"))
        {
            graph->yaxis_title = argv[++nxtarg];
        }
        else if (arg_is("-nomap"))
        {
            frame->mapped = 0;
        }
        else if (arg_is("-debug"))
        {
            debug = TRUE;
        }
        else if (arg_is("-xlh"))
        {
            graph->autoscale_x = FALSE;
            graph->xmin = farg();
            graph->xmax = farg();
        }
        else if (arg_is("-ylh"))
        {
            graph->autoscale_y = FALSE;
            graph->ymin = farg();
            graph->ymax = farg();
        }
        else if (arg_is("-geometry"))
        {
            status = XParseGeometry(argv[++nxtarg], &x, &y,
                                    (unsigned *)&width, (unsigned *)&height);

            if (status & XValue)
            {
                frame->x = x;
            }

            if (status & YValue)
            {
                frame->y = y;
            }

            if (status & WidthValue)
            {
                frame->width = width;
            }

            if (status & HeightValue)
            {
                frame->height = height;
            }
        }
        else if (!arg_starts_with('-'))
        {
            source = AddSource(graph);
            source->type = FROM_FILE;
            source->filename = argv[nxtarg];
            filecnt++;
        }
        else
        {
            fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                    argv[nxtarg]);
        }
    }

    if (filecnt == 0)
    {
        /*
         * use stdin
         */

        source = AddSource(graph);
        source->filename = "STDIN";
        source->type = FROM_FILE;
    }
}

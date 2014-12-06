/*
 * FILE: load.c
 *
 */

#include "xplot_ext.h"

#define BLOCKSIZE 1000

char *restorefile = NULL;

void
SetRestoreFile(char *name)
{
    restorefile = name;
}

void
ClosePlot(Plot *plot)
{
    if (plot->npoints > 0)
    {
        /*
         * allocate space for the screen coordinates
         */

        if (plot->coord == NULL)
        {
            plot->coord = (Coord *) malloc(plot->npoints * sizeof(Coord));
        }

        if (plot->fcoord == NULL)
        {
            plot->fcoord = (FCoord *) malloc(plot->npoints * sizeof(FCoord));
        }

        if (plot->xhi < 0)
        {
            plot->xhi = plot->npoints - 1;
        }
    }
    else
    {
        DeletePlot(plot);
    }
}

Plot *
AddPlot(Graph *graph)
{
    Plot   *plot;

    /*
     * allocate it
     */

    plot = (Plot *) calloc(1, sizeof(Plot));

    /*
     * insert it into the global list
     */

    plot->next = graph->plot;
    graph->plot = plot;
    plot->graph = graph;
    plot->visible = 1;
    plot->showpoints = 0;
    plot->pointfreq = 1;
    plot->linestyle = -1;   /* indicate that no linestyle was specified */
    plot->scale = 1;
    plot->xscale = 1;
    plot->escale = 1;
    plot->data = (FCoord *) malloc(BLOCKSIZE * sizeof(FCoord));
    plot->arraysize = BLOCKSIZE;
    plot->npoints = 0;
    plot->xhi = -1;
    plot->xlo = 0;
    return plot;
}

void
FreePlot(Plot *plot)
{
    if (plot == NULL)
        return;

    if (plot->data)
    {
        free(plot->data);
    }

    if (plot->coord)
    {
        free(plot->coord);
    }

    if (plot->fcoord)
    {
        free(plot->fcoord);
    }

    if (plot->errorbar)
    {
        free(plot->errorbar);
    }

    free(plot);
}

void
DeletePlot(Plot *plot)
{
    Plot   *p;
    Graph  *graph;

    if (plot == NULL)
        return;

    graph = plot->graph;

    if (graph->plot == plot)
    {
        graph->plot = plot->next;
        FreePlot(plot);
        return;
    }

    for (p = graph->plot; p; p = p->next)
    {
        if (p->next == plot)
        {
            p->next = plot->next;
            FreePlot(plot);
            return;
        }
    }
}


void
DeleteSource(Graph *graph, DataSource *source)
{
    DataSource *s;

    if (graph->source == source)
    {
        graph->source = source->next;
        free(source);
        return;
    }

    for (s = graph->source; s; s = s->next)
    {
        if (s->next == source)
        {
            s->next = source->next;
            free(source);
            return;
        }
    }
}


void
AddPoint(Plot *plot, char *line)
{
    float   x, y, err;
    int     nitems;
    int     have_error;

    /*
     * get the coordinates
     */

    have_error = 0;

    if (plot->auto_x > 0)
    {
        nitems = sscanf(line, "%f%f", &y, &err);

        if (nitems < 1)
        {
            return;
        }

        if (nitems > 1)
        {
            have_error = 1;
        }

        plot->data[plot->npoints].x = plot->npoints * plot->auto_x;
    }
    else
    {
        nitems = sscanf(line, "%f%f%f", &x, &y, &err);

        if (nitems < 1)
        {
            return;
        }

        if (nitems == 1)
        {
            y = x;
            x = plot->npoints;
        }

        if (nitems > 2)
        {
            have_error = 1;
        }

        plot->data[plot->npoints].x = x;
    }

    plot->data[plot->npoints].y = y;

    /*
     * check for error bar values
     */

    if (have_error)
    {
        if (plot->npoints < 1)
        {
            plot->errorbar = (ErrorData *)
                malloc(BLOCKSIZE * sizeof(ErrorData));
            plot->show_error = 1;
        }

        plot->errorbar[plot->npoints].value = err;
    }

    /*
     * keep track of the min max values
     */

    if (plot->npoints < 1)
    {
        plot->xmin = plot->xmax = x;
        plot->ymin = plot->ymax = y;
    }
    else
    {
        if (x < plot->xmin)
            plot->xmin = x;

        if (x > plot->xmax)
            plot->xmax = x;

        if (y < plot->ymin)
            plot->ymin = y;

        if (y > plot->ymax)
            plot->ymax = y;
    }

    /*
     * increment the points counter
     */

    plot->xhi = plot->npoints;
    ++(plot->npoints);

    /*
     * check the array size and allocate more space if
     * necessary
     */

    if (plot->npoints % BLOCKSIZE == 0)
    {
        plot->arraysize += BLOCKSIZE;
        plot->data = (FCoord *)
            realloc(plot->data, plot->arraysize * sizeof(FCoord));

        if (plot->errorbar)
        {
            plot->errorbar = (ErrorData *)
                realloc(plot->errorbar, plot->arraysize * sizeof(ErrorData));
        }
    }
}


void
LoadGraphData(Graph *graph)
{
    DataSource *source;

    /*
     * if restore file is set then load that
     */

    if (restorefile)
    {
        RestoreGraph(graph, restorefile);
    }
    else
    {
        /*
         * go through the data sources
         */

        for (source = graph->source; source; source = source->next)
        {
            LoadPlotFromSource(graph, source);
        }
    }
}


void
LoadPlotFromSource(Graph *graph, DataSource *source)
{
    Plot *plot;
    FILE *fp;
    char  line[1001];

    if (source->type == FROM_FILE)
    {
        /*
         * open the data file associated with the plot
         */

        if (strcmp(source->filename, "STDIN") == 0)
        {
            fp = stdin;
        }
        else if ((fp = fopen(source->filename, "r")) == NULL)
        {
            printf("can't open file '%s'\n", source->filename);
            DeleteSource(graph, source);
            return;
        }

        /*
         * read the data source file
         */

        /*
         * automatically add a plot for the data file
         */

        plot = AddPlot(graph);

        /*
         * give the plot the default title which is the
         * filename the data is coming from
         */

        plot->title = plot->filename = source->filename;

        while (!feof(fp))
        {
            /*
             * read a line from the data file
             */

            if (fgets(line, 1000, fp) == NULL)
            {
                break;
            }

            line[1000] = '\0';
            DoCommand(graph, &plot, line);
        }

        ClosePlot(plot);
        fclose(fp);
    }
}


void
LoadPlot(Graph *graph, char *filename)
{
    DataSource *AddSource(Graph *graph);
    DataSource *source;
    Plot       *plot;
    int         selected = 0;

    if (graph == NULL || filename == NULL)
        return;

    source = AddSource(graph);
    source->type = FROM_FILE;
    source->filename = CopyString(filename);
    LoadPlotFromSource(graph, source);

    /*
     * make sure there are plots. If not then exit
     */

    if (!graph->plot)
    {
        return;
    }

    /*
     * setup the default linestyle of the plots
     */

    for (plot = graph->plot; plot; plot = plot->next)
    {
        if (plot->linestyle == -1)
        {
            plot->linestyle = graph->foreground;
        }

        if (plot->selected)
        {
            selected = 1;
        }

    }

    if (!selected)
    {
        /*
         * select the first plot
         */
        graph->plot->selected = 1;
    }
}


void
DoCommand(Graph * graph, Plot ** plot, char *lineptr)
{
    Plot *p;

    do
    {
        if (lineptr[0] == ';')
        {
            lineptr++;
            /* skip white space */

            while (lineptr && *lineptr == ' ')
            {
                if (*lineptr == '\0' || *lineptr == '\n')
                    break;
                lineptr++;
            }
        }

        /*
         * Check to see if it is a plot command.
         */

        if (lineptr[0] == '/')
        {
            if (GlobalOperation())
            {
                for (p = graph->plot; p != NULL; p = p->next)
                {
                    InterpretCommand(graph, &p, lineptr);
                }

                ResetGlobalOperation();
            }
            else
            {
                InterpretCommand(graph, plot, lineptr);
            }
        }
        else if (lineptr[0] == '%')
        {
            /*
             * Skip the rest of the line for a comment.
             */

            return;
        }

        /*
         * Otherwise assume that the line contains coordinates
         * to be added to the plot.
         */

        AddPoint(*plot, lineptr);

        if (*lineptr == '\0')
            break;
    }
    while ((lineptr = strchr(lineptr + 1, ';')));
}



void
DoFileCommands(Graph * graph, Plot ** plot, char *filename)
{
    FILE   *fp;
    char    line[1001];

    /*
     * open the data file associated with the plot
     */

    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("can't open file '%s'\n", filename);
        return;
    }

    while (!feof(fp))
    {
        /*
         * read a line from the data file
         */

        if (fgets(line, 1000, fp) == NULL)
        {
            break;
        }

        line[1000] = '\0';
        DoCommand(graph, plot, line);
    }

    fclose(fp);
}

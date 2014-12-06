/*
 * FILE: save.c
 *
 */

#include "xplot_ext.h"

int
SaveGraph(Graph *graph, char *file)
{
    FILE *fp;
    Plot *plot;
    DataSource *source;
    int i;
    int offset, size;
    Label *label;

    if (graph == NULL || file == NULL)
    {
        return 0;
    }

    /*
     * open the file
     */

    if ((fp = fopen(file, "w")) == NULL)
    {
        fprintf(stderr, "unable to open file '%s'\n", file);
        return 0;
    }

    fprintf(fp, "PLOTFILE1.4\n");

    /*
     * write out the graph data structure
     */

    offset = (int) (&(graph->label)) - (int) (graph);
    size = sizeof(Graph) - offset;
    fwrite(((char *) graph) + offset, size, 1, fp);

    /*
     * store the strings at the end
     */

    fprintf(fp, "%s\n", graph->title);
    fprintf(fp, "%s\n", graph->xaxis_title);
    fprintf(fp, "%s\n", graph->yaxis_title);

    /*
     * count the labels
     */

    i = 0;

    for (label = graph->label; label; label = label->next)
    {
        i++;
    }

    fwrite(&i, sizeof(int), 1, fp);

    /*
     * write out the label list
     */

    for (label = graph->label; label; label = label->next)
    {
        fwrite(label, sizeof(Label), 1, fp);
    }

    /*
     * count the sources
     */

    i = 0;

    for (source = graph->source; source; source = source->next)
    {
        i++;
    }

    fwrite(&i, sizeof(int), 1, fp);

    /*
     * write out the data source list
     */
    for (source = graph->source; source; source = source->next)
    {
        fwrite(source, sizeof(DataSource), 1, fp);
        fprintf(fp, "%s\n", source->filename);
    }
    /*
     * write out the plots
     */

    /*
     * count the plots
     */

    i = 0;

    for (plot = graph->plot; plot; plot = plot->next)
    {
        i++;
    }

    fwrite(&i, sizeof(int), 1, fp);

    for (plot = graph->plot; plot; plot = plot->next)
    {
        /*
         * write out the plot data structure
         */

        fwrite(plot, sizeof(Plot), 1, fp);

        /*
         * store the data at the end
         */

        /*
         * write out the data
         */

        if (plot->data)
        {
            i = 1;
            fwrite(&i, sizeof(int), 1, fp);
            fwrite(plot->data, sizeof(FCoord), plot->npoints, fp);
        }
        else
        {
            i = 0;
            fwrite(&i, sizeof(int), 1, fp);
        }

        if (plot->coord)
        {
            i = 1;
            fwrite(&i, sizeof(int), 1, fp);
            fwrite(plot->coord, sizeof(Coord), plot->npoints, fp);
        }
        else
        {
            i = 0;
            fwrite(&i, sizeof(int), 1, fp);
        }

        if (plot->fcoord)
        {
            i = 1;
            fwrite(&i, sizeof(int), 1, fp);
            fwrite(plot->fcoord, sizeof(FCoord), plot->npoints, fp);
        }
        else
        {
            i = 0;
            fwrite(&i, sizeof(int), 1, fp);
        }

        if (plot->errorbar)
        {
            i = 1;
            fwrite(&i, sizeof(int), 1, fp);
            fwrite(plot->errorbar, sizeof(ErrorData), plot->npoints, fp);
        }
        else
        {
            i = 0;
            fwrite(&i, sizeof(int), 1, fp);
        }

        /*
         * store the strings at the end
         */

        fprintf(fp, "%s\n", plot->filename);
        fprintf(fp, "%s\n", plot->title);
    }

    fwrite(&F->wwidth, sizeof(int), 1, fp);
    fwrite(&F->wheight, sizeof(int), 1, fp);
    fclose(fp);
    return 1;
}


int
RestoreGraph(Graph *graph, char *file)
{
    FILE  *fp;
    Plot  *plot = NULL, *newplot;
    DataSource *source = NULL, *newsource;
    char   line[1000];
    int    count;
    int    i;
    int    flag;
    int    offset, size;
    char  *ptr;
    Label *label = NULL, *newlabel;
    int    status;

    if (graph == NULL || file == NULL)
    {
        return 0;
    }

    /*
     * open the file
     */

    if ((fp = fopen(file, "r")) == NULL)
    {
        fprintf(stderr, "unable to open file '%s'\n", file);
        return 0;
    }

    /*
     * check the header
     */

    fgets(line, 1000, fp);

    if (strcmp(line, "PLOTFILE1.4\n") != 0)
    {
        fprintf(stderr, "invalid save file\n");
        fclose(fp);
        return 0;
    }

    /*
     * read in the graph data structure
     */

    offset = (int) (&(graph->label)) - (int) (graph);
    size = sizeof(Graph) - offset;
    fread(((char *) graph) + offset, size, 1, fp);
    graph->source = NULL;
    graph->plot = NULL;
    graph->frame = F;
    graph->next = NULL;
    graph->label = NULL;


    /*
     * read the strings at the end
     */

    fgets(line, 1000, fp);

    if ((ptr = strchr(line, '\n')))
    {
        *ptr = '\0';
    }

    graph->title = CopyString(line);

    fgets(line, 1000, fp);

    if ((ptr = strchr(line, '\n')))
    {
        *ptr = '\0';
    }

    graph->xaxis_title = CopyString(line);

    fgets(line, 1000, fp);

    if ((ptr = strchr(line, '\n')))
    {
        *ptr = '\0';
    }

    graph->yaxis_title = CopyString(line);

    /*
     * read in the label list
     */

    fread(&count, sizeof(int), 1, fp);

    for (i = 0; i < count; i++)
    {
        newlabel = (Label *) malloc(sizeof(Label));
        newlabel->next = NULL;

        if (graph->label == NULL)
        {
            graph->label = newlabel;
        }
        else
        {
            label->next = newlabel;
        }

        fread(newlabel, sizeof(Label), 1, fp);
        label = newlabel;
    }

    /*
     * read in the data source list
     */

    fread(&count, sizeof(int), 1, fp);

    for (i = 0; i < count; i++)
    {
        newsource = (DataSource *) malloc(sizeof(DataSource));

        if (graph->source == NULL)
        {
            graph->source = newsource;
        }
        else
        {
            source->next = newsource;
        }

        fread(newsource, sizeof(DataSource), 1, fp);
        newsource->next = NULL;
        source = newsource;
        fgets(line, 1000, fp);

        if ((ptr = strchr(line, '\n')))
        {
            *ptr = '\0';
        }

        source->filename = CopyString(line);
    }

    /*
     * read in the plots
     */

    fread(&count, sizeof(int), 1, fp);

    for (i = 0; i < count; i++)
    {
        newplot = (Plot *) malloc(sizeof(Plot));

        if (graph->plot == NULL)
        {
            graph->plot = newplot;
        }
        else
        {
            plot->next = newplot;
        }

        fread(newplot, sizeof(Plot), 1, fp);
        newplot->next = NULL;
        newplot->graph = graph;
        plot = newplot;
        plot->data = NULL;
        plot->coord = NULL;
        plot->fcoord = NULL;
        plot->errorbar = NULL;

        /*
         * read the data at the end
         */

        fread(&flag, sizeof(int), 1, fp);

        if (flag)
        {
            plot->data = (FCoord *) malloc(plot->npoints * sizeof(FCoord));
            fread(plot->data, sizeof(FCoord), plot->npoints, fp);
        }

        fread(&flag, sizeof(int), 1, fp);

        if (flag)
        {
            plot->coord = (Coord *) malloc(plot->npoints * sizeof(Coord));
            fread(plot->coord, sizeof(Coord), plot->npoints, fp);
        }

        fread(&flag, sizeof(int), 1, fp);

        if (flag)
        {
            plot->fcoord = (FCoord *) malloc(plot->npoints * sizeof(FCoord));
            fread(plot->fcoord, sizeof(FCoord), plot->npoints, fp);
        }

        fread(&flag, sizeof(int), 1, fp);

        if (flag)
        {
            plot->errorbar = (ErrorData *)malloc(plot->npoints
                                                 * sizeof(ErrorData));
            fread(plot->errorbar, sizeof(ErrorData), plot->npoints, fp);
        }

        /*
         * read the strings at the end
         */

        fgets(line, 1000, fp);

        if ((ptr = strchr(line, '\n')))
        {
            *ptr = '\0';
        }

        plot->filename = CopyString(line);
        fgets(line, 1000, fp);

        if ((ptr = strchr(line, '\n')))
        {
            *ptr = '\0';
        }

        plot->title = CopyString(line);
    }

    /*
     * restore the window dimension
     */

    status = fread(&F->width, sizeof(int), 1, fp);
    status = fread(&F->height, sizeof(int), 1, fp);

    if (status != 1)
    {
        F->width = -1;
        F->height = -1;
    }

    fclose(fp);
    return 1;
}

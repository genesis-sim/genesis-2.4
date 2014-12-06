/*
 * FILE: save2.c
 *
 */

/* CHECKME: is this file really needed? */

/*
 * This file saves a selected plot as a standard xplot ascii file.
 * It has a number of limitations.
 */

#include "xplot_ext.h"

int
Save(Graph *graph, char *file)
{
    FILE *fp;
    Plot *plot, *last_plot, *next_plot;
    DataSource *source;
    int i, j;
    Label *label;

    if ((graph == NULL) || (file == NULL))
    {
        return 0;
    }

    /* Open the file. */

    if ((fp = fopen(file, "w")) == NULL)
    {
        fprintf(stderr, "unable to open file '%s'\n", file);
        return 0;
    }

    /* Save the title strings. */

    if (graph->title)
        fprintf(fp, "/graphtitle %s\n", graph->title);

    if (strcmp(graph->xaxis_title, "") != 0)
        fprintf(fp, "/xtitle %s\n", graph->xaxis_title);

    if (strcmp(graph->yaxis_title, "") != 0)
        fprintf(fp, "/ytitle %s\n", graph->yaxis_title);

    /* Write out the label list. */

    for (label = graph->label; label; label = label->next)
    {
        /* fwrite(label, sizeof(Label), 1, fp); */
    }

    /* Write out the data source list. */
    for (source = graph->source; source; source = source->next)
    {
        /* fwrite(source, sizeof(DataSource), 1, fp); */
        /* fprintf(fp, "%s\n", source->filename); */
    }

    /*
     * Reverse the order of the plots.
     */

    last_plot = NULL;
    plot = graph->plot;

    while(1)
    {
        next_plot = plot->next;
        plot->next = last_plot;

        if (next_plot == NULL)
            break;

        last_plot = plot;
        plot = next_plot;
    }

    graph->plot = plot;

    /*
     * Write out the plots.
     */

    for (plot = graph->plot; plot; plot = plot->next)
    {
        /* Title strings. */
        fprintf(fp, "/newplot\n");
        fprintf(fp, "/plotname %s\n", plot->title);

        if (plot->xaxisoffset != 0.0)  /* BOGOSITY ALERT! (float compare) */
            fprintf(fp, "/xaxisoffset %g\n", plot->xaxisoffset);

        if (plot->yaxisoffset != 0.0)  /* BOGOSITY ALERT! (float compare) */
            fprintf(fp, "/yaxisoffset %g\n", plot->yaxisoffset);


        /* Write out the data. */

        if (plot->data)
        {
            for (j = 0; j < plot->npoints; j++)
                fprintf(fp, "%g %g\n", plot->data[j].x, plot->data[j].y);
        }

        if (plot->errorbar)
        {
            i = 1;
        }
    }

    fclose(fp);
    return 1;
}


/*
 * FILE: command.c
 *
 */

#include "xplot_ext.h"

#ifndef Solaris
extern int putenv(const char *string);
#endif


static int argerror = 0;

/* Forward declarations: */

char *LookupVariable(Graph * g, Plot * p, char *line);
void  Help(void);


char *
Ftoa(double f)
{
    char  tmp[100];
    char *copy;

    sprintf(tmp, "%g", f);
    copy = (char *)malloc(strlen(tmp) + 1);
    strcpy(copy, tmp);
    return copy;
}


float
Atof(char *s)
{
    if (s == NULL)
    {
        return 0;
    }

    return (atof(s));
}


int
Atoi(char *s)
{
    if (s == NULL)
    {
        return 0;
    }

    return (atoi(s));
}


char *
NextDelimiter(char *s, int *quote)
{
    if (s == NULL)
        return NULL;

    *quote = 0;

    for (; *s != '\0'; s++)
    {
        /*
         * look for a quote delimiter
         */

        if (*s == '"')
        {
            *quote = 1;
            break;
        }

        /*
         * look for a space delimiter ** which is the first white space
         * followed by non-white space
         */

        if ((*s == ' ' || *s == '\t') &&
            (*(s + 1) != ' ' && *(s + 1) != '\t' && *(s + 1) != '"'))
        {
            break;
        }
    }

    return s;
}


void
Setenv(char *var, char *value)
{
    char   *newenv;

    newenv = (char *)malloc(strlen(var) + strlen(value) + 4);
    strcpy(newenv, var);
    strcat(newenv, " = ");
    strcat(newenv, value);

    if (putenv(newenv) == 0)
    {
        fprintf(stderr, "%s = %s\n", var, value);
    }
    else
    {
        fprintf(stderr, "could not change %s\n", var);
    }
}


int
CommandArgumentCount(Graph *graph, Plot *plot, char *line)
{
    int i;
    int quote;

    /*
     * advance to the correct argument
     */

    for (i = 0;; i++)
    {
        /*
         * locate the open delimiter
         */

        line = NextDelimiter(line, &quote);

        if (line == NULL || *line == '\0')
        {
            break;
        }

        /*
         * advance past the open delimiter
         */

        line++;
    }

    return i;
}


/*
 * return an argument string delimited by white space or by quotes
 */

char *
CommandArgument(Graph *graph, Plot *plot, char *line, int argnumber)
{
    int   i;
    int   count;
    char *ptr;
    char *ptr2;
    char *copy;
    char  tmp[1000];
    int   quote;

    ptr = line;

    /*
     * advance to the correct argument
     */

    for (i = 0; i < argnumber; i++)
    {
        /*
         * locate the open delimiter
         */

        ptr = NextDelimiter(ptr, &quote);

        if (ptr == NULL)
        {
            return NULL;
        }

        /*
         * advance past the open delimiter
         */

        ptr++;
    }

    /*
     * copy the contents of the line up to the closing delimiter
     */

    for (ptr2 = ptr, count = 0; *ptr2 != '\0'; ptr2++, count++)
    {
        if (quote)
        {
            if (*ptr2 == '"')
                break;
        }
        else
        {
            if (*ptr2 == ' ' || *ptr2 == '\t' || *ptr2 == '\n')
                break;
        }

    }

    if (count > 0)
    {
        strncpy(tmp, ptr, count);
        tmp[count] = '\0';

#ifdef VARIABLES
        /*
         * check for variable notation
         */

        if (tmp[0] == '$')
        {
            ptr = LookupVariable(graph, plot, tmp + 1);

            if (ptr == NULL)
            {
                fprintf(stderr, "invalid variable '%s'\n", tmp);
                return NULL;
            }
        }
        else
        {
            ptr = tmp;
        }
#else
        ptr = tmp;
#endif
        copy = (char *)malloc((count + 1) * sizeof(char));

        strcpy(copy, ptr);
    }
    else
    {
        copy = NULL;
        Beep();
        fprintf(stderr, "missing argument #%d %s", argnumber, line);
        argerror = 1;
    }

    return copy;
}


#define Match(S) (strncmp(line, S, strlen(S)) == 0)
#define CP if (!p) return 0
#define CG if (!g) return 0
#define ARG(N)  CommandArgument(g, p, line, N)
#define ARGC    CommandArgumentCount(g, p, line)

/*
 * interpret / commands
 */

int
InterpretCommand(Graph *g, Plot **plot, char *line)
{
    Plot   *p;
    int     val;
    extern int verbose_legend;
    int     plotcount, plotselection;


    if (debug)
    {
        fprintf(stderr, "%s", line);
    }

    argerror = 0;
    p = *plot;

    if (Match("/showtitle"))
    {
        CG;
        g->show_title = Atoi(ARG(1));
    }
    else if (Match("/graphtitle"))
    {
        CG;
        g->title = ARG(1);
        RenameWindow(g->title);
    }
    else if (Match("/plotname"))
    {
        CP;
        p->title = ARG(1);
    }
    else if (Match("/select"))
    {
        CG;
        CP;
        SelectPlot(g, Atoi(ARG(1)));
        *plot = SelectedPlot(g);
    }
    else if (Match("/psres"))
    {
        CG;
        g->resolution = Atoi(ARG(1));
        return 1;
    }
    else if (Match("/psheader"))
    {
        SetHeader(Atoi(ARG(1)));
        return 1;
    }
    else if (Match("/psbox"))
    {
        SetBox(Atoi(ARG(1)));
        return 1;
    }
    else if (Match("/ticksize"))
    {
        CG;
        g->ticksize = Atoi(ARG(1));
    }
    else if (Match("/xtitle"))
    {
        CG;
        g->xaxis_title = ARG(1);
    }
    else if (Match("/ytitle"))
    {
        CG;
        g->yaxis_title = ARG(1);
    }
    else if (Match("/legends"))
    {
        CG;
        g->show_legend = Atoi(ARG(1));
    }
    else if (Match("/xlabels"))
    {
        CG;
        g->show_xlabels = Atoi(ARG(1));
    }
    else if (Match("/xtickinc"))
    {
        CG;
        g->xaxis_desired_tickinc = Atof(ARG(1));
    }
    else if (Match("/ytickinc"))
    {
        CG;
        g->yaxis_desired_tickinc = Atof(ARG(1));
    }
    else if (Match("/xsubticks"))
    {
        CG;
        g->xaxis_nsubticks = Atoi(ARG(1));
    }
    else if (Match("/ysubticks"))
    {
        CG;
        g->yaxis_nsubticks = Atoi(ARG(1));
    }
    else if (Match("/xprecision"))
    {
        CG;
        g->xaxis_rightdp = Atoi(ARG(1));
    }
    else if (Match("/yprecision"))
    {
        CG;
        g->xaxis_rightdp = Atoi(ARG(1));
    }
    else if (Match("/ylabels"))
    {
        CG;
        g->show_ylabels = Atoi(ARG(1));
    }
    else if (Match("/autox"))
    {
        CP;
        p->auto_xstart = Atof(ARG(1));
        p->auto_x = Atof(ARG(2));
    }
    else if (Match("/xlo"))
    {
        CP;
        p->xlo = Atoi(ARG(1));

        if (p->xlo < 0)
            p->xlo = 0;

        if (p->xlo >= p->npoints)
            p->xlo = p->npoints - 1;
    }
    else if (Match("/xhi"))
    {
        CP;
        p->xhi = Atoi(ARG(1));

        if (p->xhi >= p->npoints)
            p->xhi = p->npoints - 1;

        if (p->xhi < 0)
            p->xhi = 0;
    }
    else if (Match("/xdatarange"))
    {
        CG;

        if (ARGC < 1)
        {
            Beep();
            fprintf(stderr, "usage: xdatarange [auto][xmin xmax]\n");
            return 0;
        }

        if (strcmp(ARG(1), "auto") == 0)
        {
            g->autoscale_x = TRUE;
        }
        else
        {
            g->xmin = Atof(ARG(1));
            g->xmax = Atof(ARG(2));
            g->autoscale_x = FALSE;
        }

        RescaleGraph(g);
    }
    else if (Match("/ydatarange"))
    {
        CG;

        if (ARGC < 1)
        {
            Beep();
            fprintf(stderr, "usage: ydatarange [auto][ymin ymax]\n");
            return 0;
        }

        if (strcmp(ARG(1), "auto") == 0)
        {
            g->autoscale_y = TRUE;
        }
        else
        {
            g->ymin = Atof(ARG(1));
            g->ymax = Atof(ARG(2));
            g->autoscale_y = FALSE;
        }

        RescaleGraph(g);
    }
    else if (Match("/wxmin"))
    {
        CG;
        g->wxmin = Atof(ARG(1));
        SuppressWxmin();
        RescaleGraph(g);
    }
    else if (Match("/wxmax"))
    {
        CG;
        g->wxmax = Atof(ARG(1));
        SuppressWxmax();
        RescaleGraph(g);
    }
    else if (Match("/wymin"))
    {
        CG;
        g->wymin = Atof(ARG(1));
        SuppressWymin();
        RescaleGraph(g);
    }
    else if (Match("/wymax"))
    {
        CG;
        g->wymax = Atof(ARG(1));
        SuppressWymax();
        RescaleGraph(g);
    }
    else if (Match("/xrange"))
    {
        CG;
        g->wxmin = Atof(ARG(1));
        g->wxmax = Atof(ARG(2));
        SuppressWxmin();
        SuppressWxmax();
        RescaleGraph(g);
    }
    else if (Match("/yrange"))
    {
        CG;
        g->wymin = Atof(ARG(1));
        g->wymax = Atof(ARG(2));
        SuppressWymin();
        SuppressWymax();
        RescaleGraph(g);
    }
    else if (Match("/xaxisoffset_ns"))
    {
        CP;
        p->xaxisoffset = Atof(ARG(1));
    }
    else if (Match("/yaxisoffset_ns"))
    {
        CP;
        p->yaxisoffset = Atof(ARG(1));
    }
    else if (Match("/xaxisoffset"))
    {
        CP;
        p->xaxisoffset = Atof(ARG(1));
        RescaleGraph(g);
    }
    else if (Match("/yaxisoffset"))
    {
        CP;
        p->yaxisoffset = Atof(ARG(1));
        RescaleGraph(g);
    }
    else if (Match("/xoffset"))
    {
        CP;
        p->xoffset = Atof(ARG(1));
    }
    else if (Match("/yoffset"))
    {
        CP;
        p->yoffset = Atof(ARG(1));
    }
    else if (Match("/yintcpt"))
    {
        CG;
        g->xaxis_yintcpt = Atof(ARG(1));
    }
    else if (Match("/xintcpt"))
    {
        CG;
        g->yaxis_xintcpt = Atof(ARG(1));
    }
    else if (Match("/yscale"))
    {
        CP;
        p->scale = Atof(ARG(1));
        RescaleGraph(g);
    }
    else if (Match("/xscale"))
    {
        CP;
        p->xscale = Atof(ARG(1));
        RescaleGraph(g);
    }
    else if (Match("/escale"))
    {
        CP;
        p->escale = Atof(ARG(1));
    }
    else if (Match("/pointstyle"))
    {
        CP;
        CG;
        p->point_symbol = Atoi(ARG(1));
        p->showpoints = 1;
    }
    else if (Match("/longlegend"))
    {
        CP;
        verbose_legend = Atoi(ARG(1));
    }
    else if (Match("/showerror"))
    {
        CP;
        p->show_error = Atoi(ARG(1));
    }
    else if (Match("/showpoints"))
    {
        CP;
        p->showpoints = Atoi(ARG(1));
    }
    else if (Match("/pointfreq"))
    {
        CP;
        p->pointfreq = Atoi(ARG(1));

        if (p->pointfreq < 1)
            p->pointfreq = 1;
    }
    else if (Match("/showaxis"))
    {
        CG;
        val = Atoi(ARG(1));

        if (val > 0)
        {
            g->quadrants |= (1 << (val - 1));
        }

        if (val < 0)
        {
            g->quadrants &= ~(1 << (-val - 1));
        }
    }
    else if (Match("/setenv"))
    {
        Setenv(ARG(1), ARG(2));
        return 1;
    }
    else if (Match("/color"))
    {
        CP;
        p->linestyle = Atoi(ARG(1));
    }
    else if (Match("/xgrid"))
    {
        CG;
        g->show_xgrid = Atoi(ARG(1));
    }
    else if (Match("/ygrid"))
    {
        CG;
        g->show_ygrid = Atoi(ARG(1));
    }
    else if (Match("/gridcolor"))
    {
        CG;
        g->gridcolor = Atoi(ARG(1));
    }
    else if (Match("/xrastaxis"))
    {
        CP;
        p->xaxis = Atoi(ARG(1));
    }
    else if (Match("/yrastaxis"))
    {
        CP;
        p->yaxis = Atoi(ARG(1));
    }
    else if (Match("/shiftpercent"))
    {
        SetShiftPercent(Atof(ARG(1)));
    }
    else if (Match("/readfile"))
    {
        CG;
        LoadPlot(g, ARG(1));
        RescaleGraph(g);
    }
    else if (Match("/deleteplot"))
    {
        CP;
        DeletePlot(p);
    }
    else if (Match("/quit"))
    {
        Quit();
    }
    else if (Match("/bargraph"))
    {
        CP;
        p->plot_type = BAR_GRAPH;
    }
    else if (Match("/scatter"))
    {
        CP;
        p->plot_type = SCATTER_GRAPH;
    }
    else if (Match("/barwidth"))
    {
        CP;

        if (strcmp(ARG(1), "auto") == 0)
        {
            AutoBarWidth(p, Atof(ARG(2)));
        }
        else
        {
            p->bar_width = Atof(ARG(1));
        }

    }
    else if (Match("/barfilled"))
    {
        CP;
        p->bar_filled = 1;
    }
    else if (Match("/geometry"))
    {
        CG;
        ChangeGeometry(ARG(1), g->frame);
    }
    else if (Match("/refresh"))
    {
        CG;
        RefreshGraph(g);
    }
    else if (Match("/newplot"))
    {
        CP;
        CG;
        ClosePlot(p);
        *plot = AddPlot(g);
    }
    else if (Match("/pwd"))
    {
        CG;
        TextWindowLabel(getcwd(NULL, 200));
        return 1;
    }
    else if (Match("/closeplot"))
    {
        CP;
        ClosePlot(p);
    }
    else if (Match("/snapmode"))
    {
        SnapMode(Atoi(ARG(1)));
        return 1;
    }
    else if (Match("/printtofile"))
    {
        CG;
        PrintToFile(g, ARG(1), "w");
    }
    else if (Match("/global"))
    {
        SetGlobalOperation();
        return 1;
    }
    else if (Match("/save"))
    {
        CG;
        Save(g, ARG(1));
    }
    else if (Match("/savegraph"))
    {
        CG;
        SaveGraph(g, ARG(1));
    }
    else if (Match("/restoregraph"))
    {
        CG;
        RestoreGraph(g, ARG(1));
    }
    else if (Match("/appendtofile"))
    {
        CG;
        PrintToFile(g, ARG(1), "a");
    }
    else if (Match("/print"))
    {
        CG;
        PrintOut(g);
    }
    else if (Match("/wait"))
    {
        if ((val = sleep(Atoi(ARG(1)))) > 0)
        {
            fprintf(stderr, "%d unslept seconds\n", val);
        }
        return 1;
    }
    else if (Match("/autoorigin"))
    {
        CG;
        AutoOrigin(g);
        RescaleGraph(g);
    }
    else if (Match("/autoscale"))
    {
        CG;
        AutoScale(g, g->autoscale_x, g->autoscale_y);
        RescaleGraph(g);
    }
    else if (Match("/hideall"))
    {
        /*
         * Make all plots invisible.
         */

        for (p = g->plot; p; p = p->next)
        {
            p->visible = 0;
        }

        /*
         * RefreshGraph(g);
         */
    }
    else if (Match("/show"))
    {
        /*
         * make plot visible
         */
        plotcount = 0;
        plotselection = Atoi(ARG(1));

        for (p = g->plot; p; p = p->next)
        {
            if (plotcount == plotselection)
                p->visible = 1;
            plotcount++;
        }
    }
    else if (Match("/jump"))
    {
        jump = Atoi(ARG(1));
    }
    else if (Match("/init_select"))
    {
        init_select = Atoi(ARG(1));
    }
    else if (Match("/help"))
    {
        Help();
        return 1;
    }
    else
    {
        if (line[0] != '\0')
        {
            Beep();
            fprintf(stderr, "unrecognized command : %s\n", line);
        }
    }

    return 0;
}


static int helpcol;

void
HP(char *s)
{
    if ((helpcol++) % 4 == 0)
    {
        fprintf(stderr, "\n");
    }

    fprintf(stderr, "%-20s", s);
}


void
Help(void)
{
    fprintf(stderr, "AVAILABLE COMMANDS:\n");
    helpcol = 0;
    HP("global");
    HP("newplot");
    HP("autoorigin");
    HP("closeplot");
    HP("readfile [s]");
    HP("graphtitle [s]");
    HP("plotname [s]");
    HP("select [plotnum]");
    HP("showtitle [0/1]");
    HP("legends [0/1]");
    HP("psres [0/1]");
    HP("psbox [0/1]");
    HP("psheader [0/1]");
    HP("ticksize [i]");
    HP("xtitle [s]");
    HP("ytitle [s]");
    HP("xlabels [0/1]");
    HP("ylabels [0/1]");
    HP("xtickinc [f]");
    HP("ytickinc [f]");
    HP("xsubticks [i]");
    HP("ysubticks [i]");
    HP("xdatarange [f][f][auto]");
    HP("ydatarange [f][f][auto]");
    HP("xrange [f][f]");
    HP("yrange [f][f]");
    HP("wxmin [f]");
    HP("wxmax [f]");
    HP("wymin [f]");
    HP("wymax [f]");
    HP("xaxisoffset [f]");
    HP("yaxisoffset [f]");
    HP("xoffset [f]");
    HP("yoffset [f]");
    HP("xprecision [i]");
    HP("yprecision [i]");
    HP("xintcpt [f]");
    HP("yintcpt [f]");
    HP("xscale [f]");
    HP("yscale [f]");
    HP("escale [f]");
    HP("xrastaxis [0/1]");
    HP("yrastaxis [0/1]");
    HP("xlo [i]");
    HP("xhi [i]");
    HP("pointstyle [i]");
    HP("showpoints [0/1]");
    HP("showerror [0/1]");
    HP("showaxis [-4..4]");
    HP("color [0..255]");
    HP("xgrid [0/1]");
    HP("ygrid [0/1]");
    HP("gridcolor [0..255]");
    HP("shiftpercent [0..1]");
    HP("deleteplot");
    HP("scatter");
    HP("bargraph");
    HP("barwidth [f][auto percent]");
    HP("barfilled");
    HP("snapmode [0/1]");
    HP("printtofile [file]");
    HP("appendtofile [file]");
    HP("print");
    HP("savegraph [file]");
    HP("restoregraph [file]");
    HP("setenv [var value]");
    HP("geometry [str]");
    HP("longlegend [0/1]");
    HP("refresh");
    HP("pwd");
    HP("wait");
    HP("quit");
    fprintf(stderr, "\n");
}


char *
LookupVariable(Graph *g, Plot *p, char *line)
{
    if (Match("wxmin"))
    {
        CP;
        CG;
        return (Ftoa(g->wxmin));
    }
    else if (Match("wxmax"))
    {
        CP;
        CG;
        return (Ftoa(g->wxmax));
    }
    else if (Match("wymin"))
    {
        CP;
        CG;
        return (Ftoa(g->wymin));
    }
    else if (Match("wymax"))
    {
        CP;
        CG;
        return (Ftoa(g->wymax));
    }
    else
    {
        return NULL;
    }
}

/*
 * FILE: color.c
 *
 */

/*
 * color routines swiped from Walter Yamada 3/1/88
 * (Thanks Walter!)
 */

#include "xplot_ext.h"

#define WI 63.0

#define arc(x)  (255.0*sqrt(1.0-(((x)-WI)/WI)*(((x)-WI)/WI)))
#define carc(x) (((0<=(x))&&((x)<= 2*WI))?arc(x):0)
#define MAX_COLORS 256

XColor  color[MAX_COLORS];
int     pixel[MAX_COLORS];

int
MakeColormap(Graph *G)
{
    int         planes[30];         /* planes altered by XGCC call */
    int         rval[MAX_COLORS];   /* color allocation bit maps   */
    int         gval[MAX_COLORS];
    int         bval[MAX_COLORS];
    int         index;
    int         ncolors;
    Colormap    cmap;
    Window      root_window;

    /*
     * define rgb values
     */

    for (ncolors = 0; ncolors < MAX_COLORS - 63; ncolors++)
    {
        bval[ncolors] = (int)carc((float)ncolors);
        gval[ncolors] = (int)carc(ncolors - 63.0);
        rval[ncolors] = (int)carc(ncolors - 126.0);
    }

    root_window = XRootWindow(G->display, G->screen_number);

    /*
     * get color map
     */

    cmap = XCreateColormap(G->display, G->window, G->visual, AllocAll);

    /*
     * allocate colors
     */

    if (XAllocColorCells(G->display, cmap, (Bool) 1,
                         (unsigned long *)planes, 8,
                         (unsigned long *)pixel, MAX_COLORS) == 0)
    {
        fprintf(stderr, "could not allocate color cells\n");
        return 0;
    }

    /*
     * define colors
     */

    for (index = 0; index < ncolors; index++)
    {
        color[index].pixel  = pixel[index];
        color[index].red    = rval[index] << 8;
        color[index].green  = gval[index] << 8;
        color[index].blue   = bval[index] << 8;
        color[index].pixel  = ncolors;
        color[index].flags  = DoRed | DoGreen | DoBlue;
    }

    XStoreColors(G->display, cmap, color, ncolors);

    /*
     * may not need this
     * XInstallColormap(G->display, cmap);
     */

    return ncolors;
}

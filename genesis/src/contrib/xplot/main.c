/*
 * FILE: main.c
 *
 */


/*
 * Xplot - program to graph sets of xy ascii formatted data
 *      under the X11 windowing system
 *
 *           Copyright 1989 Matt Wilson
 *           California Institute of Technology
 *           wilson@smaug.cns.caltech.edu
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation, and that the name of the California Institute
 * of Technology not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  Neither the author nor California Institute of Technology
 * make any representations about the suitability of this software for
 * any purpose. It is provided "as is" without express or implied warranty.
 *
 * This version HEAVILY hacked by Mike Vanier.
 *
 */

#include "xplot_ext.h"

int    debug;
Graph *G;
Frame *F;
int    init_select;


int
main(int argc, char **argv)
{
    TextWindow *T;

    F = (Frame *)calloc(1, sizeof(Frame));
    F->mapped = 1;

    G = (Graph *)calloc(1, sizeof(Graph));
    G->frame = F;
    G->mapped = 1;

    T = (TextWindow *)calloc(1, sizeof(TextWindow));
    T->frame = F;
    T->mapped = 1;

    F->graph = G;
    F->text = T;

    init_select = 0;  /* default: initial selected plot;
                       * can be overridden by command */
    ParseArgList(F, argc, argv);
    InitX(F);
    LoadGraphData(G);
    InitGraph(G);
    RescaleFrame(F);
    EventLoop(F);

    return 0;
}

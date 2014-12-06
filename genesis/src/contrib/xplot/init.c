/*
 * FILE: init.c
 *
 */

#include "xplot_ext.h"
#include <X11/cursorfont.h>
#include <X11/Xutil.h>

XContext datacontext;


void
NormalSelectInputs(Graph *graph)
{
    XSelectInput(graph->display, graph->window,
                 KeyPressMask
                 | ExposureMask
                 | ButtonPressMask
                 | ButtonReleaseMask
                 | Button1MotionMask
                 | Button2MotionMask
                 | Button3MotionMask);
}


void
TextSelectInputs(Graph *graph)
{
    XSelectInput(graph->display, graph->window,
                 KeyPressMask | ExposureMask | PointerMotionMask);
}


void
ClickSelectInputs(Graph *graph)
{
    XSelectInput(graph->display, graph->window,
                 KeyPressMask | ExposureMask | PointerMotionMask |
                 ButtonPressMask | ButtonReleaseMask);
}


void
ChangeGeometry(char *geometry, Frame *frame)
{
    int        status, x, y, width, height;
    XSizeHints hint;

    hint.flags = 0;
    status = XParseGeometry(geometry, &x, &y,
                            (unsigned *)&width, (unsigned *)&height);

    if (status & XValue)
    {
        frame->x = x;
        hint.flags |= PPosition;
        hint.x = x;
    }

    if (status & YValue)
    {
        frame->y = y;
        hint.flags |= PPosition;
        hint.y = y;
    }

    if (status & WidthValue)
    {
        frame->width = width;
        hint.flags |= PSize;
        hint.width = width;
    }

    if (status & HeightValue)
    {
        frame->height = height;
        hint.flags |= PSize;
        hint.height = height;
    }

    XMoveResizeWindow(frame->display, frame->window, frame->x, frame->y,
                      frame->width, frame->height);
}

void
SetInverse(BasicWindow *basic)
{
    if (basic->inverse)
    {
        SetPSInverse(0);
        SetBackground(basic,
                      XWhitePixel(basic->display, basic->screen_number));
        SetForeground(basic,
                      XBlackPixel(basic->display, basic->screen_number));
    }
    else
    {
        SetPSInverse(1);
        SetForeground(basic,
                      XWhitePixel(basic->display, basic->screen_number));
        SetBackground(basic,
                      XBlackPixel(basic->display, basic->screen_number));
    }
}

void
InitGraph(Graph *graph)
{
    Plot   *plot;
    extern char *restorefile;

    /*
     * make sure there are plots. If not then exit
     */

    if (!graph->plot)
    {
        Quit();
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
    }

    if (graph->plot)
    {
        /*
         * select the first (or specified) plot
         */
        SelectPlot(graph, init_select);
        /* graph->plot->selected = 1; */
    }

    if (!restorefile)
    {
        /*
         * determine the world coordinate boundaries of the window and
         * autoscaling the data if necessary
         */
        AutoScale(graph, graph->autoscale_x, graph->autoscale_y);
    }
}


void
CopyWindowStruct(BasicWindow *src, BasicWindow *dst)
{
    bcopy(src, dst, sizeof(BasicWindow));
    dst->context = XCreateGC(dst->display, dst->window, 0, 0);
    XCopyGC(src->display, src->context, ~0, dst->context);
}


void
MapWindows(Frame *frame)
{
    if (frame->mapped)
    {
        XMapWindow(frame->display, frame->window);
    }

    if (frame->graph->mapped)
    {
        XMapWindow(frame->graph->display, frame->graph->window);
    }

    if (frame->text->mapped)
    {
        XMapWindow(frame->text->display, frame->text->window);
    }
}


void
InitX(Frame *frame)
{
    char *str;
    char *XGetDefault(Display *, const char *, const char *);
    char *CopyString(char *s);

    /*
     * open the display
     */

    frame->display = XOpenDisplay(frame->display_name);

    if (frame->display == NULL)
    {
        fprintf(stderr, "Unable to open X display [%s]\n",
                frame->display_name);
        exit(1);
    }

#ifdef DEBUG
    /*
     * force all calls to complete before returning
     * this synchronizes errors with the routines that invoked them
     */

    XSynchronize(frame->display, 1);
#endif

    if (frame->fontname == NULL)
    {
        str = XGetDefault(frame->display, "xplot", "Font");
        str = NULL;

        if (str == NULL)
        {
            frame->fontname = "10x20";  /* MCV change: was "fixed"
                                         * which is WAY too small. */
        }
        else
        {
            frame->fontname = CopyString(str);
        }
    }

    if (frame->inverse == -1)
    {
        str = XGetDefault(frame->display, "xplot", "Inverse");

        if (str == NULL)
        {
            frame->inverse = 0;
        }
        else
        {
            if (strcmp(str, "yes") == 0)
            {
                frame->inverse = 1;
            }
            else
            {
                frame->inverse = 0;
            }
        }
    }

    /*
     * use the default screen
     */

    frame->screen_number = XDefaultScreen(frame->display);

    /*
     * use the default visual
     */

    frame->visual = XDefaultVisual(frame->display, frame->screen_number);

    /*
     * use the default context
     */

    frame->context = XDefaultGC(frame->display, frame->screen_number);

    /*
     * determine whether or not color can be displayed
     */

    if (XDisplayPlanes(frame->display, frame->screen_number) < 2)
    {
        frame->color_mode = 0;
    }

    /*
     * create the colormap
     if(frame->color_mode){
     MakeColormap(frame);
     }
    */

    /*
     * set the foreground and background
     */

    SetInverse((BasicWindow *)frame);
    SetFont((BasicWindow *)frame, frame->fontname);

    /*
     * create the windows
     */

    CreateWindows(frame);
    NormalCursor(frame);

    /*
     * make sure it all got to the server
     */

    XFlush(frame->display);
}


void
TargetCursor(Frame *frame)
{
    Cursor  cursor;

    /*
     * select the cursor
     */

    cursor = XCreateFontCursor(frame->display, XC_target);
    XDefineCursor(frame->display, frame->window, cursor);
}


void
NormalCursor(Frame *frame)
{
    Cursor  cursor;

    /*
     * select the cursor
     */

    cursor = XCreateFontCursor(frame->display, XC_left_ptr);
    XDefineCursor(frame->display, frame->window, cursor);
}


void
TextCursor(Frame *frame)
{
    Cursor  cursor;

    /*
     * select the cursor
     */

    cursor = XCreateFontCursor(frame->display, XC_pencil);
    XDefineCursor(frame->display, frame->window, cursor);
}


void
HandCursor(Frame *frame)
{
    Cursor  cursor;

    /*
     * select the cursor
     */

    cursor = XCreateFontCursor(frame->display, XC_hand2);
    XDefineCursor(frame->display, frame->window, cursor);
}


void
CrosshairCursor(Frame *frame)
{
    Cursor  cursor;

    /*
     * select the cursor
     */

    cursor = XCreateFontCursor(frame->display, XC_crosshair);
    XDefineCursor(frame->display, frame->window, cursor);
}


void
RenameWindow(char *s)
{
    XSizeHints hints;

    if (s == NULL)
        return;

    hints.flags = 0;

    XSetStandardProperties(F->display,
                           F->window,
                           s,       /* window name         */
                           s,       /* icon name           */
                           None,    /* icon pixmap         */
                           NULL,    /* command argv        */
                           0,       /* command argc        */
                           &hints   /* window sizing hints */
        );
}


void
CreateWindows(Frame *frame)
{
    int     width;
    int     height;
    int     x;
    int     y;
    unsigned long border;
    unsigned long background;
    XWindowAttributes winfo;
    XSetWindowAttributes attrib;
    Window  root_window;
    int     borderwidth;
    XSizeHints hints;
    WindowData *windowdata;
    Graph  *graph;
    TextWindow *text;
    int     status = 0;
    char   *windowtitle;

    graph = frame->graph;
    text  = frame->text;

    /*
     * get the root window and its attributes
     */

    root_window = XRootWindow(frame->display, frame->screen_number);
    XGetWindowAttributes(frame->display, root_window, &winfo);

    if (frame->x == -1)
    {
        frame->x = winfo.width / 20;
        status = 1;
    }

    if (frame->y == -1)
    {
        frame->y = winfo.height / 20;
        status = 1;
    }

    if (frame->width == -1)
    {
        frame->width = winfo.width / 4;
        status = 1;
    }

    if (frame->height == -1)
    {
        frame->height = winfo.height / 4;
        status = 1;
    }

    height      = frame->height;
    width       = frame->width;
    x           = frame->x;
    y           = frame->y;
    borderwidth = 1;
    border      = frame->foreground;
    background  = frame->background;

    /*
     * TOP-LEVEL FRAME WINDOW
     */

    windowdata         = (WindowData *)calloc(1, sizeof(WindowData));
    windowdata->id     = FRAME_WINDOW;
    windowdata->parent = (BasicWindow *)frame;
    windowdata->func   = (PFI)frame_event;
    frame->windowname  = "FRAME";
    frame->window      = (Window) XCreateSimpleWindow(frame->display,
                                                      root_window,
                                                      x, y, width, height, 1,
                                                      border, background);
    XSaveContext(frame->display, frame->window,
                 datacontext, (XPointer)windowdata);
    frame->drawable = frame->window;
    XSelectInput(frame->display, frame->window, StructureNotifyMask);

    /*
     * set the standard properties of the window
     */

    if (!status)
    {
        hints.flags = PSize | PPosition | USSize | USPosition;
    }
    else
    {
        hints.flags = PSize;
    }

    hints.x      = x;
    hints.y      = y;
    hints.height = height;
    hints.width  = width;

    if (graph->title)
    {
        windowtitle = graph->title;
    }
    else
    {
        windowtitle = "XPLOT";
    }

    XSetStandardProperties(frame->display,
                           frame->window,
                           windowtitle,     /* window name         */
                           windowtitle,     /* icon name           */
                           None,            /* icon pixmap         */
                           NULL,            /* command argv        */
                           0,               /* command argc        */
                           &hints           /* window sizing hints */
        );

    /*
     * note that the order in which the subwindows are created
     * determines the stacking order. This has an effect on the
     * way exposure events are generated during resizing
     */

    /*
     * GRAPH WINDOW
     */

    CopyWindowStruct((BasicWindow *)frame, (BasicWindow *)graph);
    graph->windowname  = "GRAPH";
    windowdata         = (WindowData *)calloc(1, sizeof(WindowData));
    windowdata->id     = GRAPH_WINDOW;
    windowdata->parent = (BasicWindow *)graph;
    windowdata->func   = (PFI)graph_event;
    graph->window      =
        (Window)XCreateSimpleWindow(graph->display, frame->window, 0, 0,
                                    width, height, borderwidth, border,
                                    background);
    XSaveContext(graph->display, graph->window,
                 datacontext, (XPointer)windowdata);
    graph->drawable = graph->window;
    attrib.backing_store = Always;
    XChangeWindowAttributes(graph->display, graph->window,
                            CWBackingStore, &attrib);
    graph->truncate_tick_values = 1;

    /*
     * Request that the window be notified of certain events.
     */

    XSelectInput(graph->display,
                 graph->window,
                 KeyPressMask
                 | ExposureMask
                 | ButtonPressMask
                 | ButtonReleaseMask
                 | Button1MotionMask
                 | Button2MotionMask
                 | Button3MotionMask);

    /*
     * TEXT WINDOW
     */

    CopyWindowStruct((BasicWindow *)frame, (BasicWindow *)text);
    text->windowname   = "TEXT";
    windowdata         = (WindowData *)calloc(1, sizeof(WindowData));
    windowdata->id     = TEXT_WINDOW;
    windowdata->parent = (BasicWindow *)text;
    windowdata->func   = (PFI)text_event;
    text->window       = (Window) XCreateSimpleWindow(text->display,
                                                      frame->window,
                                                      0, 0,
                                                      width, height,
                                                      borderwidth, border,
                                                      background);
    XSaveContext(text->display, text->window,
                 datacontext, (XPointer)windowdata);
    text->drawable = text->window;
    XSelectInput(text->display, text->window, ExposureMask);
}


void
Quit(void)
{
    XDestroyWindow(F->display, F->window);
    XCloseDisplay(F->display);
    exit(0);
}

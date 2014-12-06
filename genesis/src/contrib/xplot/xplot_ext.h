/*
 * FILE: xplot_ext.h
 *
 */

#ifndef XPLOT_EXT_H
#define XPLOT_EXT_H

#include     <stdio.h>
#include     <math.h>
#include     <stdlib.h>
#include     <string.h>
#include     <unistd.h>

/*
 * This is a totally bogus hack; there are incompatibilities between
 * glibc and Xlib wrt wide char types that can only be hacked around
 * this way:
 */
#define __EMX__

#include     <X11/Xlib.h>
#include     <X11/X.h>
#include     <X11/Xutil.h>
#include     "header.h"
#include     "xplot_defs.h"
#include     "xplot_struct.h"

extern int debug;
extern Graph *G;
extern Frame *F;
extern XContext datacontext;
extern int jump;
extern int init_select;

/* FIXME: alphabetize these: */

extern Plot    *AddPlot(Graph *graph);
extern void     AutoOrigin(Graph *graph);
extern void     AutoScale(Graph *graph, int autoscale_x, int autoscale_y);
extern void     AutoBarWidth(Plot *plot, float percent);
extern void     AutoOrigin(Graph *graph);
extern void     AutoScale(Graph *graph, int autoscale_x, int autoscale_y);
extern void     Beep(void);
extern void     ChangeGeometry(char *geometry, Frame *frame);
extern void     ClosePlot(Plot *plot);
extern char    *CopyString(char *s);
extern void     DeletePlot(Plot *plot);
extern Label   *FindLabel(BasicWindow *basic, short int cx, short int cy);
extern int      InterpretCommand(Graph *g, Plot **plot, char *line);
extern void     LeftShift(void);
extern void     LoadPlot(Graph *graph, char *filename);
extern void     PrintOut(Graph *graph);
extern void     PrintToFile(Graph *graph, char *fname, char *mode);
extern void     Quit(void);
extern void     RefreshGraph(Graph *graph);
extern void     RenameWindow(char *s);
extern void     RescaleGraph(Graph *graph);
extern int      RestoreGraph(Graph *graph,char *file);
extern void     RightShift(void);
extern int      SaveGraph(Graph *graph, char *file);
extern void     ScreenTransform(Graph *graph,
                                float wx, float wy, int *sx, int *sy);
extern void     SelectPlot(Graph *graph, int ival);
extern Plot    *SelectedPlot(Graph *graph);
extern void     SetBox(int mode);
extern void     SetGlobalOperation(void);
extern void     SetHeader(int mode);
extern void     SetShiftPercent(float val);
extern void     SnapMode(int mode);
extern void     SuppressWxmax(void);
extern void     SuppressWxmin(void);
extern void     SuppressWymax(void);
extern void     SuppressWymin(void);
extern void     TextWindowLabel(char *label);
extern void     TogglePinchmode(void);
extern void     ToggleDrawmode(void);
extern void     ToggleZapmode(void);

extern void AddLabelBox(BasicWindow *basic, short int sx, short int sy,
                        short int sr, float wx, float wy, float wr,
                        short int coord_mode, short int priority);
extern void AddLabelLine(BasicWindow *basic, short int sx, short int sy,
                         short int sy2, short int sx2,
                         float wx, float wy, float wx2, float wy2,
                         short int coord_mode, short int priority);
extern void AddLabelString(BasicWindow *basic, char *s,
                           short int sx, short int sy,
                           float wx, float wy,
                           short int coord_mode, short int priority);
extern void Box(int xl, int yb, int xr, int yt);
extern int  ButtonMode(void);
extern void ButtonPressAction(Graph *graph,
                              XButtonPressedEvent *buttonevent);
extern void ButtonReleaseAction(Graph *graph,
                                XButtonReleasedEvent *buttonevent);
extern void CalculateAxisParameters(Graph *graph);
extern void CalculateXTicks(Graph *graph);
extern void CalculateYTicks(Graph *graph);
extern void Circle(int cx, int cy, int d);
extern void ClearWindow(BasicWindow *basic);
extern void ClickSelectInputs(Graph *graph);
extern int  CommandSource(void);
extern void CopyWindowStruct(BasicWindow *src, BasicWindow *dst);
extern void CreateWindows(Frame *frame);
extern void CrosshairCursor(Frame *frame);
extern void DeleteLabel(BasicWindow *basic, Label *label);
extern void DisplayLabels(BasicWindow *basic);
extern void DoCommand(Graph *graph, Plot **plot, char *lineptr);
extern void DoFileCommands(Graph *graph, Plot **plot, char *filename);
extern void DrawAxes(Graph *graph);
extern void DrawDot(int x1, int y1);
extern void DrawGraph(Graph *graph);
extern void DrawGraphTitle(Graph *graph);
extern void DrawLine(int x1, int y1, int x2, int y2);
extern void DrawMode(BasicWindow *basic, int mode);
extern void DrawPlot(Plot *plot, int count);
extern void DrawPlotLegend(Plot *plot, int count);
extern void DrawPointIcon(Plot *plot, int x, int y);
extern void DrawSuperBox(BasicWindow *basic,
                         int sx1, int sy1, int sx2, int sy2);
extern void DrawSuperLine(BasicWindow *basic,
                          int sx1, int sy1, int sx2, int sy2);
extern void DrawXAxisLabel(Graph *graph);
extern void DrawXTicks(Graph *graph);
extern void DrawYAxisLabel(Graph *graph);
extern void DrawYTicks(Graph *graph);
extern void EraseSuperBox(BasicWindow *basic,
                          int sx1, int sy1, int sx2, int sy2);
extern void EraseSuperLine(BasicWindow *basic,
                           int sx1, int sy1, int sx2, int sy2);
extern void EventLoop(Frame *frame);
extern void FilledBox(int xl, int yb, int xr, int yt);
extern void FilledCircle(int cx, int cy, int d);
extern void FilledTriangle(int cx, int cy, int d);
extern void FinishPS(void);
extern void FloatMultipleLines(Coord *coord, int ncoords, FCoord *fcoord);
extern void FreeAllLabels(BasicWindow *basic);
extern void FreeLabels(BasicWindow *basic);
extern void GetDataPoint(Plot *plot, FCoord *data, float *wx, float *wy);
extern int  GlobalOperation(void);
extern void HandCursor(Frame *frame);
extern void Help(void);
extern void InitGraph(Graph *graph);
extern void InitX(Frame *frame);
extern void Invert(BasicWindow *basic);
extern void KeyAction(Graph *graph, XKeyEvent *event);
extern void LoadGraphData(Graph *graph);
extern void LoadPlotFromSource(Graph *graph, DataSource *source);
extern void MapWindows(Frame *frame);
extern void MultipleLines(Coord *coord, int ncoords);
extern void Normal(BasicWindow *basic);
extern void NormalCursor(Frame *frame);
extern void NormalSelectInputs(Graph *graph);
extern void OffsetPlotGraphically(Graph *graph, int cx, int cy, float val);
extern void PSSetLineWidth(int width);
extern void ParseArgList(Frame *frame, int argc, char **argv);
extern void PointerMotionAction(Graph *graph,
                                XPointerMovedEvent *motionevent);
extern void PreparePS(Display *display, Window window,
                      float scale, int box, int header);
extern void RefreshText(TextWindow *text);
extern int  RescaleFrame(Frame *frame);
extern void ResetGlobalOperation(void);
extern int  Save(Graph *graph, char *file);
extern void ScaleAndRefreshGraph(Graph *graph);
extern void SetAxisMode(int mode);
extern void SetBackground(BasicWindow *basic, int pixel);
extern void SetButtonMode(int mode);
extern void SetColor(int pixel);
extern void SetFont(BasicWindow *frame, char *name);
extern void SetForeground(BasicWindow *basic, int pixel);
extern void SetGlobalOperation(void);
extern void SetInverse(BasicWindow *basic);
extern void SetLineWidth(int width);
extern void SetLinestyle(BasicWindow *basic, int style);
extern void SetPSFileOutput(int state);
extern void SetPSFilemode(char *mode);
extern void SetPSFilename(char *name);
extern void SetPSInverse(int state);
extern void SetRestoreFile(char *name);
extern void TargetCursor(Frame *frame);
extern void Text(BasicWindow *basic, int x, int y, char *s);
extern void TextCursor(Frame *frame);
extern void TextExtent(char *s, int *width, int *height);
extern void TextSelectInputs(Graph *graph);
extern void TextWindowLabel(char *label);
extern void ToggleSnapMode(void);
extern void Triangle(int cx, int cy, int d);
extern void WorldTransform(Graph *graph, int sx, int sy,
                           float *wx, float *wy);
extern void XPSDrawDot(Display *display, Drawable drawable, GC context,
                       int x1, int y1);
extern void XPSDrawLine(Display * display, Drawable drawable, GC context,
            int x1, int y1, int x2, int y2);
extern void XPSDrawLines(Display *display, Drawable drawable,
                         GC context, Coord *coord, int ncoords, int mode);
extern void XPSDrawLinesFloat(Display *display, Drawable drawable, GC context,
                              Coord *coord, int ncoords, int mode,
                              FCoord *fcoord);
extern void XPSDrawRectangle(Display *display, Drawable drawable,
                             GC context, int x, int y, int w, int h);
extern void XPSDrawText(Display *display, Drawable drawable, GC context,
                        int x, int y, char *s);
extern void XPSFillPolygon(Display *display, Drawable drawable, GC context,
                           Coord *coord, int ncoords, int shape, int mode);
extern void XPSFillRectangle(Display * display, Drawable drawable,
                             GC context, int x, int y, int w, int h);
extern void ZapExpose(Display *display);
extern void ZapLabel(Graph *graph, XKeyEvent *event);
extern void _Box(BasicWindow *basic, int xl, int yb, int xr, int yt);
extern void _DrawLine(BasicWindow *b, int x1, int y1, int x2, int y2);
extern void _FilledBox(BasicWindow *basic, int xl, int yb, int xr, int yt);
extern void _SetColor(BasicWindow *basic, int pixel);
extern void frame_event(Frame *frame, XEvent *event);
extern void graph_event(Graph *graph, XEvent *event);
extern void text_event(TextWindow *text, XEvent *event);

#endif  /* XPLOT_EXT_H */


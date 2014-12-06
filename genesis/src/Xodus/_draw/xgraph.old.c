#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Draw/CoreDraw.h"
#include "Draw/Graph.h"
#include "Draw/Pix.h"
#include "Draw/Shape.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"

static Gen2Xo GXconvert[] = {
	{"xmin",		XtNgxmin},
	{"xmax",		XtNgxmax},
	{"ymin",		XtNgymin},
	{"ymax",		XtNgymax},
	{"xoffset",		XtNxoffset},
	{"yoffset",		XtNyoffset},
	{"title",		XtNtitle},
	{"xlabel",		XtNxlabel},
	{"xunits",		XtNxunits},
	{"XUnits",		XtNxunits},
	{"ylabel",		XtNylabel},
	{"yunits",		XtNyunits},
	{"YUnits",		XtNyunits},
};


#ifndef MAX_NTARGS
#define MAX_NTARGS 20
static int xoZapOverlays();
static int xoCountPlots();

XGraph (graph, action)
     register struct xgraph_type *graph;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW,xoFindParentForm();
	FramedWidget fW;
	int ntargs,i;
	char	*targs[MAX_NTARGS],**ptargs;
	MsgIn	*msg;
	Element	*child;
	Action	tempaction;
	struct xplot_type	*pchild;
	int	update_graph_offsets = 0;
	char	temp1[80],temp2[80];
	int		nplots = 0;
  
  if (Debug(0) > 1)
    ActionHeader("XGraph", graph, action);

  if (action->type & graph->soft_action_mask)
	  if (SoftAction(graph,action,BEFORE_ACTIONS)) return;

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
	/* Handle the childs action for it. Another hack to tide over till
	** the grouping object code works. */ 
	for(child=graph->child;child;child=child->next) {
		if (child->msg_in && strcmp(child->object->name,"xplot") == 0
			&& child->msg_in->type == 0) { /* PLOT msg type */
			XPlot(child,action);
		}
	}
    break;
  case RESET:
	/* This is a nasty hack till we can handle SENDMSG actions
	** directly. The ideas is to take all arriving msgs and make
	** separate plots for each, redirecting the msg to the plot.
	*/
	if (graph->overlay) {
		xoHandleOverlays(graph);
	} else { /* otherwise clear the graph of old plots. If there
			 ** were any old plots we will need to update offsets */
		update_graph_offsets = xoZapOverlays(graph);
		/* xoRedraw(graph); */
	}

	/* Count the existing plots */
	nplots = xoCountPlots(graph);

	/* move the messages onto the plots */
	for(msg=graph->msg_in,i=nplots;msg;msg=msg->next,i++) {
		xoMakeNewPlot(graph,msg,i);
		update_graph_offsets = 1;
	}
	/* and remove them from the parent graph */
	for(i = 0,child=graph->child;child;child=child->next) {
		if (strcmp(child->object->name,"xplot") != 0) continue;
		if (child->msg_in == NULL) /* if no incoming msgs, skip */ 
			continue;
		if (child->msg_in->type != 0) /* if it isnt a PLOT msg, skip */
			continue;
	/* get rid of next pointers */
		if (child->msg_in && child->msg_in->next) {
				child->msg_in->next = NULL; /* only 1 plot allowed */
		}
	/* Call childs RESET action - this deals with the fg etc */
		tempaction.argc = 1;
		tempaction.argv = targs;
		tempaction.type = RESET;
		tempaction.name = "RESET";
		tempaction.data = (char *)NULL;
		XPlot(child,&tempaction);
	}
	graph->msg_in = NULL;

	if (update_graph_offsets)
		XoUpdatePlotAxes((GraphWidget)graph->widget,
			graph->xoffset != 0.0,
			graph->yoffset != 0.0);
	if (!graph->overlay)
		xoRedraw(graph);
    break;
  case ADDMSG:
	/* move the relevant msg (which is passed in action->data)
	** onto the a new plot */
	for(msg=graph->msg_in;msg;msg=msg->next,i++) {
		if (msg == (MsgIn *)action->data) {
			xoMakeNewPlot(graph,msg,i);
			update_graph_offsets = 1;
		}
	}
	/* and remove them from the parent graph */
	for(i = 0,child=graph->child;child;child=child->next) {
		if (strcmp(child->object->name,"xplot") != 0) continue;
		if (child->msg_in == NULL) /* if no incoming msgs, skip */ 
			continue;
		if (child->msg_in->type != 0) /* if it isnt a PLOT msg, skip */
			continue;
	/* get rid of next pointers */
		if (child->msg_in && child->msg_in->next) {
				child->msg_in->next = NULL; /* only 1 plot allowed */
		}
	/* Call childs RESET action - this deals with the fg etc */
		tempaction.argc = 1;
		tempaction.argv = targs;
		tempaction.type = RESET;
		tempaction.name = "RESET";
		tempaction.data = (char *)NULL;
		XPlot(child,&tempaction);
	}
	graph->msg_in = NULL;

	if (update_graph_offsets)
		XoUpdatePlotAxes((GraphWidget)graph->widget,
			graph->xoffset != 0.0,
			graph->yoffset != 0.0);
	if (!graph->overlay)
		xoRedraw(graph);
	break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */
    
    if ((parentW = xoFindParentForm(graph)) == NULL) return(0);
	fW = (FramedWidget)XtVaCreateManagedWidget(
		av[1], framedWidgetClass, parentW,
					    XtNchildClass, graphWidgetClass,
					    XtNmappedWhenManaged, False,
					    NULL);
    graph->widget = (char *)XoGetFramedChild(fW);
	graph->xaxis_w = (char *)XtNameToWidget(XoGetFramedChild(fW),
		"xaxis");
	graph->yaxis_w = (char *)XtNameToWidget(XoGetFramedChild(fW),
		"yaxis");
	XtAddCallback(graph->widget,XtNcallback,xoCallbackFn,
		(XtPointer)graph);
    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(graph,ac,av);
	XtMapWidget(fW);
	return(1);
	break;
  case SET:
    if (ac) {			/* need to set fields */
		if (strcmp(av[0],"xunits") == 0) {
			DirectSetElement(graph,"XUnits",av[1]);
			gx_convert(graph,GXconvert,XtNumber(GXconvert),ac,av);
			return(1);
		}
		if (strcmp(av[0],"yunits") == 0) {
			DirectSetElement(graph,"YUnits",av[1]);
			gx_convert(graph,GXconvert,XtNumber(GXconvert),ac,av);
			return(1);
		}
		if (strcmp(av[0],"range") == 0) {
			float xmin,ymin,xmax,ymax;

			if (sscanf(av[1],"[%f,%f,%f,%f]",&xmin,&ymin,&xmax,&ymax)
				== 4) { /* The conversion worked */
				char	*tav[2];
				char	value[20];

				graph->xmin = xmin;
				graph->ymin = ymin;
				graph->xmax = xmax;
				graph->ymax = ymax;
				tav[1] = value;

				tav[0] = "xmin"; sprintf(value,"%g",xmin);
				gx_convert(graph,GXconvert,XtNumber(GXconvert),2,tav);
				tav[0] = "ymin"; sprintf(value,"%g",ymin);
				gx_convert(graph,GXconvert,XtNumber(GXconvert),2,tav);
				tav[0] = "xmax"; sprintf(value,"%g",xmax);
				gx_convert(graph,GXconvert,XtNumber(GXconvert),2,tav);
				tav[0] = "ymax"; sprintf(value,"%g",ymax);
				gx_convert(graph,GXconvert,XtNumber(GXconvert),2,tav);
			}
			return(1);
		}
		gx_convert(graph,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
	XtDestroyWidget((Widget)graph->widget);
	break;
  case ADDMSG:
	break;
  case DELETEMSG:
	break;
  case MSGACTION:
	break;
  case XUPDATE : /* update graph fields due to changes in widget */
		xg_convert(graph,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
	xoExecuteFunction(graph,action,graph->script,"");
	break;
  }
   if (action->type & graph->soft_action_mask)
	   if (SoftAction(graph,action,AFTER_ACTIONS)) return;

}

/* This function moves old plots to the xoverlay[overlay_no] element*/
xoHandleOverlays(graph) 
    struct xgraph_type *graph;
{
	Action	tempaction;
	char	*targs[MAX_NTARGS];
	char	temp1[80],temp2[80];
	Element	*child,*nextchild;
	Element	*overlay;
	Element	**current;
    struct xplot_type *pchild;
	int		plotno=0;
	MsgIn	*msg;

	tempaction.argc = 2;
	targs[0] = "neutral";
	targs[1] = "xoverlay";
	tempaction.argv = targs;
	tempaction.type = CREATE;
	tempaction.name = "CREATE";
	tempaction.data = (char *)graph;

	overlay=Create("neutral","xoverlay",graph,&tempaction,
		graph->overlay_no);

	for(child=graph->child,current = &(graph->child);
		child;child=nextchild) {
		nextchild = child->next;
		if (strcmp(child->object->name,"xplot") != 0) {
			current = &child->next;
			continue;
		}
		if (child->msg_in == NULL) {
			current = &child->next;
			continue;
		}
		if (child->msg_in->type != 0) { /* 0 is PLOT */
			current = &child->next;
			continue;
		}

		/* Set the values for the labels for the plots */
		pchild = (struct xplot_type *)child;
		sprintf(temp1,"%f",1.0 - plotno*0.05);
		sprintf(temp2,"%s[%d].%d",pchild->name,pchild->index,
			graph->overlay_no);
		if (pchild->labelpix)
			XtDestroyWidget(pchild->labelpix);
		pchild->labelpix = NULL;
		/*
		XtVaSetValues(pchild->labelpix,
			XtVaTypedArg,XtNpixX,XtRString,"1.05",5,
			XtVaTypedArg,XtNpixY,XtRString,temp1,1+strlen(temp1),
			XtVaTypedArg,XtNtextcolor,XtRString,pchild->fg,1+strlen(pchild->fg),
			XtNtext,temp2,
			NULL);
		*/

		if (child->msg_in) {
			if (!graph->msg_in) {
				graph->msg_in = child->msg_in;
			} else { /* tag the msg on the end of the msg list */
				for(msg = graph->msg_in;msg->next;msg=msg->next);
				msg->next = child->msg_in;
			}
			child->msg_in = NULL;
		}
		*current = child->next;
		child->parent = child->next = NULL;
		Attach(overlay,child);
	}
	graph->overlay_no++;
}

/* This function gets rid of old overlays */
static int xoZapOverlays(graph) 
    struct xgraph_type *graph;
{
	Action	tempaction;
	char	*targs[MAX_NTARGS];
	char	tempstr[80];
	Element	*child,*nextchild;
	Element	**current;
	int		ret = 0;

	for(child=graph->child,current = &(graph->child);
		child;child=nextchild) {
		nextchild = child->next;
		if (strcmp(child->object->name,"neutral") == 0 &&
			strcmp(child->name,"xoverlay") == 0) {
			*current = child->next;
			child->next = NULL;
			FreeTree(child);
			ret = 1;
		} else {
			current = &child->next;
		}
	}
	return(ret);
}

xoMakeNewPlot(graph,msg,plotno)
    struct xgraph_type *graph;
	MsgIn	*msg;
	int		plotno;
{
	Element	*child;
    struct xplot_type *pchild;
	char	*targs[MAX_NTARGS],**ptargs;
	Action	tempaction;
	char	temp0[10],temp1[80],temp2[80];
	MsgIn	*tempmsg;

	if (msg->type != 0) return; /* PLOT msg */

	tempaction.argc = 2;
	targs[0] = "xplot";
	sprintf(temp0,"plot[%d]",plotno);
	targs[1] = temp0;
	targs[2] = "-value";
	targs[3] = msg->slot[1].data;
	tempaction.argv = targs;
	tempaction.type = CREATE;
	tempaction.name = "CREATE";
	tempaction.data = (char *)graph;
	child=Create("xplot","plot",graph,&tempaction,plotno);
	child->msg_in = msg;
	pchild = (struct xplot_type *)child;
	pchild->xmin = graph->xmin;
	pchild->xmax = graph->xmax;
	pchild->ymin = graph->ymin;
	pchild->ymax = graph->ymax;
	/* pchild->value = CopyString(msg->slot[1].data); */
	pchild->wx = 1.0;
	pchild->wy = 1.0;
	/* pchild->fg = CopyString(msg->slot[2].data); */
/*
	pchild->labelpix = (char *)XtCreateManagedWidget(temp0,
		shapeObjectClass,(Widget)graph->widget,NULL,0);
*/

	/*	Disables child since the parent will handle its PROCESS */
	Block(child);
	/* Call childs RESET action - this deals with the fg etc */
	tempaction.argc = 1;
	tempaction.argv = targs;
	tempaction.type = RESET;
	tempaction.name = "RESET";
	tempaction.data = (char *)NULL;
	/* A hack here to temporarily remove the msg->next for the RESET */
	tempmsg = msg->next;
	msg->next = NULL;
	XPlot(child,&tempaction);
	msg->next = tempmsg;
#if 0
	/* Set the values for the labels for the plots */
	sprintf(temp1,"%f",1.0 - plotno*0.05);
	sprintf(temp2,"%s",msg->slot[1].data);
	XtVaSetValues(pchild->labelpix,
		XtVaTypedArg,XtNpixX,XtRString,"1.05",5,
		XtVaTypedArg,XtNpixY,XtRString,temp1,1+strlen(temp1),
		XtVaTypedArg,XtNtextcolor,XtRString,pchild->fg,1+strlen(pchild->fg),
		XtNtext,temp2,
		NULL);
	XtAddCallback((Widget)pchild->labelpix, XtNcallback,xoCallbackFn,
		(XtPointer)pchild);
#endif
}

static int xoCountPlots(graph)
    struct xgraph_type *graph;
{
	Element	*child;
	int		ret = 0;

	for(child=graph->child; child;child=child->next) {
		if (strcmp(child->object->name,"xplot") == 0)
		ret++;
	}
	return(ret);
}

int xoUpdatePlotLabels(graph)
    struct xgraph_type *graph;
{
	Element	*child;
	int		i = 0;
	Widget	w;
	char temp[20];
	Dimension ht;
	/* int ht = ((GraphWidget)(graph->widget))->core.height; */
	/*float dht = XoFontHt */
	float dht;


	XtVaGetValues((Widget)graph->widget,
		XtNheight,&ht,
		NULL);
	dht = 1.0 *  /* screen ht in graph coords */
				20.0 /  /* ht of each letter, guessed */
				(float)ht;
	for(child=graph->child; child;child=child->next) {
		if (strcmp(child->object->name,"xplot") == 0) {
			w = (Widget)(((struct xplot_type *)child)->labelpix);
			if (w) {
				sprintf(temp,"%f",1.0 - dht * (float)i); 
				XtVaSetValues(w,
					XtVaTypedArg,XtNpixY,XtRString,temp,1+strlen(temp),
					NULL);
				i++;
			}
		}
	}
	return(i);
}

#undef MAX_NTARGS 20
#endif


/* $Id: XoColor.c,v 1.3 2005/07/20 20:01:57 svitak Exp $ */
/*
 * $Log: XoColor.c,v $
 * Revision 1.3  2005/07/20 20:01:57  svitak
 * Added standard header files needed by some architectures.
 *
 * Revision 1.2  2005/07/01 10:02:50  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.30  2001/06/29 21:10:47  mhucka
 * Added some code to support the ability to set a default colormap in
 * TopLevel.c
 *
 * Revision 1.29  2000/09/11 15:48:36  mhucka
 * Used %d to %ld in one of the calls to fprintf involving a long int.
 *
 * Revision 1.28  2000/06/12 04:28:20  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.27  1997/07/25 00:38:19  venkat
 * Fixed bugs to set variables before they were used in XoInitColorscale() and
 * XoGetPixelFromString()
 *
 * Revision 1.26  1995/12/06  01:15:56  venkat
 *  XtConvertAndStore() has a buggy implementation in R4. So changed the calll
 * >> to XtConvert
 *
 * Revision 1.25  1995/09/27  00:14:17  venkat
 * XoGetPixelFromString() now passes the display of the parent draw if the
 * passed widget arg is a pix.
 *
 * Revision 1.24  1995/07/29  00:18:57  venkat
 * Freeing the colormap deemed unnecessary for readonly visuals.
 * Caused problems in cases where only readonly visuals were available.
 *
 * Revision 1.23  1995/07/08  01:29:04  venkat
 * Initial colortable values of half white and half black were not taking effect
 * for elements whose fg was hardcoded. The colorscale_alloced flag is now
 * set in the XoInitColorscale() code, so these can take effect.
 *
 * Revision 1.22  1995/06/22  21:37:07  venkat
 * XoGetPixelFromString() explicitly sets the size of the toVal strcuture
 * to the sizeof(Pixel). This is because the converter is directly called
 * from this wrapper and the size members of fromVal and toVal need to
 * indicate the correct allocation.
 *
 * Revision 1.21  1995/06/15  20:50:52  venkat
 * Some additional checking on the fromVal->addr in the
 * XoCvtStringToPixel() routine.
 *
 * Revision 1.20  1995/06/15  18:18:47  venkat
 * Additional check added at beginning of XoCvtStringToPixel for toVal->size. Also
 * changed the warning messages for allocation failures.
 *
 * Revision 1.19  1995/06/08  17:51:19  venkat
 * Modified code in XoMakeCmap() and XoInitColorScale() to use the default
 * visual for allocating colors if the -defaultcolor command line option
 * is specified when invoking GENESIS
 *
 * Revision 1.18  1995/06/02  19:47:02  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.17  1995/05/17  23:39:49  venkat
 * Some more clean up of XoInitColorscale() and addition of a number of
 * debugging statements.
 *
 * Revision 1.16  1995/05/08  20:03:33  venkat
 * Yet another Color Initialization wash-up.Fixed bug relating
 * to default read-only visuals.
 *
 * Revision 1.15  1995/04/05  00:10:00  venkat
 * Cleaned up XoInitColorscale().
 * Also added code to free allocated read-only cells in the case of
 * a read-only visual in XoMakecmap().
 * 
 *
 * Revision 1.14  1995/03/22  02:36:07  venkat
 * Fixed another bug in XoColorscale(). Now I free only
 * the intended buffer from the default colormap.
 *
 * Revision 1.13  1995/03/22  01:54:45  venkat
 * Fixed bug in XoInitColorscale() where freeing was actually being on the
 * allocated colorscale variable itself. Used a dummy colortable array to
 * fix the problem.
 *
 * Revision 1.12  1995/03/21  01:13:35  venkat
 * XoInitColorScale() changed to allocate R/W cells in the default colormap
 * enough to hold all Xodus colors plus a buffer for user defined colors.
 * Added meningful error messages for situations in which color support may
 * not be available. Also added function XoColorSupported() to check for
 * such situations from genesis.
 *
 * Revision 1.11  1995/03/15  18:15:24  venkat
 * Made make-shift changes to use the default colormap for Xodus
 * regardless of visual
 *
 * Revision 1.10  1995/02/24  19:50:51  venkat
 * Changed name to XoGetUsedColormap()
 *
 * Revision 1.9  1995/02/24  19:41:51  venkat
 * Updated toVal->addr after XtConvert() checking to
 * see if the convert failed
 *
 * Revision 1.8  1995/02/21  02:22:04  venkat
 * Changed XoInitColorscale() to copy specific entries of
 * the default color map into the virtual colormap
 * in case of overflow.
 *
 * Revision 1.7  1995/02/21  01:25:16  venkat
 * Copied default colormap into the private
 * color map and stored part of it as read-write
 * cells for Xodus' sole usage
 *
 * Revision 1.6  1995/02/20  18:40:13  dhb
 * Fixed core in XoCvtStringToPixel() when it's unable to allocate a
 * color.
 *
 * Revision 1.5  1995/02/18  00:38:32  venkat
 * Added code to XoInitColorScale() and XoMakeCmap() to
 * allocate read-write color cells depending on the visual
 * and use the old code of allocating read-only colors
 * as a fallback.
 *
 * Revision 1.4  1995/02/16  00:55:07  venkat
 * Commented XFreeColors once again to await feeback on
 * dhb's proposition for allocating private cells for
 * xcolorscale changes
 *
 * Revision 1.3  1995/02/13  23:37:34  venkat
 * Modified XoMakeCMap() to close the colormap file after
 * reading the colortable entried. Also decommented the
 * statement that frees already allocated colorscale.
 *
 * Revision 1.2  1994/03/22  15:14:47  bhalla
 * Yet another iteration on the endless problems of resource conversions,
 * this time for converting pixel values.
 * Also changed XPointer to caddr_t, which should work on more systems.
 *
 * Revision 1.1  1994/01/13  18:34:14  bhalla
 * Initial revision
 * */
#include <stdio.h>
#include <stdlib.h>	/* malloc() */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "XoDefs.h"
#include "Draw/Pix.h"

#define XO_NCOLORS 64
#define BUFFER 32
#define MINDEPTH 8

Display	*XgDisplay();

/** Color stuff **/

static Visual* 	usedvisual;
static Colormap	usedcmap;
static int	useddepth;
static unsigned long colorscale[XO_NCOLORS+BUFFER];
static int colorscale_alloced = 0;
static int have_genesis_colormap = 0;
static Boolean readonlyvisual=False;
static Boolean colorscalesupport=True;
#ifdef XOCOLOR_DEBUG
static char  *visualname[] = {
  "StaticGray",
  "GrayScale",
  "StaticColor",
  "PseudoColor",
  "TrueColor",
  "DirectColor"
};
#endif
static Boolean XoCvtStringToPixel();
static Boolean XoFindReadwriteVisual();
static int     XoCreatePrivateColormap();
static void    XoSetUsedColorParams();
static XVisualInfo* XoGetVisual();
static int	XoMonochrome();


/** External functions to set the proper 
    color resources when creating the top level shell **/

Colormap XoGetUsedColormap()
{
 return (usedcmap);
}

Visual* XoGetUsedVisual()
{
 return (usedvisual);
}

int XoGetUsedDepth()
{
return (useddepth);
}

Boolean XoColorSupported()
{
 return (colorscalesupport);
}


/*
** make_cmap:	Build our colormap from the rgb table set up in the
** mapname file. 
**
*/

void XoMakeCmap(mapname, reportmissingfile)
	char	*mapname;
	int	 reportmissingfile;
{
	extern int 	GENOPT_UseDefaultColor;
	XColor		rgb[256],color[XO_NCOLORS];
	double		sqrt();		/* for lint */
	int 		screen;
	Colormap 	default_cmap;
	FILE            *fp, *SearchForNonScript();
	int		i,j,r,g,b;

	screen = XDefaultScreen(XgDisplay());
	if (XDisplayCells(XgDisplay(),screen) < 2)
		return;

	default_cmap = DefaultColormap(XgDisplay(),screen);

	fp = SearchForNonScript(mapname,"r");
	if (fp == NULL) {
	        if (!reportmissingfile)
		        fprintf(stderr,"colormap file '%s' not found\n", mapname);
	        return;
	}
	for (i = 0 ; i < 256 ; i++)  {
		if (!fscanf(fp,"%d%d%d",&r,&g,&b))
			break;
		if (feof(fp))
			break;
		rgb[i].red = r;
		rgb[i].green = g;
		rgb[i].blue = b;
	}
	fclose(fp);
	/*
	if(readonlyvisual && colorscale_alloced)
	  XFreeColors(XgDisplay(),default_cmap,colorscale,XO_NCOLORS,0);
	*/
	for (j = 0 ; j < XO_NCOLORS ; j++)
		color[j] = rgb[(i * j)/XO_NCOLORS];

         for (i = 0 ; i < XO_NCOLORS ; i++) {

	/** For read only color tables  which is possible
	    with visuals StaticGray, TrueColor and StaticColor**/

    	   if(readonlyvisual || (GENOPT_UseDefaultColor!=0)){

	  /** Try default color map because virtual private colrmaps
             cannot be created for such visuals**/

	      if (XAllocColor(XgDisplay(),default_cmap,&color[i]) == 0) {
			fprintf(stderr,
				"Cannot allocate any more colors at i = %d."
				"Try closing other applications for better color handling \n",i);
	       return;
	      }
	      else{
               colorscale[i] = color[i].pixel;
	      }
	   }	
	
    /**  Otherwise store colors read-write in the used colormap **/

     	   else{
		color[i].pixel=colorscale[i];
		color[i].flags=DoRed|DoGreen|DoBlue;	
		XStoreColor(XgDisplay(), usedcmap, &color[i]);
           }	

	 }
	colorscale_alloced = 1;
	have_genesis_colormap = 1;
}

int XoHaveCreatedGenesisCmap()
{
    return have_genesis_colormap;
}

void XoInitColorscale()
{
	extern int 	GENOPT_UseDefaultColor;
	int		screen;
	Window		root_window;
	Colormap 	default_cmap, nondefault_cmap;
	int 		default_depth ;
	Visual		*default_vis;
        XVisualInfo 	*nondefault_vis=(XVisualInfo*) malloc(sizeof(XVisualInfo));
	unsigned long plane_masks[1];
#ifdef _DEBUG
	XVisualInfo vtemplate;
	XVisualInfo *visuallist;
#endif

	int 		*depthlist;
	int		depthindex;
	int 		numdepths;
	int i;

	static XtConvertArgRec ColorArgs[] = {
	{XtBaseOffset,
		(caddr_t)XtOffset(Widget,core.self),sizeof(Widget)}
	};

  /* --------------------------------------------------------------------------------
     -------------------------------------------------------------------------------

      Initialize the colorscale values to monochrome
      
     ------------------------------------------------------------------------------
     ------------------------------------------------------------------------------
  */
	
	int black = XBlackPixel(XgDisplay(),0);
	int white = XWhitePixel(XgDisplay(),0);
	colorscale_alloced = 1;
	for(i=XO_NCOLORS/2;i>=0;i--)
		colorscale[i] = black;
	for(i=XO_NCOLORS/2;i<XO_NCOLORS;i++)
		colorscale[i] = white;
	
  /* --------------------------------------------------------------------------------
     -------------------------------------------------------------------------------

      Set the String to XoPixel type converter 
      
     ------------------------------------------------------------------------------
     ------------------------------------------------------------------------------
  */

	XtSetTypeConverter(XtRString,XtRXoPixel,XoCvtStringToPixel,
		ColorArgs,XtNumber(ColorArgs),
		XtCacheNone,NULL);

  /* --------------------------------------------------------------------------------
     -------------------------------------------------------------------------------

	Determine if the screen is monochrome; If it isnt, determine 
	if Read-Write colormaps are supported at any depth >= 8

      ------------------------------------------------------------------------------
     ------------------------------------------------------------------------------
  */
	screen = XDefaultScreen(XgDisplay());
	root_window = XDefaultRootWindow(XgDisplay());
	default_cmap = DefaultColormap(XgDisplay(),screen);
	default_vis = XDefaultVisual(XgDisplay(),screen);
   	default_depth = DefaultDepth(XgDisplay(),screen);

#ifdef _DEBUG
	vtemplate.class = GrayScale;
	vtemplate.depth = default_depth;
  	visuallist = XoGetVisual(&vtemplate);
	default_vis = visuallist[0].visual;
 	default_cmap=XCreateColormap(XgDisplay(), root_window, default_vis, AllocNone); 
#endif

	if (XDisplayCells(XgDisplay(),screen) < 2){
	 fprintf(stderr, "This screen has a depth of %d\n",XDisplayCells(XgDisplay(),screen));
	 fprintf(stderr, "No Xodus color support in this display\n");
	 colorscalesupport=False;
	 readonlyvisual = True;
	 XoSetUsedColorParams(default_cmap,default_vis,default_depth);
         return;
        }


#ifdef XOCOLOR_DEBUG
	fprintf(stderr,"Default visual is %s ",visualname[default_vis->class]);
        fprintf(stderr,"at a depth of %d\n", default_depth);	
	fprintf(stderr,"Default Colormap is %d\n", (int)default_cmap);
#endif
	  depthlist = XListDepths(XgDisplay(), screen, &numdepths);

	if(XoMonochrome(depthlist, numdepths)) { 
		/** only StaticGray (Monochrome) visual available **/

     fprintf(stderr, "The max available depth is (Monochrome) %d \n",depthlist[numdepths-1]);
     fprintf(stderr, "No support for Xodus colorscales in this display \n");
	  colorscalesupport = False;
          readonlyvisual=True;
	  XoSetUsedColorParams(default_cmap,default_vis,default_depth);
	  XFree(depthlist);
	  return; 
	}
	else {

	 /** Decide on whether read-write colorcells can be allocated at depths>=8**/
	 if(GENOPT_UseDefaultColor!=0){
	  XoSetUsedColorParams(default_cmap,default_vis,default_depth);
	  XFree(depthlist);
	  return; 
	 }
         if(!XoFindReadwriteVisual(depthlist,&depthindex,numdepths)){
#ifdef XOCOLOR_DEBUG
	fprintf(stderr,"No Readwrite colors supporting visuals at depths>=%d\n",MINDEPTH);
#endif
		readonlyvisual = True;
	 }
	
	}

  /* --------------------------------------------------------------------------------
     -------------------------------------------------------------------------------
      If Read-Write colormaps are supported, Try Allocating "read-write" cells in the
      default colormap (provided it is allowed) with a buffer size to be freed later.
      This is done because these visuals try and match colors exactly ; 
      Please note that the default colormap may be based on a readonly visual. So it 
      is necessary to do the additional check on the default visual.
      ------------------------------------------------------------------------------
     ------------------------------------------------------------------------------
  */

      if(!readonlyvisual){


        if(default_vis->class==PseudoColor ||default_vis->class == GrayScale){
	
         if(!XAllocColorCells(XgDisplay(),default_cmap,False,plane_masks,0,
				colorscale, XO_NCOLORS+BUFFER)){
	
#ifdef XOCOLOR_DEBUG 
 fprintf(stderr, "Unable to allocate %d cells in default colormap\n", XO_NCOLORS+BUFFER);
#endif

	  /**
	      If already out of color cells in the default colormap
	      create a private virtual colormap at an acceptable depth
	      based on a PseudoColor visual with the first few color cells 
              allocated read-only and copied from the default color map. By 
              doing this other applications do not display badly with their false colors. 
	      XoCreatePrivateColormap() does just this and returns 0 on
	      any failure, in which case we revert to using the default gracefully.
	  **/

	   if(!XoCreatePrivateColormap(&nondefault_cmap,&nondefault_vis,
	&depthlist[depthindex])){
		XoSetUsedColorParams(default_cmap,default_vis,default_depth);
		readonlyvisual = True;
		XFree(depthlist);
		return;
	   }
	   else{
		XoSetUsedColorParams(nondefault_cmap, nondefault_vis[0].visual,depthlist[depthindex]);	
	   }

         }
	 else
	 {
	/**
          Set the used cmap, visual and depth for Xodus to the default cmap if there
	  is enough space and free the  buffer size for user-specified colors. This is
          done only if the default visual was read-write.
	**/
#ifdef XOCOLOR_DEBUG
  fprintf(stderr, "Using default colormap based on %s visual at depth of %d \n", visualname[default_vis->class], default_depth);
#endif
	  XFreeColors(XgDisplay(), default_cmap, &colorscale[XO_NCOLORS],BUFFER,0); 
	  XoSetUsedColorParams(default_cmap,default_vis,default_depth);
	 }
	}
       else {

		/* If the default visual is read-only we again try and create
		   a private virtual read-write colormap at the right depth */

	   if(!XoCreatePrivateColormap(&nondefault_cmap,&nondefault_vis,
	&depthlist[depthindex])){
			XoSetUsedColorParams(default_cmap,default_vis,default_depth);
			readonlyvisual = True;
			XFree(depthlist);
			return;
	   }
	   else{
		XoSetUsedColorParams(nondefault_cmap, nondefault_vis[0].visual,depthlist[depthindex]);	
	   }
	} 
      }
     else
      { /** Read-only situation: Revert to Defaults **/
	  XoSetUsedColorParams(default_cmap,default_vis,default_depth);
      }
    XFree(depthlist);
#ifdef XOCOLOR_DEBUG
 fprintf(stderr, "The visual used is %s\n",visualname[usedvisual->class]);
 fprintf(stderr, "The colormap used is %ld\n",(int)usedcmap);
 fprintf(stderr, "The used depth is %d\n", useddepth);
#endif
}
 
static int XoCreatePrivateColormap(private_cmap,visuallist,depth)
 Colormap  *private_cmap;
 XVisualInfo	**visuallist;
 int *depth;
{
	XColor 	color[XO_NCOLORS];
	XVisualInfo 	vtemplate;
	unsigned long plane_masks[1];
	int i;
	Window		root_window = XDefaultRootWindow(XgDisplay());
  	Colormap  default_cmap = DefaultColormap(XgDisplay(),XDefaultScreen(XgDisplay()));

  /**
            Depth for the private colormap should be atleast 8
            This is because we need atleast 64 cells for the Xodus colorscales
            plus a 64 to be copied from the default colormap plus the 32
            buffer cells for user-specified colors.
  **/
 
 
          if(*depth < MINDEPTH){
            /** Not enough depth in display **/

        fprintf(stderr, "Available depth %d not suited for color \n", *depth);
        fprintf(stderr, "No Xodus color support in this display\n");
	  colorscalesupport=False;
          return(0);

          }
	/** Try and match PseudoColor or GrayScale Visuals if the depth 
            is atleast 8  **/

	  vtemplate.depth = *depth;
	  vtemplate.class = PseudoColor;
	  *visuallist=XoGetVisual(&vtemplate);
	  if(*visuallist==NULL){
	   vtemplate.class = GrayScale;
	   *visuallist=XoGetVisual(&vtemplate); 
 	   if(*visuallist==NULL){
	    /** Just a fallback case  **/
       fprintf(stderr, "No read-write visual at depth of atleast %d \n", MINDEPTH);
       fprintf(stderr, "Private colorscales not supported in this display\n");
	  return(0); 
	   } 
	  }
 
#ifdef XOCOLOR_DEBUG
 fprintf(stderr, "Creating private colormap based on %s visual at depth of %d \n", visualname[(*visuallist)[0].class], *depth);
#endif
        *private_cmap = XCreateColormap(XgDisplay(),root_window, (*visuallist)[0].visual, AllocNone);
 
                /**
                  If no virtual colormaps possible use default and readonly cells
                  Just a matter of meticulous error handling
                **/
 
      if(*private_cmap==default_cmap){
       fprintf(stderr,"Cannot create private color table; ID %d matches default \n", (int)*private_cmap);
       fprintf(stderr,"Try closing other applications and restart genesis for better color handling\n");
                        return(0);
      }
#ifdef XOCOLOR_DEBUG
      else
      {
        fprintf(stderr, "Created private cmap; ID= %d \n", (int)*private_cmap);
        fprintf(stderr, "default  cmap; ID= %d \n", (int)default_cmap);
      }
#endif
 
	/** Allocate the first 64 color cells read-only
             and copied from the default color map
          **/
 
 
         for(i=0; i<XO_NCOLORS; i++){
                color[i].pixel =i;
                XQueryColor(XgDisplay(), default_cmap, &color[i]);
              if (XAllocColor(XgDisplay(),*private_cmap,&color[i]) == 0) {
                       fprintf(stderr,"Cannot allocate any more read only colors at i = %d in private colormap with ID = %d.\n",i,(int)*private_cmap);
                       fprintf(stderr,"Try closing other applications and restart genesis for better color handling\n");
                        return(0);
              }
              else{
                        colorscale[i] = color[i].pixel;
              }
         }
 
        /** Then allocate private read-write cells for Xodus' sole usage **/
 
          if(!XAllocColorCells(XgDisplay(), *private_cmap,False,plane_masks,0,
                                colorscale, XO_NCOLORS)){
                        fprintf(stderr,"Cannot allocate %d R/W cells in private color table \n", XO_NCOLORS);
                        fprintf(stderr,"Try closing other applications and restart genesis for better color handling\n");
                        return(0);
          }
 
          return(1); 
} 


static void XoSetUsedColorParams(cmap, visual, depth)
 Colormap cmap;
 Visual *visual;
 int depth;
{
 usedcmap =  cmap;
 useddepth = depth;
 usedvisual = visual;
}


static Boolean XoFindReadwriteVisual(pdepthlist, pindex, numdepths)
  int *pdepthlist;
  int *pindex;
  int numdepths;
{
 int i;
 XVisualInfo visual_info;
 Boolean readwritevisual = False;
 int screen = XDefaultScreen(XgDisplay());

  for(i=0; i< numdepths && !readwritevisual; i++){
                if(pdepthlist[i] < MINDEPTH) continue;
                readwritevisual = True;
		*pindex =i;
 
        if(!XMatchVisualInfo(XgDisplay(), screen, pdepthlist[i], PseudoColor,
                                &visual_info)){
         if(!XMatchVisualInfo(XgDisplay(), screen, pdepthlist[i], GrayScale,
                                &visual_info)){
                        readwritevisual = False;
	 }
        }
  }
#ifdef XOCOLOR_DEBUG
         fprintf(stderr,"Found a %s visual at a depth of %d\n", visualname[visual_info.visual->class],pdepthlist[*pindex]);
#endif

	return(readwritevisual); 
}
 

static int XoMonochrome(pdepthlist,numdepths)
	int *pdepthlist;
	int numdepths;
{
	int i;
	for(i=0; i< numdepths; i++){
	 if(pdepthlist[i] >= MINDEPTH) return(0);
	}
	return(1);
}

static XVisualInfo* XoGetVisual(ptemplate)
	 XVisualInfo *ptemplate;
{
	  XVisualInfo *retvisual;
	  int visualsmatched;

       retvisual= XGetVisualInfo(XgDisplay(), VisualClassMask|VisualDepthMask, 
					ptemplate, &visualsmatched);
	  if(visualsmatched==0) retvisual=NULL;
	 
	  return (retvisual);
}


static Boolean XoCvtStringToPixel(dpy,args,num_args,fromVal,toVal,destruct)
	Display		*dpy;
	XrmValue	*args;
	Cardinal 	*num_args;
	XrmValue	*fromVal;
	XrmValue 	*toVal;
	XtPointer	destruct;
{
	static XColor screenColor;
	int i;
	Widget w;
	Pixel	*ppixel;

   if(fromVal->addr != NULL){

	/** This is in strict conformity to the resource converter
        	usage explained in the Xt manuals and the done macro in R5 and up **/

	if(toVal->addr != NULL && toVal->size < sizeof(Pixel)){
		 toVal->size = sizeof(Pixel);
		 return (False);
	}

	if (*num_args != 1)
		XtErrorMsg("XoCvtStringToPixel","wrongParameters",
		"XoToolkitError",
  	   "String to pixel conversion needs Widget as an argument",
			(String *)NULL,(Cardinal*) NULL);

	w = *((Widget*)args[0].addr);

	/** Check and convert if the toVal->addr is an index into a colorscale **/
	if (colorscale_alloced &&
		sscanf((char *)(fromVal->addr),"%d",&i) == 1) {
		if (i >=0 && i < XO_NCOLORS) {
			/* a Genesis color defn */
			screenColor.pixel = colorscale[i];
			XoCvtDone(Pixel, screenColor.pixel);
		}
	}
	
 	/** If string is not an index we use the built-in converter
	    from string to pixel: So first store the toVal->addr to later
	    copy the converted string **/	

		ppixel = (Pixel *)toVal->addr;

 	/** Convert the string to Pixel **/

	XtConvert(w,XtRString,fromVal,XtRPixel,toVal);
        if(toVal->addr == NULL){
		/***
		 If the convert did not work,
		 use a contrasted default and complete the conversion ***/

		if(strcmp((char*)fromVal->addr,"LightSteelBlue")!=0)
		printf("Warning: Cannot allocate color %s. Using white default\n",
			(char*)fromVal->addr);
		else
		printf("Warning:Cannot allocate default color %s.  Using white default\n",
			(char*)fromVal->addr);
		screenColor.pixel = XWhitePixel(XgDisplay(), 
						  XDefaultScreen(XgDisplay())); 
		toVal->addr = (caddr_t)ppixel;
		XoCvtDone(Pixel,screenColor.pixel);
	}
	else{
	 /*** If the convert did work, then copy the converted value to the
	      stored address of toVal, provided it is non-null ***/
	 if(ppixel != NULL){	
		*ppixel = *((Pixel *)toVal->addr);
		toVal->addr = (caddr_t)ppixel;
		toVal->size = sizeof(Pixel);
	 }
	 else{
		/** If the stored addr is null, we go about doing the conversion
		    using the converted value. The XoCvtDone macro takes care of 
		    this situation **/
		static Pixel pixelval;
		pixelval = *((Pixel*)toVal->addr);
		toVal->addr = (caddr_t)ppixel;
		XoCvtDone(Pixel,pixelval);	
	 }
	}
	return ((Boolean)True);
   }
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRXoPixel");	
	return ((Boolean)False);
}

/* Public utility function for getting pixel values */
/* On Solaris/Openwin the XPointer cast is used instead of
** caddr_t. I do not know how general this is, certainly it does
** not work with other Sun OSs
*/
int XoGetPixelFromString(w,str)
	Widget	w;
	char	*str;
{
	XrmValue	args[2];
	XrmValue	fromVal;
	XrmValue	toVal;
	XtPointer 	destruct = NULL;
	Cardinal	num_args = 1;
	Pixel		fg = 0;

	if (!w) return(0);
#ifdef PURIFY_WATCH
	purify_watch(str);
#endif
	args[0].addr = (caddr_t)(&w);
	args[0].size = sizeof(w);
	fromVal.addr = (caddr_t)str;
	if(str != NULL)
	fromVal.size = strlen(str)+1; 
	else 
	fromVal.size = sizeof(char*);
	toVal.addr = (caddr_t)(&fg);
 	toVal.size = sizeof(Pixel);  

	if (XtIsSubclass(w, pixObjectClass)) w= XtParent(w);
	if (!(XoCvtStringToPixel(XtDisplay(w),args,&num_args,&fromVal,
		&toVal,destruct)))
		return(0);
	else {
		if (toVal.addr != (caddr_t)(&fg))
			fg = *(Pixel *)(toVal.addr);
		return((int)fg);
	}
}

#if 0
/* This is going to override the default conversion. It is
** taken from the example in the Xt manual, and changed slightly
** to handle the cmap options . Turns out that  the old example
** is horrendously wrong for returns in R4, much misery resulted.
*/
static void XoCvtStringToPixel(dpy,args,num_args,fromVal,toVal,destruct)
	Display		*dpy;
	XrmValue	*args;
	Cardinal 	*num_args;
	XrmValue	*fromVal;
	XrmValue 	*toVal;
	XtPointer	destruct;
{
	static XColor screenColor;
	XColor	exactColor;
	Screen	*screen;
	Colormap	colormap;
	Status	status;
	char	message[1000];
	XrmQuark	q;
	String	params[1];
	Cardinal	num_params= 1;
	int		i;

	if (*num_args != 2)
		XtErrorMsg("XoCvtStringToPixel","wrongParameters",
		"XoToolkitError",
  	   "String to pixel conversion needs screen and colormap arguments",
			(String *)NULL,(Cardinal*) NULL);

	/* screen = *((Screen**)args[0].addr); */
	screen = dpy->screens;
	colormap = *((Colormap *) args[1].addr);

	LowerCase((char *)fromVal->addr,message);
	q = XrmStringToQuark(message);

	if (strcmp((char *)fromVal->addr,XtDefaultBackground) == 0) {
		screenColor.pixel = XWhitePixel(XgDisplay(),0);
		status = 1;
	} else if (strcmp((char *)fromVal->addr ,XtDefaultForeground) == 0){
		screenColor.pixel = XBlackPixel(XgDisplay(),0);
		status = 1;
	} else if ((char) fromVal->addr[0] == '#') { /*some color rgb defn*/
		status = XParseColor(dpy,colormap,
			(String)fromVal->addr, &screenColor);
		if (status != 0)
			status = XAllocColor(dpy,colormap,
				&screenColor);
	} else if (colorscale_alloced &&
		sscanf((char *)(fromVal->addr),"gcol%d",&i) == 1) {
		if (i >=0 && i < XO_NCOLORS) {
			/* a Genesis color defn */
			status = 1;
			screenColor.pixel = colorscale[i];
		} else
			status = 0;
	} else { /* some color name */
		status = XAllocNamedColor(dpy,colormap,
				(String)fromVal->addr,&screenColor,&exactColor);
	}
	if (status == 0) {
		params[0] = (String)fromVal->addr;
		XtWarningMsg("XoCvtStringToPixel","noColormap","XoToolkitError",
			"Cannot allocate colormap entry for \"%s\"",
				params,&num_params);
	} else {
		if (toVal->addr == NULL)
			toVal->addr = (caddr_t) &screenColor.pixel;
		else
			*(Pixel *)toVal->addr = screenColor.pixel;
		toVal->size = sizeof(Pixel);
	}
}
#endif

unsigned long *XoColorscale()
{
	return(colorscale);
}

int XoNcolors()
{
	return(XO_NCOLORS);
}

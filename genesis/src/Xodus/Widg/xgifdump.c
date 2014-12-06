#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "Xo/Xodumputils.h"

#include <stdio.h>

typedef struct colorhist_item* colorhist_vector;
struct colorhist_item {
	XgPixel color;
	int value;
};

typedef struct colorhist_list_item* colorhist_list;
struct colorhist_list_item {
	struct colorhist_item ch;
	colorhist_list next;
};

typedef colorhist_list* colorhash_table;

#ifndef PFI_DEFINED
#define PFI_DEFINED
typedef int (*PFI) ();
#endif

#define MAXCOLORS 256
#define HASH_SIZE 20023
#define COLOR_EQUAL(p,q) ((p).r == (q).r && (p).g == (q).g && (p).b == (q).b)
#define GETR(p) ((p).r)
#define GETG(p) ((p).g)
#define GETB(p) ((p).b)
#define hashpixel(p) ( ( ( (long) GETR(p) * 33023 + (long) GETG(p) * \
 30013 + (long) GETB(p) * 27011 ) & 0x7fffffff ) % HASH_SIZE )

static void Putword (/* int w, FILE* fp */ );
static void GIFEncode();
static colorhist_vector computecolorhist();
static colorhist_vector colorhashtocolorhist();
static colorhash_table colorhisttocolorhash();
static colorhash_table computecolorhash();
static colorhash_table alloccolorhash();
static void addtocolorhash();
static void freecolorhash();
static void freecolorhist();
static int colorstobpp();
static int GetPixel();
static int closestcolor();
static int lookupcolor();
static int sqr();


static void compress(/* int init_bits, FILE** outfile, PFI ReadValue */ );
static void output ( /* code_int code */ );
static void cl_block (/* void */ );
static void cl_hash ( /* count_int hsize */ );
static void writeerr ( /* void */ );
static void char_init (/*  void */ );
static void char_out ( /* int c */ );
static void flush_char (/*  void */ );

static XgPixel  **xgpixels;
static int cols, rows;
static colorhash_table cht;
static int Red[MAXCOLORS],Green[MAXCOLORS],Blue[MAXCOLORS],perm[MAXCOLORS],permi[MAXCOLORS];
static int colors;
static pixval maxtmp;

static void dumpGIF();
Display *XgDisplay();
extern char* xocmdname;

 /** External routine that gets called from the xwindowdump command **/

void XoDumpGIF(w, giffilename)
 Widget w;
 char *giffilename;
{
 FILE *giffile;
 XImage *ximage;
 pixval maxindex;

 if((giffile = fopen(giffilename, "w")) == NULL){
	register char defaultgif[25];
	fprintf(stderr, "%s:Unable to open file '%s' for writing \n", xocmdname, giffilename);
	fprintf(stderr, "Using default file '%s'\n", "./default.gif");
	strcpy(defaultgif,"\0");
	strcat(defaultgif,"./default.gif") ; 
	if((giffile = fopen(defaultgif, "w")) == NULL){
	 XgError("'%s':Unable to open file '%s' for writing", xocmdname, defaultgif);
	 return;
	}
 }
/** Routine to create and return an Ximage from the passed Window/Drawable **/
 fprintf(stderr, "Getting the image \n");
 if((ximage = (XImage*)XoGetXImage(w))==NULL){
	XgError("'%s':Unable to create an ximage",xocmdname);
	return;
 }  
 fprintf(stderr, "Back from Getting the image (Width=%d, Height=%d)\n", ximage->width,
	ximage->height);

 rows = ximage->height;
 cols = ximage->width;
 

/** Routine to get the 2-D map of pixel colors from the created Ximage **/

 fprintf(stderr, "Getting the color pixels from the image \n");
 if((xgpixels = XoReadXgPixels(ximage, &maxindex))==NULL){
	XgError("'%s': Unable to create pixel cmap for the image",xocmdname);
	return;
 } 
 maxtmp = maxindex;

 fprintf(stderr, "Back from Getting the pixels from image with maxval=%d\n", maxtmp);
/** Dump into the GIF file using the ppm library routines **/
 
  fprintf(stderr, "Dumping the GIF file (%s) \n", giffilename);
 dumpGIF(&giffile);
}

static void dumpGIF(GIFfile)
 FILE **GIFfile;
{
 int i, j, k;
 colorhist_vector chv;
 int interlace, transparent, BitsPerPixel;
 XgPixel transcolor;
/** Assume non-interlaced image layout and no transparency extensions**/
 interlace = 0; 
 transparent = -1;

 /* Figure out the colormap */

 chv = computecolorhist( xgpixels, cols, rows, MAXCOLORS, &colors );
 if (chv == (colorhist_vector) 0) {
	XgError("%s:Too many colors (%d) found - Allocation failed", xocmdname, colors);
	return;
 }

    fprintf(stderr, "Finished computing color hist with %d different colors\n",colors);
 /* Now turn our colormap into the appropriate GIF colormap */

 if (maxtmp > 255)
	XgError("Max color value is > 255 - automatically rescaling colors");

 for ( i = 0; i < colors; ++i ){
	if ( maxtmp == 255 )
            {
            Red[i] = GETR( chv[i].color );
            Green[i] = GETG( chv[i].color );
            Blue[i] = GETB( chv[i].color );
            }
        else
            {
            Red[i] = (int) GETR( chv[i].color ) * 255 / maxtmp;
            Green[i] = (int) GETG( chv[i].color ) * 255 / maxtmp;
            Blue[i] = (int) GETB( chv[i].color ) * 255 / maxtmp;
            }
 }

  /** Sort the colormap **/

	fprintf(stderr, "%s: Sorting the colormap\n", xocmdname);
 for (i=0;i<colors;i++)
      permi[i]=i;
  for (i=0;i<colors;i++)
        for (j=i+1;j<colors;j++)
          if (((Red[i]*MAXCOLORS)+Green[i])*MAXCOLORS+Blue[i] >
              ((Red[j]*MAXCOLORS)+Green[j])*MAXCOLORS+Blue[j]) {
            k=permi[i]; permi[i]=permi[j]; permi[j]=k;
            k=Red[i]; Red[i]=Red[j]; Red[j]=k;
            k=Green[i]; Green[i]=Green[j]; Green[j]=k;
            k=Blue[i]; Blue[i]=Blue[j]; Blue[j]=k; 
	  } 
    for (i=0;i<colors;i++)
      perm[permi[i]]=i;
 
  BitsPerPixel = colorstobpp( colors ); 
	fprintf(stderr, "The number of bits per pixel: %d\n", BitsPerPixel);
  /* And make a hash table for fast lookup. */
  cht = colorhisttocolorhash( chv, colors );
  freecolorhist( chv );

   /* figure out the transparent colour index */
    if (transparent > 0) {
        transparent = lookupcolor( cht, &transcolor );
        if (transparent == -1)
            transparent = closestcolor( transcolor);
        else
            transparent = perm[transparent];
    }

   /* All set, let's do it. */
    GIFEncode(
        GIFfile, cols, rows, interlace, 0, transparent, BitsPerPixel,
        Red, Green, Blue, GetPixel );

}

static colorhist_vector computecolorhist(pixels, cols, rows, maxcolors, colorsP )
    XgPixel** pixels;
    int cols, rows, maxcolors;
    int* colorsP;
{
    colorhash_table chtab;
    colorhist_vector chv;

    chtab = computecolorhash( pixels, cols, rows, maxcolors, colorsP );
    if ( chtab == (colorhash_table) 0 )
        return (colorhist_vector) 0;
    chv = colorhashtocolorhist( chtab, maxcolors );
    freecolorhash( chtab );
    return chv;
}
 
static colorhash_table computecolorhash( pixels, cols, rows, maxcolors, colorsP )
    XgPixel** pixels;
    int cols, rows, maxcolors;
    int* colorsP;
{
    colorhash_table chtab;
    register XgPixel* pP;
    colorhist_list chl;
    int col, row, hash;

    chtab = alloccolorhash( );
    *colorsP = 0;
	fprintf(stderr, "%s:Computing colorhashtable ...\n", xocmdname);
    /* Go through the entire image, building a hash table of colors. */
    for ( row = 0; row < rows; ++row ){
	    fprintf(stderr, ".");
        for ( col = 0, pP = pixels[row]; col < cols; ++col, ++pP )
            {
	    hash = hashpixel( *pP );
            for ( chl = chtab[hash]; chl != (colorhist_list) 0; chl = chl->next )
                if ( COLOR_EQUAL( chl->ch.color, *pP ) )
                    break;
            if ( chl != (colorhist_list) 0 )
                ++(chl->ch.value);
            else
                {
                if ( ++(*colorsP) > maxcolors )
                    {
                    freecolorhash( chtab );
                    return (colorhash_table) 0;
                    }
                chl = (colorhist_list) malloc( sizeof(struct colorhist_list_item
) );
                if ( chl == 0 ){
                    XgError( "out of memory computing hash table" );
                    return (colorhash_table) 0;
		}
                chl->ch.color = *pP;
                chl->ch.value = 1;
                chl->next = chtab[hash];
		chtab[hash] = chl;
		}
            }
    }

    return chtab;
}


static colorhist_vector colorhashtocolorhist(chtab, maxcolors)
colorhash_table chtab;
int maxcolors;
{  
    colorhist_vector chv;
    colorhist_list chl;
    int i, j;

    /* Now collate the hash table into a simple colorhist array. */
    chv = (colorhist_vector) malloc( maxcolors * sizeof(struct colorhist_item) )
;
    /* (Leave room for expansion by caller.) */
    if ( chv == (colorhist_vector) 0 ){
        XgError( "out of memory generating histogram" );
	return((colorhist_vector)0);
    }

    /* Loop through the hash table. */
	j = 0;
    for ( i = 0; i < HASH_SIZE; ++i )
        for ( chl = chtab[i]; chl != (colorhist_list) 0; chl = chl->next )
            {
            /* Add the new entry. */
            chv[j] = chl->ch;
            ++j;
            }

 	fprintf(stderr, "The number of histogram items - %d\n", j);
    /* All done. */
    return chv;
}
 
static colorhash_table colorhisttocolorhash(chv, colors)
 colorhist_vector chv;
 int colors;
{
 colorhash_table chtab;
 int i, hash;
    XgPixel color;
    colorhist_list chl;

    chtab = alloccolorhash( );

    for ( i = 0; i < colors; ++i )
        {
	color = chv[i].color;
        hash = hashpixel( color );
        for ( chl = chtab[hash]; chl != (colorhist_list) 0; chl = chl->next )
            if ( COLOR_EQUAL( chl->ch.color, color ) )
                XgError(
                    "same color found twice - %d %d %d", GETR(color),
                    GETG(color), GETB(color) );
        chl = (colorhist_list) malloc( sizeof(struct colorhist_list_item) );
        if ( chl == (colorhist_list) 0 ){
            XgError( "out of memory" );
	    return((colorhash_table)0);
	}
        chl->ch.color = color;
        chl->ch.value = i;
        chl->next = chtab[hash];
        chtab[hash] = chl;
        }

    return chtab;
}
 

static void freecolorhist(chv)
 colorhist_vector chv;
{
 free((char *)chv);
}

static void freecolorhash(chtab)
 colorhash_table chtab;
{
  int i;

    colorhist_list chl, chlnext;

    for ( i = 0; i < HASH_SIZE; ++i )
        for ( chl = chtab[i]; chl != (colorhist_list) 0; chl = chlnext )
            {
            chlnext = chl->next;
            free( (char*) chl );
            }
    free( (char*) chtab );
}

static int
colorstobpp( colors )
int colors;
{
    int bpp;

    if ( colors <= 2 )
        bpp = 1;
    else if ( colors <= 4 )
        bpp = 2;
    else if ( colors <= 8 )
        bpp = 3;
    else if ( colors <= 16 )
        bpp = 4;
    else if ( colors <= 32 )
        bpp = 5;
    else if ( colors <= 64 )
        bpp = 6;
    else if ( colors <= 128 )
        bpp = 7;
    else if ( colors <= 256 )
        bpp = 8;
    else
 	XgError( "can't happen" );

    return bpp;
}

static int closestcolor(color)
 XgPixel color;
 {
 int i,r,g,b,d,
      imin,dmin;

  r=(int)GETR(color)*255/maxtmp;
  g=(int)GETG(color)*255/maxtmp;
  b=(int)GETB(color)*255/maxtmp;

  dmin=1000000;
  for (i=0;i<colors;i++) {
    d=sqr(r-Red[i])+sqr(g-Green[i])+sqr(b-Blue[i]);
    if (d<dmin) {
      dmin=d;
      imin=i; } }
  addtocolorhash(cht,&color,permi[imin]);
  return imin;
 } 

static void addtocolorhash(chtab, colorP, value)
 colorhash_table chtab;
 XgPixel *colorP;
 int value;
{
 register int hash;
    register colorhist_list chl;

    chl = (colorhist_list) malloc( sizeof(struct colorhist_list_item) );
    if ( chl == 0 )
        return;
    hash = hashpixel( *colorP );
    chl->ch.color = *colorP;
    chl->ch.value = value;
    chl->next = chtab[hash];
    chtab[hash] = chl;
}
 
static int lookupcolor(chtab, colorP)
 colorhash_table chtab;
 XgPixel *colorP;
{
  int hash;
    colorhist_list chl;

    hash = hashpixel( *colorP );
    for ( chl = chtab[hash]; chl != (colorhist_list) 0; chl = chl->next )
        if ( COLOR_EQUAL( chl->ch.color, *colorP ) )
            return chl->ch.value;

    return -1;
} 

static colorhash_table alloccolorhash()
{
 colorhash_table chtab;
    int i;

    chtab = (colorhash_table) malloc( HASH_SIZE * sizeof(colorhist_list) );
    if ( chtab == 0 ){
        XgError( "out of memory allocating hash table" );
	return (chtab);
    }

    for ( i = 0; i < HASH_SIZE; ++i )
        chtab[i] = (colorhist_list) 0;

    return chtab;
}
 
static int sqr(x)
int x;
{
 return (x*x);
} 

static int GetPixel(x , y)
int x, y;
 {
    int color;

    color = lookupcolor( cht, &xgpixels[y][x] );
    if (color == -1)
      color = closestcolor(xgpixels[y][x]);
    else
      color=perm[color];
    return color;
 }

/*****************************************************************************
 *
 * GIFENCODE.C    - GIF Image compression interface
 *
 * GIFEncode( FName, GHeight, GWidth, GInterlace, Background, Transparent,
 *            BitsPerPixel, Red, Green, Blue, GetPixel )
 *
 *****************************************************************************/

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

static int Width, Height;
static int curx, cury;
static long CountDown;
static int Pass = 0;
static int Interlace;

/*
 * Bump the 'curx' and 'cury' to point to the next pixel
 */
static void
BumpPixel()
{
        /*
         * Bump the current X position
         */
        ++curx;

        /*
         * If we are at the end of a scan line, set curx back to the beginning
         * If we are interlaced, bump the cury to the appropriate spot,
         * otherwise, just increment it.
         */
        if( curx == Width ) {
                curx = 0;

                if( !Interlace )
                        ++cury;
                else {
                     switch( Pass ) {

                       case 0:
                          cury += 8;
                          if( cury >= Height ) {
                                ++Pass;
                                cury = 4;
                          }
                          break;

                       case 1:
                          cury += 8;
                          if( cury >= Height ) {
                                ++Pass;
                                cury = 2;
                          }
                          break;

                       case 2:
			  cury += 4;
                          if( cury >= Height ) {
                             ++Pass;
                             cury = 1;
                          } break; 
		       case 3: 
			  cury += 2; 
			  break; 
		 	} 
	   	} 
	} 
} 

/*
 * Return the next pixel from the image
 */
static int
GIFNextPixel( getpixel )
PFI getpixel;
{
        int r;

        if( CountDown == 0 )
                return EOF;

        --CountDown;

        r = ( * getpixel )( curx, cury );

        BumpPixel();

        return r;
}


static void GIFEncode( pfp, GWidth, GHeight, GInterlace, Background, Transparent, BitsPerPixel, Red, Green, Blue, GetPixel ) 
FILE** pfp; 
int GWidth, GHeight; 
int GInterlace; 
int Background; 
int Transparent; 
int BitsPerPixel; 
int Red[], Green[], Blue[]; 
PFI GetPixel; 
{
 	int B;
        int RWidth, RHeight;
        int LeftOfs, TopOfs;
        int Resolution;
        int ColorMapSize;
        int InitCodeSize;
        int i;

        Interlace = GInterlace;

        ColorMapSize = 1 << BitsPerPixel;

        RWidth = Width = GWidth;
        RHeight = Height = GHeight;
        LeftOfs = TopOfs = 0;

        Resolution = BitsPerPixel;
 
	 /*
         * Calculate number of bits we are expecting
         */
        CountDown = (long)Width * (long)Height;

        /*
         * Indicate which pass we are on (if interlace)
         */
        Pass = 0;

        /*
         * The initial code size
         */
        if( BitsPerPixel <= 1 )
                InitCodeSize = 2;
        else
                InitCodeSize = BitsPerPixel;

        /*
         * Set up the current x and y position
         */
        curx = cury = 0;

        /*
         * Write the Magic header
         */
        fwrite( Transparent < 0 ? "GIF87a" : "GIF89a", 1, 6, *pfp );

        /*
         * Write out the screen width and height
         */
        Putword( RWidth, pfp );
        Putword( RHeight, pfp );

        /*
         * Indicate that there is a global colour map
         */
        B = 0x80;       /* Yes, there is a color map */
	
	/*
         * OR in the resolution
         */
        B |= (Resolution - 1) << 5;

        /*
         * OR in the Bits per Pixel
         */
        B |= (BitsPerPixel - 1);

        /*
         * Write it out
         */
        fputc( B, *pfp );

        /*
         * Write out the Background colour
         */
        fputc( Background, *pfp );

        /*
         * Byte of 0's (future expansion)
         */
        fputc( 0, *pfp );

        /*
         * Write out the Global Colour Map
         */
        for( i=0; i<ColorMapSize; ++i ) {
                fputc( Red[i], *pfp );
                fputc( Green[i], *pfp );
                fputc( Blue[i], *pfp );
        }

	/*
         * Write out extension for transparent colour index, if necessary.
         */
        if ( Transparent >= 0 ) {
            fputc( '!', *pfp );
            fputc( 0xf9, *pfp );
            fputc( 4, *pfp );
            fputc( 1, *pfp );
            fputc( 0, *pfp );
            fputc( 0, *pfp );
            fputc( Transparent, *pfp );
            fputc( 0, *pfp );
        }
	
	/*
         * Write an Image separator
         */
        fputc( ',', *pfp );

	/*
         * Write the Image header
         */

        Putword( LeftOfs, pfp );
        Putword( TopOfs, pfp );
        Putword( Width, pfp );
        Putword( Height, pfp );

        /*
         * Write out whether or not the image is interlaced
         */
        if( Interlace )
                fputc( 0x40, *pfp );
        else
                fputc( 0x00, *pfp );

        /*
         * Write out the initial code size
         */
        fputc( InitCodeSize, *pfp );

	/*
         * Go and actually compress the data
         */
        compress( InitCodeSize+1, pfp, GetPixel );

        /*
         * Write out a Zero-length packet (to end the series)
         */
        fputc( 0, *pfp );

        /*
         * Write the GIF file terminator
         */
        fputc( ';', *pfp );

        /*
         * And close the file
         */
        fclose( *pfp );
}

static void Putword(w, pfp)
 int w;
 FILE **pfp;
{
 fputc(w & 0xff, *pfp);
 fputc((w / 256) & 0xff, *pfp);
}

/***************************************************************************
 *
 *  GIFCOMPR.C       - GIF Image compression routines
 *
 *  Lempel-Ziv compression based on 'compress'.  GIF modifications by
 *  David Rowley (mgardi@watdcsu.waterloo.edu)
 *
 ***************************************************************************/

/*
 * General DEFINEs
 */

#define BITS    12

#define HSIZE  5003            /* 80% occupancy */

#ifdef NO_UCHAR
 typedef char   char_type;
#else /*NO_UCHAR*/
 typedef        unsigned char   char_type;
#endif /*NO_UCHAR*/
/*
 * a code_int must be able to hold 2**BITS values of type int, and also -1
 */
typedef int             code_int;

#ifdef SIGNED_COMPARE_SLOW
typedef unsigned long int count_int;
typedef unsigned short int count_short;
#else /*SIGNED_COMPARE_SLOW*/
typedef long int          count_int;
#endif /*SIGNED_COMPARE_SLOW*/


/*
 *
 * GIF Image compression - modified 'compress'
 *
 * Based on: compress.c - File compression ala IEEE Computer, June 1984.
 *
 * By Authors:  Spencer W. Thomas       (decvax!harpo!utah-cs!utah-gr!thomas)
 *              Jim McKie               (decvax!mcvax!jim)
 *              Steve Davies            (decvax!vax135!petsd!peora!srd)
 *              Ken Turkowski           (decvax!decwrl!turtlevax!ken)
 *              James A. Woods          (decvax!ihnp4!ames!jaw)
 *              Joe Orost               (decvax!vax135!petsd!joe)
 *
 */
#include <ctype.h>

#define ARGVAL() (*++(*argv) || (--argc && *++argv))

static int n_bits;                        /* number of bits/code */
static int maxbits = BITS;                /* user settable max # bits/code */
static code_int maxcode;                  /* maximum code, given n_bits */

static code_int maxmaxcode = (code_int)1 << BITS; /* should NEVER generate this code */
#ifdef COMPATIBLE               /* But wrong! */
# define MAXCODE(n_bits)        ((code_int) 1 << (n_bits) - 1)
#else /*COMPATIBLE*/
# define MAXCODE(n_bits)        (((code_int) 1 << (n_bits)) - 1)
#endif /*COMPATIBLE*/

static count_int htab [HSIZE];
static unsigned short codetab [HSIZE];
#define HashTabOf(i)       htab[i]
#define CodeTabOf(i)    codetab[i]

static code_int hsize = HSIZE;                 /* for dynamic table sizing */

/*
 * To save much memory, we overlay the table used by compress() with those
 * used by decompress().  The tab_prefix table is the same size and type
 * as the codetab.  The tab_suffix table needs 2**BITS characters.  We
 * get this from the beginning of htab.  The output stack uses the rest
 * of htab, and contains characters.  There is plenty of room for any
 * possible stack (stack used to be 8000 characters).
 */

#define tab_prefixof(i) CodeTabOf(i)
#define tab_suffixof(i)        ((char_type*)(htab))[i]
#define de_stack               ((char_type*)&tab_suffixof((code_int)1<<BITS))

static code_int free_ent = 0;                  /* first unused entry */

/*
 * block compression parameters -- after all codes are used up,
 * and compression rate changes, start over.
 */
static int clear_flg = 0;

static int offset;
static long int in_count = 1;            /* length of input */
static long int out_count = 0;           /* # of codes output (for debugging) */

/*
 * compress stdin to stdout
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the
 * prefix code / next character combination.  We do a variant of Knuth's
 * algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
 * secondary probe.  Here, the modular division first probe is gives way
 * to a faster exclusive-or manipulation.  Also do block compression with
 * an adaptive reset, whereby the code table is cleared when the compression
 * ratio decreases, but after the table fills.  The variable-length output
 * codes are re-sized at this point, and a special CLEAR code is generated
 * for the decompressor.  Late addition:  construct the table according to
 * file size for noticeable speed improvement on small files.  Please direct
 * questions about this implementation to ames!jaw.
 */

static int g_init_bits;
static FILE** g_outfile;

static int ClearCode;
static int EOFCode;

static void compress( init_bits, outfile, ReadValue)
int init_bits;
FILE** outfile;
PFI ReadValue;
{
    register long fcode;
    register code_int i /* = 0 */;
    register int c;
    register code_int ent;
    register code_int disp;
    register code_int hsize_reg;
    register int hshift;

    /*
     * Set up the globals:  g_init_bits - initial number of bits
     *                      g_outfile   - pointer to output file
     */
    g_init_bits = init_bits;
    g_outfile = outfile;

    /*
     * Set up the necessary values
     */
    offset = 0;
    out_count = 0;
    clear_flg = 0;
    in_count = 1;
    maxcode = MAXCODE(n_bits = g_init_bits);

    ClearCode = (1 << (init_bits - 1));
    EOFCode = ClearCode + 1;
    free_ent = ClearCode + 2;

    char_init();

    ent = GIFNextPixel( ReadValue );

    hshift = 0;
    for ( fcode = (long) hsize;  fcode < 65536L; fcode *= 2L )
        ++hshift;
    hshift = 8 - hshift;                /* set hash code range bound */

    hsize_reg = hsize;
    cl_hash( (count_int) hsize_reg);            /* clear hash table */

     output( (code_int)ClearCode );

#ifdef SIGNED_COMPARE_SLOW
    while ( (c = GIFNextPixel( ReadValue )) != (unsigned) EOF ) {
#else /*SIGNED_COMPARE_SLOW*/
    while ( (c = GIFNextPixel( ReadValue )) != EOF ) {  /* } */
#endif /*SIGNED_COMPARE_SLOW*/

        ++in_count;

        fcode = (long) (((long) c << maxbits) + ent);
        i = (((code_int)c << hshift) ^ ent);    /* xor hashing */

        if ( HashTabOf (i) == fcode ) {
            ent = CodeTabOf (i);
            continue;

	} else if ( (long)HashTabOf (i) < 0 )      /* empty slot */
            goto nomatch;
        disp = hsize_reg - i;           /* secondary hash (after G. Knott) */
        if ( i == 0 )
            disp = 1;
probe:
        if ( (i -= disp) < 0 )
            i += hsize_reg;

        if ( HashTabOf (i) == fcode ) {
            ent = CodeTabOf (i);
            continue;
        }
        if ( (long)HashTabOf (i) > 0 )
            goto probe;
	nomatch:
        output ( (code_int) ent );
        ++out_count;
        ent = c;
#ifdef SIGNED_COMPARE_SLOW
        if ( (unsigned) free_ent < (unsigned) maxmaxcode) {
#else /*SIGNED_COMPARE_SLOW*/
        if ( free_ent < maxmaxcode ) {  /* } */
#endif /*SIGNED_COMPARE_SLOW*/
            CodeTabOf (i) = free_ent++; /* code -> hashtable */
            HashTabOf (i) = fcode;
        } else
                cl_block();
    }
    /*
     * Put out the final code.
     */
    output( (code_int)ent );
    ++out_count;
    output( (code_int) EOFCode );
}

 
/*****************************************************************
 * TAG( output )
 *
 * Output the given code.
 * Inputs:
 *      code:   A n_bits-bit integer.  If == -1, then EOF.  This assumes
 *              that n_bits =< (long)wordsize - 1.
 * Outputs:
 *      Outputs code to the file.
 * Assumptions:
 *      Chars are 8 bits long.
 * Algorithm:
 *      Maintain a BITS character long buffer (so that 8 codes will
 * fit in it exactly).  Use the VAX insv instruction to insert each
 * code in turn.  When the buffer fills up empty it and start over.
 */

static unsigned long cur_accum = 0;
static int cur_bits = 0;

static unsigned long masks[] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F,
                                  0x001F, 0x003F, 0x007F, 0x00FF,
                                  0x01FF, 0x03FF, 0x07FF, 0x0FFF,
                                  0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };

static void
output( code )
code_int  code;
{
    cur_accum &= masks[ cur_bits ];

    if( cur_bits > 0 )
        cur_accum |= ((long)code << cur_bits);
    else
        cur_accum = code;

    cur_bits += n_bits;

    while( cur_bits >= 8 ) {
        char_out( (unsigned int)(cur_accum & 0xff) );
        cur_accum >>= 8;
        cur_bits -= 8;
    }

     /*
     * If the next entry is going to be too big for the code size,
     * then increase it, if possible.
     */
   if ( free_ent > maxcode || clear_flg ) {

            if( clear_flg ) {

                maxcode = MAXCODE (n_bits = g_init_bits);
                clear_flg = 0;

            } else {

                ++n_bits;
                if ( n_bits == maxbits )
                    maxcode = maxmaxcode;
                else
                    maxcode = MAXCODE(n_bits);
            }
        }

    if( code == EOFCode ) {
        /*
         * At EOF, write the rest of the buffer.
         */
        while( cur_bits > 0 ) {
                char_out( (unsigned int)(cur_accum & 0xff) );
                cur_accum >>= 8;
                cur_bits -= 8;
        }

    flush_char();

        fflush( *g_outfile );

        if( ferror( *g_outfile ) )
                writeerr();
    }
}

/*
 * Clear out the hash table
 */
static void
cl_block ()             /* table clear for block compress */
{

        cl_hash ( (count_int) hsize );
        free_ent = ClearCode + 2;
        clear_flg = 1;

        output( (code_int)ClearCode );
}

static void
cl_hash(hsize)          /* reset code table */
register count_int hsize;
{

        register count_int *htab_p = htab+hsize;

        register long i;
        register long m1 = -1;

        i = hsize - 16;
        do {                            /* might use Sys V memset(3) here */
                *(htab_p-16) = m1;
                *(htab_p-15) = m1;
                *(htab_p-14) = m1;
                *(htab_p-13) = m1;
		*(htab_p-12) = m1;
                *(htab_p-11) = m1;
                *(htab_p-10) = m1;
                *(htab_p-9) = m1;
                *(htab_p-8) = m1;
                *(htab_p-7) = m1;
                *(htab_p-6) = m1;
                *(htab_p-5) = m1;
                *(htab_p-4) = m1;
                *(htab_p-3) = m1;
                *(htab_p-2) = m1;
                *(htab_p-1) = m1;
                htab_p -= 16;
        } while ((i -= 16) >= 0);

        for ( i += 16; i > 0; --i )
                *--htab_p = m1;
}

static void
writeerr()
{
        XgError( "error writing output file" );
}

/******************************************************************************
 *
 * GIF Specific routines
 *
 ******************************************************************************/

/*
 * Number of characters so far in this 'packet'
 */
static int a_count;

/*
 * Set up the 'byte output' routine
 */
static void
char_init()
{
        a_count = 0;
}

/*
 * Define the storage for the packet accumulator
 */
static char accum[ 256 ];

/*
 * Add a character to the end of the current packet, and if it is 254
 * characters, flush the packet to disk.
 */
static void
char_out( c )
int c;
{
        accum[ a_count++ ] = c;
        if( a_count >= 254 )
                flush_char();
}

/*
 * Flush the packet to disk, and reset the accumulator
 */
static void
flush_char()
{
        if( a_count > 0 ) {
                fputc( a_count, *g_outfile );
		fwrite( accum, 1, a_count, *g_outfile );
                a_count = 0;
        }
}

/* The End */





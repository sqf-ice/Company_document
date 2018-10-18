#ifndef __NDK_TPPRNBDF_H__
#define	__NDK_TPPRNBDF_H__

typedef unsigned short	MWIMAGEBITS;	/* bitmap image unit size*/


/* builtin C-based proportional/fixed font structure */
/* based on The Microwindows Project http://microwindows.org */
typedef struct {
	char *		name;		/* font name*/
	int		maxwidth;	/* max width in pixels*/
	int 		height;		/* height in pixels*/
	int		ascent;		/* ascent (baseline) height*/
	int		firstchar;	/* first character in bitmap*/
	int		size;		/* font size in glyphs*/
	MWIMAGEBITS *	bits;		/* 16-bit right-padded bitmap data*/
	unsigned long *	offset;		/* offsets into bitmap data*/
	unsigned char *	width;		/* character widths or NULL if fixed*/
	unsigned char * bbw;
	signed char * xoff;
	int		defaultchar;	/* default char (not glyph index)*/
	long		bits_size;	/* # words of MWIMAGEBITS bits*/

	/* unused by runtime system, read in by convbdf*/
	char *		facename;	/* facename of font*/
	char *		copyright;	/* copyright info for loadable fonts*/
	int		pixel_size;
	int		descent;
	int		fbbw, fbbh, fbbx, fbby;
} MWCFONT, *PMWCFONT;

typedef struct prnfontcust
{
	int fontid;
	int start;           // firstChar ( ASCII code )
	int end;           // lastChar ( ASCII code )
	struct prnfontcust *next;           // pNext
	PMWCFONT  pf;
} PRN_SECTION_HEADER;


#endif


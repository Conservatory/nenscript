/*
 * $Id: paper.h,v 1.1 1992/10/02 01:02:32 craigs Exp $
 */

/* structure to hold metrics for a particular paper type */

struct PaperMetrics {
  char *Name;                   /* name of paper type */

  long PaperHeight;             /* height of paper in 72dpi units */
  long PaperWidth;              /* width of paper in 72dpi units */

  char *TitleFont;              /* font use for non-gaudy titles */
  char *PortraitFont;           /* font to use for portrait text */
  char *LandscapeFont;          /* font to use for landscape text */

  char *GaudyPNFont;            /* font to use for gaudy mode page numbers */
  char *GaudyDateFont;          /* font to use for gaudy mode dates */
  char *GaudyTitleFont;         /* font to use for gaudy mode title */
  char *ClassificationFont;     /* font to use for classification header/footer */

  long GaudyBoxWidth;           /* width of boxes in gaudy mode */
  long GaudyBoxHeight;          /* height of boxes in gaudy mode */
  long GaudyBarHeight;          /* height of bar in gaudy mode */

  long ColumnSep;				/* column separator */

};

extern struct PaperMetrics PaperTypes[];

/* define margins around printable area */
#ifndef SILLY_PAGE

#define	PAGE_LEFT_MARGIN	25L
#define	PAGE_RIGHT_MARGIN	25L
#define	PAGE_TOP_MARGIN		25L
#define	PAGE_BOT_MARGIN		36L

#define PAGE_LANDSCAPE_XOFFS	0L		/* origin X translate when in landscape mode */
#define PAGE_LANDSCAPE_YOFFS	0L		/* origin Y translate when in landscape mode */

#else

#define	PAGE_LEFT_MARGIN	25L			/* margin on left size of page */
#define	PAGE_RIGHT_MARGIN	25L			/* margin on right side of page */
#define	PAGE_TOP_MARGIN		75L		    /* margin at top of page */
#define	PAGE_BOT_MARGIN		36L			/* margin at bottom of page */

#define PAGE_LANDSCAPE_XOFFS	0L		/* origin X translate when in landscape mode */
#define PAGE_LANDSCAPE_YOFFS	0L		/* origin Y translate when in landscape mode */

#endif

/* define the scaling factor used */
#define	SCALE			100L

/* "colour" of gaudy mode artifacts */
#define	BOXGRAY			"0.7"
#define	BARGRAY			"0.95"


/* scaled versions of the dimensions above */
/*#define	BW			(BOX_WIDTH * SCALE)
#define	BH			(BOX_HEIGHT * SCALE)
#define	BS			(BAR_HEIGHT * SCALE) */


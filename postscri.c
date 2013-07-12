/*
 *   $Id: postscri.c,v 1.2 1992/10/02 01:02:32 craigs Exp $
 *
 *   This code was written by Craig Southeren whilst under contract
 *   to Computer Sciences of Australia, Systems Engineering Division.
 *   It has been kindly released by CSA into the public domain.
 *
 *   Neither CSA or me guarantee that this source code is fit for anything,
 *   so use it at your peril. I don't even work for CSA any more, so
 *   don't bother them about it. If you have any suggestions or comments
 *   (or money, cheques, free trips =8^) !!!!! ) please contact me
 *   care of geoffw@extro.ucc.oz.au
 *
 *   Latin1 caracter set handling and fraction point sizes added by
 *   Jonas Lagerblad (jonas@sisu.se).
 *
 *   handling for different classification banners for 1st page added by 
 *   Daniel Risacher (magnus@alum.mit.edu)
 */

#include "machdep.h"
#include "defs.h"


#include "paper.h"
#include "postscri.h"
#include "fontwidt.h"
#include "font_lis.h"

#include <ctype.h>

/********************************
  defines
 ********************************/

#define	TAB 	0x9

/********************************
  imports
 ********************************/

#include "main.h"

/********************************
  exports
 ********************************/


/********************************
  globals
 ********************************/

static int  touched_page = False;
static char *CurrentFilename;
static int  WrapFlag;
static int  GaudyFlag;
static int  TabStop;

static long Columns;			/* number of columns */
static long X;
static long Y;
static long BFH;				/* body font height */
static long BFW;				/* body font width */
static long TFH;				/* title font height */
static long PageNum;			/* page number within each documenr printed */
static long PageCount;			/* total number of pages printed */
static int  TitleEnabled;		/* True if title to be displayed */

static long PW;				/* page width transformed for landscape mode */
static long PH;				/* page height transformed for landscape mode */
static long LM;				/* left margin */
static long RM;				/* right margin */
static long TM;				/* top */
static long BM;				/* bottom margin */
static long WW;				/* width of current print window */
static unsigned int CPL;	/* characters per line */
static long CC;				/* current column */
static long LineNumber;			/* number of line on page (1 top n) */
static long LinesPerPage;		/* number of lines per page, or 0 if to use default */
static char *Classification_1st;	/* points to string to use for classification */
static char *Classification;		/* points to string to use for classification */
static long ColumnSep;      /* width between columns */

static long ClassY;			/* Y location to print classification at */
static long TitleY;			/* Y location to print title at */
static long StartY;			/* Y location to print text from */
static long EndY;			/* Y location to print text down to */
static long ClassBottomY;		/* Y location to print bottom classification at */



/*
 * forward declarations if in ANSI mode
 */

#ifdef __STDC__
void          PrintPSString   (FILE *, unsigned char *, long);
void          EndPage         (FILE *);
void          StartPage       (FILE *);
void          PrintLine       (FILE *, char *, long, int);
static char * ExtractFontSize (char *, long *);

char *font;

#endif


/********************************
  PrintPSString
    Print a string of ASCII characters of length "len" as a
    PostScript string, i.e. enclosed in brackets and with appropriate
    escape characters.
 ********************************/

void PrintPSString (stream, line, len)

     FILE *stream;
     unsigned char *line;
     long len;
     
{
  register long i;
  register unsigned char * str = line;
  
  fprintf (stream, "(");
  for (i = 0; i < len ; i++)
    if ( str[i] & 0x80 )
      fprintf (stream, "\\%03o", str[i]);
    else
      fprintf (stream, "%s%c", str[i] == ')'  ||
	       str[i] == '('  ||
	       str[i] == '\\' ? "\\" : "", str[i]);
  fprintf (stream, ")");
}

/********************************
				 ExtractFontSize
				 Extracts the fontsize from a font type description like "Courier10".
				 Also, copy the font name to the fontname ptr
********************************/

static char * ExtractFontSize (font, fontsize)
     
     char *font;
     long *fontsize;
     
{
  char *p, *s, *t;
  int i;
  extern double atof();
  
  /* get ptr to end of string */
  p = &font[strlen(font)-1];
  
  /* move backwards until we find a character that is not a digit */
  while (p > font && (isdigit (*p) || *p == '.'))
    p--;
  
  /* extract the font size */
  *fontsize = atof (++p) * SCALE;
  
  /* now duplicate and copy the font name */
  t = s = (char *)malloc (p - font + 1);
  for (i = p - font; i> 0;i--)
    *t++ = *font++;
  *t = '\0';
  
  return s;
}




/********************************
  StartPage
    Called to start a new page
********************************/

void StartPage (stream)
     
     FILE *stream;
     
{
  /* insert a page marker as per the Structuring conventions */
  fprintf (stream, "%%%%Page: %li %li\n", PageNum, PageCount);
  
  /* call the StartPage procedure with the appropriate arguments */
  fprintf (stream, "(%li) ", PageNum);
  PrintPSString (stream, CurrentFilename, strlen (CurrentFilename));
  fprintf (stream, " StartPage\n");
  
  /* set X and Y location */
  X = LM;
  Y = StartY;
  
  /* set the column back to column 0 */
  CC = 0;
  
  /* set the line number to line # 1 */
  LineNumber = 1;
  
  /* we have now touched the page */
  touched_page = True;
}


/********************************
  EndPage
    Called to complete a page
 ********************************/

void EndPage (stream)
     
     FILE *stream;
     
{
  /* call the end page procedure */
  fprintf (stream, "EndPage\n");
  
  /* increment both the job and document page counters */
  PageNum++;
  PageCount++;
  
  /* now we haven't touched the page */
  touched_page = False;
}


/*******************************

  EndColumn

 *******************************/

void EndColumn (stream)

     FILE *stream;

{
  LineNumber = 1;
  CC++;
  if (CC == Columns)
    EndPage (stream);
  else {
    X += (ColumnSep * SCALE) + WW;
    Y = StartY;
  }
}


/********************************
  PrintLine
    Called to print a line which has been chopped to
    the correct length already. The "first" flag indicates
    whether the line is a normal line or a continuation line.
    This routine performs the skipping for indents.
 ********************************/

void PrintLine (stream, line, count, first)

     FILE *stream;
     char *line;
     long  count;
     int  first;
     
{
  long i;
  
  if (!touched_page)
    StartPage (stream);
  
  /* make i point to the first non-blank character on the line */
  for (i = 0; i < count && line[i] == ' ';i++)
    ;
  
  if (i < count) {
    if (!first)
      fprintf (stream, "%li %li K ", Y, X);
    PrintPSString (stream, &line[i], count - i);
    if (i > 0)
      fprintf (stream, " %li %li %i L\n", Y, X, i);
    else
      fprintf (stream, " %li %li T\n", Y, X);
  }
  
  Y -= BFH * 11 / 10;		/* put a little bit of extra spacing between the characters */
  LineNumber++;
  
  if (Y < EndY || (LinesPerPage > 0 && LineNumber > LinesPerPage))
    EndColumn (stream);
}

/********************************
  WriteLine
    Writes a line to the output. This routine performs the wrapping and tab expansion
 ********************************/

void WriteLine (stream, line)

     FILE *stream;
     char *line;
     
{
  int first = True;
  char *p;
  char *q;
  int col;
  int i;

  char full_line[8192];

  /* expand tabs if we have to */
  if (strchr (line, TAB) != NULL) {
    col = 0;
    q = full_line;
    for (p = line; *p ; p++)
      if (*p != TAB) {
        *q++ = *p;
        col = (col + 1) % TabStop;
      } else {
        for (i = TabStop - col; i > 0; i--)
          *q++ = ' ';
        col = 0;
      }
    *q = 0;
    line = full_line;
  }

  if (WrapFlag) {
    while (strlen(line) > CPL) {
      PrintLine (stream, line, CPL, first);
      first = False;
      line += CPL;
    }
    PrintLine (stream, line, strlen(line), first);
  } else
    PrintLine (stream, line, MIN(strlen(line), CPL), True);
}


/********************************
  StartDocument
    Called when a new document is to be printed. Not to be confused with
    starting the job.
 ********************************/

void StartDocument (stream, filename)
     
     FILE *stream;
     char *filename;
     
{
  /* restart the internal page number */
  PageNum = 1;

  /* indicate that the first page has not yet been touched */
  touched_page = False;
  
  /* and set the current filename */
  CurrentFilename = filename;
}


/********************************
				 EndDocument
				 Called when a document has been completed.
********************************/

void EndDocument (stream)

     FILE *stream;

{
  /* if we have drawn a partial page, finish it off properly */
  if (touched_page)
    EndPage (stream);
}

    static char pspro_latin1_data[] = { "\
/newcodes	% foreign character encodings\n\
[\n\
160/space 161/exclamdown 162/cent 163/sterling 164/currency\n\
165/yen 166/brokenbar 167/section  168/dieresis 169/copyright\n\
170/ordfeminine 171/guillemotleft 172/logicalnot 173/hyphen 174/registered\n\
175/macron 176/degree 177/plusminus 178/twosuperior 179/threesuperior\n\
180/acute 181/mu 182/paragraph  183/periodcentered 184/cedilla\n\
185/onesuperior 186/ordmasculine 187/guillemotright 188/onequarter\n\
189/onehalf 190/threequarters 191/questiondown 192/Agrave 193/Aacute\n\
194/Acircumflex 195/Atilde 196/Adieresis 197/Aring 198/AE 199/Ccedilla\n\
200/Egrave 201/Eacute 202/Ecircumflex 203/Edieresis 204/Igrave  205/Iacute\n\
206/Icircumflex 207/Idieresis 208/Eth 209/Ntilde 210/Ograve 211/Oacute\n\
212/Ocircumflex 213/Otilde  214/Odieresis 215/multiply 216/Oslash\n\
217/Ugrave 218/Uacute 219/Ucircumflex 220/Udieresis 221/Yacute 222/Thorn\n\
223/germandbls 224/agrave 225/aacute 226/acircumflex 227/atilde\n\
228/adieresis 229/aring 230/ae 231/ccedilla  232/egrave 233/eacute\n\
234/ecircumflex 235/edieresis 236/igrave 237/iacute 238/icircumflex\n\
239/idieresis 240/eth 241/ntilde 242/ograve 243/oacute 244/ocircumflex\n\
245/otilde 246/odieresis 247/divide 248/oslash 249/ugrave  250/uacute\n\
251/ucircumflex 252/udieresis 253/yacute 254/thorn 255/ydieresis\n\
] def\n\
\n\
/reencdict 12 dict def\n\
\n\
" };

    static char pspro_latin1_func[] = { "\n\
    % change fonts using ISO Latin1 characters\n\
/ChgFnt		% size psname natname  =>  font\n\
{\n\
    dup FontDirectory exch known	% is re-encoded name known?\n\
    { exch pop }			% yes, get rid of long name\n\
    { dup 3 1 roll ReEncode } ifelse	% no, re-encode it\n\
    findfont exch scalefont setfont\n\
} def\n\
\n\
/ReEncode	%\n\
{\n\
    reencdict begin\n\
	/newname exch def\n\
	/basename exch def\n\
	/basedict basename findfont def\n\
	/newfont basedict maxlength dict def\n\
	basedict\n\
	{ exch dup /FID ne\n\
	    { dup /Encoding eq\n\
		{ exch dup length array copy newfont 3 1 roll put }\n\
		{ exch newfont 3 1 roll put } ifelse\n\
	    }\n\
	    { pop pop } ifelse\n\
	} forall\n\
	newfont /FontName newname put\n\
	newcodes aload pop newcodes length 2 idiv\n\
	{ newfont /Encoding get 3 1 roll put } repeat\n\
	newname newfont definefont pop\n\
    end\n\
} def\n\
\n\
" };

/********************************
StartJob
Called when a new job is to be started. This performs all of the
PostScript initialisation
 ********************************/

void StartJob (stream, filename, landscape, columns, BodyFont,
TitleFont, wrap, enabletitle, title, copies, gaudy, force_lines, classification_1st, classification, papermetrics, tabstop)


FILE * stream;
int  landscape;
int  columns;
char *BodyFont;
char *TitleFont;
int  wrap;
int  enabletitle;
char *title;
int  copies;
int  gaudy;
int  force_lines;
char *filename;
char *classification_1st;
char *classification;
struct PaperMetrics *papermetrics;
int  tabstop;

{
  char *bodyfont;
  char *titlefont;
  char *gaudyPNfont;
  char *gaudytitlefont;
  char *gaudydatefont;
  char *classfont;
  long  gaudyPNfontsize;
  long  gaudytitlefontsize;
  long  gaudydatefontsize;
  long  classfontsize;

  time_t thetime = time(NULL);
  char  * timestring = strtok(ctime (&thetime), "\n");  /* a simple time string */
  struct tm *tm      = localtime (&thetime);
  char tm_string[15];
  char dt_string[15];

  /* get the time and date strings */
  GETDATE (tm, dt_string);
  GETTIME (tm, tm_string);

  /* make local copies of various flags */
  WrapFlag       = wrap;
  GaudyFlag      = gaudy && enabletitle;
  TitleEnabled   = enabletitle;
  Columns        = columns;
  LinesPerPage   = force_lines;
  Classification_1st = classification_1st;
  Classification = classification;
  ColumnSep      = papermetrics->ColumnSep;
  TabStop        = tabstop;

  /* get the size and width of the font used for the body of the text */
  bodyfont = ExtractFontSize (BodyFont, &BFH);
  BFW      = GetFontWidth    (bodyfont, BFH);

  /* extract various font sizes */
  gaudyPNfont    = ExtractFontSize (papermetrics->GaudyPNFont,    &gaudyPNfontsize);
  gaudytitlefont = ExtractFontSize (papermetrics->GaudyTitleFont, &gaudytitlefontsize);
  gaudydatefont  = ExtractFontSize (papermetrics->GaudyDateFont,  &gaudydatefontsize);

  /* get the size of the font used for the title */
  titlefont = ExtractFontSize (TitleFont, &TFH);

  /* now the classification font, if there is one */
  if (Classification != NULL || Classification_1st != NULL)
    classfont = ExtractFontSize (papermetrics->ClassificationFont, &classfontsize);

  /* output obligatory Postscript header */
  fprintf (stream, "%%!PS-Adobe-1.0\n");

  /* output the filename of the first file as the title */
  fprintf (stream, "%%%%Title: %s\n", filename);

  /* put version of program in as creator */
  fprintf (stream, "%%%%Creator: %s\n", version_string);

  /* extract the users full name and put in as the creator */
  fprintf (stream, "%%%%For: %s\n", USERNAME);

  /* put in the time as a string */
  fprintf (stream, "%%%%CreationDate: %s\n", timestring);

  /* output the font list */
  add_font_to_list (titlefont);
  add_font_to_list (bodyfont);
  if (GaudyFlag) {
    add_font_to_list (gaudyPNfont);
    add_font_to_list (gaudydatefont);
    add_font_to_list (gaudytitlefont);
  }
  if (Classification != NULL || Classification_1st != NULL)
    add_font_to_list (classfont);

  fprintf (stream, "%%%%DocumentFonts: ");
  enumerate_fonts (stream);
  fprintf (stream, "\n");

  /* initialise the document page counter and indicate that the page count info
     can be found at the end of the document. This copout means we don't have
     to store the output data in a temporary file (like enscript) just so we can
     find out how many pages there are */
  PageCount = 1;
  fprintf (stream, "%%%%Pages: (atend)\n");

  /* End of header marker */
  fprintf (stream, "%%%%EndComments\n");

  /* allow Latin-1 character set by remapping most characters above 127 */
  fprintf (stream, "%s\n%s", pspro_latin1_data, pspro_latin1_func); 

  /* scale the coordinate system by SCALE so we can use integer arithmetic
     without losing accuracy */
  fprintf (stream, "1 %li div dup scale\n", SCALE);

  /* use the Postscript mechanism for duplicating pages, rather than using a flag to lpr.
     This mechanism means less data (I think!!) but it requires you to edit the file
     if you want to print a different number of copies later. It is so rarely used that
     it isn't really an issue anyway!! */
  fprintf (stream, "/#copies %i def\n", copies);

  /* rotate the page if using landscape mode, and set PW and PH to the page width and height
     as determined by the SCALE factor */
  if (landscape) {
    PW = papermetrics->PaperHeight * SCALE;
    PH = papermetrics->PaperWidth * SCALE;

    LM = PAGE_BOT_MARGIN * SCALE;
    RM = PAGE_TOP_MARGIN * SCALE;

    TM = PAGE_LEFT_MARGIN * SCALE;
    BM = PAGE_RIGHT_MARGIN * SCALE;

    fprintf (stream, "90 rotate %li %li translate\n",PAGE_LANDSCAPE_XOFFS, PAGE_LANDSCAPE_YOFFS - PH);
  } else {
    PH = papermetrics->PaperHeight * SCALE;
    PW = papermetrics->PaperWidth * SCALE;

    LM = PAGE_LEFT_MARGIN * SCALE;
    RM = PAGE_RIGHT_MARGIN * SCALE;

    TM = PAGE_TOP_MARGIN * SCALE;
    BM = PAGE_BOT_MARGIN * SCALE;
  }

   /* calculate the width of the "window" in which we draw text, and from that calculate the
     number of characters per line (CPL) */
  WW = (PW - (LM + RM) - ((columns - 1) * papermetrics->ColumnSep * SCALE)) / columns;
  CPL = (unsigned int)(WW / BFW);

  /* calculate the top position at which we start drawing text */
  StartY = PH - TM;

  /* adjust starting position if we are printing security classification */
  if (Classification != NULL || Classification_1st != NULL) {
    StartY -= classfontsize;
    ClassY = StartY;
  }

  /* adjust starting position if we are printing title */
  if (TitleEnabled) {
    if (GaudyFlag)  {
      TitleY = StartY;
      StartY -= (papermetrics->GaudyBoxHeight * SCALE) + BFH; /* StartY -= BH + BFH; */
    } else {
      StartY -= TFH;
      TitleY = StartY;
      StartY -= BFH;
    }
  }

  /* move down one line below for start of text */
  StartY -= BFH;

  /* calculate the last location on the page to be printing text */
  EndY = BM;
  if (Classification != NULL || Classification_1st != NULL) {
    ClassBottomY = BM;
    EndY += classfontsize;
  }

  /* define a variable for our body font, and calculate the character width for later use */
  fprintf (stream, "/BodyF { %li /%s /%s-Latin1 ChgFnt } def\n", BFH, bodyfont,
     bodyfont);
  fprintf (stream, "/CW BodyF ( ) stringwidth pop def\n");

  /* define variables for various other font used - title, gaudy page number, gaudy date, gaudy title */
  fprintf (stream, "/Titlef {  %li /%s /%s-Latin1 ChgFnt } def\n", TFH, titlefont,
     titlefont);
  if (GaudyFlag) {
    fprintf (stream, "/Gpnf {  %li /%s /%s-Latin1 ChgFnt } def\n", gaudyPNfontsize,
     gaudyPNfont, gaudyPNfont);
    fprintf (stream, "/Gdatef { %li /%s /%s-Latin1 ChgFnt } def\n",
     gaudydatefontsize, gaudydatefont, gaudydatefont);
    fprintf (stream, "/Gtitlef { %li /%s /%s-Latin1 ChgFnt } def\n",
     gaudytitlefontsize, gaudytitlefont, gaudytitlefont);
  }

  /* define procedures for drawing continuation line markers, continuation lines, normal lines, and performing indents */
  fprintf (stream, "/K         { -2 CW mul add exch moveto (+) show } def\n");
  fprintf (stream, "/L         { CW mul add exch moveto show } def\n");
  fprintf (stream, "/T         { exch moveto show } def\n");
  fprintf (stream, "/M         { CW mul 0 rmoveto } def\n");
  fprintf (stream, "/Centre    { dup stringwidth pop 2 div neg 0 rmoveto } def\n");

  /* define procedures for drawing gaudy page numbers, gaudy boxes, gaudy bars and gaudy titles */
  if (GaudyFlag) {
    fprintf (stream, "/Gb        { newpath moveto %li 0 rlineto 0 %li rlineto -%li 0 rlineto fill } def\n",
                                      papermetrics->GaudyBoxWidth  * SCALE,   /* BW */
                                      papermetrics->GaudyBoxHeight * SCALE,   /* BH */
                                      papermetrics->GaudyBoxWidth  * SCALE);  /* BW */
    fprintf (stream, "/Gr        { newpath moveto %li 0 rlineto 0 %li rlineto -%li 0 rlineto fill } def\n",
                                       PW - (LM + RM) - (2 * papermetrics->GaudyBoxWidth  * SCALE), papermetrics->GaudyBarHeight  * SCALE,
                                       PW - (LM + RM) - (2 * papermetrics->GaudyBoxWidth  * SCALE));
    fprintf (stream, "/G         { %s setgray %li %li Gb %li %li Gb %s setgray %li %li Gr } def\n",
					BOXGRAY,
					LM, TitleY - (papermetrics->GaudyBoxHeight * SCALE),                          /* pos of left box */
				  	PW - RM - (papermetrics->GaudyBoxWidth * SCALE), TitleY - (papermetrics->GaudyBoxHeight * SCALE),                /* pos of right box */
					BARGRAY,
					LM + (papermetrics->GaudyBoxWidth * SCALE), TitleY - (papermetrics->GaudyBoxHeight * SCALE));			   /* pos of bar */
  }

  /* define stuff for security strings */
  if (Classification != NULL || Classification_1st != NULL) {
    fprintf (stream, "/Classf {  %li /%s /%s-Latin1 ChgFnt } def\n", classfontsize, classfont, classfont);
    fprintf (stream, "/ClassString ");
    if (Classification != NULL) {
      PrintPSString (stream, Classification, strlen(classification));
    } else { 
      fprintf (stream, " () ");
    }
    fprintf (stream, " def\n");
    fprintf (stream, "/Class1String ");
    if (Classification_1st != NULL) {
      PrintPSString (stream, Classification_1st, strlen(classification_1st));
    } else if (Classification != NULL) {
      PrintPSString (stream, Classification, strlen(classification));
    } else { 
      fprintf (stream, " () ");
    }
    fprintf (stream, " def\n");
  }

  /* if we have titles enabled, define an appropriate procedure for drawing it */
  /* define the start page procedure used to start every page */

  fprintf (stream, "/StartPage { /SavedPage save def\n");
  if (Classification != NULL || Classification_1st != NULL) {
      fprintf (stream, "  Classf %li %li moveto 1 index (1) eq { Class1String } { ClassString } ifelse Centre 0 setgray show\n", PW / 2, ClassY);
      fprintf (stream, "  Classf %li %li moveto 1 index (1) eq { Class1String } { ClassString } ifelse Centre 0 setgray show\n", PW / 2, ClassBottomY);
  }

  if (TitleEnabled) {
    if (GaudyFlag) {
      fprintf (stream, "  G\n");                                                   /* draw boxes */
      fprintf (stream, "  Gtitlef %li %li moveto Centre 0 setgray show\n",   /* title */
                                         ((LM + (papermetrics->GaudyBoxWidth  * SCALE)) +
                                           (PW - RM - (papermetrics->GaudyBoxWidth  * SCALE))) / 2L,
                                         TitleY - (papermetrics->GaudyBoxHeight * SCALE) +
                                         (((papermetrics->GaudyBarHeight  * SCALE) * 7L / 10L) / 2L));
      if (title != NULL) {
        fprintf (stream, "%li %li moveto ",
                                         ((LM + (papermetrics->GaudyBoxWidth * SCALE)) + (PW - RM - (papermetrics->GaudyBoxWidth * SCALE))) / 2L,
                                         TitleY - (((papermetrics->GaudyBarHeight * SCALE) * 7L / 10L) / 2L));
        PrintPSString (stream, title, strlen(title));
        fprintf (stream, " Centre show\n");
      }
      fprintf (stream, "  Gpnf    %li %li moveto Centre 1 setgray show\n",   /* page number */
                                         PW - RM - ((papermetrics->GaudyBoxWidth * SCALE) / 2L),
                                         TitleY - ((papermetrics->GaudyBoxHeight * SCALE) / 2L) - gaudyPNfontsize * 7L / 20L);
      fprintf (stream, "  Gdatef  %li %li moveto (%s) Centre 0 setgray show\n",
					 LM + ((papermetrics->GaudyBoxWidth * SCALE) / 2L),
					 TitleY - ((papermetrics->GaudyBoxHeight * SCALE) * 3L / 5L) - gaudydatefontsize * 7L / 10L, tm_string);
      fprintf (stream, "                  %li %li moveto (%s) Centre show\n",
					 LM + ((papermetrics->GaudyBoxWidth * SCALE) / 2L),
					 TitleY - ((papermetrics->GaudyBoxHeight * SCALE) * 3L / 5L) + gaudydatefontsize * 7L / 10L, dt_string);
    } else {
      fprintf (stream, "  0 setgray Titlef %li %li moveto ", LM, TitleY);
      if (title != NULL) {
        fprintf (stream, "pop pop ");
        PrintPSString (stream, title, strlen(title));
        fprintf (stream, " show\n");
      } else {
        fprintf (stream, "show 8 M ");
        PrintPSString (stream, timestring, strlen (timestring));
        fprintf (stream, " show 8 M show\n");
      }
    }
  }
  fprintf (stream, "  BodyF 0 setgray } def\n");

  /* define end page procedure */
  fprintf (stream, "/EndPage   {");
  if (GaudyFlag && Columns == 2)
    fprintf (stream, " %li %li moveto %li -%li rlineto stroke ", LM + WW + (papermetrics->ColumnSep * SCALE / 2), StartY, 0L, StartY - EndY);
  fprintf (stream, "showpage SavedPage restore } def\n");

  /* end of the header */
  fprintf (stream, "%%%%EndProlog\n");
}

/********************************
   EndJob
 ********************************/

void EndJob (stream)

FILE *stream;

{
  /* indicate this is the end of the file */
  fprintf (stream, "%%%%Trailer\n");

  /* now we know how many pages there are!! */
  fprintf (stream, "%%%%Pages: %li\n", PageCount - 1);
}

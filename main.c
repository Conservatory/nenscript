/*
 *   $Id: main.c,v 1.3 1992/10/07 22:06:08 craigs Exp $
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
 *   handling for different classification banners for 1st page added by 
 *   Daniel Risacher (magnus@alum.mit.edu)
 *
 */

#include "machdep.h"
#include "defs.h"

#include "version.h"
#include "print.h"
#include "main.h"
#include "paper.h"
#include "postscri.h"

/********************************
  defines
 ********************************/

/* environment variables */
#define		PRINTER		"PRINTER"		/* sets name of printer */
#define		NENSCRIPT	"NENSCRIPT"		/* contains options */

/********************************
  exports
 ********************************/
char *progname;

/********************************
  usage
 ********************************/

void usage ()

{
  printf ("usage: %s -?12BGghlNnRrSsVWwZ -f font -F titlefont -b header -i filetitle -L lines -p filename -P printer -S classification -U classification_1st_page -T papertype -t tabstop -# copies file...\n", progname);
  exit (0);
}

/*********************************

variables set by options

 *********************************/

static int  landscape = False;			/* True if in ladnscape mode, i.e. -r specified */
static int  columns = 1;				/* number of columns to print with */
static char *outputfilename = NULL;			/* name of output file, if still NULL after parsing args then use lpr */
static char *bodyfont = NULL;			/* font to use for text */
static char *titlefont = NULL;			/* font to use for titles */
static int  wrap = True;				/* True if to wrap long lines */
static int  titlesenabled = True;			/* True if page titles enables */
static char *title = NULL;				/* set to title if specified */
static int  copies = 1;				/* number of copies of each page */
static int  gaudy = False;				/* enables gaudy mode */
static int  force_lines = 0;				/* force number of lines per page */
static char *classification_1st = NULL;			/* security classification for 1st page */
static char *classification = NULL;			/* security classification for body of document */
static int  line_numbers = False;                    /* true if to display line numbers */
static char *printername = DEF_PRINTER;		/* name of printer to use when output goes to lpr */
static int  tabstop = 8;			/* tab stops */
static int  passthroughflag = False;		/* if True, then check for %! to allow passthrough of raw postscript */
static char *filetitle = NULL;                  /* set file title */

#ifdef US_VERSION
static char *papertype = "US";
#else
static char *papertype = "A4";
#endif


/*
 * forward declarations if in ANSI mode
 */

#ifdef __STDC__
char * get_string  (char **);
void parse_options (int *, char ***);
void parse_env     (char *);


#endif


/********************************
  parse_options
 ********************************/

#define	ARGC	(*argc)
#define	ARGV	(*argv)

void parse_options (argc, argv)

int *argc;
char ***argv;

{
  char c;
  char *s;
  int i;

  /* parse the options */

next_argv:
 while (--ARGC > 0 && (*++ARGV)[0] == '-')
    while (c = *++ARGV[0])
      switch (c) {

/* -? : print help message */
        case '?' :
        case 'h' :
          usage ();
          break;

/* -1 : single column mode */
        case '1' :
          columns = 1;
          break;

/* -2 : double column mode */
        case '2' :
          columns = 2;
          break;

/* -#: set number of copies */
        case '#' :
          if (*++ARGV[0])
            copies = atoi (ARGV[0]);
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -# option requires argument\n", progname);
            else {
              --ARGC;
              copies = atoi (*++ARGV);
            }
          }
          if (copies < 1) {
            fprintf (stderr, "%s: illegal number of copies specified - defaulting to one\n", progname);
            copies = 1;
          }
          goto next_argv;
          break;

/* -b: set page title */
        case 'b' :
          if (*++ARGV[0])
            title = ARGV[0];
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -b option requires argument\n", progname);
            else {
              --ARGC;
              title = *++ARGV;
            }
          }
          goto next_argv;
          break;

/* -f : set font to use for printing body */
        case 'f' :
          if (*++ARGV[0])
            bodyfont = ARGV[0];
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -f option requires argument\n", progname);
            else {
              --ARGC;
              bodyfont = *++ARGV;
            }
          }
          goto next_argv;
          break;

/* -N : display line numbers */
        case 'N' :
          line_numbers = True;
          break;

/* -n : disable line numbers */
        case 'n' :
          line_numbers = False;
          break;

/* -p: send to file rather than lpr */
        case 'p' :
          if (*++ARGV[0])
            outputfilename = ARGV[0];
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -p option requires argument\n", progname);
            else {
              --ARGC;
              outputfilename = *++ARGV;
            }
          }
          goto next_argv;
          break;

/* -r : landscape mode */
        case 'r' :
          landscape = True;
          break;

/* -w : wrap mode */
        case 'w' :
          wrap = True;
          break;

/* -Z : check for postscript passthrough mode */
        case 'Z' :
          passthroughflag = True;
          break;

/* -B : disable page headings and disable gaudy mode */
        case 'B' :
          titlesenabled = False;
          break;

/* -F : set font to use for printing titles */
        case 'F' :
          if (*++ARGV[0])
            titlefont = ARGV[0];
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -F option requires argument\n", progname);
            else {
              --ARGC;
              titlefont = *++ARGV;
            }
          }
          goto next_argv;
          break;

/* -g : disable gaudy mode */
        case 'g' :
          gaudy = False;
          break;

/* -G : enable gaudy mode */
        case 'G' :
          gaudy = True;
          break;

/* -i: set file title when inputting from stdin */
        case 'i' :
          if (*++ARGV[0])
            filetitle = ARGV[0];
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -i option requires argument\n", progname);
            else {
              --ARGC;
              filetitle = *++ARGV;
            }
          }
          goto next_argv;
          break;

/* -l : don't set number of lines of page, i.e. use maximum */
        case 'l' :
          force_lines = 0;
          break;

/* -L : force number of lines per page */
        case 'L' :
          if (*++ARGV[0])
            force_lines = atoi(ARGV[0]);
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -x option requires argument\n", progname);
            else {
              --ARGC;
              force_lines = atoi (*++ARGV);
            }
          }
	  if (force_lines <= 0) {
            fprintf (stderr, "%s: ignoring illegal arguement to -L option\n", progname);
	    force_lines = 0;
          }
          goto next_argv;
          break;

/* -P: set printer name - overrides PRINTER environment variables */
        case 'P' :
          if (*++ARGV[0])
            printername = ARGV[0];
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -P option requires argument\n", progname);
            else {
              --ARGC;
              printername = *++ARGV;
            }
          }
          goto next_argv;
          break;

/* -R : portrait mode */
        case 'R' :
          landscape = False;
          break;

/* -U: set security classification for first page only*/
        case 'U' :
          if (*++ARGV[0])
            classification_1st = ARGV[0];
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -U option requires argument\n", progname);
            else {
              --ARGC;
              classification_1st = *++ARGV;
            }
          }
          goto next_argv;
          break;

/* -S: set security classification */
        case 'S' :
          if (*++ARGV[0])
            classification = ARGV[0];
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -S option requires argument\n", progname);
            else {
              --ARGC;
              classification = *++ARGV;
            }
          }
          goto next_argv;
          break;

/* -s : disable security classification printing */
        case 's' :
          classification_1st = NULL;
          classification = NULL;
          break;


/* -T: set paper size to either US or A4 */
        case 'T' :
          if (*++ARGV[0])
            papertype = ARGV[0];
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -T option requires argument\n", progname);
            else {
              --ARGC;
              papertype = *++ARGV;
            }
          }
          goto next_argv;
          break;

/* -t: set tab stop size */
        case 't' :
          if (*++ARGV[0])
            s = ARGV[0];
          else {
            if (ARGC < 1) {
              fprintf (stderr, "%s: -t option requires argument\n", progname);
	      s = 0;		/* We should flag the error in some way */
            } else {
              --ARGC;
              s = *++ARGV;
            }
          }
          if ((i = atoi (s)) <= 0) 
            fprintf (stderr, "%s: illegal tab setting %s ignored\n", progname, s);
          else
            tabstop = i;
          goto next_argv;
          break;

/* -V : print version number */
	case 'V' :
          fputs (version_string,   stderr);
          fputs ("\n",           stderr);
          /* fputs (copyright_string, stderr); */
          exit (0);
          break;

/* -W : turn off wrap mode */
        case 'W' :
          wrap = False;
          break;

/* single char option */
/*        case 'x' :
          allow_checkouts = True;
          break;
*/

/* option with string */
/*        case 'x' :
          if (*++ARGV[0])
            optionstring = ARGV[0];
          else {
            if (ARGC < 1)
              fprintf (stderr, "%s: -x option requires argument\n", progname);
            else {
              --ARGC;
              optionstring = *++ARGV;
            }
          }
          goto next_argv;
          break;
*/
        default :
          fprintf (stderr, "%s: unknown option %c\n", progname, c);
	  usage();
          break;
      }
}

/********************************
  get_string
    return the next string from the argument
 ********************************/

char * get_string (str)

char **str;

#define	STR	(*str)

{
  char *ret = NULL;
  int  in_dquote, in_squote, in_bquote;
  int  quote_count;

  /* skip leading whitespace */
  while (*STR == ' ' ||
         *STR == '\t')
    STR++;

  /* return if end of string */
  if (*STR == '\0')
    return NULL;

  /* collect characters until end of string or whitespace */
  in_dquote = -1;
  in_squote = -1;
  in_bquote = -1;
  quote_count = 0;
  ret = STR;
  while (*STR != '\0' &&
         (quote_count > 0 || (*STR != ' ' && *STR != '\t'))
        ) {
    switch (*STR) {
      case '"':
        in_dquote = -in_dquote;
        quote_count += in_dquote;
        break;

      case '\'':
        in_squote = -in_squote;
        quote_count += in_squote;
        break;

      case '`':
        in_bquote = -in_bquote;
        quote_count += in_bquote;
        break;

      case '\\':
        if (STR[1] != '\0')
          STR++;
        break;
    }
    STR++;
  }
  if (*STR != '\0') {
    *STR = '\0';
    STR++;
  }

  if (quote_count > 0)
    fprintf (stderr, "unmatched quote in %s environment variable\n", NENSCRIPT);

  return ret;
}


/********************************
  parse_env
    process the options specified in the supplied string
 ********************************/

void parse_env (env)

char *env;

{
  int argc;
  char *s;
  char **argv;

  if (env == NULL)
    return;

  /* insert a "fake" argv[0] which corresponds to the real
     argv[0] passed to main. The allows us to use same routine
     for handling the real arguments */
  argv = (char **)malloc (sizeof (char *));
  argc = 1;
  argv[0] = progname;

  /* split the single environment string into separate strings so we can pass
     them to parse_options */
  while ((s = get_string (&env)) != NULL) {
    argv = (char **)realloc ((void *)argv, (argc+1) * sizeof (char *));
    argv[argc] = s;
    argc++;
  }

  /* parse the options */
  parse_options (&argc, &argv);
}

/********************************
  passthrough
 ********************************/

static void passthrough (input, output)

FILE * input;
FILE * output;

{
  char buff [512];
  int  len;

  while ((len = fread (buff, 1, 512, input)) > 0)
    fwrite (buff, 1, len, output);
}


/********************************
  main
 ********************************/

int main (argc, argv)

int argc;
char *argv[];

{
  char *env;
  char cmd[1024];
  struct PaperMetrics * papermetrics;
  int i;
  int ch1;
  int ch2;


  FILE *outputstream;				/* opened output stream - either file or lpr */
  FILE *inputstream;				/* opened input stream, 0 if max */

  /* extract the program name */
  if ((progname = strrchr (argv[0], '/')) == NULL)
    progname = argv[0];
  else
    progname++;

  /* get the printer environment variable */
  if ((env = getenv (PRINTER)) != NULL)
    printername = env;

  /* handle the NENSCRIPT environment variable */
  if ((env = getenv (NENSCRIPT)) != NULL)
    parse_env (STRDUP(env));

  /* parse arguments */
  parse_options (&argc, &argv);

  /* open either the output file or a pipe to lpr */
  if (outputfilename != NULL) {
    if (strcmp (outputfilename, "-") == 0)
      outputstream = stdout;
    else if ((outputstream = fopen (outputfilename, "w")) == NULL) {
      perror (outputfilename);
      exit (1);
    }
  } else {
#ifdef MSDOS
    if ((outputstream = fopen (printername, "w")) == NULL) {
      perror (printername);
      exit (1);
    }
#else
    sprintf (cmd, "%s %s", LPR, printername);
    if ((outputstream = popen (cmd, "w")) == NULL) {
      perror (LPR);
      exit (1);
    }
#endif
  }

  /* can't be in passthrough mode with more than one argument */
  if (argc > 1 && passthroughflag) {
    fprintf (stderr, "%s: ignoring -Z flag as there is more than one argument", progname);
    passthroughflag = False;
  }

  /* if in passthrough mode, open the input stream and check for %! */
  if (passthroughflag) {
    if (argc < 1)
      inputstream = stdin;
    else if ((inputstream = fopen (*argv, "r")) == NULL) {
      perror (*argv);
      exit (1);
    }
    ch1 = fgetc (inputstream);
    ch2 = fgetc (inputstream);
    ungetc (ch2, inputstream);
    ungetc (ch1, inputstream);
    if (ch1 == '%' && ch2 == '!') {
      passthrough (inputstream, outputstream);
      exit (0);
    }
  } else
    inputstream = NULL;

  /* check the paper type and get ptr to record */
  papermetrics = NULL;
  for (i = 0; PaperTypes[i].Name != NULL; i++)
    if (STRICMP ((papermetrics = &PaperTypes[i])->Name, papertype) == 0)
      break;

  /* if the papermetric was not set, give error */
  if (PaperTypes[i].Name == NULL) {
    fprintf (stderr, "%s: paper type %s not known\n", progname, papertype);
    exit (1);
  }

  /* set up the fonts */
  if (bodyfont == NULL)
    bodyfont = (columns == 2 && landscape) ? papermetrics->LandscapeFont : papermetrics->PortraitFont;

  if (titlefont == NULL)
    titlefont = papermetrics->TitleFont;

  /* display the header */
  StartJob (outputstream,
            *argv,
            landscape,
            columns,
            bodyfont,
            titlefont,
            wrap,
            titlesenabled,
            title,
            copies,
            gaudy,
            force_lines,
            classification_1st,
            classification,
            papermetrics,
	    tabstop);

  /* if no arguments, then accept stdin */
  if (argc < 1)
    print_file (stdin, outputstream, (filetitle ? filetitle : "stdin"), line_numbers);
  else for (;argc > 0;argc--) {
    if (inputstream == NULL && (inputstream = fopen (*argv, "r")) == NULL)
      perror (*argv);
    else {
      print_file (inputstream, outputstream, (filetitle ? filetitle : *argv), line_numbers);
      fclose(inputstream);
      inputstream = NULL;
    }
    argv++;
  }

  /* output trailer */
  EndJob (outputstream);

  /* finished */
  return (0);
}

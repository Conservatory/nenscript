#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*****************************************************
 *
 *  MSDOS stuff 
 *
 *****************************************************/

#ifdef MSDOS

# include <ctype.h>
# include <string.h>
# include <malloc.h>

# define STRICMP(s1,s2) _stricmp (s1,s2)
# ifdef _WINDOWS
#  define	STRDUP(str)	_strdup(str)
# else
#  define	STRDUP(str)	strdup(str)
# endif

# define USERNAME	getenv("LOGNAME")!=NULL?getenv("LOGNAME"):getenv("USER")!=NULL?getenv("USER"):"Unknown")

# define	GETTIME(tm,str)	strftime(str,15,"%X",tm)

# ifdef US_VERSION
#  define 	GETDATE(tm,str) strftime(str,15,"%b %d %y",tm)
# else
#  define	GETDATE(tm,str) strftime(str,15,"%d %b %y",tm)
# endif

#else

/*****************************************************
 *
 *  Unix stuff
 *
 *****************************************************/

# include <pwd.h>
# include <string.h>
# ifdef __STDC__
#  include <unistd.h>
# endif

# define	LPR		"lpr -P "		/* spooler with option to set name of printer */

# define	STRICMP(s1,s2)	strcasecmp(s1,s2)
# ifdef ultrix
#  define	STRDUP(str)	strcpy(malloc(strlen(str)+1), str)
# else
#  define	STRDUP(str)	strdup(str)
# endif

# define	USERNAME	((getpwuid (getuid()))->pw_name)

# define 	GETTIME(tm,str)	strftime(str,15,"%T",tm)

# ifdef US_VERSION
#  define	GETDATE(tm,str) strftime(str,15,"%h %d %y",tm)
# else
#  define	GETDATE(tm,str) strftime(str,15,"%d %h %y",tm)
# endif

#endif


/*
 *   $Id: defs.h,v 1.2 1992/10/02 01:02:32 craigs Exp $
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

 */

 /* common definitions */

#ifndef True
#define True	1
#define False	0
#endif

#ifndef MIN
#define MIN(a,b) ((a)<=(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>=(b)?(a):(b))
#endif


/* default printer */
#ifdef MSDOS
#define DEF_PRINTER "prn"
#else
#define	DEF_PRINTER	"PostScript"		/* default printer */
#endif


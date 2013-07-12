#include "machdep.h"

#include "paper.h"

static char CourierBold10[]   = "Courier-Bold10";
static char Courier10[]       = "Courier10";
static char Courier7[]        = "Courier7";
static char HelveticaBold36[] = "Helvetica-Bold36";
static char TimesBold12[]     = "Times-Bold12";
static char TimesRoman14[]    = "Times-Roman14";
static char HelveticaBold28[] = "Helvetica-Bold28";


/* This table defines all of the possible paper types */

struct PaperMetrics PaperTypes[] = {

{  "a4",
			842L,				/* paper height = 297 mm */
			595L,				/* paper height = 210 mm */

 			CourierBold10,		/* non-gaudy title font */
 			Courier10,			/* portrait text font */
 			Courier7,			/* landscape text font */

			HelveticaBold36,	/* gaudy mode page number font */
			TimesBold12,		/* gaudy mode date font */
			TimesRoman14,		/* gaudy mode title font */
			HelveticaBold28,	/* classification herder/footer font */

			72L,				/* width of boxes in gaudy mode */
			36L,				/* height of boxes in gaudy mode */
			18L,				/* height of bar in gaudy mode */

			36L				/* column separation */

},

{  "us",
			792L,				/* paper height = 11 inches */
			612L,				/* paper width = 8.5 inches */

 			CourierBold10,		/* non-gaudy title font */
 			Courier10,			/* portrait text font */
 			Courier7,			/* landscape text font */

			HelveticaBold36,	/* gaudy mode page number font */
			TimesBold12,		/* gaudy mode date font */
			TimesRoman14,		/* gaudy mode title font */
			HelveticaBold28,	/* classification herder/footer font */

			72L,				/* width of boxes in gaudy mode */
			36L,				/* height of boxes in gaudy mode */
			18L,				/* height of bar in gaudy mode */

			36L				/* column separation */

},

{  NULL,
			0L, 0L, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0L, 0L, 0L, 0L
}

};


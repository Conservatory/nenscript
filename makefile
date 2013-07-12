#
# Makefile 
#
# $Id: Makefile,v 1.1 1992/10/02 01:02:32 craigs Exp $
#

OBJ		= .o

PROG		=	nenscript
MAN		=	$(PROG).1
SOURCES		=	main.c postscri.c print.c fontwidt.c font_lis.c paper.c
OBJECTS		=	$(SOURCES:.c=.o)

INSTALLDIR	=	/usr/local
BININSTALLDIR	=	$(INSTALLDIR)/bin
MANINSTALLDIR	=	$(INSTALLDIR)/man/man1

LOCALINSTALLDIR		=	$(HOME)/bin
LOCALMANINSTALLDIR	=	$(HOME)/man/man1

#
# uncomment macro below for debug version
#
#DEBUG	= -g

#
# US_VERSION selects default US paper format as well as selecting US format date
#
CFLAGS	=	-O $(DEBUG) -DUS_VERSION
LFLAGS	=	

CHMOD	=	chmod
CP	=	cp
RM	=	rm
#INSTALL	=	/etc/install
INSTALL	=	/usr/ucb/install -c

all debug:	$(PROG)

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) 

main.$(OBJ):	main.c

install: $(PROG)
	$(INSTALL) -s -m 555 $(PROG) $(BININSTALLDIR)
	$(INSTALL) -m 444 $(MAN) $(MANINSTALLDIR)

install.man: $(MAN)
	$(INSTALL) -f $(MAININSTALLDIR) -m 444 $(MAN)

install.local:	$(PROG) $(MAN)
	$(INSTALL) -s -m 555 $(PROG) $(LOCALINSTALLDIR)
	$(INSTALL) -m 444 $(MAN) $(LOCALMANINSTALLDIR)

clean:
	rm -f *.o *.obj *.exe *.mdt $(PROG)

font_lis.c: machdep.h defs.h font_lis.h main.h

fontwidt.c: machdep.h defs.h fontwidt.h main.h

main.c: machdep.h defs.h version.h postscri.h print.h main.h paper.h

paper.c: machdep.h paper.h

postscri.c: machdep.h defs.h paper.h postscri.h fontwidt.h font_lis.h main.h

print.c: machdep.h defs.h print.h postscri.h main.h

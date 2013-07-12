nenscript: Introduction & Historical Notes
==========================================

nenscript is a converter from ASCII text to PostScript.  It is in the public domain, and this version of it (the latest we know of) was made available to the public by Daniel Risacher on 28 October 1997.  His July 2013 message to the mil-oss group explains this history:

        From: Daniel Risacher
        To: mil-oss (http://groups.google.com/group/mil-oss)
        Subject: Re: [mil-oss] Posting gov code to personal GitHub
        Date: Wed, 10 Jul 2013 17:08:22 -0400
        Message-ID: <CAAD1OeKuU8gbPwSZAvPjACP+HYuTmXjz=+FYrMCXU6CwshGj2w@mail.gmail.com>
        
        I could point out a few other cases where gov't code has been posted
        to personal space [...]
        
        One easy example that comes to mind is that I wrote patches to
        nenscript (and thus became the de-facto maintainer) when I was a 2LT.
        The code is published today at http://risacher.org/nenscript/
        
        The changes were made in response to a trouble report for the E-3
        AWACS - and include this amusing (to me) comment:
        
            I added the -U classification_1st_page flag to nenscript,
            because I needed it for HDSIUCS (Hard Disk Subsystem Interface
            Unit Control Software). Since someone else might need this
            feature also, I am releasing my changes into the public domain
            also.
            
            I made trivial modifications to the options handling to provide
            marginally better diagnostics.
            
            I also renamed the package nenscript-1.13.3, because I think
            1.13++ is a poor choice of version number.
            
            These changes were made on my own time and on my own computer,
            but could easily be construed as being part of my official
            duties as an AWACS software programmer/analyst. If this is the
            case, then any changes that I made are a work of the US
            government and are not subject to copyright protection in the
            United States, and furthermore are provided, free of charge,
            with no warantee. If my changes are not legally part of my
            official duties, then I hereby disclaim all rights to the
            aforementioned changes and explicitly put them in the public
            domain, and furthermore disclaim any warantee, express or
            implied.  I am not an intellectual property lawyer, so I'm not
            sure which of these situations applies. Either way, the changes
            are free to you.
            
            Daniel Risacher, 2Lt, USAF 
            28 Oct 1997
  
The rest of this file is the original ``readme`` file from the nenscript source tree as downloaded from Risacher's site (see URL above), with minor formatting changes to make it Markdown-compatible.
  
Original ``readme`` from nenscript 1.13.3
=========================================

$Id: readme,v 1.3 1992/10/07 22:06:08 craigs Exp $

README for nenscript version 1.13
Craig Southeren 2-October-1992
---------------------------------

This directory contains the sources for nenscript, a clone of the
proprietary enscript program from Adobe Systems. For those who have
never used enscript, it is a good ASCII to Postscript converter. 

This the second version of nenscript I have released. The first
version (1.12 as of 20 August 1992) was received well, but it really
needed a few changes. Thanks to everyone who sent in suggestions
and code fragments.

nenscript was written by me over a period of several months whilst
I was working at Computer Sciences of Australia as a contractor. 
As a regular net-watcher, I saw that several people had (over the
years) asked for a public domain version of enscript. Seeing as I
had got tons of useful stuff off the net, I though this looked like
a good opportunity to pay back the net community. So I convinced
my manager that this would be a Good Thing.

The main features of nenscript are:

  * produces Postscript output which fully conforms to
    the Document Structuring Conventions
  * support for normal and "gaudy" output
  * support for single or double column output
  * allows insertion of titles and headers in any font.
  * multiple copies of a document

Features additional to nenscript are:

  * automatic wrapping of long lines
  * availability under MSDOS 
  * executable is self contained - no additional files required

The main feature that is NOT supported by nenscript is the ability
to use fonts other than Courier for text output. This would require
access to full font tables which I don't have have.

New for version 1.13
--------------------------
The most important change is the support for additional paper sizes through
the -T option. At the moment, only US and A4 paper sizes are supported, but
others can be added easily by inserting them into the table in paper.c.
Support for changeable tab stops has also been added on the -t option.
A simple script called pstext allows nenscript to be used as replacement
for the Adobe pstext program with print spooling systems such as CAP.

Notes on 1.12
-------------
nenscript has been extensively used for over six months on a Sun-3/Sun-4
network with over 50 users. No major problems are currently outstanding.
This version is identical with the addition of support for MSDOS.

Installation
------------

To install, first select whether you want the US version or not. If the
define US_VERSION is defined, then the default paper size is US, and
the date is in US format.

For Unix:

        The makefile provided will install nenscript in /usr/local/bin, with
        the man page in /usr/local/man/man1. This makfile works
        unchanged on a SIlicon Graphics Personal Iris 35D running Irix 4.0.2
        and on a Sun SparcStartion II with SunOS 4.1.2

For MSDOS:

        I've lost my ability to compile under MSDOS, so I can't claim to
        have tested this. But the previous version DID compile, and this
        one should with little or no modifications.
	

Features I would like to add:
-----------------------------

Please send me your suggestions. I can't guarantee that I will do anything
about them but if you don't tell me, I definitely won't!!!

1. Support for the ISO-Latin character set. At the moment, characters are
   just sent down to the printer.

2. Binary file detection. enscript attempts to detect binary files and will
   will reject them (unless forced to print)

3. Display of control characters. Control characters other than ^L (page
   break), ^I (tab) and newline are not trapped.

4. Support for the IBM PC character set, and perhaps some of the other
   display code pages.


If you want to modify nenscript...
----------------------------------

The paper size definitions are now in a table in paper.c, so
adding new paper sizes should be easy. Most of the machine dependent
stuff is now in machdep.h, so porting to new machines should be
easier.

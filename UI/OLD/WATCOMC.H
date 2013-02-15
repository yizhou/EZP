
	       The WATCOM C/C++ Programmer's FAQ
 
 
WW	  WW   AA   TTTTTTTT CCCC    OOOO      MM  MM	 cccc	/
WW	  WW  AAAA     TT   CC	CC  OO	OO    MMMMMMMM	cc     /
WW	  WW AA  AA    TT  CC	   OO	 OO  MM MMMM MM  cccc /
 WW	 WW  AA  AA    TT  CC	   OO	 OO  MM  MM  MM      /
 WW  WW  WW AA	  AA   TT  CC	   OO	 OO MM	 MM   MM    /
 WW WWWW WW AAAAAAAA   TT  CC	   OO	 OO MM	 MM   MM   / cccc   +	+
  WWWWWWWW AA	   AA  TT   CC	CC  OO	OO  MM	 MM   MM    cc	   +++ +++
   WW  WW  AA	   AA  TT    CCCC    OOOO   MM	 MM   MM     cccc   +	+
 
			  Programmer's
 
			  
     FFFFFFFFFFFFFFFFFF      AAAAA		 QQQQQQQQQQQQQQ
     FFFFFFFFFFFFFFFFFF     AAAAAAA	       QQQQQQQ	   QQQQQQ
     FFFFFF		   AAAAAAAAA	      QQQQQQ	     QQQQQQ 
     FFFFFF		  AAAAAAAAAAA	     QQQQQQQ	     QQQQQQ 
     FFFFFFFFFFFFFF	 AAAAAA AAAAAA	     QQQQQQQ	     QQQQQQQ
     FFFFFFFFFFFFFF	AAAAAA	 AAAAAA      QQQQQQQ	     QQQQQQQ
     FFFFFF	       AAAAAA	  AAAAAA     QQQQQQQ	 QQQ QQQQQQQ
     FFFFFF	      AAAAAAAAAAAAAAAAAAA     QQQQQQQ	QQQQQQQQQQQ 
     FFFFFF	     AAAAAAAAAAAAAAAAAAAAA     QQQQQQQQQQQQQQQQQQQ
     FFFFFF	    AAAAAA	     AAAAAA	  QQQQQQQQQQQQQQQQQQQQ 
							       QQQQQQQQQQ


Written by Paul Hsieh
qed@chromatic.com
Revision 3.91
December 11, 1996

(c) Copyright 1996, Paul Hsieh. All Rights Reserved.
This document may be freely distributed in any format desired so long as
the contents are not modified beyond non-obtrusive formatting.

Please note that although I've made a reasonable effort to verify the
material contained in this FAQ, I make no guarantees.  The things written
here are as true as I know them to be and should not be interpreted as
anything more.	None of WATCOM, PowerSoft/Sybase or TenBerry systems or
any other relevant company has officially endorsed this FAQ.  Furthermore,
I have no affiliation with WATCOM, PowerSoft/Sybase or TenBerry systems.

WATCOM, PowerSoft, Sybase, TenBerry Systems, QNX, IBM, OS/2, Microsoft,
Windows, Win32, Direct Draw, Direct X, Intel, Intel's RDX, Metaware,
Symantec, Borland, CauseWay, Fastgraph, Varmint's Audio Tools, YackIcons,
WordUp Graphics Toolkit, PC Magazine, NuMega, Bounds Checker, Soft ICE,
Pharlap, PModeW etc., are copyrights/trademarks of their respective
owners.

---------------------------------------------------------------------------- 


Introduction

Most of the questions dealt with here assume that you already own a
version of the WATCOM C/C++ compiler.  Ordering information is available
on WATCOM's world wide web site.

As my personal strengths are in DOS coding, I have slanted this FAQ at 
DOS programming using WATCOM C/C++.  If you are willing to contribute
information regarding Windows, OS/2 programming or QNX programming please
send it to me and I will include it.  Please note that I will only be able
to easily verify DOS information (and Windows information, only with great
difficulty.)

This FAQ mixes advocation with technical information.  It has been written
this way because after years of posting on USENET this has become my
natural style of writing.

----------------------------------------------------------------------------


Frequently Asked Questions
~~~~~~~~~~~~~~~~~~~~~~~~~~

These are very commonly asked questions that people ask about WATCOM C/C++.

Q01. What is WATCOM C/C++?  What does WATCOM C/C++ come with?
Q02. Where are WATCOM C/C++'s related web/ftp sites?
Q03. Why is WATCOM C/C++ so popular?
Q04. What's the syntax for ____?  How do I use the ____ tool?  How do I 
     make the code generator do ____?  I just don't get it.
Q05. I think there is a bug in the compiler, what should I do?
Q06. Why do so many games use the DOS4GW.EXE DOS extender?
Q07. What libraries are available for WATCOM C/C++?
Q08. How do I program for Direct X using WATCOM C/C++?
Q09. How do I program for Intel's RDX using WATCOM C/C++?
Q10. Is the code WATCOM C/C++ produces the fastest/tightest?
Q11. How do I use WATCOM's tools to help me optimize my code?
Q12. How does WATCOM C/C++ compare with other C compilers?
Q13. How do I debug DOS4GW apps under Windows 95?
Q14. How do I debug WATCOM apps using Soft-ICE or Bounds Checker?
Q15. How do I enable virtual memory for my DOS4GW application?	How do I
     know exactly what interrupts dos4gw handles all by itself?  How do I
     know exactly what I have to do to use dos interrupts from protected
     mode?
Q16. Why doesn't WATCOM use "_asm" for inline assembly like Microsoft and
     Borland?  After all isn't that standard?
Q17. Why does my code work only when I compile with debugging info on?
Q18. What is FLAT memory model?  What is the advantage of a 32 bit
     compiler such as WATCOM C/C++ over 16 bit compilers?  What are the
     differences?  Will I be able to move my 16 bit DOS code straight over 
     to 32 bit?
Q19. I am getting this weird error message ...
Q20. I am having malloc troubles.  It seems that I am trashing the heap 
     but I don't know how to debug it.
Q21. How do I write directly to graphics memory?
Q22. How do I install a mouse event handler?
Q23. What is DPMI and what role does it play in using WATCOM C/C++?  How
     do I communicate between the 16 bit world and 32 bit world on my PC?
Q24. What if I really need to compile a 16 bit model program?
Q25. Why do WATCOM's results and Visual C++'s results differ on the same
     source code?  Why does WATCOM implement the default signedness of
     chars different from everyone else?  I am having difficulty porting
     code to WATCOM C/C++.
Q26. Can I link external assembly files together with my C files?
Q27. How do #pragma's work?  How do I use inline assembly language?
Q28. How do I do compiled bitmaps?  How do I do on-the-fly generated
     code?
Q29. How do I install a 32 bit interrupt vector?  How do I install a
     bimodal interrupt?
Q30. How do I get rid of the DOS4GW banner?  How do I bind DOS4GW.EXE to
     my application?  How do I get rid of the external DOS4GW.EXE
     altogether?  What other DOS extenders can I use?
Q31. How do I make WATCOM C/C++ work with the latest OS/2 Toolkit?
Q32. Are there any other WATCOM C/C++ caveats?
Q33. Will WATCOM C/C++ support MMX?
Q34. So what is your story?  Why did you do this?

Cool contributions
~~~~~~~~~~~~~~~~~~

These are contributions sent to me that do not correspond to a frequently
asked question such as above, but which nevertheless are worth including.

C01. Profiling and debugging all rolled into one using an external
     terminal connected by serial.  Contributed by Charlie Wallace.
C02. Convert WATCOM's help file format to a text file for easier
     reading and potentially for easier manipulation.

----------------------------------------------------------------------------


Questions with answers
~~~~~~~~~~~~~~~~~~~~~~

Q01. What is WATCOM C/C++?  What does WATCOM C/C++ come with?

A01. WATCOM C/C++ is an Intel x86 based ANSI C++ and C compiler.  It
supports various target operating systems.  Its most notable and exercised
strength is its 32 bit DOS based support.  WATCOM has established its
reputation as the makers of a world leading C compiler and has been in
business for about 10 years.  You can get a more complete description of the
compiler on the WATCOM web site, however here are the most notable things
that are included:

  - 32 and 16 bit compilers, with standard libraries and DOS4GW.EXE.
  - Additional DOS graphics and x86 specific libraries.
  - Library manager and linker.
  - All flavors of Windows programming including 16 and 32 bit MFC support
    as well as Win32.  (Includes Windows SDK help.)
  - Numerous Windows development tools (dialogue editors and so on.)
  - Support for Dos extenders, OS/2, QNX, AutoCAD and Novell NLMs.
  - IDE: Editor, Debugger, Execution Sampler/Profiler, Make, Touch.
  - Online documentation and sample code.

----------------------------------------------------------------------------


Q02. Where are WATCOM C/C++'s related web/ftp sites?

A02. These are the URLs that I am aware of:

WATCOM's home page
    http://www.powersoft.com/products/languages/watccpl.html
WATCOM C/C++ readmes
    ftp://ftp.powersoft.com/pub/c_cpp/maintfls
WATCOM C/C++ goodies
    ftp://ftp.powersoft.com/pub/c_cpp/general
WATCOM C/C++ Technical documentation
    http://www.powersoft.com/services/techdocs/c_cpp.htm
Tenberry Software, Inc.
    http://www.tenberry.com/
KNUT's WATCOM C/C++ tutorial
    http://home.sn.no/home/oruud/wat_tut1.htm
    http://home.sn.no/home/oruud/wat_tut2.htm
X2FTP site:
    ftp://x2ftp.oulu.fi/pub/msdos/programming/watcom
Official site for the WATCOM C/C++ FAQ
    http://www.geocities.com/SiliconValley/9498/watfaq.txt
FTP site for downloading the WATCOM C/C++ FAQ
    ftp://x2ftp.oulu.fi/pub/msdos/programming/watcom/watfaq21.zip
Miscellaneous WATCOM C/C++ and OS/2 device driver notes
    http://www.wdi.co.uk/os2dd/QA__0009.htm

Email List server for WATCOM users
    WATCOM-L@umdd.umd.edu 
	 send a message to listserv@umdd.umd.edu with the first line
	 of your message containing:
	 "SUBSCRIBE WATCOM-L John Doe" to subscribe.

USENET Newsgroups
    news:alt.msdos.programmer
    news:comp.os.msdos.programmer
    news:rec.games.programmer

For tutorials on C/C++ (not specifically WATCOM) I have the following URLs:

C tutorials
    http://www.strath.ac.uk/CC/Courses/NewCcourse/ccourse.html
    http://www.iftech.com/oltc/c/c0.stm
    ftp://x2ftp.oulu.fi/pub/msdos/programming/docs/teach-c.zip
C++ tutorial
    http://www.iftech.com/oltc/cpp/cpp0.stm

----------------------------------------------------------------------------


Q03. Why is WATCOM C/C++ so popular?

A03. WATCOM C/C++ is a very good tool.	It was the first pure 32 bit C/C++
compiler widely available for x86 platforms.  It is a cross platform, high
performance object code generator with no shortage of support tools.  It
is on these merits that WATCOM has sustained its popularity.

Somewhere around 1992-93, WATCOM decided to drop the price (to a mere $400 
Cdn vs. the old price of $1200 US) and started advertising more heavily in 
magazines such as BYTE and PC Magazine.  Around this time I was heavily 
advocating it (version 9.0) it in newsgroups such as "rec.games.programmer" 
as the premiere C compiler for high performance and reliability, making such
bold statements as: "WATCOM C/C++ will produce code which is at *least* 
twice as fast as your current 16 bit compiler, and more typically around 
five times as fast" which was not stretching the truth as far as I knew it. 
Flame wars ensued where the focus was not only Borland and Microsoft, but 
WATCOM.  

Then a single program was introduced that brought immediate and serious 
attention to this compiler: DOOM.  What is this magical "DOS4GW" thing that 
makes DOOM so good?  Were the DOOM programmers serious when they said they 
used almost no assembly language?  What C compiler could they have possibly 
used that could pump that much game onto the screen?  Soon, more and more
games were made, hailing the telltale "DOS4GW" banner.	A few years later
PC Magazine ran a comparative study of a collection of C compilers.  They
hailed WATCOM C/C++ v9.5 for producing the fastest code (by a *wide* margin)
but criticized it for its klunky command line driven interface.

Since then, many game companies switched and dozens of titles appeared with
the "DOS4GW" banner displayed proudly including:  Raptor, System Shock,
Descent, Magic Carpet, NASCAR Racing, Terminal Velocity etc.  32 bit was
finally becoming a reality on the PC, and it had nothing to do with
"Windows" or "OS/2".  But even IBM was getting smart.  Numerous OS/2
utilities and parts of the OS itself were compiled using WATCOM C/C++.

With version 10.0 WATCOM introduced an IDE, added support for MFC and added
an assembler.  With 10.5 they decided to out source the new and improved IDE
to an award winning third party developer, included better support for Win
95 and have built in a higher degree of Visual C++ compatibility.  With its
licensing of MFC and ability to do ... well just about anything, WATCOM has
clearly set its sights on one goal:  To become the defacto standard C/C++
compiler for PCs. Even Microsoft dares not ignore WATCOM C/C++; "Direct X"
is compatible with WATCOM C/C++ (this was no doubt at the demand of the 3rd
party BETA testers.)

----------------------------------------------------------------------------


Q04. What's the syntax for ____?  How do I use the ____ tool?  How do I
make the code generator do ____?  I just don't get it.

A04. The first place to look when trying to deal with general questions
is the online help.  In a DOS box you can access the help via:

WHELP help_file [topic_name]

If you've installed the compiler to use the CDROM, be sure to have the
WATCOM CDROM in the CDROM drive.  It takes a little digging but there is a
lot of online information available.

If you give no parameters, most of the tools will respond with proper
command line syntax.  (Just type "wcl386" at the command to see this.)

Additionally, the file %WATCOM%\DOS4GW.DOC gives you information on
configuring DOS4GW.EXE for certain systems and for using virtual memory
under DOS.

Remember, that the WATCOM C/C++ compiler was not designed with beginners in
mind.  If you are just starting to learn C/C++ then I is suggest you get
plenty of reading on C/C++ in general and OS-specific programming manuals
(I.e., DOS, Windows, OS/2, QNX, or whatever target platform you are
interested in building for).

For a deeper understanding of WATCOM C/C++ you'll need to learn at least
bits and pieces about what object files, libraries, dll's, makefiles, and
protected mode are.  Experienced programmers find that WATCOM C/C++ doesn't
have any real brick walls that other compilers have, but that coaxing it to
do what you want it to do often takes some patience and perseverance.  That
is the issue this FAQ is trying to address.

In general your pursuit of information when dealing with difficulties using
the WATCOM C/C++ compilers should be in the order of:  (1) Check the online
help files, (2) Check reference materials such as DOS or Windows programming
books, Watcom's online technical documentation (the web address is given
above) as well as this FAQ, (3) Post a question to the internet (including
the USENET newsgroups, or the WATCOM listserver listed above) (4) Contact
WATCOM directly.

----------------------------------------------------------------------------


Q05. I think there is a bug in the compiler, what should I do?

A05. You'd be surprised by the number of "compiler bugs" people think
they've found.  Stop.  Take a deep breath and think about it for a moment.
The WATCOM C/C++ compiler is generally a very stable tool.  Is your code
proper ANSI C?	Are you violating the optimization assumptions assumed by
your choice of compiler switches?  (Aliasing is the only one really at
issue.)  Of the twenty or so "compiler bugs" that I thought I found, only
two of them were ever even close to being real compiler bugs (see "Q32.
Are there any other WATCOM C/C++ caveats?"  Numbers 3 and 6.)

The fact that something works on one compiler but not another is not
necessarily an indication that one compiler is not functioning properly.
For example, another compiler may not buffer writes to stdout via printf.
So the fact that printf("Hello world"); echos immediately on one compiler
and not at all on another (WATCOM C/C++ buffer's writes, so indeed its an
example of a compiler that would not output anything) is not an indication
that one or the other compiler is broken.

In general, you are supposed to call them and ask them for assistance, but
these days, with their compiler being as popular as it is, it may be
difficult to get satisfactory responses from their customer support.  You
may first try your luck on the newsgroups listed at the top of this FAQ,
if you have USENET access.

Powersoft/WATCOM themselves have set up a WWW bug report service on their 
web page where you can submit problem reports.	In general, if they think
your bug is nontrivial, then they will ask you to send them sample code
which isolates the problem.  WATCOM provides a "shroud" utility that will
obfuscate your code if you are worried about unwanted distribution of your
sources.  It is available at the following URL:

ftp://ftp.powersoft.com/pub/c_cpp/general/shroud.zip

However, I would highly recommend that if feasible, you simply duplicate
the problem in a clean project that does not expose any sources that you
don't want exposed.

Calling them for assistance should be a last resort, but if you feel that
you have found a genuine bug and have exhausted all reasonable avenues
then you should not hesitate to tell them about it.  When and if they fix
the problem, the compiler will be a better tool for all of us.

When there are bugs in the compiler that WATCOM is aware of and have
fixed, they usually provide a patch that is available from their www and
ftp sites (listed above.)

----------------------------------------------------------------------------


Q06. Why do so many games use the DOS4GW.EXE DOS extender?

A06. DOS4GW.EXE is the royalty free redistributable 32 bit DOS extender
that comes with WATCOM C/C++.  DOS4GW.EXE is made by "Tenberry Software"
(formerly Rational Systems) specifically for WATCOM.  Although alternative
DOS extenders with more/different features exist, users of WATCOM C/C++
for DOS programming (that is to say, game programmers) will tend to use
DOS4GW by default.  (It also has brand name recognition which may lead
some programmers to desire the telltale DOS4GW banner.)

----------------------------------------------------------------------------


Q07. What libraries are available for WATCOM C/C++?

A07. I don't have a comprehensive list, but I know that Varmint's Audio
Tools, Fast Graph, YackIcons, WordUp Graphics Toolkit, ZSVGA, Microsoft's
Direct X based Games SDK and Intel's RDX are notable libraries that come
in WATCOM C/C++ friendly flavors.  In their latest versions, WATCOM has
really gone out of their way to make themselves more compatible with
Visual C++.  Although I'm told the C++ name mangling and default calling
conventions are different, standard C libraries appear to be compatible.
(Look at WATCOM's discussion on "#pragma's" and you can decide for
yourself which calling convention you like.  Then when you do, you can set
it up using WATCOM C/C++; a trick not similarly possible with Visual C++.)
Needless to say there is no shortage of libraries out there that support
WATCOM C/C++.  The WATCOM web site lists numerous 3rd party developers
that are supporting their C/C++ compiler.

Here's a list of some stuff available for WATCOM C/C++:

Graphics libraries

WGT Watcom     ftp://x2ftp.oulu.fi/pub/msdos/programming/wgt51.zip 
Xlib Watcom    ftp://x2ftp.oulu.fi/pub/msdos/programming/xlib/xlib06p2.zip 
Watcom ModeX   ftp://x2ftp.oulu.fi/pub/msdos/programming/watcom/w_modex.zip
JLIB	       ftp://x2ftp.oulu.fi/pub/msdos/programming/djgpp2/jlib_1-6.zip
Intel's RDX    http://www.intel.com/ial/rdx/
Zephyr's ZSVGA http://www.phoenix.net/~balkum/products/zpmsvga.html
gkit05b.zip    ftp://x2ftp.oulu.fi/pub/msdos/programming/watcom/gkit05b.zip
SSE	       http://home.istar.ca/~genesoft/SSE1Demo.ZIP
 
Audio libraries

VAT Watcom     ftp://x2ftp.oulu.fi/pub/msdos/programming/mxlibs/vatpm061.zip 
SMIX Watcom    ftp://x2ftp.oulu.fi/pub/msdos/programming/mxlibs/smixw125.zip 
DSIK Watcom    ftp://x2ftp.oulu.fi/pub/msdos/programming/mxlibs/dsik205.zip 

----------------------------------------------------------------------------


Q08. How do I program for Direct X using WATCOM C/C++?

A08  I am not a Windows programming expert but the following information
appeared on the USENET:

  >How well does the Games SDK work with Watcom? If well, how many
  >trials did it take to get the 'magic options' set up right?

  Works well, so far. I did have to use the windows.h file supplied with
  the Games SDK, and that was hidden somewhere under the SAMPLES
  subdirectory. I also had to define a couple of macros to get the samples
  to compile. The whole thing took about 4 hours.

But then Ed Mueller said:

  With 10.6, you no longer need to include windows.h supplied with the 
  Games SDK.  The macros you need to define are NOIME and NOMCX.

And Marty (an employee of WATCOM) said:

  In response to questions regarding the WATCOM C/C++ compiler and DirectX:

  Watcom C/C++ 10.6 and DirectX 2.0

  FYI:
  The newly released DirectX 2.0 CD has a WATCOM directory that contains a
  zip file that will make all of the necessary changes to the sample files
  to work with Watcom C/C++ version 10.6.  Once these changes are made,
  the makeallw.bat batch file can be run to make the sample programs.

  See the readme file in the WATCOM directory of the DirectX 2.0 CD for
  more information.

"Avocado Green" wrote the following on the WATCOM List server:

  Here are some simple steps in using the Watcom IDE (10.6) to compile
  and run the DirectDraw example (DDEX3) included with the DXSDK.  I
  thought I'd save someone a few hours from messing with the make files.

  0.  Run the Watcom IDE (run it from the DOS box command line if it'd
      make you feel less guilty.)

  1.  Select menu option "File->New Project"
      Select the C:\DxSDK\Samples\DDex3 directory and enter
      "ddex3.wpj" as your project name
      Note:  If you installed the DirectX SDK in another directory
	     other than C:\DxSDK, make the changes as appropriate.

  2.  Enter "DDex3" as the target name,
      "Win32" as Target Environment, and
      "Windowed Executable [.exe]" as the Image Type.

  3.  Select menu option "Targets->Target Options->Windows Linking Switches"
      Go to "2. Import, Export and Library Switches",
      In field "Library directories(;):[libp]" enter "..\..\lib"
      In field "Libraries(,):[libr]" enter "ddraw.lib"

  4.  Add these source files into your project:
	 ..\misc\ddutil.cpp
	 ddex3.cpp
	 ddex3.rc

  5.  Highlight the "ddex3.cpp" file and select menu option
      "Sources->Sources Options->C++ Compiler Switches"
      In field "Include directories:[-i]" add ";..\..\inc;..\misc;..\include"
      so it should read

    "$(%watcom)\h;$(%watcom)\h\nt;..\..\inc;..\misc;..\include"

      ...of course you've set your Watcom environment variables already,
      right?


  6.  Repeat step 5. for ddutil.cpp also, or you can perform step 5. on the
      folder (.CPP) (representing all .CPP files)

  7.  Select menu option "Target->Make" and watch the few warning messages
      go by

  8.  Select menu option "Target->Run" and enjoy your Watcom compiler
      running something other than "helloworld.c" (well, for me anyway)

Other Direct X specific FAQ's may best be answered at the following URL:

  http://www.microsoft.com/mediadev/qanda/qa.htm

----------------------------------------------------------------------------


Q09. How do I program for Intel's RDX using WATCOM C/C++?

A09.  Intel's RDX is a library for composing sprites, playing back sound
and video.  I don't know the details, however Hossain Morshed of Intel
Corp. has responded to the inquiries of Ed Mueller and what appears to be
the popularity of this FAQ with the following information:

  The following is a description of how RDX can be used with Watcom
  compiler.
  
  Thanks,
  Hossain Morshed
  Intel Corp.
 
 
		     Intel(R) RDX and Watcom* C/C++
			       06/12/96
  
 
  The RDX v2.1 Developer's Kit has been tested with Watcom C/C++ (v10.6).
  It should be available for download this June (6/96) from the Intel RDX
  Web Site (http://www.intel.com/ial/rdx/).
  
  If you are using Watcom C/C++ with the RDX v2.1 DLLs, you will need to
  link your application to Watcom specific import libraries.  You can
  create Watcom import libraries for the RDX DLLs by using the Watcom
  WLIB tool.  For example, the following commands create the necessary
  import libraries (provided that WLIB is in your path and the DLLs
  reside in the C:\RDX\SYSTEM\RELEASE directory):
  
      wlib dmix.lib +c:\rdx\system\release\dmix.dll
      wlib dinoav.lib +c:\rdx\system\release\dinoav.dll
      wlib dino2d.lib +c:\rdx\system\release\dino2d.dll
  
  If you are trying to use Watcom C/C++ with the RDX v2.0 DLLs, we
  suggest you use the RDX v2.1 Developer's Kit.  Besides providing many
  new features and enhancements, it will also allow you to link your
  applications successfully to the data structures that the DLLs export.
  
  However, if you want to use Watcom C/C++ with RDX v2.0, it is possible
  to do this successfully by modifying the DINO.H and DINOAV.H header
  files so that they declare the FX GUIDs using the DEFINE_GUID macro all
  of the time and don't specify the DINO_SYMBOL/DINOAV_SYMBOL prefix.
  Then all you have to do is modify one of your application's C or C++
  modules to include the INITGUID.H header file before inclusion of
  DINO.H and DINOAV.H.	This will cause the FX GUID data structures to be
  declared in that module and thus be available to all of the other
  modules in your application.
  
  Note:  To successfully compile the RDX v2.0 sample programs a similar
  modification is necessary, in addition to a few other minor changes due
  to differences in how Watcom C/C++ and Microsoft Visual C++* compile
  source code.
  
  The RDX Developer's Kit does not provide Watcom specific make files for
  the sample programs.
  
  
  * Other product and corporate names may be trademarks of other
    companies and are used only for explanation and to the owners'
    benefit, without intent to infringe.

It appears that to use RDX, you must enter into an agreement with Intel
that involve NDA's of some sort.  Refer to the URL given above for more
information.

"Avocado Green" wrote the following on the WATCOM List server:

  Here are some simple steps in using the Watcom IDE (10.6) to compile
  and run the Intel RDX 2.1 (2.0 did not work fully with Watcom)
  example (EVENTS) included with the SDK.

  Note: The "lite" version of the RDX 2.1 SDK does not contain the
	.bmp files needed to build the examples, so you'd probably
	want to ask Intel for their RDX CD instead of downloading
	the 63MB archive.

	I'm not sure if Intel has got v2.1 on CD yet, so if you have
	v2.0, install that, then install the 2.1 "lite" version on top
	of it.

  1.  Create the Watcom import library files by running wlib like so:

      - cd to \RDX\Lib (if you installed the SDK there)
      - move the .lib files there to some other directory, you should
	have nothing in \RDX\Lib
      - run:
      - wlib dmix.lib +c:\RDX\SYSTEM\RELEASE\dmix.dll
      - wlib dinoav.lib +c:\RDX\SYSTEM\RELEASE\dinoav.dll
      - wlib dino2d.lib +c:\RDX\SYSTEM\RELEASE\dino2d.dll
      - wlib rdxam.lib +c:\RDX\SYSTEM\RELEASE\rdxam.dll

      There's a \RDX\SYSTEM\DEBUG directory with these same .DLL,
      Guess these are the debugging version of the .DLLs

  2.  Run the Watcom IDE (run it from the DOS box command line if it'd
      make you feel less guilty.)

  3.  Select menu option "File->New Project"
      Select the C:\RDX\Samples\Events directory and enter
      "Events.wpj" as your project name

  4.  Enter "Events" as the target name,
      "Win32" as Target Environment, and
      "Windowed Executable [.exe]" as the Image Type.

  5.  Select menu option "Targets->Target Options->Windows Linking
  Switches"
      Go to "2. Import, Export and Library Switches",
      In field "Library directories(;):[libp]" enter "..\..\lib"
      In field "Libraries(,):[libr]" enter "dmix.lib dino2d.lib"

  6.  Add these source files into your project:
	 events.c
	 help.c
	 winmain.c
	 events.rc

  7.  Highlight the folder (.C) (representing all .C files) and select
  menu option
      "Sources->Sources Options->C++ Compiler Switches"
      In field "Include directories:[-i]" add ";..\..\include"
      so it should read "$(%watcom)\h;$(%watcom)\h\nt;..\..\include"

  8.  Select menu option "Target->Make"
      - wow not even a warning message.

  9.  Select menu option "Target->Run"
      - Kill the dweebs

Hossain Morshed has the following update:

		  Intel(R) RDX and Watcom* C/C++ Addendum
				  9/10/96

  We have recently discovered that the Watcom C/C++ (v10.6) compiler's
  default behavior doesn't always force enums to be type int.  The RDX
  include files and DLLs require that enums are type int, so make sure
  when compiling your source files that enums are forced to be type int.

  To do this from the IDE, click on the Options menu and bring up the C
  Compiler Switches dialog box.  From there, turn on the Source Switch
  called "Force enums to be type int."	If you're compiling from the
  command line, just specify the "-ei" switch.

  If enums aren't defined properly when using RDX, you may encounter some
  unpredictable behavior.  For example, after applying the full screen
  effect (i.e. FX_FULL_SCREEN) to a surface, a call to prepare the
  surface may fail.  This could easily occur because the size of the
  FULL_SCREEN_PARAMS structure will be wrong as a result of the
  iColorType field being an enum.


  * Other product and corporate names may be trademarks of other
    companies and are used only for explanation and to the owners'
    benefit, without intent to infringe.

----------------------------------------------------------------------------


Q10. Is the code WATCOM C/C++ produces the fastest/tightest?

A10. The WATCOM C/C++ compiler is generally regarded as among the best in
terms of code generation.  For example, it produced significantly better
code than any 16 bit compiler.	In an article in PC Magazine a few years
back, benchmarks indicated WATCOM C/C++ version 9.5 had the tightest and
fastest code among such compilers as Visual C++, Borland C/C++, Metaware Hi
C, Zortech and Symantec (by a wide margin).  WATCOM's compilers have
generally lead in the performance category in all brands and versions of
their compilers starting with version 7.x, however among the more modern
compilers it appears as though both "djgpp" and "Visual C++ 4.x" produce
pure C code compilation which is as good (if not slightly better) than
WATCOM's.

However, among the reasons for choosing WATCOM for high performance code
generation is also its ability to use inline assembly language, the
disassembly tool and its ability to collect accurate performance profiling
information.  These tools are an indispensible part of the a "pedal to the
metal" coder.

Be sure to follow the recommended switches for compiling your code that are
given in the documentation.  Options such as "assume no aliasing" and using
"reciprocal and multiply" for floating point divides can cause the compiler
to make assumptions about your code that have great performance impact even
though the code generation techniques fall outside the rules of the ANSI
spec.  You should be sure you understand all the options for optimization.

The differences to note when comparing with Visual C++ is that WATCOM uses
register based parameter passing as well as using a superior inline assembly
paradigm.  These things may not translate to better C code generation, but
they end up being far better for integrating inline code, once you get to
the stage of doing hand optimizations.

----------------------------------------------------------------------------


Q11. How do I use WATCOM's tools to help me optimize my code?

A11. WATCOM comes with 3 tools that make the code optimization process an
easy one:  Execution sampler/profiler, disassembler and debugger.

There is a commonly held belief that in most applications 10% of your code
will be executed 90% of the time.  That is to say, most applications tend to
be bottlenecked, with the tightest bottleneck causing the greatest
performance penalties.	With WATCOM C/C++ this still holds true, but not to
such a severe extent, since WATCOM tends not to produce any "bottle-necky"
code.

It is for this reason that your first step in your optimization phase should
be to profile your code.  Even if your project is half complete, and not
representative of its final form, execution sampling and profiling can often
pinpoint unexpected bottlenecks in your code.

The execution sampler works by executing along with your code, randomly
gathering instruction addresses during its run.  The profiler then converts
the sample data into a chart which clearly shows what functions were called.
You can zoom into each function to see what lines of code are being
executed, and keep doing that right down into assembly level.  To do this of
course you have to compile your code with line number debugging information
(/d1 or /d2.)

Not too surprisingly, data collected on Pentiums may show a sample data on
machine instructions one after where the real execution occurred.  This is
because the Pentium tries to execute instructions in pairs and the delays
in one instruction may appear to be passed to the instruction it is paired
with.

The other tool is the disassembler.  Using it you can see all the
instructions compiled and correlate them their source lines.  WATCOM C/C++
generally only uses 1 cycle instructions.  However, in order to strike a
reasonable balance between code size and performance, WATCOM also uses some
complex instructions.  The most notable being: mul, imul, div, idiv, movzx
and movsx.  Simply scanning for these instructions can help you see where
your data structures and type mismatching are causing sub-optimal code to
be produced.

Finally the debugger is a simple tool not only for catching your bugs but
for tracing through your code to see the path it is really taking.  Most
of the time the execution path for small projects is fairly clear to the
programmer.  But when multiple programmers are involved or your code
becomes very fragmented, it is not so clear.  Very often, simply tracing
through your programs can lead you into good insights about the performance
of your program.

These general principles can be applied to any compiler, however few
compilers can rival WATCOM in these tools.

For information about Pentium optimizations in general, Agner Fog has
written up an excellent reference work which can be found on the following
URL: http://www.geocities.com/SiliconValley/9498/p5opt.html

----------------------------------------------------------------------------


Q12. How does WATCOM C/C++ compare with other C compilers?

A12. WATCOM C/C++'s biggest strengths right now are the performance tools
and its ability to do cross-platform development.  With WATCOM the concept
of a single project targeting multiple platforms is a practically realizable
goal.  Their 32 bit compilers have been around since about 1992, years
before Borland, or Microsoft had PC based 32 bit compiler technology
(apparently DJGPP/DJGCC had it, but was highly obscure by comparison.)
However, their recent acquisition by PowerSoft has given them a highly
acclaimed RAD tool (Optima++) that is based on top of the WATCOM C/C++
compiler; a joining of technologies that has caught their competitors with
their pants down.

By comparison, Microsoft is currently only supporting VC++ which can
create targets for Windows and DOS.  An extra dos extender (at extra cost)
is required for 32 bit DOS code (I believe Pharlap's TNT is the only one
really supported.)  DOS development is not supported by the IDE.  Version
4.x requires 24MB of memory to run smoothly (as opposed to WATCOM's
requirement of 8MB.)  Microsoft has gotten into a habit of tweaking their
compilers and operating systems in lock step; creating more and more
conventions and odd things like "Thunking" (a Windows OS and VC++ specific
way of performing 16<->32 bit execution segment translations, which I have
been lead to believe is disfunctional.)  Users of Microsoft compilers (and
products in general) become locked into their level of technology be it
good or bad.  They also get locked into their "vision" as they are clearly
pointing developers to Windows 95 and Windows NT.  Of course OS/2, QNX and
other platforms will never be supported.  The code generator for version
4.x compares favorably with WATCOM's, but the lack of reliable performance
tools and the klunky out-dated method of inline assembly keeps VC++ from
being the performance compiler of choice.

Borland is looking straight into the abyss these days.	I am not very
familiar with their most recent products, but their strength has always
been their IDE and compile speed.  But the latest WATCOM C/C++ compilers
have closed these gaps considerably.  Borland attempted to diverge from
MFC with "OWL".  They also stopped support for DOS for a brief period.
Their PowerPak solution does not completely map in the first megabyte of
memory which is problematic, and requires a lot of support to work around.
Last I heard, Borland had dropped PowerPak support completely.	These
mistakes have cost Borland valuable time and their compiler technology has
suffered as a result.  They themselves have practically admitted this and
have thrown their efforts behind "Delphi" (a RAD tool layered on top of
their non-standard Object Pascal.)

DJGPP beats all these compilers in one place: price.  For x86s up to the
486, the code generation is unbeatable, but currently does not optimize with
consideration for the Pentium (though a beta of that version of the compiler
is in the works.)  It also falls flat on its face in terms of compile speed,
interface and third party support.  DJGPP has only recently been robust
enough to produce targets that work under both DOS and Windows DOS boxes,
and the currently available Windows native 32 bit support is still in
development stages (and is a separate compiler.)  The IDE is still in
development and don't count on MFC support.  User support is there through
the internet. Standard game libraries such as WGT, Fastgraph and Direct Draw
have not been ported, and are not likely to ever be.  (On the other hand,
SVGAKit from SciTech has been ported, very recently.)

I haven't heard so much as a peep from Metaware.  As for Symantec, I hear
they are focussing on java.  Its likely that in the face of VC++ these
companies felt they simply couldn't compete.  Indeed its hard to see how
any other compiler besides WATCOM C/C++ will survive Microsoft's latest
product juggernaut.

----------------------------------------------------------------------------


Q13. How do I debug DOS4GW apps under Windows 95?

A13. WATCOM implements this using two DOS boxes.  This scheme allows the
target program to retain full control of its VM without interference from
the debugger (for, example, you should be able to debug apps which take
over the screen without any problems besides Window's ability to
virtualize that mode.)	So in theory, debugging should be more reliable.
In one DOS box you run the windows server:

winserv -trap=rsi

and in the other you run the debugger:

wd -trap=win <executable name and arguments>

The winserv program should be issued in the target executable's directory
while the wd program should be issued in the source directory.

This is essentially the same procedure used for remote debugging over a
serial cable (remote debugging.)  Be sure that the [386enh] section of
your system.ini contains the line "device=wdebug.386".	If you have
problems try explicitly entering the path for wdebug.386 (it should be
%WATCOM%\binw) and make sure that this file actually exists.  Be sure you
are running wd.exe from the %watcom%\binw directory, not the
%watcom%\binnt directory.

The "rsi" thing stands for "Rational Systems incorporated" which is the
former name of TenBerry Systems, so don't be surprised if this changes to
"tbs" in future versions of WATCOM C/C++.  For Windows NT, the server
should be run as "vdmserv -trap=rsi" and the debugger should be run as "wd
-trap=vdm <target command line>".  Similar solutions exist for other OS's.
For other extenders, "rsi" is substituted by another suitable acronym, see
the extender's documenation for more details (Thanks to Eric Kenslow,
"Gothmog" and Tomas Likens for the majority of this information.)

You must compile your application with debugging information to enable any
sort useful debugging.	This is done by giving a /d1 or /d2 to the compiler
and a "debug all" directive to the linker.  See the online help for more
information on each of these steps.

----------------------------------------------------------------------------


Q14. How do I debug WATCOM apps using Soft-ICE or Bounds Checker?

A14. Through information recieved from Richard Horrocks and an anonymous
emailer who queried WATCOM about it directly, I have surmized the
following procedure:

1) Compile with "-hc" compiler options to set the codeview debugger
   format, as well as your regular compiler options (including -d?
   debugging options of course!)

2) Link with "debug codeview opt cvpack".  By default "op symf" is set in
   the IDE, be sure to turn it off.

3) Generate a map file.  You can add "opt map" to the link options above
   (combining steps 2 and 3) or run DBG2MAP on the executable.

4) Run MSYM on the map file to get a symbol file that SoftIce can read.

Then use Soft-ICE/Bounds Checker as described in their manuals.  A common
problem with NuMega's tools when debugging large applications is running
out of memory for symbol information.  With Soft-ICE, you can expand the
amount of pre-allocated symbol memory by setting the 'SYM=####' directive
in the WINICE.DAT configuration file.  Alternatively you can reduce the
number of symbols with the -d2 or -d1 instead of -d3, and by being
selective about which modules you include debugging information for.

----------------------------------------------------------------------------


Q15. How do I enable virtual memory for my DOS4GW application?	How do I
know exactly what interrupts dos4gw handles all by itself?  How do I know
exactly what I have to do to use dos interrupts from protected mode?

A15. These and other DOS4GW specific questions are answered in the file
%WATCOM%\DOS4GW.DOC, and in the online help under the DOS/4GW section of
the user's guide.

----------------------------------------------------------------------------


Q16. Why doesn't WATCOM use "_asm" for inline assembly like Microsoft and
Borland?  After all isn't that standard?

A16.  Well, first off, that most definitely is *not* standard (as in ISO
or ANSI.)  Since the Microsoft/Borland _asm construct falls outside the
ANSI standard, it promotes the generation of C sources which are not
portable.  WATCOM instead uses the ANSI sanctioned "#pragma" directive.

In general, WATCOM's method allows stitching of C and assembly in a far more
optimal way than Microsoft/Borland's method.  It all has to do with moving
data around in registers versus requiring that you set up an interface via
the stack or memory locations.

For more information on WATCOM's decision to use this style of inline
assembly language support I refer you to:

    http://www.powersoft.com/services/techdocs/4368.htm

----------------------------------------------------------------------------


Q17. Why does my code work only when I compile with debugging info on?

A17. Dom Laflamme asked this question on the USENET newsgroup
"rec.games.programmer" and I gave the following response:

  Dom Laflamme wrote:
  > Hi,
  > I've been coding a game engine for the passed 3 months.  It works 
  > fine and it's fast.  Except that when I turn the "Debugging Info" off
  > in Watcom, _nothing_ works, it just crashes.  Since I cant debug the
  > thing properly because of lack of debugging info, I face a truly
  > problematic situation!

  There is a big difference in the code generated by using the /d2 debug
  switch and no debugging.  /d1 is supposed to be nearly identical to
  using no debugging but has significantly less debugging information.
  WATCOM's optimizations are *NOT* broken as someone else posted.  But
  they are truly aggressive.
  
  WATCOM recommends setting: /otexan, and each switch (t, e, x, a, and n)
  is sufficiently documented in the online help.  I'm guessing that you
  have these setting all on since "it's fast".	This recommendation is
  based on the assumption that you are writing correct ANSI-C and that you
  are not stepping outside the bounds to the point of breaking one of
  WATCOM's optimization strategies.
  
  As a simple test, trying setting all the optimization switches off (as
  well as the debugging switch off.)  If your engine starts working again,
  then turn the optimization switches back on one at a time until your
  engine breaks again.	Keeping in mind that the "a" switch (assume no
  aliasing) is the only one in which correctly written (but really brain
  dead) ANSI C can fail to execute.
  
  Once you have isolated the switch, read the documentation on the switch
  very carefully.  Try to isolate the problem area and disassemble the
  code with each optimization switch.  Once you see the difference, you
  should be able to figure it out.
  
  Your problem is likely one of three things:  (1) Simple mistake, like
  dereferencing a bad pointer, or accessing outside the bounds of an
  array, (2) You are misunderstanding the meaning of the "volatile" ANSI C
  keyword and hence are misusing it, or not using it (the most likely)
  when you should.  (3) You are aliasing pointers to non-disjoint
  structures and WATCOM is reordering an inner loop in too aggressive a
  manner (you will have to either recode or remove the "oa" switch
  entirely.)
  
  I have seen all three cases in my own work and in the work of others.  I
  found that using WATCOM is an exercise in "re-learning" how ANSI C
  really works.
  
----------------------------------------------------------------------------


Q18.  What is FLAT memory model?  What is the advantage of a 32 bit
compiler such as WATCOM C/C++ over 16 bit compilers?  What are the
differences?  Will I be able to move my 16 bit DOS code straight over to
32 bit?

A18. WATCOM C/C++ can compile for either 16 bit or 32 bit.  The compilers
are named wpp/wcc/wcl and wpp386/wcc386/wcl386 respectively for 16 bit 
and 32 bit compilation.  Every DOS programming model under the sun is
implemented:  TINY, SMALL, COMPACT, MEDIUM, LARGE, HUGE (the 16 bit
models) as well as FLAT and LARGE32 (the 32 bit models).

WATCOM C/C++'s default 32 bit DOS memory model is FLAT memory model.  FLAT
memory model is an analogue of TINY model for 32 bit programs.	ES and DS
are set to the same selector which points at the base of memory and maps
all 4GB of address space.  CS is a selector that points to all of memory
like DS and ES, but has the execution attribute set, meaning that it
cannot be written to directly.	SS is set to a special stack segment to
make stack overflow easier to detect.  This set up allows you to, in
nearly all situations, ignore segment/selector registers completely.  FS
and GS are available for applications to use as they see fit and by
default are set to 0.  (Hint:  Be sure to check the values of these
selectors whenever you are debugging a failure in your code.  These
selectors may change as a result of ill-behaved interrupt vector calls,
and hence may need to be saved and restored properly (see the #pragma
discussions elsewhere in this FAQ))

Unless you are dealing with interfacing to external 16 bit resources (such
as APIs implemented through interrupts) you never need to deal with 64K
segment limits, or strange keywords like FAR, NEAR or MK_FP.  Pointers and
int's are both 32 bit, and while its not advisable, you can cast between
the two and still be portable to most platforms (16 bit x86 environments
and 64 bit DEC Alpha environments being the most notable exceptions.)

The extra bandwidth/precision you get for using a 32 bit coding model over
a 16 bit one speaks for itself.  32 bit instructions executed from 32 bit
segments do not need instruction overrides.  Beyond that only TINY and
SMALL models (from the 16 bit model choices) compare with the low overhead
for context switches.  That is to say, the only 16 bit models that also
don't require selector loads for new data pointers or function calls are
restricted to 64K of data and/or code.

Familiar DOS/BIOS data areas such as 0A000:0000 and 004f:0000 have been
mapped in the most natural way:  16*segment+offset.  So VGA space is now
at _FLAT:0x0A0000 and the standard BIOS area is now at _FLAT:0x04F0.  But
this mapping doesn't necessarily hold true for the rest of system memory.
This is actually a good thing since it allows for linearly mapped graphics
cards to have their address range placed at a convenient place, namely the
end of your conventional memory.  (Or in the case of Windows; usually a
special address range with certain OS specific characteristics.)

(It is important to note that the construct MK_FP(0xA000,0); doesn't have
the same meaning and will *NOT* correctly map the screen to a usable
pointer.  Use (char *)(0x0A0000) as just described.)

This sudden ease of implementation for everything is actually likely to
make the job of porting from 16 bit DOS quite involved for many
applications.  This porting procedure would mostly be an exercise in
*undoing* all the 16 bit nonsense that was being done previously.

For more information look under "Converting to WATCOM C/C++" in the 
Programmer's Guide.

----------------------------------------------------------------------------


Q19. I am getting this weird error message ...

A19. Lets go through some of the more common ones:

(1) When I run my program I get something like:

    DOS/4GW Protected Mode Run-time  Version 1.97
    Copyright (c) Rational Systems, Inc. 1990-1994
    DOS/4GW error (2001): exception 0Eh (page fault) at 237:825A401F
    TSF32: prev_tsf32 5290
    SS	     23F DS	  23F ES       23F FS	      0 GS	  87
    EAX FFFFFFFF EBX 8244A380 ECX	 0 EDX	      0
    ESI 824493A1 EDI 8244939C EBP 8244A374 ESP 8244A370
    CS:IP  237:825A401F ID 0E COD	 6 FLG	  10202
    CS=  237, USE32, page granular, limit FFFFFFFF, base	0, acc CFFB
    SS=  23F, USE32, page granular, limit FFFFFFFF, base	0, acc CFF3
    DS=  23F, USE32, page granular, limit FFFFFFFF, base	0, acc CFF3
    ES=  23F, USE32, page granular, limit FFFFFFFF, base	0, acc CFF3
    FS=    0, USE16, byte granular, limit	 0, base	0, acc	0
    GS=   87, USE16, byte granular, limit     FFFF, base    18DD0, acc F3
    CR0: unavailable
    Crash address (unrelocated) = 1:0000001F

This means that you have an error in your program.  If the address given
in the last like looks like 1:000?????	(basically segment 1 and a
reasonably small sized offset) then chances are you can run this under the
debugger and have the address of your error shown to you.  If even the
address looks like its off in the weeds, remember that WATCOM's debugger
allows you to crawl up the execution stack to see how the crash was
arrived at.

If running the debugger is out of the question or somehow problematic for
you then you can generate a map file by putting "option map=myprog" on
your link command line, or putting "/fm=myprog" on your wcl command line.
This is most useful when you are compiling with debug info ("/d2" on your
compile command line.)	 The resulting myprog.map file will correlate the
line numbers in your program to relative addresses which can be matched
against the "Crash address" given above.

Often the target address will be in a library function.  This generally
means that either the parameters passed to the function are bogus, or
some global state that the function depends on has become damaged.
Global state is hard to be debug, but not impossible.  (Trying to do
this without a debugger is a hurting proposition.)  See "Q20.  I am
having malloc troubles.  It seems that I am trashing the heap but I
don't know how to debug it." later on in the FAQ.

Another possibility that everyone seems to run into sooner or later is the
small stack limitation.  WATCOM C/C++'s stack, by default is 16K, so
declaring a local array in a function of more than 4096 ints, say, will
cause a stack fault with no clear explanation about the error.	This error
is ordinarily caught by debugging with runtime stack checking on (the
default, without the "/s" switch.)

But most of the time you are simply accessing memory you shouldn't be
(past end of an array, stuffing a pointer without first mallocing it,
inadvertently corrupting a pointer that you use later.)  Remember,
pointers are not in "far" format, but rather in "_FLAT" format.  So
0x0A0000000 is not the address of the VGA frame buffer, 0x0A0000 is.

(2) I type:

    wcl386 /l=dos4g hello.c

and get back:

    WATCOM Linker Version 10.6
    Copyright by WATCOM International Corp. 1985, 1996. All rights
    reserved.
    WATCOM is a trademark of WATCOM International Corp.
    Warning(1107): file __WCL__.LNK: line(2): undefined system name: dos4g
    loading object files
    searching libraries
    Error(3002): ** internal ** - format not decided
    Error: Linker returned a bad status

This means that when you installed WATCOM C/C++ you did not select 32-bit
DOS target.  You can run the installation procedure again to do an 
incremental install of 32 bit DOS compiler support.

(3) I'm using the IDE and am trying to compile an example program that comes
with "SupermundoGraph" and am getting messages like:

    wlink SYS dos op m op maxe=25 op q op symf @smg_ex1.lk1
    Warning(1028): smg_move_ is an undefined reference
    Warning(1028): smg_drawmap_ is an undefined reference
    file timmy.obj(C:\smg\src\smg_ex1.c): undefined symbol smg_resize_
    file timmy.obj(C:\smg\src\smg_ex1.c): undefined symbol smg_setclip_

"undefined reference" and "undefined symbol" indicate that something has
gone wrong in the linking process.  I am not very familiar with the IDEs
but, chances are you have not added in the SupermundoGraph library
properly.  According to the v10.0 IDE help documentation, you simply add
the library (*.lib) as if it were a source file.

Also watch that you have selected the right directories for your project.
I usually set up my directories and build my project sources before
invoking the IDE (in the rare occasions that I have ever used the IDE.)
If you are not careful, you will end up building projects right in the
compiler's own Windows tool directories, which makes for file maintenance
headaches sooner or later.

If that's not it, then perhaps the example is not compatible with your
version of WATCOM C/C++.  In this case, you should contact the library
vendor and inform them of this problem.

(4) I am getting linker errors from wpp386 that make no sense.	I have
everything externed and properly prototyped as far as I can tell.

One possibility is that you are trying to combine C and C++ code.  If it is
your intention to only write C, and not import C++ libraries, then you
should use the wcl386 or wcc386 compilers.  If you want to write C++ code,
you cannot simply mix C and C++ directly.  To import C code into a C++
project you must prototype them as extern "C" { } as follows:

extern "C" // c must be a capital C
	{
	extern void Set_320_200();
	extern void Put_Pixel( BYTE Color, WORD x, WORD y );
	}

The extern's publicize the symbol, and the "C" declares them as C symbols
so that they are linked without C++ attributions.  Thus they act as C
symbols, not C++ symbols.  This means their names are not mangled in the
objects, they cannot be overloaded, etc., etc.

Thanks to "Lassche MA" for verifying this, and thanks to Eric Kenslow for
giving more a detailed explanation of extern "C".

----------------------------------------------------------------------------


Q20. I am having malloc troubles.  It seems that I am trashing the heap
but I don't know how to debug it.

A20. Someone on rec.games.programmer asked this question and I gave this
as the response:

  Apparently somewhere in version 10.0 or 10.5, WATCOM had a problem with
  mallocing that has been fixed in a subsequent build.	It is likely that
  a patch for it exists on their WWW site.

  Nevertheless, I believe that bug was an extremely obscure one with the
  "free" command and is unlikely to be your problem.  Dealing with a
  trashed heap is difficult with any compiler, but WATCOM makes it
  particularly challenging because of its "frugal" heap memory tracking
  system.  If you do a little debugging/reverse engineering, you will see
  that a WATCOM memory allocation tracks the size in the int (signed long)
  just before the actual allocation itself.  So (ptr=(int*)malloc(99))[-1] 
  should be 99.  The malloc/free functions use this and this alone to
  track the heap so if you are trashing memory, especially these sizes,
  you will start dying really soon.
  
  I found that the WATCOM debugger is particularly good at tracking down
  precisely this sort of problem.  Allow me to tell you a story:
  
  Once upon a time I had a memory corruption in a large convoluted program
  I was writing.  So I started debugging by putting a wrapper around the
  "malloc" and "free" functions that added an additional size to test the
  validity of each malloced size against when I use it (which I put on the
  *other* side of the allocation.)  I put a test in the place where my
  program was accessing it and GP faulting.  I simply output a message
  indicating that the memory was trashed.  Using the debugger I put a
  break point on the error message printer.  Then I used the "reverse
  execution" feature to get me back to the failed memory size comparison
  and noted the machine address of the WATCOM size pointer.  Then I reset
  the program and put a breakpoint on access to that memory address.  It
  turns out that my program legally accessed it several times (on the
  order of a thousand.)  So I used the breakpoint "counter" to help me
  zero in (essentially a binary search) on the number many times this
  memory was accessed before the crash occurs.	By setting the counter
  appropriately, I found the code which was pounding on the size and
  rectified it.  Total time spent was about 1.5 hours which is not bad for
  a bug of this complexity (most of the time was spent on other desperate
  attempts to find the bug *before* diving in and trying the above
  procedure, which took < 10 mins.)
  
  I can't praise the WATCOM debugger enough for enabling me to do the
  above procedure.  In particular, the reverse execution feature gives me
  unparalleled power (with the exception of Soft-ICE, which of course,
  gives you much more power) for determining my context.  Mixed assembly
  and source level debugging allowed me to reverse engineer WATCOM's
  malloc method lickedy split.

  BTW, the bug in my program was simple.  I was accessing a pointer before
  mallocing it.

Paul Keys responded to the post above with a very good suggestion:

  Watcom has functions that can be used to debug the heap.  Functions such
  as _heapset, _heapchk, _heapwalk, are indispensible.	Using _heapset I
  have managed to find cases where I was using pointers to memory that had
  been mistakenly freed.  With _heapchk, I have tracked down where the
  heap lost its integrity.  And using _heapwalk, I was able to track down
  some rampant allocation that was slowly gobbling up all available
  memory.

Finally, it should be noted that on the WATCOM C/C++ file site there is a
sample program for diagnosing memory errors.  This program is located at the
following URL: ftp://ftp.powersoft.com/pub/c_cpp/general/memcheck.zip

----------------------------------------------------------------------------


Q21. How do I write directly to graphics memory?

A21. As mentioned above, the base of VGA memory is mapped at 0x0A0000.
The ports can be accessed via the outp() function, so even setting up Mode
X is no big deal.  For an example of this see:

    http://www.geocities.com/SiliconValley/9498/modex.zip

CGA and EGA are no more challenging than VGA.

SVGA modes have their own special problems.  The examples in Ferraro's
book on VGA's and SVGA's can be easily adapted to WATCOM C/C++, even in
pure C, with the exception of the VESA SVGA stuff.  Revision 1.2 of the
VESA SVGA interface (the interface most widely supported by SVGA's out
there) requires a 16 bit memory allocation for a data buffer and a 16 bit
call or interrupt to the "Bank Switch" procedure (my own investigations
lead me to find that the WinPtr call was not reliable on many graphics
cards and thus I stuck with the interrupt).  Many novices (including
myself, when I first tried it) make the mistake of thinking that they can
just pass in some appropriately cooked values into the required registers
as the VESA spec requires and just call int 10h.  Unfortunately this does
not work when the "es" register has to be stuffed with the segment for the
buffer that VESA fills in with mode info.  You need a protected mode to
real mode translation API to make dos allocations and simulate real mode
dos interrupts.  These are both achieved by using DPMI which is discussed
elsewhere in this FAQ (see Q23.)

Revision 2.0 of the VESA SVGA interface has a 32 bit interface and also
allows the option of mapping the entire frame buffer to a single
contiguous linear memory range.  The VESA SVGA specs are available at:

ftp://x2ftp.oulu.fi/pub/msdos/programming/specs/vesasp12.zip and
ftp://x2ftp.oulu.fi/pub/msdos/programming/specs/vbe20.zip

The WATCOM C/C++ file area also has an example URL for getting you started
using VESA:

ftp://ftp.powersoft.com/pub/c_cpp/general/newvesa.c

If you require linear memory access to SVGA, then your choices are:  (1) use
a VESA 2.0 interface (the easiest way), (2) Map in the graphics controller's
physical memory yourself (the hardest way), or (3) Use a more powerful DOS 
extender (than DOS4GW) that supports the DPMI phys to linear map function
(the expensive way, see Q23 for more information.)

Programming a graphics controller to use its specific functions requires
getting specifications for the graphics controller themselves.

----------------------------------------------------------------------------


Q22. How do I install a mouse event handler?

A22. There is an example of this on the WATCOM C/C++ web site general file
page.  The URL is ftp://ftp.powersoft.com/pub/c_cpp/general/tstmouse.c The
general idea is just to make sure the declaration protocols have it taking
the right registers as its parameters, stack checking is turned off and DS
is reloaded.

----------------------------------------------------------------------------


Q23. What is DPMI and what role does it play in using WATCOM C/C++?  How
do I communicate between the 16 bit world and 32 bit world on my PC?

A23. DPMI stands for "DOS Protected Mode Interface" and was dreamed up by
Intel and Microsoft.  It was a method for extending DOS's functionality
for supporting 32 bit programming at the lowest possible level.  Windows
is an example of a program which implements and leverages the DPMI
interface to enable "Enhanced Mode" and 32 bit DOS Extended applications
to run under DOS boxes.  (QDPMI, CWSDPMI and DOS4GW itself are other
examples.)

For most purposes DPMI essentially is an API for setting up 32 bit
selectors, and call gates between 16 and 32 bit execution segments.  A
version 0.90 (the one used by Windows) specification is available at:

ftp://x2ftp.oulu.fi/pub/msdos/programming/specs/dpmispec.arj

(you need 'ARJ' to decompress this file; its available elsewhere on x2ftp)
An online 1.0 specification can be found at:

ftp://x2ftp.oulu.fi/pub/msdos/programming/specs/dpmi100.zip

Also be sure to check the WATCOM C/C++ file area directory located at

ftp://ftp.powersoft.com/pub/c_cpp/general

which has a couple of relevant example files: rintcall.c, and dpmi800.c.

The major differences between 0.90 and 1.0 is that 1.0 servers must return
with exact error codes in the event of a failure.  Among its most common
uses are:  allocating 16 bit memory and calling a 16 bit interrupt which
takes a 16 bit segmented address for a data buffer (ordinarily DOS4GW
and/or WATCOM's heap manager will not "malloc" from DOS memory, so it
essentially goes wasted.)  These are both required for supporting VESA
SVGA 1.2.  You may notice that among the many messages printed on DOOM's
fake "DOOM OS" init screen is a mention of DPMI allocations.  The
programmers at "id" probably wrote their own memory manager which uses
conventional memory in addition to regular heap memory.

The major short coming of DOS4GW is that its an incomplete implementation
of DPMI (0.90).  The WATCOM C/C++ users's guide describes what DPMI
functions *are* implemented in the "INTERRUPT 31H DPMI Functions" chapter.
DOS4GW even steps outside of the bounds in the sense that unsupported
functions do not simply return with the carry flag set indicating an error
(even though it has not completed the DPMI function.)  Fortunately the
latest DOS4GW implements both the DOS alloc function and the simulated DOS
interrupt call, which are sufficient for setting up rudimentary 16 bit
communication.	If another DPMI manager is present, however, DOS4GW will
not set up its own DPMI server.  (I purchased the CauseWay DOS Extender
which is far more robust and feature rich.)

So how does one actually coordinate 16 and 32 bit components together?
Allow me to explain one method.  All 32 bit DOS executables are bound with
a 16 bit kick-start program.  This kick-start program is, by default,
named WSTUB.EXE and is located in the %WATCOM%\BINB directory.	Obviously
you can change the contents of this file to anything you like (you can
probably do this without mucking with %WATCOM%\BINB\WSTUB.EXE directly,
but rather using the WBIND command, but I have not investigated this
fully.)

The 16 bit source code for WSTUB is also supplied, which makes the
kick starting procedure fairly clear (starting with version 9.5, WATCOM
included both that 16 and 32 bit versions of their compiler in the same
package.)  It basically parses the command line arguments and passes
control to DOS4GW with itself as the first parameter followed by the rest
of the regular parameters.  You can write all your 16 bit components in
the stub, passing critical pointer values as command line parameters to
the 32 bit portion.  The pointers could point to anything, including 16
bit function entry point addresses and data areas.  The 32 portion would
do the simple math to convert the pointers and can pass control back to
the 16 bit stub functions using the DPMI 16 bit function call simulation
call.  Voila!

The reason I've described the above procedure is that I've seen others take
a less savory approach that I'd just as soon see avoided in the future.  In
at least one sound library I noticed that the implementer wrote a 16 bit
TSR that had to be executed beforehand.  Clearly, an unsanctioned or
unreliable method of communication between the 16bit TSR and 32bit library
was being used.  Using the stub method is much cleaner.

Why would you ever want to paste 16 and 32 bit code together?  Well, the
main reason would be that you have a large database of 16 bit code that is
most logically accessed as if coming from an interrupt event.  I've also
heard rumors to the effect that using DMA is a 16 bit only operation, but
I'd have to investigate that more fully before confirming it.

For example, you might have a substitute mouse driver written in 16 bit code
that you don't want to run as a separate TSR because you want to retain the
old mouse ISR.	So you could install it at the start of WSTUB, spawn the 32
bit process, then deinstall it when you come back.

Personally, I've modified WSTUB.EXE in minor ways, but in all honesty, I
have to admit that I have not tried the above procedure myself.
Nevertheless, I cannot see any other reason WATCOM would expose the 16 bit
source for WSTUB.EXE.  If somebody ever implements (or has already done so)
this idea successfully, I'd like to know about it.  (Note that extenders
that bind to the executable typically have their *own* wstub.exe, and hence
don't allow for this sort of scenario to be set up.)

----------------------------------------------------------------------------


Q24. What if I really need to compile a 16 bit model program?

A24. Starting with version 9.5, WATCOM includes both the 16 and 32 bit
compilers in a single package.	The 16 bit compiler executables are
wpp.exe, wcl.exe and wcc.exe, while the 32 bit compilers are wpp386,
wcl386.exe and wcc386.exe.  It should just be a matter of changing your
makefile or flipping an IDE switch.

----------------------------------------------------------------------------


Q25. Why do WATCOM's results and Visual C++'s results differ on the same
source code?  Why does WATCOM implement the default signedness of chars
different from everyone else?  I am having difficulty porting code to
WATCOM C/C++?

A25. A list of these differences can be found in the online help.  Here's
a list of the major gotcha's:

- To get around the signedness default be sure to pass a "/j" on the
command line.  

- WATCOM will, by default, use registers for its parameter passing
convention which makes it incompatible with regular COFF format libraries
and object files.  However, adding the extra declaration information via
"#pragma aux <function> frame;" will force WATCOM to pass parameters to
the function via the stack.  

- WATCOM's "wmake" utility parses a makefile format that is entirely
different from Microsoft's.  WATCOM decided to mimic UNIX as closely as
possible, while Microsoft went ahead and made their own standard.

- If you do asynchronous variable updates (such as modifying global
variables in an ISR), you *need* to declare your variable as "volatile".
Other C compilers optimization strategies act as if all variables were
declared volatile, and can mislead you into thinking it is unnecessary to
declare your variables this way.

- In general, WATCOM's C/C++ compilers tend to be more stable, however one
notable exception was the initial release of version 10.0.  But WATCOM had
patches available in a couple months which fixed its most outstanding
problems.  Relatively speaking, WATCOM's compiler is fairly robust and
more ANSI compliant than Visual C++ which can explain most discrepancies.

----------------------------------------------------------------------------


Q26. Can I link external assembly files together with my C files?

A26. WATCOM's linker can accept .OBJ files in its own format or in the
standard intel format.	So using a separate assembler should not cause any
extra unnecessary complications.  Earlier versions (9.0) of the compiler
required the use of the "WOMP" utility to convert object formats.  Starting
with version 10.0, WATCOM C/C++ also comes with an external assembler called
"WASM".  However about the most praiseworthy comment I've heard about it was
something along the lines of "when I wrote everything from scratch and
followed all its rules, WASM seemed to be able to do the job."  Basically, I
don't believe WATCOM implemented many of the common, but superfluous
assembler features of MASM or TASM which a lot of people rely on and hence
were complaining about.

The most common problem when trying to link external assembly objects with
C/C++ objects is the name mangling/modification that the C/C++ compiler
does by default.  WATCOM C symbols in general are prepended with an
undescore ("_"), whereas assembler names are not modified in any way.
This can be changed with a #pragma directive (see Q27, and the online
documentation for changing the symbol renaming convention used by WATCOM
C/C++.)  However, Eric Kenslow says that this is unnecesary.  WATCOM has
changed the meaning of "cdecl" recently, so how you deal with this may be
different depending on the version of WATCOM C/C++ you are using.

According to Mats Byggmastar, in C++ you should declare your external 
variables as:

        extern "C" int Variable;
        int Variable;

And your ASM external variables as:

	extrn _Variable:dword

In any event, if you are having problems linking asm and C objects
together, use "wdisasm" (or simply a binary viewer for that matter) and
see how the shared symbol names are being modified for each of your
objects.  As recourse, in C you can use the #pragma name modification
directive, and in assembly you can simply append your public/extern names
with underscores as necessary.	Modifying your assembly symbol names is
the method Eric Kenslow recommends.

----------------------------------------------------------------------------


Q27. How do #pragma's work?  How do I use inline assembly language?

A27. WATCOM's #pragma command is generally used as an alternative way for
defining C/C++ function generation properties.	Examples:

1.  If you know that the parameters for a function are available either on
the stack or in certain x86 registers then you can assign which go where
*explicitly*.

2.  If you know that at link time you will need the exported function
names to be mangled in some nondefault way then you can deal with that in
a #pragma.

3.  If you want to generate inline assembly then you can do so as actual
assembly text or opcode bytes.	You give a function prototype and then
declare its content using the #pragma aux <function>="<asm>" directive.

This system for dealing with inline assembly is very powerful as it allows
you to explicitly declare your register usage, so that your C and
assembly stitch together perfectly.  Surprisingly, Visual C++ and
Borland's "_asm" method is not nearly as powerful.  With the "_asm" method
it is recommended that you look at the disassembly (you need to generate a
.COD file in Visual C++) of your surrounding source C to know what
registers are available to use, or simply do not modify any registers at
all.  This makes inline assembly coding tedious and difficult to maintain.

In contrast, the WATCOM method requires that you abstract your inline
assembly to have a regular C-function prototype which aids in portability
to non-x86 platforms.  A word of warning; inline assembly functions
declared with #pragma aux are not externed and hence behave like regular
C++ "inline" or "static" functions.  The typical method of dealing with
multiple C files using the same inline function is to define it in a local
include file common to your C files.  In this vein I think of #pragma's as
type checked macros, rather than functions.

Here's an example of the power of #pragma inline assembly for 32 bit FLAT
memory model.  Suppose we wish to implement the following:

  static void DWORDCopy(long *Source, long *Destination, int Length) 
  {
      for(;Length;Length--) 
      {
	  *Source++=*Destination++;
      }
  }

in tight (i.e., small) assembly.  Using the WATCOM disassembly will show
you that the above is *not* compiled to a "rep movsd" no matter what
compiler settings you use.  The functionally equivalent inline pragma is
implemented as follows:

  void DWORDCopy(long *Source, long *Destination, int Length);
  #pragma aux DWORDCopy = " rep movsd " parm [ESI] [EDI] [ECX]\
  modify [ESI EDI ECX];

The above function will copy a packed array of 32 bit quantities from a
source pointer to a destination pointer using the "rep movsd" x86 assembly
construct.  The source and destination parameters are put into ESI and
EDI, with the length in ECX which is exactly what "rep movsd" requires for
priming.  In addition movsd requires that es and ds are set up, but that
is all taken care of by the FLAT model start up code.  To implement this
same thing with Borland C++ or Visual C++ you need additional priming
code to transfer the source and destination parameters to the correct
registers.  Finally, the C-compiler needs to know what registers were
modified by the inline #pragma so that it can figure out what registers
are available to the surrounding C-code.  This cuts down on unnecessary
save and restores of registers that is generally required to safely
implement an inline "_asm".

Typically, the meat of the code appears in the double quotes, and is
several lines long, however opcodes can also be entered directly as bytes
as well.  Remember that ANSI C constants strings can be broken up in
sequence, and that lines can be stitched together with a "\" at the end.
#pragma's have to be declared on one line, so longer inline assembly
language sequences typically look more like:

  void SetMode13(void);
  #pragma aux SetMode13 = \
  "     mov ax,13h      " \
  "     int 10h         " \
  modify [ax];

(My point above is that this #pragma statement is equivalent to:  
#pragma aux SetMode13 = " mov ax,13h int 10h " modify [ax];)  You then
call "SetMode13()" as if it were a regular function, and WATCOM does the
inline pasting for you.

Why would you ever want to enter opcodes?  Well, intel has a habit of not
documenting all of their mneumonics, and MMX will be introduced which will,
at first, require opcode encoding (though WATCOM has committed to supporting
these opcodes in future versions of WATCOM C/C++).  I had, in an earlier
version of the WATFAQ.TXT claimed that rdtsc required opcode inlining,
however this is not the case as Charlie Wallace informs me:

  #pragma aux rdtsc =	  \
  ".586P                " \
  "     rdtsc           " \
  "     mov [esi],eax   " \
  "     mov [esi+4],edx " \
  parm [esi] modify [eax edx] value [eax];

Is perfectly acceptable.  Other reasons for inline are to chose between
short handed and non-short handed instructions (when differing opcodes
produce the same assembly instruction.)  The Pentium actually has some
performance penalties for executing some of the short-hand opcodes.

Note that the division of strings into separate implicitely concatenated
strings is important.  That is to say " mov eax,ebx add ebx,ecx " will not
be parsed correctly, however " mov eax,ebx " " add ebx,ecx " will.  This
clearly falls outside of what one expects from C's parsing rules. However,
I believe WATCOM is still inside the ANSI rules on this one since what is
parsed after a #pragma directive is implementation defined.  (I wouldn't
mind getting an official ruling on this one though.)

For more examples see "KNUT's WATCOM C/C++ tutorial" at

  http://home.sn.no/home/oruud/wat_tut1.htm
  http://home.sn.no/home/oruud/wat_tut2.htm

The information given there is not 100% correct, however he gives detailed
examples that should give you a rough idea about how it works.	More
detailed information can be found in the online help.

----------------------------------------------------------------------------


Q28. How do I do compiled bitmaps?  How do I do on-the-fly generated code?

A28. Its no big deal really.  Remember that CS, DS and ES are mapped to the
same physical space.  So in principle you need only malloc some space, fill 
it with 32 bit instructions (note that the opcodes between 16 and 32 bit do 
not translate directly) cast it to a function pointer (be especially careful 
with any parameters passed) and call it.

Modifying existing code (realizing of course that you take your life in
your own hands when you do this) is also possible.  Cast a function
pointer to a regular data pointer ((char *) is probably the only
appropriate pointer type) modify the data function code as if it were just
an array of opcodes and operands, then call the function.

Unfortunately, it looks like the compiler can do something really odd.	If
you assign the address of an existing function to a function pointer and
modify it directly under the same scope, the compiler will tack on a "CS:"
segment override, which will cause a page fault at execution time (Code
selectors cannot be used to write data.)  I don't quite see why they did 
this but the obvious work around of using another function to modify the 
actual code seems to work just fine:

  #include <stdio.h>
  #include <malloc.h>
  #include <stdarg.h>

  // printf compatible function prototype
  typedef int (* fntype)(char *, ... );

  char * fn;

  // Assembly language opcode to cause an immediate return:
  #define RET 0xc3

  void SetReturn(char * ptr) 
  {
	  ptr[0] = RET;
  }

  void main(void) 
  {
	  printf("Hello\n");		// Call "printf" as normal
	  fn = (char *)malloc(128);	// Get some memory
	  SetReturn(fn);		// Assign function
	  ((fntype)fn)("Hello\n");	// Call function with parameter
	  fn = (char *)&printf; 	// Point to "printf"
	  SetReturn(fn);		// Reassign printf function
	  printf("Hello\n");		// Call "printf" as normal
  }

This was compiled with the -5r and -s switches (only.)	Clearly there is 
a difference between the stack based and non-stack based libraries which
makes this sort of self-modifying code particularly hazardous to deal
with.

The details for doing compiled bitmaps are beyond the scope of this FAQ,
however the general idea is to construct a number of 

  mov [edi+offset],immediate

one after the other which plot the sprite.  This makes transparency
encoding an intrinsic part of the sprite.  However, difficulty with
clipping (especially in the x direction) and destination memory write
alignment makes compiled sprites a questionable compromise to a more
general run length copy routine.  For more information read the newsgroup:
rec.games.programmer.

Doing more exotic things like writing your own compiler and setting up
your own compiler environment with protected execution spaces are also
beyond the scope of this FAQ, but can probably be dealt with just using
DPMI.

Now, before you go off and get any not so bright ideas, read the following
USENET conversation:

  > Hi there, I was wondering how you need to set things up so you can modify
  > your code and make copies of your functions under DOS4GW/Watcom....  I've
  > tried copying the function's code around to several different areas, but
  > it always gets a GPF....  Here's a simple example that crashes, if anyone
  > has any idea how to make it work, please let me know!  Thanks in advance!
  >
  > void printstuff()
  > {
  >	printf("HELLO\n");
  > }
  >
  > void main()
  > {
  >	void (*pFn)();
  >
  >	pFn = new char[500];
  >	memcpy( pFn, printstuff, 500 );
  >	pFn();
  > }

  The reason this doesn't work is because of the x86's relative addressing
  modes.  That is to say, that the >> printf("HELLO\n") << statement is
  translated to a *relative* call.  It gets translated to something like:

      mov eax,offset L1 		    ; Pointer for string "HELLO\n"
      call relative ((offset printf)-($+5)) ; $ = current address.

  The x86 will automatically add in the current address to the relative
  offset and come back with the right address.	So in the copied version
  of the routine (in pFn()) you won't end up at the address of "printf"
  which you desire, because the relative difference was copied, rather
  than the physical address.

  There is no easy work around for this, and it is for reasons such as
  this that self-modifying code and related techniques, such as what
  you've tried to do are generally frowned upon.  The following two
  suggestions which fix the above program also serve to illustrate the
  flaw I am pointing out.

  (1) Instead of using 500, try using the number 31744 in both places.

  (2) Rather than calling printf directly, assign the address of printf
  to a global volatile (just to make sure WATCOM doesn't try any
  simplification tricks to avoid using it) function pointer, and call
  the function pointer instead of printf directly.

----------------------------------------------------------------------------


Q29. How do I install a 32 bit interrupt vector?  How do I install a
bimodal interrupt?

A29. Look up _dos_getvect, _dos_setvect, _dos_keep and _chain_intr in the
online help.  I don't know much about bimodal interrupts but the idea is to
write some assembly code which can be correctly executed under either 16 or
32 bit segments and to make sure the interrupt vector is hooked out without
DOS4GW's interrupt code segment translation intervening.

Here is an example for hooking out the timer interrupt.  Note the use of
"volatile" on the Timer_Tick variable declaration.  This is an often over
looked detail in this sort of asynchronous kind of programming.

  #include <dos.h>
  #include <stdio.h>
  #include <conio.h>

  volatile int Timer_Tick = 0;	      /* Must be "volatile" */

  void __interrupt New_Timer_ISR(void);
  void (__interrupt * Old_Timer_ISR)(void);

  void __interrupt New_Timer_ISR(void)
  {
     Timer_Tick ++;
     Old_Timer_ISR();		  /* Chain back to original interrupt */
  }

  /*  Ordinarily using INT 01Ch is the preferred method, but INT 08h can
   *  also be used.
   */

  #define TIMER_INTERRUPT (0x1C)

  void main(void)
  {
     Old_Timer_ISR = _dos_getvect( TIMER_INTERRUPT );
     _dos_setvect( TIMER_INTERRUPT, New_Timer_ISR );

     do
     {
	printf( "%d\n", Timer_Tick );
     } while( !kbhit() );

     _dos_setvect( TIMER_INTERRUPT, Old_Timer_ISR );
  }

If understanding is your goal then keep the following in mind:

- DOS4GW initializes most interrupt vectors to point into the extender
itself.  These new vectors are just a translation layer through DPMI to
switch into real mode and call the old vector.

- Since function pointers (including those declared as __interrupt) are
32 bit offsets, the standard old DOS set/get vector int 21h functions will
not work in the standard old way.  Using _dos_getvect and _dos_setvect is
really the only reliable way to do ISR management.

- When a new 32 bit ISR is installed, no translation layer is installed
because it is not needed.

- DOS4GW emulates the old real mode interrupts as closely as possible by
preserving the regular register values across the interrupt translation
layer.	However, the segment registers are clearly not preserved.  This is
not only because a direct Selector->Segment translation makes no sense but
because CS, and SS, must be set up to simulate a 16 bit context.

- DOS4GW implements its own version of int 21h, which looks very much
like a 32 bit extension of int 21h.  This is why it is referred to as a
"DOS Extender".  In these cases places where there might be Selector->
Segment translation issues, there has been a work around to use the _FLAT
model paradigm.  See the online documentation for more information.

- When you are *required* to pass a segment value to a 16 bit interrupt
routine, you need to use the DPMI 300h "Simulated interrupt" function.

Other DOS extenders such as CauseWay and Pmode/w work in similar ways.

----------------------------------------------------------------------------


Q30. How do I get rid of the DOS4GW banner?  How do I bind DOS4GW.EXE to
my application?  How do I get rid of the external DOS4GW.EXE altogether?
What other DOS extenders can I use?

A30. To get rid of the DOS4GW startup banner, bind your application to
"wstubq.exe" instead of the default "wstub.exe", or alternatively write
your own wstub.exe which does a "set DOS4GW=QUIET".

You cannot simply get rid of DOS4GW.EXE.  You need some sort of DOS
extender to drive your 32 bit app.  DOS4GW.EXE does not give you a link
option however other extenders such as DOS4G (the professional version of
DOS4GW), CauseWay, and PMODE (available at the x2ftp site) are plug in
DOS4GW compatible DOS extenders that will let you link the extender right
to your code.  This way, from the user perspective, your application is no
different from old 16 bit .exe's out there.  This is very important for
those interested in making tools that are easily transportable in a stand
alone form.

The other main advantage of using DOS Extenders *other* than DOS4GW, is
that all of them seem to support the DPMI "800" call, which is essential
for mapping physical memory to linear memory so that you can access all of
graphics memory for some SVGA cards directly.

WATCOM's 3rd party support home page includes other DOS extenders that
work with their compiler.  Pointers to these can all be found either
directly from a www search engine or on the WATCOM www page.  Here's a
quick comparison of what is out there that I am aware of.  If you have
information to add or about other extenders (such as blinker, DOS4G)
please tell me so I can include it.

Pharlap/TNT - Large proven API, which supersets DPMI.  The extender is
external and a separate link process is required, but WATCOM supports it
as part of the basic compiler environment.  I don't know the current going
rate, but last I checked it had a $500 price tag.

PMode/W v1.30 - Small, fast, links to exe, supports exe compression, plug
in compatible with DOS4GW and is free for development stages with a $500
flat charge if supplied with a shipping product.  Its very small and has
been used in a number of projects.  The latest version has implemented
exception handling, a major drawback of previous versions.  For more
information, check out http://www.di.net/pmw/

CauseWay - Fast, extended DPMI API, supports exe compression, links to final
exe, unlimited virtural memory (I accidentally allocated 800MB of memory
once, and since I had the hard drive space the app didn't complain!), plug
in compatible with DOS4GW, $200.  This is a real no nonsense dos extender
which truly supersets DOS4GW (error handling included, unlike pmodew) and
which is at least as robust. The WATCOM debugger can be used with
uncompressed CauseWay extended apps (be sure to follow the CauseWay
installation instructions carefully), but CauseWay comes with its own
debugger that can deal with compressed apps as well. It comes with an
informative manual and plenty of sample code for assembly and WATCOM C/C++
usage (including examples of using VESA SVGA.) For more information, check
out http://www.devoresoftware.com/

DOS/4G - What DOS4GW was based on.  It allows linking to your executable and
TSR programming.  It appears limited to 64MB.  They claim to be the extender
used for Lotus 1-2-3, and thus guarantee system compatibility.	There is
varying pricing, but the main Watcom C/C++ upgrade option appears to cost
$999.  See http://www.tenberry.com/ for more information.

----------------------------------------------------------------------------


Q31. How do I make WATCOM C/C++ work with the latest OS/2 Toolkit?

A31. I know nothing about WATCOM's OS/2 support, however Eric Lekven has
given me the following information:

  WATCOM does not ship its compiler with the newest OS/2 Toolkit and must
  be obtained on the IBM OS/2 Developer Connection CDROM or direct from
  IBM.	Unfortunately both require payment, although there have been
  promotional versions of the Developer Connection CDROM shipped with Dr
  Dobb's Journal magazine which contain the Toolkit for free.  Perhaps
  they will do this again when OS/2 Merlin ships.

  In order to use IBM's OS/2 Warp Toolkit header files you need to edit
  three of the files.

    os2def.h:
       Find the two identical lines containing
       #if defined(__IBMC__) || defined(__IBMCPP__)
       Edit these two lines to contain this
       #if defined(__IBMC__) || defined(__IBMCPP__) || defined(__WATCOMC__)

    mmio.h, mmioos2.h:
       Both of these have two lists of #defines, one for country codes,
       the other for keyboard codes.  The values are decimal.  However
       these files are non-ANSI in that these decimal numbers have leading
       zeroes.	Delete the leading zeroes from all these #define
       statements.

Breckan Morris has verified this and recommends this over compiling with
the stack interface (/4s or /5s) which would also solve the problem.  He
also adds that there is also a line of the form:

  #if __IBMC__ || __IBMCPP__

which should be changed to

  #if __IBMC__ || __IBMCPP__ || __WATCOMC__

According to Vance Palodichuk, OS/2 4.0 (aka Merlin) has fixed the non-ANSI
leading zeros error from their mmio*.h files, and the modification listed by
Breckan above is sufficient.  I.e., you just have to add a "|| __WATCOMC__"
to one line of the os2def.h file to make it work.

It makes me wonder why IBM didn't simply add this modification themselves
just to make it that much easier to use the OS/2 toolkit.  This has to be
contrasted with Microsoft and Intel who have clearly gone out of their way
to support WATCOM C/C++.  It seems surprising to me that the relationship
between WATCOM and IBM isn't better since they have clearly been beneficial
to each other.

----------------------------------------------------------------------------


Q32. Are there any other WATCOM C/C++ caveats?

A32. Here are some things I've learned while playing with WATCOM C/C++:

1) Structure declarations that include shorts or chars are highly unlikely
to be in physically packed offsets.  This is because WATCOM pads entries
to be a dword in size.	This is within ANSI C specifications which only
requires that offsets be in increasing order as they are declared.  Watcom
has a switch "/zp{1,2,4,8}" which allows you to change this behavior as
desired.

2) ISR's do not properly load ES.  Hence you should insert a #pragma aux
macro (that looks something like: "push ds pop es") if you are using any
library functions.  Autoinitialization of arrays or structures is out of
the question, since WATCOM C/C++ uses rep movsd.

3) To mix use of floating point in an ISR and in your main thread you must
save the floating point state (fsave? fstore?) at the beginning of your
ISR and restore it at the end (frestore?).  This is because the state of
the FPU cannot be reliably known when the ISR is executed (the stack may
be full from FPU code in the main thread).  Because saving and restoring the
FPU state is so costly, I would recommend against using the FPU entirely,
within ISRs.

4) DOS4GW ISR's do not share the same stack with your main application.
This means that there are certain function calls that will not work:
longjmp and exit are the ones that immediately come to mind.  (Of course
any non-reentrant functions are off limits as well, but those are standard
ISR restrictions.)

5) DOS4GW has no provisions for TSR programming, however it is possible with
the professional version, DOS4G and possibly other extenders.  (I am not
aware of these capabilities in other DOS extenders.)

6) The #pragma aux inline assembly language directive is limited to
assembly sequences of up to 127 bytes or 255 bytes depending on the
version of the compiler.  WATCOM recommends using external assembly for
longer assembly sequences.  Charlie Wallace makes the good observation
that you can deal with this easily by breaking up your inline fragments
into pieces:

  void setup(void);
  #pragma aux setup =	\
  " mov edi,0xa0000 "	\
  " mov ecx,2000    "	\
  modify [edi ecx];

  void copy(void);
  #pragma aux copy =	\
  " loop1:          "	\
  " mov [edi],0     "	\
  " inc edi         "	\
  " dec ecx         "	\
  " jnz loop1       "	\
  modify [edi ecx];

  void main(void) 
  {
	setup();
	copy();
  }

The "modify" statements above are identical (the code generator is smart
enough to see that it doesn't need to restore then save the registers
between setup and copy), but as usual, they must describe the registers
modified in each inline #pragma.  If your flow logic is complicated in
such a way that you might jump from one #pragma to another, then you
should include all potentially modified registers for the collection of
#pragma's, in each #pragma's modify clause.

7) Although mentioned elsewhere, I might as well reiterate that WATCOM has
by default implemented "char" as "unsigned char".  This can be overridden
with the /j switch, and the IDE does this by default.

8) Apparently the WATCOM C/C++ compiler has difficulty on DOS systems with
64MB of memory.  This problem does not occur with Windows DOS boxes.  The
solution, according to Charlie Wallace, is to replace w32run.exe (in
either %WATCOM%\binw or %WATCOM%\bin, depending on the which version you
are using) with one of tntrun.exe, d4grun.exe or x32run.exe (i.e., use the
copy command; if things screw up you can always quickly incrementally
reinstall from the CDROM.)  Apparently, the one which works is system
dependent.

9) ES is assumed to be equal to DS and the direction flag is assumed
clear.	For autoinitialization, structure copying and many library
functions, WATCOM C/C++ simply uses the "rep movsd" assembly instruction
which means that the direction flag must be clear and ES, must be set to
the _FLAT selector, as they are by default.

WATCOM does not allow you to add (E)SP, (E)BP, or segment registers to a
"modify clause" in a #pragma.  So if they are modified (by a BIOS call, or
by some inline assembly) you have to save and restore those registers
yourself.  Similarly the direction flag should be explicitly cleared with
the "cld" instruction if it is potentially set.

10) WATCOM C/C++'s stack, by default, is only 16K.  Depending on how you
write your code, this can be very limiting.  Basically, you should try to
avoid declaring arrays inside your functions, but rather declare them
statically, outside your functions.  If you need the array to be local
for recursive (or context) reasons you can either simulate your own stack
or simply expand the stack using the "stack" linker directive.	Check the
on line help for more information.

11) WATCOM C/C++, by default, does not equate "enum"s to "int"s, but
rather to chars if the number of the enums is less than 256.  As described
in the WATCOM documentation, ANSI is ambiguous about this behavior.  I
believe this optimization is unique to WATCOM's compiler. Clearly it can
cause problems for parameters and structure sizes for libraries,
especially if the libraries have been built for/by other compilers.  The
"-ei" compiler switch will force enums to be equated to "int"s.  (Thanks
Intel, for reminding me of this caveat!)

----------------------------------------------------------------------------


Q33. Will WATCOM C/C++ support MMX?

A33. Yes.  Intel made the following announcement in March 1996:

  "Powersoft Corporation has announced a commitment to deliver a version
  of Watcom C/C++ that supports Intel's new MMX(tm) technology. This
  support will enable development of lightning fast applications that
  exploit the capabilities of the MMX instruction set and that leverage
  the industry-leading optimization technology of Watcom C/C++. Watcom
  C/C++ will feature complete debugging support (including MMX technology
  register display and the disassembly of MMX instructions) as well as a
  functional interface from C/C++ that generates inline MMX instructions."

For more information about MMX and WATCOM C/C++ see:

  http://developer.intel.com/drg/mmx/Tools/watcom.htm

----------------------------------------------------------------------------


Q34. So what is your story?  Why did you do this?

A34. I was a summer hire for WATCOM many eons ago, and I appreciate that
they gave me my first real job.  I have always been impressed by their
compilers and I've always wanted to see them crush both Borland and
Microsoft in the compiler arena.  (It looks like Borland will be laying
down for the count, but Microsoft is still there of course.)

WATCOM's most notable weakness is user support.  Their documentation is
very dry and generally does not fully explain all of the compiler's
capabilities.  I'm told this is the main reason a very high profile game
company stopped using their compiler in favor of another 32 bit DOS based
C compiler.  I've also found that lately, I'd been posting on a daily
basis just answering people's basic questions about WATCOM C/C++.  It is
in the hopes of curing this problem that I've written up this FAQ.

Finally, I am hoping that people make contributions and possibly submit
corrections so that I can verify my own knowledge.  This process also
results in a more reliable FAQ.  Numerous contributions have been made
thus far (including one from a WATCOM employee, and two from an Intel
employee representing Intel), and I have received a lot of positive
feedback which has made it worth while.

----------------------------------------------------------------------------


Cool contributions
~~~~~~~~~~~~~~~~~~

C01.  Profiling and debugging all rolled into one using an external
terminal connected by serial.  Contributed by Charlie Wallace.

[PH - The following contribution describes an alternative method for
profiling and debugging using a serial port connection to another
machine. I believe the idea is to redefine default the epilogue and
prologue code for your functions to communicate with a debugging or
profiling device.  The details were too complicated for me to verify.
However, for advanced WATCOM C/C++ users looking for an alternative
debugging or profiling method, the following may be useful.]

This is a small tip on using /ee /ep /en options of watcom, usually
these are used by the watcom profiler. I`ve found them useful for
debugging in the past, especially when you can`t load into the
debugger, like when watcom debugger had no support for X32,
what I`d do was compile with those options which adds a call
before and after every function call, so when the code locked
up I knew roughly where it was happening, i made the __PRO
routine emit the function name to the serial port at a
preset speed to a terminal, with a tab added at every level
entered, then the __EPI routine removed a tab and a new line
so on the serial terminal it looked thus :

	main(i)
		my_func1(i)
			my_func2(i)
			my_func2(o)
		my_func1(o)
	main(o)

where (i) and (o) are in and out, when the code locked up
only the last call entered would be displayed.

there are many other uses of this function, profiling with
other hardware, remember though this changes the execution
flow, you can compensate for it though, making a simple
debugger, debug options, watchdogs for memory leak and
code overwrites and so on . . .

However one caveat is to make sure you don`t add profile
hooks to the profiler code, write it in asm in a separate
asm file, (don`t use #pragma aux), and don`t call
a C function of your own that has been compiled with /ep /en,
otherwise it`ll recurse.

a small example of how it works, assuming a simple compile
with wcl386 /ep /ee /en test.c pro.asm

where test.c is something like , simple..

int foo(int a)
{
	return a-1;
}

main()
{
	int a;
	a=foo(1);
}

which is compiled to

	db	'foo'
	db	3
_foo:
	push	ebp
	mov	ebp,esi
	call	__PRO

	; does the function

	call	_EPI
	pop	ebp
	ret

	db	'main'
	db	4
_main:
	push	ebp
	mov	ebp,esi
	call	__PRO

	; code goes here..

	call	_EPI
	pop	ebp
	ret


so get the function name, get the return address, esp will point to
it, subtract the length of the code before the __PRO call, ie the
push ebp + mov ebp,esi and add 1, use wdisasm to calculate this,
usually its 9, but I`m not saying it always will be, so check.
this byte is the length of the function name, subtract this from
the previous address just calculated and this gives you the
function name + length, ready to print out, or do whatever you
like..


Here is a 32bit version of __PRO and __EPI, for 32bit flat mode.


pro.asm - snip here 8-X

;
; use this fragment to code your own routines.
;
	.586p

	.model flat

	.code

	PUBLIC __PRO
	PUBLIC __EPI

; this routine gets called first
;
; usually like this, *- means subtract from current offset
;
;function_name:
;		db	'my_c_function'
;		db	*-function_name  (compute length of prev string)
;my_c_function:
;		push	ebp
;		mov	ebp,esp
;		call	__PRO
;
;

__PRO proc near
	push	esi
	push	eax

	mov	esi,[esp]

	sub	esi,9	; this is the length of the
			; call __PRO+mov ebp,esp+push ebp+1
			; it may need to be changed to match different
			; compile options. check with wdisasm

	xor	eax,eax
	mov	byte ptr al,[esi]	; length of function name
	sub	esi,eax
					; esi -> function name
					; eax -> length of string

; do what you will with these, EXCEPT ! call any function in C
; that you compiled with /ep or /en, otherwise it`ll recurse
; for ever (at least for a while anyway).


	pop	eax
	pop	esi
	ret

__PRO endp


; this gets called at the end of the function
; usually like this :-
;
;	call	__EPI
;	pop	ebp
;	ret

__EPI proc near

; do what you will here, just save all the reg's
	ret

__EPI endp


	end

pro.asm - snip here 8-X

----------------------------------------------------------------------------


C02. Convert WATCOM's help file format to a text file for easier
     reading and potentially for easier manipulation.

Joergen Bech sent me a program which will convert WATCOM's .IHP files to
a text file, a table of contents file and an index file.  This will allow
you to peruse the file from your favorite text editor, or manipulate it
as you see fit.  The program is freely available at:

    http://www.geocities.com/SiliconValley/9498/ihp_conv.zip

It assumes you have DOS4GW.EXE at your disposal, in your path.

----------------------------------------------------------------------------


Thanks for the feedback!
========================

I have received numerous emails in response to my creation and public
dissemination of the WATCOM C/C++ FAQ.	They have, for the most part,
been very encouraging.	The contributions and corrections that have been
sent to me have served to increase the quality of the FAQ tremendously.
Readers can rest assure knowing that the accuracy and content is being
examined very carefully by discerning WATCOM C/C++ users.

I would like to especially thank WATCOM Systems and Intel corp. for
their invaluable individual contributions.  I am honored to receive
their attention and grateful to include the information they have
provided.

I would also like to thank Dion Barrier, Gavin Estey and Patrick Evans
for including web links to the WATCOM C/C++ FAQ on their very prominent
programming WWW pages.

----------------------------------------------------------------------------


Request for information
=======================

I've tried to include everything I can, but this FAQ is clearly lacking in
several areas.	Open areas:

 - Information on Windows, OS/2 or QNX programming.
 - Writing ROMs or startup code.
 - Debugging with and using the PharLap DOS extender.
 - Does Direct X require WATCOM C/C++ v10.5+, or can it be used with 10.0?
 - Which versions of WATCOM debugger work with Win 95 and Win NT 3.51?
 - Info on dealing with using TASM/WASM and the C/C++ compiler together.
 - Info on sampling and profiling code written with WASM.

Got anything to add?  Suggestions?  Don't hesitate to send them my way.

----------------------------------------------------------------------------


Paul Hsieh
qed@chromatic.com
Graphics programmer

What I say and what my company says are not always the same thing.

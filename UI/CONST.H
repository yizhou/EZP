// #define REGIST_VERSION         // for secret
#ifdef REGIST_VERSION
  #define ReportMemoryError(str)
#endif

#define FILEVERSION        0x0200             /* Version 1.00 */
#define FILE_EXT_NAME      ".EZP"

#define UsePrintCut() (PG.Enable&&PG.Blocks)
#define SCRX (SXScale*XScale)
#define SCRY (SYScale*YScale)

#define PRNX ((!GlobalRorate90) ? (PXScale*XScale) : (PYScale*XScale))
#define PRNY ((!GlobalRorate90) ? (PYScale*YScale) : (PXScale*YScale))

/* Constant define */
#define GetDimBorder()        (GlobalExtFormat.dim3&0xf)
#define GetDimColor()         ((GlobalExtFormat.dim3&0xf0)>>4)
#define GetDimDir()           ((GlobalExtFormat.dim3&0x300)>>8)
#define GetHollowColor()      ((GlobalExtFormat.hollow&0xf0)>>4)
#define GetHollowBorder()     (GlobalExtFormat.hollow&0xf)


#define SetDimBorder(v)        GlobalExtFormat.dim3=((GlobalExtFormat.dim3&0xfff0)|(v&0xf))
#define SetHollowColor(v)      GlobalExtFormat.hollow=((GlobalExtFormat.hollow&0xff0f)|((v&0xf)<<4))
#define SetHollowBorder(v)     GlobalExtFormat.hollow=((GlobalExtFormat.hollow&0xfff0)|(v&0xf))
#define SetDimDir(v)           GlobalExtFormat.dim3=((GlobalExtFormat.dim3&0xfcff)|((v&3)<<8))
#define SetDimColor(v)         GlobalExtFormat.dim3=((GlobalExtFormat.dim3&0xff0f)|((v&0xf)<<4))

#define MaxPaperPara 6
#define Error(ErrorNumber) return(ErrorNumber)
#define ReturnOK() return(OpOK)
#define HandleUnlock(Handle)
enum { NO_PREV, DOT_PREV, LINE_PREV };
enum {FOOTTOP,FOOTBOTTOM};
enum {FOOTLEFT,FOOTRIGHT,FOOTLEFT_RIGHT,FOOTMID};
enum {HEADLINE_NONE,HEADLINE_SINGLE,HEADLINE_DOUBLE};

#define PAGEHEADSTRMAXLEN   60       /* see type.h::FileHeads.PageHeadStr */
#define SYMBOL_LIB      127

 #define PI 3.1415926
 #define TRUE 1
 #define FALSE 0
 #define NEG_ONE             -1
 #define HALF_ONE            (((UINT)NEG_ONE >>1)+1)
 #define UINTMAX             ((UINT)NEG_ONE)

#define OpOK 0
#define INVAILEDPARAM -1
#define TOOMANYPOLYGONS -2
#define TOOMANYPOINTS -3
#define TOOFEWPOINTSINPOLYGON -4
#define TOOMANYMESSAGE -5
#define TOOMANYWINDOWS -6
#define GRAPHICSOP -7
#define MOUSEINIT -8
#define OUTOFMEMORY -9
#define INVAILEDQQ -10
#define INVAILEDWW -11
#define INVAILEDNN -12
#define INVAILEDLIB -13
#define TOOMANYMENU -14
#define TOOMANYCHARS -15
#define TOOMANYLISTITEMS -16
#define MENUNOTFOUND -17
#define FILEOPEN -40
#define FILEREAD -41
#define FILEWRITE -42
#define LIBFORMAT -43
#define TMPOPEN -44
#define TMPREAD -45
#define TMPWRITE -46
#define FILEFORMAT -47
#define TOOMANYITEMS -48
#define INVALIDVERSION -49
#define SYSTEMQUIT -100

/*-------------------- UI -------------------------*/
#define SYSBUTTONWIDTH 20
#define SYSSCROLLWIDTH 15
#define LINESPACE 3

#define MAKELONG(HI,LO) ((unsigned long)(((unsigned long)(HI)<<16)&0xffff0000l)|(long)((unsigned long)(LO)&0xffff))
#define MAKELO(X) (((unsigned long)(X))&0xffffl)
#define MAKEHI(X) ((((unsigned long)(X))>>16)&0xffffl)

//#define FP2LONG(XX) ((unsigned long)MAKELONG(FP_SEG((void *)XX),FP_OFF((void *)XX)))
//#define LONG2FP(XX) (MK_FP((unsigned)MAKEHI(XX),(unsigned)MAKELO(XX)))
//#define LONG2FP(XX) ((char *) MK_FP(FP_SEG(XX),XX) )
#define FP2LONG(XX) ((unsigned long) XX )
#define LONG2FP(XX) ((char *) XX )

/////// System Data and Code //////////
#define NULLMESSAGE 0
#define REDRAWMESSAGE 1
#define WINDOWQUIT 2
#define WINDOWQUITCONFIRM 3
#define MOUSEMOVE 4
#define MOUSELEFTDROP 5
#define MOUSERIGHTDROP 6
#define MOUSELRDROP 7
#define TIMERTRIGGER 8
#define DRAWWINDOW 9
#define LOSTFOCUS 10
#define GETFOCUS 11
#define KEYSTRING 12
#define WINDOWSIZE 13
#define MOUSELEFTDOWNON 14
#define WINDOWCLOSE 15
#define WINDOWMOVE 16
#define WINDOWRESIZE 17
#define WMPAINT 18
#define WINDOWINIT 19
#define MENUINIT 20
#define KEYDOWN 21
#define KEYUP 22
#define MOUSELEFTDOWN 23
#define MOUSELEFTUP 24
#define MOUSERIGHTDOWN 25
#define MOUSERIGHTUP 26
#define MOUSELRDOWN 27
#define MOUSELRUP 28
#define MOUSELEFTDOUBLE 29
#define MOUSERIGHTDOUBLE 30
#define MOUSELRDOUBLE 31
#define WINDOWHHSCROLL 32
#define WINDOWVVSCROLL 33
#define WINDOWLEFTSCROLL 34
#define WINDOWRIGHTSCROLL 35
#define WINDOWUPSCROLL 36
#define WINDOWDOWNSCROLL 37
#define HHSCROLLMOVE 38
#define VVSCROLLMOVE 39
#define WINDOWMINIUM 40
#define MOUSERIGHTDOWNON 41
#define MOUSELRDOWNON 42
#define COMBOPULL 43
#define ITEMSELECT 44
#define GETSTATUS 45
#define SETSTATUS 46
#define DIALOGBOXEND 47
#define DIALOGBOXOK  48
#define DIALOGBOXCANCEL 49
#define MESSAGEBOXOK 50
#define MESSAGEBOXCANCEL 51
#define MESSAGEBOXNODO 52
#define GETLINEBUFFER 53
#define SETLINEBUFFER 54
#define WINDOWSYSMENU 55
#define LISTAPPENDITEM 56
#define LISTINSERTITEM 57
#define LISTDELETEITEM 58
#define LISTDELETELASTITEM 59
#define LISTSETITEMLENGTH 60
#define LISTSETITEMHEIGHT 61
#define LISTSETTOTALITEM 62
#define LISTGETCURRENTSELECT 63
#define LISTSETCURRENTSELECT 64
#define LISTINSERTITEMSORTED 65
#define LISTBOXCONFIRM 66
#define DIRCHANGE      70
#define FILECHANGE     71

#define MENUCOMMAND 80

#define SELECTSELECTED 90
#define SELECTUNSELECTED 91

#define STATICTEXTBKCOLOR 95

#define GETWINDOWMINWIDTH 100
#define GETWINDOWMINHEIGHT 101
#define DIALOGBOXEXIT 102

#define MOUSELEFTDOWN_ALT       110
#define MOUSELEFTDOWN_SHIFT     111
#define MOUSELEFTDOWN_CTRL      112
#define MOUSELEFTDOWNON_ALT       113
#define MOUSELEFTDOWNON_SHIFT     114
#define MOUSELEFTDOWNON_CTRL      115
#define MOUSELEFTDROP_ALT       116
#define MOUSELEFTDROP_SHIFT     117
#define MOUSELEFTDROP_CTRL      118
#define MOUSELEFTUP_ALT       119
#define MOUSELEFTUP_SHIFT     120
#define MOUSELEFTUP_CTRL      121
#define BUBLEHINT             122
#define DELBUBLE              123
#define GETLOGFILE            124
#define GETDISKSERIAL         125
#define REGISTERROR           126
#define WASTEMESSAGE          127
#define PULLDOWN              128

#define SYSTEMIDLE 1000

/////// Window ///////////
#define MAXWINDOWS 200

#define WindowInsertData() ActiveWindow++
#define WindowDeleteData() ActiveWindow--
#define WindowSetProcedure(WN,P) DataofWindows[WN].Procedure=P
#define WindowSetLeft(WN,X) DataofWindows[WN].Left=X
#define WindowSetTop(WN,Y) DataofWindows[WN].Top=Y
#define WindowSetRight(WN,X) DataofWindows[WN].Right=X
#define WindowSetBottom(WN,Y) DataofWindows[WN].Bottom=Y
#define WindowSetDC(WN,X) DataofWindows[WN].DC=X
#define WindowSetFather(WN,X) DataofWindows[WN].FatherWindow=X
#define WindowSetNext(WN,X) DataofWindows[WN].NextBrotherWindow=X
#define WindowSetPrev(WN,X) DataofWindows[WN].PrevBrotherWindow=X
#define WindowSetChild(WN,X) DataofWindows[WN].NextChildWindow=X
#define WindowSetStyle(WN,X) DataofWindows[WN].WindowStyle=X
#define WindowSetActive(WN,X) DataofWindows[WN].ActiveChildWindow=X
#define WindowSetTitle(WN,X) strcpy(DataofWindows[WN].Title,X)
#define WindowSetNoUse(WN) DataofWindows[WN].Procedure=NULL
#define WindowSetStatus(WN,X) { DataofWindows[WN].WindowStyle&=(long)(0xFF3FFFFFl); \
                              DataofWindows[WN].WindowStyle|=(long)(((long)X)<<22); }
#define WindowSetUserData(WN,X) (DataofWindows[WN].UserData=X)
#define WindowGetProcedure(WN) (DataofWindows[WN].Procedure)
#define WindowGetLeft(WN) (DataofWindows[WN].Left)
#define WindowGetTop(WN) (DataofWindows[WN].Top)
#define WindowGetRight(WN) (DataofWindows[WN].Right)
#define WindowGetBottom(WN) (DataofWindows[WN].Bottom)
#define WindowGetWidth(WN) (DataofWindows[WN].Right-DataofWindows[WN].Left+1)
#define WindowGetHeight(WN) (DataofWindows[WN].Bottom-DataofWindows[WN].Top+1)
#define WindowGetUserWidth(WN) (DataofWindows[WN].UserRight-DataofWindows[WN].UserLeft+1)
#define WindowGetUserHeight(WN) (DataofWindows[WN].UserBottom-DataofWindows[WN].UserTop+1)
#define WindowGetDC(WN) (DataofWindows[WN].DC)
#define WindowGetFather(WN) (DataofWindows[WN].FatherWindow)
#define WindowGetNext(WN) (DataofWindows[WN].NextBrotherWindow)
#define WindowGetPrev(WN) (DataofWindows[WN].PrevBrotherWindow)
#define WindowGetChild(WN) (DataofWindows[WN].NextChildWindow)
#define WindowGetStyle(WN) (DataofWindows[WN].WindowStyle)
#define WindowGetActive(WN) (DataofWindows[WN].ActiveChildWindow)
#define WindowGetTitle(WN) DataofWindows[WN].Title
#define WindowCanUse(WN) (DataofWindows[WN].Procedure==NULL)
#define WindowGetUserData(WN) (DataofWindows[WN].UserData)
#define WindowIsFatherless(WN) (DataofWindows[WN].FatherWindow==0)
#define WindowIsChildless(WN) (DataofWindows[WN].NextChildWindow==0)
#define WindowIsPrevless(WN) (DataofWindows[WN].PrevBrotherWindow==0)
#define WindowIsNextless(WN) (DataofWindows[WN].NextBrotherWindow==0)

#define WindowCanMoveable(WN) (!(DataofWindows[WN].WindowStyle&0x00000001l))
#define WindowCanResizeable(WN) (!(DataofWindows[WN].WindowStyle&0x00000002l))
#define WindowCanMiniumable(WN) (DataofWindows[WN].WindowStyle&0x00000004l)
#define WindowCanMaxiumable(WN) (DataofWindows[WN].WindowStyle&0x00000008l)
#define WindowHasSystemMenu(WN) (DataofWindows[WN].WindowStyle&0x00000010l)
#define WindowHasVScroll(WN) (DataofWindows[WN].WindowStyle&0x00000020l)
#define WindowHasHScroll(WN) (DataofWindows[WN].WindowStyle&0x00000040l)
// #define WindowCanLineEdit(WN) (DataofWindows[WN].WindowStyle&0x00000080l)
// #define WindowCanEdit(WN) (DataofWindows[WN].WindowStyle&0x00000100l)  //ByHance
#define WindowIconMustBeUp(WN) (DataofWindows[WN].WindowStyle&0x00000080l)
#define WindowIsIcon(WN) (DataofWindows[WN].WindowStyle&0x00000100l)
#define WindowCanTabOrder(WN) (DataofWindows[WN].WindowStyle&0x10000000l)

#define WindowIsMiniumButton(WN) (DataofWindows[WN].WindowStyle&0x00000200l)
#define WindowIsMaxiumButton(WN) (DataofWindows[WN].WindowStyle&0x00000400l)
#define WindowIsSystemMenuButton(WN) (DataofWindows[WN].WindowStyle&0x00000800l)
#define WindowIsVScroll(WN) (DataofWindows[WN].WindowStyle&0x00001000l)
#define WindowIsHScroll(WN) (DataofWindows[WN].WindowStyle&0x00002000l)
#define WindowIsUserButton(WN) (DataofWindows[WN].WindowStyle&0x00004000l)
#define WindowIsSingleLineEditor(WN) (DataofWindows[WN].WindowStyle&0x00008000l)
#define WindowIsMutilLineEditor(WN) (DataofWindows[WN].WindowStyle&0x00010000l)
#define WindowIsUserWindow(WN) (DataofWindows[WN].WindowStyle&0x00200000l)
#define WindowGetStatus(WN) (((long)(DataofWindows[WN].WindowStyle&0x00C00000l))>>22)
#define WindowHasMenu(WN) (DataofWindows[WN].WindowStyle&0x01000000l)
#define WindowIsVVScroll(WN) (DataofWindows[WN].WindowStyle&0x00020000l)
#define WindowIsHHScroll(WN) (DataofWindows[WN].WindowStyle&0x00040000l)
#define WindowIsLeftScroll(WN) (DataofWindows[WN].WindowStyle&0x00080000l)
#define WindowIsRightScroll(WN) (DataofWindows[WN].WindowStyle&0x00100000l)
#define WindowIsUpScroll(WN) (DataofWindows[WN].WindowStyle&0x02000000l)
#define WindowIsDownScroll(WN) (DataofWindows[WN].WindowStyle&0x04000000l)
#define WindowIsTitleBar(WN) (DataofWindows[WN].WindowStyle&0x08000000l)
#define WindowIsTopWindow(WN) (DataofWindows[WN].WindowStyle&0x20000000l)
#define WindowIsFocusAlways(WN) (DataofWindows[WN].WindowStyle&0x40000000l)
#define WindowIsMenuWindow(WN) (DataofWindows[WN].WindowStyle&0x80000000l)

#define WindowSetMoveable() (0x00000001l)
#define WindowSetResizeable() (0x00000002l)
#define WindowSetMiniumable() (0x00000004l)
#define WindowSetMaxiumable() (0x00000008l)
#define WindowSetHasSystemMenu() (0x00000010l)
#define WindowSetHasVScroll() (0x00000020l)
#define WindowSetHasHScroll() (0x00000040l)
// #define WindowSetCanLineEdit() (0x00000080l)
// #define WindowSetCanEdit() (0x00000100l)  // ByHance
#define WindowSetIconMustBeUp() (0x00000080l)
#define WindowSetIsIcon() (0x00000100l)
#define WindowSetCanTabOrder() (0x10000000l)
#define WindowSetIsMiniumButton() (0x00000200l)
#define WindowSetIsMaxiumButton() (0x00000400l)
#define WindowSetIsSystemMenuButton() (0x00000800l)
#define WindowSetIsVScroll() (0x00001000l)
#define WindowSetIsHScroll() (0x00002000l)
#define WindowSetIsUserButton() (0x00004000l)
#define WindowSetHasMenu() (0x01000000l)
#define WindowSetIsSingleLineEditor() (0x00008000l)
#define WindowSetIsMutilLineEditor() (0x00010000l)
#define WindowSetIsListBox() (0x00020000l)
#define WindowSetIsCombo() (0x00040000l)
#define WindowSetIsFileList() (0x00080000l)
#define WindowSetIsDirList() (0x00100000l)
#define WindowSetIsUserWindow() (0x00200000l)
#define WindowSetIsVVScroll() (0x00020000l)
#define WindowSetIsHHScroll() (0x00040000l)
#define WindowSetIsLeftScroll() (0x00080000l)
#define WindowSetIsRightScroll() (0x00100000l)
#define WindowSetIsUpScroll() (0x02000000l)
#define WindowSetIsDownScroll() (0x04000000l)
#define WindowSetIsTitle() (0x08000000l)
#define WindowSetIsTopWindow (0x20000000l)
#define WindowSetIsFocusAlways (0x40000000l)
#define WindowSetIsMenuWindow (0x80000000l)
#define BUTTONISDOWN   1
#define BUTTONGETFOCUS 2

///////// Message //////////
#define MAXMERGEMESSAGE 42
#define MAXMESSAGES 300

#define MessageIsEmpty(MessageNumber) (DataofMessages[MessageNumber].Message==0)
#define MessageSetQueueHead(X) MessageQueueHead=X
#define MessageSetQueueTail(X) MessageQueueTail=X
#define MessageGetQueueHead() MessageQueueHead
#define MessageGetQueueTail() MessageQueueTail
#define MessageQueueIsEmpty() (MessageGetQueueHead()==MessageGetQueueTail())

#define MessageSetWindow(MessageNumber,X) DataofMessages[MessageNumber].Window=X
#define MessageSetMessage(MessageNumber,X) DataofMessages[MessageNumber].Message=X
#define MessageSetData(MessageNumber,X) DataofMessages[MessageNumber].Data=X
#define MessageSetExtra(MessageNumber,X) DataofMessages[MessageNumber].ExtraData=X
#define MessageSetNext(MessageNumber,X) DataofMessages[MessageNumber].Next=X

#define MessageGetWindow(MessageNumber) (DataofMessages[MessageNumber].Window)
#define MessageGetMessage(MessageNumber) (DataofMessages[MessageNumber].Message)
#define MessageGetData(MessageNumber) (DataofMessages[MessageNumber].Data)
#define MessageGetExtra(MessageNumber) (DataofMessages[MessageNumber].ExtraData)
#define MessageGetNext(MessageNumber) (DataofMessages[MessageNumber].Next)

//////// Mouse  ///////////////
#define ARRAWMOUSE  0                   // normal arrow
#define FINGERMOUSE 1                   // finger
#define VCARETMOUSE 2                   // vertical edit caret
#define BROWSEHANDMOUSE 3               // browse hand
#define MOVEARROW       4               // move arrow
#define ZOOMMOUSE       5               // zoom
#define MOVINGMOUSE     6               // moving car
#define LINKMOUSE       7               // link
#define UNLINKMOUSE     8               // unlink
#define HRESIZEMOUSE    9               // h resize
#define VRESIZEMOUSE    10              // v resize
#define LOCKMOUSE       11              // lock mouse
#define RESIZEMOUSE1    12              // resize 1
#define RESIZEMOUSE2    13              // resize 2
#define MOVINGGRAYMOUSE  14             // gray car
#define CUPMOUSE1       15              // cup with word
#define CUPMOUSE2       16              // cup without word
#define FINGERMOUSE2    17              // point finger
#define CROSSMOUSE      18              // before rotating, select Center(x,y)
#define ROTATEMOUSE     19
#define BUSYMOUSE       20              // busy, when FileAccess, Printing

#define MAXMOUSESHAPE   21

//////// Chinese ////////////
#define CHAR_WIDTH_IN_DOT   256

#define CHINESELIBNAME "C:\\EZP\\FONTS\\HZK16"
#define CHINESELIBLENGTH 243648l
#define CHINESEHIGHT 16
#define CHINESEWIDTH 16
#define DOT_W_BYTE   (CHINESEWIDTH/8)
#define DOT_HEIGHT   (CHINESEHIGHT)

#define CHINESESIZE  ( (CHINESEWIDTH/8)*CHINESEHIGHT )
#define CHINESECHARSIZE ( CHINESEWIDTH/8 )

#define TOOSMALLSIZE    10
#define ASC16LIBNAME "C:/EzP/FoNtS\\asC16"
#define ASC16HIGHT 16
#define ASC16WIDTH 8
#define ASC16SIZE (((ASC16WIDTH*ASC16HIGHT)>>3)/sizeof(unsigned char))
#define MAXASC16CODE 128

#define ASC32LIBNAME "C:/EzP/FoNtS\\asC32"
#define ASC32WIDTH 14
#define ASC32HIGHT 14
#define ASC32SIZE    ( ASC32HIGHT*((ASC32WIDTH+7)/8) )
#define MAXASC32CODE (0x7e-0x21+1)
#define MAXEFONT   8

////////// Color /////////////
#define CROSSCOLOR EGA_LIGHTRED
#define FILLCOLOR EGA_WHITE
#define FILLCOLOR2 EGA_BLACK
#define SELECTCOLOR EGA_LIGHTRED
#define POLYGONUNSELECTCOLOR EGA_WHITE
#define POINTSELECTCOLOR EGA_WHITE
#define POLYGONCOLOR1 EGA_LIGHTRED
#define POLYGONCOLOR2 EGA_LIGHTGREEN
#define POINTSELECTCOLOR EGA_WHITE
#define BASELINECOLOR EGA_BROWN
#define INFOMATIONBKCOLOR EGA_WHITE
#define INFOMATIONCOLOR EGA_LIGHTRED
#define HELPBOLDCOLOR EGA_GREEN
#define HELPCOLOR EGA_YELLOW
#define HELPBKCOLOR EGA_LIGHTBLUE
#define STATUSCROSSCOLOR EGA_LIGHTRED
#define STATUSCOLOR EGA_LIGHTBLUE
#define STATUSBKCOLOR EGA_YELLOW
#define POLYGONSTATUSCOLOR EGA_WHITE

#define WINDOWBOLDCOLOR EGA_BLACK
#define WINDOWBOLDFILLCOLOR EGA_LIGHTGRAY
#define WINDOWTITLECOLOR EGA_WHITE
#define WINDOWTITLEBKCOLOR EGA_BLUE
#define WINDOWSCROLLBKCOLOR EGA_LIGHTGRAY
#define WINDOWBKCOLOR EGA_WHITE

#define BUTTONCOLOR EGA_BLACK
#define BUTTONBKCOLOR EGA_LIGHTGRAY
#define BUTTONBOLDCOLOR1 EGA_WHITE
#define BUTTONBOLDCOLOR2 EGA_BLACK

#define BUTTONPOLYGONCOLOR EGA_BLACK
#define BUTTONPOLYGONBKCOLOR EGA_BLACK

#define LISTCOLOR EGA_BLACK
#define LISTBKCOLOR EGA_WHITE

#define LISTBOXCOLOR EGA_BLACK
#define LISTBOXBKCOLOR EGA_WHITE
#define LISTBOXBOLDCOLOR EGA_BLACK

#define SINGLELEBKCOLOR EGA_WHITE
#define SINGLELECOLOR EGA_BLACK

#define COMBOBUTTONCOLOR EGA_BLACK

#define FRAMECOLOR EGA_BLACK
#define FRAMEBKCOLOR EGA_LIGHTGRAY

#define RADIOBUTTONCOLOR FRAMECOLOR
#define RADIOBUTTONBKCOLOR FRAMEBKCOLOR

#define STATICTEXTCOLOR EGA_BLACK

#define MESSAGECOLOR EGA_BLACK
#define MESSAGEBKCOLOR EGA_LIGHTGRAY

#define BOXBORDERCOLOR EGA_BLACK
#define PAGEBORDERCOLOR EGA_BLACK

/////////// Dialog /////////////
#define GLOBALITEM 0
#define USERBUTTONITEM 1
#define COMBOXITEM 2
#define SINGLELINEEDITORITEM 3
#define LISTBOXITEM 4
#define FRAMEITEM 5
#define HSCROLLITEM 6
#define SINGLESELECT 7
#define MULTISELECT 8
#define OKBUTTON 9
#define CANCELBUTTON 10
#define STATICTEXTITEM 11

#define MAXMESSAGEDISPLAYLENGTH 192

///////// Icon ////////////
#define TotalIconNumber 20
#define BmpImageLen     (TOOLBARWIDTH*TOOLBARHEIGHT/8*4+6)
#define BmpBufLen       (BmpImageLen*TotalIconNumber*2)

enum {
   IDX_NEW,     IDX_OPEN,   IDX_SAVE,      IDX_IMPORT,
   IDX_FONT,    IDX_SIZE,   IDX_SELECTBOX, IDX_INPUTBOX,  IDX_ROTATE, IDX_ZOOM,
   IDX_TEXTBOX, IDX_RECBOX, IDX_CIRBOX,    IDX_ELIBOX,    IDX_PLGBOX,
   IDX_TABLE,   IDX_LINE,   IDX_LINK,      IDX_UNLINK,    IDX_PRINT
};   /*- Total 20 icons -*/

////////////// Information /////////
#define ERRORINFORM 0
#define WARNINGINFORM 1
#define HELLOINFORM 2

#define MAXINFORM 100

#define INVALIDROTATEANGLE 0
#define INVALIDROTATEAXISX 1
#define INVALIDROTATEAXISY 2
#define INVALIDBOXLEFT 3
#define INVALIDBOXTOP 4
#define INVALIDBOXWIDTH 5
#define INVALIDBOXHEIGHT 6
#define INVALIDBOXTEXTLEFTDISTANT 7
#define INVALIDBOXTEXTTOPDISTANT 8
#define INVALIDBOXTEXTRIGHTDISTANT 9
#define INVALIDBOXTEXTBOTTOMDISTANT 10
#define INVALIDBOXROUNDLEFTDISTANT 11
#define INVALIDBOXROUNDTOPDISTANT 12
#define INVALIDBOXROUNDRIGHTDISTANT 13
#define INVALIDBOXROUNDBOTTOMDISTANT 14
#define INVALIDCOLUMN 15
#define INVALIDCOLUMNDISTANT 16
#define INVALIDPAGEWIDTH 17
#define INVALIDPAGEHEIGHT 18
#define INVALIDPAGEMARGINLEFT 19
#define INVALIDPAGEMARGINTOP 20
#define INVALIDPAGEMARGINRIGHT 21
#define INVALIDPAGEMARGINBOTTOM 22
#define FILENOTSAVE 23
#define FILELOADERROR 24
#define FILESAVEERROR 25
#define INVALIDCHARFONT 26
#define INVALIDCHARSIZE 27
#define INVALIDCHARHSIZE 28
#define INVALIDCHARSLANTANGLE 29
#define INVALIDCHARCOLOR 30
#define FILENAMEERROR 31
#define TOOMUCHREGION 32
#define INVALIDMARGIN 33

////////////// KeyBoard //////////////////
#define CTRL_A 1
#define CTRL_B 2
#define CTRL_C 3
#define CTRL_D 4
#define CTRL_E 5
#define CTRL_F 6
#define CTRL_G 7
#define CTRL_H 8
#define CTRL_I 9
#define CTRL_J 10
#define CTRL_K 11
#define CTRL_L 12
#define CTRL_M 13
#define CTRL_N 14
#define CTRL_O 15
#define CTRL_P 16
#define CTRL_P 16
#define CTRL_Q 17
#define CTRL_R 18
#define CTRL_S 19
#define CTRL_T 20
#define CTRL_U 21
#define CTRL_V 22
#define CTRL_W 23
#define CTRL_X 24
#define CTRL_Y 25
#define CTRL_Z 26

#define BACKSPACE 8
#define TAB 9
#define ENTER 13
#define ESC 27
#define BLANK 32

#define F1 315
#define F2 316
#define F3 317
#define F4 318
#define F5 319
#define F6 320
#define F7 321
#define F8 322
#define F9 323
#define F10 324
#define F11 389
#define F12 390

#define UP   328
#define DOWN 336
#define LEFT 331
#define RIGHT 333
#define HOME 327
#define END 335
#define PGDN 337
#define PGUP 329
#define INS 338
#define DEL 339

#define CTRL_F1 350
#define CTRL_F2 351
#define CTRL_F3 352
#define CTRL_F4 353
#define CTRL_F5 354
#define CTRL_F6 355
#define CTRL_F7 356
#define CTRL_F8 357
#define CTRL_F9 358
#define CTRL_F10 359

#define ALT_F1 360
#define ALT_F2 361
#define ALT_F3 362
#define ALT_F4 363
#define ALT_F5 364
#define ALT_F6 365
#define ALT_F7 366
#define ALT_F8 367
#define ALT_F9 368
#define ALT_F10 369

#define ALT_0 385
#define ALT_1 376
#define ALT_2 377
#define ALT_3 378
#define ALT_4 379
#define ALT_5 380
#define ALT_6 381
#define ALT_7 382
#define ALT_8 383
#define ALT_9 384

#define ALT_A 286
#define ALT_B 304
#define ALT_C 302
#define ALT_D 288
#define ALT_E 274
#define ALT_F 289
#define ALT_G 290
#define ALT_H 291
#define ALT_I 279
#define ALT_J 292
#define ALT_K 293
#define ALT_L 294
#define ALT_M 306
#define ALT_N 305
#define ALT_O 280
#define ALT_P 281
#define ALT_Q 272
#define ALT_R 275
#define ALT_S 287
#define ALT_T 276
#define ALT_U 278
#define ALT_V 303
#define ALT_W 273
#define ALT_X 301
#define ALT_Y 277
#define ALT_Z 300

#define ALT_LEFT   411
#define ALT_RIGHT  413
#define ALT_UP     408
#define ALT_DOWN   416

#define RIGHT_CTRL_BLANK (0x20+256)
#define ALT_BACKSPACE (0x0e+256)
#define SHIFT_DEL (0x0200+DEL)
#define CTRL_INS (0x92+256)
#define SHIFT_INS (0x0200+INS)
#define CTRL_DEL (0x93+256)
#define ALT_BLANK (0x0800+32)
#define SHIFT_TAB (0x10F)
#define CTRL_TAB (TAB+0x400)
#define SHIFT_CTRL_TAB (SHIFT_TAB+0x400)

#define SHIFT_LEFT (LEFT+0x200)
#define SHIFT_RIGHT (RIGHT+0x200)
#define SHIFT_UP (UP+0x200)
#define SHIFT_DOWN (DOWN+0x200)
#define SHIFT_PGUP (PGUP+0x200)
#define SHIFT_PGDN (PGDN+0x200)
#define SHIFT_HOME (HOME+0x200)
#define SHIFT_END (END+0x200)
#define SHIFT_CTRL_LEFT (CTRL_LEFT+0x200)
#define SHIFT_CTRL_RIGHT (CTRL_RIGHT+0x200)
#define SHIFT_CTRL_HOME (CTRL_HOME+0x200)
#define SHIFT_CTRL_END (CTRL_END+0x200)
#define SHIFT_CTRL_PGUP (CTRL_PGUP+0x200)
#define SHIFT_CTRL_PGDN (CTRL_PGDN+0x200)
#define SHIFT_CTRL_UP (CTRL_UP+0x200)
#define SHIFT_CTRL_DOWN (CTRL_DOWN+0x200)


#define CTRL_LEFT 371
#define CTRL_RIGHT 372
#define CTRL_UP (0x8d+256)
#define CTRL_DOWN (0x91+256)
#define CTRL_PGDN 374
#define CTRL_PGUP 388
#define CTRL_HOME 375
#define CTRL_END 373

///////// ListBox ////////////
#define LISTDELETEALL 1200
#define MAXLISTBOXS 10                  //By zjh ora=5

#define ListHandleFree() (CurrentAllocList--)
#define WindowList(WW) (WindowGetUserData(WW))
#define ListGetTotal(HH) (TestList[HH].TotalList)
#define ListGetTop(HH) (TestList[HH].TopList)
#define ListGetCurrent(HH) (TestList[HH].CurrentList)
#define ListGetItemLength(HH) (TestList[HH].ItemLength)
#define ListGetItems(HH) (TestList[HH].ItemStrings)
#define ListGetItem(HH,YY) (ListGetItems(HH)+(YY)*ListGetItemLength(HH))
#define ListGetHeight(HH) (TestList[HH].ListHeight)
#define ListGetHSaveImage(HH) (TestList[HH].HSaveImage)

#define ListSetTotal(HH,YY) TestList[HH].TotalList=YY
#define ListDecTotal(HH,YY) TestList[HH].TotalList-=YY
#define ListIncTotal(HH,YY) TestList[HH].TotalList+=YY
#define ListSetTop(HH,YY) TestList[HH].TopList=YY
#define ListDecTop(HH,YY) TestList[HH].TopList-=YY
#define ListIncTop(HH,YY) TestList[HH].TopList+=YY
#define ListSetCurrent(HH,YY) TestList[HH].CurrentList=YY
#define ListDecCurrent(HH,YY) TestList[HH].CurrentList-=YY
#define ListIncCurrent(HH,YY) TestList[HH].CurrentList+=YY
#define ListSetItemLength(HH,YY) TestList[HH].ItemLength=YY
#define ListSetItems(HH,YY) memset(TestList[HH].ItemStrings,YY,sizeof(TestList[HH].ItemStrings))
#define ListSetItem(HH,YY,XX) strncpy(ListGetItem(HH,YY),XX,ListGetItemLength(HH))
#define ListSetHeight(HH,YY) (TestList[HH].ListHeight=YY)
#define ListSetHSaveImage(HH,YY) (TestList[HH].HSaveImage=YY)

////////////// Menu ///////////
#define MAXMENUS 256

#define SPACEHEIGHT 8
#define CHARWIDTH 16
#define CHARHEIGHT 16
#define MENUWIDTH 128
#define MENUWINDOWLEFTSPACE 20
#define MENUWINDOWHSPACE 70
#define MENUWINDOWVSPACE 4

#define MENUSELECTCOLOR WHITE
#define MENUSELECTBKCOLOR EGA_DARKGRAY
#define MENUCOLOR BLACK
#define MENUBKCOLOR WHITE
#define MENUDISABLECOLOR EGA_LIGHTGRAY

#define MenuGetFather(M) (DataofMenus[M].FatherMenu)
#define MenuGetChild(M) (DataofMenus[M].ChildMenu)
#define MenuGetNext(M) (DataofMenus[M].NextMenu)
#define MenuGetPrev(M) (DataofMenus[M].PrevMenu)
#define MenuGetHeight(M) (DataofMenus[M].MenuHeight)
#define MenuGetType(M) (DataofMenus[M].MenuType)
#define MenuGetName(M) (DataofMenus[M].MenuName)
#define MenuGetShortChar(M) (DataofMenus[M].MenuShortChar)
#define MenuGetUseSign(M) (DataofMenus[M].MenuUseSign)
#define MenuGetShortKey(M) (DataofMenus[M].MenuShortKey)
#define MenuGetReturnValue(M) (DataofMenus[M].MenuReturnValue)
#define MenuGetSaveImage(M) (DataofMenus[M].HSaveImage)

#define MenuSetFather(M,X) (DataofMenus[M].FatherMenu=X)
#define MenuSetChild(M,X) (DataofMenus[M].ChildMenu=X)
#define MenuSetNext(M,X) (DataofMenus[M].NextMenu=X)
#define MenuSetPrev(M,X) (DataofMenus[M].PrevMenu=X)
#define MenuSetHeight(M,X) (DataofMenus[M].MenuHeight=X)
#define MenuSetType(M,X) (DataofMenus[M].MenuType=X)
#define MenuSetName(M,X) (strcpy(DataofMenus[M].MenuName,X))
#define MenuSetUseSign(M,X) (DataofMenus[M].MenuUseSign=X)
#define MenuSetShortChar(M,X) (DataofMenus[M].MenuShortChar=X)
#define MenuSetShortKey(M,X) (DataofMenus[M].MenuShortKey=X)
#define MenuSetReturnValue(M,X) (DataofMenus[M].MenuReturnValue=X)
#define MenuSetSaveImage(M,X) (DataofMenus[M].HSaveImage=X)
#define MenuSetUse(M) (DataofMenus[M].MenuUseSign=1)
#define MenuSetNoUse(M) (DataofMenus[M].MenuUseSign=0)

#define MenuSetToBox() 1
#define MenuSetToImage() 2
#define MenuSetToChecked() 4
#define MenuSetToSpaceBar() 8
#define MenuSetToDisable() 16

#define MenuIsBox(MM) (MenuGetType(MM)&1)
#define MenuIsText(MM) ((MenuGetType(MM)&2)==0)
#define MenuIsChecked(MM) (MenuGetType(MM)&4)
#define MenuIsSpaceBar(MM) (MenuGetType(MM)&8)
#define MenuIsDisable(MM) (MenuGetType(MM)&16)

#define MenuSetBoxAttr(M) MenuSetType(M,MenuGetType(M)|MenuSetToBox())
#define MenuSetImageAttr(M) MenuSetType(M,MenuGetType(M)|MenuSetToImage())
#define MenuSetCheckedAttr(M) MenuSetType(M,MenuGetType(M)|MenuSetToChecked())
#define MenuSetSpaceBarAttr(M) MenuSetType(M,MenuGetType(M)|MenuSetToSpaceBar())
#define MenuSetDisableAttr(M) MenuSetType(M,MenuGetType(M)|MenuSetToDisable())
#define MenuSetNoCheckedAttr(M) MenuSetType(M,MenuGetType(M)&~MenuSetToChecked())
#define MenuSetEnableAttr(M) MenuSetType(M,MenuGetType(M)&~MenuSetToDisable())
#define MenuCanUseable(hmenu) (MenuGetUseSign(hmenu)==0)

////////// Radio button ////////////
#define RADIOBUTTON 0x4000
#define RADIOBUTTONSELECT 0x8000
#define FRAMEBUTTON 0
#define CIRCLERADIUS 5
#define FOCUSRADIUS 3

#define RadioGetGroup(WW) ((WindowGetUserData(WW)&0x3f00)>>8)
#define RadioGetOrder(WW) (WindowGetUserData(WW)&0xff)

///////// Single Line Editor //////////
#define CURSORBLINKTIME 4
#define GETTEXTWIDTH 201

#define MAXSINGLELINEBUFFER 60     // 15

#define FreeLineBuffer(X) (CurrentAllocLine--)
#define GetLineBuffer(HH) (TestLineBuffer[HH])
#define GetLineBufferLength(HH) (GetLineBuffer(HH).StringLength)
#define GetLineLengthAddress(HH) (&(GetLineBuffer(HH).StringLength))
#define GetLineBufferLeftStart(HH) (GetLineBuffer(HH).LeftStart)
#define GetLineCursor(HH) (GetLineBuffer(HH).CursorX)
#define GetLineBufferString(HH) (GetLineBuffer(HH).String)
#define GetLineBufferChar(HH,NN) (GetLineBufferString(HH)[NN])
#define GetEditorWidth(HH) (GetLineBuffer(HH).BoundX)

#define SetLineBuffer(HH,X) memset(&TestLineBuffer[HH],X,sizeof(SingleLine))
#define SetLineBufferLength(HH,X) (GetLineBuffer(HH).StringLength=X)
#define IncLineBufferLength(HH,X) (GetLineBuffer(HH).StringLength+=X)
#define DecLineBufferLength(HH,X) (GetLineBuffer(HH).StringLength-=X)
#define SetLineBufferLeftStart(HH,X) (GetLineBuffer(HH).LeftStart=X)
#define IncLineBufferLeftStart(HH,X) (GetLineBuffer(HH).LeftStart+=X)
#define DecLineBufferLeftStart(HH,X) (GetLineBuffer(HH).LeftStart-=X)
#define SetLineCursor(HH,X) (GetLineBuffer(HH).CursorX=X)
#define IncLineCursor(HH,X) (GetLineBuffer(HH).CursorX+=X)
#define DecLineCursor(HH,X) (GetLineBuffer(HH).CursorX-=X)
#define SetLineBufferString(HH,X) (strcpy(GetLineBuffer(HH).String,X))
#define SetLineBufferChar(HH,NN,X) (GetLineBufferString(HH)[NN]=X)
#define SetEditorWidth(HH,X) (GetLineBuffer(HH).BoundX=X)

/////////// UserAttribute /////////
#define TmpAttribute (TmpBox.TmpAttr.UserAttribute1)
#define TmpAttribute2 (TmpBox.TmpAttr.UserAttribute2)
#define TmpAttribute3 (TmpBox.TmpAttr.UserAttribute3)
#define TmpAttribute4 (TmpBox.TmpAttr.UserAttribute4)

/////////// UserBox /////////
#define TmpTextBox (TmpBox.TmpTextBox)
#define TmpFormBox (TmpBox.TmpFormBox)
#define TmpImageBox (TmpBox.TmpPictureBox)

////////// UserDialog ////////////
//#define MODIFYPAGE 20000
//#define MODIFYBOX 20001
//#define NEWFILE 20002

////////// UserFunction //////////////
#define TOOLCHANGE 30001
#define CanOperatePage() (PolygonNumber==0&&RotateSign==0)
enum { NoRotateSign, XYSelectSign, RotateEdgeSelectSign };
enum { NoMoveSign,MovingSign,ResizingSign,GroupMovingSign,GroupSelectSign,\
       GroupDefineSign };             // ByHance


////////// Profile ////////////
enum{SET,GET,INT,STRING};
enum{NO,SECTIONFOUND,ENTRYFOUND,OK};

/*-------------------- KERNL -------------------------*/
//////////// File //////////
#define LIBRARYHEADSTRING "REDTEK EZ Publisher Library. Version 1.00\n\x1a"

/////////// Fixed ////////
#define Long2Fixed(f,a)   {(f).value = (short)(a >> 16); (f).fract = (unsigned short)(a&0xffff);}
#define Int2Fixed(f,a)    {(f).value = a; (f).fract = 0;}
#define Fixed2Long(f)     (((long)(f).value<<16)|(long)(f).fract)
#define Fixed2Float(f)    (float)((float)(Fixed2Long(f))/0x10000L)
#define Float2Fixed(f,v)  Long2Fixed(f,((long)(v*0x10000L)))
#define Fixed2Double(f)   (double)((double)Fixed2Long(f)/0x10000L)
#define Double2Fixed(f,v) Float2Fixed(f,v)
#define IntofFixed(f)     ((f).value)
#define Frac(f)           ((f).fract)
#define LCos(a)           LSin(90+a)

/////////// Group ///////////
#define GroupGetBox(GG,II) (GG.GroupHBox[II])
#define GroupGetTopBox(GG) (GG.GroupHBox[--GG.GroupSumBox])
#define GroupGetLeft(GG) (GG.GroupLeft)
#define GroupGetTop(GG) (GG.GroupTop)
#define GroupGetWidth(GG) (GG.GroupWidth)
#define GroupGetHeight(GG) (GG.GroupHeight)
#define GroupGetRight(GG) (GG.GroupLeft+GG.GroupWidth)
#define GroupGetBottom(GG) (GG.GroupTop+GG.GroupHeight)
#define GroupGetRotateAngle(GG) (GG.GroupRotateAngle)
#define GroupGetRotateAxisX(GG) (GG.GroupRotateAxisX)
#define GroupGetRotateAxisY(GG) (GG.GroupRotateAxisY)
#define GroupGetSumBox(GG) (GG.GroupSumBox)
#define GroupGetSign(GG) (GG.GroupSign)
#define GroupGetPage(GG) (GG.GroupPage)

#define GroupSetBox(GG,II,BB) (GG.GroupHBox[II]=BB)
#define GroupSetTopBox(GG,XX) (GG.GroupHBox[GG.GroupSumBox++]=XX)
#define GroupSetLeft(GG,XX) (GG.GroupLeft=XX)
#define GroupSetTop(GG,XX) (GG.GroupTop=XX)
#define GroupSetWidth(GG,XX) (GG.GroupWidth=XX)
#define GroupSetHeight(GG,XX) (GG.GroupHeight=XX)
#define GroupSetRotateAngle(GG,XX) (GG.GroupRotateAngle=XX)
#define GroupSetRotateAxisX(GG,XX) (GG.GroupRotateAxisX=XX)
#define GroupSetRotateAxisY(GG,XX) (GG.GroupRotateAxisY=XX)
#define GroupSetSumBox(GG,XX) (GG.GroupSumBox=XX)
#define GroupSetSign(GG) (GG.GroupSign=1)
#define GroupClearSign(GG) (GG.GroupSign=0)
#define GroupSetPage(GG,XX) (GG.GroupPage=XX)

#define GlobalGroupGetBox(II) GroupGetBox(GlobalGroup,II)
#define GlobalGroupGetTopBox() GroupGetTopBox(GlobalGroup)
#define GlobalGroupGetLeft() GroupGetLeft(GlobalGroup)
#define GlobalGroupGetTop() GroupGetTop(GlobalGroup)
#define GlobalGroupGetWidth() GroupGetWidth(GlobalGroup)
#define GlobalGroupGetHeight() GroupGetHeight(GlobalGroup)
#define GlobalGroupGetRight() GroupGetRight(GlobalGroup)
#define GlobalGroupGetBottom() GroupGetBottom(GlobalGroup)
#define GlobalGroupGetRotateAngle() GroupGetRotateAngle(GlobalGroup)
#define GlobalGroupGetRotateAxisX() GroupGetRotateAxisX(GlobalGroup)
#define GlobalGroupGetRotateAxisY() GroupGetRotateAxisY(GlobalGroup)
#define GlobalGroupGetSumBox() GroupGetSumBox(GlobalGroup)
#define GlobalGroupGetSign() GroupGetSign(GlobalGroup)
#define GlobalGroupGetPage() GroupGetPage(GlobalGroup)

#define GlobalGroupSetBox(II,BB) GroupSetBox(GlobalGroup,II,BB)
#define GlobalGroupSetTopBox(XX) GroupSetTopBox(GlobalGroup,XX)
#define GlobalGroupSetLeft(XX) GroupSetLeft(GlobalGroup,XX)
#define GlobalGroupSetTop(XX) GroupSetTop(GlobalGroup,XX)
#define GlobalGroupSetWidth(XX) GroupSetWidth(GlobalGroup,XX)
#define GlobalGroupSetHeight(XX) GroupSetHeight(GlobalGroup,XX)
#define GlobalGroupSetRotateAngle(XX) GroupSetRotateAngle(GlobalGroup,XX)
#define GlobalGroupSetRotateAxisX(XX) GroupSetRotateAxisX(GlobalGroup,XX)
#define GlobalGroupSetRotateAxisY(XX) GroupSetRotateAxisY(GlobalGroup,XX)
#define GlobalGroupSetSumBox(XX) GroupSetSumBox(GlobalGroup,XX)
#define GlobalGroupSetSign() GroupSetSign(GlobalGroup)
#define GlobalGroupClearSign() GroupClearSign(GlobalGroup)
#define GlobalGroupSetPage(XX) GroupSetPage(GlobalGroup,XX)

#define GroupClear(GG) (GG.GroupSumBox=0)
#define GlobalGroupClear() GroupClear(GlobalGroup)
#define UnGroup(GG) { GroupClear(GG); GroupClearSign(GG); GroupSetPage(GG,0); }
#define GlobalUnGroup() UnGroup(GlobalGroup)

///////// Items ////////
#define MAXITEMS 500
#define ItemGetPrev(XX) (DataofItems[XX].PrevItem)
#define ItemGetNext(XX) (DataofItems[XX].NextItem)
#define ItemGetChild(XX) (DataofItems[XX].ChildItem)
#define ItemGetFather(XX) (DataofItems[XX].FatherItem)
#define ItemGetHandle(XX) (DataofItems[XX].ItemHandle)

#define ItemSetPrev(XX,YY) (DataofItems[XX].PrevItem=YY)
#define ItemSetNext(XX,YY) (DataofItems[XX].NextItem=YY)
#define ItemSetChild(XX,YY) (DataofItems[XX].ChildItem=YY)
#define ItemSetFather(XX,YY) (DataofItems[XX].FatherItem=YY)
#define ItemSetHandle(XX,YY) (DataofItems[XX].ItemHandle=YY)

#define ItemIsPrevless(XX) (ItemGetPrev(XX)==0)
#define ItemIsNextless(XX) (ItemGetNext(XX)==0)
#define ItemIsChildless(XX) (ItemGetChild(XX)==0)
#define ItemIsFatherless(XX) (ItemGetFather(XX)==0)

#define ItemCanUse(XX) (ItemGetPrev(XX)==0 \
                        &&ItemGetNext(XX)==0 \
                        &&ItemGetFather(XX)==0 \
                        &&ItemGetChild(XX)==0 \
                        &&ItemGetHandle(XX)==0)
#define ItemSetCanUse(XX) { \
                            ItemSetPrev(XX,0); \
                            ItemSetNext(XX,0); \
                            ItemSetFather(XX,0); \
                            ItemSetChild(XX,0); \
                            ItemSetHandle(XX,0); \
                          }

///////// KnlEnv ////////
#define SCALEMETER 1000
#define SCREENDPI 100

#define TEXTBLOCKSIZE 4096

#define MAXFONTNUMBER 1024
#define MAXCFONTNUMBER 300
#define SONGTI 0

#define MAXCHARFACT  80
#define MAXPAGEWIDTH 11000     //By zjh 10.29
#define MAXPAGEHEIGHT 11000    //By zjh 10.29
#define MINCHARSIZE 0.01
//#define MAXCHARSIZE 1024     By zjh
#define MAXCHARSIZE MAXCHARFACT*512     //By zjh  10.29

#define MINCHARHSIZE 0.01
//#define MAXCHARHSIZE 1024    By zjh
#define MAXCHARHSIZE MAXCHARFACT*512    //By zjh  10.29

#define MINCHARSLANTANGLE -75
#define MAXCHARSLANTANGLE 75

#define MINCHARCOLOR 0
#define MAXCHARCOLOR 9

#define NOINCLUDESELF 0
#define INCLUDESELF 1

//#define DEFAULTBOXTEXTDISTANT (2*SCALEMETER/screendpi)
#define DEFAULTBOXTEXTDISTANT (4*SCALEMETER/screendpi)
#define DEFAULTBOXTABLEDISTANT (4*SCALEMETER/screendpi)

#define MINPAGESCALE (SCALEMETER*1/screendpi/4)
#define MAXPAGESCALE (SCALEMETER*1/screendpi*4)

#define MAXRESIZEXY 5
#define DELTASIZE  (MAXRESIZEXY*SCALEMETER*1/screendpi)

#define PAGETOPDISTANT   1000
#define PAGELEFTDISTANT  1000

#define MAX_TIMER_EVENT        5
#define DefaultPrintName        "EZPPRINT.tmp"
//#define DefaultPrintName        "PRN"

#define TOOLBARHEIGHT 24
#define TOOLBARWIDTH 24
#define RULERBARHEIGHT 16

#define INCH 0
#define CM 1

#define PointIsInRectangle(X,Y,X1,Y1,X2,Y2) (((X)>=(X1))&&((X)<(X2))&&((Y)>=(Y1))&&((Y)<(Y2)))

#define ToolBarHasToolBar() (GlobalToolBarSetting&0x1)
#define ToolBarHasRulerBar() (GlobalToolBarSetting&0x2)
#define ToolBarHasTablerBar() (GlobalToolBarSetting&0x4)
#define ToolBarHasGrid() (GlobalToolBarSetting&0x8)
#define ToolBarHasColorPattlete() (GlobalToolBarSetting&0x16)

#define ToolBarSetHasToolBar() (GlobalToolBarSetting|=0x1)
#define ToolBarSetHasRulerBar() (GlobalToolBarSetting|=0x2)
#define ToolBarSetHasTablerBar() (GlobalToolBarSetting|=0x4)
#define ToolBarSetHasGrid() (GlobalToolBarSetting|=0x8)
#define ToolBarSetHasColorPattlete() (GlobalToolBarSetting|=0x16)

#define ToolBarSetHasNoToolBar() (GlobalToolBarSetting&=~0x1)
#define ToolBarSetHasNoRulerBar() (GlobalToolBarSetting&=~0x2)
#define ToolBarSetHasNoTablerBar() (GlobalToolBarSetting&=~0x4)
#define ToolBarSetHasNoGrid() (GlobalToolBarSetting&=~0x8)
#define ToolBarSetHasNoColorPattlete() (GlobalToolBarSetting&=~0x16)

#define FileHasBeenLoaded() (GlobalFileStateSign&1)
#define FileHasBeenModified() (GlobalFileStateSign&2)
#define FileHasBeenSaved() (GlobalFileStateSign&4)
#define FileIsBeenLoading() (GlobalFileStateSign&16)

#define FileSetLoaded() (GlobalFileStateSign|=1)
#define FileSetModified() (GlobalFileStateSign=((GlobalFileStateSign|2)&~4))
#define FileSetSaved() (GlobalFileStateSign|=4)
#define FileSetBeenLoading() (GlobalFileStateSign|=16)

#define FileSetNotLoaded() (GlobalFileStateSign&=~1)
#define FileSetNotModified() (GlobalFileStateSign&=~2)
#define FileSetNotSaved() (GlobalFileStateSign&=~4)
#define FileSetDoneLoading() (GlobalFileStateSign&=~16)

#define FileSetMeterINCH() (GlobalFileStateSign&=~8)
#define FileSetMeterCM() (GlobalFileStateSign|=8)
#define FileGetMETER() ((GlobalFileStateSign&8)>>3)
#define FileMETERIsINCH() (FileGetMETER()==INCH)
#define FileMETERIsCM() (FileGetMETER()==CM)

#define TextIsOverwrite() (GlobalTextStatus&0x1)
#define TextSetOverwrite() (GlobalTextStatus|=0x1)
#define TextSetInsert() (GlobalTextStatus&=0xfe)
#define TextChangeInsertMode() {if (TextIsOverwrite()) TextSetInsert(); else TextSetOverwrite();}
#define CurrentBoxEditable() (GlobalTextStatus&0x2)
#define CurrentBoxSetEditable() (GlobalTextStatus|=0x2)
#define CurrentBoxSetNotEditable() (GlobalTextStatus&=(~0x2))
#define SetGlobalBrowser() (GlobalTextStatus|=0x4)
#define ClearGlobalBrowser() (GlobalTextStatus&=(~0x4))
#define IsInGlobalBrowser() (GlobalTextStatus&0x4)

#define GetInserttingCell() (GlobalTableCell)
#define SetInserttingCell(XX) (GlobalTableCell=XX)

///////// Line ////////
#define LineGetNext(XX) ItemGetNext(XX)
#define LineGetPrev(XX) ItemGetPrev(XX)

#define LineGetDescribeItem(XX) ((XX)->DescribeItem)
#define LineGetLineStyle(XX) ((XX)->LineStyle)
#define LineGetLineHead(XX) ((XX)->LineHead)
#define LineGetLineTail(XX) ((XX)->LineTail)
#define LineGetLineWidth(XX) ((XX)->LineWidth)
#define LineGetLineColor(XX) ((XX)->LineColor)
#define LineGetLineStatus(XX) ((XX)->LineStatus)
#define LineGetLineParameter(XX) ((XX)->LineParameter)

#define LineSetDescribeItem(XX,YY) ((XX)->DescribeItem=YY)
#define LineSetLineStyle(XX,YY) ((XX)->LineStyle=YY)
#define LineSetLineHead(XX,YY) ((XX)->LineHead=YY)
#define LineSetLineTail(XX,YY) ((XX)->LineTail=YY)
#define LineSetLineWidth(XX,YY) ((XX)->LineWidth=YY)
#define LineSetLineColor(XX,YY) ((XX)->LineColor=YY)
#define LineSetLineStatus(XX,YY) ((XX)->LineStatus=YY)
#define LineSetLineParameter(XX,YY) ((XX)->LineParameter=YY)

//////// Page //////////
/*------ ByHance, 96,3.8 -----*/
#define GetPageFootOption()      tmp_footflag
#define GetPageFootTopOption()   tmp_foottopflag
#define GetPageFootLeftOption()  tmp_footleftflag
#define GetPageFootPrevOption()  tmp_footprevflag
#define GetPageHeadOption()      tmp_headflag
#define GetPageHeadLeftOption()  tmp_headleftflag
#define GetPageHeadLineOption()  tmp_headlineflag

#define SetPageFootOption(X)       (tmp_footflag=(X))
#define SetPageFootTopOption(X)    (tmp_foottopflag=(X))
#define SetPageFootLeftOption(X)   (tmp_footleftflag=(X))
#define SetPageFootPrevOption(X)   (tmp_footprevflag=(X))
#define SetPageHeadOption(X)       (tmp_headflag=(X))
#define SetPageHeadLeftOption(X)   (tmp_headleftflag=(X))
#define SetPageHeadLineOption(X)   (tmp_headlineflag=(X))

#define PageGetBoxHead(XX) ItemGetChild(XX)
#define PageGetNext(XX) ItemGetNext(XX)
#define PageGetPrev(XX) ItemGetPrev(XX)

#define PageGetDescribeItem(XX) ((XX)->DescribeItem)
#define PageGetPageType(XX) ((XX)->PageType)
#define PageGetPageColumn(XX) ((XX)->PageColumn)
//#define PageGetPageFoot(XX) ((XX)->PageFoot)
#define PageGetVirtualPage(XX) ((XX)->VirtualPage)
//#define PageGetPrePageNumberString(XX) ((XX)->PrePageNumberString) // ???
#define PageGetMarginLeft(XX) ((XX)->MarginLeft)
#define PageGetMarginTop(XX) ((XX)->MarginTop)
#define PageGetMarginRight(XX) ((XX)->MarginRight)
#define PageGetMarginBottom(XX) ((XX)->MarginBottom)
#define PageGetColumnDistant(XX) ((XX)->ColumnDistant)
#define PageGetPageWidth(XX) ((XX)->PageWidth)
#define PageGetPageHeight(XX) ((XX)->PageHeight)

#define PageSetDescribeItem(XX,YY) ((XX)->DescribeItem=YY)
#define PageSetPageType(XX,YY) ((XX)->PageType=YY)
#define PageSetPageColumn(XX,YY) ((XX)->PageColumn=YY)
//#define PageSetPageFoot(XX,YY) ((XX)->PageFoot=YY)
#define PageSetVirtualPage(XX,YY) ((XX)->VirtualPage=YY)
//#define PageSetPrePageNumberString(XX,YY) ((XX)->PrePageNumberString=YY) // ???
#define PageSetMarginLeft(XX,YY) ((XX)->MarginLeft=YY)
#define PageSetMarginTop(XX,YY) ((XX)->MarginTop=YY)
#define PageSetMarginRight(XX,YY) ((XX)->MarginRight=YY)
#define PageSetMarginBottom(XX,YY) ((XX)->MarginBottom=YY)
#define PageSetColumnDistant(XX,YY) ((XX)->ColumnDistant=YY)
#define PageSetPageWidth(XX,YY) ((XX)->PageWidth=YY)
#define PageSetPageHeight(XX,YY) ((XX)->PageHeight=YY)

#define PageHaveInitialBox(XX) (!((XX).VirtualPage&0x80))

///////// Rotate ///////
#define DRAWANGLEINC 10
#define COMPUTEANGLEINC 30

///////// ClpBoard ///////
#define TEXTDATA 0
#define IMAGEDATA 1
#define BOXDATA 2
#define POLYGONDATA 3
#define GROUPDATA 4

#define GETTOPAGE 0
#define GETTOTEXTBOX 1

////// Undo /////////
#define CURSORGOTO 0
#define CURSORDEFINEBLOCK 1
#define CURSORUNDEFINEBLOCK 2
#define STRINGINSERT 3
#define STRINGDELETE 4
#define STRINGEXCHANGE 5

#define IMAGEMOVE 6
#define IMAGEROTATE 7
#define IMAGESKEW 8
#define IMAGEZOOM 9
#define IMAGENEGATIVE 10
#define IMAGECOLOR 11
#define IMAGECONTRAST 12
#define IMAGEINSERT 31
#define IMAGEDELETE 32

#define BOXMOVE 13
#define BOXRESIZE 14
#define BOXROTATE 15
#define BOXCHANGEPARAMETER 16
#define BOXCREAT 17
#define BOXDELETE 18
#define BOXEXCHANGE 19
#define BOXSELECT 20
#define BOXUNSELECT 21
#define BOXCHANGE 22

#define GROUPMOVE 23
#define GROUPCREAT 24
#define GROUPDELETE 25
#define GROUPALIGN 26

#define PAGEGOTO 27
#define PAGEINSERT 28
#define PAGEDELETE 29
#define PAGEEXCHANGE 30

#define TABLELINEMOVE 35
#define TABLECOLMOVE  36
#define TABLEINSERTLINE 37
#define TABLEINSERTCOL  38
#define TABLEDELETELINE 39
#define TABLEDELETECOL  40
#define TABLECELLMERGE  41
#define TABLEDISMERGE   42
#define TABLECHANGEHLINE 43
#define TABLECHANGEVLINE 44
#define TABLECHANGESLANT 45

#define COMPOSEOPERATE 100
#define MAXUNDOREDOBUFFER 20440

/////// Form ////////
enum CELLDIRECTION {LEFTCELL,RIGHTCELL,UPCELL,DOWNCELL};

////// Printer ///////
enum {  DEV_BW=0, DEV_RGB, DEV_CMYK, DEV_VGA,  };
 //--- follow is printer type ----
enum {                  // see also varc.c::PrinterName
     BJ_COLOR,
     BJ_BW_180,
     BJ_BW_360,
     M1724_BW_17,
     M1724_BW_8,
     EPSON_DJ720_COLOR_17,
     EPSON_DJ720_COLOR_8,
     EPSON_DJ360_COLOR,
     LQ_COLOR_17,
     LQ_BW_17,
     LQ_COLOR_8,
     LQ_BW_8,
     EPSON_DJ_BW,
     HP_DJ_COLOR,
     HP_DJ_BW,
     HP_LJ_A3_300,
     HP_LJ_A3_600,
     HP_LJ_A4_300,
     HP_LJ_A4_600,
     HP2_LJ_A4_300,
     LBP_LJ_A4_300,
     OKI_BW_17,
     OKI_BW_8,
     STAR_CR3240_17,
     STAR_AR3240_17,
     STAR_CR3240_8,
     STAR_AR3240_8,
     BJ_LJ_A3_300,
     BJ_LJ_A3_400,
     BJ_LJ_A3_600,
     BJ_LJ_A4_300,
     BJ_LJ_A4_400,
     BJ_LJ_A4_600,
     BJ_LJ_B4_300,
     BJ_LJ_B4_360,
     BJ_LJ_B4_400,
     BJ_LJ_B4_600,
     P6100_LJ_A4_300,
     P6500_LJ_A4_300,
     PIC_TIFF=1000,
     PIC_PCX,
     PIC_DCL,
     DEV_FAX=2000
};

////////// Box ///////////////
#define SLANTLINEWIDTH 20
//#define MAXFORMLINE 60
#define MAXLINENUMBER MAXFORMLINE
#define MAXCLOUMNNUMBER MAXFORMLINE

#define REVERSEDCLASS 3

#define TEXTBOX 0
#define TABLEBOX 1
#define LINEBOX 2
#define RECTANGLEPICTUREBOX 3
#define CORNERPICTUREBOX 4
#define ELIPSEPICTUREBOX 5
#define POLYGONPICTUREBOX 6
#define MATHBOX 7                      /* Reversed */
#define CHEMSTRYBOX 8                  /* Reversed */

#define TextBoxGetNext(XX) ItemGetNext(XX)
#define TextBoxGetPrev(XX) ItemGetPrev(XX)
#define TextBoxGetTextHead(XX) ItemGetChild(XX)

#define TextBoxGetBoxStatus(XX) ((XX)->BoxStatus)
#define TextBoxGetBoxType(XX) ((XX)->BoxType)
#define TextBoxGetBoxLeft(XX) ((XX)->BoxLeft)
#define TextBoxGetBoxTop(XX) ((XX)->BoxTop)
#define TextBoxGetBoxWidth(XX) ((XX)->BoxWidth)
#define TextBoxGetBoxHeight(XX) ((XX)->BoxHeight)
#define TextBoxGetBoxRight(XX) (TextBoxGetBoxLeft(XX)+(((TextBoxs *)(XX))->BoxWidth))
#define TextBoxGetBoxBottom(XX) (TextBoxGetBoxTop(XX)+(((TextBoxs *)(XX))->BoxHeight))
#define TextBoxGetRotateAngle(XX) ((XX)->RotateAngle)
#define TextBoxGetRotateAxisX(XX) ((XX)->RotateAxisX)
#define TextBoxGetRotateAxisY(XX) ((XX)->RotateAxisY)
#define TextBoxGetBoxBackColor(XX) ((XX)->BoxBackColor)
#define TextBoxGetTextAlign(XX) ((XX)->TextAlign)
#define TextBoxGetBoxColumn(XX) ((XX)->BoxColumn)
#define TextBoxGetColumnDistant(XX) ((XX)->ColumnDistant)
#define TextBoxGetTextDistantLeft(XX) ((XX)->TextDistantLeft)
#define TextBoxGetTextDistantTop(XX) ((XX)->TextDistantTop)
#define TextBoxGetTextDistantRight(XX) ((XX)->TextDistantRight)
#define TextBoxGetTextDistantBottom(XX) ((XX)->TextDistantBottom)
#define TextBoxGetRoundDistantLeft(XX) ((XX)->RoundDistantLeft)
#define TextBoxGetRoundDistantTop(XX) ((XX)->RoundDistantTop)
#define TextBoxGetRoundDistantRight(XX) ((XX)->RoundDistantRight)
#define TextBoxGetRoundDistantBottom(XX) ((XX)->RoundDistantBottom)
#define TextBoxGetTextLength(XX) ((XX)->TextLength)
#define TextBoxGetBlockLength(XX) ((XX)->TextBlockLength)
#define TextBoxGetVisualLength(XX) TextBoxGetTextLength(XX)
#define TextBoxGetTextHandle(XX) ((XX)->TextHandle)
#define TextBoxGetPrevLinkBox(XX) ((XX)->PrevLinkBox)
#define TextBoxGetNextLinkBox(XX) ((XX)->NextLinkBox)
#define TextBoxGetClipPolygons(XX) ((XX)->ClipPolygons)
#define TextBoxGetClipEdges(XX) ((XX)->ClipEdges)
#define TextBoxGetClipBoxXY(XX) ((XX)->ClipBoxXY)
#define TextBoxGetInvalidPolygons(XX) ((XX)->InvalidPolygons)
#define TextBoxGetInvalidEdges(XX) ((XX)->InvalidEdges)
#define TextBoxGetInvalidBoxXY(XX) ((XX)->InvalidBoxXY)

#define BoxIsLocked(XX) (TextBoxGetBoxStatus(XX)&1)
#define BoxSetLocked(XX) (TextBoxSetBoxStatus(XX,TextBoxGetBoxStatus(XX)|1))
#define BoxSetUnlocked(XX)  if (!BoxGetModuleStatus(XX))  \
             (TextBoxSetBoxStatus(XX,TextBoxGetBoxStatus(XX)&0xfe))

/*-----------By zjh For module file 96.9.12-----------*/
#define BoxGetModuleStatus(XX) ((XX)->BoxAttr&1)
#define BoxGetEditStatus(XX) ((XX)->BoxAttr&2)
#define BoxSetIsModule(XX) ((XX)->BoxAttr|=1)
#define BoxSetNotModule(XX) ((XX)->BoxAttr&=0xfffe)
#define BoxSetEditable(XX) ((XX)->BoxAttr|=2)
#define BoxSetNotEdit(XX) ((XX)->BoxAttr&=0xfffd)
#define BoxCanNotEdit(XX) (BoxGetModuleStatus(XX)&&BoxGetEditStatus(XX))
/*------------By zjh end -----------------------*/

#define TextBoxIsFormatFull(XX) (TextBoxGetBoxStatus(XX)&(1<<5))
#define TextBoxSetFormatFull(XX) (TextBoxSetBoxStatus(XX,TextBoxGetBoxStatus(XX)|(1<<5)))
#define TextBoxSetFormatNotFull(XX) (TextBoxSetBoxStatus(XX,TextBoxGetBoxStatus(XX)&(~(1<<5))))

#define TextBoxIsDisablePrint(TT) ((TT)->BoxStatus&2)
#define TextBoxDependPage(TT) ((TT)->BoxStatus&4)
#define TextBoxGetAlign(TT) (((TT)->BoxStatus&(0x3<<2))>>2)
#define TextBoxIsNoRound(TT) ((TT)->BoxStatus&16)
#define TextBoxIsDisableFrame(TT) ((TT)->BoxStatus&64)
#define TextBoxIsEmbodyBox(TT) ((TT)->BoxStatus&128)

#define TextBoxSetDisablePrint(TT) ((TT)->BoxStatus|=2)
#define TextBoxSetEnablePrint(TT) ((TT)->BoxStatus&=~2)
#define TextBoxSetDependPage(TT) ((TT)->BoxStatus|=4)
#define TextBoxClearDependPage(TT) ((TT)->BoxStatus&=~4)
#define TextBoxSetAlign(TT,AA) ( (TT)->BoxStatus&=0x3; (TT)->BoxStatus|=((AA&3)<<2) )
#define TextBoxSetNoPublish(TT) ((TT)->BoxStatus|=16)
#define TextBoxSetDisableFrame(TT) ((TT)->BoxStatus|=64)
#define TextBoxSetEmbodyBox(TT) ((TT)->BoxStatus|=128)

#define TextBoxSetRoundPublish(TT) ((TT)->BoxStatus&=~16)
#define TextBoxSetEnableFrame(TT) ((TT)->BoxStatus&=~64)
#define TextBoxSetIndependBox(TT) ((TT)->BoxStatus&=~128)

#define TextBoxSetBoxStatus(XX,YY) ((XX)->BoxStatus=YY)
#define TextBoxSetBoxType(XX,YY) ((XX)->BoxType=YY)
#define TextBoxSetBoxLeft(XX,YY) ((XX)->BoxLeft=YY)
#define TextBoxSetBoxTop(XX,YY) ((XX)->BoxTop=YY)
#define TextBoxSetBoxWidth(XX,YY) ((XX)->BoxWidth=YY)
#define TextBoxSetBoxHeight(XX,YY) ((XX)->BoxHeight=YY)
#define TextBoxSetRotateAngle(XX,YY) ((XX)->RotateAngle=YY)
#define TextBoxSetRotateAxisX(XX,YY) ((XX)->RotateAxisX=YY)
#define TextBoxSetRotateAxisY(XX,YY) ((XX)->RotateAxisY=YY)
#define TextBoxSetTextAlign(XX,YY) ((XX)->TextAlign=YY)
#define TextBoxSetBoxBackColor(XX,YY) ((XX)->BoxBackColor=YY)
#define TextBoxSetBoxColumn(XX,YY) ((XX)->BoxColumn=YY)
#define TextBoxSetColumnDistant(XX,YY) ((XX)->ColumnDistant=YY)
#define TextBoxSetTextDistantLeft(XX,YY) ((XX)->TextDistantLeft=YY)
#define TextBoxSetTextDistantTop(XX,YY) ((XX)->TextDistantTop=YY)
#define TextBoxSetTextDistantRight(XX,YY) ((XX)->TextDistantRight=YY)
#define TextBoxSetTextDistantBottom(XX,YY) ((XX)->TextDistantBottom=YY)
#define TextBoxSetRoundDistantLeft(XX,YY) ((XX)->RoundDistantLeft=YY)
#define TextBoxSetRoundDistantTop(XX,YY) ((XX)->RoundDistantTop=YY)
#define TextBoxSetRoundDistantRight(XX,YY) ((XX)->RoundDistantRight=YY)
#define TextBoxSetRoundDistantBottom(XX,YY) ((XX)->RoundDistantBottom=YY)
#define TextBoxSetTextLength(XX,YY) ((XX)->TextLength=YY)
#define TextBoxSetBlockLength(XX,YY) ((XX)->TextBlockLength=YY)
#define TextBoxSetTextHandle(XX,YY) ((XX)->TextHandle=YY)
#define TextBoxSetPrevLinkBox(XX,YY) ((XX)->PrevLinkBox=YY)
#define TextBoxSetNextLinkBox(XX,YY) ((XX)->NextLinkBox=YY)
#define TextBoxSetClipPolygons(XX,YY) ((XX)->ClipPolygons=YY)
#define TextBoxSetClipEdges(XX,YY) ((XX)->ClipEdges=YY)
#define TextBoxSetClipBoxXY(XX,YY) ((XX)->ClipBoxXY=YY)
#define TextBoxSetInvalidPolygons(XX,YY) ((XX)->InvalidPolygons=YY)
#define TextBoxSetInvalidEdges(XX,YY) ((XX)->InvalidEdges=YY)
#define TextBoxSetInvalidBoxXY(XX,YY) ((XX)->InvalidBoxXY=YY)

/* Follow is define of Table Boxs */

// used for CELL's iFirst field
#define FIRSTCELL -1
#define UNCERTAINCELL -2
#define GetTableHeadOption()      (tmp_TableHeadOption)
#define SetTableHeadOption(X)     (tmp_TableHeadOption=X)
#define GetTableLineColOption()   (tmp_TableLineColOption)
#define GetTableStyleOption()     (tmp_TableStyleOption)
#define SetTableStyleOption(X)    (tmp_TableStyleOption=X)
#define GetTableLineNumber()      (tmp_nline)
#define GetTableInsOption()       (tmp_insflag)

//#define TableBoxGetMinVertline(pFormBox)    (pFormBox->vertline[0])
//#define TableBoxGetMinHortline(pFormBox)    (pFormBox->hortline[0])
//#define TableBoxGetMinWidth(pFormBox)  (pFormBox->vertline[pFormBox->numCols]-pFormBox->vertline[0])
//#define TableBoxGetMinHeight(pFormBox) (pFormBox->hortline[pFormBox->numLines]-pFormBox->hortline[0])

#define TableBoxGetNext(XX) ItemGetNext(XX)
#define TableBoxGetPrev(XX) ItemGetPrev(XX)
#define TableBoxGetTextHead(XX) ItemGetChild(XX)

#define TableBoxGetBoxStatus(XX) ((XX)->BoxStatus)
#define TableBoxGetBoxType(XX) ((XX)->BoxType)
#define TableBoxGetBoxLeft(XX) ((XX)->BoxLeft)
#define TableBoxGetBoxTop(XX) ((XX)->BoxTop)
#define TableBoxGetBoxWidth(XX) ((XX)->BoxWidth)
#define TableBoxGetBoxHeight(XX) ((XX)->BoxHeight)
#define TableBoxGetBoxRight(XX) (TableBoxGetBoxLeft(XX)+(((TableBoxs *)(XX))->BoxWidth))
#define TableBoxGetBoxBottom(XX) (TableBoxGetBoxTop(XX)+(((TableBoxs *)(XX))->BoxHeight))
#define TableBoxGetRotateAngle(XX) ((XX)->RotateAngle)
#define TableBoxGetRotateAxisX(XX) ((XX)->RotateAxisX)
#define TableBoxGetRotateAxisY(XX) ((XX)->RotateAxisY)
#define TableBoxGetBoxBackColor(XX) ((XX)->BoxBackColor)
#define TableBoxGetBoxBorderType(XX) ((XX)->BoxBorderType)
#define TableBoxGetTextAlign(XX) ((XX)->TextAlign)
#define TableBoxGetTextDistantTop(XX) ((XX)->TextDistantTop)
#define TableBoxGetTextDistantRight(XX) ((XX)->TextDistantRight)
#define TableBoxGetTextDistantBottom(XX) ((XX)->TextDistantBottom)
#define TableBoxGetRoundDistantLeft(XX) ((XX)->RoundDistantLeft)
#define TableBoxGetRoundDistantTop(XX) ((XX)->RoundDistantTop)
#define TableBoxGetRoundDistantRight(XX) ((XX)->RoundDistantRight)
#define TableBoxGetRoundDistantBottom(XX) ((XX)->RoundDistantBottom)
#define TableBoxGetTextLength(XX) ((XX)->TextLength)
#define TableBoxGetBlockLength(XX) ((XX)->TextBlockLength)
#define TableBoxGetTextHandle(XX) ((XX)->TextHandle)
#define TableBoxGetPrevLinkBox(XX) ((XX)->PrevLinkBox)
#define TableBoxGetNextLinkBox(XX) ((XX)->NextLinkBox)
#define TableBoxGetClipPolygons(XX) ((XX)->ClipPolygons)
#define TableBoxGetClipEdges(XX) ((XX)->ClipEdges)
#define TableBoxGetClipBoxXY(XX) ((XX)->ClipBoxXY)
#define TableBoxGetnumLines(XX) ((XX)->numLines)
#define TableBoxGetnumCols(XX) ((XX)->numCols)
#define TableBoxGetvertline(XX,VV) ((XX)->vertline[VV])
#define TableBoxGethortline(XX,VV) ((XX)->hortline[VV])
#define TableBoxGethCellTable(XX) ((XX)->hCellTable)

#define TableBoxIsFormatFull(XX) (TextBoxGetBoxStatus(XX)&(1<<5))
#define TableBoxSetFormatFull(XX) (TextBoxSetBoxStatus(XX,TextBoxGetBoxStatus(XX)|(1<<5)))
#define TableBoxSetFormatNotFull(XX) (TextBoxSetBoxStatus(XX,TextBoxGetBoxStatus(XX)&(~(1<<5))))

#define TableBoxSetBoxStatus(XX,YY) ((XX)->BoxStatus=YY)
#define TableBoxSetBoxType(XX,YY) ((XX)->BoxType=YY)
#define TableBoxSetBoxLeft(XX,YY) ((XX)->BoxLeft=YY)
#define TableBoxSetBoxTop(XX,YY) ((XX)->BoxTop=YY)
#define TableBoxSetBoxWidth(XX,YY) ((XX)->BoxWidth=YY)
#define TableBoxSetBoxHeight(XX,YY) ((XX)->BoxHeight=YY)
#define TableBoxSetRotateAngle(XX,YY) ((XX)->RotateAngle=YY)
#define TableBoxSetRotateAxisX(XX,YY) ((XX)->RotateAxisX=YY)
#define TableBoxSetRotateAxisY(XX,YY) ((XX)->RotateAxisY=YY)
#define TableBoxSetTextAlign(XX,YY) ((XX)->TextAlign=YY)
#define TableBoxSetBoxBackColor(XX,YY) ((XX)->BoxBackColor=YY)
#define TableBoxSetBoxBorderType(XX,YY) ((XX)->BoxBorderType=YY)
#define TableBoxSetTextDistantLeft(XX,YY) ((XX)->TextDistantLeft=YY)
#define TableBoxSetTextDistantTop(XX,YY) ((XX)->TextDistantTop=YY)
#define TableBoxSetTextDistantRight(XX,YY) ((XX)->TextDistantRight=YY)
#define TableBoxSetTextDistantBottom(XX,YY) ((XX)->TextDistantBottom=YY)
#define TableBoxSetRoundDistantLeft(XX,YY) ((XX)->RoundDistantLeft=YY)
#define TableBoxSetRoundDistantTop(XX,YY) ((XX)->RoundDistantTop=YY)
#define TableBoxSetRoundDistantRight(XX,YY) ((XX)->RoundDistantRight=YY)
#define TableBoxSetRoundDistantBottom(XX,YY) ((XX)->RoundDistantBottom=YY)
#define TableBoxSetTextLength(XX,YY) ((XX)->TextLength=YY)
#define TableBoxSetBlockLength(XX,YY) ((XX)->TextBlockLength=YY)
#define TableBoxSetTextHandle(XX,YY) ((XX)->TextHandle=YY)
#define TableBoxSetPrevLinkBox(XX,YY) ((XX)->PrevLinkBox=YY)
#define TableBoxSetNextLinkBox(XX,YY) ((XX)->NextLinkBox=YY)
#define TableBoxSetClipPolygons(XX,YY) ((XX)->ClipPolygons=YY)
#define TableBoxSetClipEdges(XX,YY) ((XX)->ClipEdges=YY)
#define TableBoxSetClipBoxXY(XX,YY) ((XX)->ClipBoxXY=YY)
#define TableBoxSetnumLines(XX,YY) ((XX)->numLines=YY)
#define TableBoxSetnumCols(XX,YY) ((XX)->numCols=YY)
#define TableBoxSetvertline(XX,VV,LL) ((XX)->vertline[VV]=LL)
#define TableBoxSethortline(XX,VV,LL) ((XX)->hortline[VV]=LL)
#define TableBoxSethCellTable(XX,YY) ((XX)->hCellTable=YY)

/* Follow is define of Line Boxs */

#define LineBoxGetNext(XX) ItemGetNext(XX)
#define LineBoxGetPrev(XX) ItemGetPrev(XX)
#define LineBoxGetTextHead(XX) ItemGetChild(XX)

#define LineBoxGetBoxStatus(XX) ((XX)->BoxStatus)
#define LineBoxGetBoxType(XX) ((XX)->BoxType)
#define LineBoxGetBoxLeft(XX) ((XX)->BoxLeft)
#define LineBoxGetBoxTop(XX) ((XX)->BoxTop)
#define LineBoxGetBoxWidth(XX) ((XX)->BoxWidth)
#define LineBoxGetBoxHeight(XX) ((XX)->BoxHeight)
#define LineBoxGetBoxRight(XX) (LineBoxGetBoxLeft(XX)+(((LineBoxs *)(XX))->BoxWidth))
#define LineBoxGetBoxBottom(XX) (LineBoxGetBoxTop(XX)+(((LineBoxs *)(XX))->BoxHeight))
#define LineBoxGetRotateAngle(XX) ((XX)->RotateAngle)
#define LineBoxGetRotateAxisX(XX) ((XX)->RotateAxisX)
#define LineBoxGetRotateAxisY(XX) ((XX)->RotateAxisY)
#define LineBoxGetBoxBackColor(XX) ((XX)->BoxBackColor)
#define LineBoxGetBoxBorderType(XX) ((XX)->BoxBorderType)
#define LineBoxGetBoxArrowHeadType(XX) ((XX)->BoxArrowHeadType)
#define LineBoxGetBoxArrowTailType(XX) ((XX)->BoxArrowTailType)
#define LineBoxGetRoundDistantLeft(XX) ((XX)->RoundDistantLeft)
#define LineBoxGetRoundDistantTop(XX) ((XX)->RoundDistantTop)
#define LineBoxGetRoundDistantRight(XX) ((XX)->RoundDistantRight)
#define LineBoxGetRoundDistantBottom(XX) ((XX)->RoundDistantBottom)
#define LineBoxGetClipPolygons(XX) ((XX)->ClipPolygons)
#define LineBoxGetClipEdges(XX) ((XX)->ClipEdges)
#define LineBoxGetClipBoxXY(XX) ((XX)->ClipBoxXY)
#define LineBoxGetBoxArrowType(XX) (((XX)->BoxArrowHeadType)<<4)|((XX)->BoxArrowTailType)
#define LineBoxSetBoxStatus(XX,YY) ((XX)->BoxStatus=YY)
#define LineBoxSetBoxType(XX,YY) ((XX)->BoxType=YY)
#define LineBoxSetBoxLeft(XX,YY) ((XX)->BoxLeft=YY)
#define LineBoxSetBoxTop(XX,YY) ((XX)->BoxTop=YY)
#define LineBoxSetBoxWidth(XX,YY) ((XX)->BoxWidth=YY)
#define LineBoxSetBoxHeight(XX,YY) ((XX)->BoxHeight=YY)
#define LineBoxSetRotateAngle(XX,YY) ((XX)->RotateAngle=YY)
#define LineBoxSetRotateAxisX(XX,YY) ((XX)->RotateAxisX=YY)
#define LineBoxSetRotateAxisY(XX,YY) ((XX)->RotateAxisY=YY)
#define LineBoxSetTextAlign(XX,YY) ((XX)->TextAlign=YY)
#define LineBoxSetBoxBackColor(XX,YY) ((XX)->BoxBackColor=YY)
#define LineBoxSetBoxBorderType(XX,YY) ((XX)->BoxBorderType=YY)
#define LineBoxSetBoxArrowHeadType(XX,YY) ((XX)->BoxArrowHeadType=YY)
#define LineBoxSetBoxArrowTailType(XX,YY) ((XX)->BoxArrowTailType=YY)
#define LineBoxSetRoundDistantLeft(XX,YY) ((XX)->RoundDistantLeft=YY)
#define LineBoxSetRoundDistantTop(XX,YY) ((XX)->RoundDistantTop=YY)
#define LineBoxSetRoundDistantRight(XX,YY) ((XX)->RoundDistantRight=YY)
#define LineBoxSetRoundDistantBottom(XX,YY) ((XX)->RoundDistantBottom=YY)
#define LineBoxSetClipPolygons(XX,YY) ((XX)->ClipPolygons=YY)
#define LineBoxSetClipEdges(XX,YY) ((XX)->ClipEdges=YY)
#define LineBoxSetClipBoxXY(XX,YY) ((XX)->ClipBoxXY=YY)

/* Follow is define of Picture Boxs */
#define PictureBoxGetNext(XX) ItemGetNext(XX)
#define PictureBoxGetPrev(XX) ItemGetPrev(XX)
#define PictureBoxGetTextHead(XX) ItemGetChild(XX)

#define PictureBoxGetBoxStatus(XX) (((PictureBoxs *)(XX))->BoxStatus)
#define PictureBoxGetBoxType(XX) (((PictureBoxs *)(XX))->BoxType)
#define PictureBoxGetBoxLeft(XX) (((PictureBoxs *)(XX))->BoxLeft)
#define PictureBoxGetBoxTop(XX) (((PictureBoxs *)(XX))->BoxTop)
#define PictureBoxGetBoxWidth(XX) (((PictureBoxs *)(XX))->BoxWidth)
#define PictureBoxGetBoxHeight(XX) (((PictureBoxs *)(XX))->BoxHeight)
#define PictureBoxGetBoxRight(XX) (PictureBoxGetBoxLeft(XX)+(((PictureBoxs *)(XX))->BoxWidth))
#define PictureBoxGetBoxBottom(XX) (PictureBoxGetBoxTop(XX)+(((PictureBoxs *)(XX))->BoxHeight))
#define PictureBoxGetRotateAngle(XX) (((PictureBoxs *)(XX))->RotateAngle)
#define PictureBoxGetRotateAxisX(XX) (((PictureBoxs *)(XX))->RotateAxisX)
#define PictureBoxGetRotateAxisY(XX) (((PictureBoxs *)(XX))->RotateAxisY)
#define PictureBoxGetBoxBackColor(XX) (((PictureBoxs *)(XX))->BoxBackColor)
#define PictureBoxGetRoundDistant(XX) (((PictureBoxs *)(XX))->RoundDistant)
#define PictureBoxGetPictureFileName(XX) (((PictureBoxs *)(XX))->PictureFileName)
#define PictureBoxGetBorderPolygon(XX) (((PictureBoxs *)(XX))->BorderPolygon)
#define PictureBoxGetCornerRadius(XX) (((PictureBoxs *)(XX))->BorderPolygon)
#define PictureBoxGetPicturePresent(XX) (((PictureBoxs *)(XX))->PicturePresent)
#define PictureBoxGetClipPolygons(XX) (((PictureBoxs *)(XX))->ClipPolygons)
#define PictureBoxGetClipEdges(XX) (((PictureBoxs *)(XX))->ClipEdges)
#define PictureBoxGetClipBoxXY(XX) (((PictureBoxs *)(XX))->ClipBoxXY)

#define PictureBoxSetBoxStatus(XX,YY) (((PictureBoxs *)(XX))->BoxStatus=YY)
#define PictureBoxSetBoxType(XX,YY) (((PictureBoxs *)(XX))->BoxType=YY)
#define PictureBoxSetBoxLeft(XX,YY) (((PictureBoxs *)(XX))->BoxLeft=YY)
#define PictureBoxSetBoxTop(XX,YY) (((PictureBoxs *)(XX))->BoxTop=YY)
#define PictureBoxSetBoxWidth(XX,YY) (((PictureBoxs *)(XX))->BoxWidth=YY)
#define PictureBoxSetBoxHeight(XX,YY) (((PictureBoxs *)(XX))->BoxHeight=YY)
#define PictureBoxSetRotateAngle(XX,YY) (((PictureBoxs *)(XX))->RotateAngle=YY)
#define PictureBoxSetRotateAxisX(XX,YY) (((PictureBoxs *)(XX))->RotateAxisX=YY)
#define PictureBoxSetRotateAxisY(XX,YY) (((PictureBoxs *)(XX))->RotateAxisY=YY)
#define PictureBoxSetBoxBackColor(XX,YY) (((PictureBoxs *)(XX))->BoxBackColor=YY)
#define PictureBoxSetRoundDistant(XX,YY) (((PictureBoxs *)(XX))->RoundDistant=YY)
#define PictureBoxSetPictureFileName(XX,YY) (strcpy(((PictureBoxs *)(XX))->PictureFileName,YY))
#define PictureBoxSetBorderPolygon(XX,YY) (((PictureBoxs *)(XX))->BorderPolygon=YY)
#define PictureBoxSetCornerRadius(XX,YY) (((PictureBoxs *)(XX))->BorderPolygon=YY)
#define PictureBoxSetPicturePresent(XX,YY) (((PictureBoxs *)(XX))->PicturePresent=YY)
#define PictureBoxSetClipPolygons(XX,YY) (((PictureBoxs *)(XX))->ClipPolygons=YY)
#define PictureBoxSetClipEdges(XX,YY) (((PictureBoxs *)(XX))->ClipEdges=YY)
#define PictureBoxSetClipBoxXY(XX,YY) (((PictureBoxs *)(XX))->ClipBoxXY=YY)

#define BoxGetNext(XX) ItemGetNext(XX)
#define BoxGetPrev(XX) ItemGetPrev(XX)
#define BoxGetPage(XX) ItemGetFather(XX)

#define DRAWVIRTUALBORDOR 1
#define DRAWBORDERWITHRECATNGLE 2
#define DRAWXORBORDER 4
#define DRAWWITHFILL 8

#define IsDrawVirtualBorder(AA) (AA&DRAWVIRTUALBORDOR)
#define IsDrawBorderWithRectangle(AA) (AA&DRAWBORDERWITHRECATNGLE)
#define IsDrawXORBorder(AA) (AA&DRAWXORBORDER)
#define IsDrawWithFill(AA) (AA&DRAWWITHFILL)

#define BOXDEFAULTLEFT 1750
#define BOXDEFAULTTOP 3500
#define BOXDEFAULTRIGHT BOXDEFAULTLEFT
#define BOXDEFAULTBOTTOM 2000

////// PrintH ///////
#define DitherTableLen   6008

////// Lines ///////
  /* ----- for fill mode ----- */
  #define ORMODE 0
  #define ANDMODE 1
  #define XORMODE 2
  /* ----- for line arrow ----- */
  #define NOARROW 0
  #define ARROWVER 1
  #define ARROWHOR 2
  #define ARROWARC 3
  #define ARROWTRI1 4
  #define ARROWTRI2 5
  #define HALFARROWARC 7
  #define MAXARROWTYPE ARROWTRI2
  /* ----- for line style ----- */
  #define NoLineType 0
  #define LineType1  1
  #define LineType2  2
  #define LineType3  3
  #define LineType4  4
  #define MaxLineType LineType4
  #define MaxLineTypeLength 16
  /* ----- for arc style ----- */
  #define NoArcType 0
  #define ArcType1  1
  #define ArcType2  2
  #define ArcType3  3
  #define ArcType4  4
  #define MaxArcType ArcType4
  #define MaxArcTypeAngle (PI/10.)
  #define MaxArcTypeLength 16

/*-------------------- EDITOR -------------------------*/
///////// BoxEdit ///////////
#define MAXBUFFERLENGTH (INT_MAX)

#define ATTRIBUTEBITS 11
#define ATTRIBUTEPATTERN 0x07ff
#define ATTRIBUTEPRECODE 0xf800

#define MakeCHARSIZE(SS) ((SS)|(CHARSIZE<<ATTRIBUTEBITS))
#define MakeSUPERSCRIPT(SS) ((SS)|(SUPERSCRIPT<<ATTRIBUTEBITS)) /* ByDg, 96,3.12 */
#define MakeSUBSCRIPT(SS) ((SS)|(SUBSCRIPT<<ATTRIBUTEBITS))
#define MakeCHARFONT(FF) ((FF)|(CHARFONT<<ATTRIBUTEBITS))
#define MakeCHARSLANT(SS) ((SS)|(CHARSLANT<<ATTRIBUTEBITS))
#define MakeCHARHSIZE(SS) ((SS)|(CHARHSIZE<<ATTRIBUTEBITS))
#define MakeCHARCOLOR(CC) ((CC)|(CHARCOLOR<<ATTRIBUTEBITS))
#define MakePARAGRAPHALIGN(AA) ((AA)|(PARAGRAPHALIGN<<ATTRIBUTEBITS))
#define MakeINSERTBOX(BB) ((BB)|(INSERTBOX<<ATTRIBUTEBITS))

//#define MakeATTRIBUTE(II,AA) ((AA)|((II)<<ATTRIBUTEBITS))

#define MakeATTRIBUTE(II,AA) make_attr(II,AA)      //By zjh  10.29

//#define GetAttribute(GG) ((GG)&ATTRIBUTEPATTERN)
#define GetAttribute(GG) get_attr(GG)              //By zjh  10.29
#define GetCharCode(GG) (GG)
#define GetPreCode(GG) (((GG)&ATTRIBUTEPRECODE)>>ATTRIBUTEBITS)

#define EditBufferChangeCharSize(SS,PP,AA,SSLL,RRBB) EditBufferChangeAttribute(SS,PP,CHARSIZE,AA,SSLL,RRBB)
#define EditBufferChangeCharFont(SS,PP,AA,SSLL,RRBB) EditBufferChangeAttribute(SS,PP,CHARFONT,AA,SSLL,RRBB)
#define EditBufferChangeCharSlant(SS,PP,AA,SSLL,RRBB) EditBufferChangeAttribute(SS,PP,CHARSLANT,AA,SSLL,RRBB)
#define EditBufferChangeCharHSize(SS,PP,AA,SSLL,RRBB) EditBufferChangeAttribute(SS,PP,CHARHSIZE,AA,SSLL,RRBB)
#define EditBufferChangeCharColor(SS,PP,AA,SSLL,RRBB) EditBufferChangeAttribute(SS,PP,CHARCOLOR,AA,SSLL,RRBB)
#define EditBufferChangeParagraphAlign(SS,PP,AA,SSLL,RRBB) EditBufferChangeAttribute(SS,PP,PARAGRAPHALIGN,AA,SSLL,RRBB)

////////// KeyEdit ///////
#define NORMALDISPLAY 0
#define NEGATIVEDISPLAY 1

#define CURSORJUMPWINDOW 150

///////// Find /////////
#define F_CaseSensitive 4
#define F_WordOnly  64

#define FindFromHead(FFS) ((FFS).FindOption&1)
#define FindSelectRange(FFS) ((FFS).FindOption&2)
#define FindCaseSensitive(FFS) ((FFS).FindOption&4)
#define FindRepaceALL(FFS) ((FFS).FindOption&8)
#define FindPrompt(FFS) (!((FFS).FindOption&16))
#define FindBackward(FFS) ((FFS).FindOption&32)
#define FindWordOnly(FFS) ((FFS).FindOption&64)
#define FindDoReplace(FFS) ((FFS).FindOption&128)

#define FindSetFromHeadSign(FFS) ((FFS).FindOption|=1)
#define FindSetSelectRangeSign(FFS) ((FFS).FindOption|=2)
#define FindSetCaseSensitiveSign(FFS) ((FFS).FindOption|=4)
#define FindSetReplaceAllSign(FFS) ((FFS).FindOption|=8)
#define FindSetPromptSign(FFS) ((FFS).FindOption|=16)
#define FindSetBackwardSign(FFS) ((FFS).FindOption|=32)
#define FindSetWordOnlySign(FFS) ((FFS).FindOption|=64)
#define FindSetReplaceSign(FFS) ((FFS).FindOption|=128)

#define FindClearFromHeadSign(FFS) ((FFS).FindOption&=~1)
#define FindClearSelectRangeSign(FFS) ((FFS).FindOption&=~2)
#define FindClearCaseSensitiveSign(FFS) ((FFS).FindOption&=~4)
#define FindClearReplaceAllSign(FFS) ((FFS).FindOption&=~8)
#define FindClearPromptSign(FFS) ((FFS).FindOption&=~16)
#define FindClearBackwardSign(FFS) ((FFS).FindOption&=~32)
#define FindClearWordOnlySign(FFS) ((FFS).FindOption&=~64)
#define FindClearReplaceSign(FFS) ((FFS).FindOption&=~128)

//////////// Text ////////////
#define ENGLISHCHAR 0
#define CHINESECHARS 21

#define HIGHENGLISHCHAR 0
#define HIGHCHINESECHARS (0xa000)

/*---- follow is attribute tag ---------*/
#define CHARSIZE 1
#define CHARFONT 2
#define CHARSLANT 3
#define CHARHSIZE 4
#define CHARCOLOR 5
#define PARAGRAPHALIGN 6
#define INSERTBOX 7
#define ROWGAP    8             /* ByHance, 96,01.18 */
#define SUPERSCRIPT     9       /* By Dg, 96,3.12 */
#define SUBSCRIPT       10
#define COLGAP          11      //By zjh
#define UPDOWN          12      //By zjh
#define VPARAGRAPHALIGN 13      //By zjh
#define SUBLINE         14      //By zjh

/*--- paragraph alignment ---*/
enum {
        ALIGNLEFTRIGHT=0,
        ALIGNLEFT     =1,
        ALIGNRIGHT    =2,
        ALIGNCENTRE   =3,
};

enum {                                  //By zjh 9.12
        ALIGNUPDOWN   =0,
        ALIGNUP       =1,
        ALIGNDOWN     =2,
        ALIGNVCENTRE  =3,
};

/*----- row gap -----*/
#define LINEGAP1       0x10
#define LINEGAP15      0x00             //zjh 00
#define LINEGAP2       0x20
#define LINEGAPUSER    0x30
#define LINEGAP175     0x40             //zjh
#define LINEGAP125     0x50             //zjh

/*---- default setting ----*/
#define DEFAULTCHARSIZE  146
#define DEFAULTCHARHSIZE 146
#define DEFAULTCHARSLANT 0             /* 0 degree */
#define DEFAULTCHARFONT 0              /* Font Song */
#define DEFAULTCHARCOLOR 0             /* Black */

#define DEFAULTROWGAP   (LINEGAP15)     /* ByHance,96,1.18 */
#define DEFAULTCOLGAP   (LINEGAP15)     /* By zjh */
#define DEFAULTSUPERSCRIPT 0            /* By Dg */
#define DEFAULTSUBSCRIPT   0            /* */
#define DEFAULTUPDOWN      0            /* By zjh */
#define DEFAULTSUBLINE     0            /* By zjh */

//#define DEFAULTPARAGRAPHALIGN 0        /* Align to left , chaned ByHance*/
#define DEFAULTPARAGRAPHALIGN ALIGNLEFTRIGHT /* Align to left_right, 95,12.21 */
#define DEFAULTVPARAGRAPHALIGN ALIGNUPDOWN   /* Align to up_down, 96,9.12 */

//#define FONTSIZEFACT  10
#define FONTSIZEFACT  1

#define MY_ISPUNCT(val) (!(val&0xff00)&&ispunct((int)(val)))
#define Cisend(val)     ((val==0x0000))
#define Cisformtab(val) ((val=='\t'))
#define Cisparaend(val) ((val==0x000a)||(val==0x000d)||(val==0x0000)||(val==0xc)) //By zjh
#define Cispageend(val) ((val==0xc))             //By zjh
#define Cisspace(val)   (!(val&0xff00)&&isspace((int)(val))&&!Cisparaend(val))
#define Cisctext(val)   ((val>0xa000))
#define Cisstyle(val)   ( (!Cisctext(val)) && (val&0x7800) )
#define Cisascii(val)   ((val>=0x20&&val<=0x7f))      //By zjh

/*-------------------- IMAGE -------------------------*/
///// Image ////////
#define NORMALCONTRAST 0
#define HIGHCONTRAST 1
#define POSTERIZED 2

#define IMAGEBLACKWHITE 0
#define IMAGE16COLOR 1
#define IMAGE256COLOR 2
#define IMAGE16GRAY 3
#define IMAGE256GRAY 4
#define IMAGETRUECOLOR 5

#define MAXFRACT 100
#define MAXPOSTERIZEDCOLOR 10
#define HIGHCONTRASTCONST 0.3

//#define MAXSEGMENTHANDLE 10

#define ImageGetAttribute(XX) ((XX)->ImageAttribute)
#define ImageGetContrast(XX) (ImageGetAttribute((XX))&3)
#define ImageGetNegative(XX) (ImageGetAttribute((XX))&4)
#define ImageGetBitColor(XX) ((ImageGetAttribute((XX))>>3)&7)

#define ImageSetAttribute(XX,AA) ((XX)->ImageAttribute=(AA))
#define ImageSetContrast(XX,YY) ImageSetAttribute((XX),(ImageGetAttribute((XX))&0xfc)|(YY))
#define ImageSetNegative(XX,YY) ImageSetAttribute((XX),(ImageGetAttribute((XX))&0xfb)|(YY<<2))
#define ImageSetBitColor(XX,YY) ImageSetAttribute((XX),(ImageGetAttribute((XX))&0xc7)|(YY<<3))

#define ImageLineBufferGetLeft(BB) (((ImageLineBuffers *)(BB))->ImageLineLeft)
#define ImageLineBufferGetLength(BB) (((ImageLineBuffers *)(BB))->ImageLineLength)
#define ImageLineBufferSetLeft(BB,XX) (((ImageLineBuffers *)(BB))->ImageLineLeft=XX)
#define ImageLineBufferSetLength(BB,XX) (((ImageLineBuffers *)(BB))->ImageLineLength=XX)

////////// TiffFill ////////
  /* ----- tiff parameter ----- */
  #define TIFImageWidth                256
  #define TIFImageHeight               257
  #define TIFBitsPerSample             258
  #define TIFCompression               259
  #define TIFPhotometricInterpretation 262
  #define TIFStripOffset               273
  #define TIFSamplesPerPiexl           277
  #define TIFRowsPerStrip              278
  #define TIFStripByteCounts           279
  #define TIFXResolution               282
  #define TIFYResolution               283
  #define TIFPlanarConfiguration       284
  #define TIFResolutionUnit            296
  #define TIFColorResponseCurve        301
  #define TIFColorMap                  320

  #define PUTDITHERDEVICE 0
  #define PUTTRUECOLORDEVICE 1

/*--------------- FORMAT ------------*/
/////// Format //////
enum {E_ERROR,E_TEXT,E_LINEFEED,E_HARDLINEFEED,E_INSERTBOX,E_FORMTAB,E_PAGEFEED,E_OMIT,E_END};

//////// Jfont //////
/*------ define slant direction -----*/
enum { NO_SLANT=0,SLANT_LEFT,SLANT_RIGHT };
/*----- define dim direction ------*/
#define UPPERLEFT_BLOCK         1
#define UPPERRIGHT_BLOCK        2
#define LOWERLEFT_BLOCK         3
#define LOWERRIGHT_BLOCK        4
/*--- define outline type ---*/
#define WHITE_IN_BLACK  1
#define BLACK_IN_WHITE  2
/* code type definition */
#define CC_CHAR 1
#define EE_CHAR 2
#define LACE_CHAR 3
#define HG_SYMBOL 4
/* red code to GB code */
#define LACELIB         129
#define LACEFONT        24
#define FIRSTLACE       0x8580
#define LINELACE        100
#define NORMALLACE      107
#define NEWLACE         108
#define SECONDLACE      0x8c80
#define VEC_LIB         666
#define DOT_LIB         128
#define SYMBOLFONT      30
#define ASCIITYPE       0xa300
#define MATHTYPE        0xac00
#define GBCHAR          0xb0a1
#define QU1             0xa100
#define QU2             0xa200
#define QU4             0xa400
#define QU5             0xa500
#define QU6             0xa600
#define QU7             0xa700
#define QU8             0xa800
#define QU9             0xa900
#define QUE             0xae00
#define QUF             0xaf00
/*--------------device dependent constant----------*/
#define HOLLOWBORDER    1               //modi by zjh  ora=4
#define DIM3BORDER      1
#define DIM3STEP        1
#define SHADOW_ASPECT   1

#define RITALICBIT      1
#define LITALICBIT      2
#define ROTATEBIT       4
#define DIM3BIT         8
#define HOLLOWBIT       16
#define ITALICBITS      3
#define CHAR_SHADOW_W(v)     (v&0x1f)
#define CHAR_SHADOW_D(v)     ((USHORT)(v>>8)&0xff)

////////// Region ////////
enum {RT_RECT,RT_TIXIN};

///////// Jttf ////////
/*---------glyph flags----------*/
#define ONCURVE   1
#define XSHORT    2
#define YSHORT    4
#define REPEAT    8
#define XSAME     16
#define YSAME     32
/* define Table Directory constants */
#define Entries 17
#define MAX_DIRECTORY           20
#define CMAP                    0
#define GLYF                    1
#define HEAD                    2
#define HHEA                    3
#define HMTX                    4
#define LOCA                    5
#define MAXP                    6
#define NAME                    7
#define POST                    8
#define OS_2                    9
#define CVT                     10
#define FPGM                    11
#define HDMX                    12
#define KERN                    13
#define PREP                    14
#define WIN                     15
#define FOCA                    16
/*-------------- offset definations------------*/
/*---------Table Direction-----*/
#define SFNTVERTION     0
#define NUMTABS         4
#define SEARCHRANGE     6
#define ENTRYSELECTOR   8
#define RANGESHIFT      10
#define TABLEDIR        12
/*------Head-------*/
#define UNITPEREM       18
#define TTXMIN          36
#define TTYMIN          38
#define TTXMAX          40
#define TTYMAX          42
#define INDEXLOC        50
/*-----Hhea----------*/
#define ASCENDER        4
/*-----Maxp----------*/
#define NUMGLYPH        4
/*-----Glyf----*/
#define NCONTOUR            0
#define GGXMIN              2
#define GGYMIN              4
#define GGXMAX              6
#define GGYMAX              8
#define ENDPTS              10

#define MICROSOFT       3
#define PWIN            3
#define CWIN            4
#define SCHINESE        0x804
#define TCHINESE        0x404

/////// PolyFill //////
#define MAXFRAC 0x4000                 /* for fixed digit computing */
#define UPFLAG  -1
#define DOWNFLAG 1

///////filetitletype////////////
#define OPENFILE_TITLE  0
#define SAVEFILE_TITLE  1
#define SAVEAS_TITLE    2
#define EXPTEXT_TITLE   3
#define IMPTEXT_TITLE   4
#define IMPIMAGE_TITLE  5
#define PRNFILE_TITLE   6
#define IMPDATA_TITLE   7
#define OPENSET_TITLE   8
#define SAVESET_TITLE   9
#define SENDFILE_TITLE  10

#define MAXFIELDLIST 15
#define MAXFIELDBUFFERLEN 10000
#define OPEN_ERROR -1
#define FILE_FORMAT_ERROR -2

// added by Jerry 97/5/10
#define MAXVECFONT      55
#define MAXTTFONTJ      125
#define MAXTTFONTF      256
#define TOTALCFONTS     (55+81)

////////// UserMenu ////////////
#define USERMESSAGE 10000

#define MENU_NEW                   (USERMESSAGE+1)
#define MENU_OPEN                  (USERMESSAGE+2)
#define MENU_NEW1                  (USERMESSAGE+3)
#define MENU_SAVE                  (USERMESSAGE+4)
#define MENU_SAVEAS                (USERMESSAGE+5)
#define MENU_IMPORTTEXT            (USERMESSAGE+6)
  // #define MENU_IMPORTPICTURE         (USERMESSAGE+6)
#define MENU_EXPORTTEXT            (USERMESSAGE+7)
#define MENU_EXPORTFAX             (USERMESSAGE+8)
#define MENU_PAGEEXPORTPICT        (USERMESSAGE+9)
#define MENU_PRINT                 (USERMESSAGE+10)
  // #define MENU_LOADLIB               (USERMESSAGE+81)
  // #define MENU_SAVELIB               (USERMESSAGE+82)
  // #define MENU_DELETELIB             (USERMESSAGE+83)
  // #define MENU_FONTUSE               (USERMESSAGE+84)
  // #define MENU_PICTUREUSE            (USERMESSAGE+85)

#define MENU_PAGESETUP             (USERMESSAGE+11)
#define MENU_PRINTCUT              (USERMESSAGE+12)
#define MENU_OPENPRINTCUT          (USERMESSAGE+13)
#define MENU_SAVEPRINTCUT          (USERMESSAGE+14)
#define MENU_INSTALLFONT           (USERMESSAGE+15)
#define MENU_SETFONTPATH           (USERMESSAGE+16)
#define MENU_MODEMSETUP            (USERMESSAGE+17)
#define MENU_PRINTERSETUP          (USERMESSAGE+18)
#define MENU_EXIT                  (USERMESSAGE+19)

#define MENU_UNDO                  (USERMESSAGE+20)
#define MENU_COPY                  (USERMESSAGE+21)
#define MENU_PASTE                 (USERMESSAGE+22)
#define MENU_CUT                   (USERMESSAGE+23)
#define MENU_CLEAR                 (USERMESSAGE+24)
#define MENU_FIND                  (USERMESSAGE+25)
#define MENU_REPLACE               (USERMESSAGE+26)
#define MENU_NEXT                  (USERMESSAGE+27)
#define MENU_VIEWPORTFITWINDOW     (USERMESSAGE+28)
#define MENU_VIEWPORTHALF          (USERMESSAGE+29)
#define MENU_VIEWPORTQUART         (USERMESSAGE+30)
#define MENU_VIEWPORTACTUAL        (USERMESSAGE+31)
#define MENU_VIEWPORTDOUBLE        (USERMESSAGE+32)
  // #define MENU_VIEWPORTMINIZE        (USERMESSAGE+62)
  // #define MENU_SELECTALL             (USERMESSAGE+18)
#define MENU_BOXREDRAW             (USERMESSAGE+33)
#define MENU_GOTOLINE              (USERMESSAGE+34)
#define MENU_CALCULATELINE         (USERMESSAGE+35)
#define MENU_VIEWTOOLS             (USERMESSAGE+36)
#define MENU_CLIBRATION            (USERMESSAGE+37)

#define MENU_FONT                  (USERMESSAGE+38)
#define MENU_FONTSIZE0             (USERMESSAGE+39)
#define MENU_FONTSIZE1             (USERMESSAGE+40)
#define MENU_FONTSIZE2             (USERMESSAGE+41)
#define MENU_FONTSIZE3             (USERMESSAGE+42)
#define MENU_FONTSIZE4             (USERMESSAGE+43)
#define MENU_FONTSIZE4X            (USERMESSAGE+44)
#define MENU_FONTSIZE5             (USERMESSAGE+45)
#define MENU_FONTSIZE5X            (USERMESSAGE+46)
#define MENU_FONTSIZE6             (USERMESSAGE+47)
#define MENU_FONTSIZE7             (USERMESSAGE+48)
#define MENU_FONTSIZE8             (USERMESSAGE+49)
#define MENU_FONTSIZEFREE          (USERMESSAGE+50)

#define MENU_CHARBLACK             (USERMESSAGE+51)
#define MENU_CHARWHITE             (USERMESSAGE+52)
#define MENU_CHARRED               (USERMESSAGE+53)
#define MENU_CHARGREEN             (USERMESSAGE+54)
#define MENU_CHARBLUE              (USERMESSAGE+55)
#define MENU_CHARCYAN              (USERMESSAGE+56)
#define MENU_CHARMAGENTA           (USERMESSAGE+57)
#define MENU_CHARYELLOW            (USERMESSAGE+58)
#define MENU_CHARDKRED             (USERMESSAGE+59)
#define MENU_CHARDKGREEN           (USERMESSAGE+60)
#define MENU_CHARDKBLUE            (USERMESSAGE+61)
#define MENU_CHARDKCYAN            (USERMESSAGE+62)
#define MENU_CHARDKMAGENTA         (USERMESSAGE+63)
#define MENU_CHARDKYELLOW          (USERMESSAGE+64)
#define MENU_CHARLTGRAY            (USERMESSAGE+65)
#define MENU_CHARDKGRAY            (USERMESSAGE+66)
  //  #define MENU_CHARUSERCOLOR         (USERMESSAGE+247)

#define MENU_CHARNORMAL            (USERMESSAGE+67)
#define MENU_CHARITALIC            (USERMESSAGE+68)
#define MENU_SUPERSCRIPT           (USERMESSAGE+69)
#define MENU_SUBSCRIPT             (USERMESSAGE+70)
#define MENU_UPDOWN                (USERMESSAGE+71)
  //  #define MENU_CHAR3D                (USERMESSAGE+72)
  //  #define MENU_CHAROUTLINE           (USERMESSAGE+73)
#define MENU_SUBLINE0              (USERMESSAGE+72)
#define MENU_SUBLINE1              (USERMESSAGE+73)
#define MENU_SUBLINE2              (USERMESSAGE+74)
#define MENU_SUBLINE3              (USERMESSAGE+75)
#define MENU_SUBLINE4              (USERMESSAGE+76)

#define MENU_ALIGNMENTLEFT         (USERMESSAGE+77)
#define MENU_ALIGNMENTRIGHT        (USERMESSAGE+78)
#define MENU_ALIGNMENTMIDDLE       (USERMESSAGE+79)
#define MENU_ALIGNMENTAVERAGE      (USERMESSAGE+80)
  // char gap
#define MENU_COLGAP1               (USERMESSAGE+81)
#define MENU_COLGAP125             (USERMESSAGE+82)
#define MENU_COLGAP15              (USERMESSAGE+83)
#define MENU_COLGAP175             (USERMESSAGE+84)
#define MENU_COLGAP2               (USERMESSAGE+85)
#define MENU_COLGAPUSER            (USERMESSAGE+86)
  // Line space
#define MENU_ROWGAP1               (USERMESSAGE+87)
#define MENU_ROWGAP125             (USERMESSAGE+88)
#define MENU_ROWGAP15              (USERMESSAGE+89)
#define MENU_ROWGAP175             (USERMESSAGE+90)
#define MENU_ROWGAP2               (USERMESSAGE+91)
#define MENU_ROWGAPUSER            (USERMESSAGE+92)

#define MENU_BOX                   (USERMESSAGE+93)
/*---------   ///Frame Type
   #define MENU_FRAME0               (USERMESSAGE+262)
   #define MENU_FRAME1               (USERMESSAGE+263)
   #define MENU_FRAME2               (USERMESSAGE+264)
--------------*/
#define MENU_BOXLOCK               (USERMESSAGE+94)
#define MENU_BOXUNLOCK             (USERMESSAGE+95)
#define MENU_FORWARD               (USERMESSAGE+96)
#define MENU_BACKWARD              (USERMESSAGE+97)
#define MENU_FRONT                 (USERMESSAGE+98)
#define MENU_BACK                  (USERMESSAGE+99)
   // #define MENU_BOXALIGN              (USERMESSAGE+204)
#define MENU_APPENDPAGE            (USERMESSAGE+100)
#define MENU_INSERTPAGE            (USERMESSAGE+101)
#define MENU_DELETEPAGE            (USERMESSAGE+102)
#define MENU_MOVEPAGE              (USERMESSAGE+103)
#define MENU_PAGEFEED              (USERMESSAGE+104)
#define MENU_PAGEHEAD              (USERMESSAGE+105)
#define MENU_PAGENUM               (USERMESSAGE+106)
#define MENU_PREVPAGE              (USERMESSAGE+107)
#define MENU_NEXTPAGE              (USERMESSAGE+108)
#define MENU_FIRSTPAGE             (USERMESSAGE+109)
#define MENU_LASTPAGE              (USERMESSAGE+110)
#define MENU_GOTOPAGE              (USERMESSAGE+111)

#define MENU_ALIGNUPDOWN           (USERMESSAGE+112)
#define MENU_ALIGNVCENTRE          (USERMESSAGE+113)
#define MENU_TABLECALCULATE        (USERMESSAGE+114)
#define MENU_TABLEINSERTDBF        (USERMESSAGE+115)
#define MENU_TABLEROWCALC          (USERMESSAGE+116)
#define MENU_TABLECOLCALC          (USERMESSAGE+117)
#define MENU_TABLEINSERTLINE       (USERMESSAGE+118)
#define MENU_TABLEINSERTCOLUMN     (USERMESSAGE+119)
#define MENU_TABLEDELETELINE       (USERMESSAGE+120)
#define MENU_TABLEDELETECOLUMN     (USERMESSAGE+121)
#define MENU_TABLEADDROW           (USERMESSAGE+122)
#define MENU_TABLEADDCOL           (USERMESSAGE+123)
#define MENU_TABLEDELROW           (USERMESSAGE+124)
#define MENU_TABLEDELCOL           (USERMESSAGE+125)
#define MENU_TABLELINETYPE         (USERMESSAGE+126)
#define MENU_TABLESLANTTYPE        (USERMESSAGE+127)
#define MENU_TABLEMERGE            (USERMESSAGE+128)
#define MENU_TABLEDISMERGE         (USERMESSAGE+129)

#define MENU_KEYIDX                (USERMESSAGE+130)
#define MENU_SERVICE               (USERMESSAGE+131)
#define MENU_MEMSTAT               (USERMESSAGE+132)
#define MENU_ABOUT                 (USERMESSAGE+133)

#define MENU_SENDFILE              (USERMESSAGE+134)
#define MENU_RECVFILE              (USERMESSAGE+135)

#define MENU_CALCULATEPERSON       (USERMESSAGE+136)
#define MENU_CALCULATEMORE         (USERMESSAGE+137)


#define MENU_TOOLMOVE              (USERMESSAGE+800)
#define MENU_TOOLSELECT            (USERMESSAGE+801)
#define MENU_TOOLROTATE            (USERMESSAGE+802)
#define MENU_TOOLZOOM              (USERMESSAGE+803)
#define MENU_TOOLTEXT              (USERMESSAGE+804)
#define MENU_TOOLRECTANGLE         (USERMESSAGE+805)
#define MENU_TOOLCORNER            (USERMESSAGE+806)
#define MENU_TOOLELLIPSE           (USERMESSAGE+807)
#define MENU_TOOLPOLYGON           (USERMESSAGE+808)
#define MENU_TOOLTABLE             (USERMESSAGE+809)
#define MENU_TOOLSTRAIGHTLINE      (USERMESSAGE+810)
#define MENU_TOOLLINK              (USERMESSAGE+811)
#define MENU_TOOLUNLINK            (USERMESSAGE+812)

#define MENU_FONTSIZE              (USERMESSAGE+850)


/*--------------------------------------------------
#define MENU_LINEHEAD              (USERMESSAGE+41)
#define MENU_LINETAIL              (USERMESSAGE+42)
#define MENU_LINEWIDTH0            (USERMESSAGE+43)
#define MENU_LINEWIDTH1            (USERMESSAGE+44)
#define MENU_LINEWIDTH2            (USERMESSAGE+45)
#define MENU_LINEWIDTH4            (USERMESSAGE+46)
#define MENU_LINEWIDTH6            (USERMESSAGE+47)
#define MENU_LINEWIDTH8            (USERMESSAGE+48)
#define MENU_LINEWIDTH12           (USERMESSAGE+49)
#define MENU_USERLINEWIDTH         (USERMESSAGE+50)
#define MENU_LINECOLOR             (USERMESSAGE+51)
#define MENU_IMAGENORMALCONTRAST   (USERMESSAGE+52)
#define MENU_IMAGEHIGHCONTRAST     (USERMESSAGE+53)
#define MENU_IMAGEPOSTERIZE        (USERMESSAGE+54)
#define MENU_IMAGENEGATIVE         (USERMESSAGE+55)
#define MENU_IMAGECOLOR            (USERMESSAGE+56)
#define MENU_VIEWTABLE             (USERMESSAGE+64)
#define MENU_VIEWNET               (USERMESSAGE+65)
#define MENU_PALETTLE              (USERMESSAGE+67)

#define MENU_INCH                  (USERMESSAGE+109)
#define MENU_CM                    (USERMESSAGE+110)

////Char Shadow
#define MENU_CHARSHADOW0   (USERMESSAGE+250)
#define MENU_CHARSHADOW1   (USERMESSAGE+251)
#define MENU_CHARSHADOW2   (USERMESSAGE+252)
#define MENU_CHARSHADOW3   (USERMESSAGE+253)
#define MENU_CHARSHADOW4   (USERMESSAGE+254)
#define MENU_CHARSHADOW5   (USERMESSAGE+255)
#define MENU_CHARSHADOW6   (USERMESSAGE+256)
#define MENU_CHARSHADOW7   (USERMESSAGE+257)
#define MENU_CHARSHADOW8   (USERMESSAGE+258)
#define MENU_CHARSHADOW9   (USERMESSAGE+259)
#define MENU_CHARSHADOWF   (USERMESSAGE+260)
#define MENU_CHARSHADOWOTHER    (USERMESSAGE+261)
--------------------------------------------------*/

#define MENU_RUNMETA             13100
#define MENU_STOPMETA            13101

#define NULL_MODE 0
#define SEND_MODE 1
#define RECEIVE_MODE 2
#define DIAL_MODE 3
#define CONNECT_MODE 4
#define ERROR_MODE 5

#define _8K             0x2000              /* 8192 bytes                   */
#define BLK_SIZ         0x800               /* Starting block size          */
#define OVRHD           0x06                /* Private, JMODEM overhead     */
#define DAT_LEN         (_8K + 0x40)       /* Data buffer length           */
#define DAT_MAX         _8K                 /* Max block length             */

enum {  FT_TXT=0,
        FT_WPS=0x100,
        FT_WRITE=0x200,
        FT_CCED=0x300,
};

enum { LINE_NORMAL, LINE_BOLD, LINE_DBL, LINE_NONE };


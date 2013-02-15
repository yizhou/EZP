/////// Model ///////
  #define lpFAR
  #define FHUGE
  #define FFAR
  #define FAR                 lpFAR
  #define NEAR                near
  #define LONG                long
  #define VOID                void
  #define UDATA               UINT

typedef short int SHORT;
typedef unsigned short Wchar,WORD,USHORT;
typedef unsigned char UCHAR,BYTE;
typedef unsigned int  UINT;
typedef unsigned long ULONG,DWORD;
typedef int BOOL;

typedef int HFILENO;

typedef BYTE lpFAR  *LPBYTE;
typedef char lpFAR  *LPCHAR;
typedef UCHAR lpFAR *LPUCHAR;
typedef SHORT lpFAR *LPSHORT;
typedef USHORT lpFAR *LPUSHORT;
typedef int lpFAR  *LPINT;
typedef long lpFAR  *LPLONG;
typedef ULONG lpFAR *LPULONG;

typedef int ORDINATETYPE;
typedef int HANDLE;
typedef HANDLE HWND;
typedef HANDLE HMSG;
typedef HANDLE HMENU;
typedef HANDLE HGROUP;
typedef HANDLE HITEM;
typedef HANDLE HTEXT;
typedef HANDLE HLINE;
typedef HANDLE HPAGE;
typedef HANDLE HBOX;
typedef HBOX   HTEXTBOX;
typedef HBOX   HFormBoxs;
typedef HBOX   HLineBoxs;
typedef HBOX   HPICTUREBOX;

typedef int TEXTTYPE;

typedef unsigned long Function(HWND,HMSG,long,long);
//typedef int BoradcastProcedure(unsigned short,short,void *);
//typedef void BoradcastNoLowMemoryProcedure(void);

/*--- struct ---*/
#pragma pack(1);

typedef enum tagScreenModes
{
    MODE640X480X16=0,
    MODE800X600X16=1,
    MODE1024X768X16=2,
    MODE1024X768X256=3,
} ScreenModes;

/*------------- UI -------------------------*/
typedef struct tagWindows
{
  int Left,Top,Right,Bottom;           /* All Window rectangle field */
     #define MAXTITLELENGTH 60
  char Title[MAXTITLELENGTH];          /* Title of window */
  unsigned long WindowStyle;           /* see doc.txt */
  HWND FatherWindow;                   /* Farther Window, 0 is None */
  HWND PrevBrotherWindow;              /* Prev Brother Window, 0 is None */
  HWND NextBrotherWindow;              /* Next Brother Window, 0 is None */
  HWND NextChildWindow;                /* Child Window, 0 is None */
  HWND ActiveChildWindow;              /* Active Child Window */

  Function *Procedure;                 /* Window process function */
  HANDLE UserData;                     /* Extra user data */
} Windows;

typedef struct tagMessages
{
  HWND Window,Message;
  ULONG Data,ExtraData;
  HMSG Next;
} Messages;

typedef struct tagDialogs
{
  int ItemType;
  int ItemTotal;
  int ItemLeft,ItemTop,ItemRight,ItemBottom;
  int ItemUserData;
  Function *ItemProcedure;
  char ItemTitle[30];                //By zjh
} Dialogs;

typedef struct tagListBox
{
  short TotalList,CurrentList,TopList,ListHeight,ItemLength;
  HANDLE HSaveImage;
       #define MAXLISTCHAR 8012                         //By zjh ora=1012
  char ItemStrings[MAXLISTCHAR];
} ListBoxs;

typedef struct tagMenu
{
  HMENU PrevMenu,NextMenu,FatherMenu,ChildMenu;
  int MenuType;                 // see doc.txt
  char MenuName[14];
  char MenuShortChar;           // index of short char(when sub_menu is opened) in MenuName(0=NONE)
  char MenuUseSign;
  int MenuShortKey;
  int MenuReturnValue;
  int MenuHeight;                      /* Width=128 */
  HANDLE HSaveImage;
} MENUS;

typedef struct tagOpenFile
{
  char  filename[67];
  char  files[16];
  char  dirs[64];
  char  wildcard[32];
  char  drive[16];
  char  IsNewFileName;
  char  reserve[16];
} OpenFileStruct;

typedef struct tagSingleLine
{
  int StringLength;
  int LeftStart,CursorX,BoundX;
      #define MAXSINGLETEXTLENGTH 256
  unsigned char String[MAXSINGLETEXTLENGTH+1];
} SingleLine;

typedef struct tagTmpAttributes
{
  float UserAttribute1,UserAttribute2;
  int UserAttribute3,UserAttribute4;
} TmpAttributes;

typedef struct tagLoadMenus
{
  char MenuText[14];
  int MenuReturnValue;
  Wchar MenuShortKey;
  char MenuShortChar;
  int MenuType;
  int TotalSubMenu;
} LoadMenus;

/*------------- KERNL -------------------------*/
typedef struct tagFileHeads
{
  char FileSign[50];
  unsigned short Version;              /* BCD code */
  short TotalFilePages;
  short PageParameterLength;
  unsigned char PageFootType;  /* see doc.txt */
  unsigned char PageHeadType;  /* see doc.txt */
  unsigned short StartPageNum;
  UCHAR PageHeadLeftStr[60],PageHeadRightStr[60];
  UCHAR reserved[256-180-2];
  USHORT attr;                                          //By zjh for extern
  // bit 0: first block is Define Block;
} FileHeads;

typedef struct tagFilePageTables
{
  short TotalPageBox;
} FilePageTables;

typedef struct tagFileBoxTables
{
  short BoxParameterLength;
  int   TextLength;             // unsigned short //ByHance, 96,4.14
  HANDLE BoxDescribleHandle;
} FileBoxTables;

typedef struct tagFileBoxLinks
{
  HANDLE HIndexTextBox;
  HANDLE HConvertTextBox;
} FileBoxLinks;

typedef struct tagLibrayHeads
{
  char LibrarySign[58];
  unsigned short Version;              /* BCD code */
  short TotalLibraryPages;
  short PageParameterLength;
  unsigned char FileHeadReversed[64];
} LibraryHeads;

typedef struct tagLibraryPageTables
{
  short LibraryPageNumber;
  short TotalPageBox;
} LibraryPageTables;

typedef struct tagLibraryBoxTables
{
  short BoxParameterLength;
  int   TextLength;
  HANDLE BoxDescribleHandle;
} LibraryBoxTables;

typedef struct tagFIXED
{
    unsigned short fract;
    short  value;
} FIXED, lpFAR* LPFIXED;

typedef struct tagMAT2
{
    FIXED  eM11, eM12, eM21, eM22;
} MAT2, lpFAR* LPMAT2;

typedef struct tagGroupBoxs
{
    #define MAXGROUPBOX 40
  HBOX GroupHBox[MAXGROUPBOX];
  int GroupLeft,GroupTop,GroupWidth,GroupHeight;
  int GroupRotateAngle,GroupRotateAxisX,GroupRotateAxisY;
  unsigned char GroupSumBox;
  unsigned char GroupSign;             /* 0: No group, 1: Has group */
  HPAGE GroupPage;
} GroupBoxs;

typedef struct tagItems
{
  HITEM PrevItem,NextItem,ChildItem,FatherItem;
  HANDLE ItemHandle;
} Items;

typedef struct tagLines
{
  HITEM DescribeItem;
  unsigned char LineStyle,LineHead,LineTail;
  short LineWidth;
  HANDLE LineColor;
  unsigned char LineStatus;
  short LineParameter[4];
} Lines,*PLines;

typedef struct tagPages
{
  unsigned char PageType;   /* 5-0: A4,A3...6: Landscope, Patriot */
  unsigned char PageColumn;  /* 0 or 1 is none */
  //unsigned char PageFoot;    /* 0 is none, else is PageNumber style */
  //unsigned char reserved;       // ByHance, 96,3.8
  unsigned char VirtualPage; /* 6-0: VirtualPageSetting(0=none), 7:Init a box*/
  ORDINATETYPE PageWidth,PageHeight;
  //HANDLE PrePageNumberString;
  //HANDLE reserved2;       // ByHance, 96,3.8
  ORDINATETYPE MarginLeft,MarginTop,MarginRight,MarginBottom;
  short ColumnDistant;
} Pages,*PPages;

typedef struct tagClipBoards
{
  char ClipBoardDataType;
  int ClipBoardDataLength;
} ClipBoards;

//////////// IMAGE\image /////////////
typedef struct tagImageDescribes
{
  int ImageOriginX,ImageOriginY;
  float ImageScaleX,ImageScaleY;
  int ImageRotateAngle,ImageSkewAngle;
  int ImageDistant;
  HANDLE ImageColor;
  char ImageAttribute;
  HANDLE ImageHandle;
  HANDLE ImageNewHandle;
  unsigned char ByteRGBPalette[768],RGBPaletteSign;
  int ImageWidth,ImageHeight;
} ImageDescribes;

typedef struct tagInsertImages
{
  char InsertFileName[67];
  ImageDescribes InsertPresent;
} InsertImages;

typedef struct tagUndoItems
{
  unsigned char UndoType;
  int UndoBufferSize;
} UndoItems;

typedef struct extern_format {
    USHORT sub_line;       // 0: no sub_line;
    USHORT hollow;         // 0: no hollow   else low_byte 0-3:border width
                           //                     low_byte 4-7:internal color
    USHORT dim3;           // 0: no dim3     else low_byte 0-3:shadow width
                           //                              4-7 internal color
                           //                     high_byte 0,1 shadow dir
    USHORT background;     // 0: no bg       else fill type
    USHORT backcolor;
} EXT_FORMAT,*pEXT_FORMAT;

///////////// FORMAT\format //////////////
typedef struct mark_rec MarkStruct,*Pmark_rec;
struct mark_rec {
    int type;
    int selected;
    int ele_id;
    int edata_len;
    int start_pos;
    int x,y;
     //------ TextStyle,  must equ to ..\EDITOR\TextStyles -------
    USHORT CharSize;   //By zjh 10.29
    unsigned short font;
    short slant;
    USHORT CharHSize;    //By zjh 10.29
    HANDLE CharColor;
    short ParagraphAlign;//valid only when  type !=E_TEXT
    short RowGap;
    short superscript;
    short subscript;
    short ColGap;               //By zjh  See also format.h
    short UpDown;               //By zjh           text.h
    short VParagraphAlign;       //By zjh 9.12
    short SubLine;
    short Hollow;
    short Dim3;
    short BackGround;
    short BackColor;
    short no_use[13];           //By zjh
    HTEXTBOX hInsertBox;
     //---------------- End EQU ------------------------
    int width;
    int line_number;
    int line_height;
    int regno;  //when is FormBox it 's iCell
    HTEXTBOX hBox;
    Pmark_rec next;
    Pmark_rec prev;
};

typedef struct tagRTIXIN
{
  ORDINATETYPE x0Top,x1Top,y0Top,x0Bot,x1Bot,y1Bot;
} RTIXIN;
typedef struct tagRRECT
{
  ORDINATETYPE x0,y0,x1,y1;
} RRECT;

typedef struct tagREGIONITEM
{
  char type;              // RT_RECT or RT_TIXIN
  void *pRg;
} REGIONITEM,*LPREGIONITEM;

typedef struct tagTmpTextBoxs
{
  unsigned char BoxStatus; // see doc.txt
  unsigned char BoxType;   // see doc.txt
  ORDINATETYPE BoxLeft,BoxTop,BoxWidth,BoxHeight;
  //int RotateAngle;        /* 0-359 */
  WORD RotateAngle;        /* 0-359 */          //By zjh  9.12  /96
  WORD BoxAttr;                                 //By zjh  9.12  /96
  ORDINATETYPE RotateAxisX,RotateAxisY;
  HANDLE BoxBackColor;
  int BoxBorderType;
  int BoxBorderWidth;
  short RoundDistantLeft,RoundDistantTop,RoundDistantRight,RoundDistantBottom;
  short TextDistantLeft,TextDistantTop,TextDistantRight,TextDistantBottom;
  unsigned char TextAlign; // see doc.txt
  short BoxColumn;
  short ColumnDistant;
} TmpTextBoxs,*PTmpTextBoxs;

typedef struct tagTextBoxs
{
//  int ClipPolygons,ClipEdges[30];
     #define MAXPOLYGONNUMBER 120
//  ORDINATETYPE ClipBoxXY[10*MAXPOLYGONNUMBER];

  unsigned char BoxStatus; // see doc.txt
  unsigned char BoxType;   // see doc.txt
  ORDINATETYPE BoxLeft,BoxTop,BoxWidth,BoxHeight;
  //int RotateAngle;                   /* 0-359 */
  WORD RotateAngle;        /* 0-359 */          //By zjh  9.12  /96
  WORD BoxAttr;                                 //By zjh  9.12  /96
  ORDINATETYPE RotateAxisX,RotateAxisY;
  HANDLE BoxBackColor;
  int BoxBorderType;
  int BoxBorderWidth;
  short RoundDistantLeft,RoundDistantTop,RoundDistantRight,RoundDistantBottom;
  short TextDistantLeft,TextDistantTop,TextDistantRight,TextDistantBottom;
  unsigned char TextAlign; // see doc.txt
  short BoxColumn,ColumnDistant;
  int TextLength;
  int TextBlockLength;
  HANDLE TextHandle;
  HANDLE PrevLinkBox,NextLinkBox;

//     #define MAXVIRTUALREGIONNUM 250
  Wchar  *text;
  Pmark_rec formatted_elements,elements_end;
//  Pmark_rec region2mark[MAXVIRTUALREGIONNUM];

      #define MAXREGIONNUM 120
  int numRg;
  REGIONITEM rgList[MAXREGIONNUM];

  int reserved[10];

  int InvalidPolygons;
  short InvalidEdges[30];
  short InvalidBoxXY[10*MAXPOLYGONNUMBER];
  //ORDINATETYPE InvalidBoxXY[10*MAXPOLYGONNUMBER];
} TextBoxs,*PTextBoxs;

typedef struct tagCELL
{
   HFormBoxs hParentBox;
   int numLines,numCols;
   int iFirst;
   int iSelf;
   int bSlip;           // Add By DG
} CELL;

typedef struct tagTmpFormBoxs
{
  unsigned char BoxStatus; // see doc.txt
  unsigned char BoxType;   // see doc.txt
  ORDINATETYPE BoxLeft,BoxTop,BoxWidth,BoxHeight;
  //int RotateAngle;         /* 0-359 */
  WORD RotateAngle;        /* 0-359 */          //By zjh  9.12  /96
  WORD BoxAttr;                                 //By zjh  9.12  /96
  ORDINATETYPE RotateAxisX,RotateAxisY;
  HANDLE BoxBackColor;
  int BoxBorderType;
  int BoxBorderWidth;
  short RoundDistantLeft,RoundDistantTop,RoundDistantRight,RoundDistantBottom;
  short TextDistantLeft,TextDistantTop,TextDistantRight,TextDistantBottom;
  unsigned char TextAlign; // see doc.txt
  short numLines,numCols;  /* in TextBox, it is BoxColumn and ColumnDistant */
} TmpFormBoxs,*PTmpFormBoxs;

typedef struct tagFormBoxs
{
//  int ClipPolygons,ClipEdges[30];
//  ORDINATETYPE ClipBoxXY[10*MAXPOLYGONNUMBER];

  unsigned char BoxStatus; // see doc.txt
  unsigned char BoxType;   // see doc.txt
  ORDINATETYPE BoxLeft,BoxTop,BoxWidth,BoxHeight;
  //int RotateAngle;      /* 0-359 */
  WORD RotateAngle;        /* 0-359 */          //By zjh  9.12  /96
  WORD BoxAttr;                                 //By zjh  9.12  /96
  ORDINATETYPE RotateAxisX,RotateAxisY;
  HANDLE BoxBackColor;
  int BoxBorderType;
  int BoxBorderWidth;
  short RoundDistantLeft,RoundDistantTop,RoundDistantRight,RoundDistantBottom;
  short TextDistantLeft,TextDistantTop,TextDistantRight,TextDistantBottom;
  unsigned char TextAlign; // see doc.txt
  short numLines,numCols;  /* in TextBox, it is BoxColumn and ColumnDistant */
  int TextLength;
  int TextBlockLength;
  HANDLE TextHandle;
  HANDLE PrevLinkBox,NextLinkBox;      // Reserved for implement

  Wchar  *text;
  Pmark_rec formatted_elements,elements_end;
//  Pmark_rec region2mark[MAXVIRTUALREGIONNUM];

  int numRg;
  REGIONITEM rgList[MAXREGIONNUM];

  int reserved[10];

  int InvalidPolygons;
  short InvalidEdges[30];
  short InvalidBoxXY[10*MAXPOLYGONNUMBER];
  //ORDINATETYPE InvalidBoxXY[10*MAXPOLYGONNUMBER];

    #define MAXFORMLINE 60
  int vertline[MAXFORMLINE],hortline[MAXFORMLINE];
  int vertlineType[MAXFORMLINE],hortlineType[MAXFORMLINE]; //By Dg, 1996,3
  HANDLE hCellTable;
} FormBoxs,*PFormBoxs;

typedef struct tagTmpLineBoxs
{
  unsigned char BoxStatus; // see doc.txt
  unsigned char BoxType;   // see doc.txt
  ORDINATETYPE BoxLeft,BoxTop,BoxWidth,BoxHeight;
  //int RotateAngle;        /* 0-359 */
  WORD RotateAngle;        /* 0-359 */          //By zjh  9.12  /96
  WORD BoxAttr;                                 //By zjh  9.12  /96
  ORDINATETYPE RotateAxisX,RotateAxisY;
  HANDLE BoxBackColor;
  int BoxBorderType;
  char BoxArrowHeadType,BoxArrowTailType;
  short RoundDistantLeft,RoundDistantTop,RoundDistantRight,RoundDistantBottom;
} TmpLineBoxs,*PTmpLineBoxs;

typedef struct tagLineBoxs
{
//  int ClipPolygons,ClipEdges[30];
//  ORDINATETYPE ClipBoxXY[10*MAXPOLYGONNUMBER];

  unsigned char BoxStatus; // see doc.txt
  unsigned char BoxType;   // see doc.txt
  ORDINATETYPE BoxLeft,BoxTop,BoxWidth,BoxHeight;
  //int RotateAngle;       /* 0-359 */
  WORD RotateAngle;        /* 0-359 */          //By zjh  9.12  /96
  WORD BoxAttr;                                 //By zjh  9.12  /96
  ORDINATETYPE RotateAxisX,RotateAxisY;
  HANDLE BoxBackColor;
  int BoxBorderType;
  char BoxArrowHeadType,BoxArrowTailType;
  short RoundDistantLeft,RoundDistantTop,RoundDistantRight,RoundDistantBottom;

  int reserved[10];
} LineBoxs,*PLineBoxs;

typedef struct tagTmpPictureBoxs
{
  unsigned char BoxStatus; // see doc.txt
  unsigned char BoxType;   // see doc.txt
  ORDINATETYPE BoxLeft,BoxTop,BoxWidth,BoxHeight;
  //int RotateAngle;      /* 0-359 */
  WORD RotateAngle;        /* 0-359 */          //By zjh  9.12  /96
  WORD BoxAttr;                                 //By zjh  9.12  /96
  ORDINATETYPE RotateAxisX,RotateAxisY;
  HANDLE BoxBackColor;
  int BoxBorderType;
  int BoxBorderWidth;
  short RoundDistant;
  int ImageOriginX,ImageOriginY;
  float ImageScaleX,ImageScaleY;
  int ImageRotateAngle,ImageSkewAngle;
  int ImageDistant;
  HANDLE ImageColor;
  char ImageAttribute;     // see doc.txt
} TmpPictureBoxs,*PTmpPictureBoxs;

typedef struct tagPictureBoxs
{
//  int ClipPolygons,ClipEdges[30];
//  ORDINATETYPE ClipBoxXY[10*MAXPOLYGONNUMBER];

  unsigned char BoxStatus; // see doc.txt
  unsigned char BoxType;   // see doc.txt
  ORDINATETYPE BoxLeft,BoxTop,BoxWidth,BoxHeight;
  //int RotateAngle;            /* 0-359 */
  WORD RotateAngle;        /* 0-359 */          //By zjh  9.12  /96
  WORD BoxAttr;                                 //By zjh  9.12  /96
  ORDINATETYPE RotateAxisX,RotateAxisY;
  HANDLE BoxBackColor;
  int BoxBorderType;
  int BoxBorderWidth;
  short RoundDistant;
  ImageDescribes PicturePresent;
  char PictureFileName[67];
  HANDLE BorderPolygon;         // see doc.txt
  int reserved[10];
} PictureBoxs,*PPictureBoxs;

typedef union tagUserTmpBoxs
{
  TmpTextBoxs TmpTextBox;
  TmpFormBoxs TmpFormBox;
  TmpPictureBoxs TmpPictureBox;
  TmpAttributes TmpAttr;
} UserTmpBoxs;

typedef union tagBoxs
{
  TextBoxs TextBox;
  FormBoxs FormBox;
  LineBoxs LineBox;
  PictureBoxs PictureBox;
  TmpTextBoxs TmpTextBox;
  TmpFormBoxs TmpFormBox;
  TmpLineBoxs TmpLineBox;
  TmpPictureBoxs TmpPictureBox;
} Boxs;

/*------------- EDITOR -------------------------*/
typedef struct tagFindStructs
{
  unsigned char FindOption;       //see doc.txt
      #define MAXFINDLENGTH 50
  Wchar FindString[MAXFINDLENGTH];
  Wchar ReplaceToString[MAXFINDLENGTH];
} FindStructs;

typedef struct tagTextStyles
{     /* Bit 15-11: type, else: data, 0< Bit 15-11 < 10101b */
 //------------ must EQU to ..\format\mark_rec -----------
  USHORT CharSize;      //By zjh 10.29
  unsigned short CharFont;
  short CharSlant;
  USHORT CharHSize;     //By zjh 10.29
  HANDLE CharColor;
  short ParagraphAlign;
  short RowGap;
  short Superscript;
  short Subscript;
  short ColGap;               //By zjh  See also format.h
  short UpDown;               //By zjh           text.h
  short VParagraphAlign;       //By zjh 9.12
  short SubLine;
  short Hollow;
  short Dim3;
  short BackGround;
  short BackColor;
  short no_use[13];           //By zjh
  HANDLE EnbodyBox;
 //-------------- end EQU --------------
} TextStyles;

/*------------- IMAGE -------------------------*/
typedef struct tagImageLineBuffers     /* For compute a tmp image present */
{                                      /* with slant & rotate */
  short ImageLineLeft;
  short ImageLineLength;
  unsigned char ImageLineBufferData[1];
} ImageLineBuffers;

typedef struct tagTiffIFDType
{
  unsigned short SignCode;
  unsigned short DataType;
  unsigned long Length;
  unsigned long Data;
} IFDType;

typedef struct tagTIFFHeads
{
  unsigned short Sign;
  unsigned short Version;
  unsigned long  FirstIFDOffset;
} TIFFHeadType;

/*------------- FORMAT -------------------------*/

typedef struct tagCSIZE {
    UINT width;
    UINT height;
} CSIZE;

typedef struct tagCSTYLE {
    unsigned short ccfont;
    unsigned short eefont;
} CSTYLE;

typedef struct  {
    int degree;        /* from 0 to 30 */
    unsigned char direction;
} SLANT_WORD;

typedef struct {
    unsigned char toDo ;
    unsigned short patternN;
    unsigned char border;
} HOLLOW_WORD;

typedef struct {
    unsigned char toDo;
    unsigned char direction;
    int   width;
    short step;
    unsigned char border;
} DIM3;

typedef struct {
    unsigned char toDo;
    unsigned char type;
    int  width;
} OUTLINE_WORD;

typedef struct tagRCH {
    Wchar code;
    unsigned char codeType;
    CSIZE chSize;
    CSIZE chSize1;
    CSTYLE cStyle;
    SLANT_WORD Slant;
    int   Rotate;      /* rotate angle, from 0 to 360 */
    HOLLOW_WORD Hollow;
    DIM3 dim;
    OUTLINE_WORD Outline;
} RCH,*PRCH, lpFAR *LPRCH;

typedef struct cfnt_tag {
       char cfnPath[256];
       short cfnNumber;
       FILE *cfnFile;
       LONG  libstart;
       USHORT emsize;
       UCHAR cfnHead[256];
       UCHAR cbuffer[4096];
       UCHAR cfnIndex[40000];
} VECFNT,lpFAR *LPVECFNT;

typedef struct {
  ULONG tag;
  ULONG checkSum;
  ULONG offset;
  ULONG length;
} TABDIR,lpFAR *LPTABDIR;

typedef struct tagTTGlyph {
    #define TTMAXCONTOUR    128
    #define TTMAXDOTN       1024
  short   contourn;                   /* total contours */
  short   pointn;                     /* total points */
  short enddot[TTMAXCONTOUR];       /* enddot buffer */
  unsigned char flag[TTMAXDOTN];    /* flag buffer */
  int   xbuf[TTMAXDOTN];
  int   ybuf[TTMAXDOTN];
} TTGLYPH, lpFAR *LPTTGLYPH;

typedef struct {
  ULONG tag;
  LPTABDIR lpdir;
} tagPair_t;

typedef struct tagTTF {
    /*----HEAD item----*/
      #define HEADBUFLEN        60
      UCHAR HEADBuf[HEADBUFLEN];
      USHORT unitsPerEM;
      SHORT TTxMin,TTyMin,TTxMax,TTyMax;
      SHORT indexToLocFormat;
    /*----HHEA item-----*/
      #define HHEABUFLEN        36
      UCHAR HHEABuf[HHEABUFLEN];
      SHORT ascender;
    /*----MAXP item-----*/
      #define MAXPBUFLEN        40
      UCHAR MAXPBuf[MAXPBUFLEN];
      USHORT numGlyphs;
    /*----CMAP item-----*/
      UCHAR CMAPBuf[4096];
      USHORT cmapstart;
    /*----LOCA item-----*/
      UCHAR LOCABuf[0x8000];
      UCHAR HMTXBuf[2048];
    /*----GLYF item-------*/
      UCHAR GLYFBuf[4096];
      ULONG  glyphBase;
      SHORT  numberOfContours;

      char ttPath[256];
      short ttfnNumber;
      int   httf;       /* file handle */
      USHORT ttWidth,ttHeight;
      TTGLYPH aglyf;

      #define HEADERLEN       512
      UCHAR ttfHeadBuf[HEADERLEN];
      LPUSHORT  lpnumTables;
      LPTABDIR  lpTableDirectory;
}  TTFINFO,lpFAR *LPTTFINFO;

typedef struct {
  USHORT platform;
  USHORT plat_spec;
  ULONG  offset;
} TTCMAPREC,lpFAR * LPTTCMAP;

typedef struct tagDC {
  int left,top,right,bottom;
  LPVECFNT lpvfnt;        /* thvect.xxx */
  LPTTFINFO lpttf;        /* true type font */
} DC, lpFAR *LPDC;

typedef void EdgeFillLine(LPDC,int,int,int,int);
typedef void LineFillLine(int,int,int,LPDC);

typedef struct {
   int DeviceType;                    // see below
   int (*printInit)(UDATA pagew,UDATA pageh);      // send printer init string,and alloc memory
   void (*printBlock)();              // send a block of data to printer
   void (*printFormFeed)();           // eject a page
   void (*printOver)();               // free memory, send printer over string
   int (*printGetBlockHeight)();      // get block height
   LineFillLine *printScanLine;       // draw scan line

   void (*printSetColor)(int color);
   void (*printSetRGBcolor)(int r,int g,int b);
   void (*printSetCMYKcolor)(int c,int m,int y,int k);
   void (*printSetGray)(int gray);

   int resolution;                    // printer resolution
   int xpixel,ypixel;                 // printer width and height in pixels
   int LeftMargin,TopMargin;          // Fixed Margin in pixels
} PRINTER;

typedef struct tagPOINT
{
  int x,y;
} POINT, *PPOINT, lpFAR *LPPOINT;

typedef struct tagEDGE
{
  int yMax;
  int xStart;
  int dxInt;
  short dxFrac;       // dxFrac and halfOne type is according to MAXFRAC
  short halfOne;
  short nextItem;
  signed char direction;
} EDGE,*PEDGE,lpFAR *LPEDGE;

typedef struct tagFILLP
{
  LPDC lpdc;                           /* device parameter */
  short dotCount;                      /* Total dots number of polypolygon */
    #define MAXDOTN 2000       /* Must set stack deeepth great than 10*MAXDOTN */
  int sortYarray[MAXDOTN];             /* Y sorted index buffer */
  int xbuffer[MAXDOTN];                /* x cordinate buffer */
  int ybuffer[MAXDOTN];                /* y cordinate buffer */
  short lastDot[MAXDOTN];              /* last dot index buffer */
  short nextDot[MAXDOTN];              /* next dot index buffer */
      #define MAXEDGN 200
  EDGE edgeTable[MAXEDGN];             /* edge table */
  short used;                          /* used edge link */
  short free;                          /* free edge link */
} FILLP,*PFILLP,lpFAR *LPFILLP;


typedef struct {
   char               Sign;             /* must be 0xa */
   char               Version;          /* 0,2,3,5 (5=256 color) */
   char               PcxCompress;      /* must be 0x1(run_length) */
   char               BitsPerPixel;
   WORD               Xmin, Ymin;       /* normally, is 0 */
   WORD               Xmax, Ymax;
   WORD               Hres, Vres;       /* device resolution */
   char               Palette[16*3];    /* 16 colors_palette(R,G,B)  */
   char               Reserved;
   char               Planes;
   WORD               BytesPerLine;
   WORD               PaletteInfo;      /* 1=grey, 2=color */
   char               Filler[58];
} PCXHeadType;


typedef struct tagBitMapFileHeader {
   WORD         Type;
   DWORD        Size;
   WORD         Reserved1;
   WORD         Reserved2;
   DWORD        ImageOffset;
} BMPFileHead;

typedef struct tagBitMapInfoHeader {
   DWORD        ImageSize;
   DWORD        ImageWidth;
   DWORD        ImageHeight;
   WORD         Planes;
   WORD         BitsPerPixel;
   DWORD        Compression;
   DWORD        SizeImage;
   DWORD        XResolution;
   DWORD        YResolution;
   DWORD        biClrUsed;
   DWORD        biClrImportant;
} BMPInfoHead;

typedef struct tagRGBquad {
   BYTE rgbBlue;
   BYTE rgbGreen;
   BYTE rgbRed;
   BYTE rgbReserved;
} BMPRGBquad;

typedef struct tagPrintBlock {
   unsigned short Reveser;
   unsigned short Rotate;
   float Xoffset;
   float Yoffset;
   float Xscale;
   float Yscale;
   short PageOffset;
} PrintBlock;

typedef struct tagPrintCut {
   unsigned short Blocks;            //If your want to modify this struct
   unsigned short PageInc;           //please see WriteExternBlock() in filec
   unsigned short Enable;            //or will error  By zjh 12.9/96
   unsigned short Reverse[5];
   PrintBlock PageBlock[200];
} PP;

typedef struct {
   int PagePrintX0,PagePrintY0,PagePrintX1,PagePrintY1; //页面有效输出区域:
   int PrintPaperWidth,PrintPaperHeight;                //用户打印机中打印纸张的大小
   int LeftMargin,TopMargin,RightMargin,BottomMargin;   //打印纸张边空
   int LayX,LayY;                                       //重叠部分
   int pt_w,pt_h,pg_w,pg_h;
   int TotalX,TotalY;
   int AutoCenter;
} PB,*PPB;


typedef struct
{
   long  next_block ;  /* Memo Entry 1,2, ... */
} MEMOMNDX ;

typedef struct
{
      long  next_block ;  /* Memo Entry 1,2, ... */
      char  usused[2] ;
      short block_size ;  /* Bytes */
} MEMOFOX ;

typedef struct
{
      long  type ;         /* 0 for picture, 1 for text -- picture not supported */
      long  num_chars ;    /* Including the 'MemoBlock' */
} MEMOFOXBLOCK ;

typedef struct
{
      short minus_one ;    /* '-1' for dBASE IV */
      short start_pos ;
      long  num_chars ;    /* Including the 'MemoBlock' */
} MEMOBLOCKDBT ;


typedef struct
{
      long  next_block ;  /* Memo Entry 1,2, ... */
      long  zero ;
      char  file_name[8] ;
      short zero2 ;
      short x102 ;
      short block_size ;  /* Bytes */
      short zero3 ;
} MEMODBT ;

typedef struct
{
   /* Database Header Information */
   unsigned char     version ;           /* 83H with .dbt, 03H without */
   char     yy ;                /* Last Update */
   char     mm ;
   char     dd ;
   long     num_recs ;
   unsigned short header_len;   /* Header Length, Indicates start of data */
   unsigned short record_len;
   char     zero[16] ;
   char     has_mdx ;           /* 1 for true */
   char     dummy ;
   unsigned short num_field;    /* char     zero2[2] ;   modify by zjh */
} DBF_HEADER ;

typedef struct  /* Data File Format */
{
   char     name[11] ;
   char     type ;
                                   /*
                                   'N'---int;
                                   'F'---float;
                                   'C'---char;
                                   'D'---data;
                                   'L'---logical;
                                   'M'---memo;
                                   'G'---????
                                   */
   char     filler[4] ;
   unsigned char  len ;
   unsigned char  dec ;
                                   /*
                                   'N'---int;       len=int.dec+1
                                   'F'---float;     len=int.dec+1

                                   'C'---char;      len=len+dec*256

                                   'D'---data;      dec not define
                                   'L'---logical;
                                   'M'---memo;
                                   'G'---????
                                   */
   char     filler2[13] ;
   char     has_tag ;
} DBF_FIELD;

typedef struct  /* Data File Format */
{
   char                 name[12] ;
   char                 type ;
   unsigned short   len ;
   unsigned char    dec ;
   unsigned short   start;
} DBF_FIELDLIST;

typedef struct
{
  FILE *fp;
  long addr;
  FILE *memo;
  long memoaddr;
  char memotype;                /* 0:no 1:foxpro */
} DBFHandle;


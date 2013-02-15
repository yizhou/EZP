
// Pay attention: In this file 'int' referring to 32 bits int.
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

typedef enum tagScreenModes
{
    MODE640X480X16=0,
    MODE800X600X16=1,
    MODE1024X768X16=2,
    MODE1024X768X256=3,
} ScreenModes;

#define MAXCHARSIZE MAXCHARFACT*512     //By zjh  10.29
#define MAXCHARHSIZE MAXCHARFACT*512    //By zjh  10.29
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

#define FACT MAXCHARFACT
int CompressCHARSIZE(int yy)
{
    int max=FACT*512;
    yy=yy%max;
    if (yy>=4096) return yy/FACT+0x600;
    if (yy>=1024) return yy/8+0x400;
    return yy;
}

int UncompressCHARSIZE(int yy)
{
    int i;
    i=((yy&0x600)>>9);
    switch (i)
    {
        case 0:
        case 1:
            return (yy&0x3ff);
        case 2:
            return (yy&0x1ff)*8;
        case 3:
            return (yy&0x1ff)*FACT;
    }

    return (yy&0x3ff);     //cannot arrive
}

USHORT make_attr(int II,int AA)
{
    if (II==CHARSIZE||II==CHARHSIZE) AA=CompressCHARSIZE(AA);
    return ((AA)|((II)<<ATTRIBUTEBITS));
}

int get_attr(USHORT GG)
{
    if (GetPreCode(GG)==CHARSIZE||GetPreCode(GG)==CHARHSIZE)
      return  UncompressCHARSIZE((GG)&ATTRIBUTEPATTERN);
    return ((GG)&ATTRIBUTEPATTERN);
}

#define MakeATTRIBUTE(II,AA) make_attr(II,AA)      //By zjh  10.29

#define GetAttribute(GG) get_attr(GG)              //By zjh  10.29
#define GetCharCode(GG) (GG)
#define GetPreCode(GG) (((GG)&ATTRIBUTEPRECODE)>>ATTRIBUTEBITS)

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

typedef struct tagFormBoxs
{
  unsigned char BoxStatus; // see doc.txt
  unsigned char BoxType;   // see doc.txt
  ORDINATETYPE BoxLeft,BoxTop,BoxWidth,BoxHeight;
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

  int numRg;
  REGIONITEM rgList[MAXREGIONNUM];

  int reserved[10];

  int InvalidPolygons;
  short InvalidEdges[30];
  short InvalidBoxXY[10*MAXPOLYGONNUMBER];

  #define MAXFORMLINE 60
  int vertline[MAXFORMLINE],hortline[MAXFORMLINE];
  int vertlineType[MAXFORMLINE],hortlineType[MAXFORMLINE]; //By Dg, 1996,3
  HANDLE hCellTable;
} FormBoxs,*PFormBoxs;

typedef struct tagTextBoxs
{
  #define MAXPOLYGONNUMBER 120

  unsigned char BoxStatus; // see doc.txt
  unsigned char BoxType;   // see doc.txt
  ORDINATETYPE BoxLeft,BoxTop,BoxWidth,BoxHeight;
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

  Wchar  *text;
  Pmark_rec formatted_elements,elements_end;

  #define MAXREGIONNUM 120
  int numRg;
  REGIONITEM rgList[MAXREGIONNUM];

  int reserved[10];

  int InvalidPolygons;
  short InvalidEdges[30];
  short InvalidBoxXY[10*MAXPOLYGONNUMBER];
} TextBoxs,*PTextBoxs;

#define _VAR_C_
/*-------------------------------------------------------------------
* Name: varc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

PPB SubP;
const char FILEHEADSTRING[]="REDTEK EZ Publisher file. Version 1.00\n\x1a";
int GlobalXPage=0;
int GlobalYPage=0;
int GlobalSubPage=1;
int GlobalBlockHide=0;
int GlobalHollow=0;
// int zjh_HandleMax=0;
float PaperW,PaperH;
//int UsePrintCut=0;
PP PG;
float YScale=1.0;
float XScale=1.0;
float SYScale=1.0;
float SXScale=1.0;
float PYScale=1.0;
float PXScale=1.0;

EXT_FORMAT GlobalExtFormat;
BOOL fNewA4=FALSE;
int count=0;
WORD IsModule=0;
char TrueTypeLibPath[MAXSINGLETEXTLENGTH]={"d:\\ttf\\;"};
char VectLibPath[MAXSINGLETEXTLENGTH]={"d:\\ttf\\;"};
ScreenModes ScreenMode=-1;  //MODE640X480X16;
const char Symbol24FileName[]="c:/ezp/fonts/clib24t";
const char Dotlib24FileName[]="c:/ezp/fonts/clib24s";
const char SKHF_Name[]="skhf";
UCHAR *ASC16LibBuffer=NULL;

char *cfnName="THVECT.";
//char *ttName="TTFALIB.";

BOOL fEditor=FALSE,fAutoLoad=FALSE;
BOOL fFirstBlock;
char DefaultFileName[100];
UCHAR PageHeadLeftStr[PAGEHEADSTRMAXLEN],PageHeadRightStr[PAGEHEADSTRMAXLEN];
int  CurrentRow=1,PgFtStartNum=1;
BOOL tmp_footflag = FALSE;
int tmp_foottopflag = FOOTBOTTOM;
int tmp_footleftflag = FOOTLEFT_RIGHT;
int tmp_footprevflag = NO_PREV;
BOOL tmp_headflag = FALSE;
int tmp_headleftflag = FOOTLEFT_RIGHT;
int tmp_headlineflag=HEADLINE_SINGLE;
int tmp_TableHeadOption=1;
int tmp_TableStyleOption=0;
int tmp_TableLineColOption=0;
int tmp_insflag = 0;
int tmp_nline;
int GlobalFontSize=DEFAULTCHARSIZE;
DC myDC;
int GlobalPageRotate=0;
int GlobalReverse=0;
int GlobalYReverse=1;
int GlobalXOffset=0;
int GlobalYOffset=0;
int GlobalXRes=0;
int GlobalYRes=0;
int GlobalRorate90=0;
int GlobalPause=0;
int GlobalJob=0;
char fGetFocusByKey=0;
UDATA PageHightDot;

int CellisMoving=0;

HANDLE ActiveWindow=0,GlobalWindowHandle=0,GlobalHMenu=0;
HMENU  ActiveMenu=-1;
HANDLE GlobalMessageHandle=0;
HANDLE GlobalItemsHandle=0;
HITEM GlobalPageHeadHandle=0;
HITEM GlobalBoxHeadHandle=0;
HPAGE GlobalCurrentPage=0;
HWND ActiveTopMenu=0;
int  TotalPage=0;
Items *DataofItems;
MENUS  *DataofMenus;
Windows *DataofWindows;
Messages *DataofMessages;
HMSG MessageQueueHead=0,MessageQueueTail=0;

int GlobalTimer=-1;
unsigned char BmpBuf[BmpBufLen];     /* bitmap in normal and pressed status */

unsigned short  NowEFont=0;
short ASC32AW[MAXEFONT][MAXASC32CODE];

HMSG IconMenuIdxArr[TotalIconNumber]={
   MENU_NEW,  MENU_OPEN,     MENU_SAVE,     MENU_IMPORTTEXT,
   MENU_FONT, MENU_FONTSIZE,
   MENU_TOOLMOVE, MENU_TOOLSELECT, MENU_TOOLROTATE, MENU_TOOLZOOM,
   MENU_TOOLTEXT, MENU_TOOLRECTANGLE, MENU_TOOLCORNER,
   MENU_TOOLELLIPSE, MENU_TOOLPOLYGON, MENU_TOOLTABLE,
   MENU_TOOLSTRAIGHTLINE, MENU_TOOLLINK, MENU_TOOLUNLINK,
   MENU_PRINT
};
char *IconHintArr[TotalIconNumber]={
   "创建新文件 Ctrl+N",
   "读取文件 Ctrl+O",
   "将编辑的文件存盘 Ctrl+S",
   "插入文本或图像文件 F3",
   "选择字体或改变标块的字体 F5",
   "选择字号或改变标块的字号 F6",
   "版框编辑与设计 Ctrl+鼠标左键",
   "编辑版框中的文字或图像 Shift+鼠标左键",
   "旋转没加锁的版框",
   "缩放视图(鼠标左键放大,右键缩小)",
   "创建文本版框",
   "创建矩形图像版框",
   "创建圆角图像版框",
   "创建椭圆图像版框",
   "创建自由图像版框",
   "创建自动表格",
   "画直线",
   "在两文本版框间建立串文关系",
   "解除文本版框与前一个文本版框间的串文关系",
   "打印输出 Ctrl+P"
};

ListBoxs TestList[MAXLISTBOXS];
int CurrentAllocList=0;
SingleLine TestLineBuffer[MAXSINGLELINEBUFFER];
int CurrentAllocLine=0;

OpenFileStruct *NowOpenFile;
UserTmpBoxs TmpBox;
Pages TmpPage;

Dialogs TableCalculateDialog[]=
{
  #define X 124
  #define Y 133
  { GLOBALITEM, 8, 106, 133, 490, 320, 0, KeyHelpProcedure,"表格计算" },

  { STATICTEXTITEM, 0, 162-X,      190-Y, 162-X+18*8,      210-Y, 0, NULL, "计算出的值存放在裁" },
  { STATICTEXTITEM, 0, 162-X+18*8, 190-Y, 162-X+18*8+19*8, 218-Y, 0, NULL, "剪板中,您可将光标移" },
  { STATICTEXTITEM, 0, 162-X,      214-Y, 162-X+19*8,      234-Y, 0, NULL, "到适当地方,然后用粘" },
  { STATICTEXTITEM, 0, 162-X+19*8, 214-Y, 162-X+19*8+19*8, 234-Y, 0, NULL, "贴功能(Shift+Ins)将" },
  { STATICTEXTITEM, 0, 162-X,      238-Y, 162-X+19*8,      258-Y, 0, NULL, "计算结果取出!" },

  { USERBUTTONITEM, 0, 160-X, 270-Y, 259-X, 297-Y, 10091, NULL, "求和" },
  { USERBUTTONITEM, 0, 268-X, 270-Y, 367-X, 297-Y, 10092, NULL, "求平均值" },
  { CANCELBUTTON,   0, 376-X, 270-Y, 475-X, 297-Y, 0,     NULL, "放弃" },

  #undef X
  #undef Y
};

Dialogs PageFootDialog[]=
{
#define X     50
#define Y     120
  { GLOBALITEM, 8 , X, Y, (X+426-50), (Y+326-100), 0, NULL,"页码设置" },

  { MULTISELECT, 0, 82-X, 164-Y, 178-X, 184-Y, 0, PageFootEnableProcedure, "允许页号" },
      #define FX        (X+16)
      #define FY        (Y+79)
  { FRAMEITEM, 2, FX-X, FY-Y, 180-X, 278-Y, 0, NULL, "页号上下位置" },
      { SINGLESELECT, 0, 78-FX, 222-FY, 176-FX, 242-FY, 0, PgFtTDProcedure,"页面上端" },
      { SINGLESELECT, 0, 78-FX, 248-FY, 176-FX, 268-FY, 1, PgFtTDProcedure,"页面下端" },
      #undef FX
      #undef FY

      #define FX        (X+138)
      #define FY        (Y+79)
  { FRAMEITEM, 4, FX-X, FY-Y, 303-X, 278-Y, 0, NULL, "页号左右位置" },
      { SINGLESELECT, 0, 193-FX, 222-FY, 243-FX, 242-FY, 0, PgFtLRProcedure, "左边" },
      { SINGLESELECT, 0, 246-FX, 222-FY, 296-FX, 242-FY, 1, PgFtLRProcedure, "右边" },
      { SINGLESELECT, 0, 193-FX, 248-FY, 243-FX, 268-FY, 2, PgFtLRProcedure, "左右" },
      { SINGLESELECT, 0, 246-FX, 248-FY, 296-FX, 268-FY, 3, PgFtLRProcedure, "中间" },
      #undef FX
      #undef FY

      #define FX        (X+262)
      #define FY        (Y+79)
  { FRAMEITEM, 3, FX-X, FY-Y, 410-X, 278-Y, 0, NULL, "页号前后缀" },
      { SINGLESELECT, 0, 320-FX, 222-FY, 354-FX, 242-FY, 0, PgFtPrevProcedure, "无" },
      { SINGLESELECT, 0, 360-FX, 222-FY, 400-FX, 242-FY, 1, PgFtPrevProcedure, "点" },
      { SINGLESELECT, 0, 320-FX, 248-FY, 400-FX, 268-FY, 2, PgFtPrevProcedure, "短横线" },
      #undef FX
      #undef FY

  { STATICTEXTITEM, 0, 70-X, 298-Y, 134-X, 316-Y, 0, NULL, "起始页号" },
  { SINGLELINEEDITORITEM, 0, 142-X, 296-Y, 245-X, 316-Y, 0, PgFtStartNumProcedure, "" },

  { OKBUTTON,     0, 276-X, 291-Y, 340-X, 321-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 348-X, 291-Y, 412-X, 321-Y, 0, NULL, "放弃" },

#undef X
#undef Y
};

Dialogs PageHeadDialog[]=
{
#define X     50
#define Y     100
  { GLOBALITEM, 10 , X, Y, (X+426-50), (Y+350-100), 0, NULL,"篇眉设置" },

  { MULTISELECT, 0, 82-X, 144-Y, 178-X, 164-Y, 0, PageHeadEnableProcedure, "允许篇眉" },

      #define FX        (X+16)
      #define FY        (Y+79)
  { FRAMEITEM, 4, FX-X, FY-Y, 190-X, 258-Y, 0, NULL, "篇眉左右位置" },
      #undef FX
      #define FX        (X+136)
      { SINGLESELECT, 0, 193-FX, 202-FY, 243-FX, 222-FY, 0, PgHdLRProcedure, "左边" },
      { SINGLESELECT, 0, 246-FX, 202-FY, 296-FX, 222-FY, 1, PgHdLRProcedure, "右边" },
      { SINGLESELECT, 0, 193-FX, 228-FY, 243-FX, 248-FY, 2, PgHdLRProcedure, "左右" },
      { SINGLESELECT, 0, 246-FX, 228-FY, 296-FX, 248-FY, 3, PgHdLRProcedure, "中间" },
      #undef FX
      #undef FY

      #define FX        (X+162)
      #define FY        (Y+79)
  { FRAMEITEM, 3, FX-X, FY-Y, FX-X+110, 258-Y, 0, NULL, "眉线类型" },
      #undef FX
      #define FX        (X+260)
      { SINGLESELECT, 0, 320-FX, 202-FY, 354-FX, 222-FY, 0, PgHdLineProcedure, "无" },
      { SINGLESELECT, 0, 360-FX, 202-FY, 410-FX, 222-FY, 1, PgHdLineProcedure, "细线" },
      { SINGLESELECT, 0, 320-FX, 228-FY, 400-FX, 248-FY, 2, PgHdLineProcedure, "文武线" },
      #undef FX
      #undef FY

  { STATICTEXTITEM, 0, 70-X, 282-Y, 134-X, 300-Y, 0, NULL, "左页篇眉" },
  { STATICTEXTITEM, 0, 70-X, 314-Y, 134-X, 332-Y, 0, NULL, "右页篇眉" },
  { SINGLELINEEDITORITEM, 0, 142-X, 280-Y, 410-X, 300-Y, 0, PageHeadLeftProcedure, "" },
  { SINGLELINEEDITORITEM, 0, 142-X, 312-Y, 410-X, 332-Y, 0, PageHeadRightProcedure, "" },

  { OKBUTTON,     0, 340-X, 145-Y, 410-X, 175-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 340-X, 191-Y, 410-X, 221-Y, 0, NULL, "放弃" },

#undef X
#undef Y
};

Dialogs PageDialog[]=
{
  #define X     88
  #define Y     62
  { GLOBALITEM, 5 , X, Y+18, 540, 385+18, 0, NULL,"页面数据设置" },

  { OKBUTTON,     0, 122-X, 328-Y, 192-X, 358-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 200-X, 328-Y, 270-X, 358-Y, 0, NULL, "放弃" },

  { FRAMEITEM, 11, 111-X, 102-Y, 270-X, 306-Y, 0, NULL, "页面尺寸(mm)" },
      #define FX        120
      #define FY        102
      { SINGLESELECT,         0, 133-FX, 129-FY, 200-FX, 149-FY, 0, PageSizeProcedure, "大8开" },
      { SINGLESELECT,         0, 212-FX, 129-FY, 262-FX, 149-FY, 1, PageSizeProcedure, "8开" },
      { SINGLESELECT,         0, 133-FX, 154-FY, 200-FX, 174-FY, 2, PageSizeProcedure, "大16开" },
      { SINGLESELECT,         0, 212-FX, 154-FY, 262-FX, 174-FY, 3, PageSizeProcedure, "16开" },
      { SINGLESELECT,         0, 133-FX, 179-FY, 200-FX, 199-FY, 4, PageSizeProcedure, "大32开" },
      { SINGLESELECT,         0, 212-FX, 179-FY, 262-FX, 199-FY, 5, PageSizeProcedure, "32开" },
      { SINGLESELECT,         0, 133-FX, 204-FY, 200-FX, 224-FY, 6, PageSizeProcedure, "自定义" },
      { STATICTEXTITEM,       0, 132-FX, 239-FY, 168-FX, 259-FY, 0, NULL, "页宽" },
      { SINGLELINEEDITORITEM, 0, 174-FX, 237-FY, 256-FX, 257-FY, 0, PageWidthProcedure, "" },
      { STATICTEXTITEM,       0, 132-FX, 274-FY, 168-FX, 294-FY, 0, NULL, "页高" },
      { SINGLELINEEDITORITEM, 0, 174-FX, 272-FY, 256-FX, 292-FY, 0, PageHeightProcedure, "" },
      #undef FX
      #undef FY

  { FRAMEITEM, 8, 284-X, 102-Y, 514-X, 210-Y, 0, NULL, "页边空(mm)" },
      #define FX        334
      #define FY        92
      { STATICTEXTITEM,       0, 345-FX, 125-FY, 395-FX, 145-FY, 0, NULL, "上边空" },
      { SINGLELINEEDITORITEM, 0, 400-FX, 125-FY, 440-FX, 145-FY, 0, PageMarginTopProcedure, "" },
      { STATICTEXTITEM,       0, 450-FX, 125-FY, 500-FX, 145-FY, 0, NULL, "下边空" },
      { SINGLELINEEDITORITEM, 0, 505-FX, 125-FY, 545-FX, 145-FY, 0, PageMarginBottomProcedure, "" },
      { STATICTEXTITEM,       0, 345-FX, 160-FY, 395-FX, 180-FY, 0, NULL, "左边空" },
      { SINGLELINEEDITORITEM, 0, 400-FX, 160-FY, 440-FX, 180-FY, 0, PageMarginLeftProcedure, "" },
      { STATICTEXTITEM,       0, 450-FX, 160-FY, 500-FX, 180-FY, 0, NULL, "右边空" },
      { SINGLELINEEDITORITEM, 0, 505-FX, 160-FY, 545-FX, 180-FY, 0, PageMarginRightProcedure, "" },
      #undef FX
      #undef FY

  { FRAMEITEM, 6, 284-X, 224-Y, 514-X, 356-Y, 0, NULL, "页设置(mm)" },
      #define FX        334
      #define FY        244
      { MULTISELECT,          0, 345-FX, 273-FY, 505-FX, 293-FY, 0, PageRotate, "横版" },
      #undef FY
      #define FY        210
      { MULTISELECT,          0, 345-FX, 273-FY, 505-FX, 293-FY, 1, PageInitialBoxProcedure, "自动建立文本版框" },
      { STATICTEXTITEM,       0, 345-FX, 309-FY, 395-FX, 329-FY, 0, NULL, "分栏数" },
      { SINGLELINEEDITORITEM, 0, 400-FX, 309-FY, 440-FX, 329-FY, 0, PageColumnProcedure, "" },
      { STATICTEXTITEM,       0, 455-FX, 309-FY, 505-FX, 329-FY, 0, NULL, "栏间距" },
      { SINGLELINEEDITORITEM, 0, 510-FX, 309-FY, 550-FX, 329-FY, 0, PageColumnDistantProcedure, "" }
      #undef FX
      #undef FY

#undef X
#undef Y
};


Dialogs TableBoxDialog[]=
{
  { GLOBALITEM, 3 , 80, 72+28, 480, 338, 0, NULL,"文字边距设定" },
  #define X     80
  #define Y     44

  { OKBUTTON,     0, 418-X-40, 273-Y-140, 490-X-40, 303-Y-140, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 418-X-40, 273-Y-80, 490-X-40, 303-Y-80, 0, NULL, "放弃" },

  { FRAMEITEM, 8, 98-X, 86-Y, 357-X, 240-Y, 0, NULL, "版框文字边距数据" },
      #define FX        98
      #define FY        80
     { STATICTEXTITEM, 0, 108-FX, 108-FY, 230-FX, 128-FY, 0, NULL, "文字左边空(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 134-FY, 230-FX, 154-FY, 0, NULL, "文字右边空(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 160-FY, 230-FX, 180-FY, 0, NULL, "文字上边空(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 186-FY, 230-FX, 206-FY, 0, NULL, "文字下边空(mm)" },
     { SINGLELINEEDITORITEM, 0, 236-FX, 108-FY, 336-FX, 128-FY, 0, TextDistantLeftProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 236-FX, 134-FY, 336-FX, 154-FY, 0, TextDistantRightProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 236-FX, 160-FY, 336-FX, 180-FY, 0, TextDistantTopProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 236-FX, 186-FY, 336-FX, 206-FY, 0, TextDistantBottomProcedure, "" },
     #undef FX
     #undef FY

#undef X
#undef Y
};



Dialogs TextBoxDialog[]=
{
  { GLOBALITEM, 6 , 80, 72+28, 546, 338+28, 0, KeyHelpProcedure,"文本版框设定" },
  #define X     80
  #define Y     54

  { USERBUTTONITEM, 0, 140-X, 273-Y, 269-X, 303-Y, 10091, NULL, "设置文字边距" },
  { OKBUTTON, 0, 334-X, 273-Y, 406-X, 303-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 418-X, 273-Y, 490-X, 303-Y, 0, NULL, "放弃" },

  { FRAMEITEM, 10, 98-X, 86-Y, 307-X, 271-Y-15, 0, NULL, "版框数据" },
      #define FX        98
      #define FY        80
     { STATICTEXTITEM, 0, 108-FX, 108-FY, 200-FX, 128-FY, 0, NULL, "左顶点X(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 134-FY, 200-FX, 154-FY, 0, NULL, "左顶点Y(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 160-FY, 180-FX, 180-FY, 0, NULL, "宽度(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 186-FY, 180-FX, 206-FY, 0, NULL, "高度(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 212-FY, 194-FX, 232-FY, 0, NULL, "旋转角度" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 108-FY, 286-FX, 128-FY, 0, BoxBoxLeftProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 134-FY, 286-FX, 154-FY, 0, BoxBoxTopProcedure,  "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 160-FY, 286-FX, 180-FY, 0, BoxWidthProcedure,   "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 186-FY, 286-FX, 206-FY, 0, BoxHeightProcedure,  "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 212-FY, 286-FX, 232-FY, 0, BoxRotateAngleProcedure, "" },
     #undef FX
     #undef FY

  { MULTISELECT, 0, 324-X, 235-Y, 474-X, 255-Y, 0, BoxPrintableProcedure, "本版框内容不打印" },

  { FRAMEITEM, 4, 324-X, 86-Y, 530-X, 212-Y, 0, NULL, "分栏设定" },
      #define FX        324
      #define FY        80
     { STATICTEXTITEM, 0, 334-FX, 108-FY, 386-FX, 128-FY, 0, NULL, "分栏数" },
     { STATICTEXTITEM, 0, 334-FX, 134-FY, 418-FX, 154-FY, 0, NULL, "栏间距(mm)" },
     { SINGLELINEEDITORITEM, 0, 424-FX, 108-FY, 488-FX, 128-FY, 0, BoxBoxColumnProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 424-FX, 134-FY, 488-FX, 154-FY, 0, BoxColumnDistantProcedure, "" },
     #undef FX
     #undef FY
#undef X
#undef Y
};

Dialogs TextBoxDialog1[]=
{
  { GLOBALITEM, 7 , 80, 72+28, 546, 338+28, 0, KeyHelpProcedure,"文本版框设定" },
  #define X     80
  #define Y     54

  { USERBUTTONITEM, 0, 140-X, 273-Y, 269-X, 303-Y, 10091, NULL, "设置文字边距" },
  { OKBUTTON, 0, 334-X, 273-Y, 406-X, 303-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 418-X, 273-Y, 490-X, 303-Y, 0, NULL, "放弃" },

  { FRAMEITEM, 10, 98-X, 86-Y, 307-X, 271-Y-15, 0, NULL, "版框数据" },
      #define FX        98
      #define FY        80
     { STATICTEXTITEM, 0, 108-FX, 108-FY, 200-FX, 128-FY, 0, NULL, "左顶点X(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 134-FY, 200-FX, 154-FY, 0, NULL, "左顶点Y(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 160-FY, 180-FX, 180-FY, 0, NULL, "宽度(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 186-FY, 180-FX, 206-FY, 0, NULL, "高度(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 212-FY, 194-FX, 232-FY, 0, NULL, "旋转角度" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 108-FY, 286-FX, 128-FY, 0, BoxBoxLeftProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 134-FY, 286-FX, 154-FY, 0, BoxBoxTopProcedure,  "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 160-FY, 286-FX, 180-FY, 0, BoxWidthProcedure,   "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 186-FY, 286-FX, 206-FY, 0, BoxHeightProcedure,  "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 212-FY, 286-FX, 232-FY, 0, BoxRotateAngleProcedure, "" },
     #undef FX
     #undef FY

  { MULTISELECT, 0, 324-X, 223-Y, 474-X, 243-Y, 0, BoxPrintableProcedure, "本版框内容不打印" },
  { MULTISELECT, 0, 324-X, 245-Y, 474-X, 265-Y, 0, BoxEditablePro, "本版框内容不编辑" },

  { FRAMEITEM, 4, 324-X, 86-Y, 530-X, 212-Y, 0, NULL, "分栏设定" },
      #define FX        324
      #define FY        80
     { STATICTEXTITEM, 0, 334-FX, 108-FY, 386-FX, 128-FY, 0, NULL, "分栏数" },
     { STATICTEXTITEM, 0, 334-FX, 134-FY, 418-FX, 154-FY, 0, NULL, "栏间距(mm)" },
     { SINGLELINEEDITORITEM, 0, 424-FX, 108-FY, 488-FX, 128-FY, 0, BoxBoxColumnProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 424-FX, 134-FY, 488-FX, 154-FY, 0, BoxColumnDistantProcedure, "" },
     #undef FX
     #undef FY
#undef X
#undef Y
};

Dialogs PictureBoxDialog[]=
{
  { GLOBALITEM, 5 , 80, 72+8, 546, 396+8, 0, NULL,"图像版框设定" },
  #define X     80
  #define Y     54

  { OKBUTTON, 0, 324-X, 330-Y, 396-X, 360-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 414-X, 330-Y, 486-X, 360-Y, 0, NULL, "放弃" },

  { FRAMEITEM, 10, 98-X, 86-Y, 307-X, 271-Y, 0, NULL, "版框数据" },
      #define FX        98
      #define FY        80
     { STATICTEXTITEM, 0, 108-FX, 108-FY, 200-FX, 128-FY, 0, NULL, "左顶点X(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 134-FY, 200-FX, 154-FY, 0, NULL, "左顶点Y(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 160-FY, 180-FX, 180-FY, 0, NULL, "宽度(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 186-FY, 180-FX, 206-FY, 0, NULL, "高度(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 212-FY, 194-FX, 232-FY, 0, NULL, "旋转角度" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 108-FY, 286-FX, 128-FY, 0, BoxBoxLeftProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 134-FY, 286-FX, 154-FY, 0, BoxBoxTopProcedure,  "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 160-FY, 286-FX, 180-FY, 0, BoxWidthProcedure,  "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 186-FY, 286-FX, 206-FY, 0, BoxHeightProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 212-FY, 286-FX, 232-FY, 0, BoxRotateAngleProcedure, "" },
     #undef FX
     #undef FY

  { MULTISELECT, 0, 108-X, 304-Y, 258-X, 304-Y, 0, BoxPrintableProcedure, "本版框内容不打印" },

  { FRAMEITEM, 12, 324-X, 86-Y, 530-X, 312-Y, 0, NULL, "图像数据" },
      #define FX        324
      #define FY        80
     { STATICTEXTITEM, 0, 334-FX, 108-FY, 414-FX, 128-FY, 0, NULL, "原点X(mm)" },
     { STATICTEXTITEM, 0, 334-FX, 134-FY, 414-FX, 154-FY, 0, NULL, "原点Y(mm)" },
     { STATICTEXTITEM, 0, 334-FX, 160-FY, 404-FX, 180-FY, 0, NULL, "横向变倍" },
     { STATICTEXTITEM, 0, 334-FX, 186-FY, 404-FX, 206-FY, 0, NULL, "纵向变倍" },
     { STATICTEXTITEM, 0, 334-FX, 212-FY, 404-FX, 232-FY, 0, NULL, "倾斜角度" },
     { STATICTEXTITEM, 0, 334-FX, 238-FY, 404-FX, 258-FY, 0, NULL, "旋转角度" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 108-FY, 484-FX, 128-FY, 0, PictureBoxPictureOringleXProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 134-FY, 484-FX, 154-FY, 0, PictureBoxPictureOringleYProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 160-FY, 484-FX, 180-FY, 0, PictureBoxPictureImageScaleXProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 186-FY, 484-FX, 206-FY, 0, PictureBoxPictureImageScaleYProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 212-FY, 484-FX, 232-FY, 0, PictureBoxPictureSkewAngleProcedure,  "" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 238-FY, 484-FX, 258-FY, 0, PictureBoxPictureRotateAngleProcedure,"" },
     #undef FX
     #undef FY
#undef X
#undef Y
};

Dialogs PictureBoxDialog1[]=
{
  { GLOBALITEM, 6 , 80, 72+8, 546, 396+8, 0, NULL,"图像版框设定" },
  #define X     80
  #define Y     54

  { OKBUTTON, 0, 324-X, 330-Y, 396-X, 360-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 414-X, 330-Y, 486-X, 360-Y, 0, NULL, "放弃" },

  { FRAMEITEM, 10, 98-X, 86-Y, 307-X, 271-Y, 0, NULL, "版框数据" },
      #define FX        98
      #define FY        80
     { STATICTEXTITEM, 0, 108-FX, 108-FY, 200-FX, 128-FY, 0, NULL, "左顶点X(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 134-FY, 200-FX, 154-FY, 0, NULL, "左顶点Y(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 160-FY, 180-FX, 180-FY, 0, NULL, "宽度(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 186-FY, 180-FX, 206-FY, 0, NULL, "高度(mm)" },
     { STATICTEXTITEM, 0, 108-FX, 212-FY, 194-FX, 232-FY, 0, NULL, "旋转角度" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 108-FY, 286-FX, 128-FY, 0, BoxBoxLeftProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 134-FY, 286-FX, 154-FY, 0, BoxBoxTopProcedure,  "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 160-FY, 286-FX, 180-FY, 0, BoxWidthProcedure,  "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 186-FY, 286-FX, 206-FY, 0, BoxHeightProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 206-FX, 212-FY, 286-FX, 232-FY, 0, BoxRotateAngleProcedure, "" },
     #undef FX
     #undef FY

  { MULTISELECT, 0, 108-X, 304-Y, 258-X, 304-Y, 0, BoxPrintableProcedure, "本版框内容不打印" },
  { MULTISELECT, 0, 108-X, 327-Y, 258-X, 327-Y, 0, BoxEditablePro, "本版框内容不编辑" },

  { FRAMEITEM, 12, 324-X, 86-Y, 530-X, 312-Y, 0, NULL, "图像数据" },
      #define FX        324
      #define FY        80
     { STATICTEXTITEM, 0, 334-FX, 108-FY, 414-FX, 128-FY, 0, NULL, "原点X(mm)" },
     { STATICTEXTITEM, 0, 334-FX, 134-FY, 414-FX, 154-FY, 0, NULL, "原点Y(mm)" },
     { STATICTEXTITEM, 0, 334-FX, 160-FY, 404-FX, 180-FY, 0, NULL, "横向变倍" },
     { STATICTEXTITEM, 0, 334-FX, 186-FY, 404-FX, 206-FY, 0, NULL, "纵向变倍" },
     { STATICTEXTITEM, 0, 334-FX, 212-FY, 404-FX, 232-FY, 0, NULL, "倾斜角度" },
     { STATICTEXTITEM, 0, 334-FX, 238-FY, 404-FX, 258-FY, 0, NULL, "旋转角度" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 108-FY, 484-FX, 128-FY, 0, PictureBoxPictureOringleXProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 134-FY, 484-FX, 154-FY, 0, PictureBoxPictureOringleYProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 160-FY, 484-FX, 180-FY, 0, PictureBoxPictureImageScaleXProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 186-FY, 484-FX, 206-FY, 0, PictureBoxPictureImageScaleYProcedure, "" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 212-FY, 484-FX, 232-FY, 0, PictureBoxPictureSkewAngleProcedure,  "" },
     { SINGLELINEEDITORITEM, 0, 420-FX, 238-FY, 484-FX, 258-FY, 0, PictureBoxPictureRotateAngleProcedure,"" },
     #undef FX
     #undef FY
#undef X
#undef Y
};

#ifdef  UNUSED          // ByHance, 96,1.30
Dialogs BoxAlignDialog[]=
{
  #define X 122
  #define Y 107
  { GLOBALITEM, 4 , 122, 107, 512, 357, 0, NULL,"版框对齐" },
  { OKBUTTON, 0, 338-X, 312-Y, 406-X, 342-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 424-X, 312-Y, 492-X, 342-Y, 0, NULL, "放弃" },
  { FRAMEITEM, 3, 173-X, 152-Y, 456-X, 217-Y, 0, NULL, "水平对齐" },
      #define FX        173
      #define FY        152
     { SINGLESELECT, 0, 188-FX, 178-FY, 268-FX, 198-FY, 0, NULL, "" },
     { SINGLESELECT, 0, 272-FX, 178-FY, 352-FX, 198-FY, 1, NULL, "" },
     { SINGLESELECT, 0, 356-FX, 178-FY, 436-FX, 198-FY, 2, NULL, "" },
     #undef FX
     #undef FY

  { FRAMEITEM, 3, 173-X, 230-Y, 456-X, 295-Y, 0, NULL, "垂直对齐" },
      #define FX        173
      #define FY        230
     { SINGLESELECT, 0, 188-FX, 254-FY, 268-FX, 274-FY, 0, NULL, "" },
     { SINGLESELECT, 0, 272-FX, 254-FY, 352-FX, 274-FY, 1, NULL, "" },
     { SINGLESELECT, 0, 356-FX, 254-FY, 436-FX, 274-FY, 2, NULL, "" },
     #undef FX
     #undef FY

  #undef X
  #undef Y
};

Dialogs CharSlantDialog[]=
{
  { GLOBALITEM, 4 , 240, 170, 400, 310, 0, NULL,"字符倾斜设置" },

  { OKBUTTON,     0, 15, 95,  65, 125, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 95, 95, 145, 125, 0, NULL, "放弃" },

  { STATICTEXTITEM, 0, 10, 44, 140, 64, 0, NULL, "字倾斜角度" },
  { SINGLELINEEDITORITEM, 0, 10, 65, 150, 85, 0, CharSlantProcedure, "" }
};

Dialogs CharColorDialog[]=
{
  { GLOBALITEM, 6 , 240, 170, 400, 350, 0, NULL,"颜色设置" },

  { OKBUTTON,     0, 15, 141, 65, 171, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 95, 141, 145, 171, 0, NULL, "取消" },

  { STATICTEXTITEM, 0, 10, 44, 69, 64, 0, NULL, "颜色" },
  { SINGLELINEEDITORITEM, 0, 10, 65, 150, 85, 0, CharColorProcedure, "" },

  { STATICTEXTITEM, 0, 10, 90, 69, 110, 0, NULL, "阴影" },
  { SINGLELINEEDITORITEM, 0, 10, 111, 150, 131, 0, CharColorProcedure, "" }
};
#endif     //  UNUSED          // ByHance, 96,1.30

Dialogs TableLineStyleDialog[]=
{
  #define X     163
  #define Y     144
  { GLOBALITEM, 7 , X, Y, 476, 348, 0, NULL,"表格线型" },

  { STATICTEXTITEM, 0, 185-X, 215-Y, 205-X, 235-Y, 0, NULL, "第" },
  { SINGLELINEEDITORITEM, 0, 211-X, 213-Y, 259-X, 230-Y, 0, TableLineStyleProcedure, ""},

  { SINGLESELECT, 0, 270-X, 200-Y, 320-X, 220-Y, 0, TableLineColOptionProcedure, "行" },
  { SINGLESELECT, 0, 270-X, 226-Y, 320-X, 246-Y, 1, TableLineColOptionProcedure, "列" },

  { FRAMEITEM, 4, 180-X, 264-Y, 464-X, 322-Y, 0, NULL, "线型" },
      #define FX        176
      #define FY        266
    { SINGLESELECT, 0, 185-FX, 292-FY, 270-FX, 312-FY, 0, TableStyleOptionProcedure, "普通细线" },
    { SINGLESELECT, 0, 273-FX, 292-FY, 326-FX, 312-FY, 1, TableStyleOptionProcedure, "粗线" },
    { SINGLESELECT, 0, 329-FX, 292-FY, 398-FX, 312-FY, 2, TableStyleOptionProcedure, "双细线" },
    { SINGLESELECT, 0, 401-FX, 292-FY, 454-FX, 312-FY, 3, TableStyleOptionProcedure, "空线" },
      #undef FY
      #undef FX

  { OKBUTTON, 0, 344-X, 190-Y, 416-X, 218-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 344-X, 223-Y, 416-X, 251-Y, 0, NULL, "放弃" },
  #undef Y
  #undef X
};

Dialogs TableSLantStyleDialog[]=
{
  #define X     164
  #define Y     140
  { GLOBALITEM, 6, X, Y, X+270, Y+180, 0, NULL,"表元斜线线型" },

  { STATICTEXTITEM, 0, 185-X, 190-Y, 185-X+160, 190-Y+20, 0, NULL, "当前表元斜线的线型:" },
  { SINGLESELECT, 0, 185-X, 224-Y, 224-X, 244-Y, 0, TableStyleOptionProcedure, "无" },
  { SINGLESELECT, 0, 228-X, 224-Y, 314-X, 244-Y, 1, TableStyleOptionProcedure, "对分斜线" },
  { SINGLESELECT, 0, 318-X, 224-Y, 418-X, 244-Y, 2, TableStyleOptionProcedure, "三分斜线" },

  { OKBUTTON, 0, 258-X, 275-Y, 330-X, 305-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 336-X, 275-Y, 408-X, 305-Y, 0, NULL, "放弃" },
  #undef Y
  #undef X
};

Dialogs TableLineColumnDialog[]=
{
  #define X 140
  #define Y 166
  { GLOBALITEM, 7 , 140,166,430,320, 0, NULL,"创建表格" },

  { STATICTEXTITEM, 0, 160-X,214-Y,200-X,234-Y, 0, NULL, "行数" },
  { STATICTEXTITEM, 0, 160-X,247-Y,200-X,267-Y, 0, NULL, "列数" },

  { SINGLELINEEDITORITEM, 0, 203-X,212-Y,205-X+6*16,232-Y, 0, TableLineProcedure, "" },
  { SINGLELINEEDITORITEM, 0, 203-X,245-Y,205-X+6*16,265-Y, 0, TableColumnProcedure, "" },

  { MULTISELECT, 0, 168-X, 282-Y, 168-X+8*16, 302-Y, 0, TableHeadOptionProcedure, "表格位置通栏" },

  { OKBUTTON,     0, 325-X,225-Y,410-X,253-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 325-X,272-Y,410-X,300-Y, 0, NULL, "取消" },
  #undef X
  #undef Y
};

Dialogs TableInsLineDialog[]=
{
  { GLOBALITEM, 8 , 0, 0, 353, 196, 0, NULL,"插入一行" },
  { STATICTEXTITEM, 0, 42, 58, 72, 77, 0, NULL, "在第" },
  { STATICTEXTITEM, 0, 180, 58, 213, 77, 0, NULL, "行的" },
  { STATICTEXTITEM, 0, 41, 110, 180, 129, 0, NULL, "插入一个新表格行" },
  { SINGLELINEEDITORITEM, 0, 83, 58, 170, 77,0,TableInsProcedure, "" },
  { SINGLESELECT, 0, 225, 45, 325, 64, 0, TableInsOptionProcedure, "上边" },
  { SINGLESELECT, 0, 225, 76, 325, 95, 1, TableInsOptionProcedure, "下边" },
  { OKBUTTON, 0, 125, 152, 224, 179, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 237, 152, 336, 179, 0, NULL, "放弃" },
};

Dialogs TableInsColDialog[]=
{
  { GLOBALITEM, 8 , 0, 0, 353, 196, 0, NULL,"插入一列" },
  { STATICTEXTITEM, 0, 42, 58, 72, 77, 0, NULL, "在第" },
  { STATICTEXTITEM, 0, 180, 58, 213, 77, 0, NULL, "列的" },
  { STATICTEXTITEM, 0, 41, 110, 180, 129, 0, NULL, "插入一个新表格列" },
  { SINGLELINEEDITORITEM, 0, 83, 58, 170, 77,0,TableInsProcedure, "" },
  { SINGLESELECT, 0, 225, 45, 325, 64, 0, TableInsOptionProcedure, "左边" },
  { SINGLESELECT, 0, 225, 76, 325, 95, 1, TableInsOptionProcedure, "右边" },
  { OKBUTTON, 0, 125, 152, 224, 179, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 237, 152, 336, 179, 0, NULL, "放弃" },
};


/*----------------
Dialogs TableDelLineColDialog[]=
{
  #define X     163
  #define Y     144
  { GLOBALITEM, 6 , X, Y, 436, 290, 0, NULL,"删除行列" },

  { STATICTEXTITEM, 0, 185-X, 192-Y, 235-X, 212-Y, 0, NULL, "删除第" },
  { SINGLELINEEDITORITEM, 0, 237-X, 190-Y, 285-X, 210-Y, 0, TableLineProcedure, ""},

  { SINGLESELECT, 0, 294-X, 177-Y, 344-X, 197-Y, 0, TableLineColOptionProcedure, "行" },
  { SINGLESELECT, 0, 294-X, 203-Y, 344-X, 223-Y, 1, TableLineColOptionProcedure, "列" },

  { OKBUTTON, 0, 258-X, 245-Y, 330-X, 273-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 336-X, 245-Y, 408-X, 273-Y, 0, NULL, "放弃" },
  #undef X
  #undef Y
};

Dialogs TableDelLineDialog[]=
{
  { GLOBALITEM, 5, 240, 170, 400, 350, 0, NULL,"删除一行" },

  { STATICTEXTITEM, 0, 10, 44, 69, 80, 0, NULL, "删除第" },
  { SINGLELINEEDITORITEM, 0, 10, 65, 150, 85, 0, TableLineProcedure, "" },
  { STATICTEXTITEM,       0, 10, 95, 69, 120, 0, NULL, "行" },
  { OKBUTTON, 0, 15, 141, 65, 171, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 95, 141, 145, 171, 0, NULL, "取消" },
};

Dialogs TableDelColDialog[]=
{
  { GLOBALITEM, 5, 240, 170, 400, 350, 0, NULL,"删除一列" },

  { STATICTEXTITEM, 0, 10, 44, 69, 80, 0, NULL, "删除第" },
  { SINGLELINEEDITORITEM, 0, 10, 65, 150, 85, 0, TableLineProcedure, "" },
  { STATICTEXTITEM,       0, 10, 95, 69, 120, 0, NULL, "列" },
  { OKBUTTON, 0, 15, 141, 65, 171, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 95, 141, 145, 171, 0, NULL, "取消" },
};

Dialogs CharFontDialog[]=
{
  #define X 170
  #define Y 122
  { GLOBALITEM, 6 , 170, 122, 474, 300, 0, NULL,"选择字体" },

  { STATICTEXTITEM, 0, 216-X, 164-Y, 292-X, 184-Y, 0, NULL, "中文字体:" },
  { STATICTEXTITEM, 0, 216-X, 204-Y, 292-X, 224-Y, 0, NULL, "西文字体:" },
  { SINGLELINEEDITORITEM, 0, 308-X, 164-Y, 372-X, 184-Y, 0, CharCFontProcedure, "" },
  { SINGLELINEEDITORITEM, 0, 308-X, 204-Y, 372-X, 224-Y, 0, CharEFontProcedure, "" },

  { OKBUTTON, 0, 310-X, 256-Y, 376-X, 288-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 388-X, 256-Y, 454-X, 288-Y, 0, NULL, "放弃" },
  #undef X
  #undef Y
};



Dialogs CharSizeDialog[]=
{
  #define X 170
  #define Y 122
  { GLOBALITEM, 6 , 170, 122, 474, 300, 0, NULL,"自定义字号" },

  { STATICTEXTITEM, 0, 216-X, 164-Y, 292-X, 184-Y, 0, NULL, "字宽(mm):" },
  { STATICTEXTITEM, 0, 216-X, 204-Y, 292-X, 224-Y, 0, NULL, "字高(mm):" },
  { SINGLELINEEDITORITEM, 0, 308-X, 164-Y, 372-X, 184-Y, 0, CharHSizeProcedure, "" },
  { SINGLELINEEDITORITEM, 0, 308-X, 204-Y, 372-X, 224-Y, 0, CharSizeProcedure,  "" },

  { OKBUTTON, 0, 310-X, 256-Y, 376-X, 286-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 388-X, 256-Y, 454-X, 286-Y, 0, NULL, "放弃" },
  #undef X
  #undef Y
};
--------*/


Dialogs CharFontDialog[]=
{
  { GLOBALITEM, 4 , 0, 0, 515, 294, 0, NULL,"选择字体" },

  { FRAMEITEM, 6, 31, 44, 242, 235, 0, NULL, "中文字体" },
      #define X 31
      #define Y 44
      { SINGLELINEEDITORITEM, 0, 60-X, 190-Y, 200-X, 209-Y, 0, CharCFontProcedure, "" },
      { SINGLESELECT, 0, 60-X,  69-Y, 200-X,  88-Y, 0, CFontOptionProcedure, "宋体简体" },
      { SINGLESELECT, 0, 60-X,  89-Y, 200-X, 108-Y, 1, CFontOptionProcedure, "楷体简体" },
      { SINGLESELECT, 0, 60-X, 109-Y, 200-X, 128-Y, 2, CFontOptionProcedure, "黑体简体" },
      { SINGLESELECT, 0, 60-X, 129-Y, 200-X, 148-Y, 3, CFontOptionProcedure, "仿宋简体" },
      { STATICTEXTITEM,0,60-X, 165-Y, 200-X, 184-Y, 0, NULL, "中文字体编号:" },

  { FRAMEITEM, 6, 269, 44, 480, 235, 0, NULL, "西文字体" },
      { SINGLELINEEDITORITEM, 0, 60-X, 190-Y, 200-X, 209-Y, 0, CharEFontProcedure, "" },
      { SINGLESELECT, 0, 60-X,  69-Y, 200-X,  88-Y, 0, EFontOptionProcedure, "白正体" },
      { SINGLESELECT, 0, 60-X,  89-Y, 200-X, 108-Y, 1, EFontOptionProcedure, "白斜体"},
      { SINGLESELECT, 0, 60-X, 109-Y, 200-X, 128-Y, 2, EFontOptionProcedure, "黑正体" },
      { SINGLESELECT, 0, 60-X, 129-Y, 200-X, 148-Y, 3, EFontOptionProcedure, "黑斜体" },
      { STATICTEXTITEM,0,60-X, 165-Y, 200-X, 184-Y, 0, NULL, "西文字体编号:" },
      #undef X
      #undef Y

  { OKBUTTON, 0, 201, 252, 300, 279, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 333, 252, 432, 279, 0, NULL, "放弃" },
};

Dialogs RowGapDialog[]=
{
  { GLOBALITEM, 4 , 0, 0, 433, 240, 0, NULL,"自定义行距" },

  { STATICTEXTITEM, 0, 49+80,  42+32, 234+80,  61+32, 0, NULL, "行距:(以行的高度为单位)" },

  { SINGLELINEEDITORITEM, 0, 49+80,  70+30, 234+80,  89+30, 0, RowGapProcedure, "" },

  { OKBUTTON, 0, 295-200, 184, 396-200, 211, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 295-50, 184, 396-50, 211, 0, NULL, "放弃" },
};

Dialogs ColGapDialog[]=
{
  { GLOBALITEM, 4 , 0, 0, 433, 240, 0, NULL,"自定义字距" },

  { STATICTEXTITEM, 0, 49+80,  42+32, 234+80,  61+32, 0, NULL, "字距:(以字的宽度为单位)" },

  { SINGLELINEEDITORITEM, 0, 49+80,  70+30, 234+80,  89+30, 0, RowGapProcedure, "" },

  { OKBUTTON, 0, 295-200, 184, 396-200, 211, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 295-50, 184, 396-50, 211, 0, NULL, "放弃" },
};

Dialogs UpDownDialog[]=
{
  { GLOBALITEM, 4 , 0, 0, 433, 240, 0, NULL,"字符升降" },

  { STATICTEXTITEM, 0, 49+80,  42+32, 234+80,  61+32, 0, NULL, "升降:(以字的高度为单位)" },

  { SINGLELINEEDITORITEM, 0, 49+80,  70+30, 234+80,  89+30, 0, UpDownProcedure, "" },

  { OKBUTTON, 0, 295-200, 184, 396-200, 211, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 295-50, 184, 396-50, 211, 0, NULL, "放弃" },
};

Dialogs CharSizeDialog[]=
{
//  { GLOBALITEM, 9 , 0, 0, 433, 240, 0, NULL,"自定义字号" },
  { GLOBALITEM, 7 , 0, 0, 433, 240, 0, NULL,"自定义字号" },

  { STATICTEXTITEM, 0, 49,  42, 234,  61, 0, NULL, "字高(以毫米为单位)" },
  { STATICTEXTITEM, 0, 49, 106, 234, 125, 0, NULL, "字宽(以毫米为单位)" },

  { SINGLELINEEDITORITEM, 0, 49,  70, 234,  89, 0, CharSizeProcedure, "" },
  { SINGLELINEEDITORITEM, 0, 49, 130, 234, 149, 0, CharHSizeProcedure, "" },
  { MULTISELECT, 0, 49, 185, 234, 204, 0, CharSizeOptionProcedure, "字宽与字高一致" },
//  { SINGLESELECT, 0, 295, 65, 411, 74, 0, CharSizeUnitProcedure, "以毫米为单位" },
//  { SINGLESELECT, 0, 295, 95, 411, 74, 1, CharSizeUnitProcedure, "以英磅为单位" },

  { OKBUTTON, 0, 295, 140, 396, 167, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 295, 184, 396, 211, 0, NULL, "放弃" },
};

Dialogs GotoLineDialog[]=
{
  { GLOBALITEM, 5 , 0, 0, 349, 184, 0, NULL,"定位行" },
  { STATICTEXTITEM, 0, 53, 72, 125,91, 0, NULL, "定位到第" },
  { STATICTEXTITEM, 0, 223, 72, 240,91, 0, NULL, "行" },
  { SINGLELINEEDITORITEM, 0, 127, 72, 210, 91, 0, GotoLineProcedure, "" },
  { OKBUTTON, 0, 107, 130, 206, 157, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 227, 130, 326, 157, 0, NULL, "放弃" },
};

Dialogs GotoPageDialog[]=
{
  { GLOBALITEM, 5 , 0, 0, 349, 184, 0, NULL,"定位页" },
  { STATICTEXTITEM, 0, 53, 72, 125,91, 0, NULL, "定位到第" },
  { STATICTEXTITEM, 0, 223, 72, 240,91, 0, NULL, "页" },
  { SINGLELINEEDITORITEM, 0, 127, 72, 210, 91, 0, GotoPageProcedure, "" },
  { OKBUTTON, 0, 107, 130, 206, 157, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 227, 130, 326, 157, 0, NULL, "放弃" },
};


Dialogs MovePageDialog[]=
{
  { GLOBALITEM, 7 , 0, 0, 417, 224, 0, NULL,"移动页面" },

  { STATICTEXTITEM, 0, 33, 92, 148, 111, 0, NULL, "将当前页移到第"},
  { STATICTEXTITEM, 0, 253, 92, 269, 111, 0, NULL, "页"},
  { SINGLELINEEDITORITEM, 0, 157, 92, 240, 111,0,PageMoveProcedure, "" },
  { SINGLESELECT, 0, 283, 75, 400, 94, 0, PageMoveOptionProcedure, "之前" },
  { SINGLESELECT, 0, 283, 109, 400, 128, 1, PageMoveOptionProcedure, "之后" },
  { OKBUTTON,     0, 165, 170, 264, 197, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 285, 170, 384, 197, 0, NULL, "放弃" },
};


Dialogs ExportDialog[]=
{
  #define X 95
  #define Y 99
  { GLOBALITEM, 8 , 95, 99, 440, 349, 0, NULL,"保存文本" },

  { OKBUTTON,     0, 250-X, 300-Y, 330-X, 328-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 336-X, 300-Y, 416-X, 328-Y, 0, NULL, "放弃" },

  { SINGLESELECT, 0, 163-X, 150-Y, 380-X, 170-Y, 0, ExportOptionProcedure, "当前文本框中的正文" },
  { SINGLESELECT, 0, 163-X, 180-Y, 380-X, 200-Y, 1, ExportOptionProcedure, "所有文本框中的正文" },

  { STATICTEXTITEM, 0, 119-X, 224-Y, 119-X+18*8, 244-Y, 0, NULL, "若保存当前文本框中" },
  { STATICTEXTITEM, 0, 119-X+9*16, 224-Y, 119-X+18*8+19*8, 244-Y, 0, NULL, "的正文,则与其链接的" },

  { STATICTEXTITEM, 0, 119-X, 250-Y, 119-X+18*8, 270-Y, 0, NULL, "所有文本框中的正文" },
  { STATICTEXTITEM, 0, 119-X+9*16, 250-Y, 119-X+18*8+18*8, 270-Y, 0, NULL, "都将自动保存在一起" },
  #undef X
  #undef Y
};


Dialogs FindDialog[]=
{
  { GLOBALITEM, 5 , 135, 120, 500, 340, 0, NULL,"查找" },

  { STATICTEXTITEM, 0, 12, 120, 94, 140, 0, NULL, "查找字串:" },
  { SINGLELINEEDITORITEM, 0, 95, 120, 340, 140, 0, FindStringProcedure, "" },

  { FRAMEITEM,   3,  10, 45, 350, 105, 0, NULL, "查找方式" },
     { MULTISELECT, 0,  20, 25, 126, 50, 0, FindOptionProcedure, "从头开始" },
     { MULTISELECT, 0, 128, 25, 234, 50, 1, FindOptionProcedure, "选定范围" },
     { MULTISELECT, 0, 236, 25, 330, 50, 2, FindOptionProcedure, "区别大小写"},

  { OKBUTTON,     0, 140, 180, 240, 210, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 250, 180, 350, 210, 0, NULL, "放弃" },
};

Dialogs ReplaceDialog[]=
{
  { GLOBALITEM, 7 , 135, 120, 500, 360, 0, NULL,"查找并替换" },

  { STATICTEXTITEM, 0, 12, 140, 94, 160, 0, NULL, "查找字串:" },
  { SINGLELINEEDITORITEM, 0, 95, 140, 340, 160, 0, FindStringProcedure, "" },

  { STATICTEXTITEM, 0, 12, 165, 94, 185, 0, NULL, "替换成为:" },
  { SINGLELINEEDITORITEM, 0, 95, 165, 340, 185, 0, ReplaceStringProcedure, "" },

  { FRAMEITEM,   5,  10, 45, 350, 130, 0, NULL, "替换方式" },
  { MULTISELECT, 0,  20, 25, 126, 50, 0, FindOptionProcedure, "从头开始" },
  { MULTISELECT, 0, 128, 25, 234, 50, 1, FindOptionProcedure, "选定范围" },
  { MULTISELECT, 0, 236, 25, 330, 50, 2, FindOptionProcedure, "区别大小写"},
  { MULTISELECT, 0,  20, 50, 126, 75, 3, FindOptionProcedure,  "全部替换" },
  { MULTISELECT, 0, 128, 50, 234, 75, 4, FindOptionProcedure, "替换不确认" },

  { OKBUTTON,     0, 140, 200, 240, 230, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 250, 200, 350, 230, 0, NULL, "放弃" },
};


Dialogs KeyHelpDialog[]=
{
  #define X 110
  #define Y 133
  { GLOBALITEM, 6 , 106, 133, 490, 352, 0, KeyHelpProcedure,"轻松帮助" },

  { USERBUTTONITEM, 0, 162-X, 190-Y, 290-X, 218-Y, 10091, NULL, "工具条使用" },
  { USERBUTTONITEM, 0, 322-X, 190-Y, 452-X, 218-Y, 10092, NULL, "常用功能键" },
  { USERBUTTONITEM, 0, 162-X, 238-Y, 290-X, 266-Y, 10093, NULL, "编缉功能操作" },
  { USERBUTTONITEM, 0, 322-X, 238-Y, 452-X, 266-Y, 10094, NULL, "鼠标快捷功能" },
  { USERBUTTONITEM, 0, 162-X, 286-Y, 290-X, 314-Y, 10095, NULL, "轻松排版小组" },
  { OKBUTTON,       0, 374-X, 304-Y, 430-X, 334-Y, 0,     NULL, "确定" },

  #undef X
  #undef Y
};

Dialogs ExportPictDialog[]=
{
  #define X 87
  #define Y 99
  { GLOBALITEM, 7 , X, Y, 477, 323, 0, NULL,"当前页输出为图像" },

  { OKBUTTON,     0, 261-X, 260-Y, 340-X, 289-Y, 0, NULL, "输出" },
  { CANCELBUTTON, 0, 365-X, 260-Y, 444-X, 289-Y, 0, NULL, "放弃" },

  { FRAMEITEM, 3, 116-X, 150-Y, 240-X, 293-Y, 0, NULL, "图像格式" },
      #define FX        116
      #define FY        150
     { SINGLESELECT, 0, 132-FX, 185-FY, 200-FX, 205-FY, 0, PrintRangeProcedure, "TIFF" },
     { SINGLESELECT, 0, 132-FX, 222-FY, 200-FX, 242-FY, 1, PrintRangeProcedure, "PCX" },
     { SINGLESELECT, 0, 132-FX, 259-FY, 200-FX, 279-FY, 2, PrintRangeProcedure, "DCL" },
      #undef FX
      #undef FY

  { STATICTEXTITEM,       0, 248-X, 160-Y, 354-X, 180-Y, 0, NULL, "图像分辨率DPI" },
  { SINGLELINEEDITORITEM, 0, 360-X, 160-Y, 440-X, 180-Y, 0, PicDPIProcedure, "" },

  { MULTISELECT, 0, 251-X, 209-Y, 338-X, 230-Y, 1, PrintRorate90, "旋转输出" },
  { MULTISELECT, 0, 347-X, 209-Y, 428-X, 230-Y, 1, PrintReverse,  "镜像输出" },
  #undef X
  #undef Y
};

Dialogs PrintDialog[]=
{
  #define X 120
  #define Y 50
  { GLOBALITEM, 14 , X, Y, 516, 342, 0, NULL,"打印文件" },

  { OKBUTTON, 0, 358-X, 294-Y, 424-X, 324-Y, 0, NULL, "打印" },
  { CANCELBUTTON, 0, 434-X, 294-Y, 500-X, 324-Y, 0, NULL, "放弃" },

  { FRAMEITEM, 4, 148-X, 284-Y-27, 350-X, 324-Y, 0, NULL, "" },
      #define FX        148
      #define FY        284
    { MULTISELECT, 0, 164-FX, 294-FY+5, 248-FX, 324-FY+5, 1, PrintPause, "页间暂停" },
    { MULTISELECT, 0, 164-FX+95, 294-FY+5, 248-FX+95, 324-FY+5, 1, PrintJob, "作业方式" },
    { MULTISELECT, 0, 164-FX, 294-FY+5+26, 248-FX, 324-FY+5+26, 1, PrintHollow, "大字空心" },
    { MULTISELECT, 0, 164-FX+95, 294-FY+5+26, 248-FX+95, 324-FY+5+26, 1, Print2FileOptionProc, "文件输出"  },
      #undef FX
      #undef FY

  { STATICTEXTITEM, 0, 148-X, 81-Y, 263-X, 94-Y, 0, NULL, "当前打印机:" },
  { SINGLELINEEDITORITEM, 0, 146-X, 103-Y, 486-X, 123-Y, 0, DefaultPrinterProcedure, "" },

  { FRAMEITEM, 7, 148-X, 146-Y, 350-X, 283-Y-27, 0, NULL, "打印范围" },
      #define FX        148
      #define FY        146
     { SINGLESELECT, 0, 164-FX, 165-FY, 214-FX, 185-FY, 0, PrintRangeProcedure, "本页" },
     { SINGLESELECT, 0, 224-FX, 165-FY, 274-FX, 185-FY, 1, PrintRangeProcedure, "全部" },
     { SINGLESELECT, 0, 284-FX, 165-FY, 334-FX, 185-FY, 2, PrintRangeProcedure, "部分" },
     //{ SINGLESELECT, 0, 164-FX, 192-FY, 244-FX, 212-FY, 2, PrintRangeProcedure, "部分" },
     { STATICTEXTITEM, 0, 164-FX, 192-FY+2, 224-FX, 212-FY+2, 0, NULL, "起始页:" },
     { STATICTEXTITEM, 0, 164-FX, 222-FY, 224-FX, 242-FY, 0, NULL, "总页数:" },
     { SINGLELINEEDITORITEM, 0, 247-FX, 192-FY+2, 295-FX, 212-FY+2, 0, PrintStartPageProc, ""},
     { SINGLELINEEDITORITEM, 0, 247-FX, 222-FY, 295-FX, 242-FY, 0, PrintTotalPageProc, ""},
     #undef FX
     #undef FY

  { STATICTEXTITEM, 0, 377-X, 155-Y-8, 450-X, 175-Y-8+5, 0, NULL, "打印份数" },
  { SINGLELINEEDITORITEM, 0, 452-X, 155-Y-8, 500-X, 175-Y-12+4, 0, PrintCopyNumProcedure, ""},

  { STATICTEXTITEM, 0, 377-X, 22+155-Y-8+5, 450-X, 22+175-Y-8+10, 0, NULL, "水平放缩" },
  { SINGLELINEEDITORITEM, 0, 452-X, 21+155-Y-8+5, 500-X, 21+175-Y-12+9, 0, PrintLeftRightProcedure, ""},

  { STATICTEXTITEM, 0, 377-X, 44+155-Y-8+10, 450-X, 44+175-Y-8+15, 0, NULL, "垂直放缩" },
  { SINGLELINEEDITORITEM, 0, 452-X, 42+155-Y-8+10, 500-X, 42+175-Y-12+14, 0, PrintTopBottomProcedure, ""},

  { MULTISELECT, 0, 376-X, 226-Y-12+15, 468-X, 246-Y-12+20, 1, PrintRorate90, "旋转打印" },
  { MULTISELECT, 0, 376-X, 226-Y+22-10+20, 468-X, 246-Y+22-10, 1, PrintReverse, "镜像打印" },
  //{ MULTISELECT, 0, 376-X, 226-Y+44-8, 504-X, 246-Y+44-8, 0, Print2FileOptionProc, "打印到文件" },

  #undef X
  #undef Y
};

Dialogs PrinterSetupDialog[]=
{
  #define X 94
  #define Y 100
  { GLOBALITEM, 6 , 94, 100, 500, 376, 0, NULL,"打印机设定" },

  { STATICTEXTITEM, 0, 136-X, 136-Y, 347-X, 152-Y, 0, NULL, "选择打印机:" },
  { LISTBOXITEM,    0, 128-X, 156-Y, 128-X+8*30, 156-Y+8*16, 0, PrinterList, "" },
  { STATICTEXTITEM, 0, 130-X, 314-Y, 245-X, 330-Y, 0, NULL, "当前打印机:" },
  { SINGLELINEEDITORITEM, 0, 128-X, 336-Y, 468-X, 356-Y, 0, DefaultPrinterProcedure, "" },

  { OKBUTTON,     0, 390-X, 156-Y, 470-X, 186-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 390-X, 210-Y, 470-X, 240-Y, 0, NULL, "放弃" },
  #undef X
  #undef Y
};

const LoadMenus UserMenu[]=
{
  { "",0,0,0,0,8 },              // total H_Menu numbers==6
    { "文件[F]",0,ALT_F,6,1,19 },     // total sub_Menu numbers==19
      { "新文件[N]",MENU_NEW,CTRL_N,8,0,0 },
      { "打开文件[O]",MENU_OPEN,CTRL_O,10,0,0 },
      { "从模板创建[M]",MENU_NEW1,0,12,0,0 },
      { "保存文件[S]",MENU_SAVE,CTRL_S,10,0,0 },
      { "换名存盘[A]",MENU_SAVEAS,0,10,0,0 },
      { "",0,0,0,8,0 },               // Space bar(draw a line), it is also a sub_menu item
      { "插入图文[I]",MENU_IMPORTTEXT,F3,10,0,0 },
      { "保存文本[E]",MENU_EXPORTTEXT,F4,10,0,0 },
      { "传真通讯[F]",0,0,10,0,3 },
             { "发送传真[F]",MENU_EXPORTFAX,0,10,0,0 },
             { "传送文件[S]",MENU_SENDFILE,0,10,0,0 },
             { "接收文件[R]",MENU_RECVFILE,0,10,0,0 },
      { "页图像输出[C]",MENU_PAGEEXPORTPICT,0,12,0,0 },
      { "打印文件[P]",   MENU_PRINT,  CTRL_P,10,0,0 },
      { "",0,0,0,8,0 },
      { "页面设置[G]",MENU_PAGESETUP,0,10,0,0 },
      { "拼版设置[B]", 0,0,10,0,3 },
             { "设置打印版式", MENU_PRINTCUT,0,0,0,0 },
             { "打开打印版式", MENU_OPENPRINTCUT,0,0,0,0 },
             { "保存打印版式", MENU_SAVEPRINTCUT,0,0,0,0 },
      { "字库设置[V]", 0,0,10,0,2 },
             { "安装字库",     MENU_INSTALLFONT,0,0,0,0 },
             { "设置字库路径", MENU_SETFONTPATH,0,0,0,0 },
      { "通讯传真设置",  MENU_MODEMSETUP,    0,0,0,0 },
      { "打印机设置[T]", MENU_PRINTERSETUP,0,12,0,0 },
      { "",0,0,0,8,0 },
      { "退出系统[X]",MENU_EXIT,ALT_X,10,0,0 },

    { "编辑[E]",0,ALT_E,6,1,10},
      { "复原操作[U]",MENU_UNDO,ALT_BACKSPACE,10,0,0 },
      { "",0,0,0,8,0 },
      { "复制[C]",MENU_COPY,CTRL_INS,6,0,0 },
      { "粘贴[P]",MENU_PASTE,SHIFT_INS,6,0,0 },
      { "剪切[T]",MENU_CUT,SHIFT_DEL,6,0,0 },
      { "删除[D]",MENU_CLEAR,DEL,6,0,0 },
      { "",0,0,0,8,0 },
      { "查找[F]",MENU_FIND,CTRL_F,6,0,0 },
      { "替换[E]",MENU_REPLACE,0,6,0,0 },
      { "查下一个[N]",MENU_NEXT,CTRL_L,10,0,0 },

    { "查看[V]",0,ALT_V,6,1,11 },
      { "全局显示[W]",MENU_VIEWPORTFITWINDOW,ALT_1,10,0,0 },
      { "原版面50%",MENU_VIEWPORTHALF,ALT_2,0,0,0 },
      { "原版面75%",MENU_VIEWPORTQUART,ALT_3,0,0,0 },
      { "1:1大小[A]",MENU_VIEWPORTACTUAL,ALT_0,9,4,0 },
      { "原版面200%",MENU_VIEWPORTDOUBLE,ALT_4,0,0,0 },
      { "",0,0,0,8,0 },
      { "重画版面[R]",MENU_BOXREDRAW,F10,10,0,0 },
      { "计算[E]",0,0,6,0, 3 },
                { "计算当前行[A]",MENU_CALCULATELINE, CTRL_E,12,0,0 },
                { "个人所得税[B]",MENU_CALCULATEPERSON,    0,12,0,0 },
                { "增值税[C]",    MENU_CALCULATEMORE,      0, 8,0,0 },
      { "",0,0,0,8,0 },
      { "显示工具[T]",MENU_VIEWTOOLS,0,10,0,0 },
      { "显示标尺[C]",MENU_CLIBRATION,0,10,0,0 },

    { "样式[Y]",0,ALT_Y,6,1,8},
        { "字体[F]",MENU_FONT,F5,6,0,0 },
        { "字号[H]",0,0,6,0,12 },
                    { "初号字",MENU_FONTSIZE0,0,0,0,0 },
                    { "一号字",MENU_FONTSIZE1,0,0,0,0 },
                    { "二号字",MENU_FONTSIZE2,0,0,0,0 },
                    { "三号字",MENU_FONTSIZE3,0,0,0,0 },
                    { "四号字",MENU_FONTSIZE4,0,0,0,0 },
                    { "小四号字",MENU_FONTSIZE4X,0,0,0,0 },
                    { "五号字",MENU_FONTSIZE5,0,0,0,0 },
                    { "小五号字",MENU_FONTSIZE5X,0,0,0,0 },
                    { "六号字",MENU_FONTSIZE6,0,0,0,0 },
                    { "七号字",MENU_FONTSIZE7,0,0,0,0 },
                    { "八号字",MENU_FONTSIZE8,0,0,0,0 },
                    { "自定义",MENU_FONTSIZEFREE,0,0,0,0 },
        { "图文颜色[C]",0,0,10,0,16},
                    { "黑色",MENU_CHARBLACK,0,0,0,0 },
                    { "红色",MENU_CHARRED,0,0,0,0},
                    { "绿色",MENU_CHARGREEN,0,0,0,0 },
                    { "蓝色",MENU_CHARBLUE,0,0,0,0 },
                    { "青色",MENU_CHARCYAN,0,0,0,0 },
                    { "品红色",MENU_CHARMAGENTA,0,0,0,0 },
                    { "黄色",MENU_CHARYELLOW,0,0,0,0 },
                    { "深红色",MENU_CHARDKRED,0,0,0,0 },
                    { "深绿色",MENU_CHARDKGREEN,0,0,0,0 },
                    { "深蓝色",MENU_CHARDKBLUE,0,0,0,0 },
                    { "深青色",MENU_CHARDKCYAN,0,0,0,0 },
                    { "紫色",MENU_CHARDKMAGENTA,0,0,0,0 },
                    { "橙色",MENU_CHARDKYELLOW,0,0,0,0 },
                    { "深灰色",MENU_CHARDKGRAY,0,0,0,0 },
                    { "浅灰色",MENU_CHARLTGRAY,0,0,0,0 },
                    { "白色",MENU_CHARWHITE,0,0,0,0 },
//                  { "自定义色",MENU_CHARUSERCOLOR,0,0,0,0 },

//      { "图文灰阶[C]",0,0,10,0,11},
//                  { "0%",MENU_CHARSHADOW0,0,0,0,0 },
//                  { "10%",MENU_CHARSHADOW1,0,0,0,0 },
//                  { "20%",MENU_CHARSHADOW2,0,0,0,0 },
//                  { "30%",MENU_CHARSHADOW3,0,0,0,0 },
//                  { "40%",MENU_CHARSHADOW4,0,0,0,0 },
//                  { "50%",MENU_CHARSHADOW5,0,0,0,0 },
//                  { "60%",MENU_CHARSHADOW6,0,0,0,0 },
//                  { "70%",MENU_CHARSHADOW7,0,0,0,0 },
//                  { "80%",MENU_CHARSHADOW8,0,0,0,0 },
//                  { "90%",MENU_CHARSHADOW9,0,0,0,0 },
//                  { "100%",MENU_CHARSHADOWF,0,0,4,0 },
//                  { "其他...",MENU_CHARSHADOWOTHER,0,0,0,0},

        { "字符效果[T]",0,0,10,0,6},
                    { "正常[N]",MENU_CHARNORMAL,0,6,0,0 },
                    { "倾斜[S]",MENU_CHARITALIC,0,6,0,0 },
                    { "上标[U]",MENU_SUPERSCRIPT,0,6,0,0 },
                    { "下标[D]",MENU_SUBSCRIPT,0,6,0,0 },
                    { "升降[J]",MENU_UPDOWN,0,6,0,0 },
//                    { "立体",MENU_CHAR3D,0,0,0,0 },
//                    { "空心",MENU_CHAROUTLINE,0,0,0,0 },
                    { "下划线[X]",0,0,8,0,5},
                        { "取消下划线",MENU_SUBLINE0,0,0,0,0 },
                        { "点下划线",MENU_SUBLINE1,0,0,0,0 },
                        { "细下划线",MENU_SUBLINE2,0,0,0,0 },
                        { "粗下划线",MENU_SUBLINE3,0,0,0,0 },
                        { "文武下划线",MENU_SUBLINE4,0,0,0,0 },
        { "",0,0,0,8,0 },
        { "文字排列[A]",0,0,10,0,4 },
                    { "左对齐",MENU_ALIGNMENTLEFT,CTRL_F1,0,0,0 },
                    { "右对齐",MENU_ALIGNMENTRIGHT,CTRL_F3,0,0,0 },
                    { "居中",MENU_ALIGNMENTMIDDLE,CTRL_F2,0,0,0 },
                    { "两端对齐",MENU_ALIGNMENTAVERAGE,CTRL_F4,0,0,0 },

        { "段落字距[D]",0,0,10,0,6 },
                    { "一倍字距" ,MENU_COLGAP1,CTRL_F5,0,0,0 },
                    { "1.25倍字距",MENU_COLGAP125,CTRL_F6,0,0,0 },
                    { "1.5倍字距",MENU_COLGAP15,CTRL_F7,0,0,0 },
                    { "1.75倍字距",MENU_COLGAP175,0,0,0,0 },
                    { "二倍字距" ,MENU_COLGAP2,0,0,0,0 },
                    { "自定义字距",MENU_COLGAPUSER,0,0,0,0 },

        { "段落行距[L]",0,0,10,0,6 },
                    { "一倍行距" ,MENU_ROWGAP1,CTRL_F8,0,0,0 },
                    { "1.25倍行距",MENU_ROWGAP125,CTRL_F9,0,0,0 },
                    { "1.5倍行距",MENU_ROWGAP15,CTRL_F10,0,0,0 },
                    { "1.75倍行距",MENU_ROWGAP175,0,0,0,0 },
                    { "二倍行距" ,MENU_ROWGAP2,0,0,0,0 },
                    { "自定义行距",MENU_ROWGAPUSER,0,0,0,0 },

    { "版框[M]",0,ALT_M,6,1,9},
       { "版框信息[I]",MENU_BOX,0,10,0,0 },
/*---------------------
       { "边框设置[F]",0,0,10,0,3 },
                 { "无边框",MENU_FRAME0,0,0,0,0 },
                 { "单线边框",MENU_FRAME1,0,0,0,0 },
                 { "双线边框",MENU_FRAME2,0,0,0,0 },
------------*/
       { "",0,0,0,8,0 },
       { "锁定版框[+]",MENU_BOXLOCK,0,10,0,0 },
       { "解锁版框[-]",MENU_BOXUNLOCK,0,10,0,0 },
       { "",0,0,0,8,0 },
       { "上浮一层[U]",MENU_FORWARD,0,10,0,0 },
       { "下沉一层[D]",MENU_BACKWARD,0,10,0,0 },
       { "到最顶层[T]",MENU_FRONT,0,10,0,0 },
       { "到最底层[B]",MENU_BACK,0,10,0,0 },
/*-----------
       { "",0,0,0,8,0 },
       { "版框对齐[A]",MENU_BOXALIGN,0,10,0,0 },
--------*/
    { "页面[P]",0,ALT_P,6,1,14},
        { "后续新页[A]",MENU_APPENDPAGE,F7,10,0,0 },
        { "前插新页[I]",MENU_INSERTPAGE,0,10,0,0 },
        { "删除页面[D]",MENU_DELETEPAGE,0,10,0,0 },
        { "移动页面[M]",MENU_MOVEPAGE,0,10,0,0 },
        { "强制分页[B]",MENU_PAGEFEED,0,10,0,0 },
        { "",0,0,0,8,0 },
        { "篇眉[H]",MENU_PAGEHEAD,0,6,0,0 },
        { "页码[C]",MENU_PAGENUM,0,6,0,0 },
        { "",0,0,0,8,0 },
        { "前翻一页[P]",MENU_PREVPAGE,CTRL_PGUP,10,0,0 },
        { "后翻一页[N]",MENU_NEXTPAGE,CTRL_PGDN,10,0,0 },
        { "到第一页[F]",MENU_FIRSTPAGE,0,10,0,0 },
        { "到最后页[L]",MENU_LASTPAGE,0,10,0,0 },
        { "到....页[G]",MENU_GOTOPAGE,CTRL_G,10,0,0 },

/*---------------------------
    { "工具[T]",0,ALT_T,6,1,19},
        { "设计版式[M]",MENU_TOOLMOVE,0,10,0,0 },
        { "编缉图文[S]",MENU_TOOLSELECT,0,10,4,0 },
        { "旋转版框[R]",MENU_TOOLROTATE,0,10,0,0 },
        { "缩放视图[Z]",MENU_TOOLZOOM,0,10,0,0 },
        { "",0,0,0,8,0 },
        { "文本版框[T]",MENU_TOOLTEXT,0,10,0,0 },
        { "",0,0,0,8,0 },
        { "矩形图框[J]",MENU_TOOLRECTANGLE,0,10,0,0 },
        { "园角图框[C]",MENU_TOOLCORNER,0,10,0,0 },
        { "椭圆图框[E]",MENU_TOOLELLIPSE,0,10,0,0 },
        { "自由图框[P]",MENU_TOOLPOLYGON,0,10,0,0 },
        { "",0,0,0,8,0 },
        { "直线[X]",MENU_TOOLSTRAIGHTLINE,0,6,0,0 },
        { "",0,0,0,8,0 },
        { "链接文本框[L]",MENU_TOOLLINK,0,12,0,0 },
        { "解链文本框[U]",MENU_TOOLUNLINK,0,12,0,0 },
        { "",0,0,0,8,0 },
        { "自动表格[F]",MENU_TOOLTABLE,0,6,0,0 },
        { "表格编辑[A]",0,0,10,0,4 },
                { "添加行",MENU_TABLEINSERTLINE,0,1,0,0 },
                { "添加列",MENU_TABLEINSERTCOLUMN,0,1,0,0 },
                { "删除行列",MENU_TABLEDELETELINE,0,1,0,0 },
                //{ "删除列",MENU_TABLEDELETECOLUMN,0,1,0,0 },
                { "合并行列",MENU_TABLEMERGE,0,1,0,0 },
------------------------------------------------------*/
    { "表格[T]",0,ALT_T,6,1,19},

        { "文字排列[P]",0,0,10,0,7 },
            { "左对齐",MENU_ALIGNMENTLEFT,CTRL_F1,0,0,0 },
            { "右对齐",MENU_ALIGNMENTRIGHT,CTRL_F3,0,0,0 },
            { "左右居中",MENU_ALIGNMENTMIDDLE,CTRL_F2,0,0,0 },
            { "两端对齐",MENU_ALIGNMENTAVERAGE,CTRL_F4,0,0,0 },
            { "",0,0,0,8,0 },
            { "自上而下",MENU_ALIGNUPDOWN,0,0,0,0 },
            { "上下均匀",MENU_ALIGNVCENTRE,0,0,0,0 },
        { "表格计算[I]",MENU_TABLECALCULATE,0,10,0,0 },
        { "数据库注入[D]",MENU_TABLEINSERTDBF,0,12,0,0 },
        { "横向统计[R]",MENU_TABLEROWCALC,0,10,0,0 },
        { "竖向统计[C]",MENU_TABLECOLCALC,0,10,0,0 },
        { "",0,0,0,8,0 },
        { "增加一行[A]",MENU_TABLEINSERTLINE,0,10,0,0 },
        { "增加一列[B]",MENU_TABLEINSERTCOLUMN,0,10,0,0 },
        { "删除当前行",MENU_TABLEDELETELINE,  0,0,0,0 },
        { "删除当前列",MENU_TABLEDELETECOLUMN,0,0,0,0 },
        { "扩大当前行",MENU_TABLEADDROW,  ALT_DOWN,0,0,0 },
        { "扩大当前列",MENU_TABLEADDCOL,  ALT_RIGHT,0,0,0 },
        { "缩小当前行",MENU_TABLEDELROW,  ALT_UP,0,0,0 },
        { "缩小当前列",MENU_TABLEDELCOL,  ALT_LEFT,0,0,0 },
        { "",0,0,0,8,0 },
        { "表格线型[E]",MENU_TABLELINETYPE,0,10,0,0 },
        { "表元斜线[F]",MENU_TABLESLANTTYPE,0,10,0,0 },
        { "合并表元[G]",MENU_TABLEMERGE,0,10,0,0 },
        { "拆分表元[H]",MENU_TABLEDISMERGE,0,10,0,0 },

    { "帮助[H]",0,ALT_H,6,1,6 },
        { "轻松帮助[H]",MENU_KEYIDX,F1,10,0,0 },
        { "产品服务[S]",MENU_SERVICE,0,10,0,0 },
        { "",0,0,0,8,0 },
        { "内存状态[M]",MENU_MEMSTAT,0,10,0,0 },
        { "",0,0,0,8,0 },
        { "版权信息[A]",MENU_ABOUT,0,10,0,0 },
};

/***********************
      { "线[L]",0,0,4,0,6 },
        { "线型[S]",MENU_LINESTYLE,0,6,0,0 },
        { "线头[H]",MENU_LINEHEAD,0,6,0,0 },
        { "线尾[T]",MENU_LINETAIL,0,6,0,0 },
        { "线宽[W]",0,0,6,0,8 },
          { "0",MENU_LINEWIDTH0,0,1,0,0 },
          { "1",MENU_LINEWIDTH1,0,1,0,0 },
          { "2",MENU_LINEWIDTH2,0,1,0,0 },
          { "4",MENU_LINEWIDTH4,0,1,0,0 },
          { "6",MENU_LINEWIDTH6,0,1,0,0 },
          { "8",MENU_LINEWIDTH8,0,1,0,0 },
          { "12",MENU_LINEWIDTH12,0,0,0,0 },
          { "手动[M]",MENU_USERLINEWIDTH,0,6,0,0 },
        { "",0,0,0,8,0 },
        { "颜色[C]",MENU_LINECOLOR,0,6,0,0 },
      { "图象[I]",0,0,6,0,6 },
        { "正常对比[C]",MENU_IMAGENORMALCONTRAST,0,10,0,0 },
        { "高反差[H]",MENU_IMAGEHIGHCONTRAST,0,8,0,0 },
        { "灰度化[G]",MENU_IMAGEPOSTERIZE,0,8,0,0 },
        { "",0,0,0,8,0 },
        { "反色[N]",MENU_IMAGENEGATIVE,0,6,0,0 },
        { "颜色[C]",MENU_IMAGECOLOR,0,6,0,0 },
    { "视图[V]",0,ALT_V,6,1,11 },
      { "网格[N]",MENU_VIEWNET,0,6,0,0 },
      { "调色板[P]",MENU_PALETTLE,0,8,0,0 },

      { "库操作[L]",0,0,8,0,3 },
        { "取库[L]",MENU_LOADLIB,0,6,0,0 },
        { "存库[S]",MENU_SAVELIB,0,6,0,0 },
        { "删除[D]",MENU_DELETELIB,0,6,0,0 },
      { "字体使用[F]",MENU_FONTUSE,0,10,0,0 },
      { "图片使用[P]",MENU_PICTUREUSE,0,10,0,0 },
};
***********************/

char *PrinterName[]={        // must order at ascent, see also const.h
      "BJC 600系列彩色喷墨打印机",
      "BJ系列黑白喷墨打印机(高速度)",
      "BJ系列黑白喷墨打印机(高质量)",
      "Brother系列24针宽行黑白打印机",
      "Brother系列24针窄行黑白打印机",
      "EPSON彩色喷墨A3打印机(720DPI)",
      "EPSON彩色喷墨A4打印机(720DPI)",
      "EPSON彩色喷墨打印机(360DPI)",
      "EPSON系列24针宽行彩色打印机",
      "EPSON系列24针宽行黑白打印机",
      "EPSON系列24针窄行彩色打印机",
      "EPSON系列24针窄行黑白打印机",
      "EPSON系列黑白喷墨打印机",
 /*---------
      "Canon(佳能)KT-600DPI",
      "Canon(佳能)LX-400DPI",
      "Canon(佳能)SX-300DPI",
      "HP 1200C彩色喷墨打印机",
 ------------*/
      "HP系列彩色喷墨打印机",
      "HP系列黑白喷墨打印机",
      "HP系列激光打印机(A3 300DPI)",
      "HP系列激光打印机(A3 600DPI)",
      "HP系列激光打印机(A4 300DPI)",
      "HP系列激光打印机(A4 600DPI)",
      "HP系列激光打印机ⅡA4 300DPI",
      "LBP-460激光打印机 A4 300DPI",
      "OKI系列24针宽行黑白打印机",
      "OKI系列24针窄行黑白打印机",
      "STAR系列24针宽行彩色打印机",
      "STAR系列24针宽行黑白打印机",
      "STAR系列24针窄行彩色打印机",
      "STAR系列24针窄行黑白打印机",
/*
      "北佳激光打印机(A3 300DPI)",
      "北佳激光打印机(A3 400DPI)",
      "北佳激光打印机(A3 600DPI)",
      "北佳激光打印机(A4 300DPI)",
      "北佳激光打印机(A4 400DPI)",
      "北佳激光打印机(A4 600DPI)",
      "北佳激光打印机(B4 300DPI)",
      "北佳激光打印机(B4 360DPI)",
      "北佳激光打印机(B4 400DPI)",
      "北佳激光打印机(B4 600DPI)",
*/
      "松下6100激光打印机A4 300DPI",
      "松下6500激光打印机A4 300DPI",
      NULL
};
char *HintPrinterName[]={
      "佳能BJC-600,BJC-800,BJC-4000等",
      "佳能BJ-10e,BJ-10SX,BJ-200等",
      "佳能BJ-10e,BJ-10SX,BJ-200等",
      "Brother-M1724,M2024等宽行",
      "Brother-M1724,M2024等窄行",
      "爱普生NZ1500等",
      "爱普生Stylus Color,Stylus Color II等",
      "爱普生Stylus Color,Stylus Color II等",
      "爱普生LQ-2500,DLQ-2000等宽行",
      "爱普生LQ-1600,LQ-1800等宽行",
      "爱普生LQ-2500,DLQ-2000,LQ-300等窄行",
      "爱普生LQ-1600,LQ-1800,LQ-100等窄行",
      "爱普生Stylus 1000,Stylus 800等",
      "惠普DJ525Q,DJ560C,DJ400C,DJ320C,DJ600C等",
      "惠普DJ-500,DJ-500Q,DJ-400等",
      "惠普HP4V,HP4VC,HP4MV等",
      "惠普HP4V,HP4VC,HP4MV等",
      "惠普HPⅢ,HP4,HP4LC,HP4L,HP4P,HP5P等",
      "惠普HP4,HP4LC,HP4L,HP4P,HP5P等",
      "惠普HPⅡ及兼容打印机等",
      "必须在Windows的DOS窗口下使用",
      "OKI-5320,8320,8370,8570等宽行",
      "OKI-5320,8320,8370,8570等窄行",
      "STAR CR3200,CR3240等宽行",
      "STAR AR2400,AR3200,AR3240,CR3240等宽行",
      "STAR CR3200,CR3240等窄行",
      "STAR AR2400,AR3200,AR3240,CR3240等窄行",
      /*
      "视屏激光打印机(A3 300DPI)",
      "视屏激光打印机Pecan PL-A3 400DPI",
      "视屏激光打印机Pecan PL-A3 600DPI",
      "视屏激光打印机Pecan PL-3A",
      "视屏激光打印机Pecan PL-4A",
      "视屏激光打印机(A4 600DPI)",
      "视屏激光打印机Pecan 360T 360DPI",
      "视屏激光打印机Pecan 360T 400DPI",
      "视屏激光打印机Pecan PL-3B/4B 400DPI",
      "视屏激光打印机Pecan PL-3B/4B 600DPI",
      */
      "需在Windows的DOS窗口下使用 !",
      "松下KX-p6500,KX-6500激光打印机",
       NULL
};
int CurrentPrinter=0;
int PrinterFixedLeftMargin;

GroupBoxs GlobalGroup;
FindStructs GlobalFindStruct;

//BoradcastProcedure *LockBoradcast,*UnlockBoradcast;
//BoradcastNoLowMemoryProcedure *NoLowMemoryBoradcast;

FILE *prnstr;
char PrintName[64]="";          //Must greater than serial_len(50)
int TimeCountArr[MAX_TIMER_EVENT]={  0,0,0,0,0  };

unsigned short TypeSum=0;
const char logname[7]={'e', 'z'+5, 'p'-10, '.'+20, 'L'-30, 'o', 'g'+40};
char *rasts[7];
int  RastWidth,RastWidthByte,RastHeight,RastSize,MaxRastY;
int  UseHP1200=FALSE;
short *RowErrBuf=NULL;
int DitherBufLen=0,fDither=0;
unsigned char fRemapRGB=1,SerialTypeLen,fRegist=FALSE;
int ColorR,ColorG,ColorB;
int sysColor=0;

char serial[41];
int GlobalNotDisplay=0;               // ByHance, 95,12.8
int GlobalPageScale=(SCALEMETER*1/SCREENDPI);
int GlobalPageHStart=0,GlobalPageVStart=0;   /* Use for scroll bar */
char GlobalToolBarSetting=0;           /*
                                         Bit 0: Has Tool bar 0
                                             1: Has Ruler
                                             2: Has Tabler
                                             3: Has Grid
                                             4: Has Color Pattlete
                                             5-7: Reversed
                                      */
int GlobalBoxTool=-1;    /* Icon(Box Tool):: init in ..\UI\func2.c */
char GlobalFileStateSign=0;          /*
                                        Bit 0: File has been new or load
                                        Bit 1: File has been modified
                                        Bit 2: File has been saved
                                        Bit 3: 0-INCH; 1-CM
                                        Bit 4: File is being loaded
                                      */
char DebugFileName[100]="";
unsigned short SerialSum=0;
const char ProfileName[]="c:\\ezp\\EZP.Ini";
char regist_str[41];
int GlobalTextPosition=0;            /* Text Box Current char position */
int bAtLineFeed=0;        // ByHance, 95,12.14
unsigned short ssum,tsum;

/* Block Start and End position */
int GlobalTextBlockStart=0,GlobalTextBlockEnd=0;
int Ctrl_KB_pos=-1,Ctrl_KB_box;

/* Current Cell and Cell Block  */
int GlobalTableBlockStart=-1,GlobalTableBlockEnd=-1;
int GlobalTableCell=0;          // -1, ByDg, may be error also

unsigned char GlobalTextStatus=0;   // see ..\doc.txt

PRINTER *printer;
char PrintingSign=0;
int PrinterDPI=100;
int StartPrintPage=1,EndPrintPage=1,PrintCopyN=1;
int UndoOperateSum=0;      // Record the total undo class
char DoUndoSign=0;         // 0=system in do status, 1=system in undo status
int sysGray=0;
unsigned char DitherTable[DitherTableLen];
unsigned char *ByteRGBPalatte=NULL;
unsigned char RGBPalatteSign=0;
unsigned char headdot[8] = {0xff,0x7f,0x3f,0x1f,0xf,0x7,3,1};
unsigned char taildot[8] = {0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff};
unsigned char dot1tab[8] = {0x80,0x40,0x20,0x10,8,4,2,1};
unsigned char hdot[8] = {0,1,3,7,0x0f,0x1f,0x3f,0x7f};
//unsigned char BitFill[8]={ 1,3,7,15,31,63,127,255 };
//unsigned char BitMatrixs[8]={ 1,2,4,8,16,32,64,128 };

int TextCursor=-1;
int TextCursorRotateAngle=0;
int TextCursorPosX=0,TextCursorPosY=0;
int TextCursorRotateAxisX=0,TextCursorRotateAxisY=0;
char CursorBlankSign=1;

MAT2 *ImageMatrix;
unsigned char *ImageData;
unsigned char *ImageClipData=NULL;
unsigned char *ImageNewData,*ImageBufEnd;
HANDLE ImageHandle;

int RealWidth,ImageOriginX,ImageOriginY,ImageAxisX,ImageAxisY;
char NegativeSign,ContrastSign,ColorSign;
unsigned char PosterizedGray[MAXPOSTERIZEDCOLOR];

int ZoomImageAxisX,ZoomImageAxisY;

EdgeFillLine *CurrentEdgeFillLine=DefaultEdgeFillLine;
LineFillLine *CurrentLineFillLine=DefaultLineFillLine;
HANDLE cfnHandle=0;
HANDLE efnHandle=0;
DC SysDc;

TextStyles DEFAULTTYPESTYLE={
        DEFAULTCHARSIZE,
        DEFAULTCHARFONT,
        DEFAULTCHARSLANT,
        DEFAULTCHARHSIZE,
        DEFAULTCHARCOLOR,
        DEFAULTPARAGRAPHALIGN,
        DEFAULTROWGAP,
        DEFAULTSUPERSCRIPT,
        DEFAULTSUBSCRIPT,
        DEFAULTCOLGAP,
        DEFAULTUPDOWN,
};
TextStyles FormattingTextStyle={
        DEFAULTCHARSIZE,
        DEFAULTCHARFONT,
        DEFAULTCHARSLANT,
        DEFAULTCHARHSIZE,
        DEFAULTCHARCOLOR,
        DEFAULTPARAGRAPHALIGN,
        DEFAULTROWGAP,
        DEFAULTSUPERSCRIPT,
        DEFAULTSUBSCRIPT,
        DEFAULTCOLGAP,
        DEFAULTUPDOWN,
};

FILLP fillp;
Boxs TmpBuf;             // ByHance, for stack
int PolygonNumber=0;
char MoveSign=NoMoveSign;

char RotateSign=NoRotateSign;
char LinkSign=FALSE;        /*   0 -- No link   1 -- Has link */
int  BubleHintIdx=0;

const LoadMenus EditorMenu[]=
{
  { "",0,0,0,0,5 },              // total H_Menu numbers==6
    { "文件[F]",0,ALT_F,6,1,8 },     // total sub_Menu numbers==8
      { "新文件[N]",MENU_NEW,CTRL_N,8,0,0 },
      { "打开文件[O]",MENU_OPEN,CTRL_O,10,0,0 },
      { "保存文件[S]",MENU_SAVE,CTRL_S,10,0,0 },
      { "换名存盘[A]",MENU_SAVEAS,0,10,0,0 },
      { "",0,0,0,8,0 },   // Space bar(draw a line), it is also a sub_menu item
      { "插入文本[I]",MENU_IMPORTTEXT,F3,10,0,0 },
      { "",0,0,0,8,0 },
      { "退出系统[X]",MENU_EXIT,ALT_X,10,0,0 },

    { "编辑[E]",0,ALT_E,6,1,10},
      { "复原操作[U]",MENU_UNDO,ALT_BACKSPACE,10,0,0 },
      { "",0,0,0,8,0 },
      { "复制[C]",MENU_COPY,CTRL_INS,6,0,0 },
      { "粘贴[P]",MENU_PASTE,SHIFT_INS,6,0,0 },
      { "剪切[T]",MENU_CUT,SHIFT_DEL,6,0,0 },
      { "删除[D]",MENU_CLEAR,DEL,6,0,0 },
      { "",0,0,0,8,0 },
      { "查找[F]",MENU_FIND,CTRL_F,6,0,0 },
      { "查找替换[E]",MENU_REPLACE,0,10,0,0 },
      { "查下一个[N]",MENU_NEXT,CTRL_L,10,0,0 },

    { "查看[V]",0,ALT_V,6,1,10 },
      { "全局显示[W]",MENU_VIEWPORTFITWINDOW,ALT_1,10,0,0 },
      { "原版面50%",MENU_VIEWPORTHALF,ALT_2,0,0,0 },
      { "原版面75%",MENU_VIEWPORTQUART,ALT_3,0,0,0 },
      { "1:1 大小[A]",MENU_VIEWPORTACTUAL,ALT_0,10,4,0 },
      { "原版面200%",MENU_VIEWPORTDOUBLE,ALT_4,0,0,0 },
      { "",0,0,0,8,0 },
      { "重画版面[R]",MENU_BOXREDRAW,F10,10,0,0 },
      { "",0,0,0,8,0 },
      { "到....行[G]",MENU_GOTOLINE,0,10,0,0 },
      { "计算当前行[E]",MENU_CALCULATELINE,CTRL_E,12,0,0 },

    { "页面[P]",0,ALT_P,6,1,5},
        { "前翻一页[P]",MENU_PREVPAGE,CTRL_PGUP,10,0,0 },
        { "后翻一页[N]",MENU_NEXTPAGE,CTRL_PGDN,10,0,0 },
        { "到第一页[F]",MENU_FIRSTPAGE,0,10,0,0 },
        { "到最后页[L]",MENU_LASTPAGE,0,10,0,0 },
        { "到....页[G]",MENU_GOTOPAGE,CTRL_G,10,0,0 },

    { "帮助[H]",0,ALT_H,6,1,6 },
        { "轻松帮助[H]",MENU_KEYIDX,F1,10,0,0 },
        { "产品服务[S]",MENU_SERVICE,0,10,0,0 },
        { "",0,0,0,8,0 },
        { "内存状态[M]",MENU_MEMSTAT,0,10,0,0 },
        { "",0,0,0,8,0 },
        { "版权信息[A]",MENU_ABOUT,0,10,0,0 },
};

Dialogs EditorHelpDialog[]=
{
  #define X 110
  #define Y 133
  { GLOBALITEM, 4 , 106, 133, 490, 312, 0, KeyHelpProcedure,"轻松帮助" },

  { USERBUTTONITEM, 0, 162-X, 190-Y, 290-X, 218-Y, 10096, NULL, "常用功能键" },
  { USERBUTTONITEM, 0, 322-X, 190-Y, 452-X, 218-Y, 10093, NULL, "编缉功能操作"},
  { USERBUTTONITEM, 0, 162-X, 238-Y, 290-X, 266-Y, 10095, NULL, "轻松编辑小组"},
  { OKBUTTON,       0, 374-X, 244-Y, 434-X, 274-Y, 0,     NULL, "确定" },

  #undef X
  #undef Y
};

int GlobalPageSelect=1;
//////////added by Jerry/////////////
int PrinterMemoryUseLevel = 0;                  //default

////add by Hance for zoom better 97/05/05
int fInZoom=0;
int screendpi=SCREENDPI;
char Source_num;
char TargetField[100],Target_num;
int StartRecord=0,RecordNumber=5;
int StartCellRow=1,StartCellCol=1;
char DispStr[100];
DBF_FIELDLIST   FieldList[MAXFIELDLIST+1];

#ifdef OLD_VERSION
Dialogs FaxSetupDialog[]=
{
#define X     50
#define Y     100
  { GLOBALITEM, 7 , X, Y, (X+440-50), (Y+342-100), 0, NULL,"传真设置" },

      #define FX        92
      #define FY        140
  { FRAMEITEM, 2, FX-X, FY-Y, 212-X, 224-Y, 0, NULL, "电话设置" },
      { SINGLESELECT, 0, 108-FX, 172-FY, 176-FX, 192-FY, 0, PrintRangeProcedure,"脉冲" },
      { SINGLESELECT, 0, 108-FX, 198-FY, 176-FX, 218-FY, 1, PrintRangeProcedure,"音频" },
      #undef FX
      #undef FY

      #define FX        234
      #define FY        140
  { FRAMEITEM, 2, FX-X, FY-Y, 366-X, 224-Y, 0, NULL, "拨号方式" },
      { SINGLESELECT, 0, 246-FX, 172-FY, 358-FX, 192-FY, 0, TableLineColOptionProcedure, "自动" },
      { SINGLESELECT, 0, 246-FX, 198-FY, 358-FX, 218-FY, 1, TableLineColOptionProcedure, "手动" },
      #undef FX
      #undef FY

  { STATICTEXTITEM,       0,  92-X, 244-Y, 220-X, 264-Y, 0, NULL, "您的名字或电话号" },
  { SINGLELINEEDITORITEM, 0, 234-X, 242-Y, 364-X, 262-Y, 0, PageHeadLeftProcedure, "" },

  { MULTISELECT, 0, 92-X, 278-Y, 210-X, 298-Y, 0, PageFootEnableProcedure, "每页前加抬头" },

  { OKBUTTON,     0, 220-X, 300-Y, 300-X, 330-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 308-X, 300-Y, 388-X, 330-Y, 0, NULL, "放弃" },

#undef X
#undef Y
};
#else
Dialogs FaxSetupDialog[]=
{
#define X     50
#define Y     100
  { GLOBALITEM, 8 , X, Y, (X+440-50), (Y+358-100), 0, NULL,"通讯传真设置" },

      #define FX        92
      #define FY        140
  { FRAMEITEM, 2, FX-X, FY-Y, 212-X, 224-Y, 0, NULL, "电话类型" },
      { SINGLESELECT, 0, 108-FX, 168-FY, 176-FX, 188-FY, 0, TelPlusProcedure,"脉冲" },
      { SINGLESELECT, 0, 108-FX, 194-FY, 176-FX, 214-FY, 1, TelPlusProcedure,"音频" },
      #undef FX
      #undef FY

      #define FX        234
      #define FY        140
  { FRAMEITEM, 2, FX-X, FY-Y, 366-X, 224-Y, 0, NULL, "拨号方式" },
      { SINGLESELECT, 0, 246-FX, 168-FY, 358-FX, 188-FY, 0, TelAutoDialProcedure, "自动拨号" },
      { SINGLESELECT, 0, 246-FX, 194-FY, 358-FX, 214-FY, 1, TelAutoDialProcedure, "手动拨号" },
      #undef FX
      #undef FY

  { STATICTEXTITEM,       0,  92-X, 244-Y, 220-X, 264-Y, 0, NULL, "通讯串口号(1至4)" },
  { SINGLELINEEDITORITEM, 0, 234-X, 242-Y, 364-X, 262-Y, 0, TelComXProcedure, "" },

  { STATICTEXTITEM,       0,  92-X, 272-Y, 220-X, 292-Y, 0, NULL, "您的名字或电话号" },
  { SINGLELINEEDITORITEM, 0, 234-X, 270-Y, 364-X, 290-Y, 0, TelLocalIDProcedure, "" },

  { OKBUTTON,     0, 220-X, 310-Y, 300-X, 340-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 308-X, 310-Y, 388-X, 340-Y, 0, NULL, "放弃" },

#undef X
#undef Y
};
#endif

Dialogs ExportFaxDialog[]=
{
  #define X 87
  #define Y 100
  { GLOBALITEM, 6 , X, Y, 477, 320, 0, NULL,"发送传真" },

  { OKBUTTON,     0, 270-X, 270-Y, 350-X, 299-Y, 0, NULL, "发送" },
  { CANCELBUTTON, 0, 358-X, 270-Y, 438-X, 299-Y, 0, NULL, "放弃" },

  { FRAMEITEM, 7, 116-X, 146-Y, 310-X, 283-Y-27, 0, NULL, "页范围" },
      #define FX        148
      #define FY        146
     { SINGLESELECT, 0, 164-FX, 165-FY, 214-FX, 185-FY, 0, PrintRangeProcedure, "本页" },
     { SINGLESELECT, 0, 224-FX, 165-FY, 274-FX, 185-FY, 1, PrintRangeProcedure, "全部" },
     { SINGLESELECT, 0, 284-FX, 165-FY, 334-FX, 185-FY, 2, PrintRangeProcedure, "部分" },
     { STATICTEXTITEM, 0, 164-FX, 192-FY+2, 224-FX, 212-FY+2, 0, NULL, "起始页:" },
     { STATICTEXTITEM, 0, 164-FX, 222-FY, 224-FX, 242-FY, 0, NULL, "总页数:" },
     { SINGLELINEEDITORITEM, 0, 247-FX, 192-FY+2, 295-FX, 212-FY+2, 0, PrintStartPageProc, ""},
     { SINGLELINEEDITORITEM, 0, 247-FX, 222-FY, 295-FX, 242-FY, 0, PrintTotalPageProc, ""},
     #undef FX
     #undef FY

  { STATICTEXTITEM,       0, 325-X, 160-Y, 428-X, 180-Y, 0, NULL, "对方电话号码:" },
  { SINGLELINEEDITORITEM, 0, 330-X, 184-Y, 440-X, 204-Y, 0, TelRemotePhoneNumProcedure, "" },

  { MULTISELECT, 0, 328-X, 220-Y, 428-X, 240-Y, 1, PrintRorate90, "旋转页面" },
  #undef X
  #undef Y
};

Dialogs TransmitFileDialog[]=
{
  #define X 87
  #define Y 100
  { GLOBALITEM, 4 , X, Y, 420, 260, 0, NULL,"传送文件" },

  { OKBUTTON,     0, 170-X, 200-Y, 250-X, 229-Y, 0, NULL, "传送" },
  { CANCELBUTTON, 0, 258-X, 200-Y, 338-X, 229-Y, 0, NULL, "放弃" },

  { STATICTEXTITEM,       0, 120-X, 152-Y, 223-X, 172-Y, 0, NULL, "对方电话号码:" },
  { SINGLELINEEDITORITEM, 0, 228-X, 150-Y, 360-X, 170-Y, 0, TelRemotePhoneNumProcedure, "" },
};

char InitSection[]="init";
char PrinterEntry[]="Printer";
char VectLibPathEntry[]="VectLibPath";
char TrueTypeLibPathEntry[]="TrueTypeLibPath";

char DefaultSection[]="Default";
char PaperSizeSection[]="PaperSize";
char PageWidthsEntry[]="PageWidths";
char PageHeightsEntry[]="PageHeights";
char LeftMarginEntry[]="LeftMargin";
char RightMarginEntry[]="RightMargin";
char TopMarginEntry[]="TopMargin";
char BottomMarginEntry[]="BottomMargin";

char ScrModeEntry[]="ScreenMode";
char Scr640[]="640";
char Scr800[]="800";
char Scr1024[]="1024";

char FaxSection[]="Fax";
char DialNumEntry[]="DialNumber";
char ComEntry[]="Com";
char AutoDialEntry[]="AutoDial";
char ToneLineEntry[]="ToneLine";
char LocalIdEntry[]="LocalId";

char FilePathSection[]="FilePath";
char LastFilePathEntry[]="LastFilePath";

int fTelTone=1;
int fTelManualDial=0;
char LocalTelId[20];
char DialNumber[20];
int ComX=4;  /*- Com4 -*/

//////////////added by Jerry 97/5/12 for 13x13 font
int GlobalUseLIB13 = 0;
HFILENO GlobalMetaFile = -1;
int faxStartPage=-1,faxEndPage=-1;
int fRecv=0;
BOOL fChgNextCell=FALSE;

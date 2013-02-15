/*-------------------------------------------------------------------
* Name: cursorc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static char TextCursorCount;
static int TextCursorHeight;
static int OldSize=-1,OldFont=-1;
static int NewSize,NewFont;
static int OldPosition=-1;
char CFontsName[][8]={
    "宋体简",
    "楷体简",
    "黑体简",
    "仿宋简",
    "小标宋简",
    "细圆繁",
    "隶书简",
    "魏碑简",
    "姚体简",
    "宋黑简",
    "准圆简",
    "大宋简",
    "隶变简",
    "精圆简",
    "准圆简",
    "隶变繁",
    "细圆简",
    "大黑简",
    "行楷简",
    "综艺繁",
    "琥珀繁",
    "准圆繁",
    "粗克繁",
    "棋牌",
    "宋体繁",
    "仿宋繁",
    "楷体繁",
    "黑体繁",
    "魏碑繁",
    "外文",
    "细黑繁",
    "幼线繁",
    "印章",
    "维文",
    "舒体简",
    "舒体繁",
    "圆黑简",
    "圆黑繁",
    "综艺简",
    "超粗黑",
    "细黑简",
    "中黑繁",
    "平黑繁",
    "日文黑",
    "大楷繁",
    "蒙文",
    "琥珀简",
    "行楷繁",
    "报宋简",
    "大标宋简",
    "细等线简",
    "中等线简",
    "水注简",
    "秀丽繁",
    "彩云繁",
 /*----------------- Extended CFont (from 101) -----------------*/
    "宋体简",
    "楷体简",
    "黑体简",
    "中宋简",
    "粗宋简",
    "小标宋简",
    "粗黑简",
    "中楷简",
    "粗圆简",
    "特圆简",
    "仿宋简",
    "行书简",
    "隶变简",
    "粗魏碑简",
    "报宋简",
    "隶书简",
    "幼线简",
    "细圆简",
    "平黑简",
    "细宋简",
    "行楷简",
    "书宋简",
    "中等线简",
    "细等线简",
    "综艺简",
/**************************************/
    "细宋繁",
    "新宋繁",
    "中宋繁",
    "新中宋繁",
    "粗宋繁",
    "特宋繁",
    "特粗宋繁",
    "超特宋繁",
    "细黑繁",
    "中黑繁",
    "粗黑繁",
    "特黑繁",
    "特粗黑繁",
    "超特黑繁",
    "细圆繁",
    "中圆繁",
    "粗圆繁",
    "特圆繁",
    "特粗圆繁",
    "隶变繁",
    "中隶繁",
    "粗隶繁",
    "平隶繁",
    "方隶繁",
    "角隶繁",
    "粗隶繁",
    "特隶繁",
    "中楷繁",
    "新楷繁",
    "粗楷繁",
    "特楷繁",
    "仿宋繁",
    "粗仿繁",
    "古印繁",
    "淡古繁",
    "方新繁",
    "圆新繁",
    "行书繁",
    "瘦行繁",
    "特行繁",
    "彩云繁",
    "粗勘繁",
    "细勘繁",
    "综艺繁",
    "琥珀繁",
    "叠黑繁",
    "魏碑繁",
    "颜楷繁",
    "黑变繁",
    "海报繁",
    "新奇繁",
    "拙楷繁",
    "隶书繁",
    "报宋繁",
    "平黑繁",
    "书宋繁"
};


static int GetFontParamter()
{
  // int ii;
  if (GlobalTextPosition==OldPosition) return 0;
  return ToolBarHasToolBar();
}
#define GAP1  4
void DispFontWindow(int tt)
{
  int SaveColor,x,y,ii;
  struct viewporttype SaveViewPort;
  char tmpstr[100];

  MouseHidden();
  getviewsettings(&SaveViewPort);
  SaveColor=getcolor();

  setwritemode(COPY_PUT);
  setcolor(EGA_WHITE);
  setviewport(0,0,639,300,0);

  NewSize=TextSearchAttribute(GlobalBoxHeadHandle,GlobalTextPosition,CHARSIZE,&ii);
  ii=NewTextSearchAttribute(GlobalBoxHeadHandle,GlobalTextPosition-1);
  switch(ii)
  {
    case 1:   NewFont=TextSearchCFont(GlobalBoxHeadHandle,GlobalTextPosition-1,&ii);  break;
    case 2:   NewFont=TextSearchEFont(GlobalBoxHeadHandle,GlobalTextPosition-1,&ii);  break;
    default:  NewFont=TextSearchAttribute(GlobalBoxHeadHandle,GlobalTextPosition-1,CHARFONT,&ii);
  }

  x=5*TOOLBARWIDTH+GAP1+LINESPACE+6;
  y=50;
//  if (tt==1)
        {
            switch(NewFont)
             {
                case 0x400: strcpy(tmpstr,"白正"); break;
                case 0x401: strcpy(tmpstr,"白斜"); break;
                case 0x402: strcpy(tmpstr,"黑正"); break;
                case 0x403: strcpy(tmpstr,"黑斜"); break;
                default:
                   memset(tmpstr,0,10);
                   if(NewFont<MAXVECFONT)
                     strncpy(tmpstr,CFontsName[NewFont],4);
                   else
                   if(NewFont>=100 && NewFont<MAXTTFONTJ )
                     strncpy(tmpstr,CFontsName[NewFont-100+MAXVECFONT],4);
                   else
                   if(NewFont>=200&&NewFont<MAXTTFONTF)
                     strncpy(tmpstr,CFontsName[NewFont-200+MAXVECFONT+MAXTTFONTJ-100],4);
                   else
                     strcpy(tmpstr,"扩展");
                   break;
             }

            DisplayString(tmpstr,x+2,53,EGA_BLACK,EGA_WHITE);
            OldFont=NewFont;
        }
// if (tt==2)
        {
            x=x+TOOLBARWIDTH+40;
            sprintf(tmpstr,"%04d",NewSize);
            if (abs(NewSize-583)<4) {strcpy(tmpstr,"初号"); goto disp_size_str; }
            if (abs(NewSize-361)<4) {strcpy(tmpstr,"一号"); goto disp_size_str; }
            if (abs(NewSize-305)<4) {strcpy(tmpstr,"二号"); goto disp_size_str; }
            if (abs(NewSize-222)<4) {strcpy(tmpstr,"三号"); goto disp_size_str; }
            if (abs(NewSize-187)<4) {strcpy(tmpstr,"四号"); goto disp_size_str; }
            if (abs(NewSize-166)<4) {strcpy(tmpstr,"小四"); goto disp_size_str; }
            if (abs(NewSize-145)<4) {strcpy(tmpstr,"五号"); goto disp_size_str; }
            if (abs(NewSize-125)<4) {strcpy(tmpstr,"小五"); goto disp_size_str; }
            if (abs(NewSize-111)<4) {strcpy(tmpstr,"六号"); goto disp_size_str; }
            if (abs(NewSize-76)<4) {strcpy(tmpstr,"七号"); goto disp_size_str; }
            if (abs(NewSize-69)<4) {strcpy(tmpstr,"八号"); goto disp_size_str; }
            sprintf(tmpstr,"%f",(float)(NewSize/39.3f+0.009f));
            tmpstr[4]=0;
            disp_size_str:
            DisplayString(tmpstr,x+3,53,EGA_BLACK,EGA_WHITE);
            OldSize=NewSize;
        }

  setcolor(SaveColor);
  setviewport(SaveViewPort.left,SaveViewPort.top,SaveViewPort.right,
              SaveViewPort.bottom,SaveViewPort.clip);
  MouseShow();
//  CreatStatusWindow();
}

void TextCursorDisplay(void)
{
  int SaveColor;
  int x,y;
  short XY[2*4];
  int Left,Top,Right,Bottom;
  struct viewporttype SaveViewPort;


  if (GlobalBoxTool!=IDX_INPUTBOX || (GlobalBoxHeadHandle<=0)
      || CursorBlankSign<=0 || fInZoom    // ByHance, 97,5.4
      || (ItemGetFather(GlobalBoxHeadHandle)!=GlobalCurrentPage)
      || (ActiveMenu>0)||(ActiveWindow!=1)
      || GlobalTextBlockEnd>GlobalTextBlockStart
      || GlobalTableBlockStart!=GlobalTableBlockEnd
      ||!CurrentBoxEditable()||IsInGlobalBrowser())
     return;

  if (TextCursorCount)
     TextCursorCount=0;
  else
     TextCursorCount=1;

  getviewsettings(&SaveViewPort);
  SaveColor=getcolor();
  setcolor(EGA_WHITE);
  setwritemode(XOR_PUT);
  MouseHidden();
  WindowGetRect(1,&Left,&Top,&Right,&Bottom);
  setviewport(Left,Top,Right,Bottom,1);

  XY[0]=TextCursorPosX;  XY[1]=TextCursorPosY-TextCursorHeight;
  XY[6]=TextCursorPosX;  XY[7]=TextCursorPosY;
  XY[2]=XY[4]=XY[0]+5;  XY[3]=XY[1];
  XY[5]=XY[7];

  if (TextCursorRotateAngle)
  {
     RotatePoint(&x,&y,XY[0],XY[1],
           TextCursorRotateAxisX,TextCursorRotateAxisY,TextCursorRotateAngle);
     XY[0]=x; XY[1]=y;
     RotatePoint(&x,&y,XY[6],XY[7],
           TextCursorRotateAxisX,TextCursorRotateAxisY,TextCursorRotateAngle);
     XY[6]=x; XY[7]=y;
  }

  if (TextIsOverwrite())                // ByHance, 96,1.10
  {
     if (TextCursorRotateAngle)
     {
        RotatePoint(&x,&y,XY[2],XY[3],
              TextCursorRotateAxisX,TextCursorRotateAxisY,TextCursorRotateAngle);
        XY[2]=x; XY[3]=y;
        RotatePoint(&x,&y,XY[4],XY[5],
              TextCursorRotateAxisX,TextCursorRotateAxisY,TextCursorRotateAngle);
        XY[4]=x; XY[5]=y;
     }
     fillpoly(4,XY);
  }
  else
  {
     line(XY[0],XY[1],XY[6],XY[7]);
     if (TextCursorRotateAngle%180==90)
     {
        line(XY[0],XY[1]+1,XY[6]+1,XY[7]+1);
     } else {
        line(XY[0]+1,XY[1],XY[6]+1,XY[7]);
     }
  }
  setwritemode(COPY_PUT);

  setcolor(SaveColor);
  setviewport(SaveViewPort.left,SaveViewPort.top,SaveViewPort.right,
              SaveViewPort.bottom,SaveViewPort.clip);
  MouseShow();


  //    By zjh 96.6
  if (GetFontParamter()&&TextCursorCount==1)
  {
    DispFontWindow(2);
    OldPosition=GlobalTextPosition;
  }
}



void CreatFontWindow(int x1,int x2)
{
  int SaveColor;
  struct viewporttype SaveViewPort;

  MouseHidden();
  getviewsettings(&SaveViewPort);
  SaveColor=getcolor();

  setwritemode(COPY_PUT);

  //setcolor(EGA_WHITE);
  //setcolor(EGA_DARKGRAY);
  setcolor(EGA_LIGHTGRAY);      //DarkBlack
  setviewport(0,0,639,200,0);
  bar(x1,50,x2,50+20);

  setcolor(EGA_DARKGRAY);
  line(x1,50,x2,50);
  line(x1,51,x2-1,51);
  line(x1,50,x1,50+20);
  line(x1+1,50,x1+1,50+19);

  setcolor(EGA_WHITE);
  line(x1,50+20,x2,50+20);
  line(x2,50,x2,50+20);

  //DisplayString("字符",x1+3,54,0,15);

  setcolor(SaveColor);
  setviewport(SaveViewPort.left,SaveViewPort.top,SaveViewPort.right,
              SaveViewPort.bottom,SaveViewPort.clip);
  MouseShow();
  CreatStatusWindow();
}



void CreatStatusWindow()
{
  int SaveColor;
  struct viewporttype SaveViewPort;
  int maxx,maxy,use;
  use=0;
  if (use)
  {
  maxx=getmaxx();
  maxy=getmaxy();

  MouseHidden();
  getviewsettings(&SaveViewPort);
  SaveColor=getcolor();

  setwritemode(COPY_PUT);
  setcolor(EGA_DARKGRAY);       //gray
  setviewport(0,0,maxx,maxy,1);
  #define Left_Bottom   28
  #define SPAC  4
  #define WID1  130
  #define WID2  240

  bar(1,maxy-Left_Bottom,maxx,maxy-1);

  setcolor(EGA_DARKGRAY);      //DarkBlack

  bar(5,maxy-Left_Bottom+SPAC,WID1,maxy-SPAC);
  bar(WID1+SPAC,maxy-Left_Bottom+SPAC,WID2+SPAC,maxy-SPAC);
  bar(WID2+SPAC*2,maxy-Left_Bottom+SPAC,maxx-SPAC,maxy-SPAC);

  setcolor(EGA_LIGHTGRAY);

  line(SPAC,maxy-Left_Bottom+SPAC,WID1,maxy-Left_Bottom+SPAC);
  line(SPAC,maxy-Left_Bottom+SPAC+1,WID1-1,maxy-Left_Bottom+SPAC+1);
  line(SPAC,maxy-Left_Bottom+SPAC,SPAC,maxy-SPAC);
  line(SPAC+1,maxy-Left_Bottom+SPAC,SPAC+1,maxy-SPAC-1);

  line(WID1+SPAC,maxy-Left_Bottom+SPAC,WID2+SPAC,maxy-Left_Bottom+SPAC);
  line(WID1+SPAC,maxy-Left_Bottom+SPAC+1,WID2+SPAC-1,maxy-Left_Bottom+SPAC+1);
  line(WID1+SPAC,maxy-Left_Bottom+SPAC,WID1+SPAC,maxy-SPAC);
  line(WID1+SPAC+1,maxy-Left_Bottom+SPAC,WID1+SPAC+1,maxy-SPAC-1);

  line(WID2+SPAC*2,maxy-Left_Bottom+SPAC,maxx-SPAC,maxy-Left_Bottom+SPAC);
  line(WID2+SPAC*2,maxy-Left_Bottom+SPAC+1,maxx-SPAC-1,maxy-Left_Bottom+SPAC+1);
  line(WID2+SPAC*2,maxy-Left_Bottom+SPAC,WID2+SPAC*2,maxy-SPAC);
  line(WID2+SPAC*2+1,maxy-Left_Bottom+SPAC,WID2+SPAC*2+1,maxy-SPAC-1);

  setcolor(EGA_WHITE);
  line(SPAC,maxy-SPAC,WID1,maxy-SPAC);
  line(SPAC+WID1,maxy-SPAC,WID2+SPAC,maxy-SPAC);
  line(SPAC*2+WID2,maxy-SPAC,maxx-SPAC,maxy-SPAC);


  setcolor(SaveColor);
  setviewport(SaveViewPort.left,SaveViewPort.top,SaveViewPort.right,
              SaveViewPort.bottom,SaveViewPort.clip);
  MouseShow();
 }
}

int TextCreatCursor(HWND Window,int Height,int RotateAxisX,int RotateAxisY,
                    int RotateAngle)
{
  TextCursorHeight=Height;
  TextCursorCount=0;
  TextCursorRotateAxisX=RotateAxisX;
  TextCursorRotateAxisY=RotateAxisY;
  TextCursorRotateAngle=RotateAngle;
  return(TimerInsert(CURSORBLINKTIME,Window));
}

void TextCursorOff(void)
{
  if (TextCursorCount)
  {
     TextCursorDisplay();
     TextCursorCount=0;
  }
}

/*-----------
void TextCursorMoveXTo(int MovetoX)
{
  TextCursorOff();
  TextCursorPosX=MovetoX;
}
---------*/
void TextCursorMoveTo(int MovetoX,int MovetoY)
{
//  TextCursorMoveXTo(MovetoX);
  TextCursorOff();
  TextCursorPosX=MovetoX;
  TextCursorPosY=MovetoY;
}

void TextCursorSetHeight(int Height)
{
  TextCursorOff();
  TextCursorHeight=Height;
}

void TextDestroyCursor(int *Cursor)
{
  TextCursorOff();
  TimerDelete(*Cursor);
  *Cursor=-1;
}

void TextCursorSetRotate(int RotateAxisX,int RotateAxisY,int RotateAngle)
{
  TextCursorOff();
  TextCursorRotateAxisX=RotateAxisX;
  TextCursorRotateAxisY=RotateAxisY;
  TextCursorRotateAngle=RotateAngle;
}

void CloseTextCursor(void)
{
  if (CursorBlankSign==1)
     TextCursorOff();
  CursorBlankSign=0;
}

void OpenTextCursor(void)
{
  CursorBlankSign=1;
}

/*------ not used -----
int TextCursorIsOn(void)
{
  return(CursorBlankSign>0);
}
--------------*/

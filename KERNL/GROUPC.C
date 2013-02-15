/*-------------------------------------------------------------------
* Name: groupc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

int Group(HPAGE HPage,ORDINATETYPE GroupLeft,ORDINATETYPE GroupTop,
            ORDINATETYPE GroupWidth,ORDINATETYPE GroupHeight)
{
  HBOX HBox;
  TextBoxs *TextBox;
  int HBoxDots,BoxBorderLeft,BoxBorderTop,BoxBorderRight,BoxBorderBottom;
  ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
  ORDINATETYPE GroupRight,GroupBottom;

  GlobalGroupClear();

  if (!GlobalGroupGetWidth()||!GlobalGroupGetHeight())
     return(0);
  if(HPage<=0)          // ByHance
     return(0);

  if (GlobalGroupGetWidth()<0)
  {
     GlobalGroupSetLeft(GlobalGroupGetLeft()+GlobalGroupGetWidth());
     GlobalGroupSetWidth(-GlobalGroupGetWidth());
  }
  if (GlobalGroupGetHeight()<0)
  {
     GlobalGroupSetTop(GlobalGroupGetTop()+GlobalGroupGetHeight());
     GlobalGroupSetHeight(-GlobalGroupGetHeight());
  }
  GroupLeft=GlobalGroupGetLeft();
  GroupTop=GlobalGroupGetTop();
  GroupRight=GlobalGroupGetLeft()+GlobalGroupGetWidth();
  GroupBottom=GlobalGroupGetTop()+GlobalGroupGetHeight();

  HBox=PageGetBoxHead(HPage);
  while (HBox>0)
  {
    TextBox=HandleLock(ItemGetHandle(HBox));
    if (TextBox==NULL)
       return(OUTOFMEMORY);
    BoxGetPolygonBorder(TextBox,&HBoxDots,BoxXY);
    HandleUnlock(ItemGetHandle(HBox));
    PolygonGetMinRectangle(HBoxDots,BoxXY,&BoxBorderLeft,&BoxBorderTop,
                           &BoxBorderRight,&BoxBorderBottom);
    if (GroupLeft<=BoxBorderLeft&&GroupTop<=BoxBorderTop
        &&GroupRight>BoxBorderRight&&GroupBottom>BoxBorderBottom)
       GlobalGroupSetTopBox(HBox);
    HBox=BoxGetNext(HBox);
  }

  GlobalGroupSetRotateAngle(0);
  GlobalGroupSetRotateAxisX(0);
  GlobalGroupSetRotateAxisY(0);
  GlobalGroupSetSign();
  GlobalGroupSetPage(HPage);

  return(GlobalGroupGetSumBox());
}

int GroupAll(HPAGE HPage)
{
  HBOX HBox;
  TextBoxs *TextBox;
  int HBoxDots;
  ORDINATETYPE BoxBorderLeft,BoxBorderTop,BoxBorderRight,BoxBorderBottom;
  ORDINATETYPE GroupLeft=32767,GroupTop=32767,GroupRight=-32767,
               GroupBottom=-32767;
  ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];

  if(HPage<=0)          // ByHance
     return(0);

  HBox=PageGetBoxHead(HPage);
  GlobalGroupClear();

  while (HBox>0)
  {
    TextBox=HandleLock(ItemGetHandle(HBox));
    if (TextBox==NULL)
       return(OUTOFMEMORY);
    BoxGetPolygonBorder(TextBox,&HBoxDots,BoxXY);
    HandleUnlock(ItemGetHandle(HBox));
    PolygonGetMinRectangle(HBoxDots,BoxXY,&BoxBorderLeft,&BoxBorderTop,
                           &BoxBorderRight,&BoxBorderBottom);
    GlobalGroupSetTopBox(HBox);
    if (GroupLeft>BoxBorderLeft)
       GroupLeft=BoxBorderLeft;
    if (GroupTop>BoxBorderTop)
       GroupTop=BoxBorderTop;
    if (GroupRight<BoxBorderRight)
       GroupRight=BoxBorderRight;
    if (GroupBottom<BoxBorderBottom)
       GroupBottom=BoxBorderBottom;
    HBox=BoxGetNext(HBox);
  }

  GlobalGroupSetLeft(GroupLeft);
  GlobalGroupSetTop(GroupTop);
  GlobalGroupSetWidth(GroupRight-GroupLeft);
  GlobalGroupSetHeight(GroupBottom-GroupTop);
  GlobalGroupSetRotateAngle(0);
  GlobalGroupSetRotateAxisX(0);
  GlobalGroupSetRotateAxisY(0);
  GlobalGroupSetSign();
  GlobalGroupSetPage(HPage);

  return(GlobalGroupGetSumBox());
}

void GroupDelete()
{
  int i;

  for (i=0;i<GlobalGroupGetSumBox();i++)
      BoxDelete(GlobalGroupGetBox(i));
  GlobalUnGroup();
}

void GroupMove(ORDINATETYPE MoveX,ORDINATETYPE MoveY)
{
  int i;

  for (i=0;i<GlobalGroupGetSumBox();i++)
      BoxMove(GlobalGroupGetBox(i),MoveX,MoveY);

  GlobalGroupSetLeft(GlobalGroupGetLeft()+MoveX);
  GlobalGroupSetTop(GlobalGroupGetTop()+MoveY);
}

void GroupLock(void)
{
  int i;
  TextBoxs *TextBox;

  for (i=0;i<GlobalGroupGetSumBox();i++)
  {
      TextBox=HandleLock(ItemGetHandle(GlobalGroupGetBox(i)));
      if (TextBox==NULL)
         return;
      BoxSetLocked(TextBox);
      HandleUnlock(ItemGetHandle(GlobalGroupGetBox(i)));
  }
}

void GroupUnlock(void)
{
  int i;
  TextBoxs *TextBox;

  for (i=0;i<GlobalGroupGetSumBox();i++)
  {
      TextBox=HandleLock(ItemGetHandle(GlobalGroupGetBox(i)));
      if (TextBox==NULL)
         return;
      BoxSetUnlocked(TextBox);
      HandleUnlock(ItemGetHandle(GlobalGroupGetBox(i)));
  }
}

void GroupRotate(short RotateAngle)
{
  int i;
  TextBoxs *TextBox;

  GlobalGroupSetRotateAngle(GlobalGroupGetRotateAngle()+RotateAngle);

  for (i=0;i<GlobalGroupGetSumBox();i++)
  {
      TextBox=HandleLock(ItemGetHandle(GlobalGroupGetBox(i)));
      if (TextBox==NULL)
         return;
      TextBoxSetRotateAngle(TextBox,TextBoxGetRotateAngle(TextBox)
                            +RotateAngle);
      HandleUnlock(ItemGetHandle(GlobalGroupGetBox(i)));
  }
}

void GroupRotateInit(ORDINATETYPE RotateAxisX,ORDINATETYPE RotateAxisY)
{
  int i,BoxLeft,BoxTop,NewRotateAngle;
  TextBoxs *TextBox;

  for (i=0;i<GlobalGroupGetSumBox();i++)
  {
      TextBox=HandleLock(ItemGetHandle(GlobalGroupGetBox(i)));
      if (TextBox==NULL)
         return;
      BoxLeft=TextBoxGetBoxLeft(TextBox);
      BoxTop=TextBoxGetBoxTop(TextBox);
      if (TextBoxGetRotateAxisX(TextBox)==0&&
          TextBoxGetRotateAxisY(TextBox)==0&&
          TextBoxGetRotateAngle(TextBox)==0)
         NewRotateAngle=0;
      else
         NewRotateAngle=ConvertRotateAngle(TextBoxGetRotateAngle(TextBox),
                        TextBoxGetRotateAxisX(TextBox)+BoxLeft,
                        TextBoxGetRotateAxisY(TextBox)+BoxTop,
                        RotateAxisX,RotateAxisY,&BoxLeft,&BoxTop);
      TextBoxSetBoxLeft(TextBox,BoxLeft);
      TextBoxSetBoxTop(TextBox,BoxTop);
      TextBoxSetRotateAngle(TextBox,NewRotateAngle);
      TextBoxSetRotateAxisX(TextBox,RotateAxisX-BoxLeft);
      TextBoxSetRotateAxisY(TextBox,RotateAxisY-BoxTop);
      HandleUnlock(ItemGetHandle(GlobalGroupGetBox(i)));
  }

  BoxLeft=GlobalGroupGetLeft();
  BoxTop=GlobalGroupGetTop();

  if (!GlobalGroupGetRotateAngle()&&!GlobalGroupGetRotateAxisX()
      &&!GlobalGroupGetRotateAxisY())
     NewRotateAngle=0;
  else
     NewRotateAngle=ConvertRotateAngle(GlobalGroupGetRotateAngle(),
                    GlobalGroupGetRotateAxisX()+BoxLeft,
                    GlobalGroupGetRotateAxisY()+BoxTop,
                    RotateAxisX,RotateAxisY,&BoxLeft,&BoxTop);
  GlobalGroupSetLeft(BoxLeft);
  GlobalGroupSetTop(BoxTop);
  GlobalGroupSetRotateAngle(NewRotateAngle);
  GlobalGroupSetRotateAxisX(RotateAxisX-BoxLeft);
  GlobalGroupSetRotateAxisY(RotateAxisY-BoxTop);
}

void GroupSetBack(void)
{
  int i;

  for (i=GlobalGroupGetSumBox()-1;i>=0;i--)
     BoxSetBackground(GlobalGroupGetBox(i));
}

void GroupSetFront(void)
{
  int i;

  for (i=0;i<GlobalGroupGetSumBox();i++)
      BoxSetFront(GlobalGroupGetBox(i));
}

#ifdef UNUSED           // ByHance, 96,1.29
short GroupSetBackColor(long Color)
{
  int i;
  TextBoxs *TextBox;

  for (i=0;i<GlobalGroupGetSumBox();i++)
  {
      TextBox=HandleLock(ItemGetHandle(GlobalGroupGetBox(i)));
      if (TextBox==NULL)
         return(OUTOFMEMORY);
      TextBoxSetBoxBackColor(TextBox,Color);
      HandleUnlock(ItemGetHandle(GlobalGroupGetBox(i)));
  }
  ReturnOK();
}

short GroupAlign(int AlignmentType)
{
  int i,MinLeft=32767,MinTop=32767,MaxRight=-32767,MaxBottom=-32767;
  int HBoxDots,BoxBorderLeft,BoxBorderTop,BoxBorderRight,BoxBorderBottom;
  ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
  TextBoxs *TextBox;
  int SaveRotateAngle;

  for (i=0;i<GlobalGroupGetSumBox();i++)
  {
      TextBox=HandleLock(ItemGetHandle(GlobalGroupGetBox(i)));
      if (TextBox==NULL)
         return(OUTOFMEMORY);
      SaveRotateAngle=TextBoxGetRotateAngle(TextBox);
      //TextBoxSetRotateAngle(TextBox,0);
      BoxGetPolygonBorder(TextBox,&HBoxDots,BoxXY);
      //TextBoxSetRotateAngle(TextBox,SaveRotateAngle);
      HandleUnlock(ItemGetHandle(GlobalGroupGetBox(i)));
      if (SaveRotateAngle)
         continue;
      PolygonGetMinRectangle(HBoxDots,BoxXY,&BoxBorderLeft,&BoxBorderTop,
                             &BoxBorderRight,&BoxBorderBottom);
      if (BoxBorderLeft<MinLeft)
         MinLeft=BoxBorderLeft;
      if (BoxBorderTop<MinTop)
         MinTop=BoxBorderTop;
      if (BoxBorderRight>MaxRight)
         MaxRight=BoxBorderRight;
      if (BoxBorderBottom>MaxBottom)
         MaxBottom=BoxBorderBottom;
  }
  for (i=0;i<GlobalGroupGetSumBox();i++)
  {
      TextBox=HandleLock(ItemGetHandle(GlobalGroupGetBox(i)));
      if (TextBox==NULL)
         return(OUTOFMEMORY);
      SaveRotateAngle=TextBoxGetRotateAngle(TextBox);
      if (!SaveRotateAngle)
      {
         switch (AlignmentType)
         {
           case 0:                     // Align left
                if (TextBoxGetBoxLeft(TextBox)!=MinLeft)
                {
                   BoxMove(GlobalGroupGetBox(i),
                           MinLeft-TextBoxGetBoxLeft(TextBox),0);
                }
                break;
           case 1:                     // Align top
                if (TextBoxGetBoxTop(TextBox)!=MinTop)
                {
                   BoxMove(GlobalGroupGetBox(i),
                           0,MinTop-TextBoxGetBoxTop(TextBox));
                }
                break;
           case 2:                     // Align right
                if (TextBoxGetBoxRight(TextBox)!=MaxRight)
                {
                   BoxMove(GlobalGroupGetBox(i),
                           MaxRight-TextBoxGetBoxRight(TextBox),0);
                }
                break;
           case 3:                     // Align bottom
                if (TextBoxGetBoxBottom(TextBox)!=MaxBottom)
                {
                   BoxMove(GlobalGroupGetBox(i),
                           MaxBottom-TextBoxGetBoxBottom(TextBox),0);
                }
                break;
         }
      }
      HandleUnlock(ItemGetHandle(GlobalGroupGetBox(i)));
  }
  ReturnOK();
}
#endif        // UNUSED           // ByHance, 96,1.29

int MousePointInGroup(int MouseX,int MouseY)
{
  ORDINATETYPE GroupLeft,GroupTop,GroupRight,GroupBottom;
  int RotateAngle;
  ORDINATETYPE RotateAxisX,RotateAxisY;
  ORDINATETYPE TmpX,TmpY;

  if ((!GlobalGroupGetSign())||(GlobalGroupGetPage()!=GlobalCurrentPage))
     return(0);

  GroupLeft=GlobalGroupGetLeft();
  GroupTop=GlobalGroupGetTop();
  GroupRight=GlobalGroupGetRight();
  GroupBottom=GlobalGroupGetBottom();
  RotateAngle=(-1)*GlobalGroupGetRotateAngle();
  RotateAxisX=GlobalGroupGetRotateAxisX()+GroupLeft;
  RotateAxisY=GlobalGroupGetRotateAxisY()+GroupTop;
  if (RotateAngle!=0)
     Rotate(&TmpX,&TmpY,MouseX,MouseY,RotateAxisX,RotateAxisY,RotateAngle);
  else
  {
     TmpX=MouseX;
     TmpY=MouseY;
  }

  if ((TmpX>=GroupLeft)&&(TmpX<GroupRight)&&(TmpY>=GroupTop)&&(TmpY<GroupBottom))
     return(1);
  else
     return(0);
}

void GroupDrawAllBorder(char DrawType)
{
  int Edges[8],Left,Top,Right,Bottom,i;

  struct viewporttype TmpViewPort;
  int SaveColor;
  ORDINATETYPE RotateAxisX,RotateAxisY;
  int RotateAngle;
  unsigned old_style;

  if ((!GlobalGroupGetSign())||(GlobalGroupGetPage()!=GlobalCurrentPage))
     return;

  Edges[0]=GlobalGroupGetLeft();
  Edges[1]=GlobalGroupGetTop();
  Edges[2]=GlobalGroupGetRight();
  Edges[3]=GlobalGroupGetTop();
  Edges[4]=GlobalGroupGetRight();
  Edges[5]=GlobalGroupGetBottom();
  Edges[6]=GlobalGroupGetLeft();
  Edges[7]=GlobalGroupGetBottom();

  RotateAngle=GlobalGroupGetRotateAngle();
  RotateAxisX=GlobalGroupGetRotateAxisX()+GlobalGroupGetLeft();
  RotateAxisY=GlobalGroupGetRotateAxisY()+GlobalGroupGetTop();

  if (RotateAngle)
  {
     for (i=0;i<4;i++)
         Rotate((ORDINATETYPE *)&Edges[2*i],(ORDINATETYPE *)&Edges[2*i+1],
                Edges[2*i],Edges[2*i+1],
                RotateAxisX,RotateAxisY,RotateAngle);
  }

  for (i=0;i<4;i++)
  {
      Edges[2*i]=UserXToWindowX(Edges[2*i]);
      Edges[2*i+1]=UserYToWindowY(Edges[2*i+1]);
  }

  MouseHidden();
  getviewsettings(&TmpViewPort);
  WindowGetRect(1,&Left,&Top,&Right,&Bottom);
  setviewport(Left,Top,Right,Bottom,1);
  SaveColor=getcolor();

  setwritemode(XOR_PUT);
  setcolor(EGA_WHITE);

  #ifdef __TURBOC__
     struct linesettingstype SaveLineStyle;
     getlinesettings(&SaveLineStyle);
     setlinestyle(4,0x5555,1);
  #else
     old_style=getlinestyle();
     setlinestyle(0x5555);
  #endif

  moveto(Edges[0],Edges[1]);
  for (i=1;i<4;i++)
      lineto(Edges[2*i],Edges[2*i+1]);
  lineto(Edges[0],Edges[1]);

  #ifdef __TURBOC__
       setlinestyle(SaveLineStyle.linestyle,
               SaveLineStyle.upattern,
               SaveLineStyle.thickness);
  #else
       setlinestyle(old_style);
  #endif

  setwritemode(COPY_PUT);
  setcolor(SaveColor);
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  MouseShow();

  for (i=0;i<GlobalGroupGetSumBox();i++)
      BoxDrawBorder(GlobalGroupGetBox(i),DrawType);

} /* GroupDrawAllBorder */

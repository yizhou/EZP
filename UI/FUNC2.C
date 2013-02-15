/*-------------------------------------------------------------------
* Name: func2.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"
static int h_old_start=-10000,h_old_step=-1;
static int v_old_start=-10000,v_old_step=-1;
static int OldHClibration=-1,OldVClibration=-1;

static void WindowXScroll(ORDINATETYPE WindowLeftIncrease,int WindowWidth)
{
  Pages *MidPage;
  ORDINATETYPE PageWidth;
  HWND HScrollWindow;
  int HScrollWidth;

  MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
  PageWidth=PageGetPageWidth(MidPage);

  v_old_start=-10000;   // ByHance, 96,4.10

  //MessageGo(1,HHSCROLLMOVE,(GlobalPageHStart+WindowLeftIncrease*GlobalPageScale),
  //            (PageWidth+PAGETOPDISTANT));
  MessageGo(1,HHSCROLLMOVE,(GlobalPageHStart+myWindowXToUserX(WindowLeftIncrease)),
              (PageWidth+PAGETOPDISTANT));

  HandleUnlock(ItemGetHandle(GlobalCurrentPage));
  HScrollWindow=WindowGetChild(1);
  while (HScrollWindow)
  {
    if (WindowIsHScroll(HScrollWindow))
       break;
    else
       HScrollWindow=WindowGetNext(HScrollWindow);
  }
  if (HScrollWindow)
  {
     HScrollWidth=WindowGetWidth(HScrollWindow);
     HScrollWindow=WindowGetChild(HScrollWindow);
     while (HScrollWindow)
     {
       if (WindowIsHHScroll(HScrollWindow))
          break;
       else
          HScrollWindow=WindowGetNext(HScrollWindow);
     }
  }
  if (HScrollWindow)
     //MessageGo(HScrollWindow,WINDOWMOVE,(long)WindowLeftIncrease*GlobalPageScale
     //     *WindowWidth/PageWidth*(HScrollWidth-3*SYSSCROLLWIDTH)/HScrollWidth,0);
     MessageGo(HScrollWindow,WINDOWMOVE,(long)myWindowXToUserX(WindowLeftIncrease)
          *WindowWidth/PageWidth*(HScrollWidth-3*SYSSCROLLWIDTH)/HScrollWidth,0);
}

static void WindowYScroll(ORDINATETYPE WindowTopIncrease,int WindowHeight)
{
  Pages *MidPage;
  ORDINATETYPE PageHeight;
  HWND VScrollWindow;
  int VScrollWidth;

  MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
  PageHeight=PageGetPageHeight(MidPage);

  h_old_start=-10000;   // ByHance, 96,4.10

  //MessageGo(1,VVSCROLLMOVE,(GlobalPageVStart+WindowTopIncrease*GlobalPageScale),
  //          (PageHeight+PAGETOPDISTANT));
  MessageGo(1,VVSCROLLMOVE,(GlobalPageVStart+myWindowYToUserY(WindowTopIncrease)),
            (PageHeight+PAGETOPDISTANT));

  HandleUnlock(ItemGetHandle(GlobalCurrentPage));
  VScrollWindow=WindowGetChild(1);
  while (VScrollWindow)
  {
    if (WindowIsVScroll(VScrollWindow))
       break;
    else
       VScrollWindow=WindowGetNext(VScrollWindow);
  }
  if (VScrollWindow)
  {
     VScrollWidth=WindowGetHeight(VScrollWindow);
     VScrollWindow=WindowGetChild(VScrollWindow);
     while (VScrollWindow)
     {
       if (WindowIsVVScroll(VScrollWindow))
          break;
       else
          VScrollWindow=WindowGetNext(VScrollWindow);
     }
  }
  if (VScrollWindow)
     //MessageGo(VScrollWindow,WINDOWMOVE,0,(long)WindowTopIncrease*GlobalPageScale
     //     *WindowHeight/PageHeight*(VScrollWidth-3*SYSSCROLLWIDTH)/VScrollWidth);
     MessageGo(VScrollWindow,WINDOWMOVE,0,(long)myWindowYToUserY(WindowTopIncrease)
          *WindowHeight/PageHeight*(VScrollWidth-3*SYSSCROLLWIDTH)/VScrollWidth);
}

int CursorAdjusttoWindow(int CursorLeft,int CursorTop)
{                                      // Return 0 : No adjust
                                       //        1 : Adjust Left
                                       //        2 : Adjust top
                                       //        3 : Adjust Left and top
  int Left,Top,Right,Bottom;
  int Result;

  WindowGetRect(1,&Left,&Top,&Right,&Bottom);
  Result=0;
  if (CursorLeft<0)
  {
     Result|=1;
     WindowXScroll(-CURSORJUMPWINDOW,Right-Left); //CursorLeft-(Right-Left)/2,Right-Left);
     //WindowXScroll(CursorLeft-(Right-Left)/2,Right-Left);
  }
  else
     if (CursorLeft>=Right-Left)
     {
        Result|=1;
        WindowXScroll(CURSORJUMPWINDOW,Right-Left);//(Right-Left)/2-CursorLeft,Right-Left);
        //WindowXScroll((Right-Left)/2-CursorLeft,Right-Left);
     }
  if (CursorTop<0)
  {
     Result|=2;
     WindowYScroll(-CURSORJUMPWINDOW,Bottom-Top);//CursorTop-(Bottom-Top)/2,Bottom-Top);
     //WindowYScroll(CursorTop-(Bottom-Top)/2,Bottom-Top);
  }
  else
     if (CursorTop>=Bottom-Top)
     {
        Result|=2;
        WindowYScroll(CURSORJUMPWINDOW,Bottom-Top);//(Bottom-Top)/2-CursorTop,Bottom-Top);
        //WindowYScroll((Bottom-Top)/2-CursorTop,Bottom-Top);
     }
  return(Result);
}

int ToolExchange(int OldTool,int NewTool)
{
  switch (OldTool)
  {
    case IDX_INPUTBOX:
         TextCursorOff();
         BoxLeave(GlobalBoxHeadHandle);
         break;
    case IDX_SELECTBOX:
         if (NewTool!=IDX_ROTATE
             &&GlobalGroupGetSign())
         {
            GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
            GlobalUnGroup();
         }
         MoveSign=0;
         break;
    case IDX_ROTATE:
         if (NewTool!=IDX_SELECTBOX && GlobalGroupGetSign())
         {
            GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
            GlobalUnGroup();
         }
         RotateSign=0;
         break;
//    case IDX_ZOOM:
         // Clear menu's check sign
//       break;
    case IDX_LINK:
         if (NewTool!=IDX_UNLINK)
            DrawLinkedBox();       // RedrawUserField();
         LinkSign=0;
         break;
    case IDX_UNLINK:
         if (NewTool!=IDX_LINK)
            DrawLinkedBox();       // RedrawUserField();
         LinkSign=0;
         break;
  }
  // Hance 97/05/05
  if(OldTool==IDX_ZOOM)
      fInZoom=1;
  else
      fInZoom=0;


  switch (NewTool)
  {
    case IDX_INPUTBOX:
         BoxEnter(GlobalBoxHeadHandle);
         SetNewCursor();
         break;
    case IDX_TABLE:
    case IDX_TEXTBOX:
    case IDX_RECBOX:
    case IDX_CIRBOX:
    case IDX_ELIBOX:
    case IDX_PLGBOX:
    case IDX_LINE:
         BoxLeave(GlobalBoxHeadHandle);
         break;
    case IDX_LINK:
         if (OldTool!=IDX_UNLINK)
            DrawLinkedBox();       // RedrawUserField();
         break;
    case IDX_UNLINK:
         if (OldTool!=IDX_LINK)
            DrawLinkedBox();       // RedrawUserField();
         break;
  }
  return 0;
}

#ifdef VERSION10
int MenuToolChangeMethod(int ToolMethod)
{
  HMENU SelectMenu,MidMenu;

  SelectMenu=GetMenuFromReturnValue(ToolMethod);
  if (!SelectMenu)
     return(0);
  if (MenuIsChecked(SelectMenu))              // is the same menu item
     return(0);

  MidMenu=MenuGetChild(MenuGetFather(SelectMenu));    // 1st brother
  while (MidMenu)   // find menu item which is checked attrib, set it noChecked
  {
    if (MenuIsChecked(MidMenu))
    {
       MenuSetNoCheckedAttr(MidMenu);
       break;
    }
    MidMenu=MenuGetNext(MidMenu);
  }

  MenuSetCheckedAttr(SelectMenu);
}
#endif      // Version1.0

int MenuScaleChangeMethod(int ToolMethod)
{
  HMENU SelectMenu,MidMenu;

  SelectMenu=GetMenuFromReturnValue(ToolMethod);
  if (!SelectMenu)
     return(0);
  if (!MenuIsChecked(SelectMenu))
  {
     MidMenu=MenuGetChild(MenuGetFather(SelectMenu));
     while (!MenuIsSpaceBar(MidMenu))
     {
       if (MenuIsChecked(MidMenu))
       {
          MenuSetNoCheckedAttr(MidMenu);
          break;
       }
       MidMenu=MenuGetNext(MidMenu);
     }
     MenuSetCheckedAttr(SelectMenu);
  }
  // else  return(0);

  return(1);
}

void MenuScaleZoomChange(int ZoomFlag)      // 0= *2,  1= /2
{
   int method=0;

   switch(GlobalPageScale)  {
      case 10:         // 100%
         if(ZoomFlag) method=MENU_VIEWPORTHALF;         // 50%
         else method=MENU_VIEWPORTDOUBLE;               // 200%
         break;
          // 13: 75% ---- 37.5% or 150% : nothing to do
      case 20:         // 50%
         if(!ZoomFlag) method=MENU_VIEWPORTACTUAL;       // 100%
                                                       // else 25%
         break;
       case 5:          // 200%
         if(ZoomFlag) method=MENU_VIEWPORTACTUAL;       // 100%
                                                       // else 400%
         break;
       case 40:        // 25% ---- min view port
         if(!ZoomFlag) method=MENU_VIEWPORTHALF;
         break;
   } // switch

   if(!method)
        method=MENU_VIEWPORTQUART;       // use 75% as a flag

   MenuScaleChangeMethod(method);
   if(method==MENU_VIEWPORTQUART) {
       HMENU SelectMenu;
       SelectMenu=GetMenuFromReturnValue(method);
       if (!SelectMenu)
           return;
       MenuSetNoCheckedAttr(SelectMenu);
   }
}

void DrawRotateAxis(HWND Window)
{
  struct viewporttype SaveViewPort;
  int Left,Top,Right,Bottom,AxisX,AxisY,TotalPoints,i,RPoints[2*100];
  int SaveColor;
  TextBoxs *MidBox;

  if (GlobalGroupGetSign())
  {
     AxisX=UserXToWindowX(GlobalGroupGetRotateAxisX()+GlobalGroupGetLeft());
     AxisY=UserYToWindowY(GlobalGroupGetRotateAxisY()+GlobalGroupGetTop());
  }
  else
  {
     if (!GlobalBoxHeadHandle)
        return;
     MidBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
     if (MidBox==NULL)
        return;
     AxisX=UserXToWindowX(TextBoxGetRotateAxisX(MidBox)
           +TextBoxGetBoxLeft(MidBox));
     AxisY=UserYToWindowY(TextBoxGetRotateAxisY(MidBox)
           +TextBoxGetBoxTop(MidBox));
     HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
  }
  ArctoLine(AxisX,AxisY,5,5,0,360,&TotalPoints,RPoints,COMPUTEANGLEINC);

  MouseHidden();
  getviewsettings(&SaveViewPort);
  SaveColor=getcolor();
  WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
  setviewport(Left,Top,Right,Bottom,1);
  setcolor(EGA_BLACK);
  moveto(RPoints[0],RPoints[1]);
  for (i=1;i<TotalPoints;i++)
      lineto(RPoints[2*i],RPoints[2*i+1]);
  lineto(RPoints[0],RPoints[1]);
  setwritemode(XOR_PUT);
  setcolor(EGA_WHITE);
  line(AxisX-5,AxisY,AxisX+4,AxisY);
  line(AxisX,AxisY-5,AxisX,AxisY+4);
  setwritemode(COPY_PUT);
  setviewport(SaveViewPort.left,SaveViewPort.top,SaveViewPort.right,
              SaveViewPort.bottom,SaveViewPort.clip);
  setcolor(SaveColor);
  MouseShow();
}

void DrawCreatingPolygon(int PolygonNumber,ORDINATETYPE *PolygonEdges)
{
  int i;

  if (PolygonNumber)
  {
     //moveto(PolygonEdges[0]-GlobalPageHStart/GlobalPageScale,
     //       PolygonEdges[1]-GlobalPageVStart/GlobalPageScale);
     moveto(PolygonEdges[0]-myUserXToWindowX(GlobalPageHStart),
            PolygonEdges[1]-myUserYToWindowY(GlobalPageVStart));
     for (i=1;i<PolygonNumber;i++)
     {
         //lineto(PolygonEdges[2*i]-GlobalPageHStart/GlobalPageScale,
         //       PolygonEdges[2*i+1]-GlobalPageVStart/GlobalPageScale);
         lineto(PolygonEdges[2*i]-myUserXToWindowX(GlobalPageHStart),
                PolygonEdges[2*i+1]-myUserYToWindowY(GlobalPageVStart));
     }
  }
}

int PointInLine(int X,int Y,int X1,int Y1,int X2,int Y2)
{
  int MinX,MaxX,MinY,MaxY;

  if (X1<X2)
  {
     MinX=X1;
     MaxX=X2;
  }
  else
  {
     MinX=X2;
     MaxX=X1;
  }
  if (Y1<Y2)
  {
     MinY=Y1;
     MaxY=Y2;
  }
  else
  {
     MinY=Y2;
     MaxY=Y1;
  }

  if (X1==X2)
     return(abs(X-X1)<DELTASIZE && (Y>=MinY && Y<MaxY));

  if (Y1==Y2)
     return(abs(Y-Y1)<DELTASIZE && (X>=MinX && X<MaxX));

  return(sqrt(labs( X-X1)*labs( X-X1)+labs( Y-Y1)*labs( Y-Y1))
        +sqrt(labs( X-X2)*labs( X-X2)+labs( Y-Y2)*labs( Y-Y2))
        -sqrt(labs(X2-X1)*labs(X2-X1)+labs(Y2-Y1)*labs(Y2-Y1))
        < DELTASIZE );
}

static void TableBoxDrawCellVirtualEdge(FormBoxs *FormBox,
                     int Left,int Top,int Right,int Bottom)
{
  struct viewporttype TmpViewPort;
  int SaveColor;
  int WindowLeft,WindowTop,WindowRight,WindowBottom;
  // int RotateAngle,RotateAxisX,RotateAxisY;
  unsigned old_style;

  MouseHidden();
  getviewsettings(&TmpViewPort);
  WindowGetRect(1,&WindowLeft,&WindowTop,&WindowRight,&WindowBottom);
  setviewport(WindowLeft,WindowTop,WindowRight,WindowBottom,1);
  SaveColor=getcolor();
  setcolor(EGA_WHITE);

#ifdef __TURBOC__
  struct linesettingstype SaveLineStyle;
  getlinesettings(&SaveLineStyle);
  setlinestyle(4,0x5555,1);
#else
  old_style=getlinestyle();
  setlinestyle(0x5555);
#endif

  setwritemode(XOR_PUT);

  Left+=TableBoxGetBoxLeft(FormBox);
  Top+=TableBoxGetBoxTop(FormBox);
  Right+=TableBoxGetBoxLeft(FormBox);
  Bottom+=TableBoxGetBoxTop(FormBox);
  if (TableBoxGetRotateAngle(FormBox))
  {
     Rotate(&Left,&Top,Left,Top,TableBoxGetRotateAxisX(FormBox)+TableBoxGetBoxLeft(FormBox),
            TableBoxGetRotateAxisY(FormBox)+TableBoxGetBoxTop(FormBox),
            TableBoxGetRotateAngle(FormBox));
     Rotate(&Right,&Bottom,Right,Bottom,
            TableBoxGetRotateAxisX(FormBox)+TableBoxGetBoxLeft(FormBox),
            TableBoxGetRotateAxisY(FormBox)+TableBoxGetBoxTop(FormBox),
            TableBoxGetRotateAngle(FormBox));
  }
  Left=UserXToWindowX(Left);
  Right=UserXToWindowX(Right);
  Top=UserYToWindowY(Top);
  Bottom=UserYToWindowY(Bottom);
  line(Left,Top,Right,Bottom);

  setwritemode(COPY_PUT);

#ifdef __TURBOC__
  setlinestyle(SaveLineStyle.linestyle,SaveLineStyle.upattern,
               SaveLineStyle.thickness);
#else
  setlinestyle(old_style);
#endif

  setcolor(SaveColor);
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  MouseShow();
}

int BoxChangeBorder(HBOX SelectBox,unsigned long LastParam1,unsigned long Param2)
{
  TextBoxs *TextBox;
  int MouseX,MouseY,BoxDots,ResizeSign=0,XInc,YInc;
  ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
  int w;

  if(SelectBox<=0)
     ReturnOK();

  TextBox=HandleLock(ItemGetHandle(SelectBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  if (BoxIsLocked(TextBox))
  {
     HandleUnlock(ItemGetHandle(SelectBox));
     ReturnOK();
  }

  MouseX=WindowXToUserX((short)MAKEHI(LastParam1));
  MouseY=WindowYToUserY((short)MAKELO(LastParam1));
  if (TextBoxGetRotateAngle(TextBox))
  {
     int MouseX2,MouseY2;

     Rotate(&MouseX,&MouseY,MouseX,MouseY,
            TextBoxGetBoxLeft(TextBox)+TextBoxGetRotateAxisX(TextBox),
            TextBoxGetBoxTop(TextBox)+TextBoxGetRotateAxisY(TextBox),
            -TextBoxGetRotateAngle(TextBox));
     MouseX2=WindowXToUserX((short)MAKEHI(Param2));
     MouseY2=WindowYToUserY((short)MAKELO(Param2));
     Rotate(&MouseX2,&MouseY2,MouseX2,MouseY2,
            TextBoxGetBoxLeft(TextBox)+TextBoxGetRotateAxisX(TextBox),
            TextBoxGetBoxTop(TextBox)+TextBoxGetRotateAxisY(TextBox),
            -TextBoxGetRotateAngle(TextBox));
     XInc=MouseX2-MouseX;
     YInc=MouseY2-MouseY;
  }
  else
  {
     XInc=WindowXToUserX((short)MAKEHI(Param2))-MouseX;
     YInc=WindowYToUserY((short)MAKELO(Param2))-MouseY;
  }

  if (TextBoxGetBoxType(TextBox)==TEXTBOX||
      TextBoxGetBoxType(TextBox)==TABLEBOX||
      TextBoxGetBoxType(TextBox)==LINEBOX||
      TextBoxGetBoxType(TextBox)==RECTANGLEPICTUREBOX||
      TextBoxGetBoxType(TextBox)==CORNERPICTUREBOX||
      TextBoxGetBoxType(TextBox)==ELIPSEPICTUREBOX)
  {
     // Box Left Edge
     if (abs(MouseX-TextBoxGetBoxLeft(TextBox))<DELTASIZE
      && TextBoxGetBoxWidth(TextBox)-XInc>0
      && !TextBoxGetRotateAngle(TextBox))
     {
        ResizeSign=1;
        UndoInsertBoxMove(TextBoxGetBoxLeft(TextBox),
                          TextBoxGetBoxTop(TextBox));
        UndoInsertBoxResize(TextBoxGetBoxWidth(TextBox),
                            TextBoxGetBoxHeight(TextBox));
        BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
        TextBoxSetBoxLeft(TextBox,TextBoxGetBoxLeft(TextBox)+XInc);

        // New in 1996,3,29
        if (TextBoxGetBoxType(TextBox)==TABLEBOX)
        {
            FBPlusVertLine(SelectBox,0,-XInc);
            TableBoxSetBoxWidth(TextBox,TextBoxGetBoxWidth(TextBox)-XInc);
            if (TableBoxGetMinWidth((PFormBoxs)TextBox)>TextBoxGetBoxWidth(TextBox))
            {
                TableBoxSetBoxWidth(TextBox,TableBoxGetMinWidth((PFormBoxs)TextBox));
                BoxChangeAll(GlobalCurrentPage);
            }
            //ReFormatTableText(SelectBox,TRUE);
            CellisMoving=2;
        }
        else
           TextBoxSetBoxWidth(TextBox,TextBoxGetBoxWidth(TextBox)-XInc);
        // New End

        BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
     }

     // Box Right Edge
     if (abs(MouseX-TextBoxGetBoxRight(TextBox))<DELTASIZE
       && TextBoxGetBoxWidth(TextBox)+XInc>0
       //&& TextBoxGetBoxType(TextBox)!=TABLEBOX         // ByDg
     ) {
        ResizeSign=1;
        UndoInsertBoxMove(TextBoxGetBoxLeft(TextBox),
                          TextBoxGetBoxTop(TextBox));
        UndoInsertBoxResize(TextBoxGetBoxWidth(TextBox),
                            TextBoxGetBoxHeight(TextBox));
        BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);

        // New
        if (TextBoxGetBoxType(TextBox)==TABLEBOX)
        {
            TextBoxSetBoxWidth(TextBox,TextBoxGetBoxWidth(TextBox)+XInc);
            w=AdjustTableBoxRightVertline((PFormBoxs)TextBox);
            if (w>0)
                FBPlusVertLine(SelectBox,0,-w);

            //ReFormatTableText(SelectBox,TRUE);
            CellisMoving=3;
        }
        else
            TextBoxSetBoxWidth(TextBox,TextBoxGetBoxWidth(TextBox)+XInc);

        BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
     }

     if (TextBoxGetBoxType(TextBox)!=LINEBOX)
     {
        // Box Top Edge
        if (abs(MouseY-TextBoxGetBoxTop(TextBox))<DELTASIZE
         && TextBoxGetBoxHeight(TextBox)-YInc>0
         && !TextBoxGetRotateAngle(TextBox))
        {
           ResizeSign=1;
           UndoInsertBoxMove(TextBoxGetBoxLeft(TextBox),
                             TextBoxGetBoxTop(TextBox));
           UndoInsertBoxResize(TextBoxGetBoxWidth(TextBox),
                               TextBoxGetBoxHeight(TextBox));
           BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
           TextBoxSetBoxTop(TextBox,TextBoxGetBoxTop(TextBox)+YInc);

           // New in 1996,3,29
           if (TextBoxGetBoxType(TextBox)==TABLEBOX)
           {
                FBPlusHoriLine(SelectBox,0,-YInc);
                TableBoxSetBoxHeight(TextBox,TextBoxGetBoxHeight(TextBox)-YInc);
                if (TableBoxGetMinHeight((PFormBoxs)TextBox)>TextBoxGetBoxHeight(TextBox))
                {
                    TableBoxSetBoxHeight((PFormBoxs)TextBox,TableBoxGetMinHeight((PFormBoxs)TextBox));
                    BoxChangeAll(GlobalCurrentPage);
                }
                //ReFormatTableText(SelectBox,TRUE);
                CellisMoving=4;
           }
           else
                TextBoxSetBoxHeight(TextBox,TextBoxGetBoxHeight(TextBox)-YInc);
           // New End

           BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
        }

        // Box Bottom Edge
        if (abs(MouseY-TextBoxGetBoxBottom(TextBox))<DELTASIZE
          && TextBoxGetBoxHeight(TextBox)+YInc>0 )
        {
           ResizeSign=1;
           UndoInsertBoxMove(TextBoxGetBoxLeft(TextBox),
                             TextBoxGetBoxTop(TextBox));
           UndoInsertBoxResize(TextBoxGetBoxWidth(TextBox),
                               TextBoxGetBoxHeight(TextBox));
           BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
           TextBoxSetBoxHeight(TextBox,TextBoxGetBoxHeight(TextBox)+YInc);

           // New
           if (TextBoxGetBoxType(TextBox)==TABLEBOX)
           {
                w=AdjustTableBoxBottomHoriline((PFormBoxs)TextBox);
                if(w>0)
                    FBPlusHoriLine(SelectBox,0,-w);

                //ReFormatTableText(SelectBox,TRUE);
                CellisMoving=5;
           }

           BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
        }
     }

     if (TextBoxGetBoxType(TextBox)==TABLEBOX)
     {          // maybe change cell
        int FormCellNumber,Left,Top,Right,Bottom,Result;

        MouseX-=TextBoxGetBoxLeft(TextBox);
        MouseY-=TextBoxGetBoxTop(TextBox);
        FormCellNumber=FBCellofXY(SelectBox,MouseX,MouseY);
        if (GlobalTableCell<0)
           GlobalTableCell=0;
        if (FormCellNumber>=0&&FormCellNumber==GlobalTableCell)
        {
           FBGetCellRect(SelectBox,FormCellNumber,&Left,&Top,&Right,&Bottom);
      //#define OLD_VERSION
      #ifndef OLD_VERSION
           // In Cells. New int 1996,3
           if( CellisMoving==1
           || (abs(MouseX-Left)>DELTASIZE && abs(MouseX-Right)>DELTASIZE
              && abs(MouseY-Top)>DELTASIZE && abs(MouseY-Bottom)>DELTASIZE)
           ) {
              ResizeSign=1;
              Result=FBPlusVertLine(SelectBox,0,XInc);
              if (Result>=0)
              {
                 /*--------- ByHance, 96,4.3 ------
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Left,0,
                                             Left,TextBoxGetBoxHeight(TextBox));
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Left+XInc,0,
                                             Left+XInc,TextBoxGetBoxHeight(TextBox));
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Right,0,
                                             Right,TextBoxGetBoxHeight(TextBox));
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Right+XInc,0,
                                             Right+XInc,TextBoxGetBoxHeight(TextBox));
                  ----------------------*/
                 /*--------- ByHance, 96,4.3 ------
                 Left=TableBoxGetMinVertline((PFormBoxs)TextBox);
                 Right=TableBoxGetMaxWidth((PFormBoxs)TextBox);
                  ----------------------*/

                 CellisMoving=1;
                 Top=TableBoxGetMinHortline((PFormBoxs)TextBox);
                 Bottom=TableBoxGetMaxHeight((PFormBoxs)TextBox);
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Left,Top,Left,Bottom );
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Left+XInc,Top,Left+XInc,Bottom);
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Right,Top,Right,Bottom);
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Right+XInc,Top,Right+XInc,Bottom);

                 if (TextBoxGetBoxWidth(TextBox)<TableBoxGetMaxWidth((PFormBoxs)TextBox))
                 {
                     TextBoxSetBoxWidth(TextBox,TableBoxGetMaxWidth((PFormBoxs)TextBox));
                     BoxChangeAll(GlobalCurrentPage);
                 }
                 // ReFormatTableText(SelectBox,TRUE);
              }
           }
           else
       #endif

           // Cell Left Edge
           if (abs(MouseX-Left)<DELTASIZE)
           {
              ResizeSign=1;
              // Add a Undo
              UndoInsertTableColMove(TableColOfX(SelectBox,Left),Left);
              Result=FBChangeCellLeftLine(SelectBox,FormCellNumber,Left+XInc);
              // left, Nees Top & Bottom
              if (Result>=0)
              {
                 //BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                 /*--------- ByHance, 96,4.3 ------
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Left,0,
                                             Left,TextBoxGetBoxHeight(TextBox));
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Left+XInc,0,
                                             Left+XInc,TextBoxGetBoxHeight(TextBox));
                  ----------------------*/
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Left,TableBoxGetMinHortline((PFormBoxs)TextBox),
                         Left,TableBoxGetMaxHeight((PFormBoxs)TextBox) );
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Left+XInc,TableBoxGetMinHortline((PFormBoxs)TextBox),
                         Left+XInc,TableBoxGetMaxHeight((PFormBoxs)TextBox) );
                 if (TextBoxGetBoxWidth(TextBox)<TableBoxGetMaxWidth((PFormBoxs)TextBox))
                     TextBoxSetBoxWidth(TextBox,TextBoxGetBoxWidth(TextBox)+XInc);
                 //BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                 CellisMoving=6;
              }
           }
           else
           // Cell Top Edge
           if(abs(MouseY-Top)<DELTASIZE)
           {
              ResizeSign=1;
              UndoInsertTableLineMove(TableLineOfY(SelectBox,Top),Top);
              Result=FBChangeCellTopLine(SelectBox,FormCellNumber,Top+YInc);
              // TOP, Nees left & right
              if (Result>=0)
              {
                 //BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                 TextBoxSetBoxHeight(TextBox,TextBoxGetBoxHeight(TextBox)+YInc);
                 /*--------- ByHance, 96,4.3 ------
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,0,Top,
                                             TextBoxGetBoxWidth(TextBox),Top);
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,0,Top+YInc,
                                             TextBoxGetBoxWidth(TextBox),Top+YInc);
                  ----------------------*/
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         TableBoxGetMinVertline((PFormBoxs)TextBox), Top,
                         TableBoxGetMaxWidth((PFormBoxs)TextBox), Top );
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         TableBoxGetMinVertline((PFormBoxs)TextBox), Top+YInc,
                         TableBoxGetMaxWidth((PFormBoxs)TextBox), Top+YInc );
                // BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                 CellisMoving=7;
              }
           }
           else
           // Cell Right Edge
           if(abs(MouseX-Right)<DELTASIZE)
           {
              ResizeSign=1;
              UndoInsertTableColMove(TableColOfX(SelectBox,Right),Right);
              Result=FBChangeCellRightLine(SelectBox,FormCellNumber,Right+XInc);
              if (Result>=0)
              {
                 //BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                 /*--------- ByHance, 96,4.3 ------
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Right,0,
                                             Right,TextBoxGetBoxHeight(TextBox));
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Right+XInc,0,
                                             Right+XInc,TextBoxGetBoxHeight(TextBox));
                  ----------------------*/
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Right,TableBoxGetMinHortline((PFormBoxs)TextBox),
                         Right,TableBoxGetMaxHeight((PFormBoxs)TextBox) );
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Right+XInc,TableBoxGetMinHortline((PFormBoxs)TextBox),
                         Right+XInc,TableBoxGetMaxHeight((PFormBoxs)TextBox) );

                 if (TextBoxGetBoxWidth(TextBox)<TableBoxGetMaxWidth((PFormBoxs)TextBox))
                     TextBoxSetBoxWidth(TextBox,TextBoxGetBoxWidth(TextBox)+XInc);
                 //BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                 CellisMoving=8;
              }
           }
           else
           // Cell Bottom Edge
           if(abs(MouseY-Bottom)<DELTASIZE)
           {
              ResizeSign=1;
              UndoInsertTableLineMove(TableLineOfY(SelectBox,Bottom),Bottom);
              Result=FBChangeCellBottomLine(SelectBox,FormCellNumber,Bottom+YInc);
              if (Result>=0)
              {
                 //BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                 TextBoxSetBoxHeight(TextBox,TextBoxGetBoxHeight(TextBox)+YInc);
                 /*--------- ByHance, 96,4.3 ------
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,0,Bottom,
                                             TextBoxGetBoxWidth(TextBox),Bottom);
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,0,Bottom+YInc,
                                             TextBoxGetBoxWidth(TextBox),Bottom+YInc);
                  ----------------------*/
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         TableBoxGetMinVertline((PFormBoxs)TextBox),Bottom,
                         TableBoxGetMaxWidth((PFormBoxs)TextBox),Bottom );
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         TableBoxGetMinVertline((PFormBoxs)TextBox),Bottom+YInc,
                         TableBoxGetMaxWidth((PFormBoxs)TextBox),Bottom+YInc );
                 //BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                 CellisMoving=9;
              }
           }
       #ifdef OLD_VERSION
           else
           // In Cells. New int 1996,3
           /*---------------------------------
           if(TableBoxGetMinVertline((PFormBoxs)TextBox)<MouseX
           && MouseX<TableBoxGetMaxWidth((PFormBoxs)TextBox)
           && TableBoxGetMinHortline((PFormBoxs)TextBox)<MouseY
           && MouseY<TableBoxGetMaxHeight((PFormBoxs)TextBox))
           ---------------------------*/
           {
              ResizeSign=1;
              Result=FBPlusVertLine(SelectBox,0,XInc);
              if (Result>=0)
              {
                 /*--------- ByHance, 96,4.3 ------
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Left,0,
                                             Left,TextBoxGetBoxHeight(TextBox));
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Left+XInc,0,
                                             Left+XInc,TextBoxGetBoxHeight(TextBox));
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Right,0,
                                             Right,TextBoxGetBoxHeight(TextBox));
                 TableBoxDrawCellVirtualEdge((FormBoxs *)TextBox,Right+XInc,0,
                                             Right+XInc,TextBoxGetBoxHeight(TextBox));
                  ----------------------*/

                 Top=TableBoxGetMinHortline((PFormBoxs)TextBox);
                 Bottom=TableBoxGetMaxHeight((PFormBoxs)TextBox);
                 /*--------- ByHance, 96,4.3 ------
                 Left=TableBoxGetMinVertline((PFormBoxs)TextBox);
                 Right=TableBoxGetMaxWidth((PFormBoxs)TextBox);
                  ----------------------*/
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Left,Top,Left,Bottom );
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Left+XInc,Top,Left+XInc,Bottom);
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Right,Top,Right,Bottom);
                 TableBoxDrawCellVirtualEdge((PFormBoxs)TextBox,
                         Right+XInc,Top,Right+XInc,Bottom);

                 if (TextBoxGetBoxWidth(TextBox)<TableBoxGetMaxWidth((PFormBoxs)TextBox))
                     TextBoxSetBoxWidth(TextBox,TableBoxGetMaxWidth((PFormBoxs)TextBox));
                 ReFormatTableText(SelectBox,TRUE);
              }
           }
       #endif   // OLD_VERSION
        } // FormCellNumber>=0
     }  //-- TABLEBOX --
  }
  else
  if (TextBoxGetBoxType(TextBox)==POLYGONPICTUREBOX)
  {
     int i;
     ORDINATETYPE *PolygonDots;

     BoxGetPolygonDrawBorder((Boxs *)TextBox,&BoxDots,BoxXY);
     PolygonDots=HandleLock(PictureBoxGetBorderPolygon(TextBox));
     if (PolygonDots!=NULL)
     {
        for (i=0;i<BoxDots;i++)           // is it at Point ?
        {
          if (abs(MouseX-BoxXY[2*i])<DELTASIZE
           && abs(MouseY-BoxXY[2*i+1])<DELTASIZE)
          {                         // Points
             ResizeSign=i+1;
             BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
             PolygonDots[2*i+1]+=XInc;
             PolygonDots[2*i+2]+=YInc;
             break;                  // ByHance, 95,12.10
          }
        }

        if (!ResizeSign)         // is it in line ?
        {
           for (i=0;i<BoxDots-1;i++)
             if (PointInLine(MouseX,MouseY,BoxXY[2*i],
                  BoxXY[2*i+1],BoxXY[2*i+2],BoxXY[2*i+3]))
             {                      // Edges
                ResizeSign=i+1;
                BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                PolygonDots[2*i+1]+=XInc;
                PolygonDots[2*i+2]+=YInc;
                PolygonDots[2*i+3]+=XInc;
                PolygonDots[2*i+4]+=YInc;
                break;                  // ByHance, 95,12.10
             }
        }

        if (!ResizeSign&&PointInLine(MouseX,MouseY,BoxXY[2*(BoxDots-1)],
             BoxXY[2*(BoxDots-1)+1],BoxXY[0],BoxXY[1]))
        {                           // The last edge
           ResizeSign=BoxDots;
           BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
           PolygonDots[2*(BoxDots-1)+1]+=XInc;
           PolygonDots[2*(BoxDots-1)+2]+=YInc;
           PolygonDots[1]+=XInc;
           PolygonDots[2]+=YInc;
        }

        if (ResizeSign)
        {
           int NewLeft,NewTop,NewRight,NewBottom;

       PolygonGetMinRectangle(BoxDots,&PolygonDots[1],&NewLeft, //ByHance
//         PolygonGetMinRectangle(BoxDots,&PolygonDots[0],&NewLeft,
                                  &NewTop,&NewRight,&NewBottom);
           PictureBoxSetBoxLeft(TextBox,NewLeft);
           PictureBoxSetBoxTop(TextBox,NewTop);
           PictureBoxSetBoxWidth(TextBox,NewRight-NewLeft);
           PictureBoxSetBoxHeight(TextBox,NewBottom-NewTop);
           BoxDrawBorder(SelectBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
        }
        HandleLock(PictureBoxGetBorderPolygon(TextBox));
     }
  }

  HandleUnlock(ItemGetHandle(SelectBox));
  if (ResizeSign)
     FileSetModified();
  return(ResizeSign);
}

void UserFunctionInitial(HWND Window)
{
  TextCursor=TextCreatCursor(Window,CHARHEIGHT,0,0,0);
  CursorMoveTo(0,CHARHEIGHT);
  UndoInitial();
}

void UserFunctionFinish(void)
{
  if (TextCursor>=0)
     TextDestroyCursor(&TextCursor);
  UndoFinish();
}

void RedrawUserField(void)
{
  int Left,Top;

  Left=LINESPACE;
  Top=LINESPACE+2*SYSBUTTONWIDTH+1;
  if (ToolBarHasToolBar())
     Top+=TOOLBARHEIGHT+3*LINESPACE;
  if (ToolBarHasRulerBar())
  {
     Left+=RULERBARHEIGHT+1;
     Top+=RULERBARHEIGHT+1;
  }
  MessageInsert(1,DRAWWINDOW,MAKELONG(Left,Top),
                MAKELONG(WindowGetWidth(1)-2*LINESPACE-SYSSCROLLWIDTH,
                WindowGetHeight(1)-2*LINESPACE-SYSBUTTONWIDTH+2));
//  TellStatus();
}

void DrawToolBar(HWND Window,int DrawLeft,int DrawTop,int DrawRight,
                 int DrawBottom,char SelectSign)
{
  struct viewporttype TmpViewPort;
  int WindowLeft,WindowTop,WindowRight,WindowBottom;
  int SaveColor;

  WindowGetRealRect(Window,&WindowLeft,&WindowTop,&WindowRight,&WindowBottom);
  if (DrawLeft>=WindowGetWidth(Window)
      ||DrawTop>=WindowGetHeight(Window)
      ||DrawRight<0||DrawBottom<0)
     return;

  if (DrawLeft<0)
     DrawLeft=0;
  if (DrawTop<0)
     DrawTop=0;
  if (DrawRight>WindowGetWidth(Window)-1)
     DrawRight=WindowGetWidth(Window)-1;
  if (DrawBottom>WindowGetHeight(Window)-1)
     DrawBottom=WindowGetHeight(Window)-1;

  DrawLeft+=WindowLeft;
  DrawTop+=WindowTop;
  DrawRight+=WindowLeft;
  DrawBottom+=WindowTop;

  WindowLeft-=DrawLeft;
  WindowTop-=DrawTop;
  WindowRight-=DrawLeft;
  WindowBottom-=DrawTop;

  MouseHidden();
  getviewsettings(&TmpViewPort);
  SaveColor=getcolor();
  setviewport(0,0,getmaxx(),getmaxy(),1);

  DrawUserButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
                 WindowLeft,WindowTop,WindowRight,WindowBottom,
                 (SelectSign?1:0)|(WindowGetStyle(Window)&0x180),
                 WindowGetTitle(Window));
         // SelectSign?1:0,WindowGetTitle(Window)); // ByHance, for icon style

  setcolor(SaveColor);
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  MouseShow();
}

static HWND SearchToolBarWindow(HWND Window,int Order);

static long BackToolBarProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    //case GETFOCUS:
    //   break;
    case MOUSELEFTDOWN:
         WindowDefaultProcedure(Window,Message,Param1,Param2);
         MessageInsert(1,GETFOCUS,0,0);     // ByHance, 95,11.25
         break;
    case MOUSEMOVE:
         if(BubleHintIdx) {
            MessageGo(Window,DELBUBLE,0L,0L);  // delete old hint window
            BubleHintIdx=0xff;            // in tool bar menu
         }
         ToolBarMouseMove(Window,Message,Param1,Param2);
         break;
    case TOOLCHANGE:
         {
           HWND MidWindow;

           MidWindow=SearchToolBarWindow(Window,GlobalBoxTool);
           GlobalBoxTool=Param1;
           MessageInsert(MidWindow,DRAWWINDOW,0l,
                         MAKELONG(WindowGetWidth(MidWindow),
                                  WindowGetHeight(MidWindow)));
           MidWindow=SearchToolBarWindow(Window,GlobalBoxTool);
           MessageInsert(MidWindow,DRAWWINDOW,0l,
                         MAKELONG(WindowGetWidth(MidWindow),
                                  WindowGetHeight(MidWindow)));
         }
         break;
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

long ToolBarProcedure1(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case MOUSEMOVE:
         //ToolMouseMove(Window,Message,Param1,Param2);
         if(BubleHintIdx) {
            int idx=WindowGetUserData(Window);
            int mx,my;
            if(idx+1==BubleHintIdx) break;       // same window
            MouseGetPosition(&mx,&my);
            MessageGo(Window,DELBUBLE,0L,0L);  // delete old hint window
            MessageGo(Window,BUBLEHINT,MAKELONG(mx,my),Window);   // new
         }
         break;
    default:
        break;
  }
  return ButtonDefaultProc(Window,Message,Param1,Param2);
}


long ToolBarProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int idx;
  switch (Message)
  {
    case DRAWWINDOW:
         DrawToolBar(Window,MAKEHI(Param1),MAKELO(Param1),MAKEHI(Param2),
                     MAKELO(Param2),GlobalBoxTool==WindowGetUserData(Window));
         break;
    case MOUSEMOVE:
         if(BubleHintIdx) {
            int mx,my;
            idx=WindowGetUserData(Window);
            if(idx+1==BubleHintIdx) break;       // same window
            MouseGetPosition(&mx,&my);
            MessageGo(Window,DELBUBLE,0L,0L);  // delete old hint window
            MessageGo(Window,BUBLEHINT,MAKELONG(mx,my),Window);   // new
         }
         return( ButtonDefaultProc(Window,Message,Param1,Param2) );
         break;
    case MOUSELEFTDOWN:
         {
           // HWND MidWindow;          // Do What ?? deleted ByHance
           // MidWindow=SearchToolBarWindow(WindowGetFather(Window),GlobalBoxTool);
//           MessageInsert(WindowGetFather(WindowGetFather(Window)),GETFOCUS,0,0);
           if (GlobalBoxTool==WindowGetUserData(Window))
              break;
           idx=WindowGetUserData(Window);
           MessageInsert(WindowGetFather(WindowGetFather(Window)),MENUCOMMAND,
             IconMenuIdxArr[idx],0);
         }
         break;
    case MOUSELEFTUP:
         MessageGo(WindowGetFather(WindowGetFather(Window)),GETFOCUS,0,0);
         break;                         // ByHance, 95,12.4
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static HWND SearchChildWindowByFunction(HWND Window,Function *WindowProcedure)
{
  HWND MidWindow;

  MidWindow=WindowGetChild(Window);
  while (MidWindow)
  {
    if (WindowGetProcedure(MidWindow)==(Function *)WindowProcedure)
       break;
    MidWindow=WindowGetNext(MidWindow);
  }
  return(MidWindow);
}

HWND SearchBackToolBarWindow(HWND Window)
{
  return(SearchChildWindowByFunction(Window,(Function *)BackToolBarProcedure));
}

static HWND SearchToolBarWindow(HWND Window,int Order)
{
  HWND MidWindow;

  MidWindow=WindowGetChild(Window);

  while (MidWindow)
  {
    if (WindowGetProcedure(MidWindow)==(Function *)ToolBarProcedure
        &&WindowGetUserData(MidWindow)==Order)
       break;
    MidWindow=WindowGetNext(MidWindow);
  }
  return(MidWindow);
}

static int CreatToolBarWindow(int Left,int Top,int Width,int Height,int ButtonReturn,
                       char *ButtonTitle,ULONG Style,HWND FatherWindow)
{
  Windows ButtonWindow;

  memset(&ButtonWindow,0,sizeof(ButtonWindow));

  ButtonWindow.Left=Left;
  ButtonWindow.Top=Top;
  ButtonWindow.Right=Left+Width;
  ButtonWindow.Bottom=Top+Height;
  strcpy(ButtonWindow.Title,ButtonTitle);
  ButtonWindow.UserData=ButtonReturn;
  ButtonWindow.WindowStyle=Style;

  if( Style & WindowSetIconMustBeUp() )
     ButtonWindow.Procedure=(Function *)ToolBarProcedure1;
  else
     ButtonWindow.Procedure=(Function *)ToolBarProcedure;

  return(WindowAppend(&ButtonWindow,FatherWindow));
}

void ToolBarWindowModify(HWND Window)
{
  HMENU MidMenu;
  HWND MidWindow;

  h_old_start=v_old_start=-10000;
  MidMenu=GetMenuFromReturnValue(MENU_VIEWTOOLS);
  if (ToolBarHasToolBar())         // now, hide toolbar
  {
     int WindowHeight;

     MidWindow=SearchBackToolBarWindow(Window);
     if (!MidWindow)
        return;
     else
     {
        WindowHeight=WindowGetHeight(MidWindow);
        WindowDelete(MidWindow);
     }

     ToolBarSetHasNoToolBar();
     MenuSetNoCheckedAttr(MidMenu);

     if (ToolBarHasRulerBar())       // move ruler to menu's below
     {
        MidWindow=SearchHClibrationWindow(Window);
        if (MidWindow)
        {
           WindowSetTop(MidWindow,WindowGetTop(MidWindow)-WindowHeight+1);
           WindowSetBottom(MidWindow,WindowGetBottom(MidWindow)-WindowHeight+1);
        }
        MidWindow=SearchVClibrationWindow(Window);
        if (MidWindow)
           WindowSetTop(MidWindow,WindowGetTop(MidWindow)-WindowHeight+1);
     }
  }
  else                        // no toolbar, display it
  {
     char TmpString[10];
     Windows BackWindow;
     int i;
     ULONG Style;

     memset(&BackWindow,0,sizeof(BackWindow));

     BackWindow.Left=LINESPACE;
     BackWindow.Top=LINESPACE+2*SYSBUTTONWIDTH+2;
     BackWindow.Right=BackWindow.Left+WindowGetWidth(Window)
                      -3*LINESPACE-SYSSCROLLWIDTH;
     BackWindow.Bottom=BackWindow.Top+TOOLBARHEIGHT+2*LINESPACE;
     BackWindow.Procedure=(Function *)BackToolBarProcedure;
     BackWindow.WindowStyle=WindowSetIsUserWindow()|WindowSetMoveable()|
                             WindowSetResizeable();
     MidWindow=WindowAppend(&BackWindow,Window);
     if (!MidWindow)
        return;

     ToolBarSetHasToolBar();
     MenuSetCheckedAttr(MidMenu);

     if (ToolBarHasRulerBar())       // move ruler to toolbar's below
     {
        HWND MidWindow2;

        MidWindow2=SearchHClibrationWindow(Window);
        if (MidWindow2)
        {
           WindowSetTop(MidWindow2,WindowGetTop(MidWindow2)+
                        WindowGetHeight(MidWindow)-1);
           WindowSetBottom(MidWindow2,WindowGetBottom(MidWindow2)+
                        WindowGetHeight(MidWindow)-1);
        }
        MidWindow2=SearchVClibrationWindow(Window);
        if (MidWindow2)
           WindowSetTop(MidWindow2,WindowGetTop(MidWindow2)+
                        WindowGetHeight(MidWindow)-1);
     }

     Style=WindowSetIsUserButton()|WindowSetIsIcon();

  #define GAP1          4

     TmpString[1]=0;

     for(i=0;i<TotalIconNumber;i++) {
        int x;
        ULONG style;
        if(i<=IDX_IMPORT) x=LINESPACE+i*TOOLBARWIDTH;
        else if(i<=IDX_SIZE) x=LINESPACE+GAP1+i*TOOLBARWIDTH;
        else if(i<=IDX_ZOOM) x=LINESPACE+2*GAP1+i*TOOLBARWIDTH;
        else if(i<=IDX_PLGBOX) x=LINESPACE+3*GAP1+i*TOOLBARWIDTH;
        else if(i<=IDX_LINE) x=LINESPACE+4*GAP1+i*TOOLBARWIDTH;
        else if(i<=IDX_UNLINK) x=LINESPACE+5*GAP1+i*TOOLBARWIDTH;
        else x=LINESPACE+6*GAP1+i*TOOLBARWIDTH;

        if (i>=IDX_SIZE&&i<=IDX_ZOOM-3) x=x+(i-IDX_SIZE+1)*40;
        else if (i>IDX_ZOOM-3) x=x+80;

        if(i<=IDX_SIZE || i==IDX_PRINT) style=Style|WindowSetIconMustBeUp();
        else style=Style;

        TmpString[0]=2*i;
        CreatToolBarWindow(x,LINESPACE,TOOLBARWIDTH,
                TOOLBARHEIGHT,i,TmpString,style,MidWindow);
     }


  #undef GAP1

  #ifdef USEIMAGE
     if(GlobalBoxTool==-1)              // added ByHance, for first Init
            GlobalBoxTool=IDX_INPUTBOX;
     else               // using Image to SpeedUp
     {
          char buf[10000];
          FILE *fp;
          struct viewporttype ViewInformation;

          fp=fopen("toolbar.img","rb");
          fread(buf,1,10000,fp);
          fclose(fp);

          getviewsettings(&ViewInformation);
          MouseHidden();

          setviewport(0,0,getmaxx(),getmaxy(),1);
          putimage(BackWindow.Left,BackWindow.Top,buf,COPY_PUT);
          setviewport(ViewInformation.left,ViewInformation.top,
                      ViewInformation.right,ViewInformation.bottom,
                      ViewInformation.clip);
          MouseShow();
     }
  #endif // USEIMAGE
  }

  #ifndef USEIMAGE
  if(GlobalBoxTool==-1)              // added ByHance, for first Init
         GlobalBoxTool=IDX_INPUTBOX;
  else
  #endif // USEIMAGE

  MessageInsert(Window,REDRAWMESSAGE,MAKELONG(LINESPACE,LINESPACE+2*SYSBUTTONWIDTH+2),
               MAKELONG(WindowGetWidth(Window)-2*LINESPACE-SYSSCROLLWIDTH,
               WindowGetHeight(Window)-2*LINESPACE-SYSSCROLLWIDTH));
  if (ToolBarHasToolBar())
   {
     int ii;
     WaitMessageEmpty();
     ii=5*TOOLBARWIDTH+4+LINESPACE+6;
     CreatFontWindow(ii,ii+35);
     ii=ii+TOOLBARWIDTH+40;
     CreatFontWindow(ii,ii+36);
     //DispFontWindow(1);

     if (GlobalBoxTool==IDX_INPUTBOX && GlobalBoxHeadHandle>=0
      &&CurrentBoxEditable())
     DispFontWindow(2);

    }

}

#define DRAWCLIBRATIONINITIAL -1
static int FirstDrawClibration=DRAWCLIBRATIONINITIAL;


static void DrawCurrentHClibration(HWND Window,int Postion)
{
  int WindowLeft,WindowTop,WindowRight,WindowBottom;

/*----------
  if (!(ToolBarHasRulerBar()))
     return;

  Window=SearchHClibrationWindow(Window);
  if (!Window)
     return;
----------*/

  WindowGetRealRect(Window,&WindowLeft,&WindowTop,&WindowRight,&WindowBottom);
  setviewport(WindowLeft,WindowTop,WindowRight,WindowBottom,1);
  if (FirstDrawClibration>0) {
     line(OldHClibration,0,OldHClibration,WindowGetHeight(Window));
  } else
     FirstDrawClibration++;
  Postion-=WindowGetLeft(Window);
  line(Postion,0,Postion,WindowGetHeight(Window));
  OldHClibration=Postion;
}

static void DrawHClibration(HWND Window,int DrawLeft,int DrawTop,int DrawRight,
                    int DrawBottom)
{
  #define CLIBRATIONSCALE 4
  #define CLIBRATIONBOTTOMSPACE 1

  struct viewporttype TmpViewPort;
  int WindowLeft,WindowTop,WindowRight,WindowBottom;
  int SaveColor;
  int Start,End,i,LineLength,SystemMeterStep,WindowI;

  getviewsettings(&TmpViewPort);
  WindowGetRealRect(Window,&WindowLeft,&WindowTop,&WindowRight,&WindowBottom);
  if (DrawLeft>=WindowGetWidth(Window)
      ||DrawTop>=WindowGetHeight(Window)
      ||DrawRight<0||DrawBottom<0)
     return;

  if (FileMETERIsINCH())
     SystemMeterStep=SCALEMETER/CLIBRATIONSCALE;
  else
     SystemMeterStep=SCALEMETER/CLIBRATIONSCALE/2.54;

  if(GlobalPageScale>20)         // less than 50%
     SystemMeterStep*=2;
  Start=WindowXToUserX(0)/SystemMeterStep*SystemMeterStep;
  if(Start==h_old_start && GlobalPageScale==h_old_step)
     return;

  h_old_start=Start;  h_old_step=GlobalPageScale;
  End=WindowXToUserX(WindowGetWidth(Window))/SystemMeterStep*SystemMeterStep;

  MouseHidden();
  SaveColor=getcolor();
  if (DrawLeft<0)
     DrawLeft=0;
  if (DrawTop<0)
     DrawTop=0;
  if (DrawRight>WindowGetWidth(Window)-1)
     DrawRight=WindowGetWidth(Window)-1;
  if (DrawBottom>WindowGetHeight(Window)-1)
     DrawBottom=WindowGetHeight(Window)-1;

  DrawLeft+=WindowLeft;
  DrawTop+=WindowTop;
  DrawRight+=WindowLeft;
  DrawBottom+=WindowTop;

  WindowLeft-=DrawLeft;
  WindowTop-=DrawTop;
  WindowRight-=DrawLeft;
  WindowBottom-=DrawTop;

  setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
  setfillstyle(1,EGA_LIGHTGRAY);
  bar(WindowLeft,WindowTop,WindowRight,WindowBottom);
 /*----------
  setcolor(EGA_BLACK);
  line(WindowLeft,WindowTop,WindowRight,WindowTop);
  line(WindowLeft,WindowBottom,WindowRight,WindowBottom);
  line(WindowLeft,WindowBottom-CLIBRATIONBOTTOMSPACE,
       WindowRight,WindowBottom-CLIBRATIONBOTTOMSPACE);
  setcolor(EGA_WHITE);
  line(WindowLeft,WindowTop+1,WindowRight,WindowTop+1);
  line(WindowLeft,WindowBottom-CLIBRATIONBOTTOMSPACE-1,
       WindowRight,WindowBottom-CLIBRATIONBOTTOMSPACE-1);
  -----------*/
  scan_line(WindowLeft,WindowRight,WindowTop,EGA_BLACK);
  scan_line(WindowLeft,WindowRight,WindowBottom,EGA_BLACK);
  scan_line(WindowLeft,WindowRight,
          WindowBottom-CLIBRATIONBOTTOMSPACE,EGA_BLACK);

  scan_line(WindowLeft,WindowRight,WindowTop+1,EGA_WHITE);
  scan_line(WindowLeft,WindowRight,
       WindowBottom-CLIBRATIONBOTTOMSPACE-1,EGA_WHITE);

  for (i=Start;i<=End;i+=SystemMeterStep)
  {
      //circle(i,100,3);
      //printf("%d,%d  ",SystemMeterStep,i);
      //getch();
      WindowI=UserXToWindowX(i);
      if (i%(SystemMeterStep*2))
         LineLength=2;
      else
         if (i%(SystemMeterStep*4))
            LineLength=6;
         else
         {
            char TmpString[40];

            LineLength=10;
            if(GlobalPageScale>20)         // less than 50%
               sprintf(TmpString,"%d",i/(SystemMeterStep*2));
            else
               sprintf(TmpString,"%d",i/(SystemMeterStep*4));
            setcolor(EGA_BLACK);
            outtextxy(WindowI+2,4,TmpString);
         }
      setcolor(EGA_BLACK);
      line(WindowI,WindowGetHeight(Window)-LineLength-CLIBRATIONBOTTOMSPACE-1,
           WindowI,WindowGetHeight(Window)-CLIBRATIONBOTTOMSPACE-2);
      setcolor(EGA_WHITE);
      line(WindowI+1,WindowGetHeight(Window)-LineLength-CLIBRATIONBOTTOMSPACE,
           WindowI+1,WindowGetHeight(Window)-CLIBRATIONBOTTOMSPACE-1);
  }

  setcolor(EGA_BLACK);
  line(0,0,0,WindowBottom-WindowTop);
  line(WindowRight-WindowLeft,0,WindowRight-WindowLeft,WindowBottom-WindowTop);
  setcolor(SaveColor);
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  MouseShow();
}

static void DrawCurrentVClibration(HWND Window,int Postion)
{
  int WindowLeft,WindowTop,WindowRight,WindowBottom;

/*------------
  if (!(ToolBarHasRulerBar()))
     return;

  Window=SearchVClibrationWindow(Window);
  if (!Window)
     return;
--------------*/
  WindowGetRealRect(Window,&WindowLeft,&WindowTop,&WindowRight,&WindowBottom);
  setviewport(WindowLeft,WindowTop,WindowRight,WindowBottom,1);

  if (FirstDrawClibration>0) {
     line(0,OldVClibration,WindowGetWidth(Window),OldVClibration);
  } else
     FirstDrawClibration++;
  Postion-=WindowGetTop(Window);
  line(0,Postion,WindowGetWidth(Window),Postion);
  OldVClibration=Postion;
}

static void DrawVClibration(HWND Window,int DrawLeft,int DrawTop,int DrawRight,
                    int DrawBottom)
{
  struct viewporttype TmpViewPort;
  int WindowLeft,WindowTop,WindowRight,WindowBottom;
  int SaveColor;
  int Start,End,i,LineLength,SystemMeterStep,WindowI;
  int x;
//  struct linesettingstype SaveLineStyle;

  getviewsettings(&TmpViewPort);
  WindowGetRealRect(Window,&WindowLeft,&WindowTop,&WindowRight,&WindowBottom);
  if (DrawLeft>=WindowGetWidth(Window)
      ||DrawTop>=WindowGetHeight(Window)
      ||DrawRight<0||DrawBottom<0)
     return;

  if (FileMETERIsINCH())
     SystemMeterStep=SCALEMETER/CLIBRATIONSCALE;
  else
     SystemMeterStep=SCALEMETER/CLIBRATIONSCALE/2.54;

  if(GlobalPageScale>20)         // less than 50%
     SystemMeterStep*=2;
  Start=WindowYToUserY(0)/SystemMeterStep*SystemMeterStep;
  if(Start==v_old_start && GlobalPageScale==v_old_step)
     return;

  v_old_start=Start;  v_old_step=GlobalPageScale;
  End=WindowYToUserY(WindowGetHeight(Window))/SystemMeterStep*SystemMeterStep;

  MouseHidden();
  SaveColor=getcolor();

  if (DrawLeft<0)
     DrawLeft=0;
  if (DrawTop<0)
     DrawTop=0;
  if (DrawRight>WindowGetWidth(Window)-1)
     DrawRight=WindowGetWidth(Window)-1;
  if (DrawBottom>WindowGetHeight(Window)-1)
     DrawBottom=WindowGetHeight(Window)-1;

  DrawLeft+=WindowLeft;
  DrawTop+=WindowTop;
  DrawRight+=WindowLeft;
  DrawBottom+=WindowTop;

  WindowLeft-=DrawLeft;
  WindowTop-=DrawTop;
  WindowRight-=DrawLeft;
  WindowBottom-=DrawTop;

  setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
  setfillstyle(1,EGA_LIGHTGRAY);
  bar(WindowLeft,WindowTop,WindowRight,WindowBottom);
  setcolor(EGA_BLACK);
  line(WindowLeft,WindowTop,WindowLeft,WindowBottom);
  line(WindowRight,WindowTop,WindowRight,WindowBottom);
  line(WindowRight-CLIBRATIONBOTTOMSPACE,WindowTop,
       WindowRight-CLIBRATIONBOTTOMSPACE,WindowBottom);
  setcolor(EGA_WHITE);
  line(WindowLeft+1,WindowTop,WindowLeft+1,WindowBottom);
  line(WindowRight-CLIBRATIONBOTTOMSPACE-1,WindowTop,
       WindowRight-CLIBRATIONBOTTOMSPACE-1,WindowBottom);
/*------------
  if (FileMETERIsINCH())
     SystemMeterStep=SCALEMETER/CLIBRATIONSCALE;
  else
     SystemMeterStep=SCALEMETER/CLIBRATIONSCALE/2.54;

  if(GlobalPageScale>20)         // less than 50%
     SystemMeterStep*=2;
  Start=WindowYToUserY(0)/SystemMeterStep*SystemMeterStep;
  End=WindowYToUserY(WindowGetHeight(Window))/SystemMeterStep*SystemMeterStep;
------------------*/

  x=WindowGetWidth(Window)-CLIBRATIONBOTTOMSPACE;

  for (i=Start;i<=End;i+=SystemMeterStep)
  {
      WindowI=UserYToWindowY(i);
      if (i%(SystemMeterStep*2))
         LineLength=2;
      else
         if (i%(SystemMeterStep*4))
            LineLength=6;
         else
         {
            char TmpString[40];

            LineLength=10;
            if(GlobalPageScale>20)         // less than 50%
               sprintf(TmpString,"%d",i/(SystemMeterStep*2));
            else
               sprintf(TmpString,"%d",i/(SystemMeterStep*4));
            setcolor(EGA_BLACK);
            outtextxy(0,WindowI+5,TmpString);
         }
   /*-----------------
      setcolor(EGA_BLACK);
      line(WindowGetWidth(Window)-LineLength-CLIBRATIONBOTTOMSPACE-1,WindowI+3,
           WindowGetWidth(Window)-CLIBRATIONBOTTOMSPACE-2,WindowI+3);
      setcolor(EGA_WHITE);
      line(WindowGetWidth(Window)-LineLength-CLIBRATIONBOTTOMSPACE,WindowI+4,
           WindowGetWidth(Window)-CLIBRATIONBOTTOMSPACE-1,WindowI+4);
    -----------------*/
      scan_line(x-LineLength-1,x-2,WindowI+3,EGA_BLACK);
      scan_line(x-LineLength,x-1,WindowI+4,EGA_WHITE);
  }

 /*-----------------
  setcolor(EGA_BLACK);
  line(0,0,WindowRight-WindowLeft,0);
  line(0,WindowBottom-WindowTop,WindowRight-WindowLeft,
       WindowBottom-WindowTop);
  -----------------*/
  scan_line(0,WindowRight-WindowLeft,0,EGA_BLACK);
  scan_line(0,WindowRight-WindowLeft,WindowBottom-WindowTop,EGA_BLACK);

  setcolor(EGA_LIGHTGRAY);
//  getlinesettings(&SaveLineStyle);
//  setfillstyle(1,EGA_LIGHTGRAY);
  setviewport(WindowGetLeft(Window),
              WindowGetTop(Window)-WindowGetWidth(Window),
              WindowGetRight(Window),WindowGetTop(Window),1);
  bar(0,0,WindowGetWidth(Window),WindowGetWidth(Window));
//  setlinestyle(SaveLineStyle.linestyle,
//               SaveLineStyle.upattern,
//               SaveLineStyle.thickness);

  FirstDrawClibration=DRAWCLIBRATIONINITIAL;
  setcolor(SaveColor);
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  MouseShow();
}

#ifdef NOT_USED
void DispStatusBarXY(int x,int y)     //By zjh
{
  int SaveColor;
  struct viewporttype SaveViewPort;
  int maxx,maxy;
  char tmpstr[120];

  maxx=getmaxx();
  maxy=getmaxy();

  MouseHidden();
  getviewsettings(&SaveViewPort);
  SaveColor=getcolor();
  setviewport(0,0,maxx,maxy,1);
  setwritemode(COPY_PUT);

  x=OldHClibration;
  y=OldVClibration;

  x=WindowXToUserX(x);
  y=WindowYToUserY(y);

  sprintf(tmpstr,"X:%f",(float)(x/394.0f));
  tmpstr[7]=',';
  sprintf(tmpstr+8,"Y:%f",(float)(y/394.0f));
  tmpstr[15]=0;


  DisplayString(tmpstr,8,maxy-21,EGA_BLACK,EGA_WHITE);

  setcolor(SaveColor);
  setviewport(SaveViewPort.left,SaveViewPort.top,SaveViewPort.right,
              SaveViewPort.bottom,SaveViewPort.clip);
  MouseShow();
}
#endif  // NOT_USED


void DrawCurrentClibration(HWND Window,int x, int y)
{
  HWND WinH,WinV;
  struct viewporttype TmpViewPort;
//  struct linesettingstype SaveLineStyle;
  int SaveColor;
  unsigned old_style;

  if( !(ToolBarHasRulerBar()) || ActiveWindow!=1)
     return;

  WinH=SearchHClibrationWindow(Window);
  if (!WinH)
     return;
  if(y<=WindowGetBottom(WinH))
     return;
  WinV=SearchVClibrationWindow(Window);
  if (!WinV)
     return;
  if(x<WindowGetRight(WinV))
     return;

  MouseHidden();
  getviewsettings(&TmpViewPort);
  SaveColor=getcolor();

#ifdef __TURBOC__
  getlinesettings(&SaveLineStyle);
  setlinestyle(1,0,1);
#else
  old_style=getlinestyle();
  setlinestyle(0xbbbb);
#endif

  setwritemode(XOR_PUT);
  setcolor(EGA_LIGHTRED^EGA_LIGHTGRAY);

  DrawCurrentHClibration(WinH,x);
  DrawCurrentVClibration(WinV,y);



  setcolor(SaveColor);
  setwritemode(COPY_PUT);

#ifdef __TURBOC__
  setlinestyle(SaveLineStyle.linestyle,
               SaveLineStyle.upattern,
               SaveLineStyle.thickness);
#else
  setlinestyle(old_style);
#endif

  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  MouseShow();

#ifdef NOT_USED
  DispStatusBarXY(x,y);
#endif
}

static long HClibrationProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case GETFOCUS:
         return(FALSE);
    case DRAWWINDOW:
         DrawHClibration(Window,MAKEHI(Param1),MAKELO(Param1),MAKEHI(Param2),
                        MAKELO(Param2));
         break;
    case MOUSELEFTDOWN:
         MessageInsert(WindowGetFather(Window),GETFOCUS,0,0);
         break;
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static long VClibrationProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case GETFOCUS:
         return(FALSE);
    case DRAWWINDOW:
         DrawVClibration(Window,MAKEHI(Param1),MAKELO(Param1),MAKEHI(Param2),
                        MAKELO(Param2));
         break;
    case MOUSELEFTDOWN:
         MessageInsert(WindowGetFather(Window),GETFOCUS,0,0);
         break;
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

HWND SearchHClibrationWindow(HWND Window)
{
  return(SearchChildWindowByFunction(Window,(Function *)HClibrationProcedure));
}

HWND SearchVClibrationWindow(HWND Window)
{
  return(SearchChildWindowByFunction(Window,(Function *)VClibrationProcedure));
}

void ClibrationWindowModify(HWND Window)
{
  HMENU MidMenu;
  HWND MidWindow;


  TextCursorOff();
  MidMenu=GetMenuFromReturnValue(MENU_CLIBRATION);
  if (ToolBarHasRulerBar())             // now, hide it
  {
     MidWindow=SearchHClibrationWindow(Window);
     if (!MidWindow) {
        TextCursorDisplay();
        return;
     }
     WindowDelete(MidWindow);

     MidWindow=SearchVClibrationWindow(Window);
     if (!MidWindow) {
        TextCursorDisplay();
        return;
     }
     WindowDelete(MidWindow);

     ToolBarSetHasNoRulerBar();
     MenuSetNoCheckedAttr(MidMenu);
     h_old_start=v_old_start=-10000;
  }
  else
  {                             // now, display it
     // char TmpString[40];
     Windows BackWindow;

     memset(&BackWindow,0,sizeof(BackWindow));
     BackWindow.Left=LINESPACE+RULERBARHEIGHT;
     BackWindow.Top=LINESPACE+2*SYSBUTTONWIDTH+2;
     if (ToolBarHasToolBar())
        BackWindow.Top+=TOOLBARHEIGHT+2*LINESPACE;

     BackWindow.Right=WindowGetWidth(Window)-2*LINESPACE-SYSSCROLLWIDTH;
     BackWindow.Bottom=BackWindow.Top+RULERBARHEIGHT;
     BackWindow.Procedure=(Function *)HClibrationProcedure;
     BackWindow.WindowStyle|=WindowSetMoveable()|WindowSetResizeable();
     MidWindow=WindowAppend(&BackWindow,Window);
     if (!MidWindow) {
        TextCursorDisplay();
        return;
     }

     memset(&BackWindow,0,sizeof(BackWindow));
     BackWindow.Left=LINESPACE;
     BackWindow.Top=LINESPACE+2*SYSBUTTONWIDTH+2+RULERBARHEIGHT;
     if (ToolBarHasToolBar())
        BackWindow.Top+=TOOLBARHEIGHT+2*LINESPACE;
     BackWindow.Right=BackWindow.Left+RULERBARHEIGHT;
     BackWindow.Bottom=WindowGetHeight(Window)-2*LINESPACE-SYSSCROLLWIDTH;
     BackWindow.Procedure=(Function *)VClibrationProcedure;
     BackWindow.WindowStyle|=WindowSetMoveable()|WindowSetResizeable();
     MidWindow=WindowAppend(&BackWindow,Window);
     if (!MidWindow) {
        TextCursorDisplay();
        return;
     }

     MenuSetCheckedAttr(MidMenu);
     ToolBarSetHasRulerBar();
  }

  if (BoxCanEditable(GlobalBoxHeadHandle))
  {
   // added ByHance, for display cursor
   HBOX NewHBox;
   int  CursorX,CursorY;
   CursorLocate(GlobalBoxHeadHandle,&NewHBox,GlobalTextPosition,&CursorX,&CursorY);
  }

  MessageInsert(Window,REDRAWMESSAGE,MAKELONG(LINESPACE,LINESPACE+2*SYSBUTTONWIDTH+2),
                MAKELONG(WindowGetWidth(Window)-2*LINESPACE-SYSSCROLLWIDTH,
                WindowGetHeight(Window)-2*LINESPACE-SYSSCROLLWIDTH));
   if (ToolBarHasToolBar())
   { int ii;
     WaitMessageEmpty();
     ii=5*TOOLBARWIDTH+4+LINESPACE+6;
     CreatFontWindow(ii,ii+35);
     ii=ii+TOOLBARWIDTH+40;
     CreatFontWindow(ii,ii+36);
     //DispFontWindow(1);
     if (GlobalBoxTool==IDX_INPUTBOX && GlobalBoxHeadHandle>=0
      &&CurrentBoxEditable())
     DispFontWindow(2);
    }
}

void SetHScrollLeft(HWND Window,int PageWidth)
{
   HWND MidWindow;
   int  WinWidth,Left;

   MidWindow=WindowGetChild(Window);
   while (MidWindow)      // search H_Scroll
   {
      if (WindowIsHScroll(MidWindow))
         break;
      else
         MidWindow=WindowGetNext(MidWindow);
   }

   if(MidWindow)        // found it
   {
      WinWidth=WindowGetWidth(MidWindow);
      MidWindow=WindowGetChild(MidWindow);
      while (MidWindow)      // search H_Scroll_button
      {
         if (WindowIsHHScroll(MidWindow))
            break;
         else
            MidWindow=WindowGetNext(MidWindow);
      }

      if(MidWindow)
      {
         Left=(float)GlobalPageHStart*(WinWidth-3*SYSSCROLLWIDTH)
                  /(PageWidth+PAGELEFTDISTANT)
               +SYSSCROLLWIDTH+0.5;
         Left-=WindowGetLeft(MidWindow);
         MessageInsert(MidWindow,WINDOWMOVE,Left,0l);
      }
   }
} /* SetHScrollLeft */

void SetVScrollTop(HWND Window,int PageHeight)
{
   HWND MidWindow;
   int  WinHeight,Top;

   MidWindow=WindowGetChild(Window);
   while (MidWindow)      // search V_Scroll
   {
      if (WindowIsVScroll(MidWindow))
         break;
      else
         MidWindow=WindowGetNext(MidWindow);
   }

   if(MidWindow)        // found it
   {
      WinHeight=WindowGetHeight(MidWindow);
      MidWindow=WindowGetChild(MidWindow);
      while (MidWindow)      // search V_Scroll_button
      {
         if (WindowIsVVScroll(MidWindow))
            break;
         else
            MidWindow=WindowGetNext(MidWindow);
      }

      if(MidWindow)
      {
         Top=(float)GlobalPageVStart*(WinHeight-3*SYSSCROLLWIDTH)
                 /(PageHeight+PAGETOPDISTANT)
               +SYSSCROLLWIDTH+0.5;
         Top-=WindowGetTop(MidWindow);
         MessageInsert(MidWindow,WINDOWMOVE,0l,Top);
      }
   }
} /* SetVScrollTop */

void MovePageToCenter(HWND Window,HPAGE PageItem)
{
  Pages *MidPage;
  int Left,Top,Right,Bottom;
  int PageWidth,PageHeight;
  int BoxWidth,BoxHeight;
  int WinWidth,WinHeight;

  if(PageItem<=0) return;

  MidPage=HandleLock(ItemGetHandle(PageItem));
  if (MidPage==NULL)
     return;
  PageWidth=PageGetPageWidth(MidPage);
  PageHeight=PageGetPageHeight(MidPage);
  BoxWidth=PageWidth-PageGetMarginLeft(MidPage)-PageGetMarginRight(MidPage);
  BoxHeight=PageHeight-PageGetMarginTop(MidPage)-PageGetMarginBottom(MidPage);


  WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
  // change to user's 1/1000 Inch
  //WinWidth=(Right-Left)*GlobalPageScale;
  //WinHeight=(Bottom-Top)*GlobalPageScale;
  WinWidth=myWindowXToUserX(Right-Left);
  WinHeight=myWindowYToUserY(Bottom-Top);

  if(BoxWidth<=WinWidth)
  {
     //GlobalPageHStart=PAGELEFTDISTANT+(PageWidth-WinWidth)/2;
     GlobalPageHStart=(BoxWidth-WinWidth)/2+PageGetMarginLeft(MidPage);
     GlobalPageHStart+=PAGELEFTDISTANT;
  }
  else
      if (!fEditor)
         GlobalPageHStart=PAGELEFTDISTANT+(PageGetMarginLeft(MidPage)-150); //4mm
      else
         GlobalPageHStart=PAGELEFTDISTANT+(PageGetMarginLeft(MidPage)-40); //4mm

  SetHScrollLeft(Window,PageWidth);

  if(BoxHeight<=WinHeight)
  {
     //GlobalPageVStart=PAGETOPDISTANT+(PageHeight-WinHeight)/2;
     GlobalPageVStart=(BoxHeight-WinHeight)/2+PageGetMarginTop(MidPage);
     GlobalPageVStart+=PAGETOPDISTANT;
  }
  else
     if (!fEditor)
         GlobalPageVStart=PAGETOPDISTANT+(PageGetMarginTop(MidPage)-150); //4mm
     else
         GlobalPageVStart=PAGETOPDISTANT+(PageGetMarginTop(MidPage)-40); //4mm

  SetVScrollTop(Window,PageHeight);

  HandleUnlock(ItemGetHandle(PageItem));
}

/*-------- ByHance, 95,12.7 -----*/
void DrawLinkedBox()
{
  int SaveColor;
  struct viewporttype TmpViewPort;
  ORDINATETYPE BoxXY[2*1],BoxXY2[2*1];
  HBOX HBox;
  TextBoxs *MidBox;
  ORDINATETYPE Left2,Top2,Right2,Bottom2;
  int Left,Top,Right,Bottom;
  HBOX LinkBox;

  if(GlobalCurrentPage<=0)
    return;

  SaveColor=getcolor();
  MouseHidden();
  TextCursorOff();
  getviewsettings(&TmpViewPort);
//  setcolor(EGA_BLACK);
  setcolor(EGA_WHITE);
  setwritemode(XOR_PUT);

  WindowGetRect(1,&Left,&Top,&Right,&Bottom);   // user window
  setviewport(Left,Top,Right,Bottom,1);

  HBox=PageGetBoxHead(GlobalCurrentPage);
  while (HBox)
  {
    MidBox=HandleLock(ItemGetHandle(HBox));
    if (MidBox==NULL)
       break;

    if (TextBoxGetBoxType(MidBox)==TEXTBOX &&
        (LinkBox=TextBoxGetPrevLinkBox(MidBox))!=0 )
    {
       BoxGetRect(HBox,&Left,&Top,&Right,&Bottom);
       BoxXY[0]=Left;     BoxXY[1]=Top;
       BoxPolygonRotate(1,BoxXY,(PictureBoxs *)MidBox);
       BoxPolygonToWindowXY(1,BoxXY);

       if(ItemGetFather(LinkBox)==GlobalCurrentPage)
       {
          BoxGetRect(LinkBox,&Left2,&Top2,&Right2,&Bottom2);
          BoxXY2[0]=Right2;  BoxXY2[1]=Bottom2;
          BoxPolygonRotate(1,BoxXY2,HandleLock(ItemGetHandle(LinkBox)));
          HandleUnlock(ItemGetHandle(LinkBox));
          BoxPolygonToWindowXY(1,BoxXY2);
       }
       else
       {
          BoxXY2[0]=getmaxx();  BoxXY2[1]=0;    // screen (Right,Top)
       }

       line(BoxXY[0],BoxXY[1],BoxXY2[0],BoxXY2[1]);
    }

    HandleUnlock(ItemGetHandle(HBox));
    HBox=ItemGetNext(HBox);
  }

  setwritemode(COPY_PUT);
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  setcolor(SaveColor);
  MouseShow();
}

void DrawScreenPageFootHead()
{
  struct viewporttype TmpViewPort;
  int Left,Top,Right,Bottom,width;
  int SaveColor,X1,Y1,X2,Y2;
  Pages *MidPage;

  if(GlobalCurrentPage<=0)
    return;

  //width=146/GlobalPageScale;      /* use 5' size */
  width=myUserXToWindowX(146);      /* use 5' size */

  SaveColor=getcolor();
  MouseHidden();
  getviewsettings(&TmpViewPort);

  WindowGetRect(1,&Left,&Top,&Right,&Bottom);   // user window
  setviewport(Left,Top,Right,Bottom,1);

  setfillstyle(1,EGA_WHITE);

  MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
  X1=UserXToWindowX(PageGetMarginLeft(MidPage));
  X2=UserXToWindowX(PageGetPageWidth(MidPage)-PageGetMarginRight(MidPage));

  //---- clear top area ----
  Y1=UserYToWindowY(PageGetMarginTop(MidPage))-width-width;
  Y2=UserYToWindowY(PageGetMarginTop(MidPage))-1;
  bar(X1,Y1,X2,Y2);

  //---- clear bottom area ----
  Y2=UserYToWindowY(PageGetPageHeight(MidPage)
              -PageGetMarginBottom(MidPage))+width*3/2;
  Y1=Y2-width;
  bar(X1,Y1,X2,Y2);

  HandleUnlock(ItemGetHandle(GlobalCurrentPage));

  //---- draw them ----
  setcolor(EGA_BLACK);
  DrawPageFootHead(GlobalCurrentPage,TRUE,TRUE);

  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  setcolor(SaveColor);
  MouseShow();
}

void filename_cat(char *filename,char *ext_name)
{
          char *pstr;

          if( *ext_name == '\0' ) return;   /* no extend name */

          pstr = strchr(filename, '.');
          //if (pstr)    *pstr = '\0';            /* end mark of string */
          if (pstr) return;              //By zjh
          strcat(filename, ext_name);          /* add default extend name */
}



void ZoomPageAtPoint(HWND Window,ULONG MouseXY,int fZoomOut)
{
  Pages *MidPage;
  int x,y,dx,dy;
  ULONG Param;
  int Left,Top,Right,Bottom;
  int PageWidth,PageHeight;
  int BoxWidth,BoxHeight;
  int WinWidth,WinHeight;
  HPAGE PageItem=GlobalCurrentPage;

  Param=WindowToUserWindow(Window,MouseXY);
  dx=(short)MAKEHI(Param);
  dy=(short)MAKELO(Param);

  if(fZoomOut)
  {
      GlobalPageScale/=2;
      x=WindowXToUserX(dx);
      y=WindowYToUserY(dy);
  }
  else        /*-- zoom in --*/
  {
      x=myWindowXToUserX(dx);
      y=myWindowYToUserY(dy);
      GlobalPageScale*=2;
  }


  if(PageItem<=0) return;
  MidPage=HandleLock(ItemGetHandle(PageItem));
  if (MidPage==NULL)
     return;

  PageWidth=PageGetPageWidth(MidPage);
  PageHeight=PageGetPageHeight(MidPage);
  BoxWidth=PageWidth-PageGetMarginLeft(MidPage)-PageGetMarginRight(MidPage);
  BoxHeight=PageHeight-PageGetMarginTop(MidPage)-PageGetMarginBottom(MidPage);

  WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
  // change to user's 1/1000 Inch
  //WinWidth=(Right-Left)*GlobalPageScale;
  //WinHeight=(Bottom-Top)*GlobalPageScale;
  WinWidth=myWindowXToUserX(Right-Left);
  WinHeight=myWindowYToUserY(Bottom-Top);

  if(BoxWidth<=WinWidth)
  {
     GlobalPageHStart=(BoxWidth-WinWidth)/2+PageGetMarginLeft(MidPage);
     GlobalPageHStart+=PAGELEFTDISTANT;
  }
  else
  if(fZoomOut)
     GlobalPageHStart=x+PAGELEFTDISTANT; // -WinWidth/2;
  else
     GlobalPageHStart -= x;

  SetHScrollLeft(Window,PageWidth);

  if(BoxHeight<=WinHeight)
  {
     //GlobalPageVStart=PAGETOPDISTANT+(PageHeight-WinHeight)/2;
     GlobalPageVStart=(BoxHeight-WinHeight)/2+PageGetMarginTop(MidPage);
     GlobalPageVStart+=PAGETOPDISTANT;
  }
  else
  if(fZoomOut)
     GlobalPageVStart=y+PAGETOPDISTANT;  //-WinHeight/2;
  else
     GlobalPageVStart -= y;

  SetVScrollTop(Window,PageHeight);

  HandleUnlock(ItemGetHandle(PageItem));
}

/*---- 0=error, 1=not correct scale, 2=ok -----------*/
int ZoomPageByRect(HWND Window,int x1,int y1,int x2,int y2)
{
  Pages *MidPage;
  int Left,Top,Right,Bottom;
  int PageWidth,PageHeight;
  int BoxWidth,BoxHeight;
  int WinWidth,WinHeight;
  int width,height,tmp;
  HPAGE PageItem=GlobalCurrentPage;

  if(x1>x2) { tmp=x1; x1=x2; x2=tmp; }
  width=x2-x1;
  if(width<=10) return 1;

  if(y1>y2) { tmp=y1; y1=y2; y2=tmp; }
  height=y2-y1;
  if(height<=10) return 1;

  if(PageItem<=0) return 0;

  WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
   // change to user's 1/1000 Inch
   //WinWidth=(Right-Left)*GlobalPageScale;
  WinWidth=myWindowXToUserX(Right-Left);
  WinHeight=myWindowYToUserY(Bottom-Top);

  // xScale=(float)WinWidth/width;  yScale=(float)WinHeight/height;
  // if(xScale<yScale)
  if(width>height)
      tmp=GlobalPageScale*width/WinWidth;
  else
      tmp=GlobalPageScale*height/WinHeight;

  if(tmp<MINPAGESCALE || tmp>MAXPAGESCALE)
      return 1;

  MidPage=HandleLock(ItemGetHandle(PageItem));
  if (MidPage==NULL)
     return 0;

  GlobalPageScale=tmp;

  PageWidth=PageGetPageWidth(MidPage);
  PageHeight=PageGetPageHeight(MidPage);
  BoxWidth=PageWidth-PageGetMarginLeft(MidPage)-PageGetMarginRight(MidPage);
  BoxHeight=PageHeight-PageGetMarginTop(MidPage)-PageGetMarginBottom(MidPage);
   /*- because GlobalPageScale is changed, we must recaculate window's width
       and height.
    -------------*/
  WinWidth=myWindowXToUserX(Right-Left);
  WinHeight=myWindowYToUserY(Bottom-Top);

  if(BoxWidth<=WinWidth)
  {
     GlobalPageHStart=(BoxWidth-WinWidth)/2+PageGetMarginLeft(MidPage);
     GlobalPageHStart+=PAGELEFTDISTANT;
  }
  else
  {
     GlobalPageHStart=x1+PAGELEFTDISTANT;  // -WinWidth/2;
  }
  SetHScrollLeft(Window,PageWidth);

  if(BoxHeight<=WinHeight)
  {
     //GlobalPageVStart=PAGETOPDISTANT+(PageHeight-WinHeight)/2;
     GlobalPageVStart=(BoxHeight-WinHeight)/2+PageGetMarginTop(MidPage);
     GlobalPageVStart+=PAGETOPDISTANT;
  }
  else
  {
     GlobalPageVStart=y1+PAGETOPDISTANT; // -WinHeight/2;
  }
  SetVScrollTop(Window,PageHeight);

  HandleUnlock(ItemGetHandle(PageItem));

  return 2;
}



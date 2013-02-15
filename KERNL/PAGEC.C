/*-------------------------------------------------------------------
* Name: pagec.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static void PageGetLeftTop(ORDINATETYPE *PageLeft,ORDINATETYPE *PageTop);


HPAGE PageNumberToHandle(int PageNumber)
{
  HPAGE i;
  HITEM MidPageItem;

  for (i=0,MidPageItem=ItemGetChild(GlobalPageHeadHandle);i<PageNumber;i++)
      if (MidPageItem)
         MidPageItem=PageGetNext(MidPageItem);
      else
         return(0);
  return(MidPageItem);
}

int PageHandleToNumber(HPAGE PageHandle)
{
  HPAGE i;
  HITEM MidPageItem;

  for (i=0,MidPageItem=ItemGetChild(GlobalPageHeadHandle);i<TotalPage;i++)
      if (MidPageItem==PageHandle)
         return(i);
      else
         MidPageItem=PageGetNext(MidPageItem);
  return(i);
}

HPAGE PageGotoNumber(int PageNumber)
{
  HPAGE GotoPageHandle;

  if (PageNumber>TotalPage||PageNumber<0)
     return(0);
  GotoPageHandle=PageNumberToHandle(PageNumber);
  if (GlobalCurrentPage==GotoPageHandle)
     return(GlobalCurrentPage);
  if (GotoPageHandle==NULL)
     return(0);

  PageChangeCurrent(GotoPageHandle);
  return(GlobalCurrentPage);
}

HPAGE PageGotoHandle(HPAGE PageHandle)
{
  if (PageHandle==0)
     return(0);
//  PageChangeCurrent(PageHandle);
  GlobalCurrentPage=PageHandle;
  RedrawUserField();
  TellStatus();               // ByHance
  return(PageHandle);
}

#ifdef UNUSED           // ByHance, 96,1.29
int GetFontUse(unsigned char *UseStatusArray)
{
  HPAGE MidPage;
  HBOX HBox;
  TextBoxs *TextBox;
  Wchar *TextBlock,KeyAttribute;
  HANDLE TextHandle;
  int i;

  MemSet(UseStatusArray,0,MAXFONTNUMBER);
  MidPage=ItemGetChild(GlobalPageHeadHandle);
  while (MidPage)
  {
    HBox=PageGetBoxHead(MidPage);
    while (HBox)
    {
      TextBox=HandleLock(ItemGetHandle(HBox));
      if (TextBox==NULL)
         return(OUTOFMEMORY);
      if ((TextBoxGetBoxType(TextBox)==TEXTBOX
          ||TextBoxGetBoxType(TextBox)==TABLEBOX)
          &&!TextBoxGetPrevLinkBox(TextBox))
      {
         TextHandle=TextBoxGetTextHandle(TextBox);
         if (TextHandle!=0)
         {
            TextBlock=HandleLock(TextHandle);
            if (TextBlock==NULL)
            {
               HandleUnlock(ItemGetHandle(HBox));
               return(OUTOFMEMORY);
            }
            i=TextBoxGetBlockLength(TextBox);
            while (i>=0)
            {
              KeyAttribute=EditBufferSearchAttribute(TextBlock,i,CHARFONT,&i);
              if (i>0)
                 UseStatusArray[KeyAttribute]=1;
            }
            HandleUnlock(TextHandle);
         }
      }
      HandleUnlock(ItemGetHandle(HBox));
      HBox=ItemGetNext(HBox);
    }
    MidPage=ItemGetNext(MidPage);
  }
  ReturnOK();
}

int GetPictureUse(unsigned char *UseStatusArray[40])
{
  HPAGE MidPage;
  HBOX HBox;
  PictureBoxs *PictureBox;
  char *FileName;
  int TotalFile=0,i;

  UseStatusArray[0][0]=0;
  MidPage=ItemGetChild(GlobalPageHeadHandle);
  while (MidPage)
  {
    HBox=PageGetBoxHead(MidPage);
    while (HBox)
    {
      PictureBox=HandleLock(ItemGetHandle(HBox));
      if (PictureBox==NULL)
         return(OUTOFMEMORY);
      if (PictureBoxGetBoxType(PictureBox)>=RECTANGLEPICTUREBOX
          &&PictureBoxGetBoxType(PictureBox)<=POLYGONPICTUREBOX)
      {
         FileName=PictureBoxGetPictureFileName(PictureBox);
         if (FileName[0])
         {
            for (i=0;i<TotalFile;i++)
            {
                if (!strcmp(FileName,UseStatusArray[i]))
                   break;
            }
            if (i>=TotalFile)
               strncpy(UseStatusArray[TotalFile],FileName,39);
         }
      }
      HandleUnlock(ItemGetHandle(HBox));
      HBox=ItemGetNext(HBox);
    }
    MidPage=ItemGetNext(MidPage);
  }
  return(TotalFile);
}

HPAGE PageExchange(int PageNumber1,int PageNumber2)
{
  HPAGE MidPage1,MidPage2;

  if (PageNumber1==PageNumber2)
     return(0);
  /*                                   // A fast method to index two page
  int i,j;
  if (PageNumber1<PageNumber2)
     j=PageNumber1;
  else
     j=PageNumber2;

  for (i=0,MidPage1=ItemGetChild(GlobalPageHeadHandle);i<j;i++)
      if (MidPage1==0)
         return(0);
      else
         MidPage1=PageGetNext(MidPage1);

  for (j=PageNumber1+PageNumber2-j,MidPage2=MidPage1;i<j;i++)
      if (MidPage2==0)
         return(0);
      else
         MidPage2=PageGetNext(MidPage2);*/


  MidPage1=PageNumberToHandle(PageNumber1);
  MidPage2=PageNumberToHandle(PageNumber2);
  FileSetModified();
  return(ItemSetFront(MidPage1,MidPage2));
}
#endif    // UNUSED           // ByHance, 96,1.29

HPAGE PageNew(Pages *NewPage,int PageNumber)
{
  Items NewPageHandle;
  HITEM MidPageItem,MidItem,MidBoxItem;
  Pages *MidPage;
//  TextBoxs NewBox;            // ByHance
#define NewBox        (TmpBuf.TextBox)

  memset(&NewPageHandle,0,sizeof(NewPageHandle));

  if (PageNumber>0)
     MidPageItem=PageNumberToHandle(PageNumber-1);
  else
     MidPageItem=0;

  if (!(MidPageItem=ItemInsert(&NewPageHandle,GlobalPageHeadHandle,MidPageItem)))
     return(0);

  if (!PageNumber)
  {
     if (!(MidItem=HandleAlloc(sizeof(Pages),0)))
        return(0);
     if ((MidPage=(Pages *)HandleLock(MidItem))==NULL)
     {
        HandleFree(MidItem);
        return(0);
     }
     memcpy(MidPage,NewPage,sizeof(Pages));
     HandleUnlock(MidItem);
     ItemSetHandle(MidPageItem,MidItem);
  }
  else
     ItemSetHandle(MidPageItem,ItemGetHandle(PageNumberToHandle(0)));

  if (PageHaveInitialBox(*NewPage))
  {
     HBOX NewHBox;
     int CursorX,CursorY;
     TextBoxs *BoxPointer;

     DefaultNewBox(NewPage,&NewBox);
     NewBox.BoxStatus|=4;               // TextBoxSetDependPage()
     MidBoxItem=TextBoxInsert(&NewBox,MidPageItem);
     if (!MidBoxItem)
        return(OUTOFMEMORY);

     if (TotalPage==0)
     {
        GlobalBoxHeadHandle=MidBoxItem;
        GlobalTextPosition=0;
        TextBoxSeekTextPosition(GlobalBoxHeadHandle,0,1,&GlobalTextPosition);
        TextBoxSeekTextPosition(GlobalBoxHeadHandle,GlobalTextPosition,-1,&GlobalTextPosition);
        CursorLocate(GlobalBoxHeadHandle,&NewHBox,GlobalTextPosition,&CursorX,&CursorY);
     }
     // BoxPointer=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
     BoxPointer=HandleLock(ItemGetHandle(MidBoxItem));   // ByHance, 95,12.5
     if (BoxPointer)
          BoxSetLocked(BoxPointer);
     HandleUnlock(ItemGetHandle(MidBoxItem));
     // HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));

     /* else
     {
        TextBoxs *TextBox;

        GlobalBoxHeadHandle=MidBoxItem;
        TextBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
        if (TextBox!=NULL)
        {
           InitRL(TextBox);
           HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
        }
     } */
  }
  else
     if (TotalPage==0)
        GlobalBoxHeadHandle=0;
  TotalPage++;
  FileSetModified();
  return(MidPageItem);

#undef NewBox           // ByHance
}

void PageDelete(int PageNumber)
{
  HPAGE MidHPage;
  HBOX MidHBox,NextHBox;

  if (PageNumber>=TotalPage)
     return;

  MidHPage=PageNumberToHandle(PageNumber);
  if (MidHPage)
  {
     MidHBox=PageGetBoxHead(MidHPage);
     while (MidHBox)
     {
       NextHBox=BoxGetNext(MidHBox);
       BoxDelete(MidHBox);
       MidHBox=NextHBox;
     }
     if (TotalPage==1)                 // Only delete one page's page setting
        HandleFree(ItemGetHandle(MidHPage));
     else
        ItemSetHandle(MidHPage,0);
     TotalPage--;
     ItemSetChild(MidHPage,0);
     FileSetModified();
     ItemDelete(MidHPage);
  }
}

void PageDeleteAll(void)
{
  HPAGE MidHPage,MidHPage2;

  if(GlobalPageHeadHandle<=0)
     return;

  MidHPage=ItemGetChild(GlobalPageHeadHandle);
  while (MidHPage)
  {
    MidHPage2=ItemGetNext(MidHPage);
    PageDelete(PageHandleToNumber(MidHPage));
    MidHPage=MidHPage2;
  }
}

//////////////MovePage///////////Jerry//////////////////////////////////////
/// flag == 0 is before, flag ==1 is after
int MovePage(int thispagen, int destpagen, int flag)
{
    HPAGE hpg1,hpg2;

    hpg1 = PageNumberToHandle(thispagen);
    hpg2 = PageNumberToHandle(destpagen);
    if (hpg1 == hpg2) return(0);

    if (ItemGetPrev(hpg1)==0)
       ItemSetChild(ItemGetFather(hpg1),ItemGetNext(hpg1));
    else
       ItemSetNext(ItemGetPrev(hpg1),ItemGetNext(hpg1));
    if (ItemGetNext(hpg1)!=0)
       ItemSetPrev(ItemGetNext(hpg1),ItemGetPrev(hpg1));

    if (flag) {      // after
         ItemSetPrev(hpg1,hpg2);
         if (hpg2!=0)
         {
            ItemSetNext(hpg1,ItemGetNext(hpg2));
            ItemSetNext(hpg2,hpg1);
         }
         else
         {
            ItemSetNext(hpg1,ItemGetChild(ItemGetFather(hpg1)));
            ItemSetChild(ItemGetFather(hpg1),hpg1);
         }
         if (ItemGetNext(hpg1))
            ItemSetPrev(ItemGetNext(hpg1),hpg1);
    }  else {         // before
         ItemSetNext(hpg1,hpg2);
         if (hpg2!=0)
         {
            ItemSetPrev(hpg1,ItemGetPrev(hpg2));
            ItemSetPrev(hpg2,hpg1);
         } else {
            ItemSetPrev(hpg1,0);
            ItemSetChild(ItemGetFather(hpg1),hpg1);
         }
         if (ItemGetPrev(hpg1))
            ItemSetNext(ItemGetPrev(hpg1),hpg1);
    }
    return(0);
}

void PageInitial(void)
{
  GlobalCurrentPage=-1;
  TotalPage=0;
  TmpPage.PageType=2;             // ByHance: use A4
/*---------
  TmpPage.MarginLeft=DEFAULTPAGEHDISTANT;
  TmpPage.MarginTop=DEFAULTPAGEVDISTANT;
  TmpPage.MarginRight=DEFAULTPAGEHDISTANT;
  TmpPage.MarginBottom=DEFAULTPAGEVDISTANT;
------------*/
}

void PageFinish(void)
{
  PageDeleteAll();
  return;
}

ORDINATETYPE WindowXToUserX(int X)
{
  ORDINATETYPE Left,Top;

  if (!PrintingSign)
  {
     PageGetLeftTop(&Left,&Top);
     return (ORDINATETYPE)(((ORDINATETYPE)X)*GlobalPageScale/SCRX)-Left;
  }
  else
  {
       return (ORDINATETYPE)(((long)X*(long)SCALEMETER/(long)PrinterDPI)/PRNX);
  }
}

int UserXToWindowX(ORDINATETYPE X)
{
  ORDINATETYPE Left,Top;

  if (!PrintingSign)
  {
     PageGetLeftTop(&Left,&Top);
     return (int)(((X+Left)/GlobalPageScale)*SCRX);     //Get File data
  }
  else
  {
        return (int)(((long)X*(long)PrinterDPI/(long)SCALEMETER)*PRNX);
  }
}

ORDINATETYPE WindowYToUserY(int Y)         //User:File data   Window :Prn and Scr
{
  ORDINATETYPE Left,Top;

  if (!PrintingSign)
  {
     PageGetLeftTop(&Left,&Top);
     return (ORDINATETYPE)(((ORDINATETYPE)Y)*GlobalPageScale/SCRY)-Top;
  }
  else
  {
      return (ORDINATETYPE)((long)Y*(long)SCALEMETER/(long)PrinterDPI)/PRNY;
  }

}

int UserYToWindowY(ORDINATETYPE Y)
{
  ORDINATETYPE Left,Top;

  if (!PrintingSign)
  {
     PageGetLeftTop(&Left,&Top);
     return  (int)(((Y+Top)/GlobalPageScale)*SCRY);
  }
  else
  {
     return (int)((long)Y*(long)PrinterDPI/(long)SCALEMETER)*PRNY;
  }
}

ORDINATETYPE myWindowXToUserX(int X)
{

  if (!PrintingSign)
     return (ORDINATETYPE)(((ORDINATETYPE)X)*GlobalPageScale/SCRX);
  else
     return (ORDINATETYPE)(((long)X*(long)SCALEMETER/(long)PrinterDPI)/PRNX);
}

int myUserXToWindowX(ORDINATETYPE X)
{

  if (!PrintingSign)
     return (int)(((X)/GlobalPageScale)*SCRX);
  else
     return (int)(((long)X*(long)PrinterDPI/(long)SCALEMETER)*PRNX);

}

ORDINATETYPE myWindowYToUserY(int Y)
{

  if (!PrintingSign)
     return (ORDINATETYPE)(((ORDINATETYPE)Y)*GlobalPageScale/SCRY);
  else
     return (ORDINATETYPE)((long)Y*(long)SCALEMETER/(long)PrinterDPI)/PRNY;

}

int myUserYToWindowY(ORDINATETYPE Y)
{

  if (!PrintingSign)
     return  (int)(((Y)/GlobalPageScale)*SCRY);
  else
     return (int)((long)Y*(long)PrinterDPI/(long)SCALEMETER)*PRNY;

}

/* Entry: PrintingSign must be 0 */
static void PageGetLeftTop(ORDINATETYPE *PageLeft,ORDINATETYPE *PageTop)
{
     *PageLeft=PAGELEFTDISTANT-GlobalPageHStart;
     *PageTop=PAGETOPDISTANT-GlobalPageVStart;
}

void PageChangeCurrent(HPAGE NewCurrentPage)
{
  GlobalCurrentPage=NewCurrentPage;
  if(NewCurrentPage>=0)
  {
    if (BoxSelect(GlobalCurrentPage,TextCursorPosX,TextCursorPosY)>0)
    {            // has selected text or picture or table form
       if (BoxCanEditable(GlobalBoxHeadHandle))
          BoxGetCursor(GlobalBoxHeadHandle,TextCursorPosX,TextCursorPosY,
                &GlobalTextPosition,&GlobalTextBlockStart,&GlobalTextBlockEnd);
    }
    if(GlobalCurrentPage==NewCurrentPage)      // if can change, redraw
       RedrawUserField();
  }
  TellStatus();               // ByHance
}

void PageDraw(HPAGE PageHandle,HWND Window,int DrawLeft,
              int DrawTop,int DrawRight,int DrawBottom)
{
  int SaveColor;
  struct viewporttype TmpViewPort;
  #ifdef __TURBOC__
    struct linesettingstype SaveLineStyle;
  #else
    unsigned short old_style;
  #endif

  ORDINATETYPE BoxDrawLeft,BoxDrawTop,BoxDrawRight,BoxDrawBottom;
  ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
  HBOX HBox;
  Pages *MidPage;
  int Left,Top,Right,Bottom;
  int PageLeft,PageTop,PageRight,PageBottom;

  WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
  if (!RectangleIsInRectangle(DrawLeft,DrawTop,DrawRight,DrawBottom,
          Left,Top,Right,Bottom))
     return;

  if (DrawRight>=Right-Left)
     DrawRight=Right-Left-1;
  if (DrawBottom>=Bottom-Top)
     DrawBottom=Bottom-Top-1;
  if (DrawLeft<0)
     DrawLeft=0;
  if (DrawTop<0)
     DrawTop=0;
  BoxDrawLeft=WindowXToUserX(DrawLeft);
  BoxDrawTop=WindowYToUserY(DrawTop);
  BoxDrawRight=WindowXToUserX(DrawRight);
  BoxDrawBottom=WindowYToUserY(DrawBottom);

  MidPage=HandleLock(ItemGetHandle(PageHandle));
  if (MidPage==NULL)
     return;
  PageLeft=UserXToWindowX(0);
  PageTop=UserYToWindowY(0);
  PageRight=UserXToWindowX(PageGetPageWidth(MidPage));
  PageBottom=UserYToWindowY(PageGetPageHeight(MidPage));

  if(GlobalTextBlockStart<GlobalTextBlockEnd) // must clear now block
    DisplayBlock(GlobalBoxHeadHandle,GlobalTextBlockStart,GlobalTextBlockEnd);

  SaveColor=getcolor();
  MouseHidden();
  TextCursorOff();
  getviewsettings(&TmpViewPort);

  setviewport(Left,Top,Right,Bottom,1);

  /*---- clear page -----*/
  setfillstyle(1,EGA_WHITE);
  bar(DrawLeft,DrawTop,DrawRight,DrawBottom);
  /*--- draw page border ---*/
  setcolor(PAGEBORDERCOLOR);
  rectangle(PageLeft,PageTop,PageRight,PageBottom);

  if(!IsInGlobalBrowser())
       DrawPageFootHead(PageHandle,TRUE,TRUE);

  if(!fEditor)
  {
      #ifdef __TURBOC__
        getlinesettings(&SaveLineStyle);
        setlinestyle(4,0x5555,1);
      #else
        old_style=getlinestyle();
        setlinestyle(0xffff);
      #endif

      /*--- draw page shadow ---*/
      setcolor(EGA_DARKGRAY);
      line(((PageLeft>0)?PageLeft:0)+3,PageBottom+1,PageRight+3,PageBottom+1);
      line(((PageLeft>0)?PageLeft:0)+3,PageBottom+2,PageRight+3,PageBottom+2);
      line(((PageLeft>0)?PageLeft:0)+3,PageBottom+3,PageRight+3,PageBottom+3);
      line(PageRight+1,((PageTop>0)?PageTop:0)+3,PageRight+1,PageBottom+3);
      line(PageRight+2,((PageTop>0)?PageTop:0)+3,PageRight+2,PageBottom+3);
      line(PageRight+3,((PageTop>0)?PageTop:0)+3,PageRight+3,PageBottom+3);

      /*--- draw box align line ---*/
      setcolor(EGA_LIGHTBLUE);
     /*------------  use 0x5555 --
      #ifdef __TURBOC__
        setlinestyle(4,0x5f5f,1);
      #else
        setlinestyle(0x5f5f);
      #endif
     ----------------*/
      setlinestyle(0x8888);
      PageLeft=UserXToWindowX(PageGetMarginLeft(MidPage))-1;
      PageTop=UserYToWindowY(PageGetMarginTop(MidPage))-1;
      PageRight=UserXToWindowX(PageGetPageWidth(MidPage)-
                               PageGetMarginRight(MidPage))+1;
      PageBottom=UserYToWindowY(PageGetPageHeight(MidPage)-
                                PageGetMarginBottom(MidPage))+1;
      line(UserXToWindowX(0),PageTop,
           UserXToWindowX(PageGetPageWidth(MidPage)),PageTop);
      line(UserXToWindowX(0),PageBottom,
           UserXToWindowX(PageGetPageWidth(MidPage)),PageBottom);
      line(PageLeft,UserYToWindowY(0),PageLeft,
           UserYToWindowY(PageGetPageHeight(MidPage)));
      line(PageRight,UserYToWindowY(0),PageRight,
           UserYToWindowY(PageGetPageHeight(MidPage)));

      #ifdef __TURBOC__
        setlinestyle(SaveLineStyle.linestyle,SaveLineStyle.upattern,
                     SaveLineStyle.thickness);
      #else
        setlinestyle(old_style);
      #endif
  }

  HandleUnlock(ItemGetHandle(PageHandle));

  setcolor(EGA_BLACK);

  HBox=PageGetBoxHead(PageHandle);
  while (HBox)
  {
    ORDINATETYPE BoxLeft,BoxTop,BoxRight,BoxBottom;
    TextBoxs *MidBox;
    int BoxDots;

    MidBox=HandleLock(ItemGetHandle(HBox));
    if (MidBox==NULL)
       break;
    BoxGetPolygonDrawBorder((Boxs *)MidBox,&BoxDots,BoxXY);
    PolygonGetMinRectangle(BoxDots,BoxXY,&BoxLeft,&BoxTop,&BoxRight,&BoxBottom);
    if (RectangleIsInRectangle(BoxDrawLeft,BoxDrawTop,BoxDrawRight,BoxDrawBottom,
         BoxLeft,BoxTop,BoxRight,BoxBottom)||TextBoxGetBoxType(MidBox)==TEXTBOX)
    {
       HBOX LinkBox;

       BoxDraw(HBox,
               (HBox==GlobalBoxHeadHandle)?DRAWBORDERWITHRECATNGLE:
               DRAWVIRTUALBORDOR);

       if (TextBoxGetBoxType(MidBox)==TEXTBOX &&
           (GlobalBoxTool==IDX_UNLINK||GlobalBoxTool==IDX_LINK) &&
           (LinkBox=TextBoxGetPrevLinkBox(MidBox))!=0 )
       {
          ORDINATETYPE Left2,Top2,Right2,Bottom2;

          BoxGetRect(HBox,&BoxLeft,&BoxTop,&BoxRight,&BoxBottom);
          BoxXY[0]=BoxLeft;     BoxXY[1]=BoxTop;
          BoxPolygonRotate(1,BoxXY,(PictureBoxs *)MidBox);
          BoxPolygonToWindowXY(1,BoxXY);
          BoxLeft=BoxXY[0];     BoxTop=BoxXY[1];

          if(ItemGetFather(LinkBox)==PageHandle)
          {
              BoxGetRect(LinkBox,&Left2,&Top2,&Right2,&Bottom2);
              BoxXY[0]=Right2;  BoxXY[1]=Bottom2;
              BoxPolygonRotate(1,BoxXY,HandleLock(ItemGetHandle(LinkBox)));
              HandleUnlock(ItemGetHandle(LinkBox));
              BoxPolygonToWindowXY(1,BoxXY);
          }
          else
          {
              BoxXY[0]=getmaxx();  BoxXY[1]=0;    // screen (Right,Top)
          }

          line(BoxLeft,BoxTop,BoxXY[0],BoxXY[1]);
       }
    }
    HandleUnlock(ItemGetHandle(HBox));
    HBox=ItemGetNext(HBox);
  }

  if (GlobalGroupGetSign())
     GroupDrawAllBorder(DRAWXORBORDER);
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  setcolor(SaveColor);
  MouseShow();
  return;
}

int BoxAutoAppdenPage(HBOX HBox)
{
  if (HBox&&ItemGetFather(HBox))
  {
     HPAGE MidHPage;
     Pages *MidPage;
     TextBoxs *TextBox1,*TextBox2;

     MidPage=HandleLock(ItemGetHandle(ItemGetFather(HBox)));
     if (MidPage==NULL)
        return(OUTOFMEMORY);
     MidHPage=PageNew(MidPage,PageHandleToNumber(ItemGetFather(HBox))+1);
     HandleUnlock(ItemGetHandle(ItemGetFather(HBox)));
     TextBox1=HandleLock(ItemGetHandle(HBox));
     if (TextBox1==NULL)
        return(OUTOFMEMORY);
     TextBox2=HandleLock(ItemGetHandle(ItemGetChild(MidHPage)));
     if (TextBox2==NULL)
     {
        HandleUnlock(ItemGetHandle(HBox));
        return(OUTOFMEMORY);
     }
     TextBoxSetNextLinkBox(TextBox1,ItemGetChild(MidHPage));
     TextBoxSetPrevLinkBox(TextBox2,HBox);
     SetAllLinkBoxTextHandle(HBox);
     TextBoxInitialLineTable(TextBox2);
     HandleUnlock(ItemGetHandle(ItemGetChild(MidHPage)));
     HandleUnlock(ItemGetHandle(HBox));
  }
  ReturnOK();
}

void ChangePageSetup(Pages *OldPage)
{
  ORDINATETYPE left,right,top,bottom,width,height,newx,newy,neww,newh;
  HPAGE HPage;
  HBOX HBox;
  TextBoxs *MidBox;
  int i;

  left=OldPage->MarginLeft;
  top=OldPage->MarginTop;
  right=OldPage->MarginRight;
  bottom=OldPage->MarginBottom;
  width=OldPage->PageWidth - left - right;
  height=OldPage->PageHeight - top - bottom;
  newx=TmpPage.MarginLeft;
  newy=TmpPage.MarginTop;
  neww=TmpPage.PageWidth-(TmpPage.MarginLeft+TmpPage.MarginRight);
  newh=TmpPage.PageHeight-(TmpPage.MarginTop+TmpPage.MarginBottom);

  for (i=0;i<TotalPage;i++)
  {
      HPage=PageNumberToHandle(i);
      HBox=PageGetBoxHead(HPage);
      while (HBox)
      {
         MidBox=HandleLock(ItemGetHandle(HBox));
         if (MidBox==NULL)
            return;

         if( TextBoxGetBoxType(MidBox)==TEXTBOX && TextBoxDependPage(MidBox) )
         {
            if(MidBox->BoxLeft==left && MidBox->BoxTop ==top
              && MidBox->BoxWidth ==width && MidBox->BoxHeight==height )
            {                   // find the box, now, change it
               MidBox->BoxLeft=newx;
               MidBox->BoxTop=newy;
               MidBox->BoxWidth=neww;
               MidBox->BoxHeight=newh;
               BoxChangeAll(HPage);
            }

            HandleUnlock(ItemGetHandle(HBox));
            break;    // only 1 depend_box in a page, so, goto next page
         }

         HandleUnlock(ItemGetHandle(HBox));
         HBox=ItemGetNext(HBox);
      }
  } //--- i ---
}


static int CalcStrLen(char *str)
{
   int i,wid;
   Wchar code;
   USHORT aw;
   short  lsb;

   i=wid=0;
   while( (code=str[i++]) )
   {
      if( code >= 0xa0 )           // It is a Chinese char.
      {
            if(str[i]<0xa0)  // ignore this code;
               continue;
            i++;
            wid+=CHAR_WIDTH_IN_DOT;
      }
      else           // It is an Englist char.
      {
            GetTtfWidth(code,0,&aw,&lsb);
            wid+=aw;
      }
   }
   return wid;
}

static void BuildStr(int x,int y,char *str,int width)
{
   int i;
   Wchar code;
   USHORT aw;
   short  lsb;

   i=0;
   while( (code=str[i++]) )
   {
       if( code >= 0xa0 )           // It is a Chinese char.
       {
             if(str[i]<0xa0)  // ignore this code;
                continue;

             code=(code<<8)|str[i++];
             BuildChineseChar(x,y, code, 0, width, width, 0,0, EGA_BLACK);
             x+=width;
       }
       else           // It is an Englist char.
       {
             BuildEnglishChar(x,y, code, 0, width, width, 0,0, EGA_BLACK);
             GetTtfWidth(code,0,&aw,&lsb);
             x+=(float)width*aw/CHAR_WIDTH_IN_DOT+0.5;
       }
   } /*-- end of while --*/

}

enum { NO_NUM, AFTER_NUM, PREV_NUM };
void DrawPageFootHead(HPAGE PageHandle,BOOL fDrawFoot,BOOL fDrawHead)
{
  Pages *MidPage;
  int   i,k,PageNum,x,y,PgFtWid,PgHdWid;
  Wchar code;
  int   width;
  int  fString=NO_NUM;
  BOOL fBothAtMid=FALSE;
  char str[PAGEHEADSTRMAXLEN+20],headstr[PAGEHEADSTRMAXLEN];

  MidPage=HandleLock(ItemGetHandle(PageHandle));
  PageNum=PageHandleToNumber(PageHandle)+PgFtStartNum;


  if(PrintingSign)
     width=UserXToWindowX(146);
  else
     width=myUserXToWindowX(146);      // /GlobalPageScale;      /* use 5' size */

  if(GetPageHeadOption()&&GetPageFootOption()&&GetPageFootTopOption()==FOOTTOP)
  {
     if((PageNum&1)==0)        // page_head_string may be at left
     {
       if((GetPageFootLeftOption()==FOOTLEFT||GetPageFootLeftOption()==FOOTLEFT_RIGHT)
       && (GetPageHeadLeftOption()==FOOTLEFT||GetPageHeadLeftOption()==FOOTLEFT_RIGHT) )
          fString=AFTER_NUM;    //- put it after PageNum string
       else
       if(GetPageFootLeftOption()==GetPageHeadLeftOption()) // middle, right
       {
          if(GetPageFootLeftOption()==FOOTMID)
             fBothAtMid=TRUE;
          fString=PREV_NUM;    //- put it prev to PageNum string
       }
     }
     else                      // page_head_string may be at right
     {
       if((GetPageFootLeftOption()==FOOTRIGHT||GetPageFootLeftOption()==FOOTLEFT_RIGHT)
       && (GetPageHeadLeftOption()==FOOTRIGHT||GetPageHeadLeftOption()==FOOTLEFT_RIGHT) )
          fString=PREV_NUM;    //- put it prev to PageNum string
       else
       if(GetPageFootLeftOption()==GetPageHeadLeftOption())
         if(GetPageFootLeftOption()==FOOTLEFT)
            fString=AFTER_NUM;    //- left, put it after PageNum string
         else
         {
            fString=PREV_NUM;    //-  middle, put it prev to PageNum string
            fBothAtMid=TRUE;
         }
     }
  }

  /*--- draw page foot (page number) ---*/
  if(GetPageFootOption() && (fDrawFoot || fString) )
  {
     // calculate page foot string's length
     itoa(PageNum,str,10);
      //-- insert prev<sp>, <sp>suff to str --
     switch(GetPageFootPrevOption())
     {
         case DOT_PREV:
              code='.';
              goto calu_prev;
         case LINE_PREV:
              code='-';
           calu_prev:
              i=strlen(str);
              memmove(str+2,str,i);
              str[0]=str[i+3]=code;
              str[1]=str[i+2]=BLANK;
              str[i+4]=0;
              break;
     }  /* end of switch */

     if(fBothAtMid)             //  ... <PageHeadStr><sp><sp><PageNum> ...
     {     // strcat(headstr,str), then build str.
         if((PageNum&1)!=0)        // page_head_string at right
             strcpy(headstr,PageHeadRightStr);
         else
             strcpy(headstr,PageHeadLeftStr);

         i=strlen(headstr);
         memmove(str+i+2,str,i);
         memmove(str,headstr,i);
         str[i+1]=str[i]=BLANK;
         fDrawFoot=TRUE;
     }
     PgFtWid=(float)CalcStrLen(str)*width/CHAR_WIDTH_IN_DOT+0.5;

     //--- calculate Y value ---
     if(GetPageFootTopOption()==FOOTTOP)
         y=UserYToWindowY(PageGetMarginTop(MidPage))-width;
     else
         y=UserYToWindowY(PageGetPageHeight(MidPage)
                         -PageGetMarginBottom(MidPage))+width*3/2;

     //--- calculate X value ---
     switch(GetPageFootLeftOption())
     {
         case FOOTLEFT:
              x=UserXToWindowX(PageGetMarginLeft(MidPage))+width;
              break;
         case FOOTRIGHT:
              PgFtWid+=width;
              x=UserXToWindowX(PageGetPageWidth(MidPage)
                            -PageGetMarginRight(MidPage))
                   -PgFtWid;
              break;
         case FOOTLEFT_RIGHT:
              if( (PageNum&1)==0)     // odd, put PageNum at left
                  x=UserXToWindowX(PageGetMarginLeft(MidPage))+width;
              else              // even, put it at right
              {
                  PgFtWid+=width;
                  x=UserXToWindowX(PageGetPageWidth(MidPage)
                                -PageGetMarginRight(MidPage))
                    -PgFtWid;
              }
              break;
         case FOOTMID:
              x=UserXToWindowX(PageGetPageWidth(MidPage)
                            -PageGetMarginRight(MidPage))
                   -PgFtWid;
              x+=UserXToWindowX(PageGetMarginLeft(MidPage));
              x/=2;
              break;
     }  /* end of switch */

     /*----- now, draw page foot string ------*/
     if(fDrawFoot)
         BuildStr(x,y,str,width);

     if(fBothAtMid)
         goto lbl_draw_page_head_line;

     x+=PgFtWid;
  }  /*-- page foot --*/

  /*--- draw page head string ----*/
  if(GetPageHeadOption() && fDrawHead)
  {
     if((PageNum&1)!=0)
         strcpy(str,PageHeadRightStr);
     else
         strcpy(str,PageHeadLeftStr);

     PgHdWid=(float)CalcStrLen(str)*width/CHAR_WIDTH_IN_DOT+0.5;

     if(fString==AFTER_NUM)
         x+=width*3/2;          // use last_modified_X, add some SPACE
     else
     switch(GetPageHeadLeftOption())
     {
       case FOOTLEFT:
       lbl_head_left:
            x=UserXToWindowX(PageGetMarginLeft(MidPage));
            break;
       case FOOTMID:
            x=UserXToWindowX(PageGetPageWidth(MidPage)
                          -PageGetMarginRight(MidPage))
                 -PgHdWid;
            x+=UserXToWindowX(PageGetMarginLeft(MidPage));
            x/=2;
            break;
       case FOOTRIGHT:
       lbl_head_right:
            x=UserXToWindowX(PageGetPageWidth(MidPage)
                          -PageGetMarginRight(MidPage))
                  - PgHdWid;

            if(fString==PREV_NUM)       // page num is at right also
                x-=PgFtWid+width*3/2;     // add some SPACE
            break;
       case FOOTLEFT_RIGHT:
            if((PageNum&1)!=0)        // page_head_string at right
               goto lbl_head_right;
            else
               goto lbl_head_left;
     } /*- end of switch -*/

     y=UserYToWindowY(PageGetMarginTop(MidPage))-width;
     BuildStr(x,y,str,width);

     // if page head string is not NULL, draw a line
     if(str[0]&&GetPageHeadLineOption())
     {
   lbl_draw_page_head_line:
         x=UserXToWindowX(PageGetMarginLeft(MidPage));
         i=UserXToWindowX(PageGetPageWidth(MidPage)
                           -PageGetMarginRight(MidPage));
         y+=width/4;
         if(PrintingSign)
         {
             printer->printScanLine(x,i,y,&SysDc);
             printer->printScanLine(x,i,y+1,&SysDc);
         }
         else
         {
             setcolor(EGA_BLACK);
             line(x,y,i,y);
         }

         if(GetPageHeadLineOption()==HEADLINE_DOUBLE)
         {
             y+=width/6;
             for(k=0;k<width/6;k++)
                 if(PrintingSign)
                     printer->printScanLine(x,i,y+k,&SysDc);
                 else
                 {
                     line(x,y+k,i,y+k);
                 }
         }
     } /*--- page head line ---*/
  } /*-- if pagehead --*/

  HandleUnlock(ItemGetHandle(PageHandle));
}


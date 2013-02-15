/*-------------------------------------------------------------------
* Name: printc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"
#include "pattn6.h"

int NewPPageMain(int PageNum);

int PrinterBlockTop[200],PrinterBlockBottom[200];
int CurrentPrinterBlock;

// extern PRINTER NULprinter;
extern PRINTER BJCprinter;
extern PRINTER StarCRprinter;
//extern PRINTER BJC360printer;
//extern PRINTER BJC720printer;
extern PRINTER HPA3_300printer;
extern PRINTER HPA4_300printer;
extern PRINTER HPA3_600printer;
extern PRINTER HP2A4_300printer;            //By zjh
extern PRINTER HPA4_600printer;
extern PRINTER P6100_300printer;
extern PRINTER P6500_300printer;


extern PRINTER OKIprinter;
extern PRINTER M1724printer;
extern PRINTER LQBWprinter;
extern PRINTER LQColorprinter;
extern PRINTER HP1200_300printer;
extern PRINTER EPSON360SCprinter;
extern PRINTER EPSON720SCprinter;
extern PRINTER ESCP2printer;
extern PRINTER BJ10eprinter;
extern PRINTER FAXprinter;
extern unsigned char tab720[],tab360[];

void SetDeviceColor(unsigned long Color,int CharSign)
{
   if (PrintingSign)
   {
      if(printer->DeviceType==DEV_BW && Color!=EGA_WHITE && Color!=EGA_BLACK)
      {
         if(CharSign) (printer->printSetColor)(EGA_BLACK);
         else (printer->printSetGray)(Color);
      }
      else
         (printer->printSetColor)(Color);
   }
   else
      setcolor(Color);
}


int PPage(HPAGE HPage)
{
  HBOX HBox;
  int BoxDrawLeft,BoxDrawTop,BoxDrawRight,BoxDrawBottom;
  int Left,Top,Right,Bottom;   //By zjh 9.7
  UDATA pageh,pagew;    //By zjh 9.7
  int BlockHeight,RasterHeight;
  HPAGE SavePageN=GlobalCurrentPage;
  Pages *MidPage;
  BOOL fDrawFoot,fDrawHead;
  int PageFootY1,PageFootY2;
  int PageHeadY1,PageHeadY2;
  int width=UserXToWindowX(146);      /* use 5' size */

  #ifdef REGIST_VERSION
   int Result,n;
  #endif

  if (!HPage)
     ReturnOK();

  GlobalCurrentPage=HPage;

  //PrintingSign=0;

  if(printer->DeviceType==DEV_RGB)
      memset(RowErrBuf,0,DitherBufLen);

 /*------- ByHance, 96,3.8 -------*/
  MidPage=HandleLock(ItemGetHandle(HPage));
  if(GetPageFootOption())
  {
     if(GetPageFootTopOption()==FOOTTOP)
         PageFootY2=UserYToWindowY(PageGetMarginTop(MidPage))-width;
     else
         PageFootY2=UserYToWindowY(PageGetPageHeight(MidPage)
                         -PageGetMarginBottom(MidPage))+width*3/2;

     PageFootY1=PageFootY2-width;
  }

  if(GetPageHeadOption())
  {
     PageHeadY1=UserYToWindowY(PageGetMarginTop(MidPage))-width-width;
     PageHeadY2=UserYToWindowY(PageGetMarginTop(MidPage))-2;
  }

  pagew=PageGetPageWidth(MidPage);   //By zjh 96.9.7
  pageh=PageGetPageHeight(MidPage);  //By zjh 96.9.7

  HandleUnlock(ItemGetHandle(HPage));
  /*------- end, ByHance, 96,3.8 -------*/

  //------ SysDc unit is DOT, but BoxDraw unit is INCH ----
  BlockHeight = (printer->printGetBlockHeight)();
  SysDc.left=0;
  SysDc.right=printer->xpixel;
  SysDc.top = printer->TopMargin;
  SysDc.bottom = SysDc.top + BlockHeight;

  Left=0;
  Right=WindowXToUserX(SysDc.right);         // unit: Inch/1000
  Top=WindowYToUserY(SysDc.top);       //ByHance, 96,1.24
  Bottom=WindowYToUserY(SysDc.bottom);
  PageHightDot=UserYToWindowY(pageh);

  GlobalXOffset=0;
  GlobalYOffset=0;
  GlobalYReverse=0;

  if (GlobalRorate90)
  {
    GlobalYRes=(UserXToWindowX(pagew)/2+GlobalYOffset)*2;
    GlobalXRes=(UserYToWindowY(pageh)/2+GlobalXOffset)*2;
  }
  else
  {
    GlobalYRes=(UserYToWindowY(pageh)/2+GlobalYOffset)*2;
    GlobalXRes=(UserXToWindowX(pagew)/2+GlobalXOffset)*2;
  }

  //PageHightDot=(long)pageh*(long)PrinterDPI/SCALEMETER;
  //if (PageHightDot<SysDc.right) PageHightDot+=(SysDc.right-PageHightDot)/2;
  if (PageHightDot>SysDc.right) PageHightDot=SysDc.right;


  //x=y'
  //y=H-x'
  if (GlobalRorate90)
  {
      BoxDrawLeft=min(Top,Bottom);
      BoxDrawRight=max(Top,Bottom);
      BoxDrawTop=0;
      BoxDrawBottom=Right;
  }
  else
  {
      BoxDrawLeft=min(Left,Right);
      BoxDrawRight=max(Right,Left);
      BoxDrawTop=min(Top,Bottom);
      BoxDrawBottom=max(Top,Bottom);
  }

  RasterHeight=printer->ypixel;
   // while (BoxDrawBottom<DrawHeight)
  // while (SysDc.bottom<=printer->ypixel)
  while (SysDc.top<=RasterHeight)
  {
    if (GlobalRorate90 && PrintingSign)
    {
        myDC.left=SysDc.top;
        myDC.right=SysDc.bottom;
        myDC.top=0;
        myDC.bottom=PageHightDot;
    }
    else
        memcpy(&myDC,&SysDc,sizeof(myDC));

    if (myDC.left<0) myDC.left=0;
    if (myDC.top<0) myDC.top=0;

    fDither=FALSE;
    MaxRastY=-1;

 /*------- ByHance, 96,3.8 -------*/
    fDrawFoot=fDrawHead=FALSE;
    if(GetPageFootOption())
        fDrawFoot=TRUE;

    if(GetPageHeadOption())
   // && RectangleIsInRectangle(1,PageFootY1,2,PageFootY2,0,SysDc.top,3,SysDc.bottom) )
        fDrawHead=TRUE;

    if(fDrawFoot||fDrawHead)
      DrawPageFootHead(HPage,fDrawFoot,fDrawHead);


    HBox=PageGetBoxHead(HPage);
    while (HBox>0)
    {
      ORDINATETYPE BoxLeft,BoxTop,BoxRight,BoxBottom;
      ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
      TextBoxs *MidBox;
      int BoxDots;

      MidBox=HandleLock(ItemGetHandle(HBox));
      if (MidBox==NULL)
         break;
      BoxGetPolygonDrawBorder((Boxs *)MidBox,&BoxDots,BoxXY);
      PolygonGetMinRectangle(BoxDots,BoxXY,&BoxLeft,&BoxTop,&BoxRight,&BoxBottom);

      if (RectangleIsInRectangle(min(BoxLeft,BoxRight),
                                 min(BoxTop,BoxBottom),
                                 max(BoxLeft,BoxRight),
                                 max(BoxTop,BoxBottom),
                  BoxDrawLeft,BoxDrawTop,BoxDrawRight,BoxDrawBottom)
          //||TextBoxGetBoxType(MidBox)==TEXTBOX
      )
      {
           //if (GlobalRorate90) draw.Enable=1;
           BoxDraw(HBox,0);
           //draw.Enable=0;
      }
      HandleUnlock(ItemGetHandle(HBox));
      HBox=ItemGetNext(HBox);
    }

  #ifdef REGIST_VERSION
    n=0;
    Result=0xff&(serial[0]-regist_str[0]+serial[2]-regist_str[2]);
      // it must be 0
    do {
      (printer->printBlock)();     // send a block of data, clear block buffer
      n++;
    } while (n<Result);
  #else
    (printer->printBlock)();     // send a block of data, clear block buffer
  #endif   // REGIST_VERSION

    SysDc.top+=BlockHeight;
    SysDc.bottom+=BlockHeight;

    if (GlobalRorate90)
    {
        BoxDrawLeft=min(WindowYToUserY(SysDc.top),WindowYToUserY(SysDc.bottom));
        BoxDrawRight=max(WindowYToUserY(SysDc.top),WindowYToUserY(SysDc.bottom));
    }
    else
    {
        BoxDrawTop=min(WindowYToUserY(SysDc.top),WindowYToUserY(SysDc.bottom));
        BoxDrawBottom=max(WindowYToUserY(SysDc.top),WindowYToUserY(SysDc.bottom));
    }

    if (BoxDrawLeft<0) BoxDrawLeft=0;
    if (BoxDrawTop<0) BoxDrawTop=0;
  }

  //if (!GlobalPause)                    //Add By zjh 12.2
  (printer->printFormFeed)();                     // eject a page

  GlobalCurrentPage=SavePageN;
  ReturnOK();
} /* PPage */


static int Print2LPT();

#define  d_putdot(lpdc,x,y)     (printer->printScanLine(x,x,y,lpdc))
static void d_sline(LPDC lpdc,int x1,int y1,int x2,int y2)
{
    int x,y,xend,yend;
    int dx,dy,d,incr1,incr2;
    int flag1,flag2;
    int left,right,top,bottom;

    if (abs(x1-x2)<=abs(y1-y2)) return;
    if(y1==y2)
    {
        if(x1>x2) { x=x1; x1=x2; x2=x; }
        printer->printScanLine(x1,x2,y1,lpdc);
        return;
    }
    /*----------------clip the line -----------*/
    left = lpdc->left;
    right = lpdc->right;
    top = lpdc->top;
    bottom = lpdc->bottom;

    flag1 = flag2 = 0;
    dx =x2-x1;
    dy = y2-y1;

    if (x1<left) flag1 |= 1;
    if (x1>=right) flag1 |= 2;
    if (y1<top) flag1 |= 4;
    if (y1>=bottom) flag1 |=8;

    if (x2<left) flag2 |= 1;
    if (x2>=right) flag2 |= 2;
    if (y2<top) flag2 |= 4;
    if (y2>=bottom) flag2 |=8;

    if (flag1|flag2) {
         if (flag1&flag2) return;
         if (flag1&1){
              y1 = y1+dy*(left-x1)/dx;
              x1 = left;
         } else if (flag1&2) {
              y1 = y1+dy*(right-x1-1)/dx;
              x1 = right-1;
         } else if (flag1&4) {
              x1 = x1+dx*(top-y1)/dy;
              y1 = top;
         } else if (flag1&8) {
              x1 = x1+dx*(bottom-y1-1)/dy;
              y1 = bottom-1;
         }

         if (flag2&1){
              y2 = y2+dy*(left-x2)/dx;
              x2 = left;
         } else if (flag2&2) {
              y2 = y2+dy*(right-x2-1)/dx;
              x2 = right-1;
         } else if (flag2&4) {
              x2 = x2+dx*(top-y2)/dy;
              y2 = top;
         } else if (flag2&8) {
              x2 = x2+dx*(bottom-y2-1)/dy;
              y2 = bottom-1;
         }
    }

    if (x1<x2) {
           x = x1; y=y1;
           xend = x2; yend=y2;
    } else {
           x = x2; y=y2;
           xend = x1; yend=y1;
    }

    dx = abs(xend-x);
    dy = abs(yend-y);
    d = (dy<<1)-dx;
    incr1 = dy<<1;
    incr2 = (dy-dx)<<1;

    d_putdot(lpdc,x,y);
    if (y<yend)
         while (x++<xend) {
            if (d<0) d+=incr1;
            else {
               y++;
               d+=incr2;
            }
            d_putdot(lpdc,x,y);
         }
    else
         while (x++<xend) {
            if (d<0) d+=incr1;
            else {
               y--;
               d+=incr2;
            }
            d_putdot(lpdc,x,y);
         }
}

int SubPage;
int SubPageNum=1;

int PrintToDevice(int PageStart,int PageEnd)
{
  int i,copyN,j;
  HPAGE PrintingPage;
  EdgeFillLine *SaveEdgeFillLine;
  LineFillLine *SaveLineFillLine;
  int No_PrintFile;
  DC SaveDc;
  int save_pt_w,save_pt_h;

  MouseSetGraph(BUSYMOUSE);
  memcpy(&SaveDc,&SysDc,sizeof(DC));         //Bug for hollw after print
  SaveEdgeFillLine=CurrentEdgeFillLine;
  SaveLineFillLine=CurrentLineFillLine;
  save_pt_w=printer->xpixel;
  save_pt_h=printer->ypixel;

  // Using default PolyPolygon fill function to draw text.
  // CurrentEdgeFillLine=NULLProcedure;   // printer->printEdgeLine;
  // CurrentEdgeFillLine=NULL;
  CurrentEdgeFillLine=d_sline;
  CurrentLineFillLine=printer->printScanLine;

  DestoryCache();

  copyN=PrintCopyN;
  if(CurrentPrinter==HP_LJ_A4_300
   ||CurrentPrinter==HP_LJ_A4_600
   ||CurrentPrinter==HP_LJ_A3_300
   ||CurrentPrinter==HP_LJ_A3_600
   ||CurrentPrinter==BJ_LJ_A3_300
   ||CurrentPrinter==BJ_LJ_A3_400
   ||CurrentPrinter==BJ_LJ_A3_600
   ||CurrentPrinter==BJ_LJ_A4_300
   ||CurrentPrinter==BJ_LJ_A4_400
   ||CurrentPrinter==BJ_LJ_A4_600
   ||CurrentPrinter==BJ_LJ_B4_300
   ||CurrentPrinter==BJ_LJ_B4_360
   ||CurrentPrinter==BJ_LJ_B4_400
   ||CurrentPrinter==BJ_LJ_B4_600
   )
        copyN=1;
  else
        PrintCopyN=1;

  PrintingSign=1;
  No_PrintFile=0;
  if (prnstr==NULL) No_PrintFile=1;

  if (!GlobalJob && !GetPrint2FileOption() && !No_PrintFile)
  {
    fclose(prnstr);
    j=Print2LPT();
    prnstr=fopen(PrintName,"wb");
    if (j) goto pr_over;
  }

  for (i=PageStart;i<=PageEnd;i++)
   for (SubPage=0;SubPage<SubPageNum;SubPage++)   //By zjh 1997.4.13
   {
      if (SubPageNum>1 && (PG.PageBlock[SubPage].Reveser&1)) continue;
      PrintingPage=PageNumberToHandle(i);
      if (UsePrintCut())
      {
          PrintingPage=NewPPageMain(i);
          if (PG.PageInc>0) i += PG.PageInc-1;
      }
      else
          PrintingPage=PPage(PrintingPage);

      if (!GlobalJob && !GetPrint2FileOption() && !No_PrintFile)
      {
          fclose(prnstr);

          for (j=0;j<copyN;j++)
          {
              if(Print2LPT())
              {
                    prnstr=fopen(PrintName,"wb");
                    goto pr_over;
              }

              if (GlobalPause && (i<PageEnd||j<copyN-1) )
              {
                  char stt[100];
                  sprintf(stt,"按任意键继续第 %d 页打印",i+2);
                  if (MessageBox("继续打印",stt,2,1)) goto pr_over;
                  MouseSetGraph(BUSYMOUSE);
              }
          }
          prnstr=fopen(PrintName,"wb");
      }

      if (PrintingPage<0) break;
  }

 pr_over:
  (printer->printOver)();

  DestoryCache();

  if(!GetPrint2FileOption() && GlobalJob && !No_PrintFile)
     for (i=0;i<copyN;i++)
       if(Print2LPT()) break;

  PrintingSign=0;

  // Restore original Polypolygon fill function.
  CurrentEdgeFillLine=SaveEdgeFillLine;
  CurrentLineFillLine=SaveLineFillLine;
  memcpy(&SysDc,&SaveDc,sizeof(DC));         //Bug for hollw after print 12.4/96
  printer->xpixel=save_pt_w;
  printer->ypixel=save_pt_h;

  if (SubPageNum>1)
  {
     PG.Enable=0;             //By zjh 1997.4.13
     PG.Blocks=0;
     GlobalXPage=0;
     GlobalYPage=0;
  }

  SubPageNum=1;
  if(PrintCopyN==1)     // if has been modified, restore old value
     PrintCopyN=copyN;

  return(PrintingPage);
} /* PrintToDevice */


static BOOL fCheckStatus;
static int Print2LPT()
{
    FILE *fp;
    int ch;
    unsigned short rt, KeyShiftStatus;
    unsigned char st;
    union REGS r;
    char str[256];

    fp=fopen(PrintName,"rb");
    if(fp==NULL)
    {
       strcpy(str,"打印出错,请退出系统用\n CHKDSK \n检查硬盘空间!\n");
       MessageBox(GetTitleString(WARNINGINFORM),
                  str,1,1);
       return 1;
    }

    rt=0;
    while( (ch=fgetc(fp)) != EOF )
    {
        do {
           if(_bios_keybrd(0x11))     //  if(KeyPressed())
           {
               GetKey(&rt,&KeyShiftStatus);
               if(rt==CTRL_U||rt==ESC)
               {
                  rt=MessageBox(GetTitleString(WARNINGINFORM),
                              "真的停止打印?",
                              2,1);
                  if(rt==0)             // cancel print
                  {
                    rt=1;
                    goto err_exit;
                  }
                  MouseSetGraph(BUSYMOUSE);
               }
           }

           r.h.ah = 2;          // get printer status
           r.w.dx = 0;
           int386(0x17,&r,&r);

           st=r.h.ah;
           if((fCheckStatus && (st&0x20)) || (st&0x8) )   /* out of paper */
           {
                rt=MessageBox(GetTitleString(WARNINGINFORM),
                              "打印机未准备好, 再试一次?",
                              2,1);

                if(rt==1)       // cancel print
                   goto err_exit;
                MouseSetGraph(BUSYMOUSE);
           } else
           if(st&0x80) break;  /* not busy */
        } while(1);

        r.h.ah = 0;             // send char in AL to printer LPT1
        r.h.al = ch;
        r.w.dx = 0;
        int386(0x17,&r,&r);
    }

    // rt=0;
  err_exit:
    if(rt && GlobalJob)
    {
      sprintf(str,"文件已被打印到:\n  %s\n退出系统后可使用\n copy /b %s prn\n进行输出",
                PrintName, PrintName);
      MessageBox(GetTitleString(WARNINGINFORM),str,1,1);
    }

    fclose(fp);
    return(rt);
}

int NewPPage(HPAGE HPage,int Num)
{
  HBOX HBox;
  int BoxDrawLeft,BoxDrawTop,BoxDrawRight,BoxDrawBottom;
  int Left,Top,Right,Bottom;                            //By zjh 9.7
  UDATA pageh,pagew;                                    //By zjh 9.7
  // int Result,n;
  int BlockHeight,RasterHeight;
  HPAGE SavePageN=GlobalCurrentPage;

  Pages *MidPage;
  BOOL fDrawFoot,fDrawHead;
  int PageFootY1,PageFootY2;
  int PageHeadY1,PageHeadY2;
  int width=UserXToWindowX(146);      /* use 5' size */

  if (!HPage)
     ReturnOK();

  CurrentPrinterBlock=Num;

  GlobalCurrentPage=HPage;

  if(printer->DeviceType==DEV_RGB)
      memset(RowErrBuf,0,DitherBufLen);

  MidPage=HandleLock(ItemGetHandle(HPage));
  if(GetPageFootOption())
  {
     if(GetPageFootTopOption()==FOOTTOP)
         PageFootY2=UserYToWindowY(PageGetMarginTop(MidPage))-width;
     else
         PageFootY2=UserYToWindowY(PageGetPageHeight(MidPage)
                         -PageGetMarginBottom(MidPage))+width*3/2;

     PageFootY1=PageFootY2-width;
  }

  if(GetPageHeadOption())
  {
     PageHeadY1=UserYToWindowY(PageGetMarginTop(MidPage))-width-width;
     PageHeadY2=UserYToWindowY(PageGetMarginTop(MidPage))-2;
  }

  pagew=PageGetPageWidth(MidPage);   //By zjh 96.9.7
  pageh=PageGetPageHeight(MidPage);  //By zjh 96.9.7

  HandleUnlock(ItemGetHandle(HPage));

  //------ SysDc unit is DOT, but BoxDraw unit is INCH ----
  BlockHeight = (printer->printGetBlockHeight)();
  RasterHeight=printer->ypixel;

  SysDc.left=0;
  SysDc.top =0;
  if (GlobalRorate90)
  {
    SysDc.right=max(printer->xpixel,UserYToWindowY(pageh));       //for - offset and enlarge PX PY
    SysDc.bottom =max(printer->ypixel,UserXToWindowX(pagew));
  }
  else
  {
    SysDc.right=max(printer->xpixel,UserXToWindowX(pagew));
    SysDc.bottom =max(printer->ypixel,UserYToWindowY(pageh));
  }
  //SysDc.top = PrinterBlockTop[Num];
  //SysDc.bottom = PrinterBlockBottom[Num];

  //if (SysDc.bottom>RasterHeight) SysDc.bottom=RasterHeight;
  //if (SysDc.top>=RasterHeight) goto no_print;

  //Left=0-GlobalXOffset;
  //Right=WindowXToUserX(SysDc.right-GlobalXOffset);         // unit: Inch/1000
  //Top=WindowYToUserY(SysDc.top-GlobalYOffset);             //ByHance, 96,1.24
  //Bottom=WindowYToUserY(SysDc.bottom-GlobalYOffset);

  Left=SysDc.left;
  Right=WindowXToUserX(SysDc.right);         // unit: Inch/1000
  Top=WindowYToUserY(SysDc.top);             //ByHance, 96,1.24
  Bottom=WindowYToUserY(SysDc.bottom);
  PageHightDot=UserYToWindowY(pageh);

  if (GlobalRorate90)
  {
    GlobalYRes=(UserXToWindowX(pagew)/2+GlobalYOffset)*2;
    GlobalXRes=(UserYToWindowY(pageh)/2+GlobalXOffset)*2;
  }
  else
  {
    GlobalYRes=(UserYToWindowY(pageh)/2+GlobalYOffset)*2;
    GlobalXRes=(UserXToWindowX(pagew)/2+GlobalXOffset)*2;
  }

  //PageHightDot=(long)pageh*(long)PrinterDPI/SCALEMETER;
  //if (PageHightDot<SysDc.right) PageHightDot+=(SysDc.right-PageHightDot)/2;
  if (PageHightDot>SysDc.right) PageHightDot=SysDc.right;

  //x=y'
  //y=H-x'
  if (GlobalRorate90)
  {
    BoxDrawLeft=min(Top,Bottom);
    BoxDrawRight=max(Top,Bottom);
    BoxDrawTop=Left;
    BoxDrawBottom=Right;
  }
  else
  {
    BoxDrawLeft=min(Left,Right);
    BoxDrawRight=max(Right,Left);
    BoxDrawTop=min(Top,Bottom);
    BoxDrawBottom=max(Top,Bottom);
  }

  //while (SysDc.top<=RasterHeight)
  {
    if (GlobalRorate90 && PrintingSign)
    {
        myDC.left=SysDc.top;
        myDC.right=SysDc.bottom;
        myDC.top=0;
        myDC.bottom=PageHightDot;
    }
    else
        memcpy(&myDC,&SysDc,sizeof(myDC));

    if (myDC.left<0) myDC.left=0;
    if (myDC.top<0) myDC.top=0;

    fDither=FALSE;
    MaxRastY=-1;

    fDrawFoot=fDrawHead=FALSE;
    if(GetPageFootOption())
        fDrawFoot=TRUE;

    if(GetPageHeadOption())
        fDrawHead=TRUE;

    if(fDrawFoot||fDrawHead)
      DrawPageFootHead(HPage,fDrawFoot,fDrawHead);

    HBox=PageGetBoxHead(HPage);

    while (HBox>0)
    {
      ORDINATETYPE BoxLeft,BoxTop,BoxRight,BoxBottom;
      ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
      TextBoxs *MidBox;
      int BoxDots;

      MidBox=HandleLock(ItemGetHandle(HBox));
      if (MidBox==NULL)
         break;
      BoxGetPolygonDrawBorder((Boxs *)MidBox,&BoxDots,BoxXY);
      PolygonGetMinRectangle(BoxDots,BoxXY,&BoxLeft,&BoxTop,&BoxRight,&BoxBottom);

      if (RectangleIsInRectangle(min(BoxLeft,BoxRight),
                                 min(BoxTop,BoxBottom),
                                 max(BoxLeft,BoxRight),
                                 max(BoxTop,BoxBottom),
                  BoxDrawLeft,BoxDrawTop,BoxDrawRight,BoxDrawBottom) )
      {
         BoxDraw(HBox,0);
      }
      HandleUnlock(ItemGetHandle(HBox));
      HBox=ItemGetNext(HBox);
    }
  }

 // no_print:
  GlobalCurrentPage=SavePageN;
  ReturnOK();
} /* PPage */

int NewPPageMain(int PageNum)
{
    HPAGE PrintingPage;
    int i,j,n,Num,SaveRev,SaveR;
    int RasterHeight,BlockHeight;
    float saveX,saveY;
    DC SaveDc;

    saveX=XScale;
    saveY=YScale;
    SaveR=GlobalRorate90;
    // GlobalReverse &= 1;

    BlockHeight = (printer->printGetBlockHeight)();
    RasterHeight=printer->ypixel;

    n=0;
    i=printer->TopMargin;
    while (i<RasterHeight)
    {
       j = min(i+BlockHeight,RasterHeight);
       PrinterBlockTop[n]=i;
       PrinterBlockBottom[n++]=j;
       i += BlockHeight;
    }

    Num=n;
    for (j=0;j<Num;j++)
    {
      for (i=0;i<PG.Blocks;i++)
      {
         if (SubPageNum>1 && i!=SubPage) continue;

         PrintingPage=PageNumberToHandle(PageNum+PG.PageBlock[i].PageOffset);

         XScale=PG.PageBlock[i].Xscale;
         YScale=PG.PageBlock[i].Yscale;
         if (SubPageNum>1)
         {
           GlobalXOffset=((int)(PG.PageBlock[i].Xoffset*PrinterDPI/25.4));
           GlobalYOffset=((int)(PG.PageBlock[i].Yoffset*PrinterDPI/25.4));
         }
         else
         {
           GlobalXOffset=((int)(PXScale*PG.PageBlock[i].Xoffset*PrinterDPI/25.4));
           GlobalYOffset=((int)(PYScale*PG.PageBlock[i].Yoffset*PrinterDPI/25.4));
         }

         SaveRev=GlobalReverse;

         switch (PG.PageBlock[i].Rotate%4+GlobalReverse*4)
         {
              case 0:
                       GlobalYReverse=0;
                       GlobalReverse=0;
                       GlobalRorate90=0;
                       break;
              case 1:
                       GlobalYReverse=0;
                       GlobalReverse=0;
                       GlobalRorate90=1;
                       break;
              case 2:
                       GlobalYReverse=1;
                       GlobalReverse=1;
                       GlobalRorate90=0;
                       break;
              case 3:
                       GlobalYReverse=1;
                       GlobalReverse=1;
                       GlobalRorate90=1;
                       break;
              case 4:
                       GlobalYReverse=0;
                       GlobalReverse=1;
                       GlobalRorate90=0;
                       break;
              case 5:
                       GlobalYReverse=0;
                       GlobalReverse=1;
                       GlobalRorate90=1;
                       break;
              case 6:
                       GlobalYReverse=1;
                       GlobalReverse=0;
                       GlobalRorate90=0;
                       break;
              case 7:
                       GlobalYReverse=1;
                       GlobalReverse=0;
                       GlobalRorate90=1;
                       break;
              default:
                       GlobalYReverse=0;
                       break;
         }  /*- end of switch -*/

         //if (GlobalYReverse)
         //   PrintingPage=NewPPage(PrintingPage,Num-j-1);
         //else
            PrintingPage=NewPPage(PrintingPage,j);

         GlobalReverse=SaveRev;
      }  /*-- i --*/

      memcpy(&SaveDc,&SysDc,sizeof(DC));
      SysDc.right=min(RastWidth,printer->xpixel);
      SysDc.left=0;
      SysDc.top=PrinterBlockTop[CurrentPrinterBlock];
      SysDc.bottom=PrinterBlockBottom[CurrentPrinterBlock];
      (printer->printBlock)();
      memcpy(&SysDc,&SaveDc,sizeof(DC));
    } /*---- j ----*/

    XScale=saveX;
    YScale=saveY;
    GlobalRorate90=SaveR;

    (printer->printFormFeed)();                     // eject a page
    ReturnOK();
}




#define SUBS_OK  0
#define TOO_MANY_SUBS -1
#define SUBS_PARA_ERR -2
#define NOMAL_SUBS -3

static int PageToSubPage(PPB Source)
{
  int pg,i,j;
  int WidthInUse=Source->PrintPaperWidth-Source->LeftMargin-\
                 Source->RightMargin-Source->LayX;
  int HeightInUse=Source->PrintPaperHeight-Source->TopMargin-\
                 Source->BottomMargin-Source->LayY;
  int PrintWidth=abs(Source->PagePrintX1-Source->PagePrintX0);
  int PrintHeight=abs(Source->PagePrintY1-Source->PagePrintY0);
  int CountX=(PrintWidth-Source->LayX-1)/WidthInUse+1;
  int CountY=(PrintHeight-Source->LayY-1)/HeightInUse+1;
  int LeftX,LeftY;

  if (Source->AutoCenter)
  {
       LeftX=CountX*WidthInUse+Source->LayX-PrintWidth;
       LeftY=CountY*HeightInUse+Source->LayY-PrintHeight;
  }
  else { LeftX=0; LeftY=0; }

  Source->TotalX=CountX*WidthInUse+\
          Source->LayX+Source->LeftMargin+Source->RightMargin;
  Source->TotalY=CountY*HeightInUse+\
          Source->LayY+Source->TopMargin+Source->BottomMargin;

  PG.Blocks=CountX*CountY;
  if (CountX<1||CountY<1) return SUBS_PARA_ERR;
  if (PG.Blocks==1) return NOMAL_SUBS;
  if (PG.Blocks>=199) return TOO_MANY_SUBS;
  for (i=0;i<CountY;i++)
   for (j=0;j<CountX;j++)
   {
      pg=i*CountX+j;
      PG.PageBlock[pg].Xscale=1.0;
      PG.PageBlock[pg].Yscale=1.0;
      PG.PageBlock[pg].Xoffset=
            (Source->LeftMargin - Source->PagePrintX0-j*WidthInUse+LeftX/2)
              /SCALEMETER*25.4;
      PG.PageBlock[pg].Yoffset=(Source->TopMargin-Source->PagePrintY0\
                                      -i*HeightInUse+LeftY/2)/SCALEMETER*25.4;
      PG.PageBlock[pg].PageOffset=0;
      PG.PageBlock[pg].Rotate=0;
      PG.PageBlock[pg].Reveser=0;
   }

  PG.PageInc=0;
  PG.Enable=1;
  SubPageNum=PG.Blocks;
  if(SubPageNum==0)
       return -4;

/*
  if (GlobalPageSelect)
  {
      SubP=Source;
      if(MakeDialogBox(1,SubPageDialog))
         return -4;
  }
*/

  return SUBS_OK;
}

int SetPrinter(int type)
{
    extern int pic_dpi;
    unsigned char *p;
    int pg_w,pg_h,pt_w,pt_h;
    Pages *MidPage;      //By zjh 96.9.7
    UDATA xx,yy;
    int No_Init;
    unsigned char ch;
    int i;

    No_Init=0;
    if (type<0)
    {
        type=-type;
        No_Init=1;
    }

    fRemapRGB=UseHP1200=FALSE;
    fCheckStatus=TRUE;

    switch (type) {             // define in ..\ui\const.h
       case DEV_FAX:
          printer = &FAXprinter;
          break;
       case BJ_BW_360:
          printer = &BJ10eprinter;
          break;
       case BJ_COLOR:
          printer = &BJCprinter;
          break;
     /*----------------
       case BJ_COLOR360:
          printer = &BJC360printer;
          break;
       case BJ_COLOR720:
          printer = &BJC720printer;
          break;
      -----------------*/
       case HP_DJ_BW:
       case HP_LJ_A4_300:
          //printer = &NULprinter;
          printer = &HPA4_300printer;
          break;
       case HP2_LJ_A4_300:
       case LBP_LJ_A4_300:
          printer = &HP2A4_300printer;
          break;

       case P6100_LJ_A4_300:
          printer = &P6100_300printer;
          break;

       case P6500_LJ_A4_300:
          printer = &P6500_300printer;
          break;


       case HP_LJ_A4_600:
          printer = &HPA4_600printer;
          break;
       case HP_LJ_A3_300:
          printer = &HPA3_300printer;
          break;
       case HP_LJ_A3_600:
          printer = &HPA3_600printer;
          break;
       case HP_DJ_COLOR:
          printer = &HP1200_300printer;
          break;

       case OKI_BW_17:
          printer = &OKIprinter;
          goto lbl_set_spec_17_22;
       case OKI_BW_8:
          printer = &OKIprinter;
          goto lbl_set_spec_8_11;
          // break;

       case M1724_BW_17:
          printer = &M1724printer;
          goto lbl_set_spec_17_22;
          // break;
       case M1724_BW_8:
          printer = &M1724printer;
          goto lbl_set_spec_8_11;

       case LQ_COLOR_17:
          printer = &LQColorprinter;
          goto lbl_set_spec_17_22;
       case LQ_COLOR_8:
          printer = &LQColorprinter;
          goto lbl_set_spec_8_11;

       case STAR_AR3240_17:
          fCheckStatus=FALSE;
       case LQ_BW_17:
          printer = &LQBWprinter;
        lbl_set_spec_17_22:
          printer->xpixel=18*170;
          printer->ypixel=18*220;
          break;

       case STAR_AR3240_8:
          fCheckStatus=FALSE;
       case BJ_BW_180:
       case LQ_BW_8:
          printer = &LQBWprinter;
        lbl_set_spec_8_11:
          printer->xpixel=85*18;
          printer->ypixel=(110*18/24)*24;
          //printer->ypixel=110*18;  // 114*18
          break;

       case STAR_CR3240_17:
          fCheckStatus=FALSE;
          printer = &StarCRprinter;
          goto lbl_set_spec_17_22;
       case STAR_CR3240_8:
          fCheckStatus=FALSE;
          printer = &StarCRprinter;
          goto lbl_set_spec_8_11;


       case EPSON_DJ360_COLOR:     // stylus color
          printer = &EPSON360SCprinter;
          break;
       case EPSON_DJ720_COLOR_8:     // stylus color
          printer = &EPSON720SCprinter;
          printer->xpixel=85*72;
          printer->ypixel=108*72;
          break;
       case EPSON_DJ720_COLOR_17:     // stylus color
          printer = &EPSON720SCprinter;
          printer->xpixel=170*72;
          printer->ypixel=220*72;
          break;
       case EPSON_DJ_BW:        // stylus 800, 1000, ...
          printer = &ESCP2printer;
          break;

       default:
         // printer = &NULprinter;
         // break;
         return -1;
    }

    PrinterDPI = printer->resolution;
    PrinterFixedLeftMargin=printer->LeftMargin;

   #ifdef REGIST_VERSION
    //-- if exec here, it must have got logic file, & disk serial number
    ch=tsum-TypeSum;
   #endif

    p=(unsigned char *)&DitherTable[0];


    if(printer->DeviceType!=DEV_BW)
    {
        if(printer->DeviceType==DEV_CMYK)
        {
   #ifdef REGIST_VERSION
           //-- if exec here, it must have got logic file, & disk serial number
           p+=0xff&ch;
   #endif
           if(PrinterDPI>=600)
              memmove(p,tab720,DitherTableLen);
           else
              memmove(p,tab360,DitherTableLen);
   #ifdef REGIST_VERSION
           if(ch==0)
   #endif
           if( printer != &StarCRprinter
            && printer != &LQColorprinter )
               fRemapRGB=1;
        }
    }

   #ifdef REGIST_VERSION
    if(ch)
      for(i=0;i<8;i++)
      {
         headdot[i]-=ch*2;
         taildot[i]+=ch;
         hdot[i]++;
      }
   #endif



  MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
  xx=PageGetPageWidth(MidPage);
  yy=PageGetPageHeight(MidPage);

  if (!GlobalRorate90)
  {
        pg_w=xx;
        pg_h=yy;
  }
  else
  {
        pg_w=yy;
        pg_h=xx;
  }

  pg_w -= PageGetMarginRight(MidPage);
  pg_h -= PageGetMarginBottom(MidPage);
  HandleUnlock(ItemGetHandle(GlobalCurrentPage));

  #define GetPaperWidth() ((long)printer->xpixel*SCALEMETER/PrinterDPI)
  #define GetPaperHeight() ((long)printer->ypixel*SCALEMETER/PrinterDPI)
  pt_w=GetPaperWidth();
  pt_h=GetPaperHeight();

  if (!No_Init)
  {
    if (UsePrintCut())
        return (printer->printInit)(pt_w,pt_h);

    if (GlobalSubPage
    && (pt_w<pg_w*PXScale||pt_h<pg_h*PYScale))
    {
       i=MessageBox("多页拼版","当前页面超过打印纸幅面,使用分页输出吗?",3,1);
       if (i==1) return -1;     // cancel
       if (!i)  // ok
       {
         PB pb;

         pb.AutoCenter=1;
         pb.PagePrintX0=pb.PagePrintY0=pb.LeftMargin=pb.RightMargin
            =pb.TopMargin=pb.BottomMargin=0;
         pb.pg_w=pb.PagePrintX1=pg_w*PXScale;
         pb.pg_h=pb.PagePrintY1=pg_h*PYScale;
         pb.pt_w=pb.PrintPaperWidth=pt_w;
         pb.pt_h=pb.PrintPaperHeight=pt_h;
         pb.LayX=pb.LayY=300;

         i=PageToSubPage(&pb);
         if (i<0)
         {
            if (i==-4) return -1;
            if(MessageBox(GetTitleString(WARNINGINFORM),
                   "拼页错误,拼页将会被取消,继续吗?",2,1))
                return -1;
         }
         else
            return (printer->printInit)(pt_w,pt_h);
       }
    }

    if (GlobalRorate90)
       return (printer->printInit)(yy*PXScale,xx*PYScale);
    else
       return (printer->printInit)(xx*PXScale,yy*PYScale);
  }

  ReturnOK();
} /* SetPrinter */


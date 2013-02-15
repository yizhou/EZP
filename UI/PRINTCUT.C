/*-------------------------------------------------------------------
* Name: printcut.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

  #define REFLASHADD         MessageInsert(PrintCutWin[wPAGEADDWIN],ITEMSELECT,0l,0l)
  #define REFLASH            { if (InitOver) MessageInsert(PrintCutWin[wDISPWIN],MOUSELEFTDOWN,-1l,-1l); }
  //#define REFLASH
  #define BEGINOK            { if (!InitOver) return; }
  #define BEGINOK1           { if (!InitOver) return 0; }
  #define COLORP             1
  #define COLORPN            1
  #define CURRCOLOR          (12-15)
  #define EXISTCOLOR         11
  #define LEADCOLOR          (14-15)

  #define DF1               1
  #define DF2               2


  #define SMESS             10090
  #define ADDWIN            SMESS+2
  #define DELETEWIN         SMESS+3
  #define DELETETAILWIN     SMESS+4
  #define COPYWIN           SMESS+5

  #define DISPWIN           PrP0
  #define PAGEADDWIN        PrP1
  #define ROTATE0           PrP2
  #define ROTATE1           PrP3
  #define ROTATE2           PrP4
  #define ROTATE3           PrP5
  #define XPICWIN           PrP6
  #define YPICWIN           PrP7
  #define XSCALEWIN         PrP8
  #define YSCALEWIN         PrP9
  #define PAGEOFFSETWIN     PrP10
  #define PAGENUMWIN        PrP11

  #define wDISPWIN           0
  #define wPAGEADDWIN        1
  #define wROTATE0           2
  #define wROTATE1           3
  #define wROTATE2           4
  #define wROTATE3           5
  #define wXPICWIN           6
  #define wYPICWIN           7
  #define wXSCALEWIN         8
  #define wYSCALEWIN         9
  #define wPAGEOFFSETWIN     10
  #define wPAGENUMWIN        11

#define PAPERCOLOR 12
#define LONG long

static int MouseStat=0;
static int InitOver;


static float RowLead[100],ColLead[100];
static int RowLeadNum=0,ColLeadNum=0,CurrRow=0,CurrCol=0;

// int PageWI,PageHI;
static int ScreenDispWidth,ScreenDispHeight,ScreenX,ScreenY,mXw,mYw,WinX,WinY;
static float SCRscaleX,SCRscaleY,PageW,PageH;
static int CurrentBlock;

static HWND PrintCutWin[20];
static int SetPrintPara(void);
static int GetPrintPara(void);
static void DrawCurr(int color);
static void DrawRow(int color);
static void DrawCol(int color);
static void CopyBlock1(int t,int s);
static void CopyBlock(int t,int s);
static void InitPrintCut(void);

static int GetDir(void)
{
   int i;
   for (i=0;i<4;i++)
   {
     if (MessageGo(PrintCutWin[wROTATE0+i],GETSTATUS,0l,0l))
        return i;
   }
   return 0;
}

#ifdef NOT_USED
float GetColWidth(int n)
{
    float left,right;
    if (n<0) return -1.0;

    if (n!=0&&n==ColLeadNum&&PaperW-ColLead[n-1]<10.0) return -1.0;

    if (n<=ColLeadNum)
    {
        if (ColLeadNum==n) n--;
        right=ColLead[n];
        if (n==0) left=0;
        else left=ColLead[n-1];

        return right-left;
    }
    else return -1.0;
}

float GetColOffset(int n)
{
    float left;
    if (n<0) return 0.0;

    if (n<=ColLeadNum)
    {
        if (n==0) left=0;
        else left=ColLead[n-1];

        return left;
    }
    else return 0.0;
}

float GetRowHeight(int n)
{
    float Top,Bottom;
    if (n<0) return -1.0;

    if (n!=0&&n==RowLeadNum&&PaperH-RowLead[n-1]<10.0) return -1.0;

    if (n<=RowLeadNum)
    {
        if (RowLeadNum==n) n--;
        Bottom=RowLead[n];
        if (n==0) Top=0;
        else Top=RowLead[n-1];

        return Bottom-Top;
    }
    else return -1.0;
}
float GetRowOffset(int n)
{
    float Top;
    if (n<0) return 0.0;

    if (n<=RowLeadNum)
    {
        if (n==0) Top=0;
        else Top=RowLead[n-1];

        return Top;
    }
    else return 0.0;
}
#endif

static int GetUserFrame(int Pa,int *w,int *h)
{
     Pages *Mid;
     HPAGE pp;

     pp=PageNumberToHandle(Pa);
     Mid=HandleLock(ItemGetHandle(pp));

     *w=PageGetPageWidth(Mid);   //By zjh 96.9.7
     *h=PageGetPageHeight(Mid);  //By zjh 96.9.7

     HandleUnlock(ItemGetHandle(pp));
     return 0;
}

#define MouseXToWinX(x)  ((x)-ScreenX+WinX)
#define MouseYToWinY(y)  ((y)-ScreenY+WinY)
static int IsCurrentPage(int x,int y)
//  return :   0:  inter    1: left   2: right  3: top  4 :bottom
{
    int x0,x1,y0,y1,r,i;

    x=MouseXToWinX(x);
    y=MouseYToWinY(y);

    if (PG.Blocks<=0||CurrentBlock<0) goto next_comp;

    x0=PG.PageBlock[CurrentBlock].Xoffset/SCRscaleX;
    y0=PG.PageBlock[CurrentBlock].Yoffset/SCRscaleY;
    x1=PG.PageBlock[CurrentBlock].Xscale*PageW/SCRscaleX;
    y1=PG.PageBlock[CurrentBlock].Yscale*PageH/SCRscaleY;
    r=PG.PageBlock[CurrentBlock].Rotate;
    if (r&1)
    {
        r=x0+y1;
        y1=y0+x1;
        x1=r;
    }
    else
    {
        x1=x0+x1;
        y1=y0+y1;
    }
    if (x<x0-1||x>x1+1||y<y0-1||y>y1+1) goto next_comp;
    if (abs(x-x0)<DF1) return 1;
    if (abs(x-x1)<DF1) return 2;
    if (abs(y-y0)<DF1) return 3;
    if (abs(y-y1)<DF1) return 4;
    if (x>x0&&x<x1&&y>y0&&y<y1) return 0;

 next_comp:
    for (i=0;i<RowLeadNum;i++)
     {
        if (abs(y-(int)(RowLead[i]/SCRscaleY))<DF1)
        {
            CurrRow=i;
            return 21;
        }
     }

    for (i=0;i<ColLeadNum;i++)
     {
        if (abs(x-(int)(ColLead[i]/SCRscaleX))<DF1)
        {
            CurrCol=i;
            return 22;
        }
     }

    if (abs(x-0)<DF2) return 11;
    if (abs(x-mXw)<DF2) return 12;
    if (abs(y-0)<DF2) return 13;
    if (abs(y-mYw)<DF2) return 14;

    return -1;
}

static unsigned long UserWinProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int hlist,i;
  int stx,sty,mDX,mDY;
  char ss[120];

  switch (Message)
  {
     case DIALOGBOXOK:
            GetPrintPara();
            if (PG.Blocks>0)
                PG.Enable=1;
            else
                PG.Enable=0;
            return(DialogDefaultProcedure(Window, Message, Param1, Param2));
     case DIALOGBOXCANCEL:
            PG.Enable=0;
            return(DialogDefaultProcedure(Window, Message, Param1, Param2));
     case REDRAWMESSAGE:
            i=DialogDefaultProcedure(Window, Message, Param1, Param2);
            WaitMessageEmpty();
            InitOver=1;
            InitPrintCut();
            CurrentBlock=0;
            SetPrintPara();
            REFLASH;
            MessageInsert(PrintCutWin[wPAGEADDWIN],WINDOWINIT,0l,0l);
            ListSetCurrent(WindowList(PrintCutWin[wPAGEADDWIN]),CurrentBlock);

            REFLASHADD;
            return i;
            break;
     case WINDOWINIT:
            InitOver=0;

            i=DialogDefaultProcedure(Window, Message, Param1, Param2);
            //WaitMessageEmpty();
            return i;
            break;
     case ADDWIN:
            PG.Blocks=2;
            CurrentBlock=0;
            InitBlock(0);
            InitBlock(1);
            if (GetDir()&1)
               PG.PageBlock[1].Yoffset +=PageW;
            else
               PG.PageBlock[1].Xoffset +=PageW;

            SetPrintPara();

            hlist = WindowList(PrintCutWin[wPAGEADDWIN]);
            MessageGo(PrintCutWin[wPAGEADDWIN],WINDOWINIT,0,0);
            MessageGo(PrintCutWin[wPAGEADDWIN],WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
            if (CurrentBlock>=7)
               ListSetTop(hlist,CurrentBlock-7);
            else
               ListSetTop(hlist,0);
            ListSetCurrent(hlist,CurrentBlock);
            MessageGo(PrintCutWin[wPAGEADDWIN],REDRAWMESSAGE,0L,
                MAKELONG(WindowGetWidth(PrintCutWin[wPAGEADDWIN]),
                WindowGetHeight(PrintCutWin[wPAGEADDWIN])) );
            REFLASH;
            break;
     case DELETEWIN:
            PG.Blocks=4;
            CurrentBlock=0;
            InitBlock(0);
            InitBlock(1);
            InitBlock(2);
            InitBlock(3);

            if (GetDir()&1)
            {
               PG.PageBlock[2].Yoffset +=PageW;
               PG.PageBlock[0].Xoffset +=PageH;

               PG.PageBlock[1].Yoffset +=PageW;
               PG.PageBlock[1].Xoffset +=PageH;
            }
            else
            {
               PG.PageBlock[1].Xoffset +=PageW;
               PG.PageBlock[3].Yoffset +=PageH;

               PG.PageBlock[2].Xoffset +=PageW;
               PG.PageBlock[2].Yoffset +=PageH;
            }

            SetPrintPara();

            hlist = WindowList(PrintCutWin[wPAGEADDWIN]);
            MessageGo(PrintCutWin[wPAGEADDWIN],WINDOWINIT,0,0);
            MessageGo(PrintCutWin[wPAGEADDWIN],WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
            if (CurrentBlock>=7)
             ListSetTop(hlist,CurrentBlock-7);
            else
            ListSetTop(hlist,0);
            ListSetCurrent(hlist,CurrentBlock);
            MessageGo(PrintCutWin[wPAGEADDWIN],REDRAWMESSAGE,0L,
                MAKELONG(WindowGetWidth(PrintCutWin[wPAGEADDWIN]),
                WindowGetHeight(PrintCutWin[wPAGEADDWIN])) );
            REFLASH;
            break;
     case DELETETAILWIN:
            PG.Blocks=10;
            CurrentBlock=0;
            for (i=0;i<10;i++) InitBlock(i);

            GetProfileString( ProfileName,DefaultSection, "PrintCut10SX",ss,
                        "5.0");
            stx=atof(ss);

            GetProfileString( ProfileName,DefaultSection, "PrintCut10SY",ss,
                        "5.0");
            sty=atof(ss);

            GetProfileString( ProfileName,DefaultSection, "PrintCut10DX",ss,
                        "5.0");
            mDX=atof(ss);

            GetProfileString( ProfileName,DefaultSection, "PrintCut10DY",ss,
                        "5.0");
            mDY=atof(ss);

            PG.PageBlock[0].Xoffset =stx;
            PG.PageBlock[1].Xoffset =stx+PageW+mDX;
            PG.PageBlock[0].Yoffset =sty;
            PG.PageBlock[1].Yoffset =sty;

            for (i=2;i<10;i=i+2)
            {
               PG.PageBlock[i].Yoffset +=PageH+mDY+PG.PageBlock[i-2].Yoffset;
               PG.PageBlock[i+1].Yoffset +=PageH+mDY+PG.PageBlock[i-1].Yoffset;

               PG.PageBlock[i].Xoffset =PG.PageBlock[i-2].Xoffset;
               PG.PageBlock[i+1].Xoffset =PG.PageBlock[i-1].Xoffset;
            }

            SetPrintPara();

            hlist = WindowList(PrintCutWin[wPAGEADDWIN]);
            MessageGo(PrintCutWin[wPAGEADDWIN],WINDOWINIT,0,0);
            MessageGo(PrintCutWin[wPAGEADDWIN],WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
            if (CurrentBlock>=7)
             ListSetTop(hlist,CurrentBlock-7);
            else
            ListSetTop(hlist,0);
            ListSetCurrent(hlist,CurrentBlock);
            MessageGo(PrintCutWin[wPAGEADDWIN],REDRAWMESSAGE,0L,
                MAKELONG(WindowGetWidth(PrintCutWin[wPAGEADDWIN]),
                WindowGetHeight(PrintCutWin[wPAGEADDWIN])) );
            REFLASH;
            break;
     case COPYWIN:
            GetPrintPara();
            PG.Blocks=0;
            CurrentBlock=0;
            hlist = WindowList(PrintCutWin[wPAGEADDWIN]);
            MessageGo(PrintCutWin[wPAGEADDWIN],WINDOWINIT,0,0);
            MessageGo(PrintCutWin[wPAGEADDWIN],WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
            MessageGo(PrintCutWin[wPAGEADDWIN],REDRAWMESSAGE,0L,
                MAKELONG(WindowGetWidth(PrintCutWin[wPAGEADDWIN]),
                WindowGetHeight(PrintCutWin[wPAGEADDWIN])) );
            REFLASH;
            break;
     default:
            return(DialogDefaultProcedure(Window, Message, Param1, Param2));
  }
  /* return(DialogDefaultProcedure(Window, Message, Param1, Param2)); */
  return(TRUE);
}

static unsigned long PrP0(HWND Window,HMSG Message,long Param1,long Param2)
{
    int i,saveI,j;
    int x0,y0,x1,y1;
    static int LastX,LastY;

    /*
    x0=MouseXToWinX(Param1>>16);
    y0=MouseYToWinY(Param1&0xffff);
    x1=MouseXToWinX(Param2>>16);
    y1=MouseYToWinY(Param2&0xffff);
    */
    x0=(Param1>>16);
    y0=(Param1&0xffff);
    x1=(Param2>>16);
    y1=(Param2&0xffff);
    switch (Message)
    {
        #ifdef UNUSE
        case MOUSELEFTDOUBLE:
                i=IsCurrentPage(x0,y0);
                if (i>0)
                  switch(i)
                  {
                    case 13:
                    case 14:
                            x0=MouseXToWinX(x0);
                            ColLead[ColLeadNum++]=SCRscaleX*x0;
                            for (i=0;i<ColLeadNum-1;i++)
                              if (fabs(ColLead[i]-ColLead[ColLeadNum-1])<10.0f)
                                {
                                    ColLeadNum--;
                                    break;
                                }
                            REFLASH;
                            break;
                    case 11:
                    case 12:
                            y0=MouseYToWinY(y0);
                            RowLead[RowLeadNum++]=SCRscaleY*y0;
                            for (i=0;i<RowLeadNum-1;i++)
                              if (fabs(RowLead[i]-RowLead[RowLeadNum-1])<10.0f)
                                {
                                    RowLeadNum--;
                                    break;
                                }
                            REFLASH;
                            break;
                    case 21:                                  //Edit  Col
                            break;
                    case 22:                                  //Edit  Row
                            break;
                  }

                break;

        case MOUSERIGHTDOUBLE:
                i=IsCurrentPage(x0,y0);
                if (i>0)
                  switch(i)
                  {
                    case 21:                                  //Edit  Row
                            for (i=CurrRow;i<RowLeadNum-1;i++) RowLead[i]=RowLead[i+1];
                            RowLeadNum--;
                            REFLASH;
                            break;
                    case 22:                                  //Edit  Col
                            for (i=CurrCol;i<ColLeadNum-1;i++) ColLead[i]=ColLead[i+1];
                            ColLeadNum--;
                            REFLASH;
                            break;
                  }
                break;
        case MOUSEMOVE:
                i=IsCurrentPage(x0,y0);
                if (i>=0)
                 switch (i)
                    {
                        case 11:
                        case 12:
                        case 13:
                        case 14:
                                MouseSetGraph(FINGERMOUSE2);
                                MouseStat=0;
                                break;
                        case 0:
                                MouseSetGraph(MOVINGMOUSE);
                                MouseStat=5;
                                break;
                        case 1:
                        case 2:
                        case 22:
                                MouseSetGraph(HRESIZEMOUSE);
                                MouseStat=i;
                                break;
                        case 3:
                        case 4:
                        case 21:
                                MouseSetGraph(VRESIZEMOUSE);
                                MouseStat=i;
                                break;
                        default:
                                MouseSetGraph(ARRAWMOUSE);
                                MouseStat=0;
                    }
                else
                {
                 MouseSetGraph(ARRAWMOUSE);
                 MouseStat=0;
                }

                x0=MouseXToWinX(x0);
                y0=MouseYToWinY(y0);
                if (x0>=0&&y0>=0&&x0<=mXw&&y0<=mYw)
                {
                    char ss[30];
                    sprintf(ss,"(%6.1f,%6.1f)",SCRscaleX*x0,SCRscaleY*y0);
                    Window=WindowGetFather(Window);
                    x0=WindowGetLeft(Window);
                    y0=WindowGetTop(Window);
                    DisplayString(ss,x0+360,y0+5,15,1);
                }
                break;

        case MOUSELEFTDROP:
                switch (MouseStat)
                 {
                    case 22:
                            dx=x1-LastX;
                            DrawCol(LEADCOLOR);
                            Midf=(float)dx*SCRscaleX;
                            ColLead[CurrCol] += Midf;
                            DrawCol(LEADCOLOR);
                            LastX=x1;
                            LastY=y1;
                            break;
                    case 21:
                            dy=y1-LastY;
                            DrawRow(LEADCOLOR);
                            Midf=(float)dy*SCRscaleY;
                            RowLead[CurrRow] += Midf;
                            DrawRow(LEADCOLOR);
                            LastX=x1;
                            LastY=y1;
                            break;
                    case 1:
                            dx=x1-LastX;
                            DrawCurr(CURRCOLOR);
                            Midf=(float)dx*SCRscaleX;
                            PG.PageBlock[CurrentBlock].Xoffset += Midf;

                            if (PG.PageBlock[CurrentBlock].Rotate&1)
                               PG.PageBlock[CurrentBlock].Yscale -= Midf/PageH;
                            else
                               PG.PageBlock[CurrentBlock].Xscale -= Midf/PageW;

                            DrawCurr(CURRCOLOR);
                            SetPrintPara1();
                            SetPrintPara2();
                            LastX=x1;
                            LastY=y1;
                            break;
                    case 2:
                            dx=x1-LastX;
                            DrawCurr(CURRCOLOR);
                            Midf=(float)dx*SCRscaleX;
                            //PG.PageBlock[CurrentBlock].Xoffset += Midf;

                            if (PG.PageBlock[CurrentBlock].Rotate&1)
                               PG.PageBlock[CurrentBlock].Yscale += Midf/PageH;
                            else
                               PG.PageBlock[CurrentBlock].Xscale += Midf/PageW;

                            DrawCurr(CURRCOLOR);
                            //SetPrintPara1();
                            SetPrintPara2();
                            LastX=x1;
                            LastY=y1;
                            break;
                    case 3:
                            dy=y1-LastY;
                            DrawCurr(CURRCOLOR);
                            Midf=(float)dy*SCRscaleY;
                            PG.PageBlock[CurrentBlock].Yoffset += Midf;

                            if (PG.PageBlock[CurrentBlock].Rotate&1)
                               PG.PageBlock[CurrentBlock].Xscale -= Midf/PageW;
                            else
                               PG.PageBlock[CurrentBlock].Yscale -= Midf/PageH;

                            DrawCurr(CURRCOLOR);
                            SetPrintPara1();
                            SetPrintPara2();
                            LastX=x1;
                            LastY=y1;
                            break;
                    case 4:
                            dy=y1-LastY;
                            DrawCurr(CURRCOLOR);
                            Midf=(float)dy*SCRscaleY;
                            //PG.PageBlock[CurrentBlock].Yoffset += Midf;

                            if (PG.PageBlock[CurrentBlock].Rotate&1)
                               PG.PageBlock[CurrentBlock].Xscale += Midf/PageW;
                            else
                               PG.PageBlock[CurrentBlock].Yscale += Midf/PageH;

                            DrawCurr(CURRCOLOR);
                            //SetPrintPara1();
                            SetPrintPara2();
                            LastX=x1;
                            LastY=y1;
                            break;
                    case 5:
                            dx=x1-LastX;
                            dy=y1-LastY;
                            DrawCurr(CURRCOLOR);
                            PG.PageBlock[CurrentBlock].Xoffset += (float)dx*SCRscaleX;
                            PG.PageBlock[CurrentBlock].Yoffset += (float)dy*SCRscaleY;
                            DrawCurr(CURRCOLOR);
                            SetPrintPara1();
                            LastX=x1;
                            LastY=y1;
                            break;
                 }

                x0=MouseXToWinX(x1);
                y0=MouseYToWinY(y1);
                if (x0>=0&&y0>=0&&x0<=mXw&&y0<=mYw)
                {
                    char ss[30];
                    sprintf(ss,"(%6.1f,%6.1f)",SCRscaleX*x0,SCRscaleY*y0);
                    Window=WindowGetFather(Window);
                    x0=WindowGetLeft(Window);
                    y0=WindowGetTop(Window);
                    DisplayString(ss,x0+360,y0+5,15,1);
                }
                break;
        case MOUSELEFTUP:
                /*
                if (MouseStat)
                {
                  MouseStat=0;
                  REFLASH;
                }
                */
                break;
        #endif     //UNUSE
        case WINDOWINIT:
                PrintCutWin[0]=Window;
                //InitPrintCut();
                break;
        case MOUSELEFTDOWN:
                GetPrintPara();
                SetPrintPara();
                DrawCurr(0);

                if (PG.Blocks>0)
                {
                  saveI=CurrentBlock;
                  for (i=0;i<PG.Blocks;i++)
                  {
                    if (i==saveI) continue;
                    CurrentBlock=i;
                    DrawCurr(EXISTCOLOR);
                  }

                  CurrentBlock=saveI;
                  DrawCurr(CURRCOLOR);
                }

                if (Param1==-1l&&Param2==-1l) break;     //REFLASH
                LastX=x0;
                LastY=y0;
                i=IsCurrentPage(x0,y0);
                if (i<0||i>5)
                {
                    GetPrintPara();
                    saveI=CurrentBlock;
                    for (i=0;i<PG.Blocks;i++)
                    {
                        CurrentBlock=i;
                        j=IsCurrentPage(x0,y0);
                        if (j>=0&&j<=5)
                        {
                            MouseStat=5;
                            MouseSetGraph(MOVINGMOUSE);
                            SetPrintPara();
                            ListSetCurrent(WindowList(PrintCutWin[wPAGEADDWIN]),i);
                            MessageGo(PrintCutWin[wPAGEADDWIN],ITEMSELECT,0l,0l);
                            //REFLASH;
                            break;
                        }
                        else
                            CurrentBlock=saveI;
                    }
                }
                break;

        case GETFOCUS:              // ByHance, 97,5.13
             if(fGetFocusByKey)
                 return FALSE;
             goto lbl_default;
        case KEYDOWN:              // ByHance, 97,5.13
             i=MAKELO(Param1);
             if(i==TAB || i==SHIFT_TAB)
                goto lbl_default;
             return FALSE;
        case KEYSTRING:
        // case MOUSEMOVE:
             return FALSE;
       default:
        lbl_default:
             return (ListBoxDefaultProcedure(Window,Message,Param1,Param2));
    }
    return TRUE;
}

static unsigned long PrP1(HWND Window,HMSG Message,long Param1,long Param2)
{
    int i;
    char str[10];
    int hlist;

    if (Message==WINDOWINIT)
    {
        PrintCutWin[1]=Window;
        if (InitOver)
        {
            MessageGo(Window,LISTSETITEMHEIGHT,16,0);
            MessageGo(Window,LISTSETITEMLENGTH,7,0);
            MessageGo(Window,LISTDELETEALL,0L,0L);
            for (i=PG.Blocks-1;i>=0;i--)
             {
              sprintf(str,"BBNN%d",i+1);
              MessageGo(Window, LISTINSERTITEM, FP2LONG(str), 0L);
             }

            if (PG.Blocks>0)
                SetPrintPara();
            return TRUE;
        }
    }
    else
    if (Message==LISTBOXCONFIRM||Message==ITEMSELECT)
    {
        hlist = WindowList(Window);
        GetPrintPara();
        CurrentBlock=ListGetCurrent(hlist);
        SetPrintPara();

        i=ListGetTop(hlist);
        MessageGo(Window,WINDOWINIT,0l,0l);
        MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
        ListSetTop(hlist,i);
        ListSetCurrent(hlist,CurrentBlock);
        MessageGo(Window,REDRAWMESSAGE,0L,
                MAKELONG(WindowGetWidth(Window),
                WindowGetHeight(Window)) );
        REFLASH;
        return TRUE;
    }

    return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
}

static unsigned long PrP2(HWND Window,HMSG Message,long Param1,long Param2)
{
    if (Message==WINDOWINIT) PrintCutWin[2]=Window;
    else
    if (Message==SELECTSELECTED)
     {
        GetPrintPara();
        SetPrintPara();
        REFLASH;
        return TRUE;
     }

    return(RadioDefaultProcedure(Window,Message,Param1,Param2));
}

static unsigned long PrP3(HWND Window,HMSG Message,long Param1,long Param2)
{
    if (Message==WINDOWINIT) PrintCutWin[3]=Window;
    else
    if (Message==SELECTSELECTED)
     {
        GetPrintPara();
        SetPrintPara();
        REFLASH;
        return TRUE;
     }
    return(RadioDefaultProcedure(Window,Message,Param1,Param2));
}

static unsigned long PrP4(HWND Window,HMSG Message,long Param1,long Param2)
{
    if (Message==WINDOWINIT) PrintCutWin[4]=Window;
    else
    if (Message==SELECTSELECTED)
     {
        GetPrintPara();
        SetPrintPara();
        REFLASH;
        return TRUE;
     }
    return(RadioDefaultProcedure(Window,Message,Param1,Param2));
}

static unsigned long PrP5(HWND Window,HMSG Message,long Param1,long Param2)
{
    if (Message==WINDOWINIT) PrintCutWin[5]=Window;
    else
    if (Message==SELECTSELECTED)
     {
        GetPrintPara();
        SetPrintPara();
        REFLASH;
        return TRUE;
     }
    return(RadioDefaultProcedure(Window,Message,Param1,Param2));
}

static unsigned long PrP6(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
          PrintCutWin[6]=Window;
          MessageGo(Window,SETLINEBUFFER,FP2LONG("1.00"),0l);
          return TRUE;
    case GETFOCUS:              // ByHance, 97,5.13
    case MOUSEMOVE:
          return FALSE;
  }

  return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
}

static unsigned long PrP7(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
          PrintCutWin[7]=Window;
          MessageGo(Window,SETLINEBUFFER,FP2LONG("1.00"),0l);
          return TRUE;
    case GETFOCUS:              // ByHance, 97,5.13
    case MOUSEMOVE:
          return FALSE;
  }

  return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
}

static unsigned long PrP8(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
          PrintCutWin[8]=Window;
          MessageGo(Window,SETLINEBUFFER,FP2LONG("1.00"),0l);
          return TRUE;
    case GETFOCUS:              // ByHance, 97,5.13
    case MOUSEMOVE:
          return FALSE;
  }

  return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
}

static unsigned long PrP9(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
          PrintCutWin[9]=Window;
          MessageGo(Window,SETLINEBUFFER,FP2LONG("1.00"),0l);
          return TRUE;
    case GETFOCUS:              // ByHance, 97,5.13
    case MOUSEMOVE:
          return FALSE;
  }

  return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
}

static unsigned long PrP10(HWND Window,HMSG Message,long Param1,long Param2)
{
    if (Message==WINDOWINIT) PrintCutWin[10]=Window;
    return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
}

static unsigned long PrP11(HWND Window,HMSG Message,long Param1,long Param2)
{
    if (Message==WINDOWINIT) PrintCutWin[11]=Window;
    return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
}

static void GetXY(int n,int *x,int *y)
{
  HWND Window,Mid;
  if (n>4) n=4;
  if (n<1) n=1;
  Window=PrintCutWin[wPAGENUMWIN]+n;
  Mid=WindowGetFather(Window);
  *x=WindowGetRight(Window)+1+WindowGetLeft(Mid);
  *y=WindowGetTop(Window)+WindowGetTop(Mid);
}

static void DispXY(int x,int y,char *s,int color)
{
  DisplayString(s,x,y,color,EGA_LIGHTGRAY);
  /*
  int SaveColor;
  struct viewporttype SaveViewPort;
  MouseHidden();
  getviewsettings(&SaveViewPort);
  SaveColor=getcolor();

  setwritemode(COPY_PUT);
  setcolor(EGA_DARKGRAY);       //gray
  setviewport(0,0,getmaxx(),getmaxy(),1);

  DisplayString(s,x,y,color,EGA_LIGHTGRAY);

  setwritemode(COPY_PUT);
  setcolor(SaveColor);
  setviewport(SaveViewPort.left,SaveViewPort.top,SaveViewPort.right,
              SaveViewPort.bottom,SaveViewPort.clip);
  MouseShow();
  */
}

Dialogs PrintCutDialog[]=
{
  { GLOBALITEM, 15 , 0, 0, 500, 350, 0, UserWinProcedure,"打印拼版设置" },
  #define X     5
  #define Y     56

  { USERBUTTONITEM, 0, 9, 200, 81,228, ADDWIN, NULL, " 两  版 " },
  { USERBUTTONITEM, 0, 9, 238, 81,266, DELETEWIN, NULL, " 四  版 " },
  { USERBUTTONITEM, 0, 9, 276, 81,304, DELETETAILWIN, NULL, " 十  版 " },
  { USERBUTTONITEM, 0, 9, 314, 81,342, COPYWIN, NULL, " 删  除 " },

  { LISTBOXITEM, 0, 230, 50, 480, 310, 0, DISPWIN,""},

  { SINGLELINEEDITORITEM, 0, 306, 320, 340,338, 0, PAGENUMWIN, "" },
  { STATICTEXTITEM, 0, 235,    320,305,    340, 0, NULL, "需要页数" },
  { STATICTEXTITEM, 0, 235+130,320,320+130,340, 0, NULL, "定义版数" },
  { STATICTEXTITEM, 0, 235,    29, 320,    47,  0, NULL, "打印纸宽:" },
  { STATICTEXTITEM, 0, 235+130,29, 320+130,47,  0, NULL, "打印纸高:" },

  { FRAMEITEM, 4, 98-X, 249-Y, 230-X, 320-Y, 0, NULL, "旋转" },
      #define FX        98
      #define FY        80
      #define tFY       244
     { SINGLESELECT, 0, 108-FX, 264-tFY, 165-FX, 282-tFY,0, ROTATE0, "  0" },
     { SINGLESELECT, 0, 170-FX, 264-tFY, 215-FX, 282-tFY,1, ROTATE1, " 90" },
     { SINGLESELECT, 0, 108-FX, 290-tFY, 165-FX, 308-tFY,2, ROTATE2, "180" },
     { SINGLESELECT, 0, 170-FX, 290-tFY, 215-FX, 308-tFY,3, ROTATE3, "270" },

  { FRAMEITEM, 10, 98-X, 86-Y, 230-X, 244-Y, 0, NULL, "当前版框" },
     { SINGLELINEEDITORITEM, 0, 165-FX, 108-FY, 215-FX, 126-FY,0, XPICWIN, "" },
     { SINGLELINEEDITORITEM, 0, 165-FX, 134-FY, 215-FX, 152-FY,0, YPICWIN, "" },
     { SINGLELINEEDITORITEM, 0, 165-FX, 160-FY, 215-FX, 178-FY,0, XSCALEWIN, "" },
     { SINGLELINEEDITORITEM, 0, 165-FX, 186-FY, 215-FX, 204-FY,0, YSCALEWIN, "" },
     { SINGLELINEEDITORITEM, 0, 165-FX, 212-FY, 215-FX, 230-FY,0, PAGEOFFSETWIN,"" },

     { STATICTEXTITEM, 0, 108-FX, 108-FY, 158-FX, 128-FY, 0, NULL, "X 坐标" },
     { STATICTEXTITEM, 0, 108-FX, 134-FY, 158-FX, 154-FY, 0, NULL, "Y 坐标" },
     { STATICTEXTITEM, 0, 108-FX, 160-FY, 158-FX, 180-FY, 0, NULL, "X 比例" },
     { STATICTEXTITEM, 0, 108-FX, 186-FY, 158-FX, 206-FY, 0, NULL, "Y 比例" },
     { STATICTEXTITEM, 0, 108-FX, 212-FY, 158-FX, 232-FY, 0, NULL, "页偏移" },

  { OKBUTTON,     0, 98-X+20, 327-Y, 230-X-20, 357-Y, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 98-X+20, 364-Y, 230-X-20, 394-Y, 0, NULL, "放弃" },

     //{ MULTISELECT, 0,          108-FX, 238-FY, 158-FX, 256-FY, 0, NULL, "镜像" },
     //{ USERBUTTONITEM, 0,       165-FX, 238-FY, 218-FX, 259-FY, 0, NULL, "接受" },

  { FRAMEITEM, 1, 5, 30, 85,190, 0, NULL, "已有版框" },
     { LISTBOXITEM, 0, 4, 24, 76,153, 0,PAGEADDWIN,"" },

     #undef FX
     #undef FY

#undef X
#undef Y
};


static void InitBlock(int n)
{
    PG.PageBlock[n].Reveser=0;
    PG.PageBlock[n].Rotate=GetDir();
    PG.PageBlock[n].Xoffset=0;
    PG.PageBlock[n].Yoffset=0;
    PG.PageBlock[n].Xscale=1.0;
    PG.PageBlock[n].Yscale=1.0;
    PG.PageBlock[n].PageOffset=max(0,n);
    PG.PageInc=PG.Blocks;
}

#ifdef NOT_USED
static void CopyBlock(int t,int s)
{
   memcpy(&PG.PageBlock[t].Reveser,
          &PG.PageBlock[s].Reveser,
          sizeof(PrintBlock));
   PG.PageBlock[t].Xoffset += 10;
   PG.PageBlock[t].Yoffset += 10;
   PG.PageBlock[t].PageOffset = t;
}

static void CopyBlock1(int t,int s)
{
   memcpy(&PG.PageBlock[t].Reveser,
          &PG.PageBlock[s].Reveser,
          sizeof(PrintBlock));
}

static void DrawLine(int x0,int y0,int x1,int y1,int color)
{
  int SaveColor;
  struct viewporttype SaveViewPort;

  MouseHidden();
  getviewsettings(&SaveViewPort);
  SaveColor=getcolor();

  setviewport(ScreenX,ScreenY,ScreenX+mXw-1,ScreenY+mYw-1,1);

  if (color<0) { setwritemode(XOR_PUT); color=-color; }
    else setwritemode(COPY_PUT);

  setcolor(color);
  line(x0,y0,x1,y1);

  setwritemode(COPY_PUT);
  setcolor(SaveColor);
  setviewport(SaveViewPort.left,SaveViewPort.top,SaveViewPort.right,
              SaveViewPort.bottom,SaveViewPort.clip);
  MouseShow();
}

static void DrawCol(int color)
{
    int mx;
    mx=(int)(ColLead[CurrCol]/SCRscaleX);
    DrawLine(mx,0,mx,mYw,color);
}

static void DrawRow(int color)
{
    int my;
    my=(int)(RowLead[CurrRow]/SCRscaleY);
    DrawLine(0,my,mXw,my,color);
}
#endif

static void SetPI(int v,HWND Window)
{
    char str[30];
    sprintf(str,"%d",v);
    MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
    MessageGo(Window,REDRAWMESSAGE,0L,
                MAKELONG(WindowGetWidth(Window),
                WindowGetHeight(Window)) );
}

static void SetP(float v,HWND Window)
{
    char str[30];
    sprintf(str,"%.2f",v);
    MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
    MessageGo(Window,REDRAWMESSAGE,0L,
                MAKELONG(WindowGetWidth(Window),
                WindowGetHeight(Window)) );
}

static void myrectangle(int x0,int y0,int x1,int y1)
{
    line(x0,y0,x0,y1-1);
    line(x0,y1,x1-1,y1);
    line(x1,y1,x1,y0+1);
    line(x1,y0,x0+1,y0);
}


static void DrawUserPage(float x,float y,float sx,float sy,int r,int color)
{
  #define PN 10
  static char xy[][2]={{0,6},{0,3},{0,1},{1,0},{2,0},{3,1},{3,2},{2,3},{0,3},{3,6}};
  char xy2[10][2];
  int SaveColor,i;
  struct viewporttype SaveViewPort;
  int mx,my,xw,yw;
  float xs,ys;

  MouseHidden();
  getviewsettings(&SaveViewPort);
  SaveColor=getcolor();

  setviewport(ScreenX,ScreenY,ScreenX+mXw-1,ScreenY+mYw-1,1);

  if (!color)
  {
    setwritemode(COPY_PUT);
    setcolor(EGA_WHITE);
    bar(0,0,mXw-1,mYw-1);
    setwritemode(XOR_PUT);
    setcolor(abs(LEADCOLOR));
    for (i=0;i<ColLeadNum;i++)
     {
        mx=(int)(ColLead[i]/SCRscaleX);
        line(mx,0,mx,mYw);
     }

    for (i=0;i<RowLeadNum;i++)
     {
        my=(int)(RowLead[i]/SCRscaleY);
        line(0,my,mXw,my);
     }
  }
  else
  {
    // r=r%4+(GlobalReverse&1)*4;
    r=(r%4)+GlobalReverse*4;
    for (i=0;i<PN;i++)
    {
      switch(r)
      {
          case 0:
                  xy2[i][0]=xy[i][0];
                  xy2[i][1]=xy[i][1];
                  break;
          case 1:
                  xy2[i][0]=6-xy[i][1];
                  xy2[i][1]=xy[i][0];
                  break;
          case 2:
                  xy2[i][0]=3-xy[i][0];
                  xy2[i][1]=6-xy[i][1];
                  break;
          case 3:
                  xy2[i][0]=xy[i][1];
                  xy2[i][1]=3-xy[i][0];
                  break;
          case 4:
                  xy2[i][0]=3-xy[i][0];
                  xy2[i][1]=xy[i][1];
                  break;
          case 5:
                  xy2[i][0]=xy[i][1];
                  xy2[i][1]=xy[i][0];
                  break;
          case 6:
                  xy2[i][0]=xy[i][0];
                  xy2[i][1]=6-xy[i][1];
                  break;
          case 7:
                  xy2[i][0]=6-xy[i][1];
                  xy2[i][1]=3-xy[i][0];
                  break;
      }
    }   /*-- for i --*/

    if (color<0) { setwritemode(XOR_PUT); color=-color; }
    else setwritemode(COPY_PUT);

    setcolor(color);

    mx=x/SCRscaleX;
    my=y/SCRscaleY;
    xw=PageW*sx/SCRscaleX;
    yw=PageH*sy/SCRscaleY;

    if (r&1)
    {
        myrectangle(mx,my,mx+yw,my+xw);
        mx=mx+yw/4;
        my=my+xw/4;
        ys=(float)xw/6;
        xs=(float)yw/12;
    }
    else
    {
        myrectangle(mx,my,mx+xw,my+yw);
        mx=mx+xw/4;
        my=my+yw/4;
        xs=(float)xw/6;
        ys=(float)yw/12;
    }

    for (i=0;i<PN-1;i++)
    {
       line(mx+xs*xy2[i][0],my+ys*xy2[i][1],mx+xs*xy2[(i+1)%PN][0],my+ys*xy2[(i+1)%PN][1]);
       if (i<PN-2)
       line(mx+xs*xy2[i+1][0],my+ys*xy2[i+1][1],mx+xs*xy2[(i+1)%PN][0],my+ys*xy2[(i+1)%PN][1]);
    }
    line(mx+xs*xy2[8][0],my+ys*xy2[8][1],mx+xs*xy2[8][0],my+ys*xy2[8][1]);
  }

  setwritemode(COPY_PUT);
  setcolor(SaveColor);
  setviewport(SaveViewPort.left,SaveViewPort.top,SaveViewPort.right,
              SaveViewPort.bottom,SaveViewPort.clip);
  MouseShow();
}

static void DrawCurr(int color)
{
    BEGINOK;
    if (PG.Blocks>0||color==0)
    DrawUserPage(PG.PageBlock[CurrentBlock].Xoffset,
                 PG.PageBlock[CurrentBlock].Yoffset,
                 PG.PageBlock[CurrentBlock].Xscale,
                 PG.PageBlock[CurrentBlock].Yscale,
                 PG.PageBlock[CurrentBlock].Rotate,
                 color);
}

static void InitPrintCut(void)
{
   HWND Window,Mid;
   int xw,yw,i,j;
   char p[40];
   int SaveColor;
   struct viewporttype SaveViewPort;


   BEGINOK;
   //WaitMessageEmpty();

   MouseHidden();
   getviewsettings(&SaveViewPort);
   SaveColor=getcolor();

   SetPrinter(-CurrentPrinter);
   Window=PrintCutWin[wDISPWIN];
   Mid=WindowGetFather(Window);
   ScreenDispWidth=WindowGetWidth(Window)-10;
   ScreenDispHeight=WindowGetHeight(Window)-10;
   WinX=ScreenX=WindowGetLeft(Window)+WindowGetLeft(Mid);
   WinY=ScreenY=WindowGetTop(Window)+WindowGetTop(Mid);

   setviewport(WinX,WinY,WinX+ScreenDispWidth+9,WinY+ScreenDispHeight+9,1);
   setwritemode(COPY_PUT);
   setcolor(EGA_WHITE);
   bar(0,0,ScreenDispWidth+9,ScreenDispHeight+9);

   PaperW=(float)(printer->xpixel)*25.4/PrinterDPI;
   PaperH=(float)(printer->ypixel)*25.4/PrinterDPI;

   SCRscaleX=PaperW/ScreenDispWidth;
   SCRscaleY=PaperH/ScreenDispHeight;
   if (SCRscaleX>SCRscaleY)
        SCRscaleY=SCRscaleX;
   else
        SCRscaleX=SCRscaleY;

   xw=mXw=PaperW/SCRscaleX;
   yw=mYw=PaperH/SCRscaleY;

   ScreenX=ScreenX+5+(ScreenDispWidth-mXw)/2;
   ScreenY=ScreenY+5+(ScreenDispHeight-mYw)/2;

   setcolor(EGA_DARKGRAY);       //gray
   setviewport(0,0,getmaxx(),getmaxy(),1);
   rectangle(ScreenX-1,ScreenY-1,ScreenX+xw,ScreenY+yw);

   GetUserFrame(0,&xw,&yw);
   PageW=xw*25.4/SCALEMETER;
   PageH=yw*25.4/SCALEMETER;

   sprintf(p,"%4d",(int)(PaperW+.5));
   GetXY(3,&i,&j);
   DispXY(i-10,j,p,COLORP);

   sprintf(p,"%4d",(int)(PaperH+.5));
   GetXY(4,&i,&j);
   DispXY(i-10,j,p,COLORP);

   setwritemode(COPY_PUT);
   setcolor(SaveColor);
   setviewport(SaveViewPort.left,SaveViewPort.top,SaveViewPort.right,
              SaveViewPort.bottom,SaveViewPort.clip);
   MouseShow();
}

static int GetPrintPara()
{
   int i;
   float Tmp1,Tmp2,Tmp3,Tmp4;

   BEGINOK1;
   i=atof(LONG2FP(MessageGo(PrintCutWin[wPAGEOFFSETWIN],GETLINEBUFFER,0l,0l)));
   if (i<0) return i;
   Tmp1=atof(LONG2FP(MessageGo(PrintCutWin[wXPICWIN],GETLINEBUFFER,0l,0l)));
   Tmp2=atof(LONG2FP(MessageGo(PrintCutWin[wYPICWIN],GETLINEBUFFER,0l,0l)));
   Tmp3=atof(LONG2FP(MessageGo(PrintCutWin[wXSCALEWIN],GETLINEBUFFER,0l,0l)));
   Tmp4=atof(LONG2FP(MessageGo(PrintCutWin[wYSCALEWIN],GETLINEBUFFER,0l,0l)));
   PG.PageBlock[CurrentBlock].PageOffset=i;
   PG.PageBlock[CurrentBlock].Xoffset=Tmp1;
   PG.PageBlock[CurrentBlock].Yoffset=Tmp2;
   PG.PageBlock[CurrentBlock].Xscale =Tmp3;
   PG.PageBlock[CurrentBlock].Yscale =Tmp4;
   PG.PageInc =atof(LONG2FP(MessageGo(PrintCutWin[wPAGENUMWIN],GETLINEBUFFER,0l,0l)));
   for (i=0;i<4;i++)
   {
     if (MessageGo(PrintCutWin[wROTATE0+i],GETSTATUS,0l,0l))
     {
      PG.PageBlock[CurrentBlock].Rotate =i%4;
      break;
     }
   }
   return 0;
}

static void SetPrintPara()
{
   char p[30];
   int i,j;
   static int oldNum=0;

   BEGINOK;
   for (j=0;j<4;j++)
   {
     if (MessageGo(PrintCutWin[wROTATE0+j],GETSTATUS,0l,0l))
       {
        i=j+wROTATE0;
        MessageGo(PrintCutWin[i],SETSTATUS,0l,0l);
        break;
       }
   }

   SetP(PG.PageBlock[CurrentBlock].Xoffset,PrintCutWin[wXPICWIN]);
   SetP(PG.PageBlock[CurrentBlock].Yoffset,PrintCutWin[wYPICWIN]);
   SetP(PG.PageBlock[CurrentBlock].Xscale, PrintCutWin[wXSCALEWIN]);
   SetP(PG.PageBlock[CurrentBlock].Yscale, PrintCutWin[wYSCALEWIN]);
   SetPI(PG.PageBlock[CurrentBlock].PageOffset,PrintCutWin[wPAGEOFFSETWIN]);
   SetPI(PG.PageInc,PrintCutWin[wPAGENUMWIN]);

   MessageGo(PrintCutWin[wROTATE0+PG.PageBlock[CurrentBlock].Rotate%4],SETSTATUS,1l,0l);

  if (oldNum!=PG.Blocks)
  {
   sprintf(p,"%2d",PG.Blocks);
   GetXY(2,&i,&j);
   DispXY(i-10,j,p,COLORPN);
   oldNum=PG.Blocks;
  }
}

#ifdef NOT_USED
static void SetPrintPara1()
{
   BEGINOK;
   SetP(PG.PageBlock[CurrentBlock].Xoffset,PrintCutWin[wXPICWIN]);
   SetP(PG.PageBlock[CurrentBlock].Yoffset,PrintCutWin[wYPICWIN]);
}

static void SetPrintPara2()
{
   BEGINOK;
   SetP(PG.PageBlock[CurrentBlock].Xscale, PrintCutWin[wXSCALEWIN]);
   SetP(PG.PageBlock[CurrentBlock].Yscale, PrintCutWin[wYSCALEWIN]);
}
#endif

static void InitSubPage(HWND Window,int pt_w,int pt_h,int pg_w,int pg_h)
{
   HWND Mid;
   int xw,yw,i;
   int SaveColor;
   struct viewporttype SaveViewPort;

   WaitMessageEmpty();

   MouseHidden();
   getviewsettings(&SaveViewPort);
   SaveColor=getcolor();

   Mid=WindowGetFather(Window);
   ScreenDispWidth=WindowGetWidth(Window)-10;
   ScreenDispHeight=WindowGetHeight(Window)-10;
   WinX=ScreenX=WindowGetLeft(Window)+WindowGetLeft(Mid);
   WinY=ScreenY=WindowGetTop(Window)+WindowGetTop(Mid);

   SCRscaleX=SubP->TotalX/ScreenDispWidth*1.05;
   SCRscaleY=SubP->TotalY/ScreenDispHeight*1.05;
   if (SCRscaleX>SCRscaleY)
        SCRscaleY=SCRscaleX;
   else
        SCRscaleX=SCRscaleY;

   xw=mXw=pg_w/SCRscaleX;
   yw=mYw=pg_h/SCRscaleY;

   ScreenX=ScreenX+5+(ScreenDispWidth-mXw)/2;
   ScreenY=ScreenY+5+(ScreenDispHeight-mYw)/2;

   setviewport(WinX,WinY,WinX+ScreenDispWidth+9,WinY+ScreenDispHeight+9,1);
   for (i=0;i<PG.Blocks;i++)
   {
      int x0,x1,y0,y1;
      mXw=ScreenX-WinX;
      mYw=ScreenY-WinY;

    #ifdef OLD_VERSION
      x0=-PG.PageBlock[i].Xoffset*SCALEMETER/25.4/SCRscaleX+mXw;
      y0=-PG.PageBlock[i].Yoffset*SCALEMETER/25.4/SCRscaleY+mYw;
      x1=x0+pt_w/SCRscaleX;
      y1=y0+pt_h/SCRscaleY;
      if (PG.PageBlock[i].Reveser&1)
        setcolor(EGA_YELLOW);
      else
        setcolor(EGA_LIGHTCYAN);
      rectangle(x0,y0,x1,y1);
    #else
      x0=-PG.PageBlock[i].Xoffset*SCALEMETER/25.4/SCRscaleX+mXw;
      y0=-PG.PageBlock[i].Yoffset*SCALEMETER/25.4/SCRscaleY+mYw;
      x1=x0+pt_w/SCRscaleX-1;
      y1=y0+pt_h/SCRscaleY-1;
      if (PG.PageBlock[i].Reveser&1)    // not select this paper
        setcolor(EGA_LIGHTGRAY);
      else
        setcolor(EGA_LIGHTBLUE);

      bar(x0,y0,x1,y1);
      setcolor(EGA_BLACK);
      rectangle(x0,y0,x1,y1);
    #endif
   }

   setviewport(0,0,getmaxx(),getmaxy(),1);
   setcolor(EGA_WHITE);
   bar(ScreenX,ScreenY,ScreenX+xw,ScreenY+yw);
   setcolor(EGA_BLACK);
   rectangle(ScreenX,ScreenY,ScreenX+xw,ScreenY+yw);
   bar(ScreenX+10,ScreenY+15,ScreenX+xw-10,ScreenY+18);
   bar(ScreenX+10,ScreenY+25,ScreenX+xw-10,ScreenY+28);
   bar(ScreenX+10,ScreenY+35,ScreenX+xw-40,ScreenY+38);

   bar(ScreenX+10,ScreenY+yw/2+15,ScreenX+xw-10,ScreenY+yw/2+18);
   bar(ScreenX+10,ScreenY+yw/2+25,ScreenX+xw-10,ScreenY+yw/2+28);
   bar(ScreenX+10,ScreenY+yw/2+35,ScreenX+xw-40,ScreenY+yw/2+38);

   setcolor(SaveColor);
   setviewport(SaveViewPort.left,SaveViewPort.top,SaveViewPort.right,
              SaveViewPort.bottom,SaveViewPort.clip);
   MouseShow();
}

static int DispWin=1;
unsigned long DispSubPage(HWND Window,HMSG Message,long Param1,long Param2)
{
    int i;

    switch (Message)
    {
      case WINDOWINIT:
           DispWin=Window;
           break;
      case REDRAWMESSAGE:
           i=ListBoxDefaultProcedure(Window,Message,Param1,Param2);
           InitSubPage(Window,SubP->pt_w,SubP->pt_h,SubP->pg_w,SubP->pg_h);
           return i;
      case MOUSELEFTUP:
           for (i=0;i<PG.Blocks;i++)
           {
              int x0,x1,y0,y1,x,y;
              x=(Param1>>16);
              y=(Param1&0xffff);
              mXw=ScreenX-WinX;
              mYw=ScreenY-WinY;
              x0=-PG.PageBlock[i].Xoffset*SCALEMETER/25.4/SCRscaleX+mXw;
              y0=-PG.PageBlock[i].Yoffset*SCALEMETER/25.4/SCRscaleY+mYw;
              x1=x0+SubP->pt_w/SCRscaleX;
              y1=y0+SubP->pt_h/SCRscaleY;
              if (x>=x0&&x<=x1&&y>=y0&&y<=y1)
              {
                PG.PageBlock[i].Reveser ^= 1;
                break;
              }
           }

           if(i<PG.Blocks)
              MessageInsert(Window,REDRAWMESSAGE,0l,0l);
           break;
      case GETFOCUS:              // ByHance, 97,6.3
           if(fGetFocusByKey)
              return FALSE;
      default:
           return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
    }
    return TRUE;
}

unsigned long SubPageMain(HWND Window,HMSG Message,long Param1,long Param2)
{
  int i;
  switch (Message)
   {
    case 10091:         //add all
                for (i=0;i<PG.Blocks;i++)
                  PG.PageBlock[i].Reveser &=0xfffffffe;
                MessageInsert(DispWin,REDRAWMESSAGE,0l,0l);
                break;
    case 10092:         //del all
                for (i=0;i<PG.Blocks;i++)
                  PG.PageBlock[i].Reveser |=1;
                MessageInsert(DispWin,REDRAWMESSAGE,0l,0l);
                break;
   }
  return(DialogDefaultProcedure(Window, Message, Param1, Param2));
}

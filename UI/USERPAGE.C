/*-------------------------------------------------------------------
* Name: userpage.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

//////////////Must Be mm
#define DEFAULTPAGEHDISTANT 32         // 31.7            // unit: mm
#define DEFAULTPAGEVDISTANT 25

enum {
   B4b=0, B4, A4b, A4, B5b, B5, USERPAPER
};
static int PageWidths[MaxPaperPara+1]= { 297, 260, 210, 184, 149, 130,210};
static int PageHeights[MaxPaperPara+1]={ 420, 368, 297, 260, 210, 184,297};
     // xx, xx, xx, 148x210, xx, 97x143
static int LeftMargin[MaxPaperPara+1]  = { 32, 32, 32, 26, 23, 21, 32 };
static int RightMargin[MaxPaperPara+1] = { 32, 32, 32, 26, 23, 21, 32 };
static int TopMargin[MaxPaperPara+1]   = { 25, 25, 25, 35, 30, 28, 25 };
static int BottomMargin[MaxPaperPara+1]= { 25, 25, 25, 35, 30, 28, 25 };
//#define RightMargin     LeftMargin
//#define BottomMargin    TopMargin

static int  tmp_pagesize=A4b;

static int my_change(int x)
{
    return (int)((float)x*25.4f/SCALEMETER+0.5f);
}

int Page_Setup(HWND Window)
{
    int idx;
    int ret_v,i;

    if (!fNewA4)
    {
        ret_v=MakeDialogBox(Window,PageDialog);
        if (ret_v) return ret_v;
    }
    else
    {
        idx=A4b;
        fNewA4=FALSE;
        TmpPage.PageType &=0xc0;
        TmpPage.PageType |=idx;
        #define CH(val) (ConvertToUserMeter((float)(val))*SCALEMETER)
        TmpPage.PageWidth=CH(PageWidths[idx]);
        TmpPage.PageHeight=CH(PageHeights[idx]);
        TmpPage.MarginLeft=CH(LeftMargin[idx]);
        TmpPage.MarginRight=CH(RightMargin[idx]);
        TmpPage.MarginTop=CH(TopMargin[idx]);
        TmpPage.MarginBottom=CH(BottomMargin[idx]);
        TmpPage.ColumnDistant=CH(3);
        #undef CH
        ret_v=0;
    }

    idx=(TmpPage.PageType&0xf);
    PageWidths[idx]=my_change(TmpPage.PageWidth);
    PageHeights[idx]=my_change(TmpPage.PageHeight);
    LeftMargin[idx]=my_change(TmpPage.MarginLeft);
    RightMargin[idx]=my_change(TmpPage.MarginRight);
    TopMargin[idx]=my_change(TmpPage.MarginTop);
    BottomMargin[idx]=my_change(TmpPage.MarginBottom);

    if (GlobalPageRotate && idx!=USERPAPER)
    {
        i=TmpPage.PageWidth;
        TmpPage.PageWidth=TmpPage.PageHeight;
        TmpPage.PageHeight=i;

        i=TmpPage.MarginLeft;
        TmpPage.MarginLeft=TmpPage.MarginTop;
        TmpPage.MarginTop=i;

        i=TmpPage.MarginRight;
        TmpPage.MarginRight=TmpPage.MarginBottom;
        TmpPage.MarginBottom=i;
    }

    return ret_v;
}

static const char paperSize_fmt_str[]="%d,%d,%d,%d,%d,%d,%d";
void init_paper(void)
{
  char paper[128];
  int *p;

  GetProfileString( ProfileName,PaperSizeSection,PageWidthsEntry,paper,"");
  if (paper[0]==0) return ;
  p=&PageWidths[0];
  sscanf(paper,paperSize_fmt_str,p,p+1,p+2,p+3,p+4,p+5,p+6);

  GetProfileString( ProfileName,PaperSizeSection,PageHeightsEntry,paper,"");
  if (paper[0]==0) return ;
  p=&PageHeights[0];
  sscanf(paper,paperSize_fmt_str,p,p+1,p+2,p+3,p+4,p+5,p+6);

  GetProfileString( ProfileName,PaperSizeSection,LeftMarginEntry,paper,"");
  if (paper[0]==0) return ;
  p=&LeftMargin[0];
  sscanf(paper,paperSize_fmt_str,p,p+1,p+2,p+3,p+4,p+5,p+6);

  GetProfileString( ProfileName,PaperSizeSection,RightMarginEntry,paper,"");
  if (paper[0]==0) return ;
  p=&RightMargin[0];
  sscanf(paper,paperSize_fmt_str,p,p+1,p+2,p+3,p+4,p+5,p+6);

  GetProfileString( ProfileName,PaperSizeSection,TopMarginEntry,paper,"");
  if (paper[0]==0) return ;
  p=&TopMargin[0];
  sscanf(paper,paperSize_fmt_str,p,p+1,p+2,p+3,p+4,p+5,p+6);

  GetProfileString( ProfileName,PaperSizeSection,BottomMarginEntry,paper,"");
  if (paper[0]==0) return ;
  p=&BottomMargin[0];
  sscanf(paper,paperSize_fmt_str,p,p+1,p+2,p+3,p+4,p+5,p+6);
}

void save_paper(void)
{
  char paper[128];
  int *p;

  p=&PageWidths[0];
  sprintf(paper,paperSize_fmt_str,*p,*(p+1),*(p+2),*(p+3),*(p+4),*(p+5),*(p+6));
  SetProfileString( ProfileName,PaperSizeSection,PageWidthsEntry,paper);

  p=&PageHeights[0];
  sprintf(paper,paperSize_fmt_str,*p,*(p+1),*(p+2),*(p+3),*(p+4),*(p+5),*(p+6));
  SetProfileString( ProfileName,PaperSizeSection,PageHeightsEntry,paper);

  p=&LeftMargin[0];
  sprintf(paper,paperSize_fmt_str,*p,*(p+1),*(p+2),*(p+3),*(p+4),*(p+5),*(p+6));
  SetProfileString( ProfileName,PaperSizeSection,LeftMarginEntry,paper);

  p=&RightMargin[0];
  sprintf(paper,paperSize_fmt_str,*p,*(p+1),*(p+2),*(p+3),*(p+4),*(p+5),*(p+6));
  SetProfileString( ProfileName,PaperSizeSection,RightMarginEntry,paper);

  p=&TopMargin[0];
  sprintf(paper,paperSize_fmt_str,*p,*(p+1),*(p+2),*(p+3),*(p+4),*(p+5),*(p+6));
  SetProfileString( ProfileName,PaperSizeSection,TopMarginEntry,paper);

  p=&BottomMargin[0];
  sprintf(paper,paperSize_fmt_str,*p,*(p+1),*(p+2),*(p+3),*(p+4),*(p+5),*(p+6));
  SetProfileString( ProfileName,PaperSizeSection,BottomMarginEntry,paper);
}

unsigned long PageSizeProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;
  HWND MidWindow1,MidWindow2;
  char MidString[20];

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);  /* Page Size */
         if ((TmpPage.PageType&0x3f)!=Order)
            break;
         tmp_pagesize=Order;
         MessageGo(Window,SETSTATUS,(long)Order+1,0l);
         MessageInsert(Window,SELECTSELECTED,(long)Order+1,0l); // init page width, ....
         break;
    case SELECTSELECTED:
         tmp_pagesize=Order=RadioGetOrder(Window);

         /*-- page width window --*/
         MidWindow1=WindowGetChild(WindowGetFather(WindowGetFather(Window)));
         while (MidWindow1)      // frame window
         {
           MidWindow2=WindowGetChild(MidWindow1);
           while (MidWindow2)
           {
             if (WindowGetProcedure(MidWindow2)==PageWidthProcedure)
                break;
             MidWindow2=WindowGetNext(MidWindow2);
           }
           if (MidWindow2)
              break;
           MidWindow1=WindowGetNext(MidWindow1);
         }

         if (MidWindow1)
         {
            //if (Order<MaxPaperPara)
               gcvt(PageWidths[Order],6,MidString);
            /*----------
            else {
               TmpPage.PageWidth=PageWidths[A4b]; // ByHance, = A4 paper
               gcvt(TmpPage.PageWidth,6,MidString);
            }
            ----------*/
            MessageGo(MidWindow2,SETLINEBUFFER,FP2LONG(MidString),0l);
            MessageInsert(MidWindow2,REDRAWMESSAGE,0l,GetEditorWidth(WindowGetUserData(MidWindow2)));

            /*---- page height window ----*/
            MidWindow1=WindowGetChild(WindowGetFather(MidWindow2)); // same frame
            while (MidWindow1)
            {
              if (WindowGetProcedure(MidWindow1)==PageHeightProcedure)
                 break;
              MidWindow1=WindowGetNext(MidWindow1);
            }

            //if (Order<MaxPaperPara)
               gcvt(PageHeights[Order],6,MidString);
            /*----------
            else {
               TmpPage.PageHeight=PageHeights[A4b];  // ByHance, = A4 paper
               gcvt(TmpPage.PageHeight,6,MidString);
            }
            ----------*/
            MessageGo(MidWindow1,SETLINEBUFFER,FP2LONG(MidString),0l);
            MessageInsert(MidWindow1,REDRAWMESSAGE,0l,GetEditorWidth(WindowGetUserData(MidWindow1)));
         }

         /*-- page margin window --*/
         MidWindow1=WindowGetChild(WindowGetFather(WindowGetFather(Window)));
         while (MidWindow1)      // frame window
         {
           MidWindow2=WindowGetChild(MidWindow1);
           while (MidWindow2)
           {
             if (WindowGetProcedure(MidWindow2)==PageMarginLeftProcedure)
                break;
             MidWindow2=WindowGetNext(MidWindow2);
           }
           if (MidWindow2)
              break;
           MidWindow1=WindowGetNext(MidWindow1);
         }

         if (MidWindow1)
         {
            //if (Order<MaxPaperPara)
               gcvt(LeftMargin[Order],6,MidString);
            /*----------
            else {
               TmpPage.MarginLeft=LeftMargin[A4b]; // ByHance, = A4 paper
               gcvt(TmpPage.MarginLeft,6,MidString);
            }
            ----------*/
            MessageGo(MidWindow2,SETLINEBUFFER,FP2LONG(MidString),0l);
            MessageInsert(MidWindow2,REDRAWMESSAGE,0l,GetEditorWidth(WindowGetUserData(MidWindow2)));

            /*---- top margin window ----*/
            MidWindow1=WindowGetChild(WindowGetFather(MidWindow2)); // same frame
            while (MidWindow1)
            {
              if (WindowGetProcedure(MidWindow1)==PageMarginTopProcedure)
                 break;
              MidWindow1=WindowGetNext(MidWindow1);
            }

            //if (Order<MaxPaperPara)
               gcvt(TopMargin[Order],6,MidString);
            /*----------
            else {
               TmpPage.MarginTop=TopMargin[A4b];  // ByHance, = A4 paper
               gcvt(TmpPage.MarginTop,6,MidString);
            }
            ----------*/
            MessageGo(MidWindow1,SETLINEBUFFER,FP2LONG(MidString),0l);
            MessageInsert(MidWindow1,REDRAWMESSAGE,0l,GetEditorWidth(WindowGetUserData(MidWindow1)));

            /*---- right margin window ----*/
            MidWindow1=WindowGetChild(WindowGetFather(MidWindow2)); // same frame
            while (MidWindow1)
            {
              if (WindowGetProcedure(MidWindow1)==PageMarginRightProcedure)
                 break;
              MidWindow1=WindowGetNext(MidWindow1);
            }

            //if (Order<MaxPaperPara)
               gcvt(RightMargin[Order],6,MidString);
            /*----------
            else {
               TmpPage.MarginRight=RightMargin[A4b];  // ByHance, = A4 paper
               gcvt(TmpPage.MarginRight,6,MidString);
            }
            ----------*/
            MessageGo(MidWindow1,SETLINEBUFFER,FP2LONG(MidString),0l);
            MessageInsert(MidWindow1,REDRAWMESSAGE,0l,GetEditorWidth(WindowGetUserData(MidWindow1)));

            /*---- top margin window ----*/
            MidWindow1=WindowGetChild(WindowGetFather(MidWindow2)); // same frame
            while (MidWindow1)
            {
              if (WindowGetProcedure(MidWindow1)==PageMarginBottomProcedure)
                 break;
              MidWindow1=WindowGetNext(MidWindow1);
            }

            //if (Order<MaxPaperPara)
               gcvt(BottomMargin[Order],6,MidString);
            /*----------
            else {
               TmpPage.MarginBottom=BottomMargin[A4b];  // ByHance, = A4 paper
               gcvt(TmpPage.MarginBottom,6,MidString);
            }
            ----------*/
            MessageGo(MidWindow1,SETLINEBUFFER,FP2LONG(MidString),0l);
            MessageInsert(MidWindow1,REDRAWMESSAGE,0l,GetEditorWidth(WindowGetUserData(MidWindow1)));
         }
         break;
    case DIALOGBOXOK:
         tmp_pagesize=Order=RadioGetOrder(Window);
         if (MessageGo(Window,GETSTATUS,0l,0l))
         {
            TmpPage.PageType&=0xc0;
            TmpPage.PageType|=Order;
         }
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

/*----------------------------
unsigned long PageDirectionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);  // Page Direction
         if (((TmpPage.PageType&0xc0)>>6)==Order)
            MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case DIALOGBOXOK:
         Order=RadioGetOrder(Window);
         if (MessageGo(Window,GETSTATUS,0l,0l))
         {
            TmpPage.PageType&=0x3f;
            TmpPage.PageType|=(Order<<6);
         }
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}
------------------------*/

unsigned long PageWidthProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           gcvt(TmpPage.PageWidth,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if( (TmpPage.PageWidth=atof(MidString)*10)<100 )    // <10.0 mm
            // || TmpPage.PageWidth>MAXPAGEWIDTH)         // > 580.0 mm
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDPAGEWIDTH),1,1);
              return(FALSE);
           }
           TmpPage.PageWidth=ConvertToUserMeter((float)TmpPage.PageWidth/10)*SCALEMETER;
           break;
         }
    case GETFOCUS:
    case MOUSEMOVE:
         if(tmp_pagesize<MaxPaperPara)
            return FALSE;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PageHeightProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           gcvt(TmpPage.PageHeight,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if( (TmpPage.PageHeight=atof(MidString)*10)<100 )
            // || TmpPage.PageHeight>MAXPAGEHEIGHT)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDPAGEHEIGHT),1,1);
              return(FALSE);
           }
           TmpPage.PageHeight=ConvertToUserMeter((float)TmpPage.PageHeight/10)*SCALEMETER;
           break;
         }
    case GETFOCUS:
    case MOUSEMOVE:
         if(tmp_pagesize<MaxPaperPara)
             return(FALSE);
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PageMarginLeftProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           // TmpPage.MarginLeft=DEFAULTPAGEHDISTANT;
           gcvt(TmpPage.MarginLeft,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
            lbl_err:
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDPAGEMARGINLEFT),1,1);
              return(FALSE);
           }
           TmpPage.MarginLeft=ConvertToUserMeter(atof(MidString))*SCALEMETER;
           if( TmpPage.MarginLeft<0)
           {
              goto lbl_err;
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PageMarginTopProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //TmpPage.MarginTop=DEFAULTPAGEVDISTANT;
           gcvt(TmpPage.MarginTop,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
            lbl_err:
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDPAGEMARGINTOP),1,1);
              return(FALSE);
           }

           TmpPage.MarginTop=ConvertToUserMeter(atof(MidString))*SCALEMETER;
           if( TmpPage.MarginTop<0)
           {
              goto lbl_err;
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PageMarginRightProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //TmpPage.MarginRight=DEFAULTPAGEHDISTANT;
           gcvt(TmpPage.MarginRight,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
            lbl_err:
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDPAGEMARGINRIGHT),1,1);
              return(FALSE);
           }

           TmpPage.MarginRight=ConvertToUserMeter(atof(MidString))*SCALEMETER;

           if( TmpPage.MarginRight<0)
           {
              goto lbl_err;
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PageMarginBottomProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //TmpPage.MarginBottom=DEFAULTPAGEVDISTANT;
           gcvt(TmpPage.MarginBottom,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
            lbl_err:
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDPAGEMARGINBOTTOM),1,1);
              return(FALSE);
           }

           TmpPage.MarginBottom=ConvertToUserMeter(atof(MidString))*SCALEMETER;
           if( TmpPage.MarginBottom<0)
           {
              goto lbl_err;
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

/*-----------------------
unsigned long PageVirtualPageProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         if (TmpPage.VirtualPage&0x7f)
            MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case DIALOGBOXOK:
         TmpPage.VirtualPage&=0x80;
         if (MessageGo(Window,GETSTATUS,0l,0l))
            TmpPage.VirtualPage|=1;
         else
            TmpPage.VirtualPage=0;
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}
------*/

unsigned long PageInitialBoxProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         if (!(TmpPage.VirtualPage&0x7f))
            MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case DIALOGBOXOK:
         TmpPage.VirtualPage&=0x7f;
         if (MessageGo(Window,GETSTATUS,0l,0l)==FALSE)
            TmpPage.VirtualPage|=0x80;
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PageColumnProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           TmpPage.PageColumn=1;
           itoa(TmpPage.PageColumn,MidString,10);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
            lbl_err:
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCOLUMN),1,1);
              return(FALSE);
           }
           TmpPage.PageColumn=atoi(MidString);
           if( TmpPage.PageColumn<=0 || TmpPage.PageColumn>10)
           {
              goto lbl_err;
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PageColumnDistantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           TmpPage.ColumnDistant=3;
           gcvt(TmpPage.ColumnDistant,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
            lbl_err:
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCOLUMNDISTANT),1,1);
              return(FALSE);
           }
           TmpPage.ColumnDistant=atof(MidString);
           if( TmpPage.ColumnDistant<0 || TmpPage.ColumnDistant>100)
           {
              goto lbl_err;
           }
           TmpPage.ColumnDistant=ConvertToUserMeter(atof(MidString))*SCALEMETER;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

////////////////////GotoPage//////////////////////////////////////////
static int tmp_pn;
int GetPageNumber() { return tmp_pn; }
unsigned long GotoPageProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  char str[20];
  int pn;                   // page number

  switch (Message)
  {
    case WINDOWINIT:
         pn=PageHandleToNumber(GlobalCurrentPage);
         sprintf(str,"%d",pn+1);
         MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
         break;
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           pn=atoi(MidString);
           if (pn<1||pn>TotalPage) {
                  if (pn<1) pn = 1;
                  if (pn>TotalPage) pn = TotalPage;
                  sprintf(str,"%d",pn);
                  MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
                  MessageGo(Window,WMPAINT,0l,GetEditorWidth(WindowGetUserData(Window)));
                  return FALSE;
           }
           tmp_pn = pn-1;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}


////////////////////MovePage//////////////////////////////////////////
static int tmp_moveflag = 0;
int GetPageMoveOption() { return tmp_moveflag; }
unsigned long PageMoveOptionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;
  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (tmp_moveflag != Order) break;
         MessageGo(Window,SETSTATUS,1l,0l);
         MessageInsert(Window,SELECTSELECTED,1l,0l);
         break;
   /*-----------
    case SELECTSELECTED:
         tmp_moveflag=RadioGetOrder(Window);
         tmp_moveflag = Order;
         break;
   -----------*/
    case DIALOGBOXOK:
         Order=RadioGetOrder(Window);
         if (MessageGo(Window,GETSTATUS,0l,0l))
            tmp_moveflag = Order;
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PageMoveProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  char str[20];

  switch (Message)
  {
    case WINDOWINIT:
         sprintf(str,"%d",1);
         MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
         break;
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;
           int pn;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           pn=atoi(MidString);
           if (pn<1||pn>TotalPage) {
                  if (pn<1) pn = 1;
                  if (pn>TotalPage) pn = TotalPage;
                  sprintf(str,"%d",pn);
                  MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
                  MessageGo(Window,WMPAINT,0l,GetEditorWidth(WindowGetUserData(Window)));
                  return FALSE;
           }
           tmp_pn = pn-1;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

////////////////////GotoLine//////////////////////////////////////////
int GetLineNumber() { return tmp_pn; }
unsigned long GotoLineProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  char str[20];
  int pn;                   // page number

  switch (Message)
  {
    case WINDOWINIT:
         pn=1;
         sprintf(str,"%d",pn);
         MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
         break;
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           pn=atoi(MidString);
           if (pn<1) {
                  pn = 1;
                  sprintf(str,"%d",pn);
                  MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
                  MessageGo(Window,WMPAINT,0l,GetEditorWidth(WindowGetUserData(Window)));
                  return FALSE;
           }
           tmp_pn = pn;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}


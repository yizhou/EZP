/*-------------------------------------------------------------------
* Name: userprnc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

enum { CURRENT_PAGE, ALL_PAGES, SELECT_PAGE};
int tmp_range = ALL_PAGES;               // print all pages

unsigned long PrintRangeProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;
  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (tmp_range != Order) break;
         MessageGo(Window,SETSTATUS,1l,0l);
         MessageInsert(Window,SELECTSELECTED,0l,0l);
         break;
   /*---------*/
    case SELECTSELECTED:
         tmp_range = RadioGetOrder(Window);
         break;
   /*---------
    case DIALOGBOXOK:
         Order=RadioGetOrder(Window);
         if (MessageGo(Window,GETSTATUS,0l,0l))
           tmp_range = Order;
         break;
   -----------*/
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PrintStartPageProc(HWND Window,HMSG Message,long Param1,long Param2)
{
  char str[20];
  long MidBuffer;
  char *MidString;
  int pn;

  switch (Message)
  {
    case WINDOWINIT:
         sprintf(str,"%d",StartPrintPage);
         MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
         break;
    case DIALOGBOXOK:
         switch(tmp_range)
         {
           case CURRENT_PAGE:
                StartPrintPage=PageHandleToNumber(GlobalCurrentPage)+1;
                break;
           case ALL_PAGES:
                StartPrintPage = 1;
                break;
           case SELECT_PAGE:
                MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
                MidString=(char *)LONG2FP(MidBuffer);
                pn=atoi(MidString);
                if (pn<1||pn>TotalPage) {
                       if (pn<1) pn = 1;
                       else
                       if (pn>TotalPage) pn = TotalPage;
                       sprintf(str,"%d",pn);
                       MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
                       MessageGo(Window,WMPAINT,0l,GetEditorWidth(WindowGetUserData(Window)));
                       return FALSE;
                }

                StartPrintPage = pn;
                break;
         }
         break;
    case GETFOCUS:
    case MOUSEMOVE:
         if(tmp_range!=SELECT_PAGE)
              return(FALSE);
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PrintTotalPageProc(HWND Window,HMSG Message,long Param1,long Param2)
{
  char str[20];
  long MidBuffer;
  char *MidString;
  int pn;

  switch (Message)
  {
    case WINDOWINIT:
         sprintf(str,"%d",TotalPage);
         MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
         break;
    case DIALOGBOXOK:
         switch(tmp_range)
         {
           case CURRENT_PAGE:
                EndPrintPage=1;
                break;
           case ALL_PAGES:
                EndPrintPage = TotalPage;
                break;
           case SELECT_PAGE:
                MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
                MidString=(char *)LONG2FP(MidBuffer);
                pn=atoi(MidString);
                if (pn<1 || pn>TotalPage) {
                       if (pn<1) pn = 1;
                       else
                       if (pn>TotalPage) pn=TotalPage;
                       sprintf(str,"%d",pn);
                       MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
                       MessageGo(Window,WMPAINT,0l,GetEditorWidth(WindowGetUserData(Window)));
                       return FALSE;
                }
                EndPrintPage = pn;
                break;
         }
         break;
    case GETFOCUS:
    case MOUSEMOVE:
         if(tmp_range!=SELECT_PAGE)
              return(FALSE);
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PrintCopyNumProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  char str[20];

  switch (Message)
  {
    case WINDOWINIT:
         sprintf(str,"%d",PrintCopyN);
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
           if (pn<1 || pn>99) {
                  if (pn<1) pn = 1;
                  else
                  if (pn>99) pn=99;
                  sprintf(str,"%d",pn);
                  MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
                  MessageGo(Window,WMPAINT,0l,GetEditorWidth(WindowGetUserData(Window)));
                  return FALSE;
           }
           PrintCopyN = pn;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PrinterList(HWND Window,HMSG Message,long Param1,long Param2)
{
  int i,hlist;
  HWND MidWin;

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,LISTSETITEMHEIGHT,16,0);
         MessageGo(Window,LISTSETITEMLENGTH,30,0);
         hlist = WindowList(Window);
         ListSetTotal(hlist,0);
         ListSetCurrent(hlist,0);
         ListSetTop(hlist,0);
         i=0;
         while(PrinterName[i])
         {
           MessageGo(Window,LISTINSERTITEMSORTED,FP2LONG(PrinterName[i]), 0L);
           i++;
         }

         MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
         if (CurrentPrinter>7) ListSetTop(hlist,CurrentPrinter-7);
             else ListSetTop(hlist,0);
         ListSetCurrent(hlist,CurrentPrinter);
         MessageInsert(Window,REDRAWMESSAGE,0L,
         MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );

         //if (CurrentPrinter>7) ListSetTop(hlist,CurrentPrinter-7);
         //    else ListSetTop(hlist,0);
         //ListSetCurrent(hlist,CurrentPrinter);
         //ListBoxDefaultProcedure(Window,KEYDOWN,DOWN,0L);  // ByHance,96,4.11
         //ListBoxDefaultProcedure(Window,KEYDOWN,UP,0L);
         //if(ListGetTotal(hlist)==CurrentPrinter+1)
         //   ListBoxDefaultProcedure(Window,KEYDOWN,DOWN,0L);  // ByHance,96,5.24
         break;

    case ITEMSELECT:
         hlist = WindowList(Window);
         CurrentPrinter= ListGetCurrent(hlist);
         MidWin=WindowGetChild( WindowGetFather(Window) );
         while (MidWin) {
            if(WindowGetProcedure(MidWin)==DefaultPrinterProcedure)
            {
                MessageInsert(MidWin,WINDOWINIT,0,0);
                MessageInsert(MidWin,WMPAINT,0,WindowGetHeight(MidWin));
                break;
            }
            MidWin=WindowGetNext(MidWin);
         }
         break;
    case LISTBOXCONFIRM:
         MessageGo(WindowGetFather(Window),DIALOGBOXOK,0l,0l);
         break;
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long DefaultPrinterProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{                       // only used for display printer's name
  char str[64];

  switch (Message)
  {
    case WINDOWINIT:
         strcpy(str,HintPrinterName[CurrentPrinter]);
         MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
         break;
    case GETFOCUS:
    case MOUSEMOVE:
         return(FALSE);
    case DIALOGBOXOK:
         break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

int tmp_fileflag = 0;
int GetPrint2FileOption() { return tmp_fileflag; }
unsigned long Print2FileOptionProc(HWND Window,HMSG Message,long Param1,long Param2)
{
 // int Order;
  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETSTATUS,tmp_fileflag,0l);
         break;
    case DIALOGBOXOK:
         tmp_fileflag = MessageGo(Window,GETSTATUS,0l,0l);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PrintReverse(HWND Window,HMSG Message,long Param1,long Param2)
{
 // int Order;
  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETSTATUS,GlobalReverse,0l);
         break;
    case DIALOGBOXOK:
         GlobalReverse = MessageGo(Window,GETSTATUS,0l,0l);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PageRotate(HWND Window,HMSG Message,long Param1,long Param2)
{

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETSTATUS,GlobalPageRotate,0l);
         break;
    case DIALOGBOXOK:
         GlobalPageRotate = MessageGo(Window,GETSTATUS,0l,0l);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PrintRorate90(HWND Window,HMSG Message,long Param1,long Param2)
{
 // int Order;
  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETSTATUS,GlobalRorate90,0l);
         break;
    case DIALOGBOXOK:
         GlobalRorate90 = MessageGo(Window,GETSTATUS,0l,0l);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PrintPause(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETSTATUS,GlobalPause,0l);
         break;
    case DIALOGBOXOK:
         GlobalPause = MessageGo(Window,GETSTATUS,0l,0l);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PrintJob(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETSTATUS,GlobalJob,0l);
         break;
    case DIALOGBOXOK:
         GlobalJob = MessageGo(Window,GETSTATUS,0l,0l);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PrintHollow(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETSTATUS,GlobalHollow,0l);
         break;
    case DIALOGBOXOK:
         GlobalHollow = MessageGo(Window,GETSTATUS,0l,0l);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG PageFootEnableProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
 // int Order;
  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETSTATUS,tmp_footflag,0l);
         break;
    case SELECTSELECTED:
    case DIALOGBOXOK:
         tmp_footflag=MessageGo(Window,GETSTATUS,1l,0l);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG PageHeadEnableProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
 // int Order;
  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETSTATUS,tmp_headflag,0l);
         break;
    case SELECTSELECTED:
    case DIALOGBOXOK:
         tmp_headflag=MessageGo(Window,GETSTATUS,1l,0l);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG PgFtTDProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (tmp_foottopflag!=Order)
            break;
         MessageGo(Window,SETSTATUS,(long)Order+1,0l);
         break;
   /*-----------
    case SELECTSELECTED:
         tmp_foottopflag=RadioGetOrder(Window);
         break;
   -----------*/
    case DIALOGBOXOK:
         if (MessageGo(Window,GETSTATUS,0l,0l))
             tmp_foottopflag=RadioGetOrder(Window);
         break;
    case GETFOCUS:
    case MOUSEMOVE:
         if(!tmp_footflag)
            return(FALSE);
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG PgFtLRProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (tmp_footleftflag!=Order)
            break;
         MessageGo(Window,SETSTATUS,(long)Order+1,0l);
         break;
   /*-----------
    case SELECTSELECTED:
         tmp_footleftflag=RadioGetOrder(Window);
         break;
   -----------*/
    case DIALOGBOXOK:
         if (MessageGo(Window,GETSTATUS,0l,0l))
             tmp_footleftflag=RadioGetOrder(Window);
         break;
    case GETFOCUS:
    case MOUSEMOVE:
         if(!tmp_footflag)
            return(FALSE);
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG PgFtPrevProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (tmp_footprevflag!=Order)
            break;
         MessageGo(Window,SETSTATUS,(long)Order+1,0l);
         break;
   /*-----------
    case SELECTSELECTED:
         tmp_footprevflag=RadioGetOrder(Window);
         break;
   -----------*/
    case DIALOGBOXOK:
         if (MessageGo(Window,GETSTATUS,0l,0l))
             tmp_footprevflag=RadioGetOrder(Window);
         break;
    case GETFOCUS:
    case MOUSEMOVE:
         if(!tmp_footflag)
            return(FALSE);
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG PgFtStartNumProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  char str[20];
  long MidBuffer;
  char *MidString;
  int  n;

  switch (Message)
  {
    case WINDOWINIT:
         sprintf(str,"%d",PgFtStartNum);
         MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
         break;
    case DIALOGBOXOK:
         MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
         MidString=(char *)LONG2FP(MidBuffer);
         n=atoi(MidString);
         if (n<1||n>60000)
         {
            n = 1;
            sprintf(str,"%d",n);
            MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
            MessageGo(Window,WMPAINT,0l,GetEditorWidth(WindowGetUserData(Window)));
            return FALSE;
         }
         PgFtStartNum = n;
         break;

    case GETFOCUS:
    case MOUSEMOVE:
         if(!tmp_footflag)
            return(FALSE);
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG PageHeadLeftProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char *MidString;

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETLINEBUFFER,FP2LONG(PageHeadLeftStr),0l);
         break;
    case DIALOGBOXOK:
         MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
         MidString=(char *)LONG2FP(MidBuffer);
         if(strlen(MidString)>PAGEHEADSTRMAXLEN)
              MidString[PAGEHEADSTRMAXLEN]=0;
         strcpy(PageHeadLeftStr,MidString);
         break;

    case GETFOCUS:
    case MOUSEMOVE:
         if(!tmp_headflag)
            return(FALSE);
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG PageHeadRightProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char *MidString;

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETLINEBUFFER,FP2LONG(PageHeadRightStr),0l);
         break;
    case DIALOGBOXOK:
         MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
         MidString=(char *)LONG2FP(MidBuffer);
         if(strlen(MidString)>PAGEHEADSTRMAXLEN)
              MidString[PAGEHEADSTRMAXLEN]=0;
         strcpy(PageHeadRightStr,MidString);
         break;

    case GETFOCUS:
    case MOUSEMOVE:
         if(!tmp_headflag)
            return(FALSE);
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG PgHdLRProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (tmp_headleftflag!=Order)
            break;
         MessageGo(Window,SETSTATUS,(long)Order+1,0l);
         break;
   /*-----------
    case SELECTSELECTED:
         tmp_headleftflag=RadioGetOrder(Window);
         break;
   -----------*/
    case DIALOGBOXOK:
         if (MessageGo(Window,GETSTATUS,0l,0l))
             tmp_headleftflag=RadioGetOrder(Window);
         break;
    case GETFOCUS:
    case MOUSEMOVE:
         if(!tmp_headflag)
            return(FALSE);
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG PgHdLineProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (tmp_headlineflag!=Order)
            break;
         MessageGo(Window,SETSTATUS,(long)Order+1,0l);
         break;
   /*-----------
    case SELECTSELECTED:
         tmp_headlineflag=RadioGetOrder(Window);
         break;
   -----------*/
    case DIALOGBOXOK:
         if (MessageGo(Window,GETSTATUS,0l,0l))
             tmp_headlineflag=RadioGetOrder(Window);
         break;
    case GETFOCUS:
    case MOUSEMOVE:
         if(!tmp_headflag)
            return(FALSE);
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

int pic_dpi=300;
unsigned long PicDPIProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  char str[20];

  switch (Message)
  {
    case WINDOWINIT:
         sprintf(str,"%d",pic_dpi);
         MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
         break;
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           pic_dpi=atoi(MidString);
           if(pic_dpi<0)
           {
              pic_dpi=300;
              sprintf(str,"%d",pic_dpi);
              MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
              MessageGo(Window,WMPAINT,0l,GetEditorWidth(WindowGetUserData(Window)));
              return FALSE;
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}


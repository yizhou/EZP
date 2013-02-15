/*-------------------------------------------------------------------
* Name: userfaxc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

/*-------------- fax setup ------------------------------------------*/
unsigned long TelPlusProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;
  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (fTelTone != Order) break;
         MessageGo(Window,SETSTATUS,1l,0l);
         MessageInsert(Window,SELECTSELECTED,0l,0l);
         break;
    case SELECTSELECTED:
         fTelTone = RadioGetOrder(Window);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long TelAutoDialProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;
  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (fTelManualDial != Order) break;
         MessageGo(Window,SETSTATUS,1l,0l);
         MessageInsert(Window,SELECTSELECTED,0l,0l);
         break;
    case SELECTSELECTED:
         fTelManualDial = RadioGetOrder(Window);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG TelComXProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char *MidString;
  char str[20];

  switch (Message)
  {
    case WINDOWINIT:
         itoa(ComX,str,10);
         MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
         break;
    case DIALOGBOXOK:
         MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
         MidString=(char *)LONG2FP(MidBuffer);
         ComX=atoi(MidString);
         if(ComX<1 || ComX>4)
         {
              MessageBox(GetTitleString(ERRORINFORM),
                         "本版本只支持COM1至COM4!",
                         1,1);
              return(FALSE);
         }
         break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG TelLocalIDProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char *MidString;

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETLINEBUFFER,FP2LONG(LocalTelId),0l);
         break;
    case DIALOGBOXOK:
         MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
         MidString=(char *)LONG2FP(MidBuffer);
         MidString[20]=0;
         strcpy(LocalTelId,MidString);
         break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

/*-------------- send fax ------------------------------------------*/
ULONG TelRemotePhoneNumProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char *MidString;

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETLINEBUFFER,FP2LONG(DialNumber),0l);
         break;
    case DIALOGBOXOK:
         MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
         MidString=(char *)LONG2FP(MidBuffer);
         MidString[20]=0;
         strcpy(DialNumber,MidString);
         break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}


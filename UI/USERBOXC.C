/*-------------------------------------------------------------------
* Name: userboxc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

int IsInvalidDigit(char *String)
{
  char *MidString=String;
  char PointSign=0;
  char SignSign=0;

  while (*MidString)
  {
    if (!(isdigit(*MidString)))
    {
      if ((*MidString)=='.')
      {
         if (PointSign)
            return(*MidString);
         else
            PointSign=1;
      }
      else
      if ((*MidString)=='-'||(*MidString)=='+')
      {
         if (SignSign)
            return(*MidString);
         else
            SignSign=1;
      }
      else
         return(*MidString);
    }
    MidString++;
  }
  return(0);
}

float ConvertToSystemMeter(float X)
{
  if (FileMETERIsINCH())
     return(X);
  else
     return(X*25.4);
}

float ConvertToUserMeter(float X)
{
  if (FileMETERIsINCH())
     return(X);
  else
     return(X/25.4);
}

#ifdef  UNUSED          // ByHance, 96,1.30
unsigned long BoxBorderOptionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  static unsigned char TmpBoxStatus[]=
  {
      1,                        /* Lockd */
      48,                       /* Round distribute */
      64                        /* Print Border */
  };
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (TmpTextBox.BoxStatus&TmpBoxStatus[Order])
            MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case DIALOGBOXOK:
         Order=RadioGetOrder(Window);
         if (MessageGo(Window,GETSTATUS,0l,0l))
            TmpTextBox.BoxStatus|=TmpBoxStatus[Order];
         else
            TmpTextBox.BoxStatus&=~TmpBoxStatus[Order];
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxBoxAlignProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);  /* Box alignment */
         if ((TmpTextBox.BoxStatus&0x6)==(Order<<1))
            MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case DIALOGBOXOK:
         Order=RadioGetOrder(Window);
         if (MessageGo(Window,GETSTATUS,0l,0l))
         {
            TmpTextBox.BoxStatus&=0xf9;
            TmpTextBox.BoxStatus|=(Order<<1);
         }
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxTextAlignProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);  /* Box alignment */
         if (TmpTextBox.TextAlign==Order)
            MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case DIALOGBOXOK:
         Order=RadioGetOrder(Window);
         if (MessageGo(Window,GETSTATUS,0l,0l))
            TmpTextBox.TextAlign=Order;
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}
#endif    //  UNUSED          // ByHance, 96,1.30

unsigned long BoxRotateAngleProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           itoa(TmpTextBox.RotateAngle,MidString,10);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;
           int  tmp;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDROTATEANGLE),1,
                         WindowGetFather(Window));
              return(FALSE);
           }

           tmp=atoi(MidString);
           while(tmp>=360) tmp-=360;
           while(tmp<0) tmp+=360;
           TmpTextBox.RotateAngle=tmp;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxRotateAxisXProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //gcvt(ConvertToSystemMeter((float)TmpTextBox.RotateAxisX/(float)(SCALEMETER)),
             //   8,MidString);
           sprintf(MidString,"%.1f",
              ConvertToSystemMeter((float)TmpTextBox.RotateAxisX/SCALEMETER) );
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;
           float tmp;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDROTATEAXISX),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           tmp=ConvertToUserMeter((float)SCALEMETER*atof(MidString));
           if(tmp>0) tmp+=0.5; else tmp-=0.5;
           TmpTextBox.RotateAxisX=tmp;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxRotateAxisYProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //gcvt(ConvertToSystemMeter((float)TmpTextBox.RotateAxisY/(float)(SCALEMETER)),
             //   8,MidString);
           sprintf(MidString,"%.1f",
              ConvertToSystemMeter((float)TmpTextBox.RotateAxisY/SCALEMETER) );
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;
           float tmp;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDROTATEAXISY),1,
                         WindowGetFather(Window));
              return(FALSE);
           }

           tmp=ConvertToUserMeter((float)SCALEMETER*atof(MidString));
           if(tmp>0) tmp+=0.5; else tmp-=0.5;
           TmpTextBox.RotateAxisY=tmp;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxBoxLeftProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //gcvt(ConvertToSystemMeter((float)TmpTextBox.BoxLeft/(float)(SCALEMETER)),
             //   5,MidString);
           sprintf(MidString,"%.1f",
              ConvertToSystemMeter((float)TmpTextBox.BoxLeft/SCALEMETER) );
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;
           float tmp;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXLEFT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           tmp=ConvertToUserMeter((float)SCALEMETER*atof(MidString));
           if(tmp>0) tmp+=0.5; else tmp-=0.5;
           TmpTextBox.BoxLeft=tmp;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxBoxTopProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //gcvt(ConvertToSystemMeter((float)TmpTextBox.BoxTop/(float)(SCALEMETER)),
             //   5,MidString);
           sprintf(MidString,"%.1f",
              ConvertToSystemMeter((float)TmpTextBox.BoxTop/SCALEMETER) );
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;
           float tmp;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
                                                                                                            MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXTOP),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           tmp=ConvertToUserMeter((float)SCALEMETER*atof(MidString));
           if(tmp>0) tmp+=0.5; else tmp-=0.5;
           TmpTextBox.BoxTop=tmp;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxWidthProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //gcvt(ConvertToSystemMeter((float)TmpTextBox.BoxWidth/(float)(SCALEMETER)),
            //    5,MidString);
           sprintf(MidString,"%.1f",
              ConvertToSystemMeter((float)TmpTextBox.BoxWidth/SCALEMETER) );
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           TmpTextBox.BoxWidth=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           if (TmpTextBox.BoxWidth<=0)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXWIDTH),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxHeightProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //gcvt(ConvertToSystemMeter((float)TmpTextBox.BoxHeight/(float)(SCALEMETER)),5,MidString);
           sprintf(MidString,"%.1f",
              ConvertToSystemMeter((float)TmpTextBox.BoxHeight/SCALEMETER) );
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           TmpTextBox.BoxHeight=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           if (TmpTextBox.BoxHeight<=0)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXHEIGHT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

#ifdef NOTUSED
unsigned long BoxTextLeftDistantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           gcvt(ConvertToSystemMeter((float)TmpTextBox.TextDistantLeft/(float)(SCALEMETER)),
                5,MidString);
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXTEXTLEFTDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpTextBox.TextDistantLeft=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxTextTopDistantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           gcvt(ConvertToSystemMeter((float)TmpTextBox.TextDistantTop/(float)(SCALEMETER)),
                5,MidString);
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXTEXTTOPDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpTextBox.TextDistantTop=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxTextRightDistantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           gcvt(ConvertToSystemMeter((float)TmpTextBox.TextDistantRight/(float)(SCALEMETER)),
                5,MidString);
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXTEXTRIGHTDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpTextBox.TextDistantRight=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxTextBottomDistantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           gcvt(ConvertToSystemMeter((float)TmpTextBox.TextDistantBottom/(float)(SCALEMETER)),
                5,MidString);
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXTEXTBOTTOMDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpTextBox.TextDistantBottom=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxRoundLeftDistantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           gcvt(ConvertToSystemMeter((float)TmpTextBox.RoundDistantLeft/(float)(SCALEMETER)),
                5,MidString);
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXROUNDLEFTDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpTextBox.RoundDistantLeft=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxRoundTopDistantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           gcvt(ConvertToSystemMeter((float)TmpTextBox.RoundDistantTop/(float)(SCALEMETER)),
                5,MidString);
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXROUNDTOPDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpTextBox.RoundDistantTop=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxRoundRightDistantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           gcvt(ConvertToSystemMeter((float)TmpTextBox.RoundDistantRight/(float)(SCALEMETER)),
                5,MidString);
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXROUNDRIGHTDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpTextBox.RoundDistantRight=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxRoundBottomDistantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           gcvt(ConvertToSystemMeter((float)TmpTextBox.RoundDistantBottom/(float)(SCALEMETER)),
                5,MidString);
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXROUNDBOTTOMDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpTextBox.RoundDistantBottom=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}
#endif // NOTUSED

unsigned long BoxBoxColumnProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           itoa(TmpTextBox.BoxColumn,MidString,10);
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
                         GetInformString(INVALIDCOLUMN),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpTextBox.BoxColumn=atoi(MidString);
           if(TmpTextBox.BoxColumn<0 || TmpTextBox.BoxColumn>10)
              goto lbl_err;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxColumnDistantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //gcvt(ConvertToSystemMeter((float)TmpTextBox.ColumnDistant/(float)(SCALEMETER)),5,MidString);
           sprintf(MidString,"%.1f",
              ConvertToSystemMeter((float)TmpTextBox.ColumnDistant/SCALEMETER) );
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
                         GetInformString(INVALIDCOLUMNDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpTextBox.ColumnDistant=atof(MidString);
           if(TmpTextBox.ColumnDistant<0 || TmpTextBox.ColumnDistant>100)
                goto lbl_err;

           TmpTextBox.ColumnDistant=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PictureBoxPictureOringleXProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //gcvt(ConvertToSystemMeter((float)TmpImageBox.ImageOriginX/(float)(SCALEMETER)),
             //   5,MidString);
           sprintf(MidString,"%.1f",
              ConvertToSystemMeter((float)TmpImageBox.ImageOriginX/SCALEMETER) );
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;
           float tmp;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXTEXTBOTTOMDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           tmp=ConvertToUserMeter((float)SCALEMETER*atof(MidString));
           if(tmp>0) tmp+=0.5; else tmp-=0.5;
           TmpImageBox.ImageOriginX=tmp;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PictureBoxPictureOringleYProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //gcvt(ConvertToSystemMeter((float)TmpImageBox.ImageOriginY/(float)(SCALEMETER)),
             //   5,MidString);
           sprintf(MidString,"%.1f",
              ConvertToSystemMeter((float)TmpImageBox.ImageOriginY/SCALEMETER) );
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXTEXTBOTTOMDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpImageBox.ImageOriginY=ConvertToUserMeter((float)SCALEMETER*atof(MidString));
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PictureBoxPictureImageScaleXProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //gcvt(TmpImageBox.ImageScaleX,5,MidString);
           sprintf(MidString,"%.2f", TmpImageBox.ImageScaleX);
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXTEXTBOTTOMDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpImageBox.ImageScaleX=atof(MidString);
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PictureBoxPictureImageScaleYProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           //gcvt(TmpImageBox.ImageScaleY,5,MidString);
           sprintf(MidString,"%.2f", TmpImageBox.ImageScaleY);
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXTEXTBOTTOMDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpImageBox.ImageScaleY=atof(MidString);
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PictureBoxPictureRotateAngleProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           itoa(TmpImageBox.ImageRotateAngle,MidString,10);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;
           int  tmp;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDROTATEANGLE),1,
                         WindowGetFather(Window));
              return(FALSE);
           }

           tmp=atoi(MidString);
           while(tmp>=360) tmp-=360;
           while(tmp<0) tmp+=360;
           TmpImageBox.ImageRotateAngle=tmp;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PictureBoxPictureSkewAngleProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           itoa(TmpImageBox.ImageSkewAngle,MidString,10);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;
           int  tmp;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           if (IsInvalidDigit(MidString))
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDROTATEANGLE),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           tmp=atoi(MidString);
           while(tmp>=360) tmp-=360;
           while(tmp<0) tmp+=360;
           TmpImageBox.ImageSkewAngle=tmp;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

#ifdef  NOTUSED
unsigned long PictureBoxPictureDistantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           gcvt(ConvertToSystemMeter((float)TmpImageBox.ImageDistant/(float)(SCALEMETER)),
                5,MidString);
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
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDBOXTEXTBOTTOMDISTANT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           TmpImageBox.ImageDistant=ConvertToUserMeter((float)SCALEMETER*atof(MidString))+0.5;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}
#endif // NOTUSED

unsigned long BoxPrintableProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         if ((TmpTextBox.BoxStatus&2)!=0)               // if disable, set it
            MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case DIALOGBOXOK:
         TmpTextBox.BoxStatus&=~2;              // clear this flag
         if (MessageGo(Window,GETSTATUS,0l,0l)) // if disable, set it
            TmpTextBox.BoxStatus|=2;
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long BoxEditablePro(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         if ((TmpTextBox.BoxAttr&2)!=0)               // zjh for module
            MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case DIALOGBOXOK:
         TmpTextBox.BoxAttr&=0xfffd;              // clear this flag
         if (MessageGo(Window,GETSTATUS,0l,0l)) // if disable, set it
            TmpTextBox.BoxAttr|=2;
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

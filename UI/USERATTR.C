/*-------------------------------------------------------------------
* Name: userattr.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"


static int tmp_cfont_option=0;  // ST, KT, HT, FT
static int tmp_efont_option=0;
//////////////////////////////By zjh 12.5//////////////////////////
unsigned long PrintLeftRightProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char MidString[20];

  switch (Message)
  {
    case WINDOWINIT:
    case KEYDOWN:
    case KEYSTRING:
         if(Message==WINDOWINIT)
         {
           sprintf(MidString,"%.3f",PXScale);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
         }
         else

           SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2);
         break;
    case DIALOGBOXOK:
         {
           char *str;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           str=LONG2FP(MidBuffer);
           if (atof(str)>10.0f||atof(str)<0.05f)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         //GetInformString(INVALIDCHARSIZE),
                         "  放缩比例必须大于0.05且小于10.0 !",
                         1,
                         Window);
              return(FALSE);
           }
           PXScale=atof(str);
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}
unsigned long PrintTopBottomProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char MidString[20];

  switch (Message)
  {
    case WINDOWINIT:
    case KEYDOWN:
    case KEYSTRING:
         if(Message==WINDOWINIT)
         {
           sprintf(MidString,"%.3f",PYScale);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
         }
         else

           SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2);
         break;
    case DIALOGBOXOK:
         {
           char *str;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           str=LONG2FP(MidBuffer);
           if (atof(str)>10.0f||atof(str)<0.05f)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         //GetInformString(INVALIDCHARSIZE),
                         "  放缩比例必须大于0.05且小于10.0 !",
                         1,
                         Window);
              return(FALSE);
           }
           PYScale=atof(str);
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}
unsigned long TextDistantTopProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char MidString[20];

  switch (Message)
  {
    case WINDOWINIT:
    case KEYDOWN:
    case KEYSTRING:
         if(Message==WINDOWINIT)
         {
           sprintf(MidString,"%.3f",TmpFormBox.TextDistantTop*25.4/SCALEMETER);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
         }
         else

           SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2);
         break;
    case DIALOGBOXOK:
         {
           char *str;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           str=LONG2FP(MidBuffer);
           if (atof(str)>4.0f||atof(str)<0.0f)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         //GetInformString(INVALIDCHARSIZE),
                         "    边距必须大于0.0且小于4.0 !",
                         1,
                         Window);
              return(FALSE);
           }
           TmpFormBox.TextDistantTop=atof(str)*SCALEMETER/25.4;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}
unsigned long TextDistantBottomProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char MidString[20];

  switch (Message)
  {
    case WINDOWINIT:
    case KEYDOWN:
    case KEYSTRING:
         if(Message==WINDOWINIT)
         {
           sprintf(MidString,"%.3f",TmpFormBox.TextDistantBottom*25.4/SCALEMETER);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
         }
         else

           SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2);
         break;
    case DIALOGBOXOK:
         {
           char *str;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           str=LONG2FP(MidBuffer);
           if (atof(str)>4.0f||atof(str)<0.0f)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         //GetInformString(INVALIDCHARSIZE),
                         "    边距必须大于0.0且小于4.0 !",
                         1,
                         Window);
              return(FALSE);
           }
           TmpFormBox.TextDistantBottom=atof(str)*SCALEMETER/25.4;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}
unsigned long TextDistantLeftProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char MidString[20];

  switch (Message)
  {
    case WINDOWINIT:
    case KEYDOWN:
    case KEYSTRING:
         if(Message==WINDOWINIT)
         {
           sprintf(MidString,"%.3f",TmpFormBox.TextDistantLeft*25.4/SCALEMETER);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
         }
         else

           SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2);
         break;
    case DIALOGBOXOK:
         {
           char *str;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           str=LONG2FP(MidBuffer);
           if (atof(str)>4.0f||atof(str)<0.0f)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         //GetInformString(INVALIDCHARSIZE),
                         "    边距必须大于0.0且小于4.0 !",
                         1,
                         Window);
              return(FALSE);
           }
           TmpFormBox.TextDistantLeft=atof(str)*SCALEMETER/25.4;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long TextDistantRightProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char MidString[20];

  switch (Message)
  {
    case WINDOWINIT:
    case KEYDOWN:
    case KEYSTRING:
         if(Message==WINDOWINIT)
         {
           sprintf(MidString,"%.3f",TmpFormBox.TextDistantRight*25.4/SCALEMETER);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
         }
         else

           SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2);
         break;
    case DIALOGBOXOK:
         {
           char *str;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           str=LONG2FP(MidBuffer);
           if (atof(str)>4.0f||atof(str)<0.0f)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         //GetInformString(INVALIDCHARSIZE),
                         "    边距必须大于0.0且小于4.0 !",
                         1,
                         Window);
              return(FALSE);
           }
           TmpFormBox.TextDistantRight=atof(str)*SCALEMETER/25.4;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

//////////////////////////////By zjh 10.29/////////////////////////
#define FACT MAXCHARFACT
int MaxRL(HBOX hBox,int *w,int *h);
int CompressCHARSIZE(int yy)
{
    int max=FACT*512;
    yy=yy%max;
    if (yy>=4096) return yy/FACT+0x600;
    if (yy>=1024) return yy/8+0x400;
    return yy;
}

int UncompressCHARSIZE(int yy)
{
    int i;
    i=((yy&0x600)>>9);
    switch (i)
    {
        case 0:
        case 1:
            return (yy&0x3ff);
        case 2:
            return (yy&0x1ff)*8;
        case 3:
            return (yy&0x1ff)*FACT;
    }

    return (yy&0x3ff);     //cannot arrive
}

USHORT make_attr(int II,int AA)
{
    if (II==CHARSIZE||II==CHARHSIZE) AA=CompressCHARSIZE(AA);
    return ((AA)|((II)<<ATTRIBUTEBITS));
}

int get_attr(USHORT GG)
{
    if (GetPreCode(GG)==CHARSIZE||GetPreCode(GG)==CHARHSIZE)
      return  UncompressCHARSIZE((GG)&ATTRIBUTEPATTERN);
    return ((GG)&ATTRIBUTEPATTERN);
}

////////////////////////////////////end zjh ////////////////////

unsigned long CFontOptionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  HWND MidWin;

  switch (Message)
  {
    case WINDOWINIT:
         if(tmp_cfont_option==RadioGetOrder(Window))
             MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case SELECTSELECTED:
         tmp_cfont_option=RadioGetOrder(Window);
         MidWin=WindowGetFather(Window);        // frame window
         MidWin=WindowGetChild(MidWin);
         while(MidWin)
         {
            if (WindowGetProcedure(MidWin)==CharCFontProcedure)
                break;
            MidWin=WindowGetNext(MidWin);
         }
         if(MidWin)
         {
            char MidString[2];
            MidString[0]='1' + tmp_cfont_option;
            MidString[1]=0;
            MessageGo(MidWin,SETLINEBUFFER,FP2LONG(MidString),0l);
            MessageInsert(MidWin,REDRAWMESSAGE,0l,GetEditorWidth(WindowGetUserData(MidWin)));
         }
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long EFontOptionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  HWND MidWin;

  switch (Message)
  {
    case WINDOWINIT:
         if(tmp_efont_option==RadioGetOrder(Window))
             MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case SELECTSELECTED:
         tmp_efont_option=RadioGetOrder(Window);
         MidWin=WindowGetFather(Window);        // frame window
         MidWin=WindowGetChild(MidWin);
         while(MidWin)
         {
            if (WindowGetProcedure(MidWin)==CharEFontProcedure)
                break;
            MidWin=WindowGetNext(MidWin);
         }
         if(MidWin)
         {
            char MidString[2];
            MidString[0]='1' + tmp_efont_option;
            MidString[1]=0;
            MessageGo(MidWin,SETLINEBUFFER,FP2LONG(MidString),0l);
            MessageInsert(MidWin,REDRAWMESSAGE,0l,GetEditorWidth(WindowGetUserData(MidWin)));
         }
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long CharCFontProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  HWND MidWin;

  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           TmpAttribute=TextSearchCFont(GlobalBoxHeadHandle,
                        GlobalTextBlockStart,&AttributePosition);

           if(TmpAttribute!=tmp_cfont_option)   // ByHance, 96,4.5
           {
               MidWin=WindowGetFather(Window);        // frame window
               MidWin=WindowGetChild(MidWin);
               while(MidWin)
               {
                  if( WindowGetProcedure(MidWin)!=CFontOptionProcedure )
                     goto lbl_try_next;

                  if(TmpAttribute<4)
                  {
                      if(RadioGetOrder(Window)==TmpAttribute)
                      {
                         MessageGo(MidWin,SETSTATUS,1l,0l);
                         break;
                      }
                  }
                  else
                  {
                      if(RadioGetOrder(Window)==tmp_cfont_option)
                      {
                         MessageGo(MidWin,SETSTATUS,0l,0l);
                         break;
                      }
                  }

                lbl_try_next:
                  MidWin=WindowGetNext(MidWin);
               } /*- while -*/

               tmp_cfont_option=TmpAttribute;
           }

           TmpAttribute++;
            //gcvt(TmpAttribute,6,MidString);
           itoa(TmpAttribute,MidString,10);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           TmpAttribute=atoi(MidString);
           if (TmpAttribute<=0||TmpAttribute>=MAXCFONTNUMBER)
           //if (TmpAttribute<=0||TmpAttribute>=4)        // MAX Cfont=3
           {
              MessageBox(GetTitleString(ERRORINFORM),
                            // GetInformString(INVALIDCHARFONT),
                         "1.x版只提供300种中文字体,\n请修正!",
                         1,Window);
              return(FALSE);
           }
           --TmpAttribute;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long CharEFontProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  HWND MidWin;

  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           TmpAttribute2=TextSearchEFont(GlobalBoxHeadHandle,
                        GlobalTextBlockStart,&AttributePosition);
           TmpAttribute2-=1024;

           if(TmpAttribute2!=tmp_efont_option)   // ByHance, 96,4.5
           {
               MidWin=WindowGetFather(Window);        // frame window
               MidWin=WindowGetChild(MidWin);
               while(MidWin)
               {
                  if( WindowGetProcedure(MidWin)!=EFontOptionProcedure )
                     goto lbl_try_next;

                  if(TmpAttribute2<4)
                  {
                      if(RadioGetOrder(Window)==TmpAttribute2)
                      {
                         MessageGo(MidWin,SETSTATUS,1l,0l);
                         break;
                      }
                  }
                  else
                  {
                      if(RadioGetOrder(Window)==tmp_efont_option)
                      {
                         MessageGo(MidWin,SETSTATUS,0l,0l);
                         break;
                      }
                  }

                lbl_try_next:
                  MidWin=WindowGetNext(MidWin);
               } /*- while -*/

               tmp_efont_option=TmpAttribute2;
           }

           TmpAttribute2++;
           //gcvt(TmpAttribute2,6,MidString);
           itoa(TmpAttribute2,MidString,10);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=(char *)LONG2FP(MidBuffer);
           TmpAttribute2=atoi(MidString);
           if (TmpAttribute2<=0||TmpAttribute2>=MAXFONTNUMBER)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARFONT),1,
                         Window);
              return(FALSE);
           }

           --TmpAttribute2;
           TmpAttribute2+=1024;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

//static int tmp_charsize_unit=0;         // 0=use MM, 1=use Pound
static int tmp_charsize_option=1;       // 1:: CharHeight==CharWidth

unsigned long CharSizeOptionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETSTATUS,tmp_charsize_option,0l);
         break;
    case SELECTSELECTED:
         tmp_charsize_option=MessageGo(Window,GETSTATUS,0l,0l);
         if(tmp_charsize_option)      // Width==Height
         {
              HWND MidWin,MidWin2;

              TmpAttribute2=TmpAttribute;
            //---- set two single_editor's string to be EQU --
              MidWin=WindowGetFather(Window);
              MidWin=WindowGetChild(MidWin);
              while(MidWin)
              {
                 if(WindowGetProcedure(MidWin)==CharSizeProcedure)
                     break;
                 MidWin=WindowGetNext(MidWin);
              }
              if(!MidWin)
                 break;

              MidWin2=WindowGetFather(Window);
              MidWin2=WindowGetChild(MidWin2);
              while(MidWin2)
              {
                 if(WindowGetProcedure(MidWin2)==CharHSizeProcedure)
                     break;
                 MidWin2=WindowGetNext(MidWin2);
              }
              if(MidWin2)
              {
                 long MidBuffer;

                 MidBuffer=MessageGo(MidWin,GETLINEBUFFER,0l,0l);
                 MessageGo(MidWin2,SETLINEBUFFER,MidBuffer,0l);
                 MessageGo(MidWin2,WMPAINT,0l,GetEditorWidth(WindowGetUserData(MidWin2)));
              }
         }
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

#ifdef NOT_USED
unsigned long CharSizeUnitProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  //HWND MidWin;

  switch (Message)
  {
    case WINDOWINIT:
         if(tmp_charsize_unit==RadioGetOrder(Window))
             MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case SELECTSELECTED:
         tmp_charsize_unit=RadioGetOrder(Window);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}
#endif

unsigned long CharSizeProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char MidString[20];
  int  Position;

  switch (Message)
  {
    case WINDOWINIT:
    case KEYDOWN:
    case KEYSTRING:
         if(Message==WINDOWINIT)
         {
           TmpAttribute=TextSearchAttribute(GlobalBoxHeadHandle,
                        GlobalTextBlockStart,CHARSIZE,&Position);
           //TmpAttribute=UncompressCHARSIZE(TmpAttribute);    //By zjh 10.29
           //gcvt(ConvertToSystemMeter(TmpAttribute/SCALEMETER),6,MidString);
           sprintf(MidString,"%.2f",ConvertToSystemMeter(TmpAttribute/SCALEMETER));
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
         }
         else
           SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2);

         if(tmp_charsize_option)      // Width==Height
         {
           HWND MidWin;

           TmpAttribute2=TmpAttribute;
           MidWin=WindowGetFather(Window);
           MidWin=WindowGetChild(MidWin);
           while(MidWin)
           {
              if(WindowGetProcedure(MidWin)==CharHSizeProcedure)
                  break;
              MidWin=WindowGetNext(MidWin);
           }
           if(MidWin)
           {
              extern short CursorPosX;
              Position=CursorPosX; // setlinebuffer will change it, so save it
              MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
              MessageGo(MidWin,SETLINEBUFFER,MidBuffer,0l);
              CursorPosX=Position;

              if(Message!=WINDOWINIT)
                 MessageGo(MidWin,WMPAINT,0l,GetEditorWidth(WindowGetUserData(MidWin)));
           }
         }
         break;
    case DIALOGBOXOK:
         {
           char *str;
           int w,h;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           str=LONG2FP(MidBuffer);
           TmpAttribute=ConvertToUserMeter(atof(str))*SCALEMETER;

           MaxRL(GlobalBoxHeadHandle,&w,&h);    //By zjh 10.29
           //if (TmpAttribute<MINCHARSIZE||TmpAttribute>=MAXCHARSIZE)
           //By zjh 10.29
           if (TmpAttribute<MINCHARSIZE||TmpAttribute>=MAXCHARSIZE||TmpAttribute>=w)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARSIZE),1,
                         Window);
              return(FALSE);
           }
           //TmpAttribute=CompressCHARSIZE(TmpAttribute);    //By zjh 10.29
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long RowGapProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char MidString[20];

  switch (Message)
  {
    case WINDOWINIT:
    case KEYDOWN:
    case KEYSTRING:
         if(Message==WINDOWINIT)
         {
           sprintf(MidString,"1.5");
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
         }
         else

           SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2);
         break;
    case DIALOGBOXOK:
         {
           char *str;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           str=LONG2FP(MidBuffer);
           if (atof(str)>7.9f||atof(str)<1.0f)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         //GetInformString(INVALIDCHARSIZE),
                         "行、列距必须大于1.0且小于7.9 !",
                         1,
                         Window);
              return(FALSE);
           }
           TmpAttribute=(atof(str)-1.0)*10+10;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}



unsigned long UpDownProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char MidString[20];
  // int  Position;

  switch (Message)
  {
    case WINDOWINIT:
    case KEYDOWN:
    case KEYSTRING:
         if(Message==WINDOWINIT)
         {
           sprintf(MidString,"0.0");
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
         }
         else

           SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2);
         break;
    case DIALOGBOXOK:
         {
           char *str;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           str=LONG2FP(MidBuffer);
           if (atof(str)>7.9f||atof(str)<-7.90f)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         //GetInformString(INVALIDCHARSIZE),
                         "升降必须大于-7.9且小于7.9  !",
                         1,
                         Window);
              return(FALSE);
           }
           TmpAttribute=atof(str)*10;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long CharHSizeProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           if(tmp_charsize_option)      // Width==Height
               break;

           TmpAttribute2=TextSearchAttribute(GlobalBoxHeadHandle,
                        GlobalTextBlockStart,CHARHSIZE,&AttributePosition);
           //TmpAttribute2=UncompressCHARSIZE(TmpAttribute2);    //By zjh 10.29
           gcvt(ConvertToSystemMeter(TmpAttribute2/SCALEMETER),6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;
           int w,h;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpAttribute2=ConvertToUserMeter(atof(MidString)*SCALEMETER);

           MaxRL(GlobalBoxHeadHandle,&w,&h);    //By zjh 10.29

           //if (TmpAttribute2<MINCHARHSIZE||TmpAttribute2>=MAXCHARHSIZE)
           //By zjh 10.29
           if (TmpAttribute2<MINCHARHSIZE||TmpAttribute2>=MAXCHARHSIZE||TmpAttribute2>=h)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARHSIZE),1,
                         Window);
              return(FALSE);
           }
           //TmpAttribute2=CompressCHARSIZE(TmpAttribute2);    //By zjh 10.29
           break;
         }
    case GETFOCUS:
    case MOUSEMOVE:
         if(tmp_charsize_option)      // Width==Height
            return(FALSE);
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static char *TableLineColNumErrStr="表格行列数应在1至59之间,请修正";
unsigned long TableLineProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           TmpFormBox.numLines=1;
           // gcvt(TmpFormBox.numLines,4,MidString);
           itoa(TmpFormBox.numLines,MidString,10);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpFormBox.numLines=atoi(MidString);
           if (TmpFormBox.numLines<=0||TmpFormBox.numLines>=MAXLINENUMBER)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         TableLineColNumErrStr,
                         1,Window);
              return(FALSE);
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long TableLineStyleProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  char str[20];

  switch (Message)
  {
    case WINDOWINIT:
         {

           TmpFormBox.numLines=1;
           sprintf(str,"%d",TmpFormBox.numLines);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpFormBox.numLines=atof(MidString);
           if (TmpFormBox.numLines<0||TmpFormBox.numLines>=MAXLINENUMBER)
           {
              TmpFormBox.numLines = 1;
              sprintf(str,"%d",TmpFormBox.numLines);
              MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
              MessageGo(Window,WMPAINT,0l,GetEditorWidth(WindowGetUserData(Window)));
              return(FALSE);
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long TableColumnProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];

           TmpFormBox.numCols=1;
           //gcvt(TmpFormBox.numCols,4,MidString);
           itoa(TmpFormBox.numCols,MidString,10);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpFormBox.numCols=atoi(MidString);
           if (TmpFormBox.numCols<=0||TmpFormBox.numCols>=MAXCLOUMNNUMBER)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         TableLineColNumErrStr,
                         1,Window);
              return(FALSE);
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long FindOptionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);  /* Find option */
         if (GlobalFindStruct.FindOption&(1<<Order))
            MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case DIALOGBOXOK:
         Order=RadioGetOrder(Window);
         if (MessageGo(Window,GETSTATUS,0l,0l))
            GlobalFindStruct.FindOption|=(1<<Order);
         else
            GlobalFindStruct.FindOption&=~(1<<Order);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long FindStringProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char *MidString;
  unsigned char *p,str[300];
  int  i;

  switch (Message)
  {
    case WINDOWINIT:
         p=(char *)GlobalFindStruct.FindString;
         i=0;
         while(*p)
         {
            if(*p>=0xa0) { str[i+1]=*p; str[i]=*(p+1); i+=2; p+=2; }
            else str[i++]=*p++;
         }
         str[i]=0;
         MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0L);
         break;
    case DIALOGBOXOK:
         p=(char *)GlobalFindStruct.FindString;
         MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
         MidString=(char *)LONG2FP(MidBuffer);
         // strcpy(GlobalFindStruct.FindString,MidString);
         i=0;
         while(MidString[i])
         {
            if(MidString[i]>=0xa0)
            {
              *(p+1)=MidString[i];  *p=MidString[i+1];
              i+=2; p+=2;
            }
            else
              *p++=MidString[i++];
         }
         *p=0;
         break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long ReplaceStringProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  long MidBuffer;
  char *MidString;
  unsigned char *p,str[300];
  int  i;

  switch (Message)
  {
    case WINDOWINIT:
      //MessageGo(Window,SETLINEBUFFER,FP2LONG(GlobalFindStruct.ReplaceToString),0l);
         p=(char *)GlobalFindStruct.ReplaceToString;
         i=0;
         while(*p)
         {
            if(*p>=0xa0) { str[i+1]=*p; str[i]=*(p+1); i+=2; p+=2; }
            else str[i++]=*p++;
         }
         str[i]=0;
         MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0L);
         break;
    case DIALOGBOXOK:
         p=(char *)GlobalFindStruct.ReplaceToString;
         MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
         MidString=(char *)LONG2FP(MidBuffer);
        //strcpy(GlobalFindStruct.ReplaceToString,MidString);
         i=0;
         while(MidString[i])
         {
            if(MidString[i]>=0xa0)
            {
              *(p+1)=MidString[i];  *p=MidString[i+1];
              i+=2; p+=2;
            }
            else
              *p++=MidString[i++];
         }
         *p=0;
         break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

#ifdef  UNUSED          // ByHance, 96,1.30
unsigned long CharColorProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           TmpAttribute3=TextSearchAttribute(GlobalBoxHeadHandle,
                        GlobalTextBlockStart,CHARCOLOR,&AttributePosition)&0xf;
           gcvt(TmpAttribute3,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpAttribute3=atof(MidString);
           if (TmpAttribute3<0||TmpAttribute3>=MAXCHARCOLOR)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARCOLOR),1,
                         Window);
              return(FALSE);
           }
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long FontUseProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           int i;
           char TmpString[20];

           MessageGo(Window,LISTSETITEMLENGTH,3l,0l);
           MessageGo(Window,LISTSETITEMHEIGHT,16l,0l);
           for (i=0;i<MAXFONTNUMBER;i++)
           {
               if (TmpFontUse[i])
               {
                  sprintf(TmpString,"%3d",i);
                  MessageGo(Window,SETLINEBUFFER,FP2LONG(TmpString),0l);
               }
           }
           break;
         }
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long PictureUseProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           int i;

           MessageGo(Window,LISTSETITEMLENGTH,40l,0l);
           MessageGo(Window,LISTSETITEMHEIGHT,16l,0l);
           for (i=0;i<TotalFile;i++)
               MessageGo(Window,SETLINEBUFFER,FP2LONG(TmpFileName[i]),0l);
           break;
         }
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long LineHeadProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           gcvt(TmpAttribute/SCALEMETER,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpAttribute=atof(MidString)*SCALEMETER;
           if (TmpAttribute<MINCHARSIZE||TmpAttribute>=MAXCHARSIZE)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARSIZE),1,
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

unsigned long LineTailProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           gcvt(TmpAttribute2/SCALEMETER,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpAttribute2=atof(MidString)*SCALEMETER;
           if (TmpAttribute2<MINCHARHSIZE||TmpAttribute2>=MAXCHARHSIZE)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARHSIZE),1,
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

unsigned long LineStyleProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           gcvt(TmpAttribute3/SCALEMETER,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpAttribute3=atof(MidString)*SCALEMETER;
           if (TmpAttribute3<MINCHARHSIZE||TmpAttribute3>=MAXCHARHSIZE)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARHSIZE),1,
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

unsigned long UserLineWidthProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           gcvt(TmpAttribute/SCALEMETER,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpAttribute=atof(MidString)*SCALEMETER;
           if (TmpAttribute<MINCHARSIZE||TmpAttribute>=MAXCHARSIZE)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARSIZE),1,
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

unsigned long CharSlantProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           TmpAttribute=TextSearchAttribute(GlobalBoxHeadHandle,
                        GlobalTextBlockStart,CHARSLANT,&AttributePosition);
           if (TmpAttribute>MAXCHARSLANTANGLE)
              TmpAttribute-=360;
           gcvt(TmpAttribute,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpAttribute=atof(MidString);
           if (TmpAttribute<MINCHARSLANTANGLE||TmpAttribute>=MAXCHARSLANTANGLE)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARSLANTANGLE),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           if (TmpAttribute<0)
              TmpAttribute+=360;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long CharShadowProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           TmpAttribute4=(TextSearchAttribute(GlobalBoxHeadHandle,
                        GlobalTextBlockStart,CHARCOLOR,&AttributePosition)&0xf0)>>4;
           gcvt(TmpAttribute4,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpAttribute4=atof(MidString);
           if (TmpAttribute4<0||TmpAttribute4>=MAXCHARCOLOR)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARCOLOR),1,
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

unsigned long ImageColorProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           gcvt(TmpAttribute3,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpAttribute3=atof(MidString);
           if (TmpAttribute3<0||TmpAttribute3>=MAXCHARCOLOR)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARCOLOR),1,
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

unsigned long ImageShadowProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           int AttributePosition;

           gcvt(TmpAttribute4,6,MidString);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(MidString),0l);
           break;
         }
    case DIALOGBOXOK:
         {
           long MidBuffer;
           char *MidString;

           MidBuffer=MessageGo(Window,GETLINEBUFFER,0l,0l);
           MidString=LONG2FP(MidBuffer);
           TmpAttribute4=atof(MidString);
           if (TmpAttribute4<0||TmpAttribute4>=MAXCHARCOLOR)
           {
              MessageBox(GetTitleString(ERRORINFORM),
                         GetInformString(INVALIDCHARCOLOR),1,
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
#endif      // UNUSED

static int tmp_exportflag = 0;
int GetExportOption() { return tmp_exportflag; }
unsigned long ExportOptionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (tmp_exportflag != Order) break;
         MessageGo(Window,SETSTATUS,1l,0l);
         MessageInsert(Window,SELECTSELECTED,1l,0l);
         break;
    case SELECTSELECTED:
         tmp_exportflag=RadioGetOrder(Window);
         break;
 /*-----------
    case DIALOGBOXOK:
         Order=RadioGetOrder(Window);
         if (MessageGo(Window,GETSTATUS,0l,0l))
           tmp_exportflag = Order;
         break;
   -----------*/
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

//----------------- Table Insert Line Or Col Dialog -----------------
unsigned long TableInsOptionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;
  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (tmp_insflag == Order)
             MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case SELECTSELECTED:
         tmp_insflag=RadioGetOrder(Window);
         break;
 /*-----------
    case DIALOGBOXOK:
         Order=RadioGetOrder(Window);
         if (MessageGo(Window,GETSTATUS,0l,0l))
           tmp_insflag = Order;
         break;
   -----------*/
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long TableInsProcedure(HWND Window,HMSG Message,long Param1,long Param2)
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
           if (pn<1) {
                  pn = 1;
                  sprintf(str,"%d",pn);
                  MessageGo(Window,SETLINEBUFFER,FP2LONG(str),0l);
                  MessageGo(Window,WMPAINT,0l,GetEditorWidth(WindowGetUserData(Window)));
                  return FALSE;
           }
           tmp_nline = pn-1;
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long TableStyleOptionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);  /* Find option */
         if (tmp_TableStyleOption==Order)
            MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case SELECTSELECTED:
         tmp_TableStyleOption=RadioGetOrder(Window);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long TableLineColOptionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;

  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);  /* Find option */
         if (tmp_TableLineColOption==Order)
            MessageGo(Window,SETSTATUS,1l,0l);
         break;
    case SELECTSELECTED:
         tmp_TableLineColOption=RadioGetOrder(Window);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long TableHeadOptionProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
 // int Order;
  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETSTATUS,tmp_TableHeadOption,0l);
         break;
    //case SELECTSELECTED:
    case DIALOGBOXOK:
         tmp_TableHeadOption=MessageGo(Window,GETSTATUS,0l,0l);
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

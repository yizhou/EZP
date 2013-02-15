/*-------------------------------------------------------------------
* Name: dbf.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#define INSERTALL 12999

///////////////////////////////For Table Calc///////////////////////////
static char exp_1[101]={""};
static char exp_2[101]={""};
static char exp_3[101]={""};
static char resu_0[20]={""};
static char start_0[20]={""};
static char end_0[20]={""};
static char start_1[20]={""};
static char end_1[20]={""};
static char format_0[20]={""};
static char prev_0[60]={""};
static char succ_0[60]={""};
static int Calc_Mode;

static int IsNum(char *str)
{
  while (*str) if (!((*str>='0'&&(*str)<='9')||(*str)==' ')) return 1;
               else str++;
  return 0;
}

static char *trim(char *str)
{
    int len,i;

       while (*str==' ') str++;
       len=strlen(str);
       for (i=len-1;i>=0;i--) if (str[i]!=' ') break;
       str[i+1]=0;
       len=strlen(str);
       for (i=len-1;i>=0;i--) if (str[i]=='_') str[i]=' ';
       return str;
}

static int get_format_str(char *sstr,char *tstr)
{
   int i,j=0;
   char *k;
   int n1,n2;
   static char DefaultFormat[]="%.2lf";

   while (*sstr==BLANK) sstr++;
   if (!(*sstr))
   {
       strcpy(tstr,DefaultFormat);
       return 0;
   }

   switch (*sstr)
   {
      case '.':
            i=atoi(sstr+1);
            if (i<0||i>20||IsNum(sstr+1))
                goto lbl_err;

            sprintf(tstr,"%%.%dlf",i);
            break;
      case 'E':
      case 'e':
            i=atoi(sstr+1);
            if (i<0||i>20||IsNum(sstr+1))
            {
                strcpy(tstr,"%.2le");
                return -1;
            }
            if (i||(i==0&&sstr[1]=='0'))
                sprintf(tstr,"%%.%dle",i);
            else
                strcpy(tstr,"%.2le");
            break;
      case 'c':
      case 'C':
            i=atoi(sstr+1);
            if (i<0||i>20||IsNum(sstr+1))
            {
                strcpy(tstr,"C%lf");
                return -1;
            }
            if (i||(i==0&&sstr[1]=='0'))
                sprintf(tstr,"C%%.%dlf",i);
            else
                strcpy(tstr,"C%.2lf");
            break;
      case 'i':
      case 'I':
            if (*(sstr+1)=='0')
            {
                j=1;
                sstr++;
            }

            i=atoi(sstr+1);
            if (i<0||i>20||IsNum(sstr+1))
            {
                strcpy(tstr,"%.0lf");
                return -1;
            }

            if (j)
                sprintf(tstr,"%%0%d.0lf",i);
            else
                sprintf(tstr,"%%%d.0lf",i);
            break;
      case '0':
            j=1;
            sstr++;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
            k=strchr(sstr,'.');
            if (k==NULL)
            {
                i=atoi(sstr);
                if (i<1||i>20||IsNum(sstr))
                {
                    strcpy(tstr,DefaultFormat);
                    return -1;
                }
                if (j)
                    sprintf(tstr,"%%0%dlf",i);
                else
                    sprintf(tstr,"%%%dlf",i);
                return 0;
            }
            *k++=0;
            if (IsNum(sstr)||IsNum(k))
                goto lbl_err;

            n1=atoi(sstr);
            n2=atoi(k);
            *(k-1)='.';
            if ((j==1&&n1==0&&n2==0)||n1>20||n2>20||n1<0||n2<0)
                goto lbl_err;

            if (j&&n1)
                sprintf(tstr,"%%0%d.%dlf",n1,n2);
            else
                sprintf(tstr,"%%%d.%dlf",n1,n2);
            break;
      default:
      lbl_err:
            strcpy(tstr,DefaultFormat);
            return -1;
   }
   return 0;
}

static void FormatNumText(char *s,int len1)
{
   int i,j,k,len,st;

   if (*s=='C')
   {
       len=min(strlen(s),len1);

       for (i=0;i<len;i++) s[i]=s[i+1];  //Get off the C ID
       for (i=0;i<len;i++)
       {
           if (s[i]==0) break;
           else
           if (s[i]>='0'&&s[i]<='9')
           {
              for (j=i;j<len;j++)
                  if (s[j]<'0'||s[j]>'9')
                  {
                      st=strlen(s);
                      while (j-i>3&&st<len1-1)
                      {
                         st++;
                         for (k=st;k>j-3;k--) s[k]=s[k-1];
                         s[j-3]=',';
                         j=j-3;
                      }
                      break;
                  }

              break;
           }
       } /*-- i --*/
   }

   s[len1-1]=0;
}

unsigned long format_0_Procedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(format_0),0l);
           break;
    case DIALOGBOXOK:
           strncpy(format_0,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)),15);
           format_0[15]=0;
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long prev_0_Procedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(prev_0),0l);
           break;
    case DIALOGBOXOK:
           strncpy(prev_0,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)),55);
           prev_0[55]=0;
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long succ_0_Procedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(succ_0),0l);
           break;
    case DIALOGBOXOK:
           strncpy(succ_0,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)),55);
           succ_0[55]=0;
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long TableCalcPro(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
   {
    case 11900:         //sum
                Calc_Mode=1;
                Message=DIALOGBOXOK;
                break;
    case 11901:         //adv
                Calc_Mode=2;
                Message=DIALOGBOXOK;
                break;
    case 11902:         //express
                Calc_Mode=3;
                Message=DIALOGBOXOK;
                break;
   }
  return(DialogDefaultProcedure(Window, Message, Param1, Param2));
}

unsigned long End_0_Procedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(end_0),0l);
           break;
    case DIALOGBOXOK:
           strncpy(end_0,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)),10);
           end_0[10]=0;
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long End_1_Procedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(end_1),0l);
           break;
    case DIALOGBOXOK:
           strncpy(end_1,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)),10);
           end_1[10]=0;
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long Start_0_Procedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(start_0),0l);
           break;
    case DIALOGBOXOK:
           strncpy(start_0,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)),10);
           start_0[10]=0;
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long Start_1_Procedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(start_1),0l);
           break;
    case DIALOGBOXOK:
           strncpy(start_1,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)),10);
           start_1[10]=0;
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long Resu_0_Procedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(resu_0),0l);
           break;
    case DIALOGBOXOK:
           strncpy(resu_0,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)),10);
           resu_0[10]=0;
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long Exp_1_Procedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(exp_1),0l);
           break;
    case DIALOGBOXOK:
           strncpy(exp_1,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)),100);
           exp_1[100]=0;
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long Exp_2_Procedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(exp_2),0l);
           break;
    case DIALOGBOXOK:
           strncpy(exp_2,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)),100);
           exp_2[100]=0;
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long Exp_3_Procedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(exp_3),0l);
           break;
    case DIALOGBOXOK:
           strncpy(exp_3,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)),100);
           exp_3[100]=0;
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}
////////////////////////////////////By zjh /////////////////////////////////////

unsigned long StartRecordProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           // TmpPage.MarginLeft=DEFAULTPAGEHDISTANT;
           itoa(StartRecord,MidString,10);
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
                         GetInformString(INVALIDPAGEMARGINLEFT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           StartRecord=atoi(MidString);
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long RecordNumberProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           // TmpPage.MarginLeft=DEFAULTPAGEHDISTANT;
           itoa(RecordNumber,MidString,10);
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
                         GetInformString(INVALIDPAGEMARGINLEFT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           RecordNumber=atoi(MidString);
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long StartCellRowProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           // TmpPage.MarginLeft=DEFAULTPAGEHDISTANT;
           itoa(StartCellRow,MidString,10);
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
                         GetInformString(INVALIDPAGEMARGINLEFT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           StartCellRow=atoi(MidString);
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long StartCellColProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char MidString[20];
           // TmpPage.MarginLeft=DEFAULTPAGEHDISTANT;
           itoa(StartCellCol,MidString,10);
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
                         GetInformString(INVALIDPAGEMARGINLEFT),1,
                         WindowGetFather(Window));
              return(FALSE);
           }
           StartCellCol=atoi(MidString);
           break;
         }
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}


static void InsertField(int num,int val)
{
  int i;
  if (Target_num>=99) return;
  if (num>Target_num) num=Target_num;
  if (num<0) num=0;
  for (i=Target_num;i>num;i--) TargetField[i]=TargetField[i-1];
  TargetField[num]=val;
  Target_num++;
}

int InsertAllField(int num)
{
    int i;
    if (Target_num==0) num=0;
    if (num>Target_num) num=Target_num;
    for (i=0;i<Source_num;i++)
    {
        if (num>99) num=99;
        InsertField(num++,i);
    }
    return num-1;
}

#define DeleteAllField()  Target_num=0

static void DeleteField(int num)
{
  int i;
  if (Target_num<=0) return;
  if (num>=Target_num)
  {
    Target_num--;
    return ;
  }
  if (num<0) num=0;
  for (i=num;i<Target_num;i++) TargetField[i]=TargetField[i+1];
  Target_num--;
}


static Dialogs TableCalcExpress[]=
{
#define xx  280
#define yy  91
  { GLOBALITEM, 26 , 0, 0, 575, 255-yy, 0,TableCalcPro,"横向统计" },

  { STATICTEXTITEM, 0, 16, 61-22, 76, 80-25, 0,NULL, "表达式" },
  { SINGLELINEEDITORITEM, 0, 78, 61-22, 277, 80-25, 0, Exp_1_Procedure, "" },
  { SINGLELINEEDITORITEM, 0, 16, 90-30, 277, 90-13, 0, Exp_2_Procedure, "" },
  { SINGLELINEEDITORITEM, 0, 16, 90-8, 277, 90+9, 0, Exp_3_Procedure, "" },

  { STATICTEXTITEM, 0, 16, 90+20, 72, 90+37, 0,NULL, "结果列" },
  { SINGLELINEEDITORITEM, 0, 75, 90+20, 140, 90+37, 0, Resu_0_Procedure, "" },

  { STATICTEXTITEM, 0, 16, 150-14, 72, 154, 0,NULL, "格  式" },
  { SINGLELINEEDITORITEM, 0, 75, 150-14, 140, 154, 0, format_0_Procedure, "" },

  { STATICTEXTITEM, 0, 155, 90+20, 211, 90+37, 0,NULL, "前  缀" },
  { SINGLELINEEDITORITEM, 0, 212, 90+20, 277, 90+37, 0, prev_0_Procedure, "" },

  { STATICTEXTITEM, 0, 155, 150-14, 211, 154, 0,NULL, "后  缀" },
  { SINGLELINEEDITORITEM, 0, 212, 150-14, 277, 154, 0, succ_0_Procedure, "" },

  { STATICTEXTITEM, 0,13+xx, 150-20-yy, 100+xx, 170-25-yy, 0, NULL, "计算范围:" },
  { STATICTEXTITEM, 0,160+xx, 150-20-yy, 240+xx, 170-25-yy, 0, NULL, "统计范围:" },

  { STATICTEXTITEM, 0, 13+xx, 180-25-yy, 63+xx, 200-25-yy, 0, NULL, "起始行" },
  { SINGLELINEEDITORITEM, 0, 70+xx, 180-25-yy, 130+xx, 200-28-yy, 0,Start_0_Procedure, "" },

  { STATICTEXTITEM, 0, 13+xx, 210-25-yy, 63+xx, 230-25-yy, 0, NULL, "结束行" },
  { SINGLELINEEDITORITEM, 0, 70+xx, 210-25-yy, 130+xx, 230-28-yy, 0,End_0_Procedure, "" },

  { STATICTEXTITEM, 0, 160+xx, 180-25-yy, 210+xx, 200-25-yy, 0, NULL, "起始列" },
  { SINGLELINEEDITORITEM, 0, 217+xx, 180-25-yy, 277+xx, 200-28-yy, 0,Start_1_Procedure, "" },

  { STATICTEXTITEM, 0, 160+xx, 210-25-yy, 210+xx, 230-25-yy, 0, NULL, "结束列" },
  { SINGLELINEEDITORITEM, 0, 217+xx, 210-25-yy, 277+xx, 230-28-yy, 0,End_1_Procedure, "" },

  { USERBUTTONITEM,0,16+xx, 220-yy, 75+xx, 245-yy,11900, NULL, "求  和" },
  { USERBUTTONITEM,0,86+xx, 220-yy, 145+xx, 245-yy,11901, NULL, "求平均" },
  { USERBUTTONITEM,0,156+xx, 220-yy, 215+xx, 245-yy,11902, NULL, "表达式" },
  { CANCELBUTTON,0,226+xx, 220-yy, 285+xx, 245-yy, 0, NULL, "放  弃" },
#undef xx
#undef yy
};

static Dialogs TableCalcExpress1[]=
{
#define xx  280
#define yy  91
  { GLOBALITEM, 26 , 0, 0, 575, 255-yy, 0,TableCalcPro,"竖向统计" },

  { STATICTEXTITEM, 0, 16, 61-22, 76, 80-25, 0,NULL, "表达式" },
  { SINGLELINEEDITORITEM, 0, 78, 61-22, 277, 80-25, 0, Exp_1_Procedure, "" },
  { SINGLELINEEDITORITEM, 0, 16, 90-30, 277, 90-13, 0, Exp_2_Procedure, "" },
  { SINGLELINEEDITORITEM, 0, 16, 90-8, 277, 90+9, 0, Exp_3_Procedure, "" },

  { STATICTEXTITEM, 0, 16, 90+20, 72, 90+37, 0,NULL, "结果行" },
  { SINGLELINEEDITORITEM, 0, 75, 90+20, 140, 90+37, 0, Resu_0_Procedure, "" },

  { STATICTEXTITEM, 0, 16, 150-14, 72, 154, 0,NULL, "格  式" },
  { SINGLELINEEDITORITEM, 0, 75, 150-14, 140, 154, 0, format_0_Procedure, "" },

  { STATICTEXTITEM, 0, 155, 90+20, 211, 90+37, 0,NULL, "前  缀" },
  { SINGLELINEEDITORITEM, 0, 212, 90+20, 277, 90+37, 0, prev_0_Procedure, "" },

  { STATICTEXTITEM, 0, 155, 150-14, 211, 154, 0,NULL, "后  缀" },
  { SINGLELINEEDITORITEM, 0, 212, 150-14, 277, 154, 0, succ_0_Procedure, "" },

  { STATICTEXTITEM, 0,13+xx, 150-20-yy, 100+xx, 170-25-yy, 0, NULL, "计算范围:" },
  { STATICTEXTITEM, 0,160+xx, 150-20-yy, 240+xx, 170-25-yy, 0, NULL, "统计范围:" },

  { STATICTEXTITEM, 0, 13+xx, 180-25-yy, 63+xx, 200-25-yy, 0, NULL, "起始列" },
  { SINGLELINEEDITORITEM, 0, 70+xx, 180-25-yy, 130+xx, 200-28-yy, 0,Start_0_Procedure, "" },

  { STATICTEXTITEM, 0, 13+xx, 210-25-yy, 63+xx, 230-25-yy, 0, NULL, "结束列" },
  { SINGLELINEEDITORITEM, 0, 70+xx, 210-25-yy, 130+xx, 230-28-yy, 0,End_0_Procedure, "" },

  { STATICTEXTITEM, 0, 160+xx, 180-25-yy, 210+xx, 200-25-yy, 0, NULL, "起始行" },
  { SINGLELINEEDITORITEM, 0, 217+xx, 180-25-yy, 277+xx, 200-28-yy, 0,Start_1_Procedure, "" },

  { STATICTEXTITEM, 0, 160+xx, 210-25-yy, 210+xx, 230-25-yy, 0, NULL, "结束行" },
  { SINGLELINEEDITORITEM, 0, 217+xx, 210-25-yy, 277+xx, 230-28-yy, 0,End_1_Procedure, "" },

  { USERBUTTONITEM,0,16+xx, 220-yy, 75+xx, 245-yy,11900, NULL, "求  和" },
  { USERBUTTONITEM,0,86+xx, 220-yy, 145+xx, 245-yy,11901, NULL, "求平均" },
  { USERBUTTONITEM,0,156+xx, 220-yy, 215+xx, 245-yy,11902, NULL, "表达式" },
  { CANCELBUTTON,0,226+xx, 220-yy, 285+xx, 245-yy, 0, NULL, "放  弃" },
#undef xx
#undef yy
};

#ifdef UNUSE
static Dialogs TableCalcExpress1[]=
{
  { GLOBALITEM, 20 , 0, 0, 300, 255, 0,TableCalcPro,"竖向统计" },

  { STATICTEXTITEM, 0, 16, 61-22, 96, 80-25, 0,NULL, "表达式:" },
  { STATICTEXTITEM, 0, 166, 61-22, 222, 80-25, 0,NULL, "结果行:" },
  { SINGLELINEEDITORITEM, 0, 227, 61-22, 277, 80-25, 0, Resu_0_Procedure, "" },

  { SINGLELINEEDITORITEM, 0, 16, 90-30, 277, 90-13, 0, Exp_1_Procedure, "" },
  { SINGLELINEEDITORITEM, 0, 16, 90-8, 277, 90+9, 0, Exp_2_Procedure, "" },
  { SINGLELINEEDITORITEM, 0, 16, 90+14, 277, 90+31, 0, Exp_3_Procedure, "" },

  { STATICTEXTITEM, 0,13, 150-20, 100, 170-25, 0, NULL, "计算范围:" },
  { STATICTEXTITEM, 0,160, 150-20, 240, 170-25, 0, NULL, "统计范围:" },

  { STATICTEXTITEM, 0, 13, 180-25, 63, 200-25, 0, NULL, "起始列" },
  { SINGLELINEEDITORITEM, 0, 70, 180-25, 130, 200-28, 0,Start_0_Procedure, "" },

  { STATICTEXTITEM, 0, 13, 210-25, 63, 230-25, 0, NULL, "结束列" },
  { SINGLELINEEDITORITEM, 0, 70, 210-25, 130, 230-28, 0,End_0_Procedure, "" },

  { STATICTEXTITEM, 0, 160, 180-25, 210, 200-25, 0, NULL, "起始行" },
  { SINGLELINEEDITORITEM, 0, 217, 180-25, 277, 200-28, 0,Start_1_Procedure, "" },

  { STATICTEXTITEM, 0, 160, 210-25, 210, 230-25, 0, NULL, "结束行" },
  { SINGLELINEEDITORITEM, 0, 217, 210-25, 277, 230-28, 0,End_1_Procedure, "" },

  { USERBUTTONITEM,0,16, 220, 75, 245,11900, NULL, "求  和" },
  { USERBUTTONITEM,0,86, 220, 145, 245,11901, NULL, "求平均" },
  { USERBUTTONITEM,0,156, 220, 215, 245,11902, NULL, "表达式" },
  { CANCELBUTTON,0,226, 220, 285, 245, 0, NULL, "放  弃" },
};
#endif




static int UserStrToNum(char *str)
{
    char p[200];
    int len;

    while (*str==' ') str++;
    strncpy(p,str,120);
    len=119;
    while (len>0&&p[len]==' ') p[len--]=0;
    p[120]=0;
    strupr(p);
    len=strlen(p);
    if (*p>='0'&&(*p<='9')) return atoi(p);
    if (len==1&&(*p>='A')&&(*p<='Z')) return *p-'A'+1;
    if (*p=='H'||(*p=='R')) return atoi(p+1);
    if (*p=='C'||(*p=='L')) return atoi(p+1);
    return -1;
 }

static char ErrInfo[][40]={
  "求值范围:起始行值错误!",
  "求值范围:结束行值错误!",
  "结果列:值错误!",
  "统计范围:起始列值错误!",
  "统计范围:结束列值错误!",
  "结果格式错!",
  "表达式计算有误,结果可能不正确 !",
  "计算表元存在合并情况,结果可能不正确 !"
};

static char ErrInfo1[][40]={
  "求值范围:起始列值错误!",
  "求值范围:结束列值错误!",
  "结果行:值错误!",
  "统计范围:起始行值错误!",
  "统计范围:结束行值错误!",
  "结果格式错!",
  "表达式计算有误,结果可能不正确 !",
  "计算表元存在合并情况,结果可能不正确 !"
};

static int TableExpressCalcRow(HFormBoxs hFormBox,int mode)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int i,j,start,end;
   int iCell,nRow,nCol,Col;
   int ret,resu_col,err=0;
   double hold;
   char buff[5000];
   char Express[400];
   char res[100];
   char prev[100],succ[100],disp[300];

   strncpy(prev,trim(prev_0),55);
   prev[55]=0;

   strncpy(succ,trim(succ_0),55);
   succ[55]=0;

   if (get_format_str(format_0,res)) return 6;


   start=UserStrToNum(start_0);
   if (start>0) start--;
     else
   return 1;

   end=UserStrToNum(end_0);
   if (end>0) end--;
     else
   return 2;

   resu_col=UserStrToNum(resu_0);
   if (resu_col>0) resu_col--;
     else
   return 3;

   strcpy(Express,exp_1);
   strcat(Express,exp_2);
   strcat(Express,exp_3);

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   Col=pFormBox->numCols;

   if (mode==0)
   {
     nCol=pFormBox->numCols;
     nRow=pFormBox->numLines;
   }
   else
   {
     nRow=pFormBox->numCols;
     nCol=pFormBox->numLines;
   }

   if (resu_col>=nCol)
   {
      HandleUnlock(pFormBox->hCellTable);
      HandleUnlock(ItemGetHandle(hFormBox));
      return 3;
   }

   if (end>=nRow)
   {
      HandleUnlock(pFormBox->hCellTable);
      HandleUnlock(ItemGetHandle(hFormBox));
      return 2;
   }

   for (i=start;i<nRow&&i<=end;i++)
    {
        for (j=0;j<nCol;j++)
         {
            if (mode==0)
               iCell=i*Col+j;
            else
               iCell=j*Col+i;
            if (pCellTable[iCell].iFirst==FIRSTCELL)
            {
             TableBoxGetTextStr(hFormBox,iCell,buff,200);
             ret=0;
             while (buff[ret]&&
                 !(buff[ret]=='.'||buff[ret]=='-'||
                 (buff[ret]>='0'&&buff[ret]<='9'))
                 ) ret++;        // find first num
             hold=get_num(buff+ret);
             set_val(j,hold);
            }
            else
             {
             set_val(j,0.0);
             err |=0x80;
             }
         }
         ret=get_exp(Express,&hold);
         if (ret)
           {
           strcpy(buff,"***");
           err |=0x40;
           }
         else
           sprintf(buff,res,hold);
         if (mode==0)
           iCell=i*Col+resu_col;
         else
           iCell=resu_col*Col+i;
         strcpy(disp,prev);
         FormatNumText(buff,40);           //Jan 7 1997 By zjh
         strcat(disp,buff);
         strcat(disp,succ);
         TableBoxInsertText(hFormBox,iCell,disp,TRUE);
    }

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return err;
}

static int TableTotalCalcRow(HFormBoxs hFormBox,int mm,int mode)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int i,j,start,end;
   int st,ed;
   int iCell,nRow,nCol,Col;
   int ret,resu_col,err=0;
   double hold,s;
   char buff[5000];
   // char Express[400];
   char res[100];
   char prev[100],succ[100],disp[300];

   strncpy(prev,trim(prev_0),55);
   prev[55]=0;

   strncpy(succ,trim(succ_0),55);
   succ[55]=0;

   if (get_format_str(format_0,res)) return 6;

   start=UserStrToNum(start_0);
   if (start>0) start--;
     else
   return 1;

   end=UserStrToNum(end_0);
   if (end>0) end--;
     else
   return 2;

   st=UserStrToNum(start_1);
   if (st>0) st--;
     else
   return 4;

   ed=UserStrToNum(end_1);
   if (ed>0) ed--;
     else
   return 5;

   resu_col=UserStrToNum(resu_0);
   if (resu_col>0) resu_col--;
     else
   return 3;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   Col=pFormBox->numCols;

   if (mode==0)
   {
     nCol=pFormBox->numCols;
     nRow=pFormBox->numLines;
   }
   else
   {
     nRow=pFormBox->numCols;
     nCol=pFormBox->numLines;
   }

   if (resu_col>=nCol)
   {
      HandleUnlock(pFormBox->hCellTable);
      HandleUnlock(ItemGetHandle(hFormBox));
      return 3;
   }

   if (end>=nRow)
   {
      HandleUnlock(pFormBox->hCellTable);
      HandleUnlock(ItemGetHandle(hFormBox));
      return 2;
   }

   if (ed>=nCol)
   {
      HandleUnlock(pFormBox->hCellTable);
      HandleUnlock(ItemGetHandle(hFormBox));
      return 5;
   }

   for (i=start;i<nRow&&i<=end;i++)
    {
        s=0.0;
        for (j=st;j<=ed&&j<nCol;j++)
         {
            if (mode==0)
              iCell=i*Col+j;
            else
              iCell=j*Col+i;
            if (pCellTable[iCell].iFirst==FIRSTCELL)
            {
             TableBoxGetTextStr(hFormBox,iCell,buff,200);
             ret=0;
             while (buff[ret]&&
                 !(buff[ret]=='.'||buff[ret]=='-'||
                 (buff[ret]>='0'&&buff[ret]<='9'))
                 ) ret++;
             hold=get_num(buff+ret);
             s=s+hold;
            }
            else
             {
             err |=0x80;
             }
        }
         if (mm==1)
           hold=s;
         else
           hold=s/(ed-st+1);
         sprintf(buff,res,hold);
         if (mode==0)
            iCell=i*Col+resu_col;
         else
            iCell=resu_col*Col+i;
         strcpy(disp,prev);
         FormatNumText(buff,40);           //Jan 7 1997 By zjh
         strcat(disp,buff);
         strcat(disp,succ);
         TableBoxInsertText(hFormBox,iCell,disp,TRUE);
    }

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return err;
}

static void swap_str(char *s1,char *s2)
{
    int len=max(strlen(s1),strlen(s2));
    int i;
    char c;
    if (len>19) len=19;
    for (i=0;i<len;i++)
      {
        c=s1[i];
        s1[i]=s2[i];
        s2[i]=c;
      }
}

extern int fDialogBoxAtTop;
static int last_mode=-1;
int ExpressCalc(HWND Window,int mode)
{
    int ret;
    static char RowErr[]="横向统计错误";
    static char ColErr[]="竖向统计错误";

    if (mode!=last_mode)
     {
        swap_str(start_0,start_1);
        swap_str(end_0,end_1);
        sprintf(resu_0,"%d",atoi(end_1)+1);
     }

    last_mode=mode;

    if (GlobalTableBlockStart!=GlobalTableBlockEnd)
    {
        PFormBoxs pFormBox;
        CELL * pCellTable;
        int Col,x0,x1,y0,y1;

        pFormBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
        if (pFormBox==NULL)
            return(OUTOFMEMORY);

        pCellTable=HandleLock(pFormBox->hCellTable);
        if (pCellTable==NULL)
        {
            HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
            return(OUTOFMEMORY);
        }

        Col=pFormBox->numCols;

        HandleUnlock(pFormBox->hCellTable);
        HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));

        x0=GlobalTableBlockStart%Col+1;
        y0=GlobalTableBlockStart/Col+1;
        x1=GlobalTableBlockEnd%Col+1;
        y1=GlobalTableBlockEnd/Col+1;
        if (mode==0)
           {
            sprintf(start_0,"%d",y0);
            sprintf(end_0,"%d",y1);
            sprintf(start_1,"%d",x0);
            sprintf(end_1,"%d",x1);
            sprintf(resu_0,"%d",x1+1);
           }
        else
           {
            sprintf(start_1,"%d",y0);
            sprintf(end_1,"%d",y1);
            sprintf(start_0,"%d",x0);
            sprintf(end_0,"%d",x1);
            sprintf(resu_0,"%d",y1+1);
           }
    }

    fDialogBoxAtTop=1;
    if (mode==0)
      ret=MakeDialogBox(Window,TableCalcExpress);
    else
      ret=MakeDialogBox(Window,TableCalcExpress1);
    fDialogBoxAtTop=0;

    if (!ret)
    {
      switch (Calc_Mode)
      {
        case 3:
            ret=TableExpressCalcRow(GlobalBoxHeadHandle,mode);
            break;
        case 1:
        case 2:
            ret=TableTotalCalcRow(GlobalBoxHeadHandle,Calc_Mode,mode);
            break;
      }

      if (mode==0)
      {
          if ((ret&15)>0&&(ret&15)<7)
             MessageBox(RowErr,ErrInfo[ret-1],1,Window);
          else
          if (ret&0x80)
             MessageBox(RowErr,ErrInfo[7],1,Window);
          else
          if (ret&0x40)
             MessageBox(RowErr,ErrInfo[6],1,Window);
          else
            ret=-1;
      }
      else
      {
          if ((ret&15)>0&&(ret&15)<7)
             MessageBox(ColErr,ErrInfo1[ret-1],1,Window);
          else
          if (ret&0x80)
             MessageBox(ColErr,ErrInfo1[7],1,Window);
          else
          if (ret&0x40)
             MessageBox(ColErr,ErrInfo1[6],1,Window);
          else
            ret=-1;
      }
    }

    return (-ret);
}

/*
main()
{
  int i,j;
  char buff[1200];

  InitDbf("e:\\codeplus\\examples\\data2.dbf");
  for (i=0;i<DbfHeader.num_field;i++)
   {
    printf("\n%12s  %c  %02d  %02d",FieldList[i].name,FieldList[i].type,
                              FieldList[i].len,FieldList[i].dec);
   }
  getch();
  for (i=0;i<DbfHeader.num_recs;i++)
   {
    ReadDbf(i,buff);
    printf("\n");
    for (j=0;j<DbfHeader.num_field;j++)
     {
     printf("%s ",GetField(j,buff));
     }
   }

  i=CloseDbf();
}
*/

#include "ezpHead.h"

#ifndef REGIST_VERSION
void ReportMemoryError(char *str)
{
  printf("malloc error in %s.\n",str);
}
#endif

#define Qt      65530           /* quiet */
#define HanceMUSICLen   7
static unsigned short Music1[2*HanceMUSICLen]={
        523,698,880,1046,Qt, 880,1046,          //-- Freq
        200,200,120,130, 80, 140, 640           //-- Delay
};

/*---------------------------
#define JerryMUSICLen   51
static unsigned short Music3[2*JerryMUSICLen]={
  Qt,523,698, Qt,889,698,523, Qt,889, Qt,698, Qt,889,698,523,
  Qt,189, Qt,698, Qt,889,698,889,698, Qt,523,698, Qt,889,698,
 523, Qt,889, Qt,698, Qt,889,698,523, Qt,889, Qt,698, Qt,889,
 698,523, Qt,523, Qt,698,                      //-- Freq
  57,171,128,  6, 75, 96,128,  1,100, 17,128,  6, 75, 96,128,
   1,100, 17,128,  6, 75,192,402,192, 57,171,128,  6, 75, 96,
 128,  1,100, 17,128,  6, 75, 96,128,  1,100, 17,128,  6, 75,
  96,128,  1,128, 17,543
};
---------------------------*/

#define _0      Qt
#define _1      1046
#define _2      1174
#define _3      1318
#define _4      1396
#define _5      1568
#define _6      1760
#define _6_     698

#define _F      400
#define _H      200
#define _D      800
#define JerryMUSICLen   40
static unsigned short Music2[] = {
/*
 1 2 3 1 0 1 2 3 1 0 3 4 5 0 3 4 5 0
 5 6 5 4 3 1
 5 6 5 4 3 1
 2 -6 1
 2 -6 1
*/
 _1,_2,_3,_1,_0,_1,_2,_3,_1,_0,
 _3,_4,_5,_0,_3,_4,_5,_0,
 _5,_6,_5,_4,_3,_1,_0,
 _5,_6,_5,_4,_3,_1,_0,
 _2,_6_,_1,_0,
 _2,_6_,_1,_0,

 _F,_F,_F,_F,_H,_F,_F,_F,_F,_H,
 _F,_F,_D,_H,_F,_F,_D,_H,
 _H,_H,_H,_H,_F,_H,_H,
 _H,_H,_H,_H,_F,_H,_H,
 _F,_F,_D,_H,
 _F,_F,_D,_H,
};
#undef Qt

static void PlayMusic(unsigned short MusicTable[],int TableLen)
{
  int i;
  int  SaveColor;
  struct viewporttype ViewInformation;
  int flash=0;
  unsigned long *p=(unsigned long *)0x46c;
  unsigned long now,end,start;

  SetIntSign();

  if(MusicTable==Music2)
  {
      flash=1;
      SaveColor=getcolor();
      getviewsettings(&ViewInformation);
      MouseHidden();
      setviewport(0,0,getmaxx(),getmaxy(),1);
      setcolor(EGA_BLUE);
  }

  start=*p;
  for( i=0; i<TableLen; i++ ) {
       sound(MusicTable[i]);

       //----- delay --------
       now=*p;
       end=now+((float)MusicTable[i+TableLen]*18.2/1000+0.5);
       while(now<end)
       {
          //p=(unsigned long *)0x46c;
          now=*p;
          if(now-start>=6)
          {
             start=now;
            #define X      32      /* see also, scrollc.c{TellFileName) */
            #define Y      8
            #define MAXLEN 28
             if(flash==1)
             {
                flash=2;
                bar(X,Y,X+MAXLEN*ASC16WIDTH,Y+ASC16HIGHT);
             } else
             if(flash==2)
             {
                flash=1;
                DisplayString(DebugFileName,X,Y,EGA_WHITE,EGA_BLUE);
             }
            #undef Y
            #undef X
            #undef MAXLEN
          }
          now=*p;
       }

       nosound();
  }

  if(flash)
  {
     setcolor(SaveColor);
     setviewport(ViewInformation.left,ViewInformation.top,
               ViewInformation.right,ViewInformation.bottom,
               ViewInformation.clip);
     MouseShow();
  }
  ClearIntSign();
}

#define MAXIMAGENUM     5        //By zjh ora=5       /* for nest dialog */
static char *DialogImage[MAXIMAGENUM];
static int DialogImageN=0;

  // creat Tab Link, ByHance, 95,11.22
#define MAXTABWIN 50
static HWND TabLinkArr[MAXTABWIN];
static int  TotalTabLinkNum[MAXIMAGENUM+1]={0},NowTabIdx[MAXIMAGENUM+1]={0};

static void CreatTabLink(HWND Window)
{
  HWND MidWindow;

  MidWindow=WindowGetChild(Window);
  while (MidWindow)
  {
    if (WindowCanTabOrder(MidWindow)) {
        if(TotalTabLinkNum[DialogImageN]>=MAXTABWIN) break;
        TabLinkArr[TotalTabLinkNum[DialogImageN]]=MidWindow;
        TotalTabLinkNum[DialogImageN]++;
    }
    CreatTabLink(MidWindow);            // search its children
    MidWindow=WindowGetNext(MidWindow);  // search its brothers
  }
}

int WindowTableOrderNext(HWND Window)
{
  HWND MidWindow;
  int  i=TotalTabLinkNum[DialogImageN-1];

  if(!TotalTabLinkNum[DialogImageN])
        ReturnOK();
  while(i<TotalTabLinkNum[DialogImageN] && TabLinkArr[i]!=Window) i++;
  if(i==TotalTabLinkNum[DialogImageN])          // not found the window
      i=NowTabIdx[DialogImageN];           // ReturnOK();
  else
      i++;


  fGetFocusByKey=1;
  do {
      if(i==TotalTabLinkNum[DialogImageN])
          i=TotalTabLinkNum[DialogImageN-1];

      NowTabIdx[DialogImageN]=i;

      MidWindow=TabLinkArr[i];      // get NextOrderWindow
      if (MidWindow==Window)         // the same window, do nothing
          ReturnOK();

    //  MessageInsert(MidWindow,GETFOCUS,0l,0l);
      MessageGo(MidWindow,GETFOCUS,0l,0l);
      i++;
  } while (ActiveWindow!=MidWindow);

  fGetFocusByKey=0;

  return(TRUE);
}

int WindowTableOrderPrev(HWND Window)
{
  HWND MidWindow;
  int  i=TotalTabLinkNum[DialogImageN-1];

  if(!TotalTabLinkNum[DialogImageN])
        ReturnOK();
  while(i<TotalTabLinkNum[DialogImageN] && TabLinkArr[i]!=Window) i++;
  if(i==TotalTabLinkNum[DialogImageN])          // not found the window
      i=NowTabIdx[DialogImageN];           // ReturnOK();
  else
      i--;

  fGetFocusByKey=1;
  do {
      if(i<TotalTabLinkNum[DialogImageN-1])
         i=TotalTabLinkNum[DialogImageN]-1;

      NowTabIdx[DialogImageN]=i;

      MidWindow=TabLinkArr[i];      // get PrevOrderWindow
      if (MidWindow==Window)         // the same window, do nothing
         ReturnOK();

    //  MessageInsert(MidWindow,GETFOCUS,0l,0l);
      MessageGo(MidWindow,GETFOCUS,0l,0l);
      i--;
  } while (ActiveWindow!=MidWindow);

  fGetFocusByKey=0;

  return(TRUE);
}

long DialogDefaultProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case KEYDOWN:
         switch (MAKELO(Param1))
         {
           case TAB:
                WindowTableOrderNext(Window);
                break;
           case SHIFT_TAB:
                WindowTableOrderPrev(Window);
                break;
           case ESC:
                Param1=DIALOGBOXCANCEL;
                //MessageInsert(Window,DIALOGBOXCANCEL,Param1,Param2);
                //break;
                goto proc_msg;
           case ENTER:
                Param1=DIALOGBOXOK;
                //MessageInsert(Window,DIALOGBOXOK,Param1,Param2);
            proc_msg:
                MessageInsert(Window,(HMSG)Param1,Param1,Param2);
                break;
         }
         break;
    case REDRAWMESSAGE:
         Param2=MAKELONG( WindowGetWidth(Window),WindowGetHeight(Window) );
         WindowDefaultProcedure(Window,Message,0L,Param2);
         break;
    case WINDOWCLOSE:
         Param1=DIALOGBOXCANCEL;
    case DIALOGBOXEND:
         MessageGo(Window,WINDOWQUIT,Message,0l);
         GlobalNotDisplay=1;
         WindowDefaultProcedure(Window,WINDOWCLOSE,Param1,Param2);
         GlobalNotDisplay=0;
 //------ ByHance, 96,1.8 ---
         if(DialogImageN)
         {
           int Left,Top,Right,Bottom;
           struct viewporttype TmpViewPort;

           MouseHidden();
           getviewsettings(&TmpViewPort);
           WindowGetRealRect(1,&Left,&Top,&Right,&Bottom);
           setviewport(Left,Top,Right,Bottom,1);

           putimage( WindowGetLeft(Window)-1,WindowGetTop(Window)-1,
                     DialogImage[--DialogImageN], COPY_PUT );
           free(DialogImage[DialogImageN]);

           setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                       TmpViewPort.bottom,TmpViewPort.clip);
           MouseShow();
         }
         return(Param1);
    case DIALOGBOXOK:
         if (WindowDefaultProcedure(Window,Message,Param1,Param2)==FALSE)
            break;
    case DIALOGBOXCANCEL:
         MessageInsert(Window,DIALOGBOXEND,Message,0l);
         break;
    case MOUSEMOVE:
         DialogMouseMove(Window,Message,Param1,Param2); // ByHance, 95,12.6
         break;
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static int CreatSubDialogBox(Dialogs *DialogItems,HWND Window,
                    HWND *DefaultActiveWindow,int *CurrentDefaultClass)
{                                      // The topest call must let
                                       // DefaultActiveWindow = 0 ( for default )
                                       // and CurrentDefaultClass = 0
  int j,TotalItem;
  HWND MidWindow;

  TotalItem=1;

  switch (DialogItems[0].ItemType)
  {
    case OKBUTTON:
         (DialogItems[0]).ItemUserData=DIALOGBOXOK;
         if (DialogItems[0].ItemTitle[0]==0)
            strcpy(DialogItems[0].ItemTitle,"È·¶¨");
         goto creat_win;
    case CANCELBUTTON:
         (DialogItems[0]).ItemUserData=DIALOGBOXCANCEL;
         if (DialogItems[0].ItemTitle[0]==0)
            strcpy(DialogItems[0].ItemTitle,"È¡Ïû");
    case USERBUTTONITEM:
        creat_win:
         MidWindow=CreatButton((DialogItems[0]).ItemLeft,
                  (DialogItems[0]).ItemTop,
                  (DialogItems[0]).ItemRight,
                  (DialogItems[0]).ItemBottom,
                  WindowSetIsUserButton(),
                  (DialogItems[0]).ItemUserData,
                  (DialogItems[0]).ItemTitle,
                  (DialogItems[0]).ItemProcedure,
                  Window);
         if (*CurrentDefaultClass<1&&DialogItems[0].ItemType==OKBUTTON)
         {
            *DefaultActiveWindow=MidWindow;
            *CurrentDefaultClass=1;
         }
         break;
    case COMBOXITEM:
         CreatCombo((DialogItems[0]).ItemLeft,
                    (DialogItems[0]).ItemTop,
                    (DialogItems[0]).ItemRight,
                    (DialogItems[0]).ItemBottom,
                    (DialogItems[0]).ItemProcedure,
                    Window);
         break;
    case SINGLELINEEDITORITEM:
         MidWindow=CreatSingleLineEditor((DialogItems[0]).ItemLeft,
                              (DialogItems[0]).ItemTop,
                              (DialogItems[0]).ItemRight,
                              (DialogItems[0]).ItemBottom,
                              (DialogItems[0]).ItemProcedure,
                              Window);
         if (*CurrentDefaultClass<3)
         {
            *DefaultActiveWindow=MidWindow;
            *CurrentDefaultClass=3;
         }
         break;
    case LISTBOXITEM:
         MidWindow=CreatListBox((DialogItems[0]).ItemLeft,
                   (DialogItems[0]).ItemTop,
                   (DialogItems[0]).ItemRight,
                   (DialogItems[0]).ItemBottom,
                   (DialogItems[0]).ItemProcedure,
                   Window);
         if (*CurrentDefaultClass<2)
         {
            *DefaultActiveWindow=MidWindow;
            *CurrentDefaultClass=2;
         }
         break;
    case SINGLESELECT:
         CreatRadioButton((DialogItems[0]).ItemLeft,
                          (DialogItems[0]).ItemTop,
                          (DialogItems[0]).ItemRight,
                          (DialogItems[0]).ItemUserData&0x00ff,
                          (DialogItems[0]).ItemUserData&0x8000,
                          (DialogItems[0]).ItemTitle,
                          ((DialogItems[0]).ItemUserData&0x3f00)>>8,
                          (DialogItems[0]).ItemProcedure,
                          Window);
         break;
    case MULTISELECT:
         CreatFrameButton((DialogItems[0]).ItemLeft,
                          (DialogItems[0]).ItemTop,
                          (DialogItems[0]).ItemRight,
                          (DialogItems[0]).ItemUserData&0x00ff,
                          (DialogItems[0]).ItemUserData&0x8000,
                          (DialogItems[0]).ItemTitle,
                          ((DialogItems[0]).ItemUserData&0x3f00)>>8,
                          (DialogItems[0]).ItemProcedure,
                          Window);
         break;
    case FRAMEITEM:
         {
           HWND FrameWindow;
           int  i;

           FrameWindow=CreatFrameWindow((DialogItems[0]).ItemLeft,
                                        (DialogItems[0]).ItemTop,
                                        (DialogItems[0]).ItemRight,
                                        (DialogItems[0]).ItemBottom,
                                        (DialogItems[0]).ItemTitle,
                                        (DialogItems[0]).ItemProcedure,
                                        Window);

           if(FrameWindow<0) return 10000;

           for (i=0,j=1;i<(DialogItems[0]).ItemTotal;i++) {
               j+=CreatSubDialogBox(&DialogItems[j], FrameWindow,
                    DefaultActiveWindow,CurrentDefaultClass);
           }
           TotalItem+=j-1;
         }
         break;
    case STATICTEXTITEM:
         CreatStaticText((DialogItems[0]).ItemLeft,
                         (DialogItems[0]).ItemTop,
                         (DialogItems[0]).ItemRight,
                         (DialogItems[0]).ItemTitle,
                         (DialogItems[0]).ItemProcedure,
                         Window);
         break;
    case HSCROLLITEM:
         CreatHScroll((DialogItems[0]).ItemLeft,
                      (DialogItems[0]).ItemTop,
                      (DialogItems[0]).ItemRight,
                      (DialogItems[0]).ItemProcedure,
                      Window);
         break;
  }
  return(TotalItem);
}

static int CreatDialogBox(Dialogs *DialogItems,
                   HWND *DefaultActiveWindow)
{
  HWND MidWindow;
  char *pImage;
  int i,j,TotalItem,CurrentDefaultClass=0;
  Function *fun=(DialogItems[0]).ItemProcedure;

  if(DialogImageN>=MAXIMAGENUM)
       return -1;

  i=imagesize( DialogItems[0].ItemLeft-1,DialogItems[0].ItemTop-1,
               DialogItems[0].ItemRight,DialogItems[0].ItemBottom );
  pImage=(char *)malloc(i);
  //if(pImage==NULL)
  if(pImage<0x1000)
  {
      ReportMemoryError("creatdialog");
      return -1;
  }

  {
    int Left,Top,Right,Bottom;
    struct viewporttype TmpViewPort;

    MouseHidden();
    getviewsettings(&TmpViewPort);
    WindowGetRealRect(1,&Left,&Top,&Right,&Bottom);
    setviewport(Left,Top,Right,Bottom,1);
    getimage( DialogItems[0].ItemLeft-1,DialogItems[0].ItemTop-1,
            DialogItems[0].ItemRight,DialogItems[0].ItemBottom, pImage);
    setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                TmpViewPort.bottom,TmpViewPort.clip);
    MouseShow();
  }


  if (fun==NULL) fun=(Function *)DialogDefaultProcedure;
  MidWindow=CreatWindow((DialogItems[0]).ItemLeft,(DialogItems[0]).ItemTop,
                        (DialogItems[0]).ItemRight,
                        (DialogItems[0]).ItemBottom,0,
                        fun,
                        2|16|WindowSetIsFocusAlways,
                        (DialogItems[0]).ItemTitle,NULL);

  if(MidWindow<0)               // ByHnace
  {
     free(pImage);
     return(MidWindow);
  }

  DialogImage[DialogImageN++]=pImage;

  if ((DialogItems[0]).ItemType!=GLOBALITEM)
     return(MidWindow);
  else
     TotalItem=(DialogItems[0]).ItemTotal;

  for (i=1,j=0;j<TotalItem;j++)
      i+=CreatSubDialogBox(&DialogItems[i],MidWindow,DefaultActiveWindow,
                           &CurrentDefaultClass);
  return(MidWindow);
}

int fDialogBoxAtTop=0;

int MakeDialogBox(HWND FatherWindow,Dialogs *Dialog)
{
  HWND Window,/*MidWindow,*/DefaultActiveWindow=0;
  int  Result,width,height;
  HMSG Message,MessageNumber;
  ULONG Param1,Param2;

  SetIntSign();

  width=Dialog[0].ItemRight-Dialog[0].ItemLeft;
  height=Dialog[0].ItemBottom-Dialog[0].ItemTop;
  Dialog[0].ItemLeft = (getmaxx()-width)/2-4;     // ByHance, 96,3.13

  if(fDialogBoxAtTop)           // ByHance, 97,5.7
     Dialog[0].ItemTop = 6;
  else
     Dialog[0].ItemTop = (getmaxy()-height)/2+6;

  Dialog[0].ItemRight=Dialog[0].ItemLeft+width;
  Dialog[0].ItemBottom=Dialog[0].ItemTop+height;

  Window=CreatDialogBox(Dialog,&DefaultActiveWindow);
  if (Window<0)
     return(Window);

  MouseSetGraph(ARRAWMOUSE);            // ByHance, 95,12.4
  MessageInsert(Window,WINDOWINIT,0, MAKELONG(width,height));
  //MessageGo(Window,WINDOWINIT,0, MAKELONG(width,height));

/*------------------------   ByHance, 96,3.1
  MidWindow=WindowGetChild(Window);
  while (MidWindow)
  {
    if (WindowGetUserData(MidWindow)==DIALOGBOXOK)
    {
       MessageInsert(MidWindow,GETFOCUS,0l,0l);
       break;
    }
    MidWindow=WindowGetNext(MidWindow);
  }
---------------------------*/
  NowTabIdx[DialogImageN]=
  TotalTabLinkNum[DialogImageN]=TotalTabLinkNum[DialogImageN-1];

  CreatTabLink(Window);         // ByHance, 95,11.22
  MessageInsert(Window,REDRAWMESSAGE,0,MAKELONG(width,height));

  WaitMessageEmpty();
  MessageInsert(TabLinkArr[TotalTabLinkNum[DialogImageN-1]],GETFOCUS,0l,0l);

  ClearIntSign();

  while(1)
  {
    KeyToMessage();
    if ((Result=MessageGet(&Window,&Message,&Param1,&Param2))<OpOK)
       break;

    MessageNumber=Result;
    if (MessageNumber<MAXMESSAGES)
    {
       Result=MessageGo(Window,Message,Param1,Param2);
       if ((Result==DIALOGBOXOK)||(Result==DIALOGBOXCANCEL)
           ||(Result==MESSAGEBOXNODO)|| Result>10000)
          break;
    }
    else
       MessageGo(Window,SYSTEMIDLE,Param1,Param2);
  }

  //TotalTabLinkNum=0;            // ByHance, 96,1.24

  WaitMessageEmpty();

  if (Result==DIALOGBOXOK)
     return(0);
  if (Result==DIALOGBOXCANCEL)
     return(1);
  if (Result==MESSAGEBOXNODO)
     return(2);
  return(Result);
}


static char *MessageMessage;          // keep Information
#undef  MAXMESSAGEDISPLAYLENGTH
#define MAXMESSAGEDISPLAYLENGTH 256

static long MessageBoxProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int ch;
  int Length,DisplayLeft,DisplayTop,i,DisplayLength,VLength;
  unsigned char MidString[MAXMESSAGEDISPLAYLENGTH/(CHARWIDTH/2)+1];
  unsigned char *p;
  struct viewporttype TmpViewPort;
  int Left,Top,Right,Bottom,CharPoint1,CharPoint2;

  static char BkDoor_st=0;

  switch (Message)
  {
    case DRAWWINDOW:
         DialogDefaultProcedure(Window,Message,Param1,Param2);

         MouseHidden();
         getviewsettings(&TmpViewPort);

         WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
         setviewport(Left,Top,Right,Bottom,1);

         CharPoint1=0;
         Length=strlen(MessageMessage);
         VLength=0;
         DisplayTop=50;
         while (CharPoint1<Length)
         {
           for (CharPoint2=CharPoint1;CharPoint2<Length;CharPoint2++)
               if (MessageMessage[CharPoint2]=='\n')
                  break;
           VLength=(CharPoint2-CharPoint1)*CHARWIDTH/2;
           for (i=0;i<VLength;i+=DisplayLength*(CHARWIDTH/2))
           {
             DisplayLength=MAXMESSAGEDISPLAYLENGTH/(CHARWIDTH/2);
             if ((i+(CharPoint1+DisplayLength)*(CHARWIDTH/2)<VLength)
                 &&IsInChineseChar(MessageMessage,
                   i/(CHARWIDTH/2)+CharPoint1+DisplayLength))
                DisplayLength--;
             if (DisplayLength>CharPoint2-CharPoint1-i/(CHARWIDTH/2))
                DisplayLength=CharPoint2-CharPoint1-i/(CHARWIDTH/2);
             strncpy(MidString,&MessageMessage[i/(CHARWIDTH/2)+CharPoint1],
                     DisplayLength);
             MidString[DisplayLength]=0;
    // DisplayLeft=WindowGetWidth(Window)/2-strlen(MidString)/2*(CHARWIDTH/2);
           DisplayLeft=(WindowGetWidth(Window)-MAXMESSAGEDISPLAYLENGTH)/2+8;
             ViewportDisplayString(MidString,DisplayLeft,DisplayTop,
                                   MESSAGECOLOR,MESSAGEBKCOLOR);
             DisplayTop+=CHARHEIGHT+4;
           }
           CharPoint1=CharPoint2+1;
         }
         setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                     TmpViewPort.bottom,TmpViewPort.clip);
         MouseShow();

         BkDoor_st=0;
         break;
    case MESSAGEBOXNODO:
         MessageInsert(Window,DIALOGBOXEND,Message,0l);
         break;

    case WINDOWINIT:            // ByHance, 95,12.19  for back door init
         BkDoor_st=0;
         return(DialogDefaultProcedure(Window,Message,Param1,Param2));

    case KEYDOWN:
         ch=toupper(Param1);

         switch (ch)
         {
           HWND MidWindow;

           default :
                BkDoor_st=0; break;
           case 'N':
                if (BkDoor_st==8)
                {
                    BkDoor_st++;
                    return(TRUE);
                }
                BkDoor_st=0;
                goto lbl_proc_button;
           case 'Y':
                if (BkDoor_st==15)              // ...(Music)JERRY
                {
                   static unsigned char REDTEK_Company[]={
                      'À'-4,'í'-5,
                      'µ'-6,'Â'-7,
                      'É'-8,'Ì'-9,
                      'Ó'-10,'Ã'-11,
                      '¼'-12,'¼'-13,
                      'Ê'-14,'õ'-15,
                      'Ó'-16,'Ð'-17,
                      'Ï'-18,'Þ'-19,
                      '¹'-20,'«'-21,
                      'Ë'-22,'¾'-23,
                      '°'-24,'æ'-25,
                      'È'-26,'¨'-27,
                      'Ë'-28,'ù'-29,
                      'Ó'-30,'Ð'-31
                   };
                   strcpy(MidString,DebugFileName);
                   for(i=0;i<28;i++) DebugFileName[i]=REDTEK_Company[i]+i+4;
                   TellFileName();
                   PlayMusic(Music2,JerryMUSICLen);
                   strcpy(DebugFileName,MidString);
                   TellFileName();
                   return(TRUE);
                }
           lbl_proc_button:
                MidWindow=WindowGetChild(Window);        //
                while (MidWindow)
                {
                  if( strlen(WindowGetTitle(MidWindow))==5
                  && DataofWindows[MidWindow].Title[3]==ch)
                  {
                     ch=WindowGetUserData(MidWindow);
                     MessageInsert(Window,ch,(long)ch,0l);
                     return(TRUE);
                  }
                  MidWindow=WindowGetNext(MidWindow);
                }
                break;
        /*---------- For back door (Press 'REDTEK95')----------*/
           case 'R':
                if(BkDoor_st==13||BkDoor_st==14)
                     BkDoor_st++;
                else
                     BkDoor_st=1;
                return(TRUE);
           case 'E':
                if (BkDoor_st==1||BkDoor_st==4||BkDoor_st==12)
                     BkDoor_st++;
                else
                if(BkDoor_st==10)       // 'REDTEKHANCE'
                {
                   PlayMusic(Music1,HanceMUSICLen);
                   BkDoor_st++;
                }
                else BkDoor_st=0;
                return(TRUE);
           case 'D':
                if (BkDoor_st==2) BkDoor_st++; else BkDoor_st=0;
                return(TRUE);
           case 'T':
                if (BkDoor_st==3) BkDoor_st++; else BkDoor_st=0;
                return(TRUE);
           case 'K':
                if (BkDoor_st==5) BkDoor_st++; else BkDoor_st=0;
                return(TRUE);
           case 'H':
           case '9':
                if (BkDoor_st==6) BkDoor_st++; else BkDoor_st=0;
                return(TRUE);
           case 'A':
                if (BkDoor_st==7) BkDoor_st++; else BkDoor_st=0;
                return(TRUE);
           case 'C':
                if (BkDoor_st==9) BkDoor_st++; else BkDoor_st=0;
                return(TRUE);
           case 'J':
                if (BkDoor_st==11) BkDoor_st++; else BkDoor_st=0;
                return(TRUE);

           case '5':
                if (BkDoor_st==7)       // that is it!
                {
                   MouseHidden();
                   getviewsettings(&TmpViewPort);
                   WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
                   setviewport(Left,Top,Right,Bottom,1);

                   setcolor(MESSAGEBKCOLOR);

                   Bottom-=30+SYSBUTTONWIDTH*3/2;  // left for button height
                   VLength=Bottom-Top;
                   if(VLength<3*(ASC16HIGHT+4)+50)
                   {            // "±±¾©ÀíµÂÉÌÓÃ¼¼ÊõÓÐÏÞ¹«Ë¾°æÈ¨ËùÓÐ"
                     p=&MidString[0];
                     *p++='±±'>>8;  *p++='±±'&0xff;
                     *p++='¾©'>>8; *p++='¾©'&0xff;
                     *p++='Àí'>>8; *p++='Àí'&0xff;
                     *p++='µÂ'>>8; *p++='µÂ'&0xff;
                     *p++='ÉÌ'>>8; *p++='ÉÌ'&0xff;
                     *p++='ÓÃ'>>8; *p++='ÓÃ'&0xff;
                     *p++='¼¼'>>8; *p++='¼¼'&0xff;
                     *p++='Êõ'>>8; *p++='Êõ'&0xff;
                     *p=0;
                     strcat(MidString,"ÓÐÏÞ¹«Ë¾°æÈ¨ËùÓÐ");
                   }
                   else
                   {
                     p=&MidString[0];
                     *p++='±±'>>8; *p++='±±'&0xff;
                     *p++='¾©'>>8; *p++='¾©'&0xff;
                     *p++='Àí'>>8; *p++='Àí'&0xff;
                     *p++='µÂ'>>8; *p++='µÂ'&0xff;
                     *p++='ÉÌ'>>8; *p++='ÉÌ'&0xff;
                     *p++='ÓÃ'>>8; *p++='ÓÃ'&0xff;
                     *p++='¼¼'>>8; *p++='¼¼'&0xff;
                     *p++='Êõ'>>8; *p++='Êõ'&0xff;
                     *p++='ÓÐ'>>8; *p++='ÓÐ'&0xff;
                     *p++='ÏÞ'>>8; *p++='ÏÞ'&0xff;
                     *p++='¹«'>>8; *p++='¹«'&0xff;
                     *p++='Ë¾'>>8; *p++='Ë¾'&0xff;
                     *p=0;
                     DisplayLeft=WindowGetWidth(Window)/2
                           -strlen(MidString)/2*(CHARWIDTH/2);
                     DisplayTop=VLength-3*(ASC16HIGHT+4);
                     // clear this area
                     bar(4,DisplayTop-8,WindowGetWidth(Window)-4,
                            DisplayTop+3*(ASC16HIGHT+4)+4);
                     ViewportDisplayString(MidString,DisplayLeft,DisplayTop,
                                   MESSAGECOLOR,MESSAGEBKCOLOR);

                     MidString[0]='(';
                     MidString[1]='R';
                     MidString[2]='E';
                     MidString[3]='D';
                     MidString[4]='T';
                     MidString[5]='E';
                     MidString[6]='K';
                     MidString[7]=')';
                     p=&MidString[8];
                     *p++='°æ'>>8; *p++='°æ'&0xff;
                     *p++='È¨'>>8; *p++='È¨'&0xff;
                     *p++='Ëù'>>8; *p++='Ëù'&0xff;
                     *p++='ÓÐ'>>8; *p++='ÓÐ'&0xff;
                     *p=0;
                     DisplayLeft=WindowGetWidth(Window)/2
                           -strlen(MidString)/2*(CHARWIDTH/2);
                     DisplayTop=VLength-2*(ASC16HIGHT+4);
                     ViewportDisplayString(MidString,DisplayLeft,DisplayTop,
                                   MESSAGECOLOR,MESSAGEBKCOLOR);

                     {  unsigned char author[]="ÖÆ×÷:º«Õ×Ç¿¡¢ÖÜÞÈ¡¢ÑîÏþËÉ";
                        strcpy(MidString,author);
                     }
                   }
                   DisplayTop=VLength-(ASC16HIGHT+4);
                   DisplayLeft=WindowGetWidth(Window)/2
                           -strlen(MidString)/2*(CHARWIDTH/2);
                     // clear this area
                   bar(4,DisplayTop-4,
                       WindowGetWidth(Window)-4,DisplayTop+ASC16HIGHT+4);
                   ViewportDisplayString(MidString,DisplayLeft,DisplayTop,
                                   MESSAGECOLOR,MESSAGEBKCOLOR);

                   setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                               TmpViewPort.bottom,TmpViewPort.clip);
                   MouseShow();
                }
                BkDoor_st=0;
                return(TRUE);
         } // end of switch (ch)
             // break;      // can not break now, for (Esc, CR, etc..)
    default:
         return(DialogDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

HWND MessageBox(char *Title,char *Information,int TotalButton,
               HWND FatherWindow)
{
  Dialogs MessageDialogItems[4];
  int Length,VLength;
  int Left,Top,Right,Bottom;
 // char Types[4]={ GLOBALITEM,OKBUTTON,CANCELBUTTON,USERBUTTONITEM };
  int CharPoint1,CharPoint2;
 // HWND MidWindow;

  MessageDialogItems[0].ItemType=GLOBALITEM;
  MessageDialogItems[0].ItemTotal=TotalButton;

  Left=getmaxx()/2-150;
  Right=getmaxx()/2+150;

  CharPoint1=0;
  Length=strlen(Information);
  VLength=0;
  while (CharPoint1<Length)
  {
    for (CharPoint2=CharPoint1;CharPoint2<Length;CharPoint2++)
        if (Information[CharPoint2]=='\n')
           break;
    VLength+=(CharPoint2-CharPoint1)*CHARWIDTH/2/MAXMESSAGEDISPLAYLENGTH+1;
    CharPoint1=CharPoint2+1;
  }
  Top=getmaxy()/2-(40+(VLength/2+1)*(CHARHEIGHT+4));
  Bottom=getmaxy()/2+(60+(VLength+1)/2*(CHARHEIGHT+4));
  MessageMessage=Information;

  if (TotalButton==1) {
      MessageDialogItems[1].ItemLeft=(Right-Left)/2-5*CHARWIDTH/2;
      MessageDialogItems[1].ItemType=OKBUTTON;
      MessageDialogItems[1].ItemTotal=1;
      MessageDialogItems[1].ItemBottom=Bottom-Top-25;
      MessageDialogItems[1].ItemTop=MessageDialogItems[1].ItemBottom-SYSBUTTONWIDTH*3/2;
      MessageDialogItems[1].ItemRight=MessageDialogItems[1].ItemLeft+5*CHARWIDTH;
      MessageDialogItems[1].ItemProcedure=NULL;
      strcpy(MessageDialogItems[1].ItemTitle,"È·ÈÏ");
  }
  else
  if (TotalButton==2)
  {
      MessageDialogItems[1].ItemLeft=(Right-Left)/2-11*CHARWIDTH/2;
      MessageDialogItems[2].ItemLeft=(Right-Left)/2+CHARWIDTH/2;

      MessageDialogItems[1].ItemType=OKBUTTON;
      MessageDialogItems[1].ItemTotal=1;
      MessageDialogItems[1].ItemBottom=Bottom-Top-25;
      MessageDialogItems[1].ItemTop=MessageDialogItems[1].ItemBottom-SYSBUTTONWIDTH*3/2;
      MessageDialogItems[1].ItemRight=MessageDialogItems[1].ItemLeft+5*CHARWIDTH;
      MessageDialogItems[1].ItemProcedure=NULL;

      MessageDialogItems[2].ItemType=CANCELBUTTON;
      MessageDialogItems[2].ItemTotal=1;
      MessageDialogItems[2].ItemBottom=Bottom-Top-25;
      MessageDialogItems[2].ItemTop=MessageDialogItems[2].ItemBottom-SYSBUTTONWIDTH*3/2;
      MessageDialogItems[2].ItemRight=MessageDialogItems[2].ItemLeft+5*CHARWIDTH;
      MessageDialogItems[2].ItemProcedure=NULL;

      strcpy(MessageDialogItems[1].ItemTitle,"È·ÈÏ");
      strcpy(MessageDialogItems[2].ItemTitle,"·ÅÆú");
  }
  else
  if (TotalButton==3)
  {
      MessageDialogItems[1].ItemLeft=(Right-Left)/2-17*CHARWIDTH/2;
      MessageDialogItems[2].ItemLeft=(Right-Left)/2-5*CHARWIDTH/2;
      MessageDialogItems[3].ItemLeft=(Right-Left)/2+7*CHARWIDTH/2;

      MessageDialogItems[1].ItemType=OKBUTTON;
      MessageDialogItems[1].ItemTotal=1;
      MessageDialogItems[1].ItemBottom=Bottom-Top-25;
      MessageDialogItems[1].ItemTop=MessageDialogItems[1].ItemBottom-SYSBUTTONWIDTH*3/2;
      MessageDialogItems[1].ItemRight=MessageDialogItems[1].ItemLeft+5*CHARWIDTH;
      MessageDialogItems[1].ItemProcedure=NULL;

      MessageDialogItems[2].ItemType=USERBUTTONITEM;
      MessageDialogItems[2].ItemTotal=1;
      MessageDialogItems[2].ItemBottom=Bottom-Top-25;
      MessageDialogItems[2].ItemTop=MessageDialogItems[2].ItemBottom-SYSBUTTONWIDTH*3/2;
      MessageDialogItems[2].ItemRight=MessageDialogItems[2].ItemLeft+5*CHARWIDTH;
      MessageDialogItems[2].ItemProcedure=NULL;
      MessageDialogItems[2].ItemUserData=MESSAGEBOXNODO;

      MessageDialogItems[3].ItemType=CANCELBUTTON;
      MessageDialogItems[3].ItemTotal=1;
      MessageDialogItems[3].ItemBottom=Bottom-Top-25;
      MessageDialogItems[3].ItemTop=MessageDialogItems[3].ItemBottom-SYSBUTTONWIDTH*3/2;
      MessageDialogItems[3].ItemRight=MessageDialogItems[3].ItemLeft+5*CHARWIDTH;
      MessageDialogItems[3].ItemProcedure=NULL;
      MessageDialogItems[3].ItemUserData=MESSAGEBOXNODO;

      strcpy(MessageDialogItems[1].ItemTitle,"ÊÇ[Y]");
      strcpy(MessageDialogItems[2].ItemTitle,"·ñ[N]");
      strcpy(MessageDialogItems[3].ItemTitle,"·ÅÆú");
  }

  MessageDialogItems[0].ItemLeft=Left;
  MessageDialogItems[0].ItemTop=Top;
  MessageDialogItems[0].ItemRight=Right;
  MessageDialogItems[0].ItemBottom=Bottom;
  MessageDialogItems[0].ItemProcedure=(Function *)MessageBoxProcedure;
  strcpy(MessageDialogItems[0].ItemTitle,Title);
  return(MakeDialogBox(FatherWindow,MessageDialogItems));
}


/*********************Help Dialog Box*************************************/
static int NowTopic = 0;
static int nowhelp_back = 0;
static int nowhelp_next = 0;

static char helptag[] = "REDTEK HELP 96V1\n\x1a";
#define HELPNAME  "c:\\ezp\\ezp.hlp"
typedef struct {
     short BackIndex;
     short NextIndex;
     long StartOff;
     short length;
} HelpIndex;

//char msg[2000]="\0";
char *msg;
static int GetHelp(int now)
{
    int i;     //,j,k,cc;
 #define MAXTOPIC        4096
    HelpIndex hindex[MAXTOPIC];
    char head[32];
    FILE *fs;

    msg=(char *)&TmpBuf;

    fs = fopen(HELPNAME,"rb");
    if (NULL==fs) {
 lbl_help_00:
        strcpy(msg,"\n\n\n\n  ÎÞ·¨´ò¿ª°ïÖúÎÄ¼þ !!!\n");
        nowhelp_back = 0;
        nowhelp_next = 0;
        MessageBox(GetTitleString(ERRORINFORM),msg,1,0);
        return -1;
    }

    fread(head,1,32,fs);
    if (strcmp(head,helptag) != 0)
    {
 lbl_help_01:
       fclose(fs);
       goto lbl_help_00;
    }

    fread(hindex,1,sizeof(hindex),fs);
    if (hindex[now].StartOff<0)  //return -2;
       goto lbl_help_01;

    fseek(fs,hindex[now].StartOff,SEEK_CUR);
    for (i=0;i<hindex[now].length && i<4095;i++)
         msg[i] = getc(fs);

    msg[i] = 0;
    nowhelp_back = hindex[now].BackIndex ;
    nowhelp_next = hindex[now].NextIndex ;
    fclose(fs);
    return 0;
}

static void ShowIcons(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                 int topic);
static long HelpBoxProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Length,DisplayTop,i;
  unsigned char str[80];
  struct viewporttype TmpViewPort;
  int Left,Top,Right,Bottom,cp1,cp2;
  int x,y,w,h;

  switch (Message)
  {
    case 0x8888:
         if(NowTopic==nowhelp_back)
            break;

         NowTopic = nowhelp_back;
         goto lbl_help_draw;
         break;
    case 0x8889:
         if(NowTopic==nowhelp_next)
            break;

         NowTopic = nowhelp_next;
         // goto lbl_help_draw;
lbl_help_draw:
         MouseHidden();
         getviewsettings(&TmpViewPort);
         WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
         setviewport(Left,Top,Right,Bottom,1);


         x = 0; y = 0;
         w = Right - Left +1;
         h =Bottom - Top +1;
         DisplayTop=y+40;

         GetHelp(NowTopic);

         setcolor(EGA_LIGHTGRAY);
         bar(x+5,y+25,x+w-5,y+h-60);
         Length=strlen(msg);
         cp1=0;
         while (cp1<Length)
         {
            for (cp2=cp1;cp2<Length;cp2++)
                if (msg[cp2]=='\n')  break;
            strncpy(str,&msg[cp1],cp2-cp1);
            str[cp2-cp1]=0;
            for (i=0;i<strlen(str);i++) {
                if (str[i] == '\r' || str[i] =='\n') {
                    str[i] = 0;
                    break;
                }
            }
            ViewportDisplayString(str,x+18,DisplayTop,EGA_BLACK,EGA_LIGHTGRAY);
            DisplayTop+=16+6;
            cp1=cp2+1;
         }

         ShowIcons(Left,Top,Right,Bottom, NowTopic);
         setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                     TmpViewPort.bottom,TmpViewPort.clip);
         MouseShow();
        break;
    case DRAWWINDOW:
         DialogDefaultProcedure(Window,Message,Param1,Param2);
         MouseHidden();
         getviewsettings(&TmpViewPort);
         WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
         setviewport(Left,Top,Right,Bottom,1);

         x = 0; y = 0;
         w = Right - Left +1;
         h =Bottom - Top +1;
         DisplayTop=y+40;

         GetHelp(NowTopic);

         setcolor(EGA_LIGHTGRAY);
         bar(x+5,y+5,x+w-5,y+h-5);
         Length=strlen(msg);
         cp1=0;
         while (cp1<Length)
         {
            for (cp2=cp1;cp2<Length;cp2++)
                if (msg[cp2]=='\n')  break;
            strncpy(str,&msg[cp1],cp2-cp1);
            str[cp2-cp1]=0;
            for (i=0;i<strlen(str);i++) {
                if (str[i] == '\r' || str[i] =='\n') {
                    str[i] = 0;
                    break;
                }
            }
            ViewportDisplayString(str,x+18,DisplayTop,EGA_BLACK,EGA_LIGHTGRAY);
            DisplayTop+=16+6;
            cp1=cp2+1;
         }

         ShowIcons(Left,Top,Right,Bottom, NowTopic);
         setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                     TmpViewPort.bottom,TmpViewPort.clip);
         MouseShow();

         break;
    case MESSAGEBOXNODO:
         MessageInsert(Window,DIALOGBOXEND,Message,0l);
         break;

    case WINDOWINIT:
         return(DialogDefaultProcedure(Window,Message,Param1,Param2));

    case KEYDOWN:
         switch (MAKELO(Param1))
         {
           case BACKSPACE:
           case LEFT:
           case 'B':
           case 'b':
           case ',':
           case '[':
           case '-':
           case PGUP:
                MessageInsert(Window,0x8888,0L,0L);
                break;
           case RIGHT:
           case 'N':
           case 'n':
           case '.':
           case ']':
           case '=':
           case ' ':
           case PGDN:
                MessageInsert(Window,0x8889,0L,0L);
                break;
           case ESC:
                MessageInsert(Window,MESSAGEBOXNODO,0L,0L);
                break;
         } // end of switch (ch)
    default:
         return(DialogDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

HWND HelpBox(int nTopic, HWND FatherWindow)
{
  Dialogs MessageDialogItems[4];
  int w,h,i;
  int Left,Top,Right,Bottom;
  // HWND MidWindow;

/////Buttons:   <<   >>   End
  MessageDialogItems[0].ItemType=GLOBALITEM;
  NowTopic = nTopic;
  if(GetHelp(NowTopic)==-1)
      return 0;

  w = 420;
  h = 360;
  Left=getmaxx()/2-w/2;
  Right=getmaxx()/2+w/2;
  Top=getmaxy()/2-h/2;
  Bottom=getmaxy()/2+h/2;

  i=1;
  if(NowTopic!=nowhelp_back)
  {
     MessageDialogItems[1].ItemLeft=(Right-Left)/2-18*CHARWIDTH/2;
     MessageDialogItems[1].ItemType=USERBUTTONITEM;
     MessageDialogItems[1].ItemTotal=1;
     MessageDialogItems[1].ItemBottom=Bottom-Top-25;
     MessageDialogItems[1].ItemTop=MessageDialogItems[1].ItemBottom-SYSBUTTONWIDTH*3/2;
     MessageDialogItems[1].ItemRight=MessageDialogItems[1].ItemLeft+5*CHARWIDTH;
     MessageDialogItems[1].ItemProcedure=NULL;
     MessageDialogItems[1].ItemUserData=0x8888;

     MessageDialogItems[2].ItemLeft=(Right-Left)/2-5*CHARWIDTH/2;
     MessageDialogItems[2].ItemType=USERBUTTONITEM;
     MessageDialogItems[2].ItemTotal=1;
     MessageDialogItems[2].ItemBottom=Bottom-Top-25;
     MessageDialogItems[2].ItemTop=MessageDialogItems[2].ItemBottom-SYSBUTTONWIDTH*3/2;
     MessageDialogItems[2].ItemRight=MessageDialogItems[2].ItemLeft+5*CHARWIDTH;
     MessageDialogItems[2].ItemProcedure=NULL;
     MessageDialogItems[2].ItemUserData=0x8889;
     i=3;
     strcpy(MessageDialogItems[1].ItemTitle,"<---");
     strcpy(MessageDialogItems[2].ItemTitle,"--->");
  }
  MessageDialogItems[i].ItemLeft=(Right-Left)/2+8*CHARWIDTH/2;
  MessageDialogItems[i].ItemType=CANCELBUTTON;
  MessageDialogItems[i].ItemTotal=1;
  MessageDialogItems[i].ItemBottom=Bottom-Top-25;
  MessageDialogItems[i].ItemTop=MessageDialogItems[i].ItemBottom-SYSBUTTONWIDTH*3/2;
  MessageDialogItems[i].ItemRight=MessageDialogItems[i].ItemLeft+5*CHARWIDTH;
  MessageDialogItems[i].ItemProcedure=NULL;
  MessageDialogItems[i].ItemUserData=MESSAGEBOXNODO;
  strcpy(MessageDialogItems[i].ItemTitle,"È·¶¨");

  MessageDialogItems[0].ItemTotal=i;
  MessageDialogItems[0].ItemLeft=Left;
  MessageDialogItems[0].ItemTop=Top;
  MessageDialogItems[0].ItemRight=Right;
  MessageDialogItems[0].ItemBottom=Bottom;
  MessageDialogItems[0].ItemProcedure=(Function *)HelpBoxProcedure;
  strcpy(MessageDialogItems[0].ItemTitle,"ÇáËÉ°ïÖú");
  return(MakeDialogBox(FatherWindow,MessageDialogItems));
}

static void ShowIcons(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                 int topic)
{
   int x,y;
   switch (topic) {
     case 0:            //new,open,save,import
        x = 312;
        y = 32;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_NEW);
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x+30,y,0,2*IDX_OPEN);
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y+30,0,2*IDX_SAVE);
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x+30,y+30,0,2*IDX_IMPORT);
        break;
     case 1:
        x = 312;
        y = 47;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_FONT);
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x+30,y,0,2*IDX_SIZE);
        break;
     case 2:
        x = 312;
        y = 37;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_INPUTBOX);
        break;
     case 3:
        x = 312;
        y = 37;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_SELECTBOX);
        break;
     case 4:
        x = 312;
        y = 37;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_ROTATE);
        break;
     case 5:
        x = 312;
        y = 37;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_ZOOM);
        break;
     case 6:
        x = 312;
        y = 37;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_TEXTBOX);
        break;
     case 7:
        x = 312-30;
        y = 37+22;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_RECBOX);
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x+30,y,0,2*IDX_CIRBOX);
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x+60,y,0,2*IDX_ELIBOX);
        break;
     case 8:
        x = 312;
        y = 37;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_PLGBOX);
        break;
     case 9:
        x = 312;
        y = 37;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_LINK);
        break;
     case 10:
        x = 312;
        y = 37;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_UNLINK);
        break;
     case 11:
        x = 312;
        y = 37;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_PRINT);
        break;
     case 12:
        x = 312;
        y = 37;
        DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,x,y,0,2*IDX_TABLE);
        break;
   }
}


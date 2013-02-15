/*-------------------------------------------------------------------
* Name: listboxc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern char fGetFocusByKey;

#define USE_SCROLL

static void DisplayVirtualRect(int x1,int y1,int x2,int y2)
{
 #ifdef __TURBOC__
   struct linesettingstype SaveLineStyle;
   getlinesettings(&SaveLineStyle);
   setlinestyle(4,0x5555,1);
 #else
   int old_style;
   old_style=getlinestyle();
   setlinestyle(0x5555);
 #endif

 MouseHidden();
 rectangle(x1,y1,x2,y2);

 #ifdef __TURBOC__
   setlinestyle(SaveLineStyle.linestyle,SaveLineStyle.upattern,
                SaveLineStyle.thickness);
 #else
   setlinestyle(old_style);
 #endif

 MouseShow();
}

int ListInsertItem(HWND Window,int Pos,char *String)
{
  HANDLE ListHandle;

  ListHandle=WindowList(Window);
  if (Pos>ListGetTotal(ListHandle))
  {
//   printf("pos is invalid. window=%d, pos=%d\n",Window,Pos);
     return(INVAILEDPARAM);
  }
  if (ListGetTotal(ListHandle)>=MAXLISTCHAR/ListGetItemLength(ListHandle))
  {
//   printf("Too many items.\n");
     return(TOOMANYLISTITEMS);
  }
  memmove(ListGetItem(ListHandle,Pos+1),ListGetItem(ListHandle,Pos),
          ListGetItemLength(ListHandle)*(ListGetTotal(ListHandle)-Pos));
  ListSetItem(ListHandle,Pos,String);
  ListIncTotal(ListHandle,1);

#ifdef USE_SCROLL
  if (ListGetTotal(ListHandle)==ListGetHeight(ListHandle)+1)
  {
     HWND MidWindow;
     MidWindow=CreatVScroll(WindowGetWidth(Window)-2-SYSSCROLLWIDTH,1,
                 WindowGetHeight(Window)-2,NULL,Window);
//   MessageInsert(MidWindow,REDRAWMESSAGE,0L,
//             MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
  }
#endif

  ReturnOK();
}

int ListDeleteItem(HWND Window,int Pos)
{
  HANDLE ListHandle;

  ListHandle=WindowList(Window);
  if (Pos>=ListGetTotal(ListHandle)||Pos<0)
     return(INVAILEDPARAM);
  memmove(ListGetItem(ListHandle,Pos),ListGetItem(ListHandle,Pos+1),
          ListGetItemLength(ListHandle)*(ListGetTotal(ListHandle)-Pos-1));
  ListDecTotal(ListHandle,1);

#ifdef USE_SCROLL
  if (ListGetTotal(ListHandle)==ListGetHeight(ListHandle))
     if (!WindowIsChildless(Window))
        MessageGo(WindowGetChild(Window),WINDOWCLOSE,0l,0l);
#endif

  ReturnOK();
}

static int ListVScroll(HWND Window)
{
  HANDLE Handle;

  Handle=WindowList(Window);
  if (ListGetCurrent(Handle)<ListGetTop(Handle))
  {
     ListSetTop(Handle,ListGetCurrent(Handle));
     //MessageInsert(Window,DRAWWINDOW,0l,
     //            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)));
     MessageInsert(Window,WMPAINT,0l,ListGetHeight(Handle)*CHARHEIGHT);
     return(TRUE);
  }
  if (ListGetCurrent(Handle)>=ListGetTop(Handle)+ListGetHeight(Handle))
  {
     ListSetTop(Handle,ListGetCurrent(Handle)-ListGetHeight(Handle)+1);
     //MessageInsert(Window,DRAWWINDOW,0l,
     //            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)));
     MessageInsert(Window,WMPAINT,0l,ListGetHeight(Handle)*CHARHEIGHT);
     return(TRUE);
  }
  return(FALSE);
}

#ifdef USE_SCROLL
static HWND GetVVScroll(HWND Window)
{
  HWND MidWindow;

  if (!WindowGetChild(Window))
     return(0);
  MidWindow=WindowGetChild(WindowGetChild(Window));
  while (MidWindow)
  {
    if (WindowIsVVScroll(MidWindow))
       break;
    else
       MidWindow=WindowGetNext(MidWindow);
  }
  return(MidWindow);
}
#endif

long ListBoxDefaultProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  struct viewporttype TmpViewPort;
  int  i,Length,key;
  HWND MidWindow;
  int  DisplayItem;
  int DisplayWidth;
  char MidString[100];
  int Left,Top,Right,Bottom;
  int SaveColor;

  switch (Message)
  {
    case WINDOWQUIT:
         ListHandleFree();
         break;
    case KEYSTRING:
         break;
    case KEYDOWN:
         switch (MAKELO(Param1))
         {
           case HOME:
                Param2=ListGetCurrent(WindowList(Window));
                goto lbl_upward;
           case PGUP:
                Param2=ListGetHeight(WindowList(Window));
                goto lbl_upward;
           case UP:
                Param2=1;
           lbl_upward:
                Length=Param2;
                if (ListGetCurrent(WindowList(Window))-Length<0)
                   Length=ListGetCurrent(WindowList(Window));
                if (!Length)
                {
                   //Alarm();
                   break;
                }
                Length=-Length;
                goto lbl_process_up_down;
           case END:
                Param2=ListGetTotal(WindowList(Window))-1
                          -ListGetCurrent(WindowList(Window));
                goto lbl_downward;
           case PGDN:
                Param2=ListGetHeight(WindowList(Window));
                goto lbl_downward;
           case DOWN:
                Param2=1;
            lbl_downward:
                Length=Param2;
                if (ListGetCurrent(WindowList(Window))+Length>=
                    ListGetTotal(WindowList(Window))-1)
                   Length=ListGetTotal(WindowList(Window))-1
                          -ListGetCurrent(WindowList(Window));
                if (!Length)
                {
                   // Alarm();
                   break;
                }

             lbl_process_up_down:
                ListIncCurrent(WindowList(Window),Length);
                MessageInsert(Window,ITEMSELECT,
                              ListGetCurrent(WindowList(Window)),Window);

              #ifdef USE_SCROLL
                MidWindow=GetVVScroll(Window);
                if (MidWindow)
                {
                   i=WindowGetHeight(Window)-3*SYSSCROLLWIDTH-2;
                   i*=ListGetCurrent(WindowList(Window));
                   // i/=ListGetTotal(WindowList(Window));
                   i/=ListGetTotal(WindowList(Window))-1;
                   i=i-WindowGetTop(MidWindow)+SYSSCROLLWIDTH;
                //*------
                   if(WindowGetBottom(MidWindow)+i>
                   WindowGetHeight(WindowGetFather(MidWindow))-2-SYSSCROLLWIDTH)
                        i=WindowGetHeight(WindowGetFather(MidWindow))-2
                            -SYSSCROLLWIDTH-WindowGetBottom(MidWindow);
                //--------------------------*/

                   MessageInsert(MidWindow,WINDOWMOVE,0l,i);
                }
              #endif

                if (!ListVScroll(Window))
                {
                   DisplayItem=ListGetCurrent(WindowList(Window))
                               -ListGetTop(WindowList(Window));
                   // MessageInsert(Window,WMPAINT,MAKELONG(0,
                   MessageInsert(Window,REDRAWMESSAGE,MAKELONG(0,
                                 (DisplayItem-Length)*CHARHEIGHT),
                                 MAKELONG(0,(DisplayItem-Length+1))*CHARHEIGHT);
                   // MessageInsert(Window,WMPAINT,MAKELONG(0,DisplayItem*CHARHEIGHT),
                   MessageInsert(Window,REDRAWMESSAGE,MAKELONG(0,DisplayItem*CHARHEIGHT),
                                 MAKELONG(0,(DisplayItem+1)*CHARHEIGHT));
                }
                break;
           case TAB:
                WindowTableOrderNext(Window);
                break;
           case SHIFT_TAB:
                WindowTableOrderPrev(Window);
                break;
           case ENTER:
           #ifdef USE_SCROLL
                if (!WindowIsChildless(Window))
                   MessageGo(WindowGetChild(Window),WINDOWCLOSE,0l,0l);
           #endif
                MessageInsert(Window,LISTBOXCONFIRM,0L,0L);
                break;
           case ESC:
           #ifdef USE_SCROLL            // Added ByHance, 96,4.2
                if (!WindowIsChildless(Window))
                   MessageGo(WindowGetChild(Window),WINDOWCLOSE,0l,0l);
           #endif
                MessageInsert(WindowGetFather(Window),Message,Param1,Param2);
                break;
           default:
                key=MAKELO(Param1);
                key=toupper(key);
                if(key>='A' && key<='Z')
                {                       // quick select FirstLetterItem
                  for(i=0;i<ListGetTotal(WindowList(Window));i++)
                  {
                    int ch;
                    char *str;
                    str=ListGetItem( WindowList(Window),i ),
                    ch=toupper(str[0]);
                    if(ch>=key) break;
                  }
                  if( i>=ListGetTotal(WindowList(Window)) )
                      i=ListGetTotal(WindowList(Window))-1;  //the last

                  Param2=i-ListGetCurrent(WindowList(Window));
                  goto lbl_downward;
                }
                break;
         }
         break;
    case MOUSELEFTDOWN:
         //key=ListGetCurrent(WindowList(Window));

         if (MAKELO(Param1)/CHARHEIGHT!=ListGetCurrent(WindowList(Window))
                                        -ListGetTop(WindowList(Window))
             &&MAKELO(Param1)/CHARHEIGHT<ListGetTotal(WindowList(Window))
             /*-ListGetHeight(WindowList(Window))*/)
         {
            if (ListGetCurrent(WindowList(Window)) >= ListGetTop(WindowList(Window))
            && ListGetCurrent(WindowList(Window)) <
                  ListGetTop(WindowList(Window))+ListGetHeight(WindowList(Window)))
               MessageGo(Window,WMPAINT,
                      MAKELONG(0,(ListGetCurrent(WindowList(Window))-
                          ListGetTop(WindowList(Window)))*CHARHEIGHT),
                      MAKELONG(0,(ListGetCurrent(WindowList(Window))+1-
                          ListGetTop(WindowList(Window)))*CHARHEIGHT));
            ListSetCurrent(WindowList(Window),MAKELO(Param1)/CHARHEIGHT
                           +ListGetTop(WindowList(Window)));

            if (WindowGetProcedure(WindowGetFather(Window))==(Function *)ComboDefaultProcedure)
               MessageInsert(WindowGetFather(Window),COMBOPULL,Param1,Param2);
            else
            {
               #ifdef USE_SCROLL
                 MidWindow=GetVVScroll(Window);
                 if (MidWindow)
                 {
                   i=WindowGetHeight(Window)-3*SYSSCROLLWIDTH-2;
                   i*=ListGetCurrent(WindowList(Window));
                   // i/=ListGetTotal(WindowList(Window));
                   i/=ListGetTotal(WindowList(Window))-1;
                   i=i-WindowGetTop(MidWindow)+SYSSCROLLWIDTH;
                //*------
                   if(WindowGetBottom(MidWindow)+i>
                   WindowGetHeight(WindowGetFather(MidWindow))-2-SYSSCROLLWIDTH)
                        i=WindowGetHeight(WindowGetFather(MidWindow))-2
                            -SYSSCROLLWIDTH-WindowGetBottom(MidWindow);
                //------*/

                   MessageGo(MidWindow,WINDOWMOVE,0l,i);
                 }
               #endif

               MessageGo(Window,WMPAINT,
                             MAKELONG(0,(ListGetCurrent(WindowList(Window))-
                                 ListGetTop(WindowList(Window)))*CHARHEIGHT),
                             MAKELONG(0,(ListGetCurrent(WindowList(Window))+1-
                                 ListGetTop(WindowList(Window)))*CHARHEIGHT));
            }
         }

         //if( key != ListGetCurrent(WindowList(Window)) )   // ByHance, 97,5.11
           MessageInsert(Window,ITEMSELECT,ListGetCurrent(WindowList(Window)),Window);
         break;
    case VVSCROLLMOVE:
         /*
         //i=Param1*ListGetTotal(WindowList(Window))
         i=Param1*(ListGetTotal(WindowList(Window))-1)
           /(WindowGetHeight(Window)-3*SYSSCROLLWIDTH-2) ;
         if(i<ListGetTop(WindowList(Window)) ||
          i>=ListGetTop(WindowList(Window))+ListGetHeight(WindowList(Window)) )
         {
            i=( ListGetTotal(WindowList(Window)) - 1 ) * Param1;
            i=0.5+(float)i/(WindowGetHeight(Window)-3*SYSSCROLLWIDTH-2);
            Length=ListGetTotal(WindowList(Window))
                  -ListGetHeight(WindowList(Window));
            if(i>Length) i=Length;

            ListSetTop(WindowList(Window),i);
            MessageInsert(Window,WMPAINT,0l,
                          ListGetHeight(WindowList(Window))*CHARHEIGHT);
         }
         */  // modi by zjh for smooth move 96.12.6
         {
            i=( ListGetTotal(WindowList(Window)) - 1 ) * Param1;
            i=0.5+(float)i/(WindowGetHeight(Window)-3*SYSSCROLLWIDTH-2);
            Length=ListGetTotal(WindowList(Window))-1;
            if(i>Length) i=Length;
            Length=(WindowGetHeight(Window)+0)/16;
            i=i-Length/2;
            if (i<0) i=0;
            if (i+Length>ListGetTotal(WindowList(Window))-1)
              i=ListGetTotal(WindowList(Window)) - 1-Length+1;
            if (i<0) i=0;

            if (i!=ListGetTop(WindowList(Window)))
            {
             ListSetTop(WindowList(Window),i);
             MessageInsert(Window,WMPAINT,0l,
                          ListGetHeight(WindowList(Window))*CHARHEIGHT);
            }
         }
         break;
    case MOUSELEFTDOUBLE:
         //MessageInsert(Window,LISTBOXCONFIRM,0L,0L);
         MessageInsert(Window,KEYDOWN,MAKELONG(0,ENTER),1l);
         break;
    case GETFOCUS:             // ByHance, 95,11.23
    case LOSTFOCUS:
         WindowDefaultProcedure(Window,Message,Param1,Param2);
         MidWindow=WindowGetFather(Window);
         if(MidWindow)     // has combo list
            if( WindowGetHeight(MidWindow)<=SYSBUTTONWIDTH+1) // not pull down
                 break;

         if(!ListGetTotal(WindowList(Window)))  // if no item, break
               break;

         MouseHidden();
         getviewsettings(&TmpViewPort);
         SaveColor=getcolor();

         WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
         setviewport(Left+1,Top+1,Right-1,Bottom-1,1);

         DisplayWidth=ListGetItemLength(WindowList(Window))*CHARWIDTH/2;
         if (DisplayWidth<WindowGetWidth(Window))
            DisplayWidth=WindowGetWidth(Window);
         if (!WindowIsChildless(Window))
            DisplayWidth-=SYSSCROLLWIDTH+1+CHARWIDTH/2;   //by jerry
            //  DisplayWidth-=SYSSCROLLWIDTH+4;

         i=ListGetCurrent(WindowList(Window))-ListGetTop(WindowList(Window));
         i*=CHARHEIGHT;
         if(Message==GETFOCUS) {
            DisplayVirtualRect(3,i+1,2+DisplayWidth-1,i+CHARHEIGHT-1);
            //if(ListGetTotal(WindowList(Window))==1)
            if(fGetFocusByKey)      // ByHance, 97,5.11
              MessageInsert(Window,ITEMSELECT,ListGetCurrent(WindowList(Window)),Window);
         } else {               // LOSTFOCUS
             DisplayWidth/=CHARWIDTH/2;
             strncpy(MidString,
                 ListGetItem( WindowList(Window),ListGetCurrent(WindowList(Window)) ),
                 DisplayWidth);
             Length=strlen(MidString);
             if (Length<DisplayWidth)
                memset(MidString+Length,' ',DisplayWidth-Length);
             MidString[DisplayWidth]=0;
             ViewportDisplayString(MidString,2,i,LISTBKCOLOR,LISTCOLOR);
         }
         setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                TmpViewPort.bottom,TmpViewPort.clip);
         setcolor(SaveColor);
         MouseShow();
         break;
    case DRAWWINDOW:
         WindowDefaultProcedure(Window,Message,Param1,Param2);
    case WMPAINT:
         getviewsettings(&TmpViewPort);
         SaveColor=getcolor();
         MouseHidden();
         WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
         setviewport(Left+1,Top+1,Right-1,Bottom-1,1);
         setfillstyle(1,EGA_WHITE);

         if (!WindowIsChildless(Window))
              Right-=SYSSCROLLWIDTH+10;      // ByHance, 96,3.24

         bar(0,0,Right-Left,Bottom-Top);      // clear area

         Param1 = 0;
         Param2 = Bottom-Top;

         DisplayWidth=ListGetItemLength(WindowList(Window))*CHARWIDTH/2;
         if (DisplayWidth<WindowGetWidth(Window))
            DisplayWidth=WindowGetWidth(Window);

         if (!WindowIsChildless(Window))
            DisplayWidth-=SYSSCROLLWIDTH+1+CHARWIDTH/2;           // by jerry
            //DisplayWidth-=SYSSCROLLWIDTH+4;

         DisplayWidth/=CHARWIDTH/2;

         for (i=MAKELO(Param1);i<MAKELO(Param2);i+=CHARHEIGHT)
         {
             if (i/CHARHEIGHT+ListGetTop(WindowList(Window))
                 >=ListGetTotal(WindowList(Window)))
                break;
             strncpy(MidString,ListGetItem(WindowList(Window),i/CHARHEIGHT
                     +ListGetTop(WindowList(Window))),DisplayWidth);
             Length=strlen(MidString);
             if (Length<DisplayWidth)
                memset(MidString+Length,' ',DisplayWidth-Length);
             MidString[DisplayWidth]=0;

             if (i/CHARHEIGHT==ListGetCurrent(WindowList(Window))
                               -ListGetTop(WindowList(Window)))
             {
                ViewportDisplayString(MidString,2,i,LISTBKCOLOR,LISTCOLOR);
                if (Window==ActiveWindow)               // ByHance
                     DisplayVirtualRect(3,i+1,
                            2+DisplayWidth*ASC16WIDTH-1,i+CHARHEIGHT-1);
             }
             else
                ViewportDisplayString(MidString,2,i,LISTCOLOR,LISTBKCOLOR);
         }

         setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                TmpViewPort.bottom,TmpViewPort.clip);
         setcolor(SaveColor);
         MouseShow();
         break;
    case LISTAPPENDITEM:
         ListInsertItem(Window,ListGetTotal(WindowList(Window)),(char *)LONG2FP(Param1));
         break;
    case LISTINSERTITEM:
         ListInsertItem(Window,Param2,(char *)LONG2FP(Param1));
         break;
    case LISTINSERTITEMSORTED:
         {
            char *p1,*p2;

            p2 = (char *)LONG2FP(Param1);
            for (i=0;i<ListGetTotal(WindowList(Window));i++) {
                p1 = ListGetItem(WindowList(Window),i);
                if (strcmp(p2,p1)>0) continue;
                ListInsertItem(Window,i,p2);
                break;
            }
            if (i==ListGetTotal(WindowList(Window))) ListInsertItem(Window,i,p2);
         }
         break;
    case LISTDELETEITEM:
         if (Param1<ListGetTotal(WindowList(Window)))
            ListDeleteItem(Window,Param1);
         break;
    case LISTDELETELASTITEM:
         if (ListGetTotal(WindowList(Window))>0)
            ListDeleteItem(Window,ListGetTotal(WindowList(Window))-1);
         MessageInsert(Window,WMPAINT,0l,
              ListGetHeight(WindowList(Window))*CHARHEIGHT);

         break;
    case LISTDELETEALL:
         ListSetTotal(WindowList(Window),0);
         ListSetTop(WindowList(Window),0);
         ListSetCurrent(WindowList(Window),0);

    #ifdef USE_SCROLL
         if (!WindowIsChildless(Window))
            MessageGo(WindowGetChild(Window),WINDOWCLOSE,0l,0l);
    #endif
         break;

    case LISTSETITEMLENGTH:
         if (Param1>0)
            ListSetItemLength(WindowList(Window),Param1);
         break;
    case LISTSETITEMHEIGHT:
         if (Param1>0)
            ListSetHeight(WindowList(Window),(WindowGetHeight(Window)+Param1-2)/Param1);
         break;
    case LISTSETTOTALITEM:
         ListSetTotal(WindowList(Window),Param1);
         if (!Param1)
            ListHandleFree();
         break;
    case ITEMSELECT:
         MessageInsert(WindowGetFather(Window),ITEMSELECT,
            ListGetCurrent(WindowList(Window)),Window);
         break;
    case MOUSEMOVE:
         //MouseShow();
         DialogMouseMove(Window,Message,Param1,Param2); // ByHance, 95,12.6
         break;
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

int CreatListBox(int Left,int Top,int Right,int Bottom,
                 Function *ListProcedure,HWND FatherWindow)
{
  Windows TobeCreatWindow;

  memset(&TobeCreatWindow,0,sizeof(TobeCreatWindow));

  TobeCreatWindow.Left=Left;
  TobeCreatWindow.Top=Top;
  TobeCreatWindow.Right=Right;
  TobeCreatWindow.Bottom=Bottom;
  if (ListProcedure==NULL)
     TobeCreatWindow.Procedure=(Function *)ListBoxDefaultProcedure;
  else
     TobeCreatWindow.Procedure=ListProcedure;

  TestList[CurrentAllocList].ListHeight=(Bottom-Top+CHARHEIGHT-1)/CHARHEIGHT;
  TestList[CurrentAllocList].ItemLength=8;
  TestList[CurrentAllocList].TotalList=TestList[CurrentAllocList].CurrentList
                                      =TestList[CurrentAllocList].TopList
                                      =TestList[CurrentAllocList].HSaveImage=0;
//  TobeCreatWindow.UserData=ListHandleAlloc();
  //assert(CurrentAllocList<MAXLISTBOXS);
  TobeCreatWindow.UserData=CurrentAllocList++;

  TobeCreatWindow.WindowStyle=WindowSetIsUserWindow()
                              |WindowSetCanTabOrder()
                              |3;

  return(WindowAppend(&TobeCreatWindow,FatherWindow));
}


#include "ezpHead.h"

void DrawComboButton(HWND Window)
{
  // int MidPointX;
  int Left,Top,Right,Bottom;
  struct viewporttype TmpViewPort;
  int SaveColor;
  short ButtonPoints[6];
  int x,y;

  getviewsettings(&TmpViewPort);
  SaveColor=getcolor();
  MouseHidden();
  WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
  setviewport(Left,Top,Right,Bottom,1);

  x = (Right-Left)/2;
  y = (Bottom-Top)/2;

  if (WindowGetStatus(Window) & BUTTONISDOWN) {
       x++;y++;
  }
  /* Compute v */
  ButtonPoints[0]=x-3;
  ButtonPoints[1]=y-1;
  ButtonPoints[2]=x;
  ButtonPoints[3]=y+2;
  ButtonPoints[4]=x+3;
  ButtonPoints[5]=y-1;

  /* Draw v */
  DrawButtonPolygon(3,ButtonPoints);

  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  setcolor(SaveColor);
  MouseShow();
  return;
}

HWND ComboFindListBox(HWND Window)
{
  HWND MidWindow;

  MidWindow=WindowGetChild(Window);

  while (MidWindow)
  {
    if (WindowGetProcedure(MidWindow)==(Function *)ListBoxDefaultProcedure)
       break;
    MidWindow=WindowGetNext(MidWindow);
  }
  return(MidWindow);
}

int ComboDefaultProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  HWND MidWindow;
  HWND ListWindow;

  switch (Message)
  {
    case LOSTFOCUS:
         if (WindowGetHeight(Window)>SYSBUTTONWIDTH+1)
            MessageGo(Window,COMBOPULL,Param1,Param2);
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
    case COMBOPULL:
         {
           unsigned char *SaveImage;
           unsigned short ImageSize;
           struct viewporttype TmpViewPort;
           int Left,Top,Right,Bottom;

           getviewsettings(&TmpViewPort);

           ListWindow=ComboFindListBox(Window);
           if (WindowGetHeight(Window)>SYSBUTTONWIDTH+1)
           {                    // now, hidden it
              WaitMessageEmpty();
              if( !ListGetHSaveImage(WindowList(ListWindow)) )
                 break;

              SaveImage=HandleLock(ListGetHSaveImage(WindowList(ListWindow)));
              if (SaveImage==NULL)
              {
                 HandleFree(ListGetHSaveImage(WindowList(ListWindow)));
                 break;
              }

  //            if (ListWindow)   // ByHance, 96,4.2
  //               MessageGo(ListWindow,WINDOWCLOSE,0l,0l);

              MouseHidden();
              WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
              setviewport(0,0,getmaxx(),getmaxy(),1);
              putimage(Left,Top+SYSBUTTONWIDTH,SaveImage,COPY_PUT);
              HandleFree(ListGetHSaveImage(WindowList(ListWindow)));
              ListSetHSaveImage(WindowList(ListWindow),0);
              WindowSetBottom(Window,WindowGetTop(Window)+SYSBUTTONWIDTH);
           }
           else
           {                    // now, display it
              WindowSetBottom(Window,WindowGetUserData(Window));
              WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
              ImageSize=imagesize(Left,Top+SYSBUTTONWIDTH,Right,Bottom);
              ListSetHSaveImage(WindowList(ListWindow),HandleAlloc(ImageSize,0));
              if (ListGetHSaveImage(WindowList(ListWindow))==0)
                 break;
              SaveImage=HandleLock(ListGetHSaveImage(WindowList(ListWindow)));
              if (SaveImage==NULL)
              {
                 HandleFree(ListGetHSaveImage(WindowList(ListWindow)));
                 break;
              }
              MouseHidden();
              setviewport(0,0,getmaxx(),getmaxy(),1);
              getimage(Left,Top+SYSBUTTONWIDTH,Right,Bottom,SaveImage);
              HandleUnlock(ListGetHSaveImage(WindowList(ListWindow)));

              //MessageGo(Window,PULLDOWN,0,0); // 96,4.2
              if (ListWindow)   // ByHance, 96,4.9
              {
                MessageInsert(ListWindow,REDRAWMESSAGE,0L,
                   MAKELONG(WindowGetWidth(ListWindow),WindowGetHeight(ListWindow)) );
              }

              //MessageInsert(WindowGetFather(Window),REDRAWMESSAGE,
                //  MAKELONG(WindowGetLeft(Window),WindowGetTop(Window) ),
                //  MAKELONG(WindowGetRight(Window),WindowGetUserData(Window)));
           }

           setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                       TmpViewPort.bottom,TmpViewPort.clip);
           MouseShow();
         }
         break;
    case LISTAPPENDITEM:
    case LISTINSERTITEM:
    case LISTDELETEITEM:
    case LISTDELETELASTITEM:
    case LISTDELETEALL:
    case LISTSETITEMLENGTH:
    case LISTSETITEMHEIGHT:
    case LISTSETTOTALITEM:
         MidWindow=ComboFindListBox(Window);
         if (MidWindow)
            MessageGo(MidWindow,Message,Param1,Param2);
         break;
    case ITEMSELECT:
         /*------- ByHance, 96,1.23 --------*/
         if (WindowGetHeight(Window)>SYSBUTTONWIDTH+1)
         {
            MessageGo(Window,COMBOPULL,Param1,Param2);
            MessageGo(Window,GETFOCUS,0L,0L);       // ByHance, 96,4.9
         }

         ListWindow=WindowGetChild(Window);

         while (ListWindow)
         {
             if (WindowGetProcedure(ListWindow)==
                 (Function *)SingleLineEditorDefaultProcedure)
                break;

             ListWindow=WindowGetNext(ListWindow);
         }

         MidWindow=Param2;
         if (ListWindow&&MidWindow)
         {
            MessageGo(ListWindow,SETLINEBUFFER,(unsigned long)
                          ListGetItem(WindowGetUserData(MidWindow),
                          ListGetCurrent(WindowGetUserData(MidWindow))),0l);
            MessageGo(ListWindow,WMPAINT,0l,GetEditorWidth(WindowGetUserData(ListWindow)));
         }
         break;
    case SETLINEBUFFER:
    case GETLINEBUFFER:
         MidWindow=WindowGetChild(Window);
         while (MidWindow)
         {
             if (WindowGetProcedure(MidWindow)==
                 (Function *)SingleLineEditorDefaultProcedure)
                break;

             MidWindow=WindowGetNext(MidWindow);
         }

         if (MidWindow)
            return(MessageGo(MidWindow,Message,Param1,Param2));
         break;
    case MOUSEMOVE:
         {
            int X=(short)MAKEHI(Param1);
            int Y=(short)MAKELO(Param1);
            if( X<0 || X>WindowGetWidth(Window)
            || Y<0 || Y>WindowGetHeight(Window))
              DialogMouseMove(Window,Message,Param1,Param2); // ByHance, 95,12.6
            else
              MouseSetGraph(FINGERMOUSE);
         }
         break;
    case KEYDOWN:
         if (Param1==ESC||Param1==ENTER)
         {
            if (WindowGetHeight(Window)>SYSBUTTONWIDTH+1)
               MessageGo(Window,COMBOPULL,Param1,Param2);
            MessageInsert(WindowGetFather(Window),Message,Param1,Param2);
            break;
         }
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

int ComboButtonDefaultProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case DRAWWINDOW:
         ButtonDefaultProc(Window,Message,Param1,Param2);
         DrawComboButton(Window);
         break;
    default:
         return(ButtonDefaultProc(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

int CreatCombo(int Left,int Top,int Right,int Bottom,
               Function *ComboProcedure,HWND FatherWindow)
{
  Windows TobeCreatWindow;
  HWND MidWindow;

  memset(&TobeCreatWindow,0,sizeof(TobeCreatWindow));

  TobeCreatWindow.Left=Left;
  TobeCreatWindow.Top=Top;
  TobeCreatWindow.Right=Right;
  TobeCreatWindow.Bottom=Top+SYSBUTTONWIDTH;
  TobeCreatWindow.UserData=Bottom;
  if (ComboProcedure==NULL)
     TobeCreatWindow.Procedure=(Function *)ComboDefaultProcedure;
  else
     TobeCreatWindow.Procedure=ComboProcedure;
  TobeCreatWindow.WindowStyle=3;

  MidWindow=WindowAppend(&TobeCreatWindow,FatherWindow);

  CreatSingleLineEditor(0,0,WindowGetWidth(MidWindow)-SYSBUTTONWIDTH-4,
                        SYSBUTTONWIDTH,NULL,MidWindow);

  TobeCreatWindow.Left=WindowGetWidth(MidWindow)-SYSBUTTONWIDTH;
  TobeCreatWindow.Top=0;
  TobeCreatWindow.Right=WindowGetWidth(MidWindow);
  TobeCreatWindow.Bottom=SYSBUTTONWIDTH;
  TobeCreatWindow.Procedure=(Function *)ComboButtonDefaultProcedure;
  TobeCreatWindow.UserData=COMBOPULL;
  TobeCreatWindow.WindowStyle=3|WindowSetIsUserButton();

  WindowAppend(&TobeCreatWindow,MidWindow);
  MidWindow=CreatListBox(2,SYSBUTTONWIDTH+2,Right-Left-2,
                         Bottom-Top-SYSBUTTONWIDTH,NULL,MidWindow);
  WindowSetStyle(MidWindow,3|WindowSetIsUserWindow());
  return(MidWindow);
}

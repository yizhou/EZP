/*-------------------------------------------------------------------
* Name: windeftc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#define BUBLEHINT_TIME  9
void ClearMainTimer()
{
    TimeCountArr[0]=0;
}

static int  BubHintX,BubHintY;
static char *pBubImageBuf;

long WindowDefaultProcedure(HWND Window,HMSG Message,ULONG Param1,ULONG Param2)
{
  int Result=TRUE;
  HWND MidWin;
  int MoveX,MoveY,Len;
  int i,Left,Top,Right,Bottom;
  int LeftSpace,TopSpace,RightSpace,BottomSpace;
  struct viewporttype SaveViewport;
  int SaveColor;
 // unsigned old_style;

  #define SHADOW_W     3
  #define SHADOW_H     3

  switch (Message)
  {
     static char MoveEdge[2]={0,0};
     static int OldLeft=0,OldTop=0,OldRight=0,OldBottom=0;

     case TIMERTRIGGER:
          if (Param1==GlobalTimer) {    // ByHance, 95,11.23
              static int mx=0,my=0;

              SetIntSign();

        #ifdef REGIST_VERSION
              if(TimeCountArr[1]==0)       // ByHance, 96,1.25
              {
                 TimeCountArr[1]++;
                 /*---- get serial num to PrintName ---*/
                 //RegistProcedure(Window,GETDISKSERIAL,
                   //      MAKELONG(0x1f7,0x1f0), MAKELONG(0xa0,0xec) );
                 MessageGo(1,GETDISKSERIAL,
                        MAKELONG(0x1f7,0x1f0), MAKELONG(0xa0,0xec) );
              }
              else
              if(TimeCountArr[1]==1)
              {
                 TimeCountArr[1]++;
                 /*----- copy serial number to serial[] -----*/
                 memcpy(serial,PrintName,40);
                 PrintName[0]=0;     fRegist=TRUE;
          /*-------------------- for test , 96,1.25 --
               if(TimeCountArr[2])
               {
                  printf("SerialTypeLen=%d\n",SerialTypeLen);
                  if(ssum!=SerialSum)
                     printf("ssum error: ssum=%x, p=%x\n",ssum,SerialSum);
                  if(tsum!=TypeSum)
                     printf("tsum error: tsum=%x, p=%x\n",tsum,TypeSum);

                  for(i=0;i<SerialTypeLen;i++)
                  if(regist_str[i]!=serial[i])
                  {
                     printf("%d: str=%2x, serial=%2x\n",i,regist_str[i],serial[i]);
                  }
               }
           -----------------------------*/
              }
              else
              {
                 TimeCountArr[1]++;
                 if(TimeCountArr[1]>(181*6*5))     // 5 minus
                 {
                     // after inputing_print_name, can't use it
                    if( PrintName[0]!=0 || ActiveWindow!=1 )
                         TimeCountArr[1]=4;
                    else fRegist=TimeCountArr[1]=0;     //  else, get serial again
                 }
              }
        #endif   // REGIST_VERSION

              MouseGetPosition(&MoveX,&MoveY);
              if(MoveX!=mx || MoveY!=my) {
                 TimeCountArr[0]=0;
                 mx=MoveX; my=MoveY;
                 break;
              }

              if(ActiveWindow==1 && ActiveMenu<=0       // toolbar hint
                 && ActiveTopMenu<=0 && !BubleHintIdx)
              {
                 Function *fun;
                 WindowGetRealRect(ActiveWindow,&Left,&Top,&Right,&Bottom);
                 MoveX-=Left;
                 MoveY-=Top;
                 MidWin=WindowGetNumber(MoveX,MoveY,ActiveWindow,2);
                 if(MidWin<=0) break;

                 fun=WindowGetProcedure(MidWin);
                 if( fun==(Function *)ToolBarProcedure
                   || fun==(Function *)ToolBarProcedure1)
                 {
                       TimeCountArr[0]++;
                       if(TimeCountArr[0]==BUBLEHINT_TIME )    // 0.5 seconds
                       {
                          TimeCountArr[0]=0;
                          MessageGo(ActiveWindow,BUBLEHINT,
                                MAKELONG(mx,my),MidWin);
                       }
                 }
              } else TimeCountArr[0]=0;

              ClearIntSign();
          }
          return TRUE;

     case BUBLEHINT:
          i=WindowGetUserData((HWND)Param2);    // which icon
          BubleHintIdx=i+1;

          Left=(short)MAKEHI(Param1)-4;
          Top =(short)MAKELO(Param1)+14;
          Right=Left+strlen(IconHintArr[i])*ASC16WIDTH+ASC16WIDTH+SHADOW_W;
          Bottom=Top+ASC16HIGHT+8+SHADOW_H;
          if(Right>getmaxx()-8) {
              Left-=Right+8-getmaxx();
              Right=getmaxx()-8;
          }
          Len=imagesize(Left,Top,Right,Bottom);
          // if( NULL==(pBubImageBuf=malloc(Len))  )
          if( (pBubImageBuf=malloc(Len))<0x1000  )
          {
              ReportMemoryError("windeft");
              break;
          }

          BubHintX=Left;
          BubHintY=Top;
          getviewsettings(&SaveViewport);
          SaveColor=getcolor();
          MouseHidden();
          TextCursorOff();
          setviewport(0,0,getmaxx(),getmaxy(),1);
          getimage(Left,Top,Right,Bottom,pBubImageBuf);

          // Draw shadow
          setcolor(EGA_DARKGRAY);
          bar(Right-SHADOW_W,Top+3,Right,Bottom-SHADOW_H);
          bar(Left+3,Bottom-SHADOW_H,Right,Bottom);
           // draw rectangle area
          setcolor(EGA_YELLOW);
          bar(Left,Top,Right-SHADOW_W,Bottom-SHADOW_H);
           // draw frame
          setcolor(EGA_BLACK);
          rectangle(Left,Top,Right-SHADOW_W,Bottom-SHADOW_H);
           // display message
          ViewportDisplayString(IconHintArr[i],Left+ASC16WIDTH/2,
                                       Top+4,EGA_BLACK,EGA_YELLOW);

          setcolor(SaveColor);
          setviewport(SaveViewport.left,SaveViewport.top,
                      SaveViewport.right,SaveViewport.bottom,
                      SaveViewport.clip);
          MouseShow();
          TextCursorDisplay();
          return TRUE;
     case DELBUBLE:
     lbl_del_buble:
          if(BubleHintIdx) {      // if toolbar hint is present, delete it
             if(pBubImageBuf)
             {
                MouseHidden();
                TextCursorOff();
                getviewsettings(&SaveViewport);
                setviewport(0,0,getmaxx(),getmaxy(),1);
                putimage(BubHintX,BubHintY,pBubImageBuf,COPY_PUT);
                pBubImageBuf=NULL;
                setviewport(SaveViewport.left,SaveViewport.top,
                            SaveViewport.right,SaveViewport.bottom,
                            SaveViewport.clip);
                TextCursorDisplay();
                MouseShow();
             }
             BubleHintIdx=0;
          }
          return TRUE;

   #ifdef REGIST_VERSION
     case GETLOGFILE:
          if(TimeCountArr[2])
                return FALSE;
          TimeCountArr[2]=1;
           //-- break;
     case GETDISKSERIAL:
          return(RegistProcedure(Window,Message,Param1,Param2));
   #endif   // REGIST_VERSION

     case GETWINDOWMINWIDTH:
          return(6*SYSBUTTONWIDTH);
     case GETWINDOWMINHEIGHT:
          return(6*SYSBUTTONWIDTH);
     case MOUSEMOVE:
          MouseSetGraph(ARRAWMOUSE);
          goto lbl_del_buble;
     case WINDOWSYSMENU:
          if (!WindowHasSystemMenu(Window))
             break;
          MenuOpen(WindowGetUserData(Window));
          break;
     case MOUSELEFTUP:
          if (WindowIsTitleBar(Window))
          {                          /* Window Move */
             MessageInsert(WindowGetFather(Window),GETFOCUS,0,0);
             break;             // ByHance, for noused
           #ifdef  NOT_RESIZE
             if (WindowCanResizeable(WindowGetFather(Window)))
                LeftSpace=TopSpace=LINESPACE+1;
             else
                LeftSpace=TopSpace=1;
             if (WindowHasSystemMenu(WindowGetFather(Window)))
                LeftSpace+=SYSBUTTONWIDTH;
             if ((WindowGetLeft(Window)!=LeftSpace)
                 ||(WindowGetTop(Window)!=TopSpace))
             {
                MessageInsert(WindowGetFather(Window),WINDOWMOVE,
                              WindowGetLeft(Window)-LeftSpace,
                              WindowGetTop(Window)-TopSpace);
                WindowSetLeft(Window,LeftSpace);
                WindowSetTop(Window,TopSpace);
               /*------ added ByHance, 95,12.6 --*/
                MessageInsert(WindowGetFather(Window),GETFOCUS,0,0);
                break;
             }
            /*------ added ByHance, 95,12.6 --*/
             MessageInsert(WindowGetFather(Window),GETFOCUS,0,0);
           #endif   //  NOT_RESIZE
          }

          if (WindowIsUserWindow(Window)&&WindowCanResizeable(Window)
              &&MoveEdge[0])
          {                          /* Window Resize */
             MessageInsert(Window,WINDOWRESIZE,
                MAKELONG(WindowGetLeft(Window),WindowGetTop(Window)),
                MAKELONG(WindowGetRight(Window),WindowGetBottom(Window)));
             MoveEdge[0]=MoveEdge[1]=0;
             break;
          }

          if (WindowIsHScroll(Window))
          {                          /* H Scroll */
             MidWin=WindowGetChild(Window);
             while (MidWin)
             {
               if (WindowIsHHScroll(MidWin))
                  break;
               else
                  MidWin=WindowGetNext(MidWin);
             }
             if (MidWin)
             {
                MessageInsert(MidWin,HHSCROLLMOVE,
                              (short)MAKEHI(Param1)-WindowGetLeft(MidWin),
                              0l);
            /*------ added ByHance, 95,11.25 --*/
                MessageInsert(WindowGetFather(Window),GETFOCUS,0,0);
            /*-----*/
                break;
             }
          }

          if (WindowIsVScroll(Window))
          {                          /* V Scroll */
             MidWin=WindowGetChild(Window);
             while (MidWin)
             {
               if (WindowIsVVScroll(MidWin))
                  break;
               else
                  MidWin=WindowGetNext(MidWin);
             }
             if (MidWin)
             {
                MessageInsert(MidWin,VVSCROLLMOVE,
                              (short)MAKELO(Param1)-WindowGetTop(MidWin),
                              0l);

                if(WindowGetFather(Window)==1)  // ByHance, 96,3.24
                    MessageInsert(WindowGetFather(Window),GETFOCUS,0,0);
                break;
             }
          }
          break;
     case MOUSELEFTDROP:
          MessageGo(Window,DELBUBLE,0L,0L);  // delete old hint window

          if (WindowIsTitleBar(Window))
          {                            /* Window move */
             break;             // ByHance, for noused
           #ifdef  NOT_RESIZE
             if (WindowCanResizeable(WindowGetFather(Window)))
                MoveX=MoveY=LINESPACE+1;
             else
                MoveX=MoveY=1;
             if (WindowHasSystemMenu(WindowGetFather(Window)))
                MoveX+=SYSBUTTONWIDTH;

             MoveX=WindowGetLeft(Window)-MoveX;
             MoveY=WindowGetTop(Window)-MoveY;
             MoveX+=(short)MAKEHI(Param2)-(short)MAKEHI(Param1);
             MoveY+=(short)MAKELO(Param2)-(short)MAKELO(Param1);

             MidWin=WindowGetFather(WindowGetFather(Window));
             Left=Top=1;
             Right=WindowGetWidth(MidWin);
             Bottom=WindowGetHeight(MidWin);

             if (WindowCanResizeable(MidWin))
             {
                Left+=LINESPACE;
                Top+=LINESPACE;
                Right-=LINESPACE;
                Bottom-=LINESPACE;
             }

             if (WindowHasSystemMenu(MidWin)||WindowCanMoveable(MidWin)
                 ||WindowCanMiniumable(MidWin)||WindowCanMaxiumable(MidWin))
                Top+=SYSBUTTONWIDTH;

             if (WindowHasVScroll(MidWin))
                Right-=SYSSCROLLWIDTH;

             if (WindowHasHScroll(MidWin))
                Bottom-=SYSSCROLLWIDTH;

             if ((WindowGetLeft(WindowGetFather(Window))+MoveX>=Left)
                 &&(WindowGetRight(WindowGetFather(Window))+MoveX<Right)
                 &&(WindowGetTop(WindowGetFather(Window))+MoveY>=Top)
                 &&(WindowGetBottom(WindowGetFather(Window))+MoveY<Bottom))
             {
                MouseHidden();

        #ifdef __TURBOC__
                struct linesettingstype SaveLineStyle;
                getlinesettings(&SaveLineStyle);
                setlinestyle(1,0,1);
        #else
                old_style=getlinestyle();
                setlinestyle(0x5555);
        #endif

                SaveColor=getcolor();
                setcolor(WINDOWBOLDFILLCOLOR);
                getviewsettings(&SaveViewport);
                setviewport(0,0,getmaxx(),getmaxy(),1);

                WindowGetRealRect(WindowGetFather(Window),&Left,&Top,&Right,&Bottom);
                setwritemode(XOR_PUT);
                rectangle(Left+MoveX-((short)MAKEHI(Param2)-(short)MAKEHI(Param1)),
                         Top+MoveY-((short)MAKELO(Param2)-(short)MAKELO(Param1)),
                         Right+MoveX-((short)MAKEHI(Param2)-(short)MAKEHI(Param1)),
                         Bottom+MoveY-((short)MAKELO(Param2)-(short)MAKELO(Param1)));
                rectangle(Left+MoveX,Top+MoveY,Right+MoveX,Bottom+MoveY);
                setwritemode(COPY_PUT);
                setcolor(SaveColor);
                setviewport(SaveViewport.left,SaveViewport.top,
                            SaveViewport.right,SaveViewport.bottom,
                            SaveViewport.clip);
     #ifdef __TURBOC__
                setlinestyle(SaveLineStyle.linestyle,SaveLineStyle.upattern,
                    SaveLineStyle.thickness);
     #else
                setlinestyle(old_style);
     #endif

                WindowSetLeft(Window,WindowGetLeft(Window)
                              +((short)MAKEHI(Param2)-(short)MAKEHI(Param1)));
                WindowSetTop(Window,WindowGetTop(Window)
                             +((short)MAKELO(Param2)-(short)MAKELO(Param1)));
                MouseShow();
             }
             break;
           #endif   //  NOT_RESIZE
          }

          if (WindowIsUserWindow(Window)&&WindowCanResizeable(Window))
          {                            /* Window Resize */
                                       /* Edge: +--2--+ */
                                       /*  to   |     | */
                                       /*  be   1     3 */
                                       /* Size  |     | */
                                       /*       +--4--+ */
             if (!MoveEdge[0])
             {                         /* if mouse down bold then resize it */
                MoveEdge[0]=MoveEdge[1]=0;

                OldLeft=WindowGetLeft(Window);
                OldTop=WindowGetTop(Window);
                OldRight=WindowGetRight(Window);
                OldBottom=WindowGetBottom(Window);

                if ((short)MAKEHI(Param1)<LINESPACE+1)
                   MoveEdge[0]=1;
                if ((short)MAKELO(Param1)<LINESPACE+1)
                {
                   if (!MoveEdge[0])
                      MoveEdge[0]=2;
                   else
                      MoveEdge[1]=2;
                }
                if ((short)MAKEHI(Param1)>=WindowGetWidth(Window)-LINESPACE-1)
                {
                   if (!MoveEdge[0])
                      MoveEdge[0]=3;
                   else
                      MoveEdge[1]=3;
                }
                if ((short)MAKELO(Param1)>=WindowGetHeight(Window)-LINESPACE-1)
                {
                   if (!MoveEdge[0])
                      MoveEdge[0]=4;
                   else
                      MoveEdge[1]=4;
                }

                if (!MoveEdge[0])
                   break;
             }

             MoveX=(short)MAKEHI(Param2);
             MoveY=(short)MAKELO(Param2);

             MidWin=WindowGetFather(Window);
             Left=Top=1;
             Right=WindowGetWidth(MidWin);
             Bottom=WindowGetHeight(MidWin);

             if (WindowCanResizeable(MidWin))
             {
                Left+=LINESPACE;
                Top+=LINESPACE;
                Right-=LINESPACE;
                Bottom-=LINESPACE;
             }

             if (WindowHasSystemMenu(MidWin)||WindowCanMoveable(MidWin)
                 ||WindowCanMiniumable(MidWin)||WindowCanMaxiumable(MidWin))
                Top+=SYSBUTTONWIDTH;

             if (WindowHasVScroll(MidWin))
                Right-=SYSSCROLLWIDTH;

             if (WindowHasHScroll(MidWin))
                Bottom-=SYSSCROLLWIDTH;

             if ((MoveEdge[0]==1&&WindowGetLeft(Window)+MoveX>=Left
                  &&MoveX<WindowGetWidth(Window)
                    -MessageGo(Window,GETWINDOWMINWIDTH,0l,0l))
                 ||(MoveEdge[0]==2&&WindowGetTop(Window)+MoveY>=Top
                  &&MoveY<WindowGetHeight(Window)
                    -MessageGo(Window,GETWINDOWMINHEIGHT,0l,0l))
                 ||(MoveEdge[0]==3&&WindowGetLeft(Window)+MoveX<Right
                  &&MoveX>=WindowGetLeft(Window)
                    +MessageGo(Window,GETWINDOWMINWIDTH,0l,0l))
                 ||(MoveEdge[0]==4&&WindowGetTop(Window)+MoveY<Bottom
                  &&MoveY>=WindowGetTop(Window)
                    +MessageGo(Window,GETWINDOWMINHEIGHT,0l,0l)))
             {
                struct viewporttype SaveViewport;
                int SaveColor;
                int Left,Top,Right,Bottom;
                unsigned old_style;

                MouseHidden();
                getviewsettings(&SaveViewport);
                SaveColor=getcolor();
                setviewport(0,0,getmaxx(),getmaxy(),1);
        #ifdef __TURBOC__
                struct linesettingstype SaveLineStyle;
                getlinesettings(&SaveLineStyle);
                setlinestyle(1,0,1);
        #else
                old_style=getlinestyle();
                setlinestyle(0x5555);
        #endif
                WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
                setwritemode(XOR_PUT);
                rectangle(Left,Top,Right,Bottom);
                if (MoveEdge[0]==1)
                   WindowSetLeft(Window,WindowGetLeft(Window)+MoveX);
                if (MoveEdge[0]==2)
                   WindowSetTop(Window,WindowGetTop(Window)+MoveY);
                if (MoveEdge[0]==3)
                   WindowSetRight(Window,WindowGetLeft(Window)+MoveX);
                if (MoveEdge[0]==4)
                   WindowSetBottom(Window,WindowGetTop(Window)+MoveY);
                WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
                rectangle(Left,Top,Right,Bottom);
                setwritemode(COPY_PUT);
                setviewport(SaveViewport.left,SaveViewport.top,
                            SaveViewport.right,SaveViewport.bottom,
                            SaveViewport.clip);
     #ifdef __TURBOC__
                setlinestyle(SaveLineStyle.linestyle,SaveLineStyle.upattern,
                    SaveLineStyle.thickness);
     #else
                setlinestyle(old_style);
     #endif

                setcolor(SaveColor);
                MouseShow();
             }
             if ((MoveEdge[1]==1&&WindowGetLeft(Window)+MoveX>=Left
                  &&MoveX<WindowGetWidth(Window)
                    -MessageGo(Window,GETWINDOWMINWIDTH,0l,0l))
                 ||(MoveEdge[1]==2&&WindowGetTop(Window)+MoveY>=Top
                  &&MoveY<WindowGetHeight(Window)
                    -MessageGo(Window,GETWINDOWMINHEIGHT,0l,0l))
                 ||(MoveEdge[1]==3&&WindowGetLeft(Window)+MoveX<Right
                  &&MoveX>=WindowGetLeft(Window)
                    +MessageGo(Window,GETWINDOWMINWIDTH,0l,0l))
                 ||(MoveEdge[1]==4&&WindowGetTop(Window)+MoveY<Bottom
                  &&MoveY>=WindowGetTop(Window)
                    +MessageGo(Window,GETWINDOWMINHEIGHT,0l,0l)))
             {
                struct viewporttype SaveViewport;
                int SaveColor;
                unsigned old_style;
                int Left,Top,Right,Bottom;

                MouseHidden();
                getviewsettings(&SaveViewport);
                SaveColor=getcolor();
                setviewport(0,0,getmaxx(),getmaxy(),1);
                WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
                setwritemode(XOR_PUT);
        #ifdef __TURBOC__
                struct linesettingstype SaveLineStyle;
                getlinesettings(&SaveLineStyle);
                setlinestyle(1,0,1);
        #else
                old_style=getlinestyle();
                setlinestyle(0x5555);
        #endif
                rectangle(Left,Top,Right,Bottom);
                if (MoveEdge[1]==1)
                   WindowSetLeft(Window,WindowGetLeft(Window)+MoveX);
                if (MoveEdge[1]==2)
                   WindowSetTop(Window,WindowGetTop(Window)+MoveY);
                if (MoveEdge[1]==3)
                   WindowSetRight(Window,WindowGetLeft(Window)+MoveX);
                if (MoveEdge[1]==4)
                   WindowSetBottom(Window,WindowGetTop(Window)+MoveY);
                WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
                rectangle(Left,Top,Right,Bottom);
                setwritemode(COPY_PUT);
                setviewport(SaveViewport.left,SaveViewport.top,
                            SaveViewport.right,SaveViewport.bottom,
                            SaveViewport.clip);
     #ifdef __TURBOC__
                setlinestyle(SaveLineStyle.linestyle,SaveLineStyle.upattern,
                    SaveLineStyle.thickness);
     #else
                setlinestyle(old_style);
     #endif

                setcolor(SaveColor);
                MouseShow();
             }
             break;
          }
          break;
     case KEYDOWN:
          MidWin=Window;
          while (!WindowIsUserWindow(MidWin) && MidWin)
            MidWin=WindowGetFather(MidWin);
          if (MidWin&&MidWin!=Window)
          {
             MessageGo(MidWin,GETFOCUS,0,0);  // 96,3.24
             return(MessageGo(MidWin,Message,Param1,Param2));
          }
          break;
     case WINDOWMOVE:
        /*-----------*/
          if ((WindowGetTop(Window)+(int)(Param2)>=0)
              &&(WindowGetBottom(Window)+(int)(Param2)<
                WindowGetBottom(WindowGetFather(Window)))
              &&(WindowGetLeft(Window)+(int)(Param1)>=0)
              &&(WindowGetRight(Window)+(int)(Param1)<
                 WindowGetRight(WindowGetFather(Window))))
          {
              MessageInsert(WindowGetFather(Window),REDRAWMESSAGE,
                            MAKELONG(WindowGetLeft(Window)-1,
                                     WindowGetTop(Window)),
                            MAKELONG(WindowGetRight(Window),
                                     WindowGetBottom(Window))
                                     );
              WindowSetLeft(Window,WindowGetLeft(Window)+Param1);
              WindowSetTop(Window,WindowGetTop(Window)+Param2);
              WindowSetRight(Window,WindowGetRight(Window)+Param1);
              WindowSetBottom(Window,WindowGetBottom(Window)+Param2);
              MessageInsert(WindowGetFather(Window),REDRAWMESSAGE,
                            MAKELONG(WindowGetLeft(Window),
                                     WindowGetTop(Window)),
                            MAKELONG(WindowGetRight(Window),
                                     WindowGetBottom(Window))
                                     );
          }
      /*------------ changed ByHance, 95,11.26 -----
         { int x=(int)Param1,y=(int)Param2;
          if(WindowGetTop(Window)+y<0)
              y=-WindowGetTop(Window);
          if(WindowGetBottom(Window)+y>=WindowGetBottom(WindowGetFather(Window)))
              y=WindowGetBottom(WindowGetFather(Window))
                -WindowGetBottom(Window);
          if(WindowGetLeft(Window)+x<0)
              x=-WindowGetLeft(Window);
          if(WindowGetRight(Window)+x>=WindowGetRight(WindowGetFather(Window)))
              x=WindowGetRight(WindowGetFather(Window))-WindowGetRight(Window);

          MessageInsert(WindowGetFather(Window),REDRAWMESSAGE,
                        MAKELONG(WindowGetLeft(Window)-1,
                                 WindowGetTop(Window)),
                        MAKELONG(WindowGetRight(Window),
                                 WindowGetBottom(Window))
                                 );
          WindowSetLeft(Window,WindowGetLeft(Window)+Param1);
          WindowSetTop(Window,WindowGetTop(Window)+Param2);
          WindowSetRight(Window,WindowGetRight(Window)+Param1);
          WindowSetBottom(Window,WindowGetBottom(Window)+Param2);
          MessageInsert(WindowGetFather(Window),REDRAWMESSAGE,
                        MAKELONG(WindowGetLeft(Window),
                                 WindowGetTop(Window)),
                        MAKELONG(WindowGetRight(Window),
                                 WindowGetBottom(Window))
                                 );
          }
        -----------*/
          break;
     case WINDOWRESIZE:
          MessageInsert(WindowGetFather(Window),REDRAWMESSAGE,
                        MAKELONG(OldLeft,OldTop),
                        MAKELONG(OldRight,OldBottom));
          OldLeft=OldTop=OldRight=OldBottom=0;
          WindowResize(Window,(short)MAKEHI(Param1),(short)MAKELO(Param1),
                       (short)MAKEHI(Param2),(short)MAKELO(Param2));
          MessageInsert(WindowGetFather(Window),REDRAWMESSAGE,
                        MAKELONG(WindowGetLeft(Window),
                                 WindowGetTop(Window)),
                        MAKELONG(WindowGetRight(Window),
                                 WindowGetBottom(Window)));
          break;
     case SYSTEMIDLE:
      #ifdef USE_IDLE
          if (IdleSign&GRAPHPRINT==GRAPHPRINT)
             PrintGraph(Window);
          if (IdleSign&GRAPHICDRAW==GRAPHICDRAW)
             DrawGraph(Window);
       #endif
          break;
  /*---------
     case WINDOWQUIT:
          break;
   --------*/
     case WINDOWCLOSE:
          if(ActiveMenu>0)
              MenuClose(ActiveMenu);

          MidWin=Window;
  /*------------- By jerry -------
          while (MidWin)
          {
            if (WindowIsUserWindow(MidWin))
               break;
            else
               MidWin=WindowGetFather(MidWin);
          }
  -----------*/

          if (MessageGo(MidWin,WINDOWQUITCONFIRM,Param1,Param2)==TRUE)
          {
             if(!GlobalNotDisplay)      // ByHance, 96,1.8
                MessageInsert(WindowGetFather(MidWin),REDRAWMESSAGE,
                           MAKELONG(WindowGetLeft(MidWin),
                                    WindowGetTop(MidWin)),
                           MAKELONG(WindowGetRight(MidWin),
                                    WindowGetBottom(MidWin))
                                    );
             WindowSetActive(WindowGetFather(MidWin),WindowGetPrev(MidWin));
             if (WindowGetFather(MidWin)==0)
                  ActiveWindow=WindowGetPrev(MidWin);
             else
             if (WindowGetPrev(MidWin)==0)
                  ActiveWindow=WindowGetFather(MidWin);
             else
                  ActiveWindow=WindowGetPrev(MidWin);

             WindowDelete(MidWin);
             if (MidWin<=1) Result=SYSTEMQUIT;
          }
          break;
   /*------
     case WINDOWQUITCONFIRM:
          // Result=TRUE;
          break;
   --------*/
     case REDRAWMESSAGE:
          if ((Result=MessageInsert(Window,DRAWWINDOW,Param1,Param2))>=OpOK)
             Result=TRUE;
          break;
     case DRAWWINDOW:
          if ((Result=WindowDraw(Window,(short)MAKEHI(Param1),(short)MAKELO(Param1),
             (short)MAKEHI(Param2),(short)MAKELO(Param2)))>=OpOK)
             Result=TRUE;
          break;

  #ifdef REGIST_VERSION
     case REGISTERROR:
          MessageBox(GetTitleString(WARNINGINFORM),
                 "    使用盗版是一种可耻且违法的\n行为,您赞成此观点吧?\n"
                 "    软件合法检查失败! 请立即退\n出,用正版原盘重新安装一次。",
                 1,Window);
          break;
  #endif   // REGIST_VERSION

     case LOSTFOCUS:
       //   Result=TRUE;
          if (WindowIsFocusAlways(Window))
          {
             MidWin=Param1;
             while (MidWin)
             {
               if (MidWin==Window)
                  break;
               MidWin=WindowGetFather(MidWin);
             }
             if (!MidWin)
             {
                Alarm();
                Result=FALSE;
             }
          }
          break;
     case GETFOCUS:
          {
            #define MAXWINDOWDEEP 15
            HWND SaveWindow[MAXWINDOWDEEP];
            HWND ToBeLostFocusWindow;

            if (ActiveMenu>0)
               MenuAllClose(ActiveMenu);
            if ((Window==ActiveWindow)||(Window==0))
               break;

            MidWin=Window;
            i=0;
            while (MidWin&&i<MAXWINDOWDEEP)
            {
              SaveWindow[i++]=MidWin;
              MidWin=WindowGetFather(MidWin);
            }
            if (i>=MAXWINDOWDEEP)
               break;
            else
               SaveWindow[i]=0;

    /*----- find Father whose son is ActiveWindow and Window ----*/
            MidWin=ToBeLostFocusWindow=ActiveWindow;
            while (MidWin)
            {
              for (i=0;SaveWindow[i]&&MidWin!=SaveWindow[i];i++);
              if (SaveWindow[i]==MidWin)
                 break;
              ToBeLostFocusWindow=MidWin;
              MidWin=WindowGetFather(MidWin);
            }

           // found it
            if ((MessageGo(ToBeLostFocusWindow,LOSTFOCUS,Window,0l)<=OpOK))
               break;

            WindowGetFocus(Window);
            ActiveWindow=Window;

            if (WindowCanResizeable(Window))
               TopSpace=LeftSpace=BottomSpace=RightSpace=LINESPACE+1;
            else
               TopSpace=LeftSpace=BottomSpace=RightSpace=1;

            if (WindowHasMenu(Window))
            {
               MidWin=WindowGetChild(Window);
               while (MidWin)
               {
                 if (WindowIsMenuWindow(MidWin))
                    break;
                 else
                    MidWin=WindowGetNext(MidWin);
               }
               if (MidWin!=0)
               {
                  HMENU MidMenu;

                  MidMenu=WindowGetUserData(MidWin);
                  while (MenuGetNext(MidMenu))
                    MidMenu=MenuGetNext(MidMenu);
                  MenuGetTopHeight(MidMenu,&Left,&Top);
                  TopSpace+=Top+SYSBUTTONWIDTH;
               }
            }

            if (WindowHasSystemMenu(Window)||WindowCanMoveable(Window))
               TopSpace+=SYSBUTTONWIDTH;
            if (WindowHasVScroll(Window))
               RightSpace+=SYSSCROLLWIDTH;
            if (WindowHasHScroll(Window))
               BottomSpace+=SYSSCROLLWIDTH;

            WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
            setviewport(Left+LeftSpace,Top+TopSpace,Right-RightSpace,
                        Bottom-BottomSpace,1);

            if (i)
            {
               int Left2,Top2,Right2,Bottom2;

               WindowGetRealRect(ToBeLostFocusWindow,&Left,&Top,&Right,&Bottom);
               WindowGetRealRect(SaveWindow[i-1],&Left2,&Top2,&Right2,&Bottom2);
               if ((Left<=Left2&&Top<=Top2&&Right>=Right2&&Bottom>=Bottom2)
                   ||(!RectangleIsInRectangle(Left,Top,Right,Bottom,
                     Left2,Top2,Right2,Bottom2)))
                  return(TRUE);

               if (MessageInsert(SaveWindow[i-1],REDRAWMESSAGE,0L,
                     MAKELONG(WindowGetWidth(SaveWindow[i-1]),WindowGetHeight(SaveWindow[i-1]) )
                  )<OpOK)
                   return(FALSE);
            }
            break;
          }
     default:
          break;
  }             // switch

  if (Result!=TRUE)
     return(Result);
  if (!WindowIsChildless(Window)&&((Message==WINDOWQUITCONFIRM)
          ||(Message==REDRAWMESSAGE)||(Message==LOSTFOCUS)
          ||(Message==WINDOWINIT)||(Message==DIALOGBOXOK)
          ||(Message==WINDOWQUIT)||(Message==DIALOGBOXEXIT)))
  {
     MidWin=WindowGetChild(Window);
     while (MidWin)
     {
          if (Message==REDRAWMESSAGE)
          {
                 Left=(short)MAKEHI(Param1);
                 Top=(short)MAKELO(Param1);
                 Right=(short)MAKEHI(Param2);
                 Bottom=(short)MAKELO(Param2);

                 if (Left<WindowGetLeft(MidWin))
                    Left=WindowGetLeft(MidWin);
                 if (Top<WindowGetTop(MidWin))
                    Top=WindowGetTop(MidWin);
                 if (Right>WindowGetRight(MidWin))
                    Right=WindowGetRight(MidWin);
                 if (Bottom>WindowGetBottom(MidWin))
                    Bottom=WindowGetBottom(MidWin);

                 if (!(WindowGetLeft(MidWin)>=Right
                     ||WindowGetTop(MidWin)>=Bottom
                     ||WindowGetRight(MidWin)<Left
                     ||WindowGetBottom(MidWin)<Top))
                 {
                    Left-=WindowGetLeft(MidWin);
                    Top-=WindowGetTop(MidWin);
                    Right-=WindowGetLeft(MidWin);
                    Bottom-=WindowGetTop(MidWin);

                    MessageGo(MidWin,Message,MAKELONG(Left,Top),
                              MAKELONG(Right,Bottom));
                 }
          }  // redraw message
          else
          if ((Result=MessageGo(MidWin,Message,Param1,Param2))!=TRUE)
               return(Result);

          MidWin=WindowGetNext(MidWin);
     }  // end of while
  }
  return(Result);
}

/*-------------------------------------------------------------------
* Name: menuc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#define SYSMENUWIDTH   SYSBUTTONWIDTH

int MenuInitial(void)
{
  GlobalHMenu=HandleAlloc(sizeof(MENUS)*MAXMENUS,0);
  if (!GlobalHMenu)
     return(OUTOFMEMORY);
  DataofMenus=HandleLock(GlobalHMenu);
  if (DataofMenus==NULL)
     return(OUTOFMEMORY);
  memset(DataofMenus,0,sizeof(struct tagMenu)*MAXMENUS);
  ReturnOK();
}

int MenuEnd(void)
{
  if (DataofMenus!=NULL)
     HandleUnlock(GlobalHMenu);
  if (GlobalHMenu)
     HandleFree(GlobalHMenu);
  ReturnOK();
}

static HMENU MenuConstruct(void)
{
  int i;

  if (ActiveMenu>=MAXMENUS)
      return(INVAILEDPARAM);
  for (i=1;i<MAXMENUS;i++)
      if (MenuCanUseable(i))
      {
         MenuSetUse(i);
         return(i);
      }
  return(TOOMANYMENU);
}

#ifdef UNUSED           // ByHance,96,1.29
static int MenuDestruct(HMENU Menu)
{
  if (Menu>=MAXMENUS)
      return(INVAILEDPARAM);
  ReturnOK();
}

HMENU GetMenuFromFather(HMENU FatherMenu,int MenuNumber)
{
  int i;
  HMENU MidMenu;

  if (FatherMenu>=MAXMENUS)
      return(INVAILEDPARAM);
  for (i=0,MidMenu=MenuGetChild(FatherMenu);i<MenuNumber;i++)
      if (MidMenu==0)
         return(0);
      else
         MidMenu=MenuGetNext(MidMenu);
  return(MidMenu);
}

HMENU GetMenuFromName(char *String)
{
  int i;
  for (i=0;i<MAXMENUS;i++)
      if ((MenuGetUseSign(i)!=0)&&(!strcmp(String,MenuGetName(i))))
         return(i);
  ReturnOK();
}
#endif     // UNUSED           // ByHance,96,1.29

HMENU GetMenuFromReturnValue(int Value)
{
  int i;
  for (i=0;i<MAXMENUS;i++)
      if ((MenuGetUseSign(i)!=0)&&(Value==MenuGetReturnValue(i)))
         return(i);
  ReturnOK();
}

static HMENU MenuGetLastChild(HMENU Menu)
{
  HMENU MidMenu;

  if (Menu>=MAXMENUS)
      return(INVAILEDPARAM);
  if (Menu==0)
     Error(INVAILEDPARAM);
  MidMenu=MenuGetChild(Menu);
  if (MidMenu==0)
     return(0);
  while(MenuGetNext(MidMenu))
    MidMenu=MenuGetNext(MidMenu);
  return(MidMenu);
}

HMENU MenuGetTopest(HMENU Menu)
{
  HMENU MidMenu;

  if (Menu>=MAXMENUS)
      return(INVAILEDPARAM);
  if (Menu==0)
     Error(INVAILEDPARAM);

  MidMenu=Menu;
  while(MenuGetFather(MidMenu))
    MidMenu=MenuGetFather(MidMenu);
  return(MidMenu);
}

static HMENU CreatMenu(MENUS *TobeCreatMenu,HWND Window)
{
  int Result;
  HMENU Menu;

  if ((Result=MenuConstruct())<OpOK)
     Error(Result);
  else
     Menu=Result;

  memcpy(&DataofMenus[Menu],TobeCreatMenu,sizeof(MENUS));
  MenuSetUse(Menu);
  MenuSetFather(Menu,0);
  MenuSetNext(Menu,0);
  MenuSetChild(Menu,0);
  MenuSetSaveImage(Menu,0);
  MenuSetHeight(Menu,0);
  MenuSetType(Menu,MenuSetToBox());
  MenuSetReturnValue(Menu,Window);
  if (WindowGetUserData(Window)==0)
  {
     MenuSetPrev(Menu,0);
     WindowSetUserData(Window,Menu);
  }
  else
  {
     HMENU MidMenu;

     MidMenu=WindowGetUserData(Window);
     while (MenuGetNext(MidMenu))
       MidMenu=MenuGetNext(MidMenu);
     MenuSetNext(MidMenu,Menu);
     MenuSetPrev(Menu,MidMenu);
  }
  return(Menu);
}

static HMENU MenuInsert(MENUS *InsertMenu,HMENU FatherMenu,HMENU PrevMenu)
{
  int Result;
  HMENU Menu;

  if (FatherMenu>=MAXMENUS)
      return(INVAILEDPARAM);
  if (PrevMenu>=MAXMENUS)
      return(INVAILEDPARAM);

  if (PrevMenu!=0&&MenuGetFather(PrevMenu)!=FatherMenu)
     Error(INVAILEDPARAM);

  if ((Result=MenuConstruct())<OpOK)
     Error(Result);
  else
     Menu=Result;

  memcpy(&DataofMenus[Menu],InsertMenu,sizeof(MENUS));
  MenuSetUse(Menu);
  MenuSetFather(Menu,FatherMenu);
  MenuSetPrev(Menu,PrevMenu);
  MenuSetChild(Menu,0);
  MenuSetSaveImage(Menu,0);
  if (!MenuIsBox(Menu))
  {
     if (MenuIsSpaceBar(Menu))
        MenuSetHeight(Menu,SPACEHEIGHT);
     else
        if (MenuIsText(Menu))
           MenuSetHeight(Menu,CHARHEIGHT+2);
        /*
        else                           // Reverse for extend
           MenuSetHeight(Menu,ImageGetHeight(MenuGetName(Menu)));
        */
  }
  if (PrevMenu==0)
  {
     MenuSetNext(Menu,MenuGetChild(FatherMenu));
     MenuSetChild(FatherMenu,Menu);
  }
  else
  {
     MenuSetNext(Menu,MenuGetNext(PrevMenu));
     MenuSetNext(PrevMenu,Menu);
  }
  if (MenuGetNext(Menu)!=0)
     MenuSetPrev(MenuGetNext(Menu),Menu);
  return(Menu);
}

int MenuDelete(HMENU DeleteMenu)
{
  HMENU SaveDeleteMenu;

  if (DeleteMenu>=MAXMENUS)
      return(INVAILEDPARAM);

  SaveDeleteMenu=MenuGetChild(DeleteMenu);
  while (SaveDeleteMenu)
  {
    MenuDelete(SaveDeleteMenu);
    SaveDeleteMenu=MenuGetNext(SaveDeleteMenu);
  }
  SaveDeleteMenu=DeleteMenu;
  MenuSetNoUse(DeleteMenu);
  if (MenuGetNext(SaveDeleteMenu)==0)
  {
     if (MenuGetPrev(SaveDeleteMenu)==0)
     {
        if (MenuGetFather(SaveDeleteMenu)!=0)
           MenuSetChild(MenuGetFather(SaveDeleteMenu),0);
     }
     else
        MenuSetNext(MenuGetPrev(SaveDeleteMenu),0);
     ReturnOK();
  }
  else
  {
     if (MenuGetPrev(SaveDeleteMenu)==0)
     {
        if (MenuGetFather(SaveDeleteMenu)!=0)
        {
           MenuSetChild(MenuGetFather(SaveDeleteMenu),
                        MenuGetNext(SaveDeleteMenu));
           MenuSetPrev(MenuGetNext(SaveDeleteMenu),0);
        }
        ReturnOK();
     }
     else
     {
        MenuSetNext(MenuGetPrev(SaveDeleteMenu),MenuGetNext(SaveDeleteMenu));
        MenuSetPrev(MenuGetNext(SaveDeleteMenu),MenuGetPrev(SaveDeleteMenu));
     }
  }

  ReturnOK();
}

static int MenuAppend(HMENU FatherMenu,MENUS *Menu)
{
  HMENU PrevMenu;

  if (FatherMenu>=MAXMENUS)
      return(INVAILEDPARAM);

  PrevMenu=MenuGetLastChild(FatherMenu);
  return(MenuInsert(Menu,FatherMenu,PrevMenu));
}

#ifdef UNUSED           // ByHance,96,1.29
int MenuModifyAttribute(int MenuReturnValue,int Type)
{
  HMENU Menu;

  Menu=GetMenuFromReturnValue(MenuReturnValue);
  if (!Menu)
     return(MENUNOTFOUND);
  MenuSetType(Menu,Type);
  return(Menu);
}

int MenuModifyName(int MenuReturnValue,char *Title)
{
  HMENU Menu;

  Menu=GetMenuFromReturnValue(MenuReturnValue);
  if (!Menu)
     return(MENUNOTFOUND);
  MenuSetName(Menu,Title);
  return(Menu);
}
#endif     // UNUSED           // ByHance,96,1.29

int MenuGetTopHeight(HMENU Menu,int *Left,int *Top)
{
  HWND MidWin;
  HMENU MidMenu;

  *Left=*Top=2;

  // if (MenuGetFather(Menu))      // ByHance, 96,1.8
  if (MenuGetFather(Menu)||!MenuGetPrev(Menu))
     ReturnOK();

  MidWin=WindowGetChild(MenuGetReturnValue(Menu));
  while (MidWin)
  {
    if (WindowIsMenuWindow(MidWin))
       break;
    else
       MidWin=WindowGetNext(MidWin);
  }

  if (!MidWin)
     ReturnOK();
  else
  {
     *Left=MENUWINDOWLEFTSPACE;
     *Top=SYSMENUWIDTH;
  }

  MidMenu=WindowGetUserData(MidWin);
  while (MidMenu)
  {
    if (MidMenu==Menu)
       break;
    *Left+=MENUWINDOWHSPACE;
    if (*Left+MENUWINDOWHSPACE>=WindowGetWidth(MidWin))
    {
       *Left=MENUWINDOWLEFTSPACE;
       *Top+=SYSMENUWIDTH;
    }
    MidMenu=MenuGetNext(MidMenu);
  }

  if (!Menu||MidMenu!=Menu)
  {
     *Left=0;
     *Top=0;
  }
  ReturnOK();
}

int MenuGetRealLeftTop(HMENU Menu,int *Left,int *Top)
{
  #define MENUTOPSPACE 10
  #define MENULEFTSPACE 40

  int MidLeft,MidTop,MidRight,MidBottom;

  *Left=*Top=0;

  if (Menu==0)
     ReturnOK();
  if (MenuGetFather(Menu))
  {
    *Left=-MENULEFTSPACE;
    *Top=-MENUTOPSPACE;
  }
  while (MenuGetFather(Menu))
  {
    while (MenuGetPrev(Menu))
    {
      *Top+=MenuGetHeight(Menu);
      Menu=MenuGetPrev(Menu);
    }
    *Top+=MENUTOPSPACE;
    *Left+=MENULEFTSPACE;
    Menu=MenuGetFather(Menu);
  }
  WindowGetRealRect(MenuGetReturnValue(Menu),&MidLeft,&MidTop,&MidRight,&MidBottom);
  *Left+=MidLeft;
  *Top+=MidTop+SYSMENUWIDTH;
  if (WindowCanResizeable(MenuGetReturnValue(Menu)))
  {
     *Left+=LINESPACE;
     *Top+=LINESPACE;
  }
  else
  {
     (*Left)++;
     (*Top)++;
  }
  MenuGetTopHeight(Menu,&MidLeft,&MidTop);
  (*Left)+=MidLeft;
  (*Top)+=MidTop;
  ReturnOK();
}

static void MenuDraw(HMENU Menu,char SelectSign,char ChildSign)
{
  if (Menu>=MAXMENUS)
      return;

  if (ChildSign==0)                   // ????
  {                                    /* DrawMenu */
     int MidLeft,MidTop;
     struct viewporttype SaveViewport;
     unsigned old_style;

     int SaveColor;
     int width=MENUWIDTH;

     MouseHidden();
     getviewsettings(&SaveViewport);
    #ifdef __TURBOC__
     struct fillsettingstype SaveFillStyle;
     struct linesettingstype SaveLineStyle;
         getlinesettings(&SaveLineStyle);
         getfillsettings(&SaveFillStyle);
    #else
         old_style=getlinestyle();
    #endif

     SaveColor=getcolor();
     MenuGetRealLeftTop(Menu,&MidLeft,&MidTop);
     setviewport(0,0,getmaxx(),getmaxy(),1);

     if(MidLeft+width>getmaxx())
         width=getmaxx()-MidLeft-4;

     if (MenuIsSpaceBar(Menu))           // only need draw a line
     {
         Hline3DDown(0,0,getmaxx(),getmaxy(),MidLeft+2,
                     MidLeft+width-2,MidTop+MenuGetHeight(Menu)/2+10);
     }
     else
     {
        if (MenuGetFather(Menu)!=0)
        {
           if (SelectSign)              // if menu is selected
              setfillstyle(1,MENUSELECTBKCOLOR);
           else
              // setfillstyle(1,MENUBKCOLOR);           // ByHance
              setfillstyle(1,EGA_LIGHTGRAY);
          // clear this bar_area
           bar(MidLeft+2,MidTop+2,MidLeft+width-2,MidTop+1+MenuGetHeight(Menu));
        }

        setcolor(EGA_RED);

        if (MenuIsChecked(Menu))            // draw 'v' before item
        {
           line(MidLeft+5,MidTop+MenuGetHeight(Menu)/2+1,
                MidLeft+9,MidTop+MenuGetHeight(Menu)-3);
           line(MidLeft+4,MidTop+MenuGetHeight(Menu)/2+1,
                MidLeft+8,MidTop+MenuGetHeight(Menu)-3);
           line(MidLeft+9,MidTop+MenuGetHeight(Menu)-3,
                MidLeft+15,MidTop+5);
           line(MidLeft+8,MidTop+MenuGetHeight(Menu)-3,
                MidLeft+14,MidTop+5);
        }

        if (SelectSign)
           setcolor(MENUSELECTCOLOR);
        else
           setcolor(MENUCOLOR);


        if (MenuIsText(Menu))
        {
           if (MenuGetFather(Menu)==0&&MenuGetPrev(Menu)!=0)
              MidTop-=SYSMENUWIDTH;
           if (SelectSign)
              DisplayString(MenuGetName(Menu),MidLeft+16,MidTop+2,
                            MENUSELECTCOLOR,MENUSELECTBKCOLOR);
           else
              // DisplayString(MenuGetName(Menu),MidLeft+16,MidTop+2,MENUCOLOR,MENUBKCOLOR);
              DisplayString(MenuGetName(Menu),MidLeft+16,MidTop+2,MENUCOLOR,EGA_LIGHTGRAY);

           if (MenuGetShortChar(Menu))     // draw under_line under shortChar
              line(MidLeft+15+(MenuGetShortChar(Menu)-1)*CHARWIDTH/2,
                   MidTop+2+CHARHEIGHT,
                   MidLeft+15+MenuGetShortChar(Menu)*CHARWIDTH/2,
                   MidTop+2+CHARHEIGHT);
        }

        if (MenuGetFather(Menu)&&MenuGetChild(Menu))     // if it has sub_menu
        {
           int menux,menuy;
           if (SelectSign)   {           // if menu is selected
              setcolor(EGA_LIGHTGRAY);
           } else {
              setcolor(EGA_BLACK);
           }

           menux = MidLeft+MENUWIDTH-21;
           menuy = MidTop+MenuGetHeight(Menu)/2-3;
           #define d_icn(x1,x2,y) line(menux+x1,menuy+y,menux+x2,menuy+y)
           d_icn(4,16,0);
           d_icn(2,4,1); d_icn(16,16,1);
           d_icn(1,2,2); d_icn(9,16,2);
           d_icn(1,1,3); d_icn(6,9,3);
           d_icn(1,1,4); d_icn(9,9,4);
           d_icn(1,1,5); d_icn(5,9,5);
           d_icn(1,1,6); d_icn(8,8,6);
           d_icn(1,1,7); d_icn(5,8,7);
           d_icn(1,1,8); d_icn(6,6,8);
           d_icn(1,6,9);
           #undef d_icn
        }
     }

     setviewport(SaveViewport.left,SaveViewport.top,
                 SaveViewport.right,SaveViewport.bottom,
                 SaveViewport.clip);
    #ifdef __TURBOC__
         setlinestyle(SaveLineStyle.linestyle,
                  SaveLineStyle.upattern,
                  SaveLineStyle.thickness);
         setfillstyle(SaveFillStyle.pattern,SaveFillStyle.color);
    #else
         setlinestyle(old_style);
    #endif

     setcolor(SaveColor);
     MouseShow();
  }
  else
  {            // ChildSign != 0
     int MidMenu;

     MidMenu=MenuGetChild(Menu);
     while(MidMenu)
     {
                                       /* Draw Menu */
//       if ((MenuGetPrev(MidMenu)==0)&&(MenuGetFather(MidMenu)))   // ByHance
       if (MenuGetPrev(MidMenu)==0)
                                       /* First Child is Default Selected */
          MenuDraw(MidMenu,1,0);
       else                            /* Other child */
          MenuDraw(MidMenu,0,0);
       MidMenu=MenuGetNext(MidMenu);
     }
  }     /* Draw Box */
}

/*------- when open a H_menu, this proc will draw v_menu ------*/
int MenuOpen(HMENU Menu)
{
  char *MidPoint=NULL;
  int TotalHeight=4,ImageSize,MidLeft,MidTop,RectRight,RectBottom;
  int LeftSpace,TopSpace;
  HMENU MidMenu;

  if (Menu>=MAXMENUS)
      return(INVAILEDPARAM);

  MidMenu=MenuGetChild(Menu);
  if (MidMenu==0)
     Error(INVAILEDPARAM);
  else
     ActiveMenu=MidMenu;

  if (MenuGetFather(Menu))
  {
     LeftSpace=MENULEFTSPACE;
     TopSpace=MENUTOPSPACE;
  }
  else
     LeftSpace=TopSpace=0;

  while (MidMenu)
  {
    TotalHeight+=MenuGetHeight(MidMenu);
    MidMenu=MenuGetNext(MidMenu);
  }
  MenuGetRealLeftTop(Menu,&MidLeft,&MidTop);

  RectRight=MidLeft+LeftSpace+MENUWIDTH;
  RectBottom=TotalHeight+MidTop+TopSpace;
/*-------- changed ByHance -----
  if (RectRight>getmaxx())
     RectRight=getmaxx();
  if (RectBottom>getmaxy())
     RectBottom=getmaxy();
--------------------*/
  if (RectRight>getmaxx()-4)
     RectRight=getmaxx()-4;

  /*----- 1: save the area which will be drawed -------*/
  ImageSize=imagesize(MidLeft+LeftSpace,MidTop+TopSpace,
                      RectRight,RectBottom);
  if (MenuSetSaveImage(Menu,HandleAlloc(ImageSize,0))==0)
     Error(OUTOFMEMORY);
  if ((MidPoint=HandleLock(MenuGetSaveImage(Menu)))==NULL)
  {
     HandleFree(MenuGetSaveImage(Menu));
     MenuSetSaveImage(Menu,0);
     Error(OUTOFMEMORY);
  }
  else
  {
   /*-------- 2: draw v_menu ----------*/
     struct viewporttype SaveViewport;
     int SaveColor;
     unsigned old_style;

     MouseHidden();
     getviewsettings(&SaveViewport);
     SaveColor=getcolor();

    #ifdef __TURBOC__
     struct fillsettingstype SaveFillStyle;
     struct linesettingstype SaveLineStyle;
     getfillsettings(&SaveFillStyle);
     getlinesettings(&SaveLineStyle);
    #else
     old_style=getlinestyle();
    #endif

     setviewport(0,0,getmaxx(),getmaxy(),1);

  /*----- 1: save the area which will be drawed -------*/
     getimage(MidLeft+LeftSpace,MidTop+TopSpace,
              RectRight,RectBottom,MidPoint);
     HandleUnlock(MenuGetSaveImage(Menu));

   /*-------- 2: draw v_menu ----------*/
     /*----- clear this area -----*/
//     setfillstyle(SOLID_FILL,MENUBKCOLOR);  // ByHance
     setfillstyle(SOLID_FILL,EGA_LIGHTGRAY);
     bar(MidLeft+LeftSpace+1,MidTop+TopSpace+1,RectRight-1,
         RectBottom-1);

    /*------ draw v_menu_frame ------*/
    /*--------- changed byHance ---------------
     setcolor(MENUCOLOR);
     rectangle(MidLeft+LeftSpace,MidTop+TopSpace,
               RectRight,
               RectBottom);
     rectangle(MidLeft+LeftSpace+1,MidTop+TopSpace+1,
               RectRight-1,
               RectBottom-1);
     ------------------------------------*/
 //  Area3DUp(0,0,getmaxx(),getmaxy(),
//           MidLeft+LeftSpace+1,MidTop+TopSpace+1,RectRight-1,RectBottom-1,2);

     setcolor(EGA_WHITE);
     line(MidLeft+LeftSpace+1,MidTop+TopSpace+1,RectRight-1,MidTop+TopSpace+1);
//     line(MidLeft+LeftSpace+2,MidTop+TopSpace+2,RectRight-2,MidTop+TopSpace+2);

     line(MidLeft+LeftSpace+1,MidTop+TopSpace+1,MidLeft+LeftSpace+1,RectBottom-1);
//     line(MidLeft+LeftSpace+2,MidTop+TopSpace+2,MidLeft+LeftSpace+2,RectBottom-2);

     setcolor(EGA_DARKGRAY);
     line(MidLeft+LeftSpace+1,RectBottom-1,RectRight-1,RectBottom-1);
//     line(MidLeft+LeftSpace+2,RectBottom-2,RectRight-1,RectBottom-2);

     line(RectRight-1,MidTop+TopSpace+1,RectRight-1,RectBottom-1);
//     line(RectRight-2,MidTop+TopSpace+2,RectRight-2,RectBottom-2);

     rectangle(MidLeft+LeftSpace,MidTop+TopSpace,RectRight,RectBottom);
     /*-- end added ----*/

     if (!MenuGetFather(Menu)&&(MenuGetPrev(Menu)||
           !WindowHasSystemMenu(MenuGetReturnValue(Menu))))
        MenuDraw(Menu,1,0);

     setcolor(SaveColor);
    #ifdef __TURBOC__
         setlinestyle(SaveLineStyle.linestyle,
                  SaveLineStyle.upattern,
                  SaveLineStyle.thickness);
         setfillstyle(SaveFillStyle.pattern,SaveFillStyle.color);
    #else
         setlinestyle(old_style);
    #endif

     setviewport(SaveViewport.left,SaveViewport.top,
                 SaveViewport.right,SaveViewport.bottom,
                 SaveViewport.clip);
     MouseShow();
  }
  MenuDraw(Menu,0,1);
  return(0);
}

int MenuClose(HMENU Menu)
{
  struct viewporttype SaveViewport;
  int MidLeft,MidTop,LeftSpace,TopSpace;
  HMENU MidMenu;
  unsigned char *SaveMenuBuffer;

  if (Menu>=MAXMENUS)
      return(INVAILEDPARAM);

  MidMenu=MenuGetChild(Menu);
  while (MidMenu>0)
  {
     if (MenuGetSaveImage(MidMenu)!=0)
        MenuClose(MidMenu);
     else
        MidMenu=MenuGetNext(MidMenu);
  }

  if (MenuGetFather(Menu))
  {
     LeftSpace=MENULEFTSPACE;
     TopSpace=MENUTOPSPACE;
  }
  else
     LeftSpace=TopSpace=0;

  if (MenuGetSaveImage(Menu)!=0)
  {
     MenuGetRealLeftTop(Menu,&MidLeft,&MidTop);
     MouseHidden();
     getviewsettings(&SaveViewport);
     setviewport(0,0,getmaxx(),getmaxy(),1);
     SaveMenuBuffer=HandleLock(MenuGetSaveImage(Menu));
     if (SaveMenuBuffer!=NULL)
        putimage(MidLeft+LeftSpace,MidTop+TopSpace,
                 SaveMenuBuffer,COPY_PUT);
     HandleUnlock(MenuGetSaveImage(Menu));
     HandleFree(MenuGetSaveImage(Menu));
     MenuSetSaveImage(Menu,0);
     if (!MenuGetFather(Menu)&&(MenuGetPrev(Menu)||
           !WindowHasSystemMenu(MenuGetReturnValue(Menu))))
        MenuDraw(Menu,0,0);
     setviewport(SaveViewport.left,SaveViewport.top,
                 SaveViewport.right,SaveViewport.bottom,
                 SaveViewport.clip);
     MouseShow();
  }
  ActiveMenu=0;

  if(MenuGetReturnValue(Menu)<=1)
      ActiveWindow=1;               // ByHance, 95,11.24
  ReturnOK();
}

int MenuAllClose(HMENU Menu)
{
  HMENU MidMenu;

  if (Menu>=MAXMENUS)
      return(INVAILEDPARAM);

  MidMenu=MenuGetTopest(Menu);
  if (MidMenu>0)
     return(MenuClose(MidMenu));
  else
     ReturnOK();
}

int MenuGetFocus(HMENU hMenu,int X,int Y)
{
  HMENU MidMenu;
  int MidLeft,MidTop;

  if ((hMenu==0)||(MenuGetFather(hMenu)==0))
     return(0);

  MidMenu=hMenu;
  while (MenuGetPrev(MidMenu))
    MidMenu=MenuGetPrev(MidMenu);

  MenuGetRealLeftTop(MidMenu,&MidLeft,&MidTop);

  if (!(X>=MidLeft&&X<MidLeft+MENUWIDTH))
     MidMenu=0;
  else
  while (MidMenu)
  {
       if (Y>=MidTop&&Y<MidTop+MenuGetHeight(MidMenu))
          break;
       else
       {
          MidTop+=MenuGetHeight(MidMenu);
          MidMenu=MenuGetNext(MidMenu);
       }
  }

  if (MidMenu)
     return(MidMenu);
  else
     return(MenuGetFocus(MenuGetFather(hMenu),X,Y));
}

long MenuDefaultProc(HWND Window,HMSG Message,long Param1,long Param2)
{
  HMENU MidMenu;

  switch (Message)
  {
    case KEYSTRING:             // cancel this operation
         break;
    case KEYDOWN:
         switch (MAKELO(Param1))
         {
           case LEFT:
                if ((MenuGetFather(ActiveMenu)!=0)
                    &&(MenuGetFather(MenuGetFather(ActiveMenu))==0))
                {
                   MidMenu=ActiveMenu;
                   MenuClose(MenuGetFather(ActiveMenu));
                   if (MenuGetPrev(MenuGetFather(MidMenu))!=0)
                      ActiveMenu=MenuGetPrev(MenuGetFather(MidMenu));
                   else
                   {
                      ActiveMenu=MenuGetTopest(MidMenu);
                      while (MenuGetNext(ActiveMenu))
                        ActiveMenu=MenuGetNext(ActiveMenu);
                   }
                   MenuOpen(ActiveMenu);
                }
                else
                {
                   if ((MenuGetFather(ActiveMenu)==0)
                       &&(MenuGetPrev(ActiveMenu)!=0||
                        !WindowHasSystemMenu(MenuGetReturnValue(ActiveMenu))))
                   {
                      MenuDraw(ActiveMenu,0,0);
                      if (MenuGetPrev(MenuGetPrev(ActiveMenu))
                         ||(MenuGetPrev(ActiveMenu)&&
                         !WindowHasSystemMenu(MenuGetReturnValue(ActiveMenu))))
                         ActiveMenu=MenuGetPrev(ActiveMenu);
                      else
                      {
                         while (MenuGetNext(ActiveMenu))
                           ActiveMenu=MenuGetNext(ActiveMenu);
                      }
                      MenuDraw(ActiveMenu,1,0);
                   }
                }
                break;
           case RIGHT:
                if ((MenuGetFather(ActiveMenu)!=0)
                    &&(MenuGetFather(MenuGetFather(ActiveMenu))==0))
                {
                   MidMenu=ActiveMenu;
                   MenuClose(MenuGetFather(ActiveMenu));
                   if (MenuGetNext(MenuGetFather(MidMenu))!=0)
                      ActiveMenu=MenuGetNext(MenuGetFather(MidMenu));
                   else
                   {
                      ActiveMenu=MenuGetTopest(MidMenu);
                      ActiveMenu=WindowGetUserData(MenuGetReturnValue(ActiveMenu));
                   }
                   MenuOpen(ActiveMenu);
                }
                else
                {
                   if ((MenuGetFather(ActiveMenu)==0)
                       &&(MenuGetPrev(ActiveMenu)!=0||
                        !WindowHasSystemMenu(MenuGetReturnValue(ActiveMenu))))
                   {
                      MenuDraw(ActiveMenu,0,0);
                      if (MenuGetNext(ActiveMenu))
                         ActiveMenu=MenuGetNext(ActiveMenu);
                      else
                      {
                         ActiveMenu=WindowGetUserData(
                                    MenuGetReturnValue(ActiveMenu));
                         if (WindowHasSystemMenu(MenuGetReturnValue(ActiveMenu)))
                            ActiveMenu=MenuGetNext(ActiveMenu);
                      }
                      MenuDraw(ActiveMenu,1,0);
                   }
                }
                break;
           case UP:
                if (MenuGetFather(ActiveMenu))
                {
                   MenuDraw(ActiveMenu,0,0);
                   if (MenuGetPrev(ActiveMenu)!=0)
                   {
                      ActiveMenu=MenuGetPrev(ActiveMenu);
                      if (MenuIsSpaceBar(ActiveMenu))
                         ActiveMenu=MenuGetPrev(ActiveMenu);
                   }
                   else
                      if (MenuGetFather(ActiveMenu)!=0)
                         ActiveMenu=MenuGetLastChild(MenuGetFather(ActiveMenu));
                   MenuDraw(ActiveMenu,1,0);
                }
                else
                   MenuOpen(ActiveMenu);
                break;
           case DOWN:
                if (MenuGetFather(ActiveMenu))
                {
                   MenuDraw(ActiveMenu,0,0);
                   if (MenuGetNext(ActiveMenu)!=0)
                   {
                      ActiveMenu=MenuGetNext(ActiveMenu);
                      if (MenuIsSpaceBar(ActiveMenu))
                         ActiveMenu=MenuGetNext(ActiveMenu);
                   }
                   else
                      if (MenuGetFather(ActiveMenu)!=0)
                         ActiveMenu=MenuGetChild(MenuGetFather(ActiveMenu));
                   MenuDraw(ActiveMenu,1,0);
                }
                else
                   MenuOpen(ActiveMenu);
                break;
           case ESC:
                if (ActiveMenu>0)
                {
                   MidMenu=ActiveMenu;
                   if (MenuGetFather(MidMenu))
                   {
                      MenuClose(MenuGetFather(ActiveMenu));
                      ActiveMenu=MenuGetFather(MidMenu);
                      if (!MenuGetFather(ActiveMenu))
                            ActiveMenu=0;
                      /*----------------- deleted ByHance -------
                         if (MenuGetPrev(ActiveMenu))
                            MenuDraw(ActiveMenu,1,0);
                         else
                            ActiveMenu=0;
                       --------------------------*/
                   }
                   else
                   {
                      MenuDraw(MidMenu,0,0);
                      MessageInsert(MenuGetReturnValue(MenuGetTopest(ActiveMenu)),
                                 GETFOCUS,0l,0l);
                      ActiveMenu=0;
                    }
                }
                break;
           case ENTER:
                if (!MenuGetChild(ActiveMenu))
                {
                   MessageInsert(MenuGetReturnValue(MenuGetTopest(ActiveMenu)),
                                 GETFOCUS,0l,0l);
                   if (MenuGetFather(MenuGetFather(ActiveMenu))==0
                       &&MenuGetPrev(MenuGetFather(ActiveMenu))==0)
                      MessageInsert(MenuGetReturnValue(MenuGetTopest(ActiveMenu)),
                                    MenuGetReturnValue(ActiveMenu),0,0);
                   else
                      MessageInsert(MenuGetReturnValue(MenuGetTopest(ActiveMenu)),
                                    MENUCOMMAND,MenuGetReturnValue(ActiveMenu),0);
                   MenuAllClose(ActiveMenu);
                }
                else
                   MenuOpen(ActiveMenu);
                break;
           default:
                if (MenuGetFather(ActiveMenu))
                   MidMenu=MenuGetChild(MenuGetFather(ActiveMenu));
                else
                   MidMenu=WindowGetUserData(MenuGetReturnValue(ActiveMenu));
                while(MidMenu)
                {
                  if (MenuIsText(MidMenu)&&MenuGetShortChar(MidMenu)
                      &&toupper(MenuGetName(MidMenu)[MenuGetShortChar(MidMenu)-1])
                        ==toupper(Param1))
                  {
                     if (MenuGetChild(MidMenu))
                     {
                        MenuDraw(ActiveMenu,0,0);
                        MenuDraw(MidMenu,1,0);
                        MenuOpen(MidMenu);
                     }
                     else
                     {
                        if (MenuGetFather(MenuGetFather(MidMenu))==0
                           &&MenuGetPrev(MenuGetFather(MidMenu))==0)
                           MessageInsert(MenuGetReturnValue(MenuGetTopest(MidMenu)),
                                         MenuGetReturnValue(MidMenu),0,0);
                        else
                           MessageInsert(MenuGetReturnValue(MenuGetTopest(MidMenu)),
                                         MENUCOMMAND,MenuGetReturnValue(MidMenu),0);
                        MenuAllClose(ActiveMenu);
                     }
                     break;
                  }
                  else
                     MidMenu=MenuGetNext(MidMenu);
                }
                break;
         }
         break;
    case MOUSEMOVE:
         //ToolBarMouseMove(Window,Message,Param1,Param2);
         MouseSetGraph(FINGERMOUSE);
         break;
    case MOUSELEFTDROP:
    case MOUSELEFTDOWN:
         if (Message==MOUSELEFTDROP)
            MidMenu=MenuGetFocus(ActiveMenu,(short)MAKEHI(Param2),(short)MAKELO(Param2));
         else
            MidMenu=MenuGetFocus(ActiveMenu,(short)MAKEHI(Param1),(short)MAKELO(Param1));

         if (MidMenu&&MidMenu!=ActiveMenu)
         {
            HMENU SaveFatherMenu;
          #if 0
            HMENU SubMenu;
            if (MenuGetFather(ActiveMenu))
            {
               SaveFatherMenu=ActiveMenu;
               SubMenu=MenuGetChild(MenuGetFather(ActiveMenu));

               while (MenuGetFather(SubMenu))
               {
                    while (MenuGetNext(SubMenu))
                    {
                      if (SubMenu!=MidMenu)
                         SubMenu=MenuGetNext(SubMenu);
                      else
                         break;
                    }
                    if (SubMenu==MidMenu) break;
                    else
                    {
                       SaveFatherMenu=MenuGetFather(SaveFatherMenu);
                       SubMenu=MenuGetFather(SubMenu);
                    }
               }

               if (SubMenu==MidMenu)
               {
                  if (SaveFatherMenu!=ActiveMenu)  {
                     MenuClose(SaveFatherMenu);
                  }
                  MenuDraw(SaveFatherMenu,0,0);
                  ActiveMenu=MidMenu;
                  MenuDraw(ActiveMenu,1,0);
                  break;
               }
            }
          #else
            if (MenuGetFather(ActiveMenu))
            {
               SaveFatherMenu=MenuGetFather(ActiveMenu);
               if(MenuGetFather(MidMenu)!=SaveFatherMenu)
               {
                   MenuClose(SaveFatherMenu);
                   MenuDraw(SaveFatherMenu,0,0);
               }
               else
                   MenuDraw(ActiveMenu,0,0);

               ActiveMenu=MidMenu;
               MenuDraw(ActiveMenu,1,0);
            }
          #endif
         }
         break;
    case MOUSELEFTUP:
         if (MenuGetChild(ActiveMenu))
            MenuOpen(ActiveMenu);           // open sub_sub_menu
         else
         {
            if (MenuIsSpaceBar(ActiveMenu))
               break;

            MidMenu=MenuGetFocus(ActiveMenu,(short)MAKEHI(Param1),(short)MAKELO(Param1));
            if (!MidMenu||MidMenu!=ActiveMenu)  // not at this item
               break;

            MidMenu=MenuGetTopest(ActiveMenu);
            MessageInsert(MenuGetReturnValue(MidMenu),GETFOCUS,
                          0l,0l);
            if (MenuGetFather(MenuGetFather(ActiveMenu))==0
                &&MenuGetPrev(MenuGetFather(ActiveMenu))==0)
               MessageInsert(MenuGetReturnValue(MidMenu),
                             MenuGetReturnValue(ActiveMenu),0,0);
            else
               MessageInsert(MenuGetReturnValue(MidMenu),
                             MENUCOMMAND,MenuGetReturnValue(ActiveMenu),0);
            MenuAllClose(ActiveMenu);
         }
         break;
    /*case REDRAWMESSAGE:
         MenuDraw(Param1,(short)MAKEHI(Param2),(short)MAKELO(Param2));
         break;*/
    default:
         return(DataofWindows[Window].Procedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static int CreatUserSubMenu(HMENU FatherMenu,LoadMenus *Menu,int *TotalMenu)
{
  int i,j,TotalSub;
  HMENU NewFatherMenu;
  MENUS TobeCreatMenu;

  for (i=0,j=1;i<Menu[0].TotalSubMenu;i++)
  {
      memset(&TobeCreatMenu,0,sizeof(TobeCreatMenu));
      TobeCreatMenu.MenuReturnValue=Menu[j].MenuReturnValue;
      TobeCreatMenu.MenuShortKey=Menu[j].MenuShortKey;
      TobeCreatMenu.MenuShortChar=Menu[j].MenuShortChar;
      TobeCreatMenu.MenuType=Menu[j].MenuType;
      strcpy(TobeCreatMenu.MenuName,Menu[j].MenuText);

      NewFatherMenu=MenuAppend(FatherMenu,&TobeCreatMenu);
      if (Menu[j].TotalSubMenu==0)
         j++;
      else
      {
         CreatUserSubMenu(NewFatherMenu,&Menu[j],&TotalSub);
         j+=TotalSub;
      }
  }
  *TotalMenu=j;
  ReturnOK();
}

static int CreatUserMenu(HWND MidWin,LoadMenus *Menu)
{
  int i,j,TotalMenu,MenuLeftSpace=MENUWINDOWLEFTSPACE;
  MENUS TobeCreatMenu;
  HMENU MidMenu,ReturnMenu;

  for (i=0,j=1;i<Menu[0].TotalSubMenu;i++)
  {
    memset(&TobeCreatMenu,0,sizeof(TobeCreatMenu));
    TobeCreatMenu.MenuReturnValue=Menu[j].MenuReturnValue;
    TobeCreatMenu.MenuShortKey=Menu[j].MenuShortKey;
    TobeCreatMenu.MenuShortChar=Menu[j].MenuShortChar;
    TobeCreatMenu.MenuType=Menu[j].MenuType;
    strcpy(TobeCreatMenu.MenuName,Menu[j].MenuText);
    MidMenu=CreatMenu(&TobeCreatMenu,MidWin);
    if (i==0)
       ReturnMenu=MidMenu;
    MenuSetHeight(MidMenu,MenuLeftSpace);
    CreatUserSubMenu(MidMenu,&Menu[j],&TotalMenu);
    j+=TotalMenu;
    MenuLeftSpace+=MENUWINDOWHSPACE;
  }
  return(ReturnMenu);
}

HMENU CreatSystemMenu(HWND Window)
{
  int Result,Left,Top,Right,Bottom;
  HMENU hMenu;
  MENUS SysMenu;

  if ((Result=MenuConstruct())<OpOK)
     Error(Result);
  else
     hMenu=Result;

  // lock this handle, set default value
  memset(&DataofMenus[hMenu],0,sizeof(MENUS));
  MenuSetUse(hMenu);
  WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
  MenuSetHeight(hMenu,0);
  MenuSetShortKey(hMenu,ALT_BLANK);
  MenuSetBoxAttr(hMenu);

  memset(&SysMenu,0,sizeof(SysMenu));

  SysMenu.MenuHeight=10;
  strcpy(SysMenu.MenuName,"关闭[C]");
  SysMenu.MenuShortKey=ALT_F4;
  SysMenu.MenuShortChar=6;
  SysMenu.MenuReturnValue=WINDOWCLOSE;
  SysMenu.MenuUseSign=1;
  MenuAppend(hMenu,&SysMenu);

/**************
  if (WindowCanMoveable(Window))
  {
     strcpy(SysMenu.MenuName,"移动[M]");
     SysMenu.MenuShortKey=0;
     SysMenu.MenuShortChar=6;
     SysMenu.MenuReturnValue=WINDOWMOVE;
     MenuAppend(hMenu,&SysMenu);
  }

  if (WindowCanResizeable(Window))
  {
     strcpy(SysMenu.MenuName,"大小[R]");
     SysMenu.MenuShortKey=0;
     SysMenu.MenuShortChar=6;
     SysMenu.MenuReturnValue=WINDOWRESIZE;
     MenuAppend(hMenu,&SysMenu);
  }

  if (WindowCanMiniumable(Window))
  {
     strcpy(SysMenu.MenuName,"最小[i]");
     SysMenu.MenuShortKey=0;
     SysMenu.MenuShortChar=6;
     SysMenu.MenuReturnValue=WINDOWMINIUM;
     MenuAppend(hMenu,&SysMenu);
  }

  if (WindowCanMaxiumable(Window))
  {
     strcpy(SysMenu.MenuName,"最大[x]");
     SysMenu.MenuShortKey=0;
     SysMenu.MenuShortChar=6;
     SysMenu.MenuReturnValue=WINDOWSIZE;
     MenuAppend(hMenu,&SysMenu);
  }

******************/
  WindowSetUserData(Window,hMenu);
  MenuSetReturnValue(hMenu,Window);
  return(hMenu);
}

/*--- return menu handle, which is in Window, and (X,Y) in the menu
        0=failed
 ---*/
static HMENU MenuWindowGetFocus(HWND Window,int X,int Y)
{
  int Left=MENUWINDOWLEFTSPACE,Top=0;
  HMENU MidMenu;

  MidMenu=WindowGetUserData(Window);
  if (Left>X&&Y<Top+SYSMENUWIDTH)
     return(0);

  while (MidMenu)
  {
    if (Left<=X&&X<Left+MENUWINDOWHSPACE&&Top<=Y&&Y<Top+SYSMENUWIDTH)
       break;
    else
    {
       MidMenu=MenuGetNext(MidMenu);
       Left+=MENUWINDOWHSPACE;
       if (Left+MENUWINDOWHSPACE>=WindowGetWidth(Window))
       {
          Left=MENUWINDOWLEFTSPACE;
          Top+=SYSMENUWIDTH;
       }
    }
  } // while

  return(MidMenu);
}

static int MenuWindowProc(int Window,int Message,long Param1,long Param2)
{
  HWND MidMenu;

  switch (Message)
  {
    case MOUSELEFTDROP:
    case MOUSELEFTDOWN:
         if (Message==MOUSELEFTDROP)
            MidMenu=MenuWindowGetFocus(Window,(short)MAKEHI(Param2),(short)MAKELO(Param2));
         else
            MidMenu=MenuWindowGetFocus(Window,(short)MAKEHI(Param1),(short)MAKELO(Param1));

         if (MidMenu&&MidMenu!=ActiveTopMenu)
         {
            if (ActiveTopMenu>0)
               MenuDraw(ActiveTopMenu,0,0);           // hidden old h_menu
            ActiveTopMenu=MidMenu;
            MenuDraw(MidMenu,1,0);            // draw new h_menu
         }
         break;
    case MOUSELEFTUP:
        if (ActiveTopMenu>0)          // ByHance
           MenuDraw(ActiveTopMenu,0,0);       // hidden this H_menu

         MidMenu=MenuWindowGetFocus(Window,(short)MAKEHI(Param1),(short)MAKELO(Param1));
         if (MidMenu)
         {
            if (MenuGetChild(MidMenu))  {    // if it has sub_menu, open them
               MenuOpen(MidMenu);
            } else {
               MessageInsert(Window,MenuGetReturnValue(MidMenu),0,0);
               MenuAllClose(MidMenu);
            }
         }
         else
            MessageInsert(WindowGetFather(Window),GETFOCUS,0,0);

       /*------------
         else
         {                      // not in menu_area
            if (ActiveTopMenu>0)
               MenuDraw(ActiveTopMenu,0,0);       // hidden this H_menu
         }
         -----------*/
         ActiveTopMenu=0;                   // ????
         break;
    case MOUSEMOVE:
         MessageGo(Window,DELBUBLE,0L,0L);  // delete old hint window
         ToolBarMouseMove(Window,Message,Param1,Param2);
         break;
    /*case REDRAWMESSAGE:
         MenuDraw(Param1,(short)MAKEHI(Param2),(short)MAKELO(Param2));
         break;*/
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

int CreatMenuWindow(HWND FatherWindow,LoadMenus *MenuData)
{
  Windows TobeCreatWindow;
  int LineSpace,TopSpace;
  HWND MidWindow;

  if (WindowCanResizeable(FatherWindow))
     LineSpace=LINESPACE+1;
  else
     LineSpace=1;                                       // frame_edge width

  if (WindowHasSystemMenu(FatherWindow)||WindowCanMoveable(FatherWindow))
     TopSpace=SYSMENUWIDTH;
  else
     TopSpace=0;

  memset(&TobeCreatWindow,0,sizeof(TobeCreatWindow));

  TobeCreatWindow.Left=LineSpace;
  TobeCreatWindow.Top=LineSpace+TopSpace;
  TobeCreatWindow.Right=WindowGetWidth(FatherWindow)-LineSpace-1;
  TobeCreatWindow.Bottom=SYSMENUWIDTH+LineSpace+TopSpace;
  TobeCreatWindow.Procedure=(Function *)MenuWindowProc;
  TobeCreatWindow.WindowStyle=3|WindowSetIsUserWindow()|WindowSetIsMenuWindow;
  TobeCreatWindow.NextChildWindow=0;
  TobeCreatWindow.ActiveChildWindow=0;

  MidWindow=WindowAppend(&TobeCreatWindow,FatherWindow);
  WindowSetStyle(FatherWindow,
                 WindowGetStyle(FatherWindow)|WindowSetHasMenu());

  WindowSetUserData(MidWindow,CreatUserMenu(FatherWindow,MenuData));
  return(MidWindow);
}

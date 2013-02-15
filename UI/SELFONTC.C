/*-------------------------------------------------------------------
* Name: selfontc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern char CFontsName[][8];
static int zjh_color,zjh_width,zjh_height,zjh_cfont,zjh_ty,zjh_size,zjh_font;
static int color_alt,size_alt;
// static int last_window=0;

  #ifdef NO_USE         // ByHance, 97,5.11
static unsigned long SetCharAttribute(HWND Window,HMSG Message,long Param1,long Param2)
{
  return(DialogDefaultProcedure(Window, Message, Param1, Param2));
}
   #endif

#define Dispsx  18
#define Dispsy  245
#define Dispex  280
#define Dispey  290


static char DotLib[][7]={"宋体","楷体","黑体","仿宋体"};
static char ColorList[][7]={"黑色",
                     "深蓝色",
                     "深绿色",
                     "深青色",
                     "深红色",
                     "紫色",
                     "橙色",
                     "深灰色",
                     "浅灰色",
                     "蓝色",
                     "绿色",
                     "青色",
                     "红色",
                     "品红色",
                     "黄色",
                     "白色"
                    } ;
static char FontList[][7]={"初号字",
                    "一号字",
                    "二号字",
                    "三号字",
                    "四号字",
                    "小四号",
                    "五号字",
                    "小五号",
                    "六号字",
                    "七号字",
                    "八号字",
                    "自定义"
                    };
char EFontName[][8]={
                        "白正001",
                        "白斜002",
                        "黑正003",
                        "黑斜004",
                        "扩展005"
};


static void DispStrPro(void);
static unsigned long ColorListPro(HWND Window,HMSG Message,long Param1,long Param2)
{
  int hlist,i,curr;

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,LISTSETITEMHEIGHT,16,0);
         MessageGo(Window,LISTSETITEMLENGTH,8,0);
         MessageGo(Window,LISTDELETEALL,0L,0L);

         for (i=0;i<16;i++)
           MessageGo(Window, LISTINSERTITEM, FP2LONG(ColorList[15-i]), 0L);
         hlist = WindowList(Window);
         curr=zjh_color;
         if (curr>2) ListSetTop(hlist,(curr-3)); else ListSetTop(hlist,0);
         ListSetCurrent(hlist,zjh_color);
         break;
    case ITEMSELECT:
         color_alt=1;
         hlist = WindowList(Window);
         curr = ListGetCurrent(hlist);
         zjh_color=curr;
         //if (last_window==6) DispStrPro();
         DispStrPro();
         //last_window=6;
         break;
    case LISTBOXCONFIRM:
   #ifdef NOT_CORRECT      // ByHance, 97,5.11
         hlist = WindowList(Window);
         i = ListGetTop(hlist);
         curr = ListGetCurrent(hlist);
         MessageGo(Window,WINDOWINIT,0,0);
         MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
         ListSetTop(hlist,i);
         ListSetCurrent(hlist,curr);
         MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
   #else
         MessageGo(WindowGetFather(WindowGetFather(Window)),DIALOGBOXOK,0l,0l);
   #endif
         break;
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long FontListPro(HWND Window,HMSG Message,long Param1,long Param2)
{
  int hlist,i,curr;
  float pointn;
  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,LISTSETITEMHEIGHT,16,0);
         MessageGo(Window,LISTSETITEMLENGTH,10,0);
         MessageGo(Window,LISTDELETEALL,0L,0L);

         for (i=0;i<12;i++)
           MessageGo(Window, LISTINSERTITEM, FP2LONG(FontList[11-i]), 0L);

         curr=11;
         if (abs(zjh_width-583)<5) curr=0;
         else
         if (abs(zjh_width-361)<5) curr=1;
         else
         if (abs(zjh_width-305)<5) curr=2;
         else
         if (abs(zjh_width-222)<5) curr=3;
         else
         if (abs(zjh_width-187)<5) curr=4;
         else
         if (abs(zjh_width-166)<5) curr=5;
         else
         if (abs(zjh_width-145)<5) curr=6;
         else
         if (abs(zjh_width-125)<5) curr=7;
         else
         if (abs(zjh_width-111)<5) curr=8;
         else
         if (abs(zjh_width-76)<5)  curr=9;
         else
         if (abs(zjh_width-69)<5)  curr=10;

         hlist = WindowList(Window);
         if (curr>2) ListSetTop(hlist,(curr-3)); else ListSetTop(hlist,0);
         ListSetCurrent(hlist,curr);
         break;
    case ITEMSELECT:
         size_alt=1;
         hlist = WindowList(Window);
         curr = ListGetCurrent(hlist);
         zjh_size=curr;
         switch (zjh_size)
         {
           case 11:  break;
           case 10:  pointn = 5;    goto lbl_Size;
           case 9:   pointn = 5.5;  goto lbl_Size;
           case 8:   pointn = 8;    goto lbl_Size;
           case 7:   pointn = 9;    goto lbl_Size;
           case 6:   pointn = 10.5; goto lbl_Size;
           case 5:   pointn = 12;   goto lbl_Size;
           case 4:   pointn = 13.5; goto lbl_Size;
           case 3:   pointn = 16;   goto lbl_Size;
           case 2:   pointn = 22;   goto lbl_Size;
           case 1:   pointn = 26;   goto lbl_Size;
           case 0:   pointn = 42;   // goto lbl_Size;
           default:
            lbl_Size:
                   zjh_width=pointn*SCALEMETER/72;
                   zjh_height=zjh_width;
                   break;
         }

         //if (last_window==5) DispStrPro();
         DispStrPro();
         //last_window=5;
         break;
    case LISTBOXCONFIRM:
   #ifdef NOT_CORRECT      // ByHance, 97,5.11
         hlist = WindowList(Window);
         i = ListGetTop(hlist);
         curr = ListGetCurrent(hlist);
         MessageGo(Window,WINDOWINIT,0,0);
         MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
         ListSetTop(hlist,i);
         ListSetCurrent(hlist,curr);
         MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
   #else
         MessageGo(WindowGetFather(WindowGetFather(Window)),DIALOGBOXOK,0l,0l);
   #endif
         break;
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long DotLibList(HWND Window,HMSG Message,long Param1,long Param2)
{
  int hlist,i,curr;

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,LISTSETITEMHEIGHT,16,0);
         MessageGo(Window,LISTSETITEMLENGTH,10,0);
         MessageGo(Window,LISTDELETEALL,0L,0L);

         for (i=0;i<4;i++)
           MessageGo(Window, LISTINSERTITEM, FP2LONG(DotLib[3-i]), 0L);
         if (zjh_ty==1)
         {
           curr=zjh_cfont%4;
           hlist = WindowList(Window);
           ListSetCurrent(hlist,curr);
         }
         break;
    case ITEMSELECT:
         hlist = WindowList(Window);
         curr = ListGetCurrent(hlist);
         zjh_ty=1;
         zjh_cfont=curr;
         //if (last_window==1) DispStrPro();
         DispStrPro();
         //last_window=1;
         break;
    case LISTBOXCONFIRM:
   #ifdef NOT_CORRECT      // ByHance, 97,5.11
         hlist = WindowList(Window);
         i = ListGetTop(hlist);
         curr = ListGetCurrent(hlist);
         MessageGo(Window,WINDOWINIT,0,0);
         MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
         ListSetTop(hlist,i);
         ListSetCurrent(hlist,curr);
         MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
   #else
         MessageGo(WindowGetFather(WindowGetFather(Window)),DIALOGBOXOK,0l,0l);
   #endif
         break;
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long VectLibList(HWND Window,HMSG Message,long Param1,long Param2)
{
  int hlist,i,curr;

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,LISTSETITEMHEIGHT,16,0);
         MessageGo(Window,LISTSETITEMLENGTH,8,0);
         MessageGo(Window,LISTDELETEALL,0L,0L);

           for (i=0;i<MAXVECFONT;i++)
             MessageGo(Window, LISTINSERTITEM, FP2LONG(CFontsName[MAXVECFONT-i-1]), 0L);
           if (zjh_ty<3)
           {
           curr=zjh_cfont%MAXVECFONT;
           hlist = WindowList(Window);
           if (curr>4) ListSetTop(hlist,(curr-5)); else ListSetTop(hlist,0);
           ListSetCurrent(hlist,curr);
           }
         break;
    case ITEMSELECT:
         hlist = WindowList(Window);
         curr = ListGetCurrent(hlist);
         zjh_ty=2;
         zjh_cfont=curr;
         //if (last_window==2) DispStrPro();
         DispStrPro();
         //last_window=2;
         break;
    case LISTBOXCONFIRM:
   #ifdef NOT_CORRECT      // ByHance, 97,5.11
         hlist = WindowList(Window);
         i = ListGetTop(hlist);
         curr = ListGetCurrent(hlist);
         MessageGo(Window,WINDOWINIT,0,0);
         MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
         ListSetTop(hlist,i);
         ListSetCurrent(hlist,curr);
         MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
   #else
         MessageGo(WindowGetFather(WindowGetFather(Window)),DIALOGBOXOK,0l,0l);
   #endif
         break;
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long TrueLibList(HWND Window,HMSG Message,long Param1,long Param2)
{
  int hlist,i;
  int curr;

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,LISTSETITEMHEIGHT,16,0);
         MessageGo(Window,LISTSETITEMLENGTH,8,0);
         MessageGo(Window,LISTDELETEALL,0L,0L);

           for (i=MAXVECFONT;i<TOTALCFONTS;i++)
             MessageGo(Window, LISTINSERTITEM, FP2LONG(CFontsName[TOTALCFONTS-i+MAXVECFONT-1]), 0L);

           curr = 0;
           if (zjh_ty==3)
           {
              if (zjh_cfont<200) {
                 curr = zjh_cfont-100;
              } else {
                 curr = zjh_cfont-200+(MAXTTFONTJ-100);
              }
           }

           hlist = WindowList(Window);
           if (curr>4) ListSetTop(hlist,(curr-5)); else ListSetTop(hlist,0);
           ListSetCurrent(hlist,curr);
         break;
    case ITEMSELECT:
         hlist = WindowList(Window);

         curr = ListGetCurrent(hlist);
         if (curr<MAXTTFONTJ-100) zjh_cfont= curr+100;
         else
            zjh_cfont = curr-(MAXTTFONTJ-100)+200;
         zjh_ty=3;

         //if (last_window==3) DispStrPro();
         DispStrPro();
         //last_window=3;

         break;
    case LISTBOXCONFIRM:
   #ifdef NOT_CORRECT      // ByHance, 97,5.11
         hlist = WindowList(Window);
         i = ListGetTop(hlist);
         curr = ListGetCurrent(hlist);
         MessageGo(Window,WINDOWINIT,0,0);
         MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
         ListSetTop(hlist,i);
         ListSetCurrent(hlist,curr);
         MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
   #else
         MessageGo(WindowGetFather(WindowGetFather(Window)),DIALOGBOXOK,0l,0l);
   #endif
         break;
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long EnglishLibList(HWND Window,HMSG Message,long Param1,long Param2)
{
  int hlist,i;
  int curr;

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,LISTSETITEMHEIGHT,16,0);
         MessageGo(Window,LISTSETITEMLENGTH,8,0);
         MessageGo(Window,LISTDELETEALL,0L,0L);
         for (i=994;i>=0;i--)
         {
           sprintf(EFontName[4]+4,"%03d",i+5);
           MessageGo(Window, LISTINSERTITEM, FP2LONG(EFontName[4]), 0L);
         }
         for (i=0;i<4;i++)
           MessageGo(Window, LISTINSERTITEM, FP2LONG(EFontName[3-i]), 0L);
         if (zjh_ty==4)
         {
           curr=(zjh_font-0x400)%124;
           hlist = WindowList(Window);
           if (curr>4) ListSetTop(hlist,(curr-5)); else ListSetTop(hlist,0);
           ListSetCurrent(hlist,curr);
         }
         break;
    case ITEMSELECT:
         // MessageGo(WindowGetFather(Window),FILECHANGE,Param1,Param2);
         hlist = WindowList(Window);
         curr = ListGetCurrent(hlist)+0x400;
         zjh_ty=4;
         zjh_font=curr;
         //if (last_window==4) DispStrPro();
         DispStrPro();
         //last_window=4;
         break;
    case LISTBOXCONFIRM:
   #ifdef NOT_CORRECT      // ByHance, 97,5.11
         hlist = WindowList(Window);
         i = ListGetTop(hlist);
         curr = ListGetCurrent(hlist);
         MessageGo(Window,WINDOWINIT,0,0);
         MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
         ListSetTop(hlist,i);
         ListSetCurrent(hlist,curr);
         MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
   #else
         MessageGo(WindowGetFather(WindowGetFather(Window)),DIALOGBOXOK,0l,0l);
   #endif
         break;
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static Dialogs ChangeChar[]=
{
   #define xx 26
   #define yy 7
  //{ GLOBALITEM, 5 , 0, 0, 538, 296, 0,SetCharAttribute,"选择字体" },
  { GLOBALITEM, 5 , 0, 0, 538, 296, 0, NULL, "选择字体" },

  { FRAMEITEM, 8, 14, 26, 440, 240, 0, NULL, "字  体" },
    { STATICTEXTITEM, 0, 16, 50-xx,  116,     70-xx, 0, NULL, "点阵字体:" },
    { LISTBOXITEM, 0, 16-yy, 72-xx, 110-yy,  232-xx, 0, DotLibList, "" },
    { STATICTEXTITEM, 0, 120, 50-xx, 220,     70-xx, 0, NULL, "矢量字体:" },
    { LISTBOXITEM, 0, 120-yy, 72-xx, 214-yy, 232-xx, 0, VectLibList, "" },
    { STATICTEXTITEM, 0, 225, 50-xx, 325,     70-xx, 0, NULL, "曲线字体:" },
    { LISTBOXITEM, 0, 225-yy, 72-xx, 319-yy, 232-xx, 0, TrueLibList, "" },
    { STATICTEXTITEM, 0, 330, 50-xx, 420,     70-xx, 0, NULL, "西文字体:" },
    { LISTBOXITEM, 0, 330-yy, 72-xx, 424-yy, 232-xx, 0, EnglishLibList, "" },

 // { LISTBOXITEM, 0, Dispsx, Dispsy, Dispex, Dispey, 0, NULL, ""},
  { FRAMEITEM, 1,440, 26, 530, 130, 0,NULL , "字  号" },
    { LISTBOXITEM, 0, 5, 21, 85, 100, 0,FontListPro, ""},
  { FRAMEITEM, 1,440, 135, 530, 240, 0, NULL, "颜  色" },
    { LISTBOXITEM, 0, 5, 21, 85, 100, 0,ColorListPro , ""},

  { OKBUTTON, 0, 421-120, 250, 520-120, 286, 0, NULL, "确定" },
  { CANCELBUTTON,0,421, 250, 520, 286, 0, NULL, "放弃" },
   #undef yy
   #undef xx
};

static void DispStrPro(void)
{
   int ty=zjh_ty;
   int CFont=zjh_cfont;
   int Width=zjh_width/10;
   int Height=zjh_height/10;
   int Color=zjh_color;
   static char Tmp_str[]={"RrEeDdTtEeKkEeZzPpPpUuBbLlIiSsHh"};
   static char TestDispStr[]={"理德轻松排版"};
   char *p;
   struct viewporttype ViewInformation;
   UINT Code;
   int PosX,PosY;
   int sx,sy;
   int oldcolor=getcolor();

   if (size_alt==0)
    {
       Width=41;
       Height=40;
    }

   if (Width==Height) Width+=1;
   sx=ChangeChar[0].ItemLeft;
   sy=ChangeChar[0].ItemTop;

   p=TestDispStr;

   getviewsettings(&ViewInformation);
   MouseHidden();
   setviewport(sx+Dispsx+1,sy+Dispsy+1,sx+Dispex-1,sy+Dispey-1,1);
   //setviewport(50,0,getmaxx(),getmaxy(),1);
   setcolor(15);
   bar(0,0,Dispex-Dispsx-2,Dispey-Dispsy-2);
   PosX=10;
   PosY=Dispey-Dispsy-2;

   switch (ty)
   {
      case 1:
           if(!DetectDot24Font(CFont))
                ty=5;
           break;
      case 2:
           if(!DetectVecFont(CFont)||CFont==23
             ||CFont==29||CFont==32||CFont==33||CFont==45)
                ty=5;
           break;
      case 3:
           if(!DetectTrueTypeFont(CFont))
                ty=5;
           break;
      case 4:
           CFont=zjh_font-0x400;
           p=Tmp_str;
           Width/=2;
           if (!detectEFont(CFont)) ty=5;
           break;
   }

   while (*p)
   {
      if(kbhit())
         break;

      switch(ty)
      {
         case 1:
                   Code=((*p++)<<8)+(*p++);
                   ViewportDisplayChar24(PosX,PosY,Code,CFont,Width,Height,0,0,Color);
                   PosX=PosX+Width;
                   break;
         case 2:
                   Code=((*p++)<<8)+(*p++);
                   ViewportDisplayCharVec(PosX,PosY,Code,CFont,Width,Height,0,0,Color);
                   PosX=PosX+Width;
                   break;
         case 3:
                   Code=((*p++)<<8)+(*p++);
                   ViewportDisplayCharTTF(PosX,PosY,Code,CFont,Width,Height,0,0,Color);
                   PosX=PosX+Width;
                   break;
         case 4:
                   Code=*p++;
                   //if (Code==' ') break;
                   //ViewportDisplayEChar(PosX,PosY,Code,CFont,Width,Height,0,0,Color);
                   BuildChar(PosX,PosY,Code,CFont+1,Width,Height,0,0,Color,0,RITALICBIT|ROTATEBIT,0);

                   PosX=PosX+Width;
                   break;
         default:  p++;
                   break;
      }
   }

   setviewport(ViewInformation.left,ViewInformation.top,
                  ViewInformation.right,ViewInformation.bottom,
                  ViewInformation.clip);
   MouseShow();
   setcolor(oldcolor);
}

int SetCharAttr(HWND Window)
{
   int pos,ii;
   int SaveUndoNum;

   //SetDimBorder(5);
   //SetDimDir(0);
   //SetDimColor(12);

   if(!fChgNextCell)             // ByHance,97,8.11, for change table_cell
   {
      color_alt=0;
      size_alt=0;
   }

   if( GlobalBoxTool==IDX_INPUTBOX
   && BoxCanEditable(GlobalBoxHeadHandle))
   {
      if (GlobalTextBlockStart<GlobalTextBlockEnd)
            pos = GlobalTextBlockStart;
      else
            pos = GlobalTextPosition;

      if(fChgNextCell)             // ByHance,97,8.11, for change table_cell
          goto lbl_next;

      zjh_size=-1;
      zjh_color=(TextSearchAttribute(GlobalBoxHeadHandle,
                pos,CHARCOLOR,&ii)&0xf);
      zjh_width=TextSearchAttribute(GlobalBoxHeadHandle,
                pos,CHARSIZE,&ii);
      zjh_height=TextSearchAttribute(GlobalBoxHeadHandle,
                pos,CHARHSIZE,&ii);
      zjh_cfont=TextSearchCFont(GlobalBoxHeadHandle,pos,&ii);
      zjh_font=TextSearchEFont(GlobalBoxHeadHandle,pos,&ii);

      ii=NewTextSearchAttribute(GlobalBoxHeadHandle,GlobalTextPosition);

      switch(ii)
      {
        case 1:
                if (zjh_cfont>=100) zjh_ty=3;
                    else
                if (zjh_cfont>=4) zjh_ty=2;
                    else zjh_ty=1;
                break;
        default:
                zjh_ty=4;
      }

      if (MakeDialogBox(Window,ChangeChar))
          return 1;

   lbl_next:
      GlobalNotDisplay=1;     //1.26
      SaveUndoNum=UndoOperateSum;

      /*---- change Chinese_Font -------*/
      TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                GlobalTextBlockEnd-GlobalTextBlockStart,
                                CHARFONT,zjh_cfont,&GlobalTextPosition,
                                &GlobalTextBlockStart,&GlobalTextBlockEnd);

      if ( GlobalTextBlockEnd>GlobalTextBlockStart)
           pos = GlobalTextBlockStart;
      else pos = GlobalTextPosition;

      GlobalNotDisplay=0;
      /*---- change English_Font -------*/
      TextChangeAttribute(GlobalBoxHeadHandle,pos,
                          GlobalTextBlockEnd-GlobalTextBlockStart,
                          CHARFONT,zjh_font,&GlobalTextPosition,
                          &GlobalTextBlockStart,&GlobalTextBlockEnd);

      if (color_alt)
      {
          if (GlobalTextBlockStart<GlobalTextBlockEnd)
             pos = GlobalTextBlockStart;
          else
             pos = GlobalTextPosition;

          ii=(TextSearchAttribute(GlobalBoxHeadHandle,
                             pos,CHARCOLOR,&ii)&0xf0)>>4;
          TextChangeAttribute(GlobalBoxHeadHandle,pos,
                            GlobalTextBlockEnd-GlobalTextBlockStart,
                            CHARCOLOR,(zjh_color&0xf)|(ii<<4),
                            &GlobalTextPosition,
                            &GlobalTextBlockStart,&GlobalTextBlockEnd);
      }

      if (size_alt)
      {
          GlobalNotDisplay=1;
          if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                pos = GlobalTextBlockStart;
          else
                pos = GlobalTextPosition;

          TextChangeAttribute(GlobalBoxHeadHandle,pos,
                              GlobalTextBlockEnd-GlobalTextBlockStart,
                              CHARSIZE,zjh_width,&GlobalTextPosition,
                              &GlobalTextBlockStart,&GlobalTextBlockEnd);

          if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                 pos = GlobalTextBlockStart;
          else
                 pos = GlobalTextPosition;

          GlobalNotDisplay=0;
          TextChangeAttribute(GlobalBoxHeadHandle,pos,
                              GlobalTextBlockEnd-GlobalTextBlockStart,
                              CHARHSIZE,zjh_height,&GlobalTextPosition,
                              &GlobalTextBlockStart,&GlobalTextBlockEnd);
      }
      UndoInsertCompose(UndoOperateSum-SaveUndoNum);
  } //End if editable

  return 0;
}


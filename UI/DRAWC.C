/*-------------------------------------------------------------------
* Name: drawc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#define BUTTONLINESPACE 2
#define SYSBUTTONSPACE 8

static void SetDirectVideoStatus(int EnableDirectVideo)
{
   union REGS regs;
   regs.w.ax = 0xff0f;
   regs.h.bl = 0;
   regs.h.bh = EnableDirectVideo;
   int86(0x10,&regs,&regs);
}

static void set_1_pal(int color,int r,int g,int b)
{
   union REGS regs;
   regs.w.bx = color;
   regs.h.dh = r;
   regs.h.ch = g;
   regs.h.cl = b;
   regs.w.ax=0x1010;
   int86(0x10,&regs,&regs);
   regs.h.bh = color%16;
   regs.h.bl = color%16;
   regs.w.ax=0x1000;
   int86(0x10,&regs,&regs);
}

static void ResetDispMode(int dd)
{
  union REGS Reg;

  if (!dd)
  {
  Reg.w.ax = 0x0f00;
  int386(0x10,&Reg,&Reg);
  }
  else
   Reg.w.ax=dd;
  Reg.w.ax=(Reg.w.ax&0xff);
  int386(0x10,&Reg,&Reg);

  Reg.w.ax=0x484a;
  Reg.w.bx=0xff03;
  Reg.w.cx=1;
  int386(0x16,&Reg,&Reg);
}

static short  RGB[16][3]={
         {0,0,0},            //EGA_BLACK
         {0,0,128},          //EGA_DBLUE
         {0,128,0},          //EGA_DGREEN
         {0,128,128},        //EGA_DCYAN
         {128,0,0},          //EGA_DRED
         {128,0,128},        //EGA_DMAGENTA
         {128,128,0},        //EGA_BROWN
         {128,128,128},      //EGA_DGRAY
         {192,192,192},       //EGA_GRAY
         {0,0,255},          //EGA_BLUE
         {0,255,0},         //EGA_GREEN
         {0,255,255},        //EGA_CYAN
         {255,00,0},         //EGA_RED
         {255,0,255},        //EGA_MAGENTA
         {255,255,0},        //EGA_YELLOW
         {255,255,255},      //EGA_WHITE
};


int GraphInitial()
{
  union REGS Reg;
  //initgraph();
  int mode=_VRES16COLOR;        // 640x480x16, use as default

  if(ScreenMode==MODE800X600X16)
      mode=_SVRES16COLOR;
  else
  if(ScreenMode==MODE1024X768X16)
      mode=_XRES16COLOR;
  // else
  // if(ScreenMode==MODE640X480X16)
  //    mode=_VRES16COLOR;
  // else  mode=_VRES16COLOR;

  setmode(mode);

  if (_grstatus()!=_GROK)
  {
     if(mode==_VRES16COLOR)
        return(-1);
     mode=_VRES16COLOR;
     ScreenMode=MODE640X480X16;
     setmode(mode);
     if (_grstatus()!=_GROK)
        return(-1);
  }

  switch(ScreenMode) {
    case MODE640X480X16:
        screendpi=89;
        break;
    case MODE800X600X16:
        screendpi=110;
        break;
    case MODE1024X768X16:
        screendpi=110;
        break;
   }


 if (getmaxx()>700) ResetDispMode(0);    // for s3
 if (getmaxx()>1000)
     outpw(0x3ce,0x106);

  //set_1_pal(EGA_DARKGRAY,35,35,37);
  //set_1_pal(EGA_LIGHTGRAY,49,49,51);

  {
    int i;
    for (i=0;i<16;i++)
      set_1_pal(i,RGB[i][0]>>2,RGB[i][1]>>2,RGB[i][2]>>2);
  }

  GlobalPageScale=(SCALEMETER*1/screendpi);
  Init_dc();
  /*
  {
  unsigned char buff[1000];
  setviewport(0,0,23,23,0);
  setfillstyle(1,1);
  bar(0,0,23,23);
  getimage(0,0,23,23,buff);
  getch();
  }
  */
  Reg.w.ax=0x484a;
  Reg.w.bx=0xff03;
  Reg.w.cx=1;
  int386(0x16,&Reg,&Reg);
  return(0);
}


int GraphFinish(void)
{
  SetDirectVideoStatus(1);    //for UCDOS
  closegraph();
  return(0);
}


//////////////////3D effect UI gadget drawing///////////////////////////

// Draw a UP 3D area
static void Area3DUp(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
            int Left,int Top,int Right,int Bottom,int Depth)
{
    int i;

     setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
     setcolor(EGA_WHITE);
     for (i=0;i<Depth;i++)
     {
         //line(Left+i,Top+i,Right-i,Top+i);
         scan_line(Left+i,Right-i,Top+i,EGA_WHITE);
         line(Left+i,Top+i,Left+i,Bottom-i);
     }
     setcolor(EGA_DARKGRAY);
     for (i=0;i<Depth;i++)
     {
         line(Right-i,Top+i,Right-i,Bottom-i);
         //line(Left+i,Bottom-i,Right-i,Bottom-i);
         scan_line(Left+i,Right-i,Bottom-i,EGA_DARKGRAY);
     }
     setfillstyle(1,EGA_LIGHTGRAY);
     bar(Left+Depth,Top+Depth,Right-Depth,Bottom-Depth);
}

// Draw a down 3D area
void Area3DDown(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
            int Left,int Top,int Right,int Bottom,int Depth)
{
    int i;
     setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
     setcolor(EGA_DARKGRAY);
     for (i=0;i<Depth;i++)
     {
         //line(Left+i,Top+i,Right-i,Top+i);
         scan_line(Left+i,Right-i,Top+i,EGA_DARKGRAY);
         line(Left+i,Top+i,Left+i,Bottom-i);
     }

     setcolor(EGA_WHITE);
     for (i=0;i<Depth;i++)
     {
         line(Right-i,Top+i,Right-i,Bottom-i);
         //line(Left+i,Bottom-i,Right-i,Bottom-i);
         scan_line(Left+i,Right-i,Bottom-i,EGA_WHITE);
     }
     setfillstyle(1,EGA_LIGHTGRAY);
     bar(Left+Depth,Top+Depth,Right-Depth,Bottom-Depth);
}

static void Area3DFlat(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
            int Left,int Top,int Right,int Bottom,int Depth)
{
    int i;
     setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
     setcolor(EGA_DARKGRAY);
     for (i=0;i<Depth;i++)
     {
         //line(Left+i,Top+i,Right-i,Top+i);
         scan_line(Left+i,Right-i,Top+i,EGA_DARKGRAY);
         line(Left+i,Top+i,Left+i,Bottom-i);
     }
     setcolor(EGA_LIGHTGRAY);
     for (i=0;i<Depth;i++)
     {
         line(Right-i,Top+i,Right-i,Bottom-i);
         //line(Left+i,Bottom-i,Right-i,Bottom-i);
         scan_line(Left+i,Right-i,Bottom-i,EGA_LIGHTGRAY);
     }
     //setfillstyle(1,EGA_LIGHTGRAY);
     bar(Left+Depth,Top+Depth,Right-Depth,Bottom-Depth);
}

// Draw a down 3D area with Bottom Color
void Area3DDownColor(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
            int Left,int Top,int Right,int Bottom,int Depth,int Color)
{
    int i;
     setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
     setcolor(EGA_DARKGRAY);
     for (i=0;i<Depth;i++)
     {
         //line(Left+i,Top+i,Right-i,Top+i);
         scan_line(Left+i,Right-i,Top+i,EGA_DARKGRAY);
         line(Left+i,Top+i,Left+i,Bottom-i);
     }
     setcolor(EGA_WHITE);
     for (i=0;i<Depth;i++)
     {
         line(Right-i,Top+i,Right-i,Bottom-i);
         //line(Left+i,Bottom-i,Right-i,Bottom-i);
         scan_line(Left+i,Right-i,Bottom-i,EGA_WHITE);
     }
     setfillstyle(1,Color);
     bar(Left+Depth,Top+Depth,Right-Depth,Bottom-Depth);
}

void Hline3DDown(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
            int x1,int x2,int y)
{
     setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
     /*-----------
     setcolor(EGA_DARKGRAY);
     line(x1,y,x2,y);
     setcolor(EGA_WHITE);
     line(x1,y+1,x2,y+1);
     ----------*/
     scan_line(x1,x2,y,EGA_DARKGRAY);
     scan_line(x1,x2,y+1,EGA_WHITE);
}

void Vline3DDown(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
            int x,int y1,int y2)
{
     setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
     setcolor(EGA_DARKGRAY);
     line(x,y1,x,y2);
     setcolor(EGA_WHITE);
     line(x+1,y1,x+1,y2);
}

/*----------
void Rectangle3DDown(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
            int x1,int y1,int x2,int y2)
{
     setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
     Vline3DDown(DrawLeft,DrawTop,DrawRight,DrawBottom,x1,y1,y2);
     Vline3DDown(DrawLeft,DrawTop,DrawRight,DrawBottom,x2-1,y1,y2);
     Hline3DDown(DrawLeft,DrawTop,DrawRight,DrawBottom,x1+1,x2,y1);
     Hline3DDown(DrawLeft,DrawTop,DrawRight,DrawBottom,x1+2,x2-1,y2-1);
}
--------*/
//////////////////////////////////////////////////////////////////////////////
static void DrawButton(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                int Left,int Top,int Right,int Bottom,int ButtonState)
{
  setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
  if (ButtonState & BUTTONISDOWN)          // pressed  flag
     Area3DFlat(DrawLeft,DrawTop,DrawRight,DrawBottom,Left,Top,Right,Bottom,2);
  else
     Area3DUp(DrawLeft,DrawTop,DrawRight,DrawBottom,Left,Top,Right,Bottom,2);

  setcolor(EGA_BLACK);
  rectangle(Left,Top,Right,Bottom);
}

void DrawButtonPolygon(int PointNumber,short *ButtonPoints)
{
  setcolor(BUTTONPOLYGONCOLOR);
//  drawpoly(PointNumber,ButtonPoints);         // ByHance
//  setfillstyle(1,BUTTONPOLYGONBKCOLOR);
  fillpoly(PointNumber,ButtonPoints);
}

#ifdef UNUSED
void DrawMaxButton(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                   int Left,int Top,int Right,int Bottom,int ButtonStyle)
{
  short ButtonPoints[6];

  /* Draw Bold */
  DrawButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
             Left,Top,Right,Bottom,ButtonStyle);

  /* Compute ^ */
  ButtonPoints[0]=(Left+Right)/2;
  ButtonPoints[1]=Top+SYSBUTTONSPACE;
  ButtonPoints[2]=Left+SYSBUTTONSPACE-2;
  ButtonPoints[3]=Bottom-SYSBUTTONSPACE;
  ButtonPoints[4]=Right-SYSBUTTONSPACE+2;
  ButtonPoints[5]=Bottom-SYSBUTTONSPACE;

  /* Draw ^ */
  DrawButtonPolygon(3,&ButtonPoints);
}

void DrawMinButton(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                   int Left,int Top,int Right,int Bottom,int ButtonStyle)
{
  short ButtonPoints[6];

  /* Draw Bold */
  DrawButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
             Left,Top,Right,Bottom,ButtonStyle);

  /* Compute v */
  ButtonPoints[0]=Left+SYSBUTTONSPACE-2;
  ButtonPoints[1]=Top+SYSBUTTONSPACE;
  ButtonPoints[2]=Right-SYSBUTTONSPACE+2;
  ButtonPoints[3]=Top+SYSBUTTONSPACE;
  ButtonPoints[4]=(Left+Right)/2;
  ButtonPoints[5]=Bottom-SYSBUTTONSPACE;

  /* Draw v */
  DrawButtonPolygon(3,&ButtonPoints);
}

void DrawSizeButton(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                    int Left,int Top,int Right,int Bottom,int ButtonStyle)
{
  short ButtonPoints[6];

  /* Draw Bold */
  DrawButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
             Left,Top,Right,Bottom,ButtonStyle);

  /* Compute ^ */
  ButtonPoints[0]=(Left+Right)/2;
  ButtonPoints[1]=Top+SYSBUTTONSPACE/2;
  ButtonPoints[2]=Left+SYSBUTTONSPACE-2;
  ButtonPoints[3]=Bottom-(Bottom-Top)/2-2;
  ButtonPoints[4]=Right-SYSBUTTONSPACE+2;
  ButtonPoints[5]=Bottom-(Bottom-Top)/2-2;

  /* Draw ^ */
  DrawButtonPolygon(3,&ButtonPoints);

  /* Compute v */
  ButtonPoints[0]=Left+SYSBUTTONSPACE-2;
  ButtonPoints[1]=Bottom-(Bottom-Top)/2+1;
  ButtonPoints[2]=Right-SYSBUTTONSPACE+2;
  ButtonPoints[3]=Bottom-(Bottom-Top)/2+1;
  ButtonPoints[4]=(Left+Right)/2;
  ButtonPoints[5]=Bottom-SYSBUTTONSPACE/2-1;

  /* Draw v */
  DrawButtonPolygon(3,&ButtonPoints);
}
#endif // UNUSED

void DrawMenuButton(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                    int Left,int Top,int Right,int Bottom,int ButtonStyle)
{
  short ButtonPoints[8];

  /* Draw Bold */
  DrawButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
             Left,Top,Right,Bottom,ButtonStyle);

  /* Compute - */
  ButtonPoints[0]=Left+SYSBUTTONSPACE/2+1;
  ButtonPoints[1]=(Bottom+Top-SYSBUTTONSPACE/2)/2;
  ButtonPoints[2]=Right-SYSBUTTONSPACE/2-1;
  ButtonPoints[3]=(Bottom+Top-SYSBUTTONSPACE/2)/2;
  ButtonPoints[4]=Right-SYSBUTTONSPACE/2-1;
  ButtonPoints[5]=(Bottom+Top-SYSBUTTONSPACE/2)/2+2;
  ButtonPoints[6]=Left+SYSBUTTONSPACE/2+1;
  ButtonPoints[7]=(Bottom+Top-SYSBUTTONSPACE/2)/2+2;

  /* Draw - */
  DrawButtonPolygon(4,&ButtonPoints);
}

void DrawLeftScrollButton(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                          int Left,int Top,int Right,int Bottom,int ButtonStyle)
{
  short ButtonPoints[6];
  int x,y;

  /* Draw Bold */
  DrawButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
             Left,Top,Right,Bottom,ButtonStyle);

  x = (Left+Right)/2;
  y = (Top+Bottom)/2;

  if (ButtonStyle & BUTTONISDOWN) {
       x++;y++;
  }
  /* Compute < */
  ButtonPoints[0]=x+1;
  ButtonPoints[1]=y-3;
  ButtonPoints[2]=x-2;
  ButtonPoints[3]=y;
  ButtonPoints[4]=x+1;
  ButtonPoints[5]=y+3;

  /* Draw < */
  DrawButtonPolygon(3,&ButtonPoints);
}

void DrawRightScrollButton(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                           int Left,int Top,int Right,int Bottom,int ButtonStyle)
{
  short ButtonPoints[6];
  int x,y;

  /* Draw Bold */
  DrawButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
             Left,Top,Right,Bottom,ButtonStyle);

  x = (Left+Right)/2;
  y = (Top+Bottom)/2;

  if (ButtonStyle & BUTTONISDOWN) {
       x++;y++;
  }
  /* Compute > */
  ButtonPoints[0]=x-1;
  ButtonPoints[1]=y-3;
  ButtonPoints[2]=x+2;
  ButtonPoints[3]=y;
  ButtonPoints[4]=x-1;
  ButtonPoints[5]=y+3;

  /* Draw > */
  DrawButtonPolygon(3,&ButtonPoints);
}

void DrawUpScrollButton(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                        int Left,int Top,int Right,int Bottom,int ButtonStyle)
{
  short ButtonPoints[6];
  int x,y;

  /* Draw Bold */
  DrawButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
             Left,Top,Right,Bottom,ButtonStyle);

  x = (Left+Right)/2;
  y = (Top+Bottom)/2;

  if (ButtonStyle & BUTTONISDOWN) {
       x++;y++;
  }
  /* Compute ^ */
  ButtonPoints[0]=x;
  ButtonPoints[1]=y-2;
  ButtonPoints[2]=x-3;
  ButtonPoints[3]=y+1;
  ButtonPoints[4]=x+3;
  ButtonPoints[5]=y+1;

  /* Draw ^ */
  DrawButtonPolygon(3,&ButtonPoints);
}

void DrawDownScrollButton(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                          int Left,int Top,int Right,int Bottom,int ButtonStyle)
{
  short ButtonPoints[6];
  int x,y;

  /* Draw Bold */
  DrawButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
             Left,Top,Right,Bottom,ButtonStyle);

  x = (Left+Right)/2;
  y = (Top+Bottom)/2;

  if (ButtonStyle & BUTTONISDOWN) {
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
  DrawButtonPolygon(3,&ButtonPoints);
}

void DrawScroll(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                int Left,int Top,int Right,int Bottom)
{
   Area3DDown(DrawLeft,DrawTop,DrawRight,DrawBottom,Left,Top,Right,Bottom,2);
}

void DrawScrollButton(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                      int Left,int Top,int Right,int Bottom,int ButtonState)
{
  DrawButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
             Left,Top,Right,Bottom,ButtonState);
}

void DrawTitleBar(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                  int Left,int Top,int Right,int Bottom,char *Title)
{
  int AdjustPostion;
  char Text[128];

  setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
  setfillstyle(1,WINDOWTITLEBKCOLOR);
  bar(Left,Top,Right,Bottom);

  if (!Title[0])
     return;
  strncpy(Text,Title,(Right-Left)/8);
  AdjustPostion=Left+((Right-Left)-strlen(Text)*8)/2;
  ViewportDisplayString(Text,AdjustPostion,Top+2,WINDOWTITLECOLOR,WINDOWTITLEBKCOLOR);
}

void DrawWindow(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                int Left,int Top,int Right,int Bottom,int LineSpace)
{
  if (LineSpace==0)
     return;

//  setcolor(WINDOWBOLDCOLOR);
 // setfillstyle(1,WINDOWBKCOLOR);
//  bar(DrawLeft,DrawTop,DrawRight,DrawBottom);

  Area3DUp(DrawLeft,DrawTop,DrawRight,DrawBottom,
           Left-DrawLeft,Top-DrawTop,Right-DrawLeft,Bottom-DrawTop,
           LineSpace);
  ////GGGGGGGGGGGGGGG////////////
  setcolor(EGA_DARKGRAY);
  rectangle(Left-DrawLeft,Top-DrawTop,Right-DrawLeft,Bottom-DrawTop);
} /* DrawWindow */


#define UnLockBmpBuf(handle)

static unsigned char *LockBmpBuf(int handle,int pressed)
{
         // handle :: release ,    (handle+1) :: press
      if(pressed) handle++;
// #define OLDVERSION
 #ifdef OLDVERSION
      return(&BmpBuf[handle*288]);      // 288=24*24/2
 #else
      return(&BmpBuf[handle*BmpImageLen]);      // 288=24*24/2
 #endif
}

//static mmy=0;
void DrawIcon(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
              int Left,int Top,int ButtonState,int Handle)
{
   char *pBuf;
   int pressed=ButtonState & BUTTONISDOWN;         // Button is pressed ?

   setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);

   pBuf=LockBmpBuf(Handle,pressed);

 #ifdef OLDVERSION
   int i,j;
   int from;
   unsigned short colr;
   for(i=0;i<TOOLBARHEIGHT;i++) {
       unsigned short cc;
       from=0;     cc=(*pBuf&0xf0)>>4;
       for(j=1;j<TOOLBARWIDTH;j++) {
           colr=(j&1)?(*pBuf++)&0xf:(*pBuf&0xf0)>>4;
           if(cc==colr && j<TOOLBARWIDTH-1)
                continue;

           if(j==TOOLBARWIDTH-1) j++;
           if(cc==colr)
                continue;
          if(cc==7) cc=8;
           else if(cc==8) cc=7;

          /*-----------
           setcolor(cc);
           line(Left+from,Top+i,Left+j-1,Top+i);
           ------------*/
           scan_line(Left+from,Left+j-1,Top+i,cc);

           cc=colr;     from=j;
        } /*--- j ---*/
   }
 #else


   ///////////////GGGGGGGGGGGGGG//////////////
   ////test;
   #ifdef UNUSE
   if (mmy==0)
   {
    int i,j,k;
    UCHAR co;
    for (k=0;k<40;k++)
    {
    putimage(Left,Top,BmpBuf+k*BmpImageLen,COPY_PUT);
    for (i=0;i<TOOLBARWIDTH;i++)
     for (j=0;j<TOOLBARHEIGHT;j++)
      {
           co=_getpixel(i,j);
           if (co==7) {
                       setcolor(8);
                       _setpixel(i,j);
                      }
           else
           if (co==8)
                      {
                       setcolor(7);
                       _setpixel(i,j);
                      }
      }
   getimage(Left,Top,Left+TOOLBARWIDTH-1,Top+TOOLBARHEIGHT-1,BmpBuf+k*BmpImageLen);
   }
   mmy++;
     {
        FILE *fp;
        fp=fopen("c:\\ezp\\fonts\\Icon","wb");
        fwrite(BmpBuf,1,40*BmpImageLen,fp);
        fclose(fp);
     }
   }
   #endif
   putimage(Left,Top,pBuf,COPY_PUT);
 #endif

   UnLockBmpBuf(Handle);
} /* DrawIcon */

 #ifdef OLDVERSION
DrawIconImage()
{
   int i,len;
   FILE *fp;
   char buf[1000];

   len=imagesize(0,0,TOOLBARWIDTH-1,TOOLBARHEIGHT-1);

   setviewport(0,0,639,400,1);
   setcolor(EGA_WHITE);
   fp=fopen("icon.img","wb");

   for(i=0;i<TotalIconNumber;i++)
   {
        bar(100,100,160,160);
        DrawIcon(0,0,639,400,100,100,~BUTTONISDOWN,2*i);
        getimage(100,100,100+TOOLBARWIDTH-1,100+TOOLBARHEIGHT-1,buf);
        fwrite(buf,len,1,fp);

        bar(100,100,160,160);
        DrawIcon(0,0,639,400,100,100,BUTTONISDOWN,2*i);
        getimage(100,100,100+TOOLBARWIDTH-1,100+TOOLBARHEIGHT-1,buf);
        fwrite(buf,len,1,fp);
   }

   fclose(fp);
}
 #endif

void DrawUserButton(int DrawLeft,int DrawTop,int DrawRight,int DrawBottom,
                    int Left,int Top,int Right,int Bottom,int ButtonState,
                    char *ButtonText)
{
  int AdjustHPostion,AdjustVPostion;
  char Text[128];

  if (ButtonState & WindowSetIsIcon())   {   // Button is IconButton
     DrawIcon(DrawLeft,DrawTop,DrawRight,DrawBottom,
                Left,Top,ButtonState,ButtonText[0]);
     return;
  }

  /* Draw Bold */
  DrawButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
             Left,Top,Right,Bottom,ButtonState);

  if (!ButtonText[0])
     return;
  strncpy(Text,ButtonText,(Right-Left)/8);
  AdjustHPostion=Left+((Right-Left)-strlen(Text)*CHARWIDTH/2)/2;
  AdjustVPostion=Top+((Bottom-Top)-CHARHEIGHT)/2;

  if (ButtonState & BUTTONISDOWN)   // Button pressed
  {
      AdjustHPostion+=1;
      AdjustVPostion+=1;
  }

  ViewportDisplayString(Text,AdjustHPostion,AdjustVPostion,
                BUTTONCOLOR,BUTTONBKCOLOR);

  if (ButtonState&BUTTONGETFOCUS)
  {
    #ifdef __TURBOC__
         struct linesettingstype SaveLine;
         getlinesettings(&SaveLine);
         setlinestyle(1,0,1);
    #else
         unsigned old_style=getlinestyle();
         setlinestyle(0x5555);
    #endif

    setcolor(BUTTONBOLDCOLOR2);
    rectangle(AdjustHPostion-3,AdjustVPostion-2,
              AdjustHPostion+strlen(Text)*CHARWIDTH/2+3,
              AdjustVPostion+CHARHEIGHT+2);

    #ifdef __TURBOC__
         setlinestyle(SaveLine.linestyle,SaveLine.upattern,SaveLine.thickness);
    #else
         setlinestyle(old_style);
    #endif
   }
}


void scan_line(int x1,int x2,int y,int color)
{
   char *p;
   int xx1,xx2,y1;
   int x1bit,x2bit,x1byte,x2byte;
   unsigned char maskbyte;
   struct viewporttype vp;

   //setcolor(color);       //for test
   //line(x1,y,x2,y);
   //return ;

   getviewsettings(&vp);
   y1 = y+vp.top;
   xx1 = x1+vp.left;
   xx2 = x2+vp.left;
   if (y1<vp.top||y1>=vp.bottom||xx1>=vp.right||xx2<vp.left) return;

   if (xx1<vp.left) xx1 = vp.left;
   if (xx2>=vp.right) xx2 = vp.right;

//start device
   outpw(0x3ce,0x205);
   outpw(0x3ce,0x3);                    // PUT mode added by jerry

// reset ega
   outpw(0x3ce,0x0005);
   outpw(0x3ce,0xff08);
   outpw(0x3ce,0x0f07);
   outpw(0x3ce,3);

   outpw(0x3ce,color<<8);
   outpw(0x3ce,0xf01);
   outpw(0x3ce,0x3);                    // PUT mode added by jerry
   outpw(0x3c4,0xf02);

   x1byte = xx1 >> 3;
   x1bit = xx1 & 7;
   x2byte = xx2 >> 3;
   x2bit = xx2 & 7;

   outpw(0x3ce,0xf01);
   outpw(0x3c4,0xf02);
   p = (char *)(0xa0000+y1*(getmaxx()+1)/8+x1byte);

   outp(0x3ce,8);
   if (x1byte==x2byte) {
      maskbyte = headdot[x1bit]&taildot[x2bit];
      // outp(0x3ce,8);           outp(0x3cf,maskbyte);
      outp(0x3cf,maskbyte);
      *p |= maskbyte;
   } else {
          maskbyte = headdot[x1bit];
          outp(0x3cf,maskbyte);
          *p++ |= maskbyte;

          outp(0x3cf,0xff);
          while (++x1byte<x2byte){
            *p++ |= 0xff;
          }

          maskbyte = taildot[x2bit];
          outp(0x3cf,maskbyte);
          *p |= maskbyte;
   }

   outpw(0x3ce,0x0005);
   outpw(0x3ce,0xff08);
   outpw(0x3ce,0x0f07);
   outpw(0x3ce,3);

   outpw(0x3ce,0xff08); //  outp(0x3cf,0xff);
   outpw(0x3ce,1);  // outp(0x3cf,0);
}

void copymono(char *buf,int x,int y,int w,int h,int color)
{
   register char *p,*p1,*q,*q1;
   int x1,x2,lines,y1,y2,startx,starty,startbit;
   int x1byte,x2byte,byte1,byte2;
   int wbyte,BytePerLine;
   unsigned char maskbyte,cc,cc1,bit,bit1,tbit,x1bit,x2bit;
   struct viewporttype vp;

   getviewsettings(&vp);
   x1 = x+vp.left;
   y1 = y+vp.top;
   x2 = x1+w-1;
   y2 = y1+h-1;

   if (x1>=vp.right || x2<vp.left || y1>=vp.bottom || y2<vp.top) return;

   startx = starty = 0;

   if (y1<vp.top) {
           starty = vp.top-y1;
           y1 = vp.top;
   }

   if (x1<vp.left) {
           startx = vp.left - x1;
           startbit = startx & 7;
           x1 = vp.left;
   }

   if (y2>=vp.bottom) y2 = vp.bottom;
   if (x2>vp.right) x2 = vp.right;

   outpw(0x3ce,color<<8);
   outpw(0x3ce,0xf01);
   outpw(0x3c4,0xf02);
   outp(0x3ce,8);


   byte1 = x1byte = x1 >> 3;
   x1bit = x1 & 7;
   byte2 = x2byte = x2 >> 3;
   x2bit = x2 & 7;
   wbyte = (w+7)>>3;

   bit1 =  hdot[x1bit];
   tbit =  taildot[x2bit];
   bit = 8-x1bit;

   // p1 = 0xa0000+y1*80+x1byte;
   BytePerLine=(getmaxx()+1)/8;
   p1 = (char *)(0xa0000+y1*BytePerLine+x1byte);
   q1 = buf+starty*wbyte+((startx)>>3);

   if (!startx) {
           for (lines=y1;lines<=y2;lines++) {
               p = p1;
               q = q1;
               if (x1byte == x2byte) {
                   maskbyte = ((*q)>>x1bit) & tbit;
                   outp(0x3cf,maskbyte);
                   *p |= maskbyte;
               } else {
                   maskbyte = *q++;
                   cc = maskbyte & bit1;
                   maskbyte >>= x1bit;
                   outp(0x3cf,maskbyte);
                   *p++ |= maskbyte;

                   while (++x1byte<x2byte){
                     maskbyte = *q++;
                     cc1 = maskbyte & bit1;
                     maskbyte = (maskbyte>>x1bit) | (cc<<bit);
                     outp(0x3cf,maskbyte);
                     *p++ |= maskbyte;                      //latch !!!
                     cc = cc1;
                   }

                   maskbyte = ( ((*q++)>>x1bit) | (cc<<bit) )   &  tbit;
                   outp(0x3cf,maskbyte);
                   *p |= maskbyte;
              }
              p1 += BytePerLine;                 //next crt line
              q1 += wbyte;              //next buf line
              x1byte = byte1;
              x2byte = byte2;
           }
   } else  {
           for (lines=y1;lines<=y2;lines++) {
               p = p1;
               q = q1;
               if (x1byte == x2byte) {
                   maskbyte = (*q)>>startbit;
                   maskbyte = (maskbyte>>x1bit) & tbit;
                   outp(0x3cf,maskbyte);
                   *p |= 0xff;
               } else {
                   maskbyte = ((*q)<<startbit) | ( (*(q+1)) >> (8-startbit));
                   q++;
                   cc = maskbyte & bit1;
                   maskbyte >>= x1bit;
                   outp(0x3cf,maskbyte);
                   *p++ |= 0xff;

                   while (++x1byte<x2byte){
                     maskbyte = ((*q)<<startbit) | ( (*(q+1)) >> (8-startbit));
                     q++;
                     cc1 = maskbyte & bit1;
                     maskbyte = (maskbyte>>x1bit)|(cc<<bit);
                     outp(0x3cf,maskbyte);
                     *p++ |= 0xff;                      //latch !!!
                     cc = cc1;
                   }

                   maskbyte = ((*q)<<startbit) | ( (*(q+1)) >> (8-startbit));
                   maskbyte = ((maskbyte>>x1bit)|(cc<<bit)) & tbit;
                   outp(0x3cf,maskbyte);
                   *p |= 0xff;
              }
              p1 += BytePerLine;                 //next crt line
              q1 += wbyte;              //next buf line
              x1byte = byte1;
              x2byte = byte2;
           }
   }

//-- copy_end: ----
   outp(0x3cf,0xff);
   outpw(0x3ce,1);  // outp(0x3cf,0);
}

static char * cnum[]={
    "○","一","二","三","四","五","六","七","八","九",
    "十","十一","十二","十三","十四","十五","十六","十七","十八","十九",
    "二十","二十一","二十二","二十三","二十四","二十五","二十六","二十七",
    "二十八","二十九", "三十","三十一",
};
static char cnumstr[16];

static char *Year2Chinese(int year)
{
  int i,j;
  int flag;

  cnumstr[0]=flag=0;

  i = year;
  j = i/1000;
  if (j!=0) {
      strcpy(cnumstr,cnum[j]);
      flag = 1;
  }

  i %= 1000;
  j = i/100;
  if (j!=0 || flag) {
      strcat(cnumstr,cnum[j]);
      flag = 1;
  }

  i %= 100;
  j = i/10;
  if (j!=0 || flag) {
      strcat(cnumstr,cnum[j]);
      flag = 1;
  }

  i %= 10;
  if (i!=0 || flag)
      strcat(cnumstr,cnum[i]);
  return cnumstr;
}

static char *MD2Chinese(int md)
{
  int i;
  i = md % 32;
  strcpy(cnumstr,cnum[i]);
  return cnumstr;
}

char *GetDateString()
{
  static char str[30];
  time_t t;
  char *p;
  int year,month,date;
  struct tm *today;

  t = time(NULL);
  today = localtime(&t);
  year = today->tm_year+1900;
  month = today->tm_mon+1;
  date = today->tm_mday;

  p = Year2Chinese(year);
  strcpy(str,p);
  strcat(str,"年");

  p = MD2Chinese(month);
  strcat(str,p);
  strcat(str,"月");

  p = MD2Chinese(date);
  strcat(str,p);
  strcat(str,"日");

  return str;
}

/*------- Star CR3240 dot maxtrix Color Printer : 180DPI ----------*/
/*-------------------------------------------------------------------
* Name: devstarc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern int lqline;
extern unsigned char *lqbuffer;
extern int blanklines;
static void flush_star_line();
static void send_star_line(LPBYTE cbuf,LPBYTE mbuf,
                         LPBYTE ybuf,LPBYTE kbuf,int cnt);

static int starCR_init(UDATA pagew,UDATA pageh);
static void starCR_FF()
{
   if (lqline) flush_star_line();
   blanklines = 0;
   fputc(0x0c,prnstr);        //FORM FEED
}

static void starCR_over()
{
     free(lqbuffer);
     free(rasts[0]);
     CloseDitherBuf();
     fclose(prnstr);
     RestoreColorForRBYKDotMatrixPrinter();
}

static int starCR_getheight() { return RastHeight; }
static void starCR_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   RGB_scanline(x1,x2,y,lpdc);
}

static void starCR_block()
{
   LPDC lpdc=&SysDc;
   int  i,byteoff;

   if(fDither) DitherRGB(lpdc);    // dither RGB to CMYK
   else memset(rasts[0],0,RastHeight*RastWidthByte*4);

   byteoff = 0;
   for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
   {
      send_star_line(rasts[0]+byteoff,
                   rasts[1]+byteoff,
                   rasts[2]+byteoff,
                   rasts[3]+byteoff, RastWidthByte);

      byteoff += RastWidthByte;
   }
   //if(fDither)
   memset(rasts[4],0xff,RastSize);        // clear RGB buffer
}

static void starCR_setcolor(int color)
{
   setSYScolor(color);
}
static void starCR_setRGBcolor(int r,int g,int b)
{
   setRGBcolor(r,g,b);
}
static void starCR_setCMYKcolor(int c,int m,int y,int k)
{
   setCMYKcolor(c,m,y,k);
}
static void starCR_setgray(int gray)
{
   setGray(gray);
}

void Zhuan24(LPBYTE cbuf,LPBYTE mbuf,LPBYTE ybuf,LPBYTE kbuf,int cnt,int LinesPerRow);
static void send_star_line(LPBYTE cbuf,LPBYTE mbuf,LPBYTE ybuf,LPBYTE kbuf,int cnt)
{
   Zhuan24(cbuf,mbuf,ybuf,kbuf,cnt,24);

   lqline++;
   if (lqline==24)
   {
       lqline=0;
       flush_star_line();
   }
}

void epsonLQ_skip_line(int lines);

static void flush_star_line()
{
    int i;
    int len;
    static int color[]={ 2, 1, 4, 0 };
    // static unsigned char color_cmd[]="\033r\002((c))\002\xd\033*\x27";
    static unsigned char color_cmd[]="\033r\002\xd\033*\x27";

    if( lqbuffer[0]==0 && !memicmp(lqbuffer,lqbuffer+1,512*24*4-1) )
    {
       blanklines++;
       return;
    }

    if(blanklines)
    {
       epsonLQ_skip_line(blanklines);
       blanklines=0;
    }

    for(i=0;i<4;i++)
    {
        len = 512*24;
        while (len>0&&lqbuffer[len-1+i*512*24]==0) len--;
        if(len)
        {
            len = len/3+1;
            //color_cmd[2]=color_cmd[8]=color[i];
            //fwrite(color_cmd,1,13,prnstr);
            color_cmd[2]=color[i];
            fwrite(color_cmd,1,7,prnstr);
            fputc(len%256,prnstr); fputc(len/256,prnstr);
            fwrite(&lqbuffer[i*512*24],1,len*3,prnstr);
        }
    }

    // free a line
    epsonLQ_skip_line(1);

 //  for (i=0;i<512*24*4;i++) lqbuffer[i]=0;
    memset(lqbuffer,0,512*24*4);
}

PRINTER StarCRprinter = {
  DEV_CMYK,
  starCR_init,
  starCR_block,
  starCR_FF,
  starCR_over,
  starCR_getheight,
  starCR_scanfill,
  starCR_setcolor,
  starCR_setRGBcolor,
  starCR_setCMYKcolor,
  starCR_setgray,
  180,
  18*85,                // 180x8.5
  18*108,               // 180*10.8        // height%24 must be 0
  0,0,
};

static int starCR_init(UDATA pagew,UDATA pageh)
{
   int m,lines;

   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);
   fputs("\033@", prnstr);               /* reset printer */

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;

   // lines= printer->ypixel+32;
   //lines=(16*1024*1024)/(4*RastWidthByte+3*RastWidth)/4+32;
            // for other malloc's space   -----------^
   lqbuffer=malloc(512*24*4);
   if(lqbuffer==NULL)
       goto err_exit;

   lines=128+24;
   rasts[0]=NULL;
   while (rasts[0]==NULL && lines>24)
   {
       lines -= 24;
       m = lines*(4*RastWidthByte+3*RastWidth);
       rasts[0] = malloc(m);
   }

   if(rasts[0]==NULL)
   {
    err_exit:
       fclose(prnstr);
       return(-1);
   }

   RastHeight=lines;
   m=lines*RastWidthByte;                // cmyk
   rasts[1] = rasts[0]+m;
   rasts[2] = rasts[1]+m;
   rasts[3] = rasts[2]+m;
   rasts[4] = rasts[3]+m;

   m=lines*RastWidth;                    // rgb
   RastSize = m*3;
   rasts[5] = rasts[4]+m;
   rasts[6] = rasts[5]+m;
   memset(rasts[4],0xff,RastSize);         // clear it

 //  for (i=0;i<512*24*4;i++) lqbuffer[i]=0;
   memset(lqbuffer,0,512*24*4);
   lqline = blanklines = 0;
   fRemapRGB = FALSE;

   m=InitDitherBuf();
   if(m<0)
   {
       free(lqbuffer);
       free(rasts[0]);
       fclose(prnstr);
       return m;
   }

   ChangeColorForRBYKDotMatrixPrinter();
   return 1;
}

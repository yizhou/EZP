/*------- Epson dot maxtrix Color Printer : 360DPI ----------*/
/*-------------------------------------------------------------------
* Name: devlq24.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

int lqline = 0;
//unsigned char lqbuffer[512*48*4+20];
 unsigned char *lqbuffer;
static void flush_lq_line();
extern int blanklines;

static int lq24_init(UDATA pagew,UDATA pageh)
{
   int m,lines;

   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);
   // reset printer
   fwrite("\033@\033\x1a\x11\000\033\x32\033CF\033\x19\004\x12\033P",
         1, 17, prnstr);

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;

   // lines= printer->ypixel+32;
   //lines=(16*1024*1024)/(4*RastWidthByte+3*RastWidth)/4+32;
            // for other malloc's space   -----------^
   lines=100+48;
   rasts[0]=NULL;
   lqbuffer=malloc(512*48*4);
   if(lqbuffer==NULL)
       goto err_exit;

   while (rasts[0]==NULL && lines>48)
   {
       lines -= 48;
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

 //  for (i=0;i<512*48*4;i++) lqbuffer[i]=0;
   memset(lqbuffer,0,512*48*4);
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

static void lq24_FF()
{
   if (lqline) flush_lq_line();
   blanklines = 0;
   fputc(0x0c,prnstr);        //FORM FEED
}

static void lq24_over()
{
     free(lqbuffer);
     free(rasts[0]);
     CloseDitherBuf();
     fputs("\033@", prnstr);
     fclose(prnstr);
     RestoreColorForRBYKDotMatrixPrinter();
}

static int lq24_getheight() { return RastHeight; }
static void lq24_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   RGB_scanline(x1,x2,y,lpdc);
}

static void send_lq_line(LPBYTE cbuf,LPBYTE mbuf,
                              LPBYTE ybuf,LPBYTE kbuf,int cnt);
static void lq24_block()
{
   LPDC lpdc=&SysDc;
   int  i,byteoff;

   if(fDither) DitherRGB(lpdc);    // dither RGB to CMYK
   else memset(rasts[0],0,RastHeight*RastWidthByte*4);

   byteoff = 0;
   for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
   {
      send_lq_line(rasts[0]+byteoff,
                   rasts[1]+byteoff,
                   rasts[2]+byteoff,
                   rasts[3]+byteoff, RastWidthByte);

      byteoff += RastWidthByte;
   }
   //if(fDither)
   memset(rasts[4],0xff,RastSize);        // clear RGB buffer
}

static void lq24_setcolor(int color)
{
   setSYScolor(color);
}
static void lq24_setRGBcolor(int r,int g,int b)
{
   setRGBcolor(r,g,b);
}
static void lq24_setCMYKcolor(int c,int m,int y,int k)
{
   setCMYKcolor(c,m,y,k);
}
static void lq24_setgray(int gray)
{
   setGray(gray);
}

void Zhuan24(LPBYTE cbuf,LPBYTE mbuf,LPBYTE ybuf,LPBYTE kbuf,int cnt,int LinesPerRow)
{
   int i,j,n,cn,mn,yn,kn;
   int cc,mm,yy,kk;
   int line,bit;
   int x,BytesPerRow;

   n = min(cnt,512);
   line = lqline/8;
   bit = lqline%8;
   cc=mm=yy=kk=1;
   if( cbuf[0]==0 && !memicmp(cbuf,cbuf+1,n-1) )  cc=0;
   if( mbuf[0]==0 && !memicmp(mbuf,mbuf+1,n-1) )  mm=0;
   if( ybuf[0]==0 && !memicmp(ybuf,ybuf+1,n-1) )  yy=0;
   if( kbuf[0]==0 && !memicmp(kbuf,kbuf+1,n-1) )  kk=0;

   if(cc||mm||yy||kk)
   {
      BytesPerRow=LinesPerRow/8;

      for (i=0;i<n;i++)
         for (j=0;j<8;j++) {
             x = i*8+j;
             cn = x*BytesPerRow+line;
             mn = cn+512*LinesPerRow;
             yn = mn+512*LinesPerRow;
             kn = yn+512*LinesPerRow;

             if (cbuf[i]&dot1tab[j])
                 lqbuffer[cn] |= dot1tab[bit];

             if (mbuf[i]&dot1tab[j])
                 lqbuffer[mn] |= dot1tab[bit];

             if (ybuf[i]&dot1tab[j])
                 lqbuffer[yn] |= dot1tab[bit];

             if (kbuf[i]&dot1tab[j])
                 lqbuffer[kn] |= dot1tab[bit];
         }
   }
}

static void send_lq_line(LPBYTE cbuf,LPBYTE mbuf,LPBYTE ybuf,LPBYTE kbuf,int cnt)
{
   Zhuan24(cbuf,mbuf,ybuf,kbuf,cnt,48);

   lqline++;
   if (lqline==48)
   {
       lqline=0;
       flush_lq_line();
   }
}

static void epsonLQ24_skip_line(int lines)
{
    int i;

    for(i=0;i<lines;i++)
      fwrite("\033+\x30\x0a",1,4,prnstr);
}

static void flush_lq_line()
{
    int i,len;
    static int color[]={ 2, 1, 4, 0 };
    static unsigned char color_cmd[]="\033r\002\x0d\033*\x48";

    if( lqbuffer[0]==0 && !memicmp(lqbuffer,lqbuffer+1,512*48*4-1) )
    {
       blanklines++;
       return;
    }

    if(blanklines)
    {
       epsonLQ24_skip_line(blanklines);
       blanklines=0;
    }

    for(i=0;i<4;i++)
    {
        len = 512*48;
        while (len>0&&lqbuffer[len-1+i*512*48]==0) len--;
        if(len)
        {
            len = len/6+1;
            color_cmd[2]=color[i];
            fwrite(color_cmd,1,7,prnstr);
            fputc(len%256,prnstr); fputc(len/256,prnstr);
            fwrite(&lqbuffer[i*512*48],1,len*6,prnstr);
        }
    }

    // free a line
    epsonLQ24_skip_line(1);

 //  for (i=0;i<512*48*4;i++) lqbuffer[i]=0;
    memset(lqbuffer,0,512*48*4);
}

PRINTER LQColorprinter = {
  DEV_CMYK,
  lq24_init,
  lq24_block,
  lq24_FF,
  lq24_over,
  lq24_getheight,
  lq24_scanfill,
  lq24_setcolor,
  lq24_setRGBcolor,
  lq24_setCMYKcolor,
  lq24_setgray,
  360,
  36*85,                // 360x8.5
  36*108,               // 360*10.8        // height%48 must be 0
  0,0,
};


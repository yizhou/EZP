/*--------- HP: A4,A3, 300, 600DPI ----------*/
/*-------------------------------------------------------------------
* Name: devhp.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#define USE_COMPRESS

#ifdef USE_COMPRESS
  static char a3_init_str[]="\033&l0E\033*p0x0Y\033*r0A\033*b2M";
  static char a4_init_str[]="\033&l26a0E\033*p0x0Y\033*r0A\033*b2M";
  static char a42_init_str[]="\033&l26a0E\033*p0X\033*p0Y\033*r0A";          //By zjh for hp2
  static char *init_str;
#else
  static char a3_init_str[]="\033&l0E\033*p0X\033*p0Y\033*r0A";
  static char a4_init_str[]="\033&l26a0E\033*p0X\033*p0Y\033*r0A";
  static char *init_str;
#endif

static char copyn_str[]="\033&l001X";
static char resolution_str[]="\033*t600R";
extern int blanklines;

static int hp_init()
{
   int m,lines;

  //_setvideomode(0x12);
   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);
   fputs("\033E", prnstr);               /* reset printer */
   if(printer->resolution==300)
      resolution_str[3]='3';
   else
      resolution_str[3]='6';
   fputs(resolution_str, prnstr);          /* resolution */
   fputs(init_str, prnstr);              /* init string */

   if(PrintCopyN>1) {
        if(PrintCopyN>999) PrintCopyN=999;
        copyn_str[3]=(PrintCopyN/100)+'0';
        copyn_str[4]=((PrintCopyN%100)/10)+'0';
        copyn_str[5]=(PrintCopyN%10)+'0';
        fputs(copyn_str, prnstr);
   }

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;
   //lines= printer->ypixel+32;
   lines=512+32;
   //lines=(16*1024*1024)/(4*RastWidthByte+3*RastWidth)/4+32;
            // for other malloc's space   -----------^
   rasts[0]=NULL;

   while (rasts[0]==NULL && lines>32)
   {
       lines -= 32;
       m = lines*RastWidthByte;
       rasts[0] = malloc(m);
   }

   if(rasts[0]==NULL)
   {
       fclose(prnstr);
       return(-1);
   }

   RastHeight=lines;
   RastSize=m;
   memset(rasts[0],0,RastSize);         // clear it
   blanklines = 0;
   fFirstBlock=TRUE;
   return 1;
}

static int hp_a4_init(UDATA pagew,UDATA pageh) { init_str = a4_init_str; return(hp_init()); }
static int hp_a3_init(UDATA pagew,UDATA pageh) { init_str = a3_init_str; return(hp_init()); }
static int hp2_a4_init(UDATA pagew,UDATA pageh) { init_str = a42_init_str; return(hp_init()); }

static void hp_FF()
{
//   fputc(0x0c,prnstr);        //FORM FEED
//   fputs("\033&l0H",prnstr);        //FORM FEED
   fputs("\f\033E", prnstr);               /* reset printer */
   fputs(resolution_str, prnstr);          /* resolution */
   fputs(init_str, prnstr);              /* init string */

   if(PrintCopyN>1) fputs(copyn_str, prnstr);

   fFirstBlock=TRUE;
   blanklines = 0;
}

static void hh_FF()
{
   fputc(0x0c,prnstr);                     //FORM FEED

   //fputs("\f\033E", prnstr);               /* reset printer */
   //fputs(resolution_str, prnstr);          /* resolution */
   //fputs(init_str, prnstr);                /* init string */

   //if(PrintCopyN>1) fputs(copyn_str, prnstr);

   fFirstBlock=TRUE;
   blanklines = 0;
}

static void hp_over()
{
    //fputs("\033*rB\033E",prnstr);
     fputs("\033E",prnstr);
     fclose(prnstr);
     free(rasts[0]);
}

static int hp_getheight() { return RastHeight; }
static void hp_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   //setcolor(12);
   //line(x1/12,y/12,x2/12,y/12);
   BW_scanline(x1,x2,y,lpdc);
}

static void hp_block()
{
    int  i;
    // int blanklines = 0;
    char *linebuf, *end_word;
    int  out_count;
    unsigned char outbuf[0x1280];
    LPDC lpdc=&SysDc;

    for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
    {
         linebuf=&rasts[0][(i-lpdc->top)*RastWidthByte];
         end_word=linebuf+RastWidthByte;

#ifdef USE_COMPRESS
          // check if this line is zero line, from right side to left
         while ( end_word > linebuf && end_word[-1] == 0 ) end_word-- ;

         if ( end_word == linebuf) {    /* Blank line */
              blanklines++;
              continue;
         }
#endif

 #define  SkipByData
 #ifdef SkipByData
         if(fFirstBlock) {
            // for(;blanklines;blanklines--) fputs("\033*bW",prnstr);
            fputs("\033*bW",prnstr);    // ByHance, 96,7.3
            fFirstBlock=FALSE;
         }
      //   else         // ByHance, 96,7.3

  ////////if there are blank lines , skip the blank lines///////////////
         if( blanklines>0 )  {    /* move down from current position */
              fprintf(prnstr,"\033*b%dY",blanklines);
              blanklines=0;
         }
 #else
         if( blanklines>0 )   { /* move down from current position */
              fprintf(prnstr,"\033*p+%dY", blanklines);
              blanklines=0;
         }
 #endif

  #ifdef USE_COMPRESS
         out_count= RLEcompress((DWORD *)linebuf,(DWORD *)end_word,(char *)outbuf);
  #else
         out_count=end_word-linebuf;
         memcpy(outbuf,linebuf,out_count);
  #endif // USE_COMPRESS

         fprintf(prnstr, "\033*b%dW",out_count);
         fwrite(outbuf,sizeof(char),out_count,prnstr);
    } /*--- i ---*/

   /*-----------------
    if( lpdc->bottom<printer->ypixel && blanklines>0 ) {
         if(fFirstBlock) {
            if(blanklines<RastHeight) fFirstBlock=FALSE;
            for(;blanklines;blanklines--) fputs("\033*bW",prnstr);
         }
         else
         {   // if there are blank lines , skip the blank lines
              fprintf(prnstr,"\033*b%dY",blanklines);
              blanklines=0;
         }
    }
   -------------*/

   // if(fDither)
    memset(rasts[0],0,RastSize);        // clear buffer
} /* hp_block */

static void hp2_block()
{
    int  i;
    char *linebuf, *end_word;
    int  out_count;
    LPDC lpdc=&SysDc;

    for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
    {
         linebuf=&rasts[0][(i-lpdc->top)*RastWidthByte];
         end_word=linebuf+RastWidthByte;

         if(fFirstBlock) {
            fputs("\033*bW",prnstr);    // ByHance, 96,7.3
            fFirstBlock=FALSE;
         }

         if( blanklines>0 )  {    /* move down from current position */
              fprintf(prnstr,"\033*b%dY",blanklines);
              blanklines=0;
         }

         out_count=end_word-linebuf;
         fprintf(prnstr, "\033*b%dW",out_count);
         fwrite(linebuf,sizeof(char),out_count,prnstr);
    } /*--- i ---*/

    memset(rasts[0],0,RastSize);        // clear buffer
} /* hp2_block */

#ifdef USE_LX
static void lx_block()
{
    int  i;
    char *linebuf, *end_word;
    int  out_count;
    unsigned char outbuf[0x1280];
    LPDC lpdc=&SysDc;

    for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
    {
         linebuf=&rasts[0][(i-lpdc->top)*RastWidthByte];
         end_word=linebuf+RastWidthByte;

          // check if this line is zero line, from right side to left
         while ( end_word > linebuf && end_word[-1] == 0 ) end_word-- ;

         if ( end_word == linebuf) {    /* Blank line */
              blanklines++;
              continue;
         }

         if(fFirstBlock) {
            fputs("\033*bW",prnstr);    // ByHance, 96,7.3
            fFirstBlock=FALSE;
         }

         if( blanklines>0 )   { /* move down from current position */
              fprintf(prnstr,"\033*p+%dY", blanklines);
              blanklines=0;
         }

         out_count= RLEcompress((DWORD *)linebuf,(DWORD *)end_word,(char *)outbuf);
         fprintf(prnstr, "\033*b%dW",out_count);
         fwrite(outbuf,sizeof(char),out_count,prnstr);
    } /*--- i ---*/

    memset(rasts[0],0,RastSize);        // clear buffer
} /* lx_block */
#endif

static void hp_setcolor(int color)
{
   sysColor = color;
}
static void hp_setGray(int gray)
{
   BW_setGray(gray);
}
static void hp_setRGBcolor(int r,int g,int b)
{
   int gray=(30*r+59*g+11*b)/100;
   hp_setGray(gray);
}
static void hp_setCMYKcolor(int c,int m,int y,int k)
{
}

PRINTER HPA3_600printer = {
  DEV_BW,
  hp_a3_init,
  hp_block,
  hp_FF,
  hp_over,
  hp_getheight,
  hp_scanfill,
  hp_setcolor,
  hp_setRGBcolor,
  hp_setCMYKcolor,
  hp_setGray,
  600,
  60*115,                // 600x11.5
  60*170,                // 600*17.0
  118,0,               // leftmargin=5mm
};

PRINTER HPA3_300printer = {
  DEV_BW,
  hp_a3_init,
  hp_block,
  hp_FF,
  hp_over,
  hp_getheight,
  hp_scanfill,
  hp_setcolor,
  hp_setRGBcolor,
  hp_setCMYKcolor,
  hp_setGray,
  300,
  30*115,                // 300x11.5
  30*170,                // 300*17.0
  59,0,                // leftmargin=5mm
};

PRINTER HPA4_600printer = {
  DEV_BW,
  hp_a4_init,
  hp_block,
  hp_FF,
  hp_over,
  hp_getheight,
  hp_scanfill,
  hp_setcolor,
  hp_setRGBcolor,
  hp_setCMYKcolor,
  hp_setGray,
  600,
  60*85,                 // 600x8.5
  60*115,                // 600*11.5
  118,0,               // leftmargin=5mm,
};

PRINTER HPA4_300printer = {
  DEV_BW,
  hp_a4_init,
  hp_block,
  hp_FF,
  hp_over,
  hp_getheight,
  hp_scanfill,
  hp_setcolor,
  hp_setRGBcolor,
  hp_setCMYKcolor,
  hp_setGray,
  300,
  30*85,                 // 300x8.5
  30*115,                // 300*11.5
  59,0,                // leftmargin=5mm,
};

PRINTER HP2A4_300printer = {
  DEV_BW,
  hp2_a4_init,
  hp2_block,
  hp_FF,
  hp_over,
  hp_getheight,
  hp_scanfill,
  hp_setcolor,
  hp_setRGBcolor,
  hp_setCMYKcolor,
  hp_setGray,
  300,
  30*85,                 // 300x8.5
  30*115,                // 300*11.5
  59,0,                // leftmargin=5mm,
};

PRINTER P6100_300printer = {
  DEV_BW,
  hp2_a4_init,
  hp2_block,
  hh_FF,
  hp_over,
  hp_getheight,
  hp_scanfill,
  hp_setcolor,
  hp_setRGBcolor,
  hp_setCMYKcolor,
  hp_setGray,
  300,
  30*85,                 // 300x8.5
  30*115,                // 300*11.5
  59,0,                // leftmargin=5mm,
};

PRINTER P6500_300printer = {
  DEV_BW,
  hp_a4_init,
  hp_block,
  hh_FF,
  hp_over,
  hp_getheight,
  hp_scanfill,
  hp_setcolor,
  hp_setRGBcolor,
  hp_setCMYKcolor,
  hp_setGray,
  300,
  30*85,                 // 300x8.5
  30*115,                // 300*11.5
  59,0,                // leftmargin=5mm,
};


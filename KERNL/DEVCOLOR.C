/*-------------------------------------------------------------------
* Name: devcolor.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static void RemapRGB(int r,int g,int b,int *newr,int *newg,int *newb);

static int sysRGB[16][3]={
         {0,0,0},            //EGA_BLACK
         {0,0,127},          //EGA_DBLUE
         {0,127,0},          //EGA_DGREEN
         {0,127,127},        //EGA_DCYAN
         {127,0,0},          //EGA_DRED
         {127,0,127},        //EGA_DMAGENTA
         {127,127,0},        //EGA_BROWN
         {127,127,127},      //EGA_DGRAY
         {180,180,180},       //EGA_GRAY
         {0,0,255},          //EGA_BLUE
         {0,255,0},         //EGA_GREEN
         {0,255,255},        //EGA_CYAN
         {255,00,0},         //EGA_RED
         {255,0,255},        //EGA_MAGENTA
         {255,255,0},        //EGA_YELLOW
         {255,255,255},      //EGA_WHITE
};
#define         EGA_PROCESSBLK   16

/*-- some Dot Matrix Printer, such as LQ2500, CR3240, etc,
    has only Red,Blue,Yellow, blacK color,
    so, we must change some colors' value in sysColor table
----------------------------------------------------*/
void ChangeColorForRBYKDotMatrixPrinter()
{
    sysRGB[EGA_BLUE][1]=127;    // to EGA_CYAN
    sysRGB[EGA_RED][2]=127;     // to EGA_MAGENTA

    sysRGB[EGA_LIGHTBLUE][1]=255;    // to EGA_LIGHTCYAN
    sysRGB[EGA_LIGHTRED][2]=255;     // to EGA_LIGHTMAGENTA
}
void RestoreColorForRBYKDotMatrixPrinter()
{
    sysRGB[EGA_BLUE][1]=0;
    sysRGB[EGA_RED][2]=0;
    sysRGB[EGA_LIGHTBLUE][1]=0;
    sysRGB[EGA_LIGHTRED][2]=0;
}

void setSYScolor(int sColor)
{
   sysColor = sColor;
   if (sColor==EGA_PROCESSBLK) {
      setRGBcolor(0,0,0);
      return;
   }
   setRGBcolor(sysRGB[sColor][0],
               sysRGB[sColor][1],sysRGB[sColor][2]);
}

static VOID RGB2CMYK(int r,int g,int b, int *c,int *m,int *y,int *k)
{
    int cc,mm,yy,kk;
    cc = 255-r; mm = 255-g; yy=255-b;
             // kk = min(cc,min(mm,yy));
    kk=cc; if(kk>mm) kk=mm; if(kk>yy) kk=yy;
    cc-=kk;  // cc = min(255,max(0,cc-kk));
    mm-=kk;  // mm = min(255,max(0,mm-kk));
    yy-=kk;  // yy = min(255,max(0,yy-kk));
             // kk = min(255,max(0,kk));
//    *c = gamma[cc];
//    *m = gamma[mm];
//    *y = gamma[yy];
//    *k = gamma[kk];

    *c = cc; *m=mm ;*y=yy; *k=kk;

} /* RGB2CMYK */


void setGray(int gray)
{
   int s,r,g,b,c,m,y,k;
   int shade = 255-gray;

   s = sysColor;
   if (gray==0) {
         setSYScolor(s);
         return;
   }
   if (gray==255) {
         setSYScolor(EGA_WHITE);
         return;
   }

   if (s==EGA_PROCESSBLK) {
        c = m = y = k =255;
   } else {
        r = sysRGB[s][0];
        g = sysRGB[s][1];
        b = sysRGB[s][2];
        RGB2CMYK(r,g,b,&c,&m,&y,&k);
   }

   c = (long)c*shade/256;
   m = (long)m*shade/256;
   y = (long)y*shade/256;
   k = (long)k*shade/256;

         //   c = min(255,c+k); m = min(255,m+k); y= min(255,y+k);
   c+=k;  if(c>255) c=255;
   m+=k;  if(m>255) m=255;
   y+=k;  if(y>255) y=255;
         // r = max(0,255-c); g = max(0,255-m); b = max(0,255-y);
   r=255-c; g=255-m; b=255-y;

   setRGBcolor(r,g,b);
}


void setRGBcolor(int r,int g,int b)
{
   int newr,newg,newb;

      //   if(_resolution>=600) {
   if(fRemapRGB) {
      RemapRGB(r,g,b,&newr,&newg,&newb);
   } else {
      newr=r;newg=g;newb=b;
   }

   ColorR=newr;
   ColorG=newg;
   ColorB=newb;
}

void setCMYKcolor(int c,int m,int y,int k)
{
}

#define  MAXSUM     0x4770

#define  R_R    0x100
#define  R_G    (R_R+0x200)
#define  R_B    (R_G+0x200)
short *RRtable=(short *) (&DitherTable[R_R]);
short *RGtable=(short *) (&DitherTable[R_G]);
short *RBtable=(short *) (&DitherTable[R_B]);

#define  G_R    (R_B+0x200)
#define  G_G    (G_R+0x200)
#define  G_B    (G_G+0x200)
short *GRtable=(short *) (&DitherTable[G_R]);
short *GGtable=(short *) (&DitherTable[G_G]);
short *GBtable=(short *) (&DitherTable[G_B]);

#define  B_R    (G_B+0x200)
#define  B_G    (B_R+0x200)
#define  B_B    (B_G+0x200)
short *BRtable=(short *) (&DitherTable[B_R]);
short *BGtable=(short *) (&DitherTable[B_G]);
short *BBtable=(short *) (&DitherTable[B_B]);

#define  SUM    (B_B+0x200)
unsigned char  *SumTable=&DitherTable[SUM];

static void RemapRGB(int r,int g,int b,int *newr,int *newg,int *newb)
{
     short rsum,gsum,bsum;
     short tR,tG,tB;
     short deltaSum, MaxRG,minval;
     short r1,g1,b1,r2,g2,b2;

     rsum=RRtable[r]+RGtable[g]+RBtable[b];
     if(rsum<0) rsum=0;
     else if(rsum>MAXSUM) rsum=MAXSUM;
     r1=SumTable[(8+rsum)>>4];
     r2=DitherTable[r1];

     gsum=GRtable[r]+GGtable[g]+GBtable[b];
     if(gsum<0) gsum=0;
     else if(gsum>MAXSUM) gsum=MAXSUM;
     g1=SumTable[(8+gsum)>>4];
     g2=DitherTable[g1];

     bsum=BRtable[r]+BGtable[g]+BBtable[b];
     if(bsum<0) bsum=0;
     else if(bsum>MAXSUM) bsum=MAXSUM;
     b1=SumTable[(8+bsum)>>4];
     b2=DitherTable[b1];

     MaxRG=r1;
     if(MaxRG<g1) MaxRG=g1;

     if(b1>MaxRG) {   /* adjust g2,b2 */
          short delta=(b1-MaxRG)/8;
          g2+=delta; if(g2>0xff) g2=0xff;
          b2-=(delta>>2); if(b2<0) b2=0;
     }

     tR=0xff-r2;     tG=0xff-g2;     tB=0xff-b2;
     minval=tR;
     if(minval>tG) minval=tG;
     if(minval>tB) minval=tB;

     tR-=minval;     tG-=minval;     tB-=minval;
     deltaSum=tR+tG+tB+minval;

     if(deltaSum>0x99) {
          short tmp=deltaSum/2;
          if(tR) tR=(0x99*tR+tmp)/deltaSum;
          if(tG) tG=(0x99*tG+tmp)/deltaSum;
          if(tB) tB=(0x99*tB+tmp)/deltaSum;
          if(minval) minval=(0x99*minval+tmp)/deltaSum;

          tmp=0xff-minval;
          r2=tmp-tR;          g2=tmp-tG;          b2=tmp-tB;
     }

     *newr=r2;     *newg=g2;     *newb=b2;

}  /* RemapRGB */

#define ErrTableLen  (0x200*4)
extern unsigned char errtab[];

int InitDitherBuf()
{
   DitherBufLen=4*(RastWidth+2)*sizeof(short);
   RowErrBuf=malloc(DitherBufLen);
   if(RowErrBuf==NULL) {
       return(-10);
   }
   //memset(RowErrBuf,0,DitherBufLen);
   return 1;
}

void CloseDitherBuf()
{
   if(RowErrBuf)
     free(RowErrBuf);
}

static void FromLeft(LPDC lpdc,int Row)
{
     LONG rgboff,cmykoff;
     int  width=RastWidth;
     int  i,rowoff /*,old_rowoff*/ ;
     short  err0,err1,err2,err3;
     BYTE   rpttn,gpttn,bpttn,pttn,cnt;
     short  *pErr;
     BYTE  *rrast,*grast,*brast,*crast,*mrast,*yrast,*krast;

     rowoff=1;
     rgboff = (Row-lpdc->top)*width;
     cmykoff = (Row-lpdc->top)*RastWidthByte;
     rrast=&rasts[4][rgboff];
     grast=&rasts[5][rgboff];
     brast=&rasts[6][rgboff];
     crast=&rasts[0][cmykoff];
     mrast=&rasts[1][cmykoff];
     yrast=&rasts[2][cmykoff];
     krast=&rasts[3][cmykoff];

     pErr=&RowErrBuf[rowoff];
     err0=*pErr + *(pErr-1);
     //RowErrBuf[rowoff]=RowErrBuf[rowoff-1]=0;
     *pErr=*(pErr-1)=0;

     //err1=RowErrBuf[rowoff+width] + RowErrBuf[rowoff+width-1];
     pErr+=width;
     err1=*pErr + *(pErr-1);
     *pErr=*(pErr-1)=0;

     //err2=RowErrBuf[rowoff+2*width] + RowErrBuf[rowoff+2*width-1];
     pErr+=width;
     err2=*pErr + *(pErr-1);
     *pErr=*(pErr-1)=0;

     //err3=RowErrBuf[rowoff+3*width] + RowErrBuf[rowoff+3*width-1];
     pErr+=width;
     err3=*pErr + *(pErr-1);
     *pErr=*(pErr-1)=0;

     rpttn=gpttn=bpttn=pttn=0xff; cnt=0x80;

     for(i=0;i<width;i++) {
           short r,g,b,maxRGB,idx,t;

           r=*rrast++;  g=*grast++; b=*brast++;
           if(r+g+b != 3*0xff) {
               maxRGB=r;
               if(g>maxRGB) maxRGB=g;
               if(b>maxRGB) maxRGB=b;
               r-=maxRGB;  g-=maxRGB;  b-=maxRGB;
               maxRGB+=err0;   r+=err1; g+=err2; b+=err3;
               if(maxRGB<0x80) {
                   err0=maxRGB;  pttn^=cnt;
               } else {
                   err0=maxRGB-0xff;
                   if(r+0x7f<0) { rpttn^=cnt; r+=0xff; }
                   if(g+0x7f<0) { gpttn^=cnt; g+=0xff; }
                   if(b+0x7f<0) { bpttn^=cnt; b+=0xff; }
               }
               err1=r; err2=g; err3=b;
           } // r=g=b=0xff

           pErr=&RowErrBuf[rowoff++];

           t=*(pErr+1);
           err0+=0x80;
           if(err0>=0) {
               if(err0>0xff) { t+=err0-0xff; idx=2*0xff; }
               else idx=err0<<1;
           } else { t+=err0; idx=0; }

           *(pErr-1) += *(LPUSHORT)(errtab+idx);
           *(pErr)   += *(LPUSHORT)(errtab+idx+0x200);
           *(pErr+1)  = *(LPUSHORT)(errtab+idx+0x400);
           err0 = t+ *(LPUSHORT)(errtab+idx+0x600);

           pErr+=width;
           t=*(pErr+1);
           err1+=0x80;
           if(err1>=0) {
               if(err1>0xff) { t+=err1-0xff; idx=2*0xff; }
               else idx=err1<<1;
           } else { t+=err1; idx=0; }

           *(pErr-1) += *(LPUSHORT)(errtab+idx);
           *(pErr)   += *(LPUSHORT)(errtab+idx+0x200);
           *(pErr+1)  = *(LPUSHORT)(errtab+idx+0x400);
           err1 = t+ *(LPUSHORT)(errtab+idx+0x600);

           pErr+=width;
           t=*(pErr+1);
           err2+=0x80;
           if(err2>=0) {
               if(err2>0xff) { t+=err2-0xff; idx=2*0xff; }
               else idx=err2<<1;
           } else { t+=err2; idx=0; }

           *(pErr-1) += *(LPUSHORT)(errtab+idx);
           *(pErr)   += *(LPUSHORT)(errtab+idx+0x200);
           *(pErr+1)  = *(LPUSHORT)(errtab+idx+0x400);
           err2 = t+ *(LPUSHORT)(errtab+idx+0x600);

           pErr+=width;
           t=*(pErr+1);
           err3+=0x80;
           if(err3>=0) {
               if(err3>0xff) { t+=err3-0xff; idx=2*0xff; }
               else idx=err3<<1;
           } else { t+=err3; idx=0; }

           *(pErr-1) += *(LPUSHORT)(errtab+idx);
           *(pErr)   += *(LPUSHORT)(errtab+idx+0x200);
           *(pErr+1)  = *(LPUSHORT)(errtab+idx+0x400);
           err3 = t+ *(LPUSHORT)(errtab+idx+0x600);

           //rowoff++;
           //rgboff++;

           cnt>>=1;
           if((cnt&0xff)==0) {
                BYTE c,m,y,k;
                cnt=0x80;
                r=pttn & rpttn;
                g=pttn & gpttn;
                b=pttn & bpttn;
                if (UseHP1200) {
                   c = ~r; m = ~b; y = ~g;
                } else {
                   k=~(r|g|b);  c=~(k|r); m=~(k|g); y=~(k|b);
                }
                *crast++=c;
                *mrast++=m;
                *yrast++=y;
                *krast++=k;
                //cmykoff++;
                pttn=rpttn=gpttn=bpttn=0xff;
           } // if 8 bits
     }  // for i...

} /* FromLeft */

static void FromRight(LPDC lpdc,int Row)
{
     LONG rgboff,cmykoff;
     int  width=RastWidth;
     int  i,rowoff /*,old_rowoff*/ ;
     short  err0,err1,err2,err3;
     BYTE   rpttn,gpttn,bpttn,pttn,cnt;
     short  *pErr;
     BYTE  *rrast,*grast,*brast,*crast,*mrast,*yrast,*krast;

     rowoff=width;
     rgboff = (Row-lpdc->top+1)*width-1;
     cmykoff = (Row-lpdc->top+1)*RastWidthByte-1;
     rrast=&rasts[4][rgboff];
     grast=&rasts[5][rgboff];
     brast=&rasts[6][rgboff];
     crast=&rasts[0][cmykoff];
     mrast=&rasts[1][cmykoff];
     yrast=&rasts[2][cmykoff];
     krast=&rasts[3][cmykoff];

     pErr=&RowErrBuf[rowoff];
     err0=*pErr + *(pErr+1);
     *pErr=*(pErr+1)=0;

     //err1=RowErrBuf[rowoff+width] + RowErrBuf[rowoff+width+1];
     pErr+=width;
     err1=*pErr + *(pErr+1);
     *pErr=*(pErr+1)=0;

     //err2=RowErrBuf[rowoff+2*width] + RowErrBuf[rowoff+2*width+1];
     pErr+=width;
     err2=*pErr + *(pErr+1);
     *pErr=*(pErr+1)=0;

     //err3=RowErrBuf[rowoff+3*width] + RowErrBuf[rowoff+3*width+1];
     pErr+=width;
     err3=*pErr + *(pErr+1);
     *pErr=*(pErr+1)=0;

     rpttn=gpttn=bpttn=pttn=0xff; cnt=1;
     for(i=0;i<width;i++) {
           short r,g,b,maxRGB,idx,t;

           r=*rrast--;  g=*grast--; b=*brast--;
           if(r+g+b != 3*0xff) {
               maxRGB=r;
               if(g>maxRGB) maxRGB=g;
               if(b>maxRGB) maxRGB=b;
               r-=maxRGB;  g-=maxRGB;  b-=maxRGB;
               maxRGB+=err0;   r+=err1; g+=err2; b+=err3;
               if(maxRGB<0x80) {
                   err0=maxRGB;  pttn^=cnt;
               } else {
                   err0=maxRGB-0xff;
                   if(r+0x7f<0) { rpttn^=cnt; r+=0xff; }
                   if(g+0x7f<0) { gpttn^=cnt; g+=0xff; }
                   if(b+0x7f<0) { bpttn^=cnt; b+=0xff; }
               }
               err1=r; err2=g; err3=b;
           } // r=g=b=0xff

           pErr=&RowErrBuf[rowoff--];

           t=*(pErr-1);
           err0+=0x80;
           if(err0>=0) {
               if(err0>0xff) { t+=err0-0xff; idx=2*0xff; }
               else idx=err0<<1;
           } else { t+=err0; idx=0; }

           *(pErr+1) += *(LPUSHORT)(errtab+idx);
           *(pErr)   += *(LPUSHORT)(errtab+idx+0x200);
           *(pErr-1)  = *(LPUSHORT)(errtab+idx+0x400);
           err0 = t+ *(LPUSHORT)(errtab+idx+0x600);

           pErr+=width;
           t=*(pErr-1);
           err1+=0x80;
           if(err1>=0) {
               if(err1>0xff) { t+=err1-0xff; idx=2*0xff; }
               else idx=err1<<1;
           } else { t+=err1; idx=0; }

           *(pErr+1) += *(LPUSHORT)(errtab+idx);
           *(pErr)   += *(LPUSHORT)(errtab+idx+0x200);
           *(pErr-1)  = *(LPUSHORT)(errtab+idx+0x400);
           err1 = t+ *(LPUSHORT)(errtab+idx+0x600);

           pErr+=width;
           t=*(pErr-1);
           err2+=0x80;
           if(err2>=0) {
               if(err2>0xff) { t+=err2-0xff; idx=2*0xff; }
               else idx=err2<<1;
           } else { t+=err2; idx=0; }

           *(pErr+1) += *(LPUSHORT)(errtab+idx);
           *(pErr)   += *(LPUSHORT)(errtab+idx+0x200);
           *(pErr-1)  = *(LPUSHORT)(errtab+idx+0x400);
           err2 = t+ *(LPUSHORT)(errtab+idx+0x600);

           pErr+=width;
           t=*(pErr-1);
           err3+=0x80;
           if(err3>=0) {
               if(err3>0xff) { t+=err3-0xff; idx=2*0xff; }
               else idx=err3<<1;
           } else { t+=err3; idx=0; }

           *(pErr+1) += *(LPUSHORT)(errtab+idx);
           *(pErr)   += *(LPUSHORT)(errtab+idx+0x200);
           *(pErr-1)  = *(LPUSHORT)(errtab+idx+0x400);
           err3 = t+ *(LPUSHORT)(errtab+idx+0x600);

           //rowoff=old_rowoff-1;
           //rgboff--;

           cnt<<=1;
           if((cnt&0xff)==0) {
                BYTE c,m,y,k;
                cnt=1;
                r=pttn & rpttn;
                g=pttn & gpttn;
                b=pttn & bpttn;
                if (UseHP1200) {
                   c = ~r; m = ~b; y = ~g;
                } else {
                   k=~(r|g|b);  c=~(k|r); m=~(k|g); y=~(k|b);
                }
                *crast--=c;
                *mrast--=m;
                *yrast--=y;
                *krast--=k;
                //cmykoff--;
                pttn=rpttn=gpttn=bpttn=0xff;
           } // if 8 bits
     }  // for i...

} /* FromRight */

void DitherRGB(LPDC lpdc)
{
     int i,end,maxy=MaxRastY;

     memset(rasts[0],0,RastHeight*RastWidthByte*4);  // clear cmyk

     if(lpdc->bottom>printer->ypixel)
         end=printer->ypixel;
     else
         end=lpdc->bottom;

     for (i=lpdc->top;(i<end)&&(i<=maxy);i++)
     {
         if(i&1) FromRight(lpdc,i); else FromLeft(lpdc,i);
     }
}

int RLEcompress(DWORD *row, DWORD *end_row, char *compressed)
{
        DWORD *exam = row;     /* WORDbeing examined in the row to compress */
        char *cptr = compressed; /* output pointer into compressed bytes */

        while ( exam < end_row )
           {    /* Search ahead in the input looking for a run */
                /* of at least 4 identical bytes. */
                const char *compr = (const char *)exam;
                const char *end_dis;
                const DWORD *next;
                register DWORD test;
                while ( exam < end_row )
                  { test = *exam;
                    if ( ((test << 8) ^ test) <= 0xff )
                      break;
                    exam++;
                  }

                /* Find out how long the run is */
                end_dis = (const char *)exam;
                if ( exam >= end_row )  /* no run */
                  { /* See if any of the last 3 "dissimilar" bytes are 0. */
                    /*----------------------------
                    if ( end_dis > compr && end_dis[-1] == 0 )
                      { if ( end_dis[-2] != 0 ) end_dis--;
                        else if ( end_dis[-3] != 0 ) end_dis -= 2;
                        else end_dis -= 3;
                      }
                     -----------------------------*/
                    next = --end_row;
                  }
                else
                  { next = exam + 1;
                    while ( next < end_row && *next == test )
                      next++;
                    /* See if any of the last 3 "dissimilar" bytes */
                    /* are the same as the repeated byte. */
                    /*----------------------------*/
                    if ( end_dis > compr && end_dis[-1] == (BYTE)test )
                      { if ( end_dis[-2] != (BYTE)test ) end_dis--;
                        else if ( end_dis[-3] != (BYTE)test ) end_dis -= 2;
                        else end_dis -= 3;
                      }
                     /*-----------------------------*/
                  }

                /* Now [compr..end_dis) should be encoded as dissimilar, */
                /* and [end_dis..next) should be encoded as similar. */
                /* Note that either of these ranges may be empty. */

                for ( ; ; )
                   {    /* Encode up to 128 dissimilar bytes */
                        UINT count = end_dis - compr; /* UINT for faster switch */
                        switch ( count )
                          { /* Use memcpy only if it's worthwhile. */
                          case 6: cptr[6] = compr[5];
                          case 5: cptr[5] = compr[4];
                          case 4: cptr[4] = compr[3];
                          case 3: cptr[3] = compr[2];
                          case 2: cptr[2] = compr[1];
                          case 1: cptr[1] = compr[0];
                            *cptr = count - 1;
                            cptr += count + 1;
                          case 0: /* all done */
                            break;
                          default:
                            if ( count > 128 ) count = 128;
                            *cptr++ = count - 1;
                            memcpy(cptr, compr, count);
                            cptr += count, compr += count;
                            continue;
                          }
                        break;
                   }

                   {    /* Encode up to 128 similar bytes. */
                        /* Note that count may be <0 at end of row. */
                        int count = (const char *)next - end_dis;
                        while ( count > 0 )
                          { int this = (count > 128 ? 128 : count);
                            *cptr++ = 257 - this;
                            *cptr++ = (BYTE)test;
                            count -= this;
                          }
                        exam = (DWORD *)next;
                   }
           }
        return (cptr - compressed);
}

void RGB_scanline(int x1,int x2,int y1,LPDC lpdc)
{
      long byteoff;
      int len;
      UCHAR *pR,*pG,*pB;
      int nx,ny1,ny2,i;

      if (x1>x2) {
         i=x2;
         x2=x1;
         x1=i;
      }

      if (GlobalRorate90)
      {
            nx=PageHightDot-y1;
            ny1=x1;
            ny2=x2;

            if (GlobalReverse)
            {
              nx=lpdc->left+lpdc->right-nx;
            }

            if ((ny1>=lpdc->bottom)||(ny2<lpdc->top)) return;
            if ((nx>=lpdc->right)||(nx<lpdc->left)) return;
            if (ny1<lpdc->top) ny1=lpdc->top;
            if (ny2>=lpdc->bottom) ny2=lpdc->bottom-1;

            if (GlobalYReverse)
            {
              i=lpdc->left+lpdc->right-ny1;
              ny1=lpdc->left+lpdc->right-ny2;
              ny2=i;
            }

         /*--------- added ByHance, 96,4.11, for printer's fixed margin ----*/
            nx-=PrinterFixedLeftMargin;
            if(nx<0) return;
          /*------- end ------*/

            fDither=TRUE;
            if(ny2>MaxRastY) MaxRastY=ny2;

            byteoff = (ny1 - lpdc->top)*RastWidth+nx;

            pR=&(rasts[4][byteoff]);
            pG=&(rasts[5][byteoff]);
            pB=&(rasts[6][byteoff]);

            len=ny2-ny1+1;

            for (i=0;i<len;i++)
            {
              rasts[4][byteoff]=ColorR;
              rasts[5][byteoff]=ColorG;
              rasts[6][byteoff]=ColorB;
              byteoff+=RastWidth;
            }

            return ;
      }

      if (GlobalReverse)
      {
        i=lpdc->left+lpdc->right-x1;
        x1=lpdc->left+lpdc->right-x2;
        x2=i;
      }

      if ((x1>=lpdc->right)||(x2<lpdc->left)) return;
      if ((y1>=lpdc->bottom)||(y1<lpdc->top)) return;
      if (x1<lpdc->left) x1=lpdc->left;
      if (x2>=lpdc->right) x2=lpdc->right-1;

      if (GlobalYReverse)
      {
        y1=lpdc->top+lpdc->bottom-y1;
      }

   /*--------- added ByHance, 96,4.11, for printer's fixed margin ----*/
      x2-=PrinterFixedLeftMargin;
      if(x2<0) return;
      x1-=PrinterFixedLeftMargin;
      if(x1<0) x1=0;
    /*------- end ------*/

      fDither=TRUE;
      if(y1>MaxRastY) MaxRastY=y1;

      byteoff = (y1 - lpdc->top)*RastWidth+x1;
      pR=&(rasts[4][byteoff]);
      pG=&(rasts[5][byteoff]);
      pB=&(rasts[6][byteoff]);

      len=x2-x1+1;
      memset(pR,ColorR,len);
      memset(pG,ColorG,len);
      memset(pB,ColorB,len);
}

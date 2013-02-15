/*-------------------------------------------------------------------
* Name: true.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static int cmap_format = 1;
#define MACINTOSH   1
static int numberOfHMetrics = 0;
#define NUMOFHMETRICS    34

#define HEADBuf         lpttf->HEADBuf
#define unitsPerEM      lpttf->unitsPerEM
#define TTxMin          lpttf->TTxMin
#define TTyMin          lpttf->TTyMin
#define TTxMax          lpttf->TTxMax
#define TTyMax          lpttf->TTyMax
#define indexToLocFormat lpttf->indexToLocFormat
#define HHEABuf         lpttf->HHEABuf
#define HMTXBuf         lpttf->HMTXBuf
#define ascender        lpttf->ascender
#define MAXPBuf         lpttf->MAXPBuf
#define numGlyphs       lpttf->numGlyphs
#define CMAPBuf         lpttf->CMAPBuf
#define cmapstart       lpttf->cmapstart
#define LOCABuf         lpttf->LOCABuf
#define GLYFBuf         lpttf->GLYFBuf
#define glyphBase       lpttf->glyphBase
#define numberOfContours lpttf->numberOfContours
#define ttfnNumber      lpttf->ttfnNumber
#define httf            lpttf->httf
#define ttWidth         lpttf->ttWidth
#define ttHeight        lpttf->ttHeight
#define aglyf           lpttf->aglyf
#define ttfHeadBuf      lpttf->ttfHeadBuf
#define lpnumTables     lpttf->lpnumTables
#define lpTableDirectory lpttf->lpTableDirectory


/*-------------Mac2Intel---------*/
static VOID Mac2IntelSHORT(LPSHORT);
static VOID Mac2IntelUSHORT(LPUSHORT);
static VOID Mac2IntelLONG(LPLONG);
static VOID Mac2IntelULONG(LPULONG);
static VOID Mac2IntelTABLE(LPTABDIR);
static BOOL ReadGlyph(LPTTFINFO lpttf,
                      int hf,
                      USHORT n);
static VOID LocateGlyph(LPTTFINFO lpttf,
                 int n,
                 LPULONG poff,
                 LPINT plen);

static int  Char2Glyph(LPTTFINFO lpttf,USHORT c);
static BOOL initCMAP(LPTTFINFO lpttf);
static BOOL OpenTTFont(LPTTFINFO lpttf,  int  fnt);
//static VOID CloseTTFont(LPTTFINFO lpttf);

static void ttfoutline (LPPOINT lppoint,
         int x0,
         int x1,
         int x2,
         int y0,
         int y1,
         int y2,
         UCHAR tag,
         LPINT lpdotn);
static int bezier (LPPOINT lppoint,
         int x0,
         int y0,
         int x1,
         int y1,
         int x2,
         int y2,
         LPINT lpdotn);


static int OpenTTF (LPTTFINFO lpttf, int h);

static BOOL OpenTTFont(LPTTFINFO lpttf, int fnt)
{
  char ext[20];
  char name[64];
  char *cp1, *cp2;
  int hf;
  int ft = fnt & 0x7fff;

  if (ttfnNumber == fnt) return(TRUE);
  if(ft<=0 || ft>999) return(FALSE);

  if ((SysDc.lpttf=HandleLock(efnHandle))==NULL)
        return(FALSE);

  ft = fnt;
  if ((fnt & 0x8000)!=0) {          //    HZ TTF
        ft = fnt & 0x7fff;
        /* make file extent */
        ext[0]  = '0'+ft / 1000;
        ext[1]  = '0'+(ft % 1000)/100;
        ext[2]  = '0'+(ft % 100)/10;
        ext[3]  = '0'+ft % 10;
        ext[4] = '\0';

        cp1 = SysDc.lpttf->ttPath;

        hf = NEG_ONE;
        while (*cp1) {                        /* SEARCH FOR ALL PATH */
           cp2 = name;
           while(*cp1 != ';' && *cp1 != '\0')
                       *cp2++ = *cp1++;
           if (*(cp2-1) !='/' && *(cp2-1) != '\\') *cp2++ = '\\';
           if (*cp1 == ';') cp1++;

           strcpy(cp2,"red");
           strcat(cp2,ext);
           strcat(cp2,".ttf");
           if ((hf = open(name,O_RDONLY|O_BINARY)) != NEG_ONE) break;
        }
  } else {
        /* make file extent */
        ext[0]  = '0'+fnt / 100;
        ext[1]  = '0'+(fnt % 100)/10;
        ext[2]  = '0'+fnt % 10;
        ext[3] = '\0';

        cp1 = SysDc.lpttf->ttPath;

        hf = NEG_ONE;
        while (*cp1) {                        /* SEARCH FOR ALL PATH */
           cp2 = name;
           while(*cp1 != ';' && *cp1 != '\0')
                       *cp2++ = *cp1++;
           if (*(cp2-1) !='/' && *(cp2-1) != '\\') *cp2++ = '\\';
           if (*cp1 == ';') cp1++;

           strcpy(cp2,"ttfalib.");
           strcat(cp2,ext);
           if ((hf = open(name,O_RDONLY|O_BINARY)) != NEG_ONE) break;
        }
  }



  HandleUnlock(efnHandle);
  if (hf == -1 ) return FALSE;

  hf = OpenTTF(lpttf,hf);
  if (hf == NEG_ONE)  return FALSE;

  if (ttfnNumber!=-1 && httf!=-1) close(httf);
  httf = hf;
  ttfnNumber = fnt;
  return TRUE;
}

/* close font function */
#ifdef UNUSED
static VOID CloseTTFont(LPTTFINFO lpttf)
{
 if (httf !=NEG_ONE) close(httf);
 ttfnNumber = NEG_ONE;
}
#endif

BOOL TTOutline(LPDC lpdc,
               Wchar code,
               int font,
               int ww,
               int hh,
               LPPOINT lppoint,
               LPINT lpdotn,
               LPINT lppolyn,
               LPMAT2 lpmatrix)
{

  LONG x,y;
  FIXED fx,fy,f1,f2,f3,f4;
  int glyphn;
  int i,j,k,l;
  int startpnt,endpnt,dotn;
  UCHAR tag;
  LPTTFINFO lpttf = lpdc->lpttf;

  if (!OpenTTFont(lpttf,font)) {
          if (httf != NEG_ONE) close(httf);
          ttfnNumber = NEG_ONE;
          return FALSE;
  }
  lseek(httf,0,SEEK_SET);
  glyphn = Char2Glyph(lpttf,code);
  if (!ReadGlyph(lpttf,httf,glyphn)) {
           if (httf != NEG_ONE) close(httf);
           ttfnNumber = NEG_ONE;
           return FALSE;
  }
  /*-----rescale font------*/
  for (i=0;i<aglyf.pointn;i++) {
           x = aglyf.xbuf[i];
                   y = aglyf.ybuf[i];
           Long2Fixed(fx,((LONG)(x*ww/unitsPerEM)<<16));
           Long2Fixed(fy,((LONG)(y*hh/unitsPerEM)<<16));
           FixedMul(&fx,&(lpmatrix->eM11),&f1);
           FixedMul(&fy,&(lpmatrix->eM12),&f2);
           FixedMul(&fx,&(lpmatrix->eM21),&f3);
           FixedMul(&fy,&(lpmatrix->eM22),&f4);
           FixedAdd(&f1,&f2,&fx);
           FixedAdd(&f3,&f4,&fy);
           aglyf.xbuf[i] = IntofFixed(fx);

                   aglyf.ybuf[i] = IntofFixed(fy)-hh/5+hh-(long)ascender*hh/unitsPerEM;

           /*
           aglyf.ybuf[i] = IntofFixed(fy);
           */
  }

  /*------do coordinates---------*/
  *lppolyn = aglyf.contourn;
  startpnt = 0;
  k = 0;l=0;
  for (i=0;i<aglyf.contourn ;i++) {
         endpnt = aglyf.enddot[i];
         k = 0;
         if (aglyf.flag[l]&1) {
           lppoint[l].x = aglyf.xbuf[startpnt];
           lppoint[l].y = aglyf.ybuf[startpnt];
  //         moveto(lppoint[l].x*2,lppoint[l].y*2);
           k ++;
           l++;
         }
         for (j = startpnt;j<endpnt-1;j++) {
                  tag =((aglyf.flag[j]&1)|((aglyf.flag[j+1]&1)<<1)|((aglyf.flag[j+2]&1)<<2))&7;
                  ttfoutline(&lppoint[l],aglyf.xbuf[j],aglyf.xbuf[j+1],aglyf.xbuf[j+2],
                     aglyf.ybuf[j],aglyf.ybuf[j+1],aglyf.ybuf[j+2],tag,&dotn);
                  k += dotn;
                  l += dotn;
         }
         tag = ((aglyf.flag[j]&1)|((aglyf.flag[j+1]&1)<<1)|((aglyf.flag[startpnt]&1)<<2)) &7;
         ttfoutline(&lppoint[l],aglyf.xbuf[j],aglyf.xbuf[j+1],aglyf.xbuf[startpnt],
                aglyf.ybuf[j],aglyf.ybuf[j+1],aglyf.ybuf[startpnt],tag,&dotn);
         k += dotn;
         l+=dotn;

         tag = ((aglyf.flag[j+1]&1)|((aglyf.flag[startpnt]&1)<<1)|((aglyf.flag[startpnt+1]&1)<<2)) &7;
         ttfoutline(&lppoint[l],aglyf.xbuf[j+1],aglyf.xbuf[startpnt],aglyf.xbuf[startpnt+1],
                aglyf.ybuf[j+1],aglyf.ybuf[startpnt],aglyf.ybuf[startpnt+1],tag,&dotn);
         k += dotn;
         l+=dotn;
         lpdotn[i] = k;
         startpnt = endpnt+1;
  }
  return TRUE;
}


/*---make ttf outline---*/
static void ttfoutline(lppoint,x0,x1,x2,y0,y1,y2,tag,lpdotn)
LPPOINT lppoint;
int x0,x1,x2,y0,y1,y2;
UCHAR tag;
LPINT lpdotn;
{
   switch (tag) {
     case 0:                    /* off,off,off */
       bezier(lppoint,(x0+x1)/2,(y0+y1)/2,x1,y1,(x1+x2)/2,(y1+y2)/2,lpdotn);
       break;
         case 4:                    /* off,off,on */
       bezier(lppoint,(x0+x1)/2,(y0+y1)/2,x1,y1,x2,y2,lpdotn);
       break;
     case 2:                    /* off,on,off */
       *lpdotn = 0;
       break;
         case 6:                    /* off,on,on */
           lppoint[0].x = x2;
           lppoint[0].y = y2;
           *lpdotn = 1;
       break;
         case 1:                    /* on, off,off */
       bezier(lppoint,x0,y0,x1,y1,(x1+x2)/2,(y1+y2)/2,lpdotn);
       break;
     case 5:                    /* on, off,on */
       bezier(lppoint,x0,y0,x1,y1,x2,y2,lpdotn);
       break;
         case 3:                    /* on,on,off */
         case 7:                    /* on on on */
           lppoint[0].x = x1;
           lppoint[0].y = y1;
           *lpdotn = 1;
       break;
   } /* switch */
}

#define FLATNESS 0.5
static bezier(lppoint,x0,y0,x1,y1,x2,y2,lpdotn)
LPPOINT lppoint;
int x0,y0,x1,y1,x2,y2;
LPINT lpdotn;
{
  double a,b,d,s;
  int x4,x5,x6,y4,y5,y6;
  int i,j;
  a = (double)(x0-x2); b = (double)(y0-y2);  d =(a*a+b*b)*FLATNESS;
  s = (double)x0*(y1-y2)+(double)x1*(y2-y0)+(double)x2*(y0-y1);
  s =s*s;
  if (s<=d) {
          lppoint[0].x = x2;
          lppoint[0].y = y2;
          *lpdotn = 1;
      return 0;
  } else {
          x4 = (x0+x1)/2; x6 = (x1+x2)/2; x5 = (x4+x6)/2;
          y4 = (y0+y1)/2; y6 = (y1+y2)/2; y5 = (y4+y6)/2;
      bezier(lppoint,x0,y0,x4,y4,x5,y5,&i);
          bezier(&lppoint[i],x5,y5,x6,y6,x2,y2,&j);
      *lpdotn = i+j;
      return 0;
  }
}

/*-------------------Low level True Type routines-----------------------*/
tagPair_t tagPair[Entries] = {
             {0x636d6170,NULL},
             {0x676c7966,NULL},
             {0x68656164,NULL},
             {0x68686561,NULL},
             {0x686d7478,NULL},
             {0x6c6f6361,NULL},
             {0x6d617870,NULL},
             {0x6e616d65,NULL},
             {0x706f7374,NULL},
             {0x4f532f32,NULL},
             {0x63767420,NULL},
             {0x6670676d,NULL},
             {0x68646d78,NULL},
             {0x6b65726e,NULL},
             {0x70726570,NULL},
             {0x57494e20,NULL},
             {0x464f4341,NULL}
};


/*---------------ttf buffer-------------------*/
/*
* open a ttf font file;
* return file handle
*/
static int OpenTTF(LPTTFINFO lpttf,int handle)
{
   int hf;
   int i,j;
   LPTABDIR lpdir;
   ULONG   len;
   USHORT recn;
   USHORT plat,plats,format;
   ULONG off;
   LPTTCMAP lpc;

   hf = handle;

   //added by Jerry 97/5/5
   if (HEADERLEN != read(hf,(LPCHAR)ttfHeadBuf,HEADERLEN))
   {
        close(hf);
        return NEG_ONE;
   }

   Mac2IntelUSHORT(lpnumTables);
   for (i=0;i<*lpnumTables;i++) {
         Mac2IntelTABLE(&lpTableDirectory[i]);
         j = 0;
         while (lpTableDirectory[i].tag != tagPair[j].tag && j<Entries) j++;
         if (j<Entries) tagPair[j].lpdir = &lpTableDirectory[i];
   }
   /*-------do cmaps-----------*/
   cmap_format = 4;
   if ((lpdir=tagPair[CMAP].lpdir)== NULL) goto errOpen;
   if (-1==lseek(hf,lpdir->offset,SEEK_SET)) {
        close(hf);
        return NEG_ONE;
   }
   read(hf,(LPCHAR)CMAPBuf,4096);

   recn = *(LPUSHORT)(&CMAPBuf[2]);
   Mac2IntelUSHORT(&recn);
   lpc =   (LPTTCMAP) (&CMAPBuf[4]);
   for (i=0;i<recn;i++) {
       plat =  lpc[i].platform;
       plats = lpc[i].plat_spec;
       Mac2IntelUSHORT(&plat);
       Mac2IntelUSHORT(&plats);
       if (plat == MICROSOFT && plats == PWIN) {
                 off = lpc[i].offset;
                 Mac2IntelULONG(&off);
                 format =*(LPUSHORT)(&CMAPBuf[off]);
                 Mac2IntelUSHORT(&format);
                 if ( format == 4) goto label1;
////////CD-ROM fix
                 if (format == 2) goto label0;
       }
  }

   recn = *(LPUSHORT)(&CMAPBuf[2]);
   Mac2IntelUSHORT(&recn);
   lpc =   (LPTTCMAP) (&CMAPBuf[4]);
   for (i=0;i<recn;i++) {
       plat =  lpc[i].platform;
       plats = lpc[i].plat_spec;
       Mac2IntelUSHORT(&plat);
       Mac2IntelUSHORT(&plats);
       if (plat == MICROSOFT && plats == 1) {
                 off = lpc[i].offset;
                 Mac2IntelULONG(&off);
                 format =*(LPUSHORT)(&CMAPBuf[off]);
                 Mac2IntelUSHORT(&format);
                 if ( format == 4) goto label1;
////////CD-ROM fix
                 if ( format == 2) goto label0;
       }
  }


   recn = *(LPUSHORT)(&CMAPBuf[2]);
   Mac2IntelUSHORT(&recn);
   lpc =   (LPTTCMAP) (&CMAPBuf[4]);
   for (i=0;i<recn;i++) {
       plat =  lpc[i].platform;
       plats = lpc[i].plat_spec;
       Mac2IntelUSHORT(&plat);
       Mac2IntelUSHORT(&plats);
       if (plat == MACINTOSH && plats == 0) {
                 off = lpc[i].offset;
                 Mac2IntelULONG(&off);
                 format =*(LPUSHORT)(&CMAPBuf[off]);
                 Mac2IntelUSHORT(&format);
                 if ( format == 0) {
                    cmap_format = 0;
                    goto label1;
                 }
       }
  }

  goto errOpen;

////////CD-ROM fix
label0:
  cmap_format = 2;
////////CD-ROM fix


label1:

  cmapstart = off;

   /*-------do head-----------*/
   if ((lpdir=tagPair[HEAD].lpdir)== NULL) goto errOpen;
   if (-1==lseek(hf,lpdir->offset,SEEK_SET)) goto errOpen;
   if (HEADBUFLEN !=read(hf,(LPCHAR)HEADBuf,HEADBUFLEN)) goto errOpen;

   Mac2IntelUSHORT((LPUSHORT)&HEADBuf[UNITPEREM]);
   unitsPerEM = (USHORT)*(LPUSHORT)&HEADBuf[UNITPEREM];

   Mac2IntelSHORT((LPSHORT)&HEADBuf[TTXMIN]);
   TTxMin = (SHORT)*(LPSHORT)&HEADBuf[TTXMIN];

   Mac2IntelSHORT((LPSHORT)&HEADBuf[TTYMIN]);
   TTyMin = (SHORT)*(LPSHORT)&HEADBuf[TTYMIN];

   Mac2IntelSHORT((LPSHORT)&HEADBuf[TTXMAX]);
   TTxMax = (SHORT)*(LPSHORT)&HEADBuf[TTXMAX];

   Mac2IntelSHORT((LPSHORT)&HEADBuf[TTYMAX]);
   TTyMax = (SHORT)*(LPSHORT)&HEADBuf[TTYMAX];

   Mac2IntelSHORT((LPSHORT)&HEADBuf[INDEXLOC]);
   indexToLocFormat = (SHORT)*(LPSHORT)&HEADBuf[INDEXLOC];

   ttWidth = TTxMax - TTxMin;
   ttHeight = TTyMax - TTyMin;

   /*-------do hhea-----------*/
   if ((lpdir=tagPair[HHEA].lpdir)== NULL) goto errOpen;
   if (-1==lseek(hf,lpdir->offset,SEEK_SET)) goto errOpen;
   read(hf,(LPCHAR)HHEABuf,HHEABUFLEN);
   Mac2IntelSHORT((LPSHORT)&HHEABuf[ASCENDER]);
   ascender = (SHORT)*(LPSHORT)&HHEABuf[ASCENDER];
   Mac2IntelSHORT((LPSHORT)&HHEABuf[NUMOFHMETRICS]);
   numberOfHMetrics = (SHORT)*(LPSHORT)&HHEABuf[NUMOFHMETRICS];

   /*-------do hmtx-----------*/
   if ((lpdir=tagPair[HMTX].lpdir)== NULL) goto errOpen;
   if (-1==lseek(hf,lpdir->offset,SEEK_SET)) goto errOpen;
   len = (lpdir->length+511)&0xfffffe00;
   if (2048 !=read(hf,(LPCHAR)HMTXBuf,2048)) goto errOpen;

   /*-------do loca-----------*/
   if ((lpdir=tagPair[LOCA].lpdir)== NULL) goto errOpen;
   if (-1==lseek(hf,lpdir->offset,SEEK_SET)) goto errOpen;
   read(hf,(LPCHAR)LOCABuf,2048);

   /*-------do maxp-----------*/
   if ((lpdir=tagPair[MAXP].lpdir)== NULL) goto errOpen;
   if (-1==lseek(hf,lpdir->offset,SEEK_SET)) goto errOpen;
   read(hf,(LPCHAR)MAXPBuf,MAXPBUFLEN);


   /*-------do glyf-----------*/
   if ((lpdir=tagPair[GLYF].lpdir)== NULL) goto errOpen;
   glyphBase = lpdir->offset;

   Mac2IntelUSHORT((LPUSHORT)&MAXPBuf[NUMGLYPH]);
   numGlyphs = (USHORT)*(LPUSHORT)&HEADBuf[NUMGLYPH];
   return hf;

errOpen:
   close(hf);
   return NEG_ONE;
}


static int Char2Glyph(LPTTFINFO lpttf,USHORT c)
{
  USHORT gn,i,off,delta;
  USHORT  scnt,end,start;
  UCHAR  cc;

 if (cmap_format ==0) {
  cc = CMAPBuf[cmapstart+c+6];
  return cc;
 }

////////CD-ROM fix
  if (cmap_format == 2) {
     int hi,lo;
     USHORT HeaderKey,firstCode,entryCount,idRangeOffset;
     SHORT idDelta;
     // ULONG loc1,loc2,index;
     // long  len;
     int glyfn;
     hi = c/256;   lo = c%256;
     HeaderKey = *(LPUSHORT)(&CMAPBuf[cmapstart+6+hi*2]);
     Mac2IntelUSHORT(&HeaderKey);
     firstCode = *(LPUSHORT)(&CMAPBuf[cmapstart+6+512+HeaderKey]);
     entryCount= *(LPUSHORT)(&CMAPBuf[cmapstart+6+512+HeaderKey+2]);
     idDelta   = *(LPUSHORT)(&CMAPBuf[cmapstart+6+512+HeaderKey+4]);
     idRangeOffset = *(LPUSHORT)(&CMAPBuf[cmapstart+6+512+HeaderKey+6]);
     Mac2IntelUSHORT(&firstCode);
     Mac2IntelUSHORT(&entryCount);
     Mac2IntelSHORT(&idDelta);
     Mac2IntelUSHORT(&idRangeOffset);

     if (lo<firstCode||lo>=firstCode+entryCount) glyfn = 0;
     else {
        glyfn = *(LPUSHORT)(&CMAPBuf[cmapstart+6+512+HeaderKey+
                6+idRangeOffset+(lo - firstCode)*2]);
        Mac2IntelUSHORT(&glyfn);
        glyfn+=idDelta;
     }
     return glyfn;
  }
////////CD-ROM fix

  scnt = *(LPUSHORT) & CMAPBuf[cmapstart+6];
  Mac2IntelUSHORT(&scnt);
  scnt /=2;
  for (i=0;i<scnt;i++) {
       end = *(LPUSHORT) & CMAPBuf[cmapstart+14+i*2];
       Mac2IntelUSHORT(&end);
       start = *(LPUSHORT) & CMAPBuf[cmapstart+14+i*2+scnt*2+2];
       Mac2IntelUSHORT(&start);
       if (c>= start && c<= end) {
           delta = *(LPUSHORT) & CMAPBuf[cmapstart+14+i*2+scnt*4+2];
           Mac2IntelUSHORT(&delta);
           off = *(LPUSHORT) & CMAPBuf[cmapstart+14+i*2+scnt*6+2];
           Mac2IntelUSHORT(&off);
           if (off==0) return c+delta;
           gn = *(LPUSHORT) & CMAPBuf[cmapstart+14+i*2+scnt*6+2+(c-start)*2+off];
           Mac2IntelUSHORT(&gn);
           if (gn==0) return 0;
           return gn+delta;
       }
  }
  return 0;
}

/*
* get glyph n's location
*/

static VOID LocateGlyph(LPTTFINFO lpttf,
                 int n,
                 LPULONG poff,
                 LPINT plen)
{
   ULONG seekn,t;
   USHORT t1,t2;
   if (indexToLocFormat) {
         seekn = n*4;
         *poff = (ULONG)*(LPULONG)&LOCABuf[seekn];
         t = (ULONG)*(LPULONG)&LOCABuf[seekn+4];
         Mac2IntelULONG(poff);
         Mac2IntelULONG(&t);
         *plen = t-*poff;
         return;
   } else {
         seekn = n*2;
         t1 = (USHORT)*(LPUSHORT)&LOCABuf[seekn];
         t2 = (USHORT)*(LPUSHORT)&LOCABuf[seekn+2];
         Mac2IntelUSHORT(&t1);
                 Mac2IntelUSHORT(&t2);
                 t1 *=2; t2*=2;
         *poff = t1;
         *plen = t2-t1;
         return;
   }
}


/* read a glyph's raw data */

static BOOL ReadGlyph(LPTTFINFO lpttf,
               int hf,
               USHORT n)
{
    ULONG offset,t;
    USHORT instructionLength;
    int pointn;
    int len;
    int i,j;
    int wval,tmp;
    UCHAR flag,rep;

    LPBYTE pbuf;

    if (n<256) {
       LocateGlyph(lpttf,n,&offset,&len);
    } else {
      lseek(hf,tagPair[LOCA].lpdir->offset+n*4,SEEK_SET);
      read (hf,&offset,4);
      read (hf,&t,4);
      Mac2IntelULONG(&offset);
      Mac2IntelULONG(&t);
      if (t>offset) len=t-offset;
      else len = 0;
    }

    len +=511;
    len &= 0xfe00;
    offset += glyphBase;
    if (-1==lseek(hf,offset,SEEK_SET)) return FALSE;
    if (len ==0 || len >4096) return FALSE;
    read(hf,(LPCHAR)GLYFBuf,len);
    numberOfContours = (SHORT) *(LPSHORT)&GLYFBuf[NCONTOUR];
    Mac2IntelSHORT((LPSHORT)&numberOfContours);

    if (numberOfContours <0 || numberOfContours>TTMAXCONTOUR) return FALSE;
    pbuf = (LPBYTE)&GLYFBuf[ENDPTS];
    aglyf.contourn = numberOfContours;
    pointn = 0;
    for (i=0;i<numberOfContours;i++) {
           Mac2IntelUSHORT((LPUSHORT)pbuf);
           aglyf.enddot[i] = (USHORT)*(LPUSHORT)pbuf;
           pbuf += 2;
    }


    aglyf.pointn = pointn = aglyf.enddot[i-1]+1;
    if (pointn>TTMAXDOTN) return FALSE;

    Mac2IntelUSHORT((LPUSHORT)pbuf);
    instructionLength = (USHORT)*(LPUSHORT)pbuf;
    pbuf += (instructionLength+2);

    for (i=0;i<pointn;i++) {
         flag = *pbuf++;
         aglyf.flag[i] = flag;
         if (flag&REPEAT) {
               rep = *pbuf++;
               for (j=0;j<rep;j++) aglyf.flag[i+j+1] = flag;
               i+=rep;
         }
    }

    wval = tmp = 0;                             /* start xvalue */
    for (i=0;i<pointn;i++) {
       flag = aglyf.flag[i];

       if (flag&XSHORT) {
           tmp = *pbuf++;
           if (!(flag&XSAME)) tmp = -tmp;
           wval += tmp;
       }
       else  if (!(flag&XSAME)){
               Mac2IntelSHORT((LPSHORT)pbuf);
               wval += *(LPSHORT)pbuf;
               pbuf += 2;
       }
       aglyf.xbuf[i] = wval - TTxMin;
    }

    wval = tmp = 0;                             /* start yvalue */
    for (i=0;i<pointn;i++) {
        flag = aglyf.flag[i];

        if (flag&YSHORT) {
           tmp = *pbuf++;
           if (!(flag&YSAME)) tmp = -tmp;
           wval += tmp;
        } else if (!(flag&YSAME)) {
               Mac2IntelSHORT((LPSHORT)pbuf);
               wval += *(LPSHORT)pbuf;
               pbuf += 2;
        }
        aglyf.ybuf[i] =TTyMax-wval;          /* change coordinate */
    }
    return TRUE;
}

/*---------------------Mac2Intels------------*/
static VOID Mac2IntelUSHORT(p)
LPUSHORT p;
{
  UCHAR a;
  LPUCHAR pc;
  pc = (LPUCHAR)p;
  a = pc[1];
  pc[1] = pc[0];
  pc[0] = a;
}

static VOID Mac2IntelSHORT(p)
LPSHORT p;
{
  UCHAR a;
  LPUCHAR pc;
  pc = (LPUCHAR)p;
  a = pc[1];
  pc[1] = pc[0];
  pc[0] = a;
}

#ifdef UNUSED
static VOID Mac2IntelLONG(p)
LPLONG p;
{
  UCHAR a;
  LPUCHAR pc;
  pc = (LPUCHAR)p;
  a = pc[0];
  pc[0] = pc[3];
  pc[3] = pc[0];
  a = pc[1];
  pc[1] = pc[2];
  pc[2] = a;
}
#endif

static VOID Mac2IntelULONG(p)
LPULONG p;
{
  UCHAR a;
  LPUCHAR pc;
  pc = (LPUCHAR)p;
  a = pc[0];
  pc[0] = pc[3];
  pc[3] = a;
  a = pc[1];
  pc[1] = pc[2];
  pc[2] = a;
}

static VOID Mac2IntelTABLE(lpdir)
LPTABDIR lpdir;
{
   Mac2IntelULONG(&lpdir->tag);
   Mac2IntelULONG(&lpdir->checkSum);
   Mac2IntelULONG(&lpdir->offset);
   Mac2IntelULONG(&lpdir->length);
}

#define DEFAULT_AW 128                       //  1/2 of 256
#define DEFAULT_LSB 0

void TTWidth(LPDC lpdc,Wchar code,int font,USHORT *aw, SHORT *lsb)
{
  int glyphn;
  //int i,j,k,l;
  LPTTFINFO lpttf = lpdc->lpttf;

  *aw = DEFAULT_AW;
  *lsb = DEFAULT_LSB;

  //httf = NEG_ONE;
  if (!OpenTTFont(lpttf,font)) {
        close(httf);
        return;
  }
  //  code = GLYFN;

  glyphn = Char2Glyph(lpttf,code);
  if (glyphn > numGlyphs) return;

  if (glyphn <numberOfHMetrics) {
      *aw  = *(USHORT *) (HMTXBuf+glyphn*4);
      *lsb = *(SHORT *)  (HMTXBuf+glyphn*4+2);
  } else {
      *aw  = *(USHORT *) (HMTXBuf+(numberOfHMetrics-1)*4);
      *lsb = *(SHORT *)  (HMTXBuf+numberOfHMetrics*4+(glyphn-numberOfHMetrics)*2);
  }
  Mac2IntelUSHORT(aw);
  Mac2IntelSHORT(lsb);

  *aw = (long)CHAR_WIDTH_IN_DOT*(*aw)/unitsPerEM;
  *lsb =(long)CHAR_WIDTH_IN_DOT*(*lsb)/unitsPerEM;
}


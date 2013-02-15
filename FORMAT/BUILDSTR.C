/*-------------------------------------------------------------------
* Name: buildstr.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

// static char FontsPath[]="C:\\ezP\\fOnts\\";

static int detectDot16Font(int fontn)
{
///////only accept simplified songti font (fontn == 0)///////
    return (fontn == 0);
}


int DetectDot16Font(int fontn)
{
  #define MAX16FONT    1
  static signed char existfont[MAX16FONT] =  {-1};
  static unsigned char offset = 0;
  int i;

  for (i=0;i<MAX16FONT;i++)
      if (existfont[i] == fontn) return 1;

  if (!detectDot16Font(fontn)) return 0;
  offset ++;
  if (offset == MAX16FONT) offset = 0;
  existfont[offset] = fontn;
  return 1;
}

// added by Jerry 97/5/12 for 13x13 font
int DetectDot13Font(int fontn)
{
  if (fontn !=0 || !GlobalUseLIB13) return 0;
  return 1;
}

static int detectDot24Font(int fontn)
{
  //detect 24X24 dot matrix font, only for song,kai,hei,fang
  //clib name is: /ezp/fonts/clib24s,clib24k,clib24h,clib24f
  char str[32];
  FILE *fp;

  if (fontn<0 || fontn >3) return 0;    // out of range !
  strcpy (str,Dotlib24FileName);
  str[19] = SKHF_Name[fontn];

  fp = fopen(str,"rb");
  if (NULL==fp) return 0;
  fseek(fp,0L,SEEK_END);
  if (ftell(fp) > 65536L) {
        fclose (fp);
        return 1;
  } else {
        fclose (fp);
        return 0;
  }
}

int DetectDot24Font(int fontn)
{
  #define MAX24FONT    4
  static signed char existfont[MAX24FONT] =  {-1,-1,-1,-1};
  static unsigned char  offset = 0;
  int i;

  for (i=0;i<MAX24FONT;i++)
      if (existfont[i] == fontn) return 1;

  if (!detectDot24Font(fontn)) return 0;
  offset ++;
  if (offset == MAX24FONT) offset = 0;
  existfont[offset] = fontn;
  return 1;
}

int DetectDot24GBSymbol()
{
  static signed char found = -1;

  if (found != -1) return found;
  if (access(Symbol24FileName,0)==0)
        return (found = 1);

  return (found = 0);
}

int detectEFont(int CFont)
{
  char *cp1,*cp2;
  char name[80];
  char fontname[13];
  char ext[4];
  int  fontn=CFont+1;

  if (fontn<1 || fontn>999)
      return 0;

  ext[0]  = '0'+(fontn/100)%10;
  ext[1]  = '0'+ (fontn%100)/10;
  ext[2]  = '0'+ fontn%10;
  ext[3] = '\0';
  strcpy(fontname,"TTFALIB.");             // THVECT.
  strcat(fontname,ext);

  cp1=TrueTypeLibPath;

  while (*cp1) {                        /* SEARCH FOR ALL PATH */
     cp2 = name;
     while(*cp1 != ';' && *cp1 != '\0')
                 *cp2++ = *cp1++;
     if (*(cp2-1) !='/' && *(cp2-1) != '\\') *cp2++ = '\\';
     if (*cp1 == ';') cp1++;
     strcpy(cp2,fontname);
     if(access(name,0)==0)      // this lib is exist
        return 1;
  }
  return 0;

}

static int detectVecFont(int CFont)
{
  char *cp1,*cp2;
  char name[80];
  char fontname[13];
  char ext[4];
  int  fontn=CFont+1;

  if (fontn<1 || fontn>100)
      return 0;

  ext[0]  = '0';      // + fontn/100;
  ext[1]  = '0'+ (fontn%100)/10;
  ext[2]  = '0'+ fontn%10;
  ext[3] = '\0';
  strcpy(fontname,cfnName);             // THVECT.
  strcat(fontname,ext);
  cp1=VectLibPath;

  while (*cp1) {                        /* SEARCH FOR ALL PATH */
     cp2 = name;
     while(*cp1 != ';' && *cp1 != '\0')
                 *cp2++ = *cp1++;
     if (*(cp2-1) !='/' && *(cp2-1) != '\\') *cp2++ = '\\';
     if (*cp1 == ';') cp1++;
     strcpy(cp2,fontname);
     if(access(name,0)==0)      // this lib is exist
        return 1;
  }
return 0;
}

int DetectVecFont(int fontn)
{
  #define MAXVECFONTCACHE    8
  static signed char existfont[MAXVECFONTCACHE] =  { -1,-1,-1,-1,-1,-1,-1,-1
                                               };
  static unsigned char  offset = 0;
  int i;

  for (i=0;i<MAXVECFONTCACHE;i++)
     if (existfont[i] == fontn) return 1;

  if (!detectVecFont(fontn)) return 0;
  offset ++;
  if (offset == MAXVECFONTCACHE) offset = 0;
  existfont[offset] = fontn;
  return 1;
}

static int detectTrueTypeFont(int CFont)
{
  char *cp1,*cp2;
  char name[80];
  char fontname[13];
  int  fontn=CFont+1;

  if (fontn<100 || fontn>300)
      return 0;

//.... find font in font path c:\;d:\;e:\;c:\ezp\fonts\ ...
//  if find, return 1 else return 0 ...
  strcpy(fontname,"RED0000.TTF");
  fontname[4]  += fontn/100;
  fontname[5]  += (fontn%100)/10;
  fontname[6]  += fontn%10;

  cp1=TrueTypeLibPath;

  while (*cp1) {                        /* SEARCH FOR ALL PATH */
     cp2 = name;
     while(*cp1 != ';' && *cp1 != '\0')
                 *cp2++ = *cp1++;
     if (*(cp2-1) !='/' && *(cp2-1) != '\\') *cp2++ = '\\';
     if (*cp1 == ';') cp1++;
     strcpy(cp2,fontname);
     if(access(name,0)==0)      // this lib is exist
        return 1;
  }
  return 0;
}

int DetectTrueTypeFont(int fontn)
{

  #define MAXTTFFONT    8
  static signed char existfont[MAXTTFFONT] =  {-1,-1,-1,-1,-1,-1,-1,-1};
  static unsigned char  offset = 0;
  int i;

  for (i=0;i<MAXTTFFONT;i++)
     if (existfont[i] == fontn) return 1;

  if (!detectTrueTypeFont(fontn)) return 0;
  offset ++;
  if (offset == MAXTTFFONT) offset = 0;
  existfont[offset] = fontn;
  return 1;

}

int hh(void)
{
   if (GlobalExtFormat.dim3||GlobalExtFormat.hollow) return 1;
   return 0;
}
void BuildChineseChar( int PosX, int PosY, Wchar Code, int CFont,
                       int Width, int Height, int Slant,
                       int RotateAngle, int Color)
{
  /*
  RotateAngle%=360;
  RotateAngle+=360*(0x0105);      //By zjh DIM3
  */
  //if (Width==Height) Height--;      //By zjh test


  if(Code<=0xa1a1)             // space
      return;



  if(fEditor)
  {
      ViewportDisplayChar16(PosX,PosY,Code,0,Width,Height,Slant,
                     RotateAngle,Color);
      return;
  }
  if (hh())
  {
      if( DetectVecFont(CFont) )
         {
          BuildChar(PosX,PosY,Code,CFont+1,Width,Height,Slant,RotateAngle,
                     Color,0,RITALICBIT|ROTATEBIT,0);
          return ;
         }
      else if (DetectTrueTypeFont(CFont))
         {
           BuildCharTTF(PosX,PosY,Code,CFont,Width,Height,Slant,RotateAngle,Color);
           return ;
         }
  }

  if(!PrintingSign)
  {
     if (Width<TOOSMALLSIZE || Height<TOOSMALLSIZE)
     {
        ViewportDisplaySmallChar(PosX,PosY,Width,Height,Color);
        return;
     }
  }

  if (!PrintingSign) {
      if( Code>0xa1a1 && Code<0xb0a1 )       // symbol
      {
         if( (Width>20||Height>20) && DetectDot24GBSymbol() )
         {
            ViewportDisplaySymbol24(PosX,PosY,Code,Width,Height,Slant,
                     RotateAngle,Color);
            return;
         }
         // added by Jerry 97/5/12 for 13x13 font!
         // !!!! No Rotation  NO Slant !!!!
         if (Width==13 && Height == 13 && DetectDot13Font(0)
             && Slant == 0 && RotateAngle == 0) {
             BuildChar13(PosX,PosY,Code,Color);
         } else
         ViewportDisplayChar16(PosX,PosY,Code,0,Width,Height,Slant,
                     RotateAngle,Color);
      }
      else
         // added by Jerry 97/5/12 for 13x13 font!
         // !!!! No Rotation  NO Slant !!!!
         if (Width==13 && Height == 13 && DetectDot13Font(CFont)
             && Slant == 0 && RotateAngle == 0) {
             BuildChar13(PosX,PosY,Code,Color);
         } else
      if ( Width<20 && Height<20 && DetectDot16Font(CFont) )
         ViewportDisplayChar16(PosX,PosY,Code,CFont,Width,Height,Slant,
                     RotateAngle,Color);
      else
      if ( Width<34 && Height<34 && DetectDot24Font(CFont) )
         ViewportDisplayChar24(PosX,PosY,Code,CFont,Width,Height,Slant,
                     RotateAngle,Color);
      else if( DetectVecFont(CFont) )
         ViewportDisplayCharVec(PosX,PosY,Code,CFont,Width,Height,Slant,
                     RotateAngle,Color);
      else if (DetectTrueTypeFont(CFont))
         ViewportDisplayCharTTF(PosX,PosY,Code,CFont,Width,Height,Slant,
                     RotateAngle,Color);
      else if( DetectDot24Font(CFont) )
         ViewportDisplayChar24(PosX,PosY,Code,CFont,Width,Height,Slant,
                     RotateAngle,Color);
      else
      {
         if( CFont>0 && !DetectDot16Font(CFont) )
              CFont=0;        // SongTi

         ViewportDisplayChar16(PosX,PosY,Code,CFont,Width,Height,Slant,
                     RotateAngle,Color);
      }
   }
   else  /*---- printing ----*/
   {
      if((RotateAngle%360))
      {
         BuildChar(PosX,PosY,Code,CFont+1,Width,Height,Slant,RotateAngle,
              Color,0,RITALICBIT|ROTATEBIT,0);
         return;
      }

      //if(CFont<3)        // MAX Cfont=3
      if( Code>0xa1a1 && Code<0xb0a1 )       // symbol
      {
         if(Width<20 && Height<20)       // very small size
         {
             if( DetectDot16Font(0) )    // use songti
                 BuildChar16(PosX,PosY,Code,0,Width,Height,Slant,RotateAngle,Color);
             else goto lbl_symbol24;
         }
         else
         if(Width<28&&Height<28)         // small size
         {
          lbl_symbol24:
             if( DetectDot24GBSymbol() )  // use clib24t
                 BuildSymbol24(PosX,PosY,Code,Width,Height,Slant,RotateAngle,Color);
             else goto lbl_symbol30;
         }
         else                            // medium or large  size
         {
          lbl_symbol30:          // use thvect.030
             BuildChar(PosX,PosY,Code,30,Width,Height,Slant,RotateAngle,
                Color,0,RITALICBIT|ROTATEBIT,0);
         }
      }
      else
      if ( Width<17 && Height<17 )       // very small size
      {
         if( DetectDot16Font(CFont) )
             BuildChar16(PosX,PosY,Code,CFont,Width,Height,Slant,RotateAngle,Color);
         else goto lbl_small;
      }
      else
      if ( Width<28 && Height<28 )       // small size
      {
        lbl_small:
         if( DetectDot24Font(CFont) )
             BuildChar24(PosX,PosY,Code,CFont,Width,Height,Slant,RotateAngle,Color);
         else if( DetectVecFont(CFont) )
             BuildChar(PosX,PosY,Code,CFont+1,Width,Height,Slant,RotateAngle,
                        Color,0,RITALICBIT|ROTATEBIT,0);
         else if (DetectTrueTypeFont(CFont))
             BuildCharTTF(PosX,PosY,Code,CFont,Width,Height,Slant,RotateAngle,Color);
         else            // use songti
             BuildChar16(PosX,PosY,Code,0,Width,Height,Slant,RotateAngle,Color);
      }
      else
      if ( Width<60 && Height<60 )       // medium size
      {
         if( DetectVecFont(CFont) )
             BuildChar(PosX,PosY,Code,CFont+1,Width,Height,Slant,RotateAngle,
                        Color,0,RITALICBIT|ROTATEBIT,0);
         else if (DetectTrueTypeFont(CFont))
             BuildCharTTF(PosX,PosY,Code,CFont,Width,Height,Slant,RotateAngle,Color);
         else if( DetectDot24Font(CFont) )
             BuildChar24(PosX,PosY,Code,CFont,Width,Height,Slant,RotateAngle,Color);
         else            // use songti
             BuildChar16(PosX,PosY,Code,0,Width,Height,Slant,RotateAngle,Color);
      }
      else                       // large size
      {
         if (DetectTrueTypeFont(CFont))
             BuildCharTTF(PosX,PosY,Code,CFont,Width,Height,Slant,RotateAngle,Color);
         else if( DetectVecFont(CFont) )
             BuildChar(PosX,PosY,Code,CFont+1,Width,Height,Slant,RotateAngle,
                        Color,0,RITALICBIT|ROTATEBIT,0);
         else if( DetectDot24Font(CFont) )
             BuildChar24(PosX,PosY,Code,CFont,Width,Height,Slant,RotateAngle,Color);
         else            // use songti
             BuildChar16(PosX,PosY,Code,0,Width,Height,Slant,RotateAngle,Color);
      }
   } /*------ PrintingSign --------*/
}

void BuildEnglishChar( int PosX, int PosY, Wchar Code, int EFont,
                       int Width, int Height, int Slant,
                       int RotateAngle, int Color )
{
  // if (Code==ENTER ||Code==BLANK)
  if (Code<=BLANK)
     return;

  if(!PrintingSign)
  {
     if (Width<TOOSMALLSIZE || Height<TOOSMALLSIZE)
     {
        USHORT aw;
        short  lsb;
        GetTtfWidth(Code,EFont,&aw,&lsb);
        ViewportDisplaySmallChar(PosX,PosY,Width*aw/CHAR_WIDTH_IN_DOT,Height,Color);
        return;
     }
  }


  if(fEditor)
  {
      //ViewportDisplayChar(PosX,PosY,Code,Width,Height,Slant,
      ViewportDisplayEChar(PosX,PosY,Code,0,Width,Height,Slant,
                     RotateAngle,Color);
      return;
  }

  // if (!PrintingSign && EFont==SONGTI)
  if (!PrintingSign && EFont<MAXEFONT)
  {
      //NowEFont=EFont;
      //ViewportDisplayChar(PosX,PosY,Code,Width,Height,Slant,
      //By zjh 11.3 below tow rows
      ViewportDisplayEChar(PosX,PosY,Code,EFont,Width,Height,Slant,
                     RotateAngle,Color);
      //BuildChar(PosX,PosY,Code,EFont+1,Width,Height,Slant,RotateAngle,
      //         Color,0,RITALICBIT|ROTATEBIT,0);
  }
  else
  {
      BuildChar(PosX,PosY,Code,EFont+1,Width,Height,Slant,RotateAngle,
               Color,0,RITALICBIT|ROTATEBIT,0);
  }
}

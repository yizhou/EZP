#include <stdio.h>
#include <dir.h>
#include <graphics.h>
#include <string.h>
#include "pdef.h"

struct tagBitMapFileHeader {
   WORD         bfType;
   DWORD        bfSize;
   WORD Reserved1;
   WORD Reserved2;
   DWORD        bfOffset;
} BitMapFileHead;

struct tagBitMapInfoHeader {
   DWORD        biSize;
   DWORD        biWidth;
   DWORD        biHeight;
   WORD         biPlanes;
   WORD biBitCount;
   DWORD        biCompression;
   DWORD        biSizeImage;
   DWORD        biXpelsPerMeter;
   DWORD        biYpelsPerMeter;
   DWORD        biClrUsed;
   DWORD        biClrImportant;
} BitMapInfoHead;

struct tagRGBquad {
   BYTE rgbBlue;
   BYTE rgbGreen;
   BYTE rgbRed;
   BYTE rgbReserved;
} RGBquad;

void chang_bmp(FILE *fp,FILE *fd)
{
    DWORD h,w, seekp;
    WORD WidthByte;
    unsigned char buf[128];
    int i;

    fseek(fp, BitMapFileHead.bfOffset, 0);
    h=BitMapInfoHead.biHeight;
    w=BitMapInfoHead.biWidth;
    WidthByte=((w+7)/8)*8/2;              /*- div 2: 2 colors/byte -*/
    seekp=BitMapFileHead.bfOffset+WidthByte*(h-1);

    for(i=0;i<h;i++) {
       fseek(fp, seekp-(long)WidthByte*i, 0);
       fread(buf,1,WidthByte,fp);
       fwrite(buf,1,w/2,fd);
    }

}

FileCat(filename,extname)
char *filename,*extname;
{
   char *p1,*p2,c;
   char *fn=filename;

   do {
     p1=strchr(fn,'.');
     if(p1!=NULL) {
        p2=p1+1;
        c=*p2;
        if(c=='.') {p2++; c=*p2;}      /* .. */
        if(c=='\\')  fn=p2+1;
          else *p1=0;      /* trunc filename */
     }
  } while(p1!=NULL);

   strcat(filename,extname);
}

main(int argc,char *argv[])
{
    FILE   *fp,*fd;
    char   SrcFile[80],DstFile[80];
    struct ffblk f;
    char   drv[4];
    char   dir[80];
    char   f_name[14];
    char   f_ext[5];
    int    done;
    int    total_file=0;

    if(argc<2) {
       puts("Usage : CHGBMP filename[.bmp]");
       exit(1);
    }

    strcpy(SrcFile,argv[1]);
    FileCat(SrcFile,".BMP");

    done=findfirst(SrcFile,&f,0);
    if(done==-1) {
         printf("Can not find file %s\n",SrcFile);
         exit(2);
    }

    fnsplit(SrcFile,drv,dir,f_name,f_ext);

    while(!done) {
         strcpy(SrcFile,drv);
         strcat(SrcFile,dir);
         strcat(SrcFile,f.ff_name);
         strcpy(DstFile,SrcFile);
	 FileCat(DstFile,".");

         fp=fopen(SrcFile,"rb");
         fread(&BitMapFileHead,sizeof(BitMapFileHead), 1, fp);
         fread(&BitMapInfoHead,sizeof(BitMapInfoHead), 1, fp);
         if(BitMapFileHead.bfType!=0x4d42) {
	     printf("File(%s) is not a BMP file.\n",SrcFile);
	     goto chg_next;
          }

         if(BitMapFileHead.bfOffset!=118
	    ||BitMapInfoHead.biHeight!=24
	    ||BitMapInfoHead.biWidth!=24 ) {
	     printf("File(%s) is not a Correct BMP file.\n",SrcFile);
	     goto chg_next;
          }

        /*  set_palette(fp); */
         fd=fopen(DstFile,"wb");
         chang_bmp(fp,fd);
	 fclose(fd);
	 total_file++;
    chg_next:
	 fclose(fp);
	 done=findnext(&f);
    }
    closegraph();
    if(total_file)
      printf("\nOK! Total %d file(s) have been changed.\n",total_file);
    else printf("\nDo nothing.\n");
}

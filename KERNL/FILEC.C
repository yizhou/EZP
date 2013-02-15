/*-------------------------------------------------------------------
* Name: filec.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static int mycopy(char *nas,char *nad)
{
  #define COPY_LEN      4096
  FILE *fp,*fp1;
  unsigned char buff[COPY_LEN+4];
  int len;

  fp=fopen(nas,"rb");
  if (fp==NULL) return -1;

  fp1=fopen(nad,"wb");
  if (fp1==NULL)
  {
    fclose(fp);
    return -1;
  }

  len=COPY_LEN;
  while (len==COPY_LEN && (!feof(fp)))
  {
    len=fread(buff,1,COPY_LEN,fp);
    if (len) fwrite(buff,1,len,fp1);
  }

  fclose(fp);
  fclose(fp1);
  return 0;
 #undef COPY_LEN
}

static int SaveFileToBackup(char *FileName)
{
    char BackupFile[200];
    // int i,len;

    strcpy(BackupFile,FileName);
    name_ext(BackupFile,"bak");
    return mycopy(FileName,BackupFile);
}

#define IsSavePrintBlock()    (1)
#define IsSavePrintScale()    (1)

#define FileHaveExternBlock(val)  ((val)&1)

void SetDefaultExternBlock(void)
{
   PG.Blocks=0;
   PXScale=1.0;
   PYScale=1.0;
}

//int SetExternBlock(unsigned short *v){   *v |=1;   return 1;}
#define SetExternBlock(v)    (v|=1)

int WriteExternBlock(FILE *fp)
{
    unsigned short attr;
    if (IsSavePrintBlock())
    {
        attr=1;
        if (fwrite(&attr,sizeof(attr),1,fp)<1) return -1;
        attr=sizeof(PrintBlock)*PG.Blocks+8*sizeof(short);
        if (fwrite(&attr,sizeof(attr),1,fp)<1) return -1;
        if (attr)
          if (fwrite(&PG.Blocks,attr,1,fp)<1) return -1;
    }

    if (IsSavePrintScale())
    {
        attr=2;
        if (fwrite(&attr,sizeof(attr),1,fp)<1) return -1;
        attr=sizeof(PXScale)+sizeof(PYScale);
        if (fwrite(&attr,sizeof(attr),1,fp)<1) return -1;
        if (attr)
          {
          if (fwrite(&PXScale,sizeof(PXScale),1,fp)<1) return -1;
          if (fwrite(&PYScale,sizeof(PYScale),1,fp)<1) return -1;
          }
    }

    attr=0;   //Over
    if (fwrite(&attr,sizeof(attr),1,fp)<1) return -1;

    return 1;
    //return -1;   //Read error
    //return 1 ;   //Read Ok
}

int ReadExternBlock(FILE *fp)
{
    unsigned short attr;
    while (1)
    {
      if (fread(&attr,sizeof(attr),1,fp)<1) return -1;
      switch (attr)
      {
        case 0:
                return 1;
        case 1:     //Print Control Block;
                {
                unsigned short Len;
                if (fread(&Len,sizeof(Len),1,fp)<1) return -1;
                if (Len)
                  if (fread(&PG.Blocks,Len,1,fp)<1) return -1;
                }
                break;
        case 2:
                if (fread(&attr,sizeof(attr),1,fp)<1) return -1;
                if (attr)
                  {
                  if (fread(&PXScale,sizeof(PXScale),1,fp)<1) return -1;
                  if (fread(&PYScale,sizeof(PYScale),1,fp)<1) return -1;
                  }
                break;
        default:
                return -1;
      }
    }
    //return -1;   //Read error
    //return 1;   //Read Ok
}

static int FileSaveBox(FILE *fp,HTEXTBOX HBox)
{
  TextBoxs *HBoxSetting;
  Wchar *HText;
  FileBoxTables FileBoxTable;

  HBoxSetting=HandleLock(ItemGetHandle(HBox));
  if (HBoxSetting==NULL)
     return(OUTOFMEMORY);

  memset(&FileBoxTable,0,sizeof(FileBoxTable));
  FileBoxTable.BoxDescribleHandle=HBox;
  if (IsModule)
  {
    BoxSetLocked(HBoxSetting);
    BoxSetIsModule(HBoxSetting);
  }

  switch (TextBoxGetBoxType(HBoxSetting))
  {
    case TEXTBOX:
         FileBoxTable.BoxParameterLength=sizeof(TextBoxs);
         FileBoxTable.TextLength=TextBoxGetTextLength(HBoxSetting);
         if (fwrite(&FileBoxTable,sizeof(FileBoxTable),1,fp)<1)
         {
            HandleUnlock(ItemGetHandle(HBox));
            return(FILEWRITE);
         }
         if (fwrite(HBoxSetting,sizeof(TextBoxs),1,fp)<1)
         {
            HandleUnlock(ItemGetHandle(HBox));
            return(FILEWRITE);
         }
         if (FileBoxTable.TextLength&&!TextBoxGetPrevLinkBox(HBoxSetting))
         {
            int Result,i;
            HBOX InsertHBox;

            HText=HandleLock(TextBoxGetTextHandle(HBoxSetting));
            if (HText==NULL)
            {
               HandleUnlock(TextBoxGetTextHandle(HBoxSetting));
               HandleUnlock(ItemGetHandle(HBox));
               return(FILEWRITE);
            }
            if( fwrite(HText,sizeof(Wchar)*TextBoxGetTextLength(HBoxSetting),
                         1,fp)<1 )
            {
               HandleUnlock(TextBoxGetTextHandle(HBoxSetting));
               HandleUnlock(ItemGetHandle(HBox));
               return(FILEWRITE);
            }
            i=0;
            while (i<FileBoxTable.TextLength)
            {
              InsertHBox=EditBufferSearchNextAttribute(HText,i,
                         FileBoxTable.TextLength,INSERTBOX,&i);
              if (InsertHBox>0)
              {
                 if ((Result=FileSaveBox(fp,InsertHBox))<OpOK)
                 {
                    HandleUnlock(TextBoxGetTextHandle(HBoxSetting));
                    return(Result);
                 }
                 i++;
              }
              else
                 break;
            }
            HandleUnlock(TextBoxGetTextHandle(HBoxSetting));
         }
         break;
    case TABLEBOX:
         {
            PFormBoxs FormBox;

            FileBoxTable.BoxParameterLength=sizeof(FormBoxs);
            FormBox=(PFormBoxs)HBoxSetting;
            FileBoxTable.TextLength=TableBoxGetTextLength(FormBox);
            if (fwrite(&FileBoxTable,sizeof(FileBoxTable),1,fp)<1)
            {
               HandleUnlock(ItemGetHandle(HBox));
               return(FILEWRITE);
            }
            if (fwrite(FormBox,sizeof(FormBoxs),1,fp)<1)
            {
               HandleUnlock(ItemGetHandle(HBox));
               return(FILEWRITE);
            }
            if (TableBoxGethCellTable(FormBox))
            {
               CELL *pCell;

               pCell=HandleLock(TableBoxGethCellTable(FormBox));
               if (fwrite(pCell,sizeof(CELL)*
                 TableBoxGetnumLines(FormBox)*
                 TableBoxGetnumCols(FormBox),1,fp)<1)
               {
                  HandleUnlock(TableBoxGethCellTable(FormBox));
                  HandleUnlock(ItemGetHandle(HBox));
                  return(FILEWRITE);
               }
               HandleUnlock(TableBoxGethCellTable(FormBox));
            }
            if (FileBoxTable.TextLength&&!TableBoxGetPrevLinkBox(FormBox))
            {
               HText=HandleLock(TableBoxGetTextHandle(FormBox));
               if (HText==NULL)
               {
                  HandleUnlock(TextBoxGetTextHandle(HBoxSetting));
                  HandleUnlock(ItemGetHandle(HBox));
                  return(FILEWRITE);
               }
               if (fwrite(HText,sizeof(Wchar)*TableBoxGetTextLength(FormBox),1,fp)<1)
               {
                  HandleUnlock(TableBoxGetTextHandle(FormBox));
                  HandleUnlock(ItemGetHandle(HBox));
                  return(FILEWRITE);
               }
               HandleUnlock(TableBoxGetTextHandle(FormBox));
            }
         }
         break;
    case LINEBOX:
         {
            LineBoxs *LineBox;

            if (IsModule)
            BoxSetNotEdit(HBoxSetting);         //By zjh 9.12

            FileBoxTable.BoxParameterLength=sizeof(LineBoxs);
            LineBox=(LineBoxs *)HBoxSetting;
            FileBoxTable.TextLength=0;
            if (fwrite(&FileBoxTable,sizeof(FileBoxTable),1,fp)<1)
            {
               HandleUnlock(ItemGetHandle(HBox));
               return(FILEWRITE);
            }
            if (fwrite(LineBox,sizeof(LineBoxs),1,fp)<1)
            {
               HandleUnlock(ItemGetHandle(HBox));
               return(FILEWRITE);
            }
         }
         break;
    case RECTANGLEPICTUREBOX:
    case CORNERPICTUREBOX:
    case ELIPSEPICTUREBOX:
         {
           ImageDescribes *TiffPresent;

           FileBoxTable.BoxParameterLength=sizeof(PictureBoxs);
           TiffPresent=&PictureBoxGetPicturePresent(HBoxSetting);
           FileBoxTable.TextLength=TiffPresent->ImageWidth*
                                   TiffPresent->ImageHeight/2;
                                     /* No Text but Image data */

           if (fwrite(&FileBoxTable,sizeof(FileBoxTable),1,fp)<1)
           {
              HandleUnlock(ItemGetHandle(HBox));
              return(FILEWRITE);
           }
           if (fwrite(HBoxSetting,sizeof(PictureBoxs),1,fp)<1)
           {
              HandleUnlock(ItemGetHandle(HBox));
              return(FILEWRITE);
           }
           {
             unsigned char *ImagePresentData;
             // long ImageSize;

             if (TiffPresent->ImageHandle)
             {
                ImagePresentData=HandleLock(TiffPresent->ImageHandle);
                if (ImagePresentData==NULL)
                {
                   HandleUnlock(ItemGetHandle(HBox));
                   return(OUTOFMEMORY);
                }
                if (fwrite(ImagePresentData,FileBoxTable.TextLength,1,fp)<1)
                {
                   HandleUnlock(ItemGetHandle(HBox));
                   HandleUnlock(TiffPresent->ImageHandle);
                   return(FILEWRITE);
                }
                HandleUnlock(TiffPresent->ImageHandle);
             }
           }
         }
         break;
    case POLYGONPICTUREBOX:
         {
           int Polygonpointsize,*PolygonPointer;
           ImageDescribes *TiffPresent;

           PolygonPointer=HandleLock(PictureBoxGetBorderPolygon(HBoxSetting));
           if (PolygonPointer==NULL)
              Polygonpointsize=0;
           else
              Polygonpointsize=((*PolygonPointer)*2+1)*sizeof(*PolygonPointer);
           FileBoxTable.BoxParameterLength=sizeof(PictureBoxs)
                                           +Polygonpointsize;
           TiffPresent=&PictureBoxGetPicturePresent(HBoxSetting);
           FileBoxTable.TextLength=TiffPresent->ImageWidth*
                                   TiffPresent->ImageHeight/2;
                                     /* No Text but Image data */

           if (fwrite(&FileBoxTable,sizeof(FileBoxTable),1,fp)<1)
           {
              HandleUnlock(PictureBoxGetBorderPolygon(HBoxSetting));
              HandleUnlock(ItemGetHandle(HBox));
              return(FILEWRITE);
           }
           if (fwrite(HBoxSetting,sizeof(PictureBoxs),1,fp)<1)
           {
              HandleUnlock(PictureBoxGetBorderPolygon(HBoxSetting));
              HandleUnlock(ItemGetHandle(HBox));
              return(FILEWRITE);
           }
           if (fwrite(PolygonPointer,FileBoxTable.BoxParameterLength
                      -sizeof(PictureBoxs),1,fp)<1)
           {
              HandleUnlock(PictureBoxGetBorderPolygon(HBoxSetting));
              HandleUnlock(ItemGetHandle(HBox));
              return(FILEWRITE);
           }
           HandleUnlock(PictureBoxGetBorderPolygon(HBoxSetting));
           {
             unsigned char *ImagePresentData;

             if (TiffPresent->ImageHandle)
             {
                ImagePresentData=HandleLock(TiffPresent->ImageHandle);
                if (ImagePresentData==NULL)
                {
                   HandleUnlock(ItemGetHandle(HBox));
                   return(OUTOFMEMORY);
                }
                if (fwrite(ImagePresentData,FileBoxTable.TextLength,1,fp)<1)
                {
                   HandleUnlock(ItemGetHandle(HBox));
                   HandleUnlock(TiffPresent->ImageHandle);
                   return(FILEWRITE);
                }
                HandleUnlock(TiffPresent->ImageHandle);
             }
           }
         }
         break;
    default:
         FileBoxTable.BoxParameterLength=0;
         break;
  }

  if (IsModule)
  {
  BoxSetNotModule(HBoxSetting);         //By zjh 9.12
  BoxSetUnlocked(HBoxSetting);
  }

  HandleUnlock(ItemGetHandle(HBox));
  ReturnOK();
}

int FileSave(char *FileName)
{
  FileHeads FileHead;
  FilePageTables FilePageTable;
  // FileBoxTables FileBoxTable;
  FILE *fp;
  HITEM HPage,HBox;
  long SavePageOffset;
  int Result;

  strupr(FileName);
  Result=strlen(FileName)-4;
  if (strncmp(FileName+Result,".FRA",4)) IsModule=0;
    else IsModule=1;


  if ((fp=fopen(FileName,"wb"))==NULL)
     return(FILEOPEN);

  memset(&FileHead,0,sizeof(FileHead));
  strcpy(FileHead.FileSign,FILEHEADSTRING);
  FileHead.Version=FILEVERSION;
  FileHead.TotalFilePages=0;
  FileHead.PageParameterLength=sizeof(Pages);

  strcpy(FileHead.PageHeadLeftStr,PageHeadLeftStr);     // ByHance, 96,3.8
  strcpy(FileHead.PageHeadRightStr,PageHeadRightStr);     // ByHance, 96,3.8
  FileHead.StartPageNum=PgFtStartNum;
  FileHead.PageFootType=GetPageFootOption()
      | ( GetPageFootTopOption()<<1 )
      | ( GetPageFootLeftOption()<<2 )
      | ( GetPageFootPrevOption()<<4 );
  FileHead.PageHeadType=GetPageHeadOption()
      | ( GetPageHeadLeftOption()<<2 )
      | ( GetPageHeadLineOption()<<4 );

  //SetExternBlock(&FileHead.attr);
  SetExternBlock(FileHead.attr);

  if (fwrite(&FileHead,sizeof(FileHead),1,fp)<1)
  {
     fclose(fp);
     return(FILEWRITE);
  }

  if (FileHaveExternBlock(FileHead.attr))
      if (WriteExternBlock(fp)<1) return (FILEWRITE);

  memset(&FilePageTable,0,sizeof(FilePageTable));
  HPage=ItemGetChild(GlobalPageHeadHandle);
  while (HPage)            /* From the first page to the last one */
  {
    long CurrentFilePosition;
    Pages *MidPage;

    FilePageTable.TotalPageBox=0;
    SavePageOffset=ftell(fp);
    if (fwrite(&FilePageTable,sizeof(FilePageTable),1,fp)<1)
    {
       fclose(fp);
       return(FILEWRITE);
    }

    MidPage=HandleLock(ItemGetHandle(HPage));
    if (MidPage==NULL)
    {
       HandleUnlock(ItemGetHandle(HPage));
       fclose(fp);
       return(OUTOFMEMORY);
    }
    if (fwrite(MidPage,sizeof(Pages),1,fp)<1)
    {
       HandleUnlock(ItemGetHandle(HPage));
       fclose(fp);
       return(FILEWRITE);
    }
    else
       HandleUnlock(ItemGetHandle(HPage));

    HBox=PageGetBoxHead(HPage);
    while (HBox)                       /* Form the first box to the last one */
    {
      if ((Result=FileSaveBox(fp,HBox))<0)
      {
         HandleUnlock(ItemGetHandle(HPage));
         fclose(fp);
         return(Result);
      }
      HBox=ItemGetNext(HBox);
      FilePageTable.TotalPageBox++;
    }

    CurrentFilePosition=ftell(fp);
    fseek(fp,SavePageOffset,SEEK_SET); /* Write back page head */
    if (fwrite(&FilePageTable,sizeof(FilePageTable),1,fp)<1)
    {
       fclose(fp);
       return(FILEWRITE);
    }
    fseek(fp,CurrentFilePosition,SEEK_SET);

    HPage=ItemGetNext(HPage);
    FileHead.TotalFilePages++;
  }

  fseek(fp,0,SEEK_SET);                /* Write back file head */
  if (fwrite(&FileHead,sizeof(FileHead),1,fp)<1)
  {
     fclose(fp);
     return(FILEWRITE);
  }

  fclose(fp);
  ReturnOK();
}

static int FileLoadBox(FILE *fp,FileBoxLinks *FileBoxLinkArray,
                         int *BoxLinkIndex,HPAGE MidHPage,char SeekSign)
{
  FileBoxTables FileBoxTable;
//  Boxs MidBox;                // ByHance
#define MidBox  TmpBuf
  TextBoxs *MidPBox;
  HBOX MidHBox;
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  unsigned char *ImageReadData;

  if (fread(&FileBoxTable,sizeof(FileBoxTable),1,fp)<1)
     return(FILEREAD);
  if (fread(&MidBox,FileBoxTable.BoxParameterLength,1,fp)<1)
     return(FILEREAD);                 /* equal to sizeof(Boxs) */

                                       /* for read next version's file */
  /*if (FileBoxTable.BoxParameterLength-sizeof(Boxs)!=0)
     fseek(fp,(short)(FileBoxTable.BoxParameterLength-
              (short)sizeof(Boxs)),SEEK_CUR);*/

  if (IsModule)
   {
    MidBox.TextBox.BoxAttr&=0xfffe;        //Clear module sign  By zjh
    MidBox.TextBox.BoxStatus&=0xfe;        //Clear lock sign  By zjh
   }

  switch (MidBox.TextBox.BoxType)
  {
    // int BoxDescribeSize;
    Wchar *TextBlock;

    case TEXTBOX:
         TextBoxInitialLineTable(&MidBox.TextBox);
         if (!SeekSign)
         {
            FileSetBeenLoading();
            MidHBox=TextBoxInsert(&MidBox.TextBox,MidHPage);
            FileSetDoneLoading();
            if (!MidHBox)
               return(OUTOFMEMORY);
            FileBoxLinkArray[*BoxLinkIndex].HIndexTextBox=
                            FileBoxTable.BoxDescribleHandle;
            FileBoxLinkArray[*BoxLinkIndex].HConvertTextBox=MidHBox;
            (*BoxLinkIndex)++;
            MidPBox=HandleLock(ItemGetHandle(MidHBox));
         }                             /* Read and Insert Text */
         else
            MidPBox=&MidBox.TextBox;

         if (MidPBox==NULL)
            return(OUTOFMEMORY);
         if ((TextBoxGetBlockLength(MidPBox)!=0)
             &&(!TextBoxGetPrevLinkBox(MidPBox)))
         {
            HBOX InsertHBox;
            int i,Result;

            TextBoxSetTextHandle(MidPBox,HandleAlloc(sizeof(unsigned short)*
                                 TextBoxGetBlockLength(MidPBox),0));
            if (TextBoxGetTextHandle(MidPBox)==0)
            {
               TextBoxSetBlockLength(MidPBox,0);
               TextBoxSetTextLength(MidPBox,0);
               HandleUnlock(ItemGetHandle(MidHBox));
               return(OUTOFMEMORY);
            }
            TextBlock=HandleLock(TextBoxGetTextHandle(MidPBox));
            if (TextBlock==NULL)
            {
               HandleFree(TextBoxGetTextHandle(MidPBox));
               TextBoxSetBlockLength(MidPBox,0);
               TextBoxSetTextLength(MidPBox,0);
               HandleUnlock(ItemGetHandle(MidHBox));
               return(OUTOFMEMORY);
            }
            if (fread(TextBlock,sizeof(unsigned short)*
                     TextBoxGetTextLength(MidPBox),1,fp)<1)
            {
               HandleUnlock(TextBoxGetTextHandle(MidPBox));
               HandleFree(TextBoxGetTextHandle(MidPBox));
               TextBoxSetBlockLength(MidPBox,0);
               TextBoxSetTextLength(MidPBox,0);
               HandleUnlock(ItemGetHandle(MidHBox));
               return(OUTOFMEMORY);
            }

            i=0;
            while (i<FileBoxTable.TextLength)
            {
              InsertHBox=EditBufferSearchNextAttribute(TextBlock,i,
                         FileBoxTable.TextLength,INSERTBOX,&i);
              if (InsertHBox>0)
              {
                 if ((Result=FileLoadBox(fp,FileBoxLinkArray,BoxLinkIndex,
                      MidHPage,SeekSign))<=0)
                 {
                    TextBlock[i]=' ';
                    HandleUnlock(TextBoxGetTextHandle(MidPBox));
                    HandleFree(TextBoxGetTextHandle(MidPBox));
                    HandleUnlock(ItemGetHandle(MidHBox));
                    return(Result);
                 }
                 else
                    TextBlock[i]=MakeINSERTBOX(Result);
                 i++;
              }
              else
                 break;
            }
            //TextBlock[TextBoxGetTextLength(MidPBox)-1]=
            //TextBlock[TextBoxGetTextLength(MidPBox)]=0;
            HandleUnlock(TextBoxGetTextHandle(MidPBox));
            if (SeekSign)
               HandleFree(TextBoxGetTextHandle(MidPBox));
         }
         if (!SeekSign)
            HandleUnlock(ItemGetHandle(MidHBox));
         break;
    case TABLEBOX:
         TextBoxInitialLineTable((PTextBoxs)&MidBox.TextBox);
         if (!SeekSign)
         {
            CELL *pCell;

            MidHBox=TableBoxInsert(&MidBox.FormBox,MidHPage);
            if (!MidHBox)
               return(OUTOFMEMORY);
            FileBoxLinkArray[*BoxLinkIndex].HIndexTextBox=
                            FileBoxTable.BoxDescribleHandle;
            FileBoxLinkArray[*BoxLinkIndex].HConvertTextBox=MidHBox;
            (*BoxLinkIndex)++;
            MidPBox=HandleLock(ItemGetHandle(MidHBox));
            if (MidPBox==NULL)
               return(OUTOFMEMORY);

                  /* Read and Insert Text */
            TableBoxSethCellTable((FormBoxs *)MidPBox,
              HandleAlloc(sizeof(CELL)*TableBoxGetnumLines((PFormBoxs)MidPBox)
                  *TableBoxGetnumCols((PFormBoxs)MidPBox),0));
            if (!TableBoxGethCellTable((PFormBoxs)MidPBox))
            {
               HandleUnlock(ItemGetHandle(MidHBox));
               return(OUTOFMEMORY);
            }

            pCell=HandleLock(TableBoxGethCellTable((PFormBoxs)MidPBox));
            if (fread(pCell,sizeof(CELL)*
              TableBoxGetnumLines((PFormBoxs)MidPBox)*
              TableBoxGetnumCols((PFormBoxs)MidPBox),1,fp)<1)
            {
               HandleUnlock(TableBoxGethCellTable((PFormBoxs)MidPBox));
               HandleUnlock(ItemGetHandle(MidHBox));
               return(OUTOFMEMORY);
            }
            HandleUnlock(TableBoxGethCellTable((PFormBoxs)MidPBox));
         }
         else
         {
            MidPBox=&MidBox.TextBox;
            fseek(fp,sizeof(CELL)*TableBoxGetnumLines((PFormBoxs)MidPBox)
                        *TableBoxGetnumCols((PFormBoxs)MidPBox),SEEK_CUR);
         }

         if( TableBoxGetBlockLength((PFormBoxs)MidPBox)!=0
         && !TableBoxGetPrevLinkBox((PFormBoxs)MidPBox) )
         {
            if (!SeekSign)
            {
               TableBoxSetTextHandle((PFormBoxs)MidPBox,
                     HandleAlloc(sizeof(Wchar)*
                            TableBoxGetBlockLength((PFormBoxs)MidPBox),0));
               if (TableBoxGetTextHandle((PFormBoxs)MidPBox)==0)
               {
                  TableBoxSetBlockLength((PFormBoxs)MidPBox,0);
                  TableBoxSetTextLength((PFormBoxs)MidPBox,0);
                  HandleUnlock(ItemGetHandle(MidHBox));
                  return(OUTOFMEMORY);
               }
               TextBlock=HandleLock(TableBoxGetTextHandle((PFormBoxs)MidPBox));
               if (TextBlock==NULL)
               {
                  HandleFree(TableBoxGetTextHandle((PFormBoxs)MidPBox));
                  TableBoxSetBlockLength((PFormBoxs)MidPBox,0);
                  TableBoxSetTextLength((PFormBoxs)MidPBox,0);
                  HandleUnlock(ItemGetHandle(MidHBox));
                  return(OUTOFMEMORY);
               }

               if (fread(TextBlock,sizeof(Wchar)*
                        TableBoxGetTextLength((PFormBoxs)MidPBox),1,fp)<1)
               {
                  HandleUnlock(TableBoxGetTextHandle((PFormBoxs)MidPBox));
                  HandleFree(TableBoxGetTextHandle((PFormBoxs)MidPBox));
                  TableBoxSetBlockLength((PFormBoxs)MidPBox,0);
                  TableBoxSetTextLength((PFormBoxs)MidPBox,0);
                  HandleUnlock(ItemGetHandle(MidHBox));
                  return(OUTOFMEMORY);
               }
               //TextBlock[TableBoxGetTextLength((FormBoxs *)MidPBox)]=
               //TextBlock[TableBoxGetTextLength((FormBoxs *)MidPBox)-1]=0;
               HandleUnlock(TableBoxGetTextHandle((FormBoxs *)MidPBox));
            }
            else
               fseek(fp,sizeof(Wchar)*
                     TableBoxGetTextLength((PFormBoxs)MidPBox),SEEK_CUR);
         }

         if (!SeekSign)
              HandleUnlock(ItemGetHandle(MidHBox));
         break;
    case LINEBOX:
         if (!SeekSign)
         {
            MidHBox=LineBoxInsert(&MidBox.LineBox,MidHPage);
            if (!MidHBox)
               return(OUTOFMEMORY);
         }
         break;
    case RECTANGLEPICTUREBOX:
    case CORNERPICTUREBOX:
    case ELIPSEPICTUREBOX:
         if (!SeekSign)
         {
            MidHBox=PictureBoxInsert(&MidBox.PictureBox,MidHPage);
            if (MidHBox<=0)
               return(OUTOFMEMORY);
            PictureBox=HandleLock(ItemGetHandle(MidHBox));
            if (PictureBox==NULL)
               return(OUTOFMEMORY);
         }
         else
            PictureBox=&MidBox.PictureBox;
                             /* Get picture present */
         TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
         if (PictureBoxGetPictureFileName(PictureBox)[0])
         {
            if (!SeekSign)
            {
               TiffPresent->ImageNewHandle=0;
               TiffPresent->ImageHandle=HandleAlloc(FileBoxTable.TextLength,0);
               ImageReadData=HandleLock(TiffPresent->ImageHandle);
               if ((TiffPresent->ImageHandle==0)||(ImageReadData==NULL))
               {
                  HandleUnlock(ItemGetHandle(MidHBox));
                  return(OUTOFMEMORY);
               }
               if (fread(ImageReadData,FileBoxTable.TextLength,1,fp)<1)
               {
                  HandleUnlock(TiffPresent->ImageHandle);
                  HandleUnlock(ItemGetHandle(MidHBox));
                  return(FILEREAD);
               }
               ImageGetNewHandle(TiffPresent);
               HandleUnlock(TiffPresent->ImageHandle);
            }
            else
               fseek(fp,FileBoxTable.TextLength,SEEK_CUR);
         }

         if (!SeekSign)
              HandleUnlock(ItemGetHandle(MidHBox));
         break;
    case POLYGONPICTUREBOX:
         {
           int *PolygonBorderPoints;

           if (!SeekSign)
           {
              MidHBox=PictureBoxInsert(&MidBox.PictureBox,MidHPage);
              if (MidHBox<=0)
                 return(OUTOFMEMORY);
              PictureBox=HandleLock(ItemGetHandle(MidHBox));
              if (PictureBox==NULL)
                 return(OUTOFMEMORY);
           }
           else
              PictureBox=&MidBox.PictureBox;
                               /* Get Polygon points */
           if (!SeekSign)
           {
              PictureBoxSetBorderPolygon(PictureBox,HandleAlloc(
                  sizeof(int)+2*sizeof(ORDINATETYPE)*MAXPOLYGONNUMBER,0));
              PolygonBorderPoints=HandleLock(PictureBoxGetBorderPolygon(PictureBox));
              if (PolygonBorderPoints==NULL)
              {
                 HandleUnlock(ItemGetHandle(MidHBox));
                 return(OUTOFMEMORY);
              }
              memcpy(PolygonBorderPoints,
                     &(((unsigned char*)&MidBox)[sizeof(*PictureBox)]),
                     FileBoxTable.BoxParameterLength-sizeof(*PictureBox)
                    );
              HandleUnlock(PictureBoxGetBorderPolygon(PictureBox));
           }
                               /* Get picture present */
           TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
           if (PictureBoxGetPictureFileName(PictureBox)[0])
           {
              if (!SeekSign)
              {
                 TiffPresent->ImageNewHandle=0;
                 TiffPresent->ImageHandle=HandleAlloc(FileBoxTable.TextLength,0);
                 ImageReadData=HandleLock(TiffPresent->ImageHandle);
                 if ((TiffPresent->ImageHandle==0)||(ImageReadData==NULL))
                 {
                    HandleUnlock(ItemGetHandle(MidHBox));
                    return(OUTOFMEMORY);
                 }
                 if (fread(ImageReadData,FileBoxTable.TextLength,1,fp)<1)
                 {
                    HandleUnlock(TiffPresent->ImageHandle);
                    HandleUnlock(ItemGetHandle(MidHBox));
                    return(FILEREAD);
                 }
                 ImageGetNewHandle(TiffPresent);
                 HandleUnlock(TiffPresent->ImageHandle);
              }
              else
                 fseek(fp,FileBoxTable.TextLength,SEEK_CUR);
           }

           if (!SeekSign)
                HandleUnlock(ItemGetHandle(MidHBox));
         }
    default:
         break;
  }

  if (!MidHBox&&!SeekSign&&MidBox.TextBox.BoxType<=POLYGONPICTUREBOX)
     return(FILEREAD);

  if (TextBoxIsEmbodyBox((TextBoxs *)&MidBox))
  {
     ItemSetNext(ItemGetPrev(MidHBox),0);
     ItemSetPrev(MidHBox,0);
     ItemSetNext(MidHBox,0);
     ItemSetFather(MidHBox,0);
     if( !SeekSign
     && (MidBox.TextBox.BoxType==TEXTBOX||MidBox.TextBox.BoxType==TABLEBOX) )
        FormatAll(MidHBox);        //FormatText(MidHBox,0);
  }

  if (SeekSign)
     return(1);
  else
     return(MidHBox);
#undef MidBox
}

int FileLoad(char *FileName,HPAGE *FirstPage,HBOX *FirstBox)
{
  FileHeads FileHead;
  FilePageTables FilePageTable;
 // FileBoxTables FileBoxTable;
  FILE *fp;
  HITEM HPage,MidHPage,MidHBox;
  Pages MidPage;
//  Boxs MidBox;                // ByHance
#define MidBox  TmpBuf
  TextBoxs *MidPBox;
  int i,j,BoxLinkIndex=0,Result;
  HANDLE HTextBoxLink;
  FileBoxLinks *FileBoxLinkArray;

  strupr(FileName);                     //By zjh For module file 96.9.12
  Result=strlen(FileName)-4;
  if (strncmp(FileName+Result,".FRA",4)) IsModule=0;
    else IsModule=1;

  if ((fp=fopen(FileName,"rb"))==NULL)
     return(FILEOPEN);

  if (fread(&FileHead,sizeof(FileHead),1,fp)!=1)
  {
  lbl_err_format:
     fclose(fp);
     return(FILEFORMAT);
  }

  if (FileHaveExternBlock(FileHead.attr))
  {
      if (ReadExternBlock(fp)<1)
      {
         fclose(fp);
         return (FILEREAD);
      }
  }
  else
      SetDefaultExternBlock();

  if ( strcmp(FileHead.FileSign,FILEHEADSTRING) )
     goto lbl_err_format;

  if(FileHead.Version>FILEVERSION)
  {
     fclose(fp);
     return(INVALIDVERSION);
  }

   /* now, fileversion is 1.0 or 2.0 */
  HTextBoxLink=HandleAlloc(1000*sizeof(FileBoxLinks),0);
  if (!HTextBoxLink)
  {
     fclose(fp);
     return(OUTOFMEMORY);
  }

  FileBoxLinkArray=(FileBoxLinks *)HandleLock(HTextBoxLink);
  if (FileBoxLinkArray==NULL)
  {
     HandleFree(HTextBoxLink);
     fclose(fp);
     return(OUTOFMEMORY);
  }

  strcpy(PageHeadLeftStr,FileHead.PageHeadLeftStr);     // ByHance, 96,3.8
  strcpy(PageHeadRightStr,FileHead.PageHeadRightStr);     // ByHance, 96,3.8
  PgFtStartNum=FileHead.StartPageNum;
  Result=FileHead.PageFootType;
  SetPageFootOption(Result&1);
  Result>>=1;
  SetPageFootTopOption(Result&1);
  Result>>=1;
  SetPageFootLeftOption(Result&3);
  Result>>=2;
  SetPageFootPrevOption(Result&3);

  Result=FileHead.PageHeadType;
  SetPageHeadOption(Result&1);
  Result>>=2;
  SetPageHeadLeftOption(Result&3);
  Result>>=2;
  SetPageHeadLineOption(Result&3);

  for (i=0;i<FileHead.TotalFilePages;i++)
  {                                    /* Read page by page */
      if (fread(&FilePageTable,sizeof(FilePageTable),1,fp) != 1
      || fread(&MidPage,sizeof(Pages),1,fp) != 1 )
      {
         HandleUnlock(HTextBoxLink);
         HandleFree(HTextBoxLink);
         fclose(fp);
         return(FILEREAD);
      }

                       /* for read next version's file */
      if (FileHead.PageParameterLength-sizeof(Pages)>0)
         fseek(fp,FileHead.PageParameterLength-sizeof(Pages),SEEK_CUR);

      j=MidPage.VirtualPage;
      MidPage.VirtualPage|=0x80;       /* For no initial box in page */
      MidHPage=PageNew(&MidPage,i);
      MidPage.VirtualPage=j;
      if (!MidHPage)
      {
         HandleUnlock(HTextBoxLink);
         HandleFree(HTextBoxLink);
         fclose(fp);
         return(OUTOFMEMORY);
      }

      if (i==0)                 // First page,  save InitBoxOption in it
      {
         Pages *MidSavePage;

         *FirstPage=MidHPage;
         MidSavePage=HandleLock(ItemGetHandle(MidHPage));
         if (MidSavePage==NULL)
         {
            HandleUnlock(HTextBoxLink);
            HandleFree(HTextBoxLink);
            fclose(fp);
            return(OUTOFMEMORY);
         }
         MidSavePage->VirtualPage=j;
         HandleUnlock(ItemGetHandle(MidHPage));
      }

      for (j=0;j<FilePageTable.TotalPageBox;j++)
      {                                /* Read box by box */
          if ((Result=FileLoadBox(fp,FileBoxLinkArray,&BoxLinkIndex,
               MidHPage,0))<OpOK)
          {
             HandleUnlock(HTextBoxLink);
             HandleFree(HTextBoxLink);
             fclose(fp);
             return(Result);
          }

          if (i==0&&j==0)  // First box, we will do with it after read them
             *FirstBox=Result;
      } /*---- j::EachBox -----*/
  } /*----- i::EachPage ----*/
  fclose(fp);

  for (i=0;i<BoxLinkIndex;i++)         /* Deal with link */
  {
      MidPBox=HandleLock(ItemGetHandle(FileBoxLinkArray[i].HConvertTextBox));
      if (!MidPBox)
      {
         HandleUnlock(HTextBoxLink);
         HandleFree(HTextBoxLink);
         return(OUTOFMEMORY);
      }

      if (TextBoxGetPrevLinkBox(MidPBox))
      {
         for (j=0;j<BoxLinkIndex;j++)
             if (TextBoxGetPrevLinkBox(MidPBox)==FileBoxLinkArray[j].HIndexTextBox)
                break;
         if (j<BoxLinkIndex)
            TextBoxSetPrevLinkBox(MidPBox,FileBoxLinkArray[j].HConvertTextBox);
         else
            TextBoxSetPrevLinkBox(MidPBox,0);
      }

      if (TextBoxGetNextLinkBox(MidPBox))
      {
         for (j=0;j<BoxLinkIndex;j++)
             if (TextBoxGetNextLinkBox(MidPBox)==FileBoxLinkArray[j].HIndexTextBox)
                break;
         if (j<BoxLinkIndex)
            TextBoxSetNextLinkBox(MidPBox,FileBoxLinkArray[j].HConvertTextBox);
         else
            TextBoxSetNextLinkBox(MidPBox,0);
      }
      HandleUnlock(ItemGetHandle(FileBoxLinkArray[i].HConvertTextBox));
  }

  HandleUnlock(HTextBoxLink);
  HandleFree(HTextBoxLink);

 #ifdef OLD_VERSION
 /*----- set all LinkedBox with same text handle ----*/
  HPage=*FirstPage;
  while (HPage)
  {
    MidHBox=ItemGetChild(HPage);        // get first box in this page
    while (MidHBox)
    {
      MidPBox=HandleLock(ItemGetHandle(MidHBox));
      if (MidPBox==NULL)
         return(OUTOFMEMORY);

      if (TextBoxGetBoxType(MidPBox)==TEXTBOX)
      {
         InitRL(MidPBox);
         if(!TextBoxGetPrevLinkBox(MidPBox) && TextBoxGetNextLinkBox(MidPBox))
            SetAllLinkBoxTextHandle(MidHBox);
      }
      HandleUnlock(ItemGetHandle(MidHBox));
      MidHBox=BoxGetNext(MidHBox);
    }
    HPage=ItemGetNext(HPage);
  }

  HPage=*FirstPage;
  while (HPage)
  {
    MidHBox=ItemGetChild(HPage);
    while (MidHBox)
    {
      MidPBox=HandleLock(ItemGetHandle(MidHBox));
      if (MidPBox==NULL)
         return(OUTOFMEMORY);

      if (TextBoxGetBoxType(MidPBox)==TEXTBOX
      || TextBoxGetBoxType(MidPBox)==TABLEBOX)
      {
         if (!TextBoxGetPrevLinkBox(MidPBox))
            FormatAll(MidHBox);        //FormatText(MidHBox,0);
      }

      HandleUnlock(ItemGetHandle(MidHBox));
      MidHBox=BoxGetNext(MidHBox);
    }
    HPage=ItemGetNext(HPage);
  }
#else           // ByHance, 96,4.3
 /*----- set all LinkedBox with same text handle, --
  ---  and format Box which has not prev_link_box ----*/
  HPage=*FirstPage;
  while (HPage)
  {
    MidHBox=ItemGetChild(HPage);
    while (MidHBox)
    {
      MidPBox=HandleLock(ItemGetHandle(MidHBox));
      if (MidPBox==NULL)
         return(OUTOFMEMORY);

      if (TextBoxGetBoxType(MidPBox)==TEXTBOX)
      {
         // InitRL(MidPBox);    // TextBoxInitialLineTable has done it !!
         if(!TextBoxGetPrevLinkBox(MidPBox) && TextBoxGetNextLinkBox(MidPBox))
            SetAllLinkBoxTextHandle(MidHBox);
      }

      if (TextBoxGetBoxType(MidPBox)==TEXTBOX
      || TextBoxGetBoxType(MidPBox)==TABLEBOX)
      {
         if (!TextBoxGetPrevLinkBox(MidPBox))
            FormatAll(MidHBox);        //FormatText(MidHBox,0);
      }
      HandleUnlock(ItemGetHandle(MidHBox));
      MidHBox=BoxGetNext(MidHBox);      // try next box
    }
    HPage=ItemGetNext(HPage);   // try next page
  }
#endif  // OLD_VERSION

  SaveFileToBackup(FileName);

  ReturnOK();
#undef MidBox
}

#ifdef UNUSED           //ByHance, 96,1.29
int GeneralFileDelete(char *FileName,long DeleteStart,long DeleteSize)
{
  #define TMPLIBRARYNAME "TMPLIB$$.$$$"
  FILE *fp,*fptmp;
  int TmpChar;
  long i;

  if ((fp=fopen(FileName,"rb"))==NULL)
     return(FILEOPEN);
  if ((fptmp=fopen(TMPLIBRARYNAME,"wb"))==NULL)
  {
     fclose(fp);
     return(FILEOPEN);
  }
  for (i=0;i<DeleteStart;i++)
  {
      TmpChar=fgetc(fp);
      if (TmpChar==EOF)
      {
         fclose(fptmp);
         fclose(fp);
         return(FILEREAD);
      }
      if (fputc(TmpChar,fptmp)==EOF)
      {
         fclose(fptmp);
         fclose(fp);
         return(FILEWRITE);
      }
  }
  fseek(fp,DeleteSize,SEEK_SET);
  while ((TmpChar=fgetc(fp))!=EOF)
  {
    if (fputc(TmpChar,fptmp)==EOF)
    {
       fclose(fptmp);
       fclose(fp);
       return(FILEWRITE);
    }
  }

  fclose(fptmp);
  fclose(fp);
  ReturnOK();
}

int LibraryDelete(char *FileName,int LibNumber)
{
  LibraryHeads LibraryHead;
  LibraryPageTables LibraryPageTable;
  LibraryBoxTables LibraryBoxTable;
  FILE *fp;
  Pages MidPage;
//  Boxs MidBox;                // ByHance
#define MidBox  TmpBuf
  int i,j,Result,MidHPage=1;
  int *LibraryBoxLinkArray,BoxLinkIndex;
  long DeleteStart=-1,DeleteSize;

  if ((fp=fopen(FileName,"r+b"))==NULL)
     return(FILEOPEN);

  if (fread(&LibraryHead,sizeof(LibraryHead),1,fp)<1)
  {
     fclose(fp);
     return(FILEREAD);
  }

  if (strcmp(LibraryHead.LibrarySign,LIBRARYHEADSTRING)||LibraryHead.Version!=FILEVERSION)
  {
     fclose(fp);
     return(FILEFORMAT);
  }

  for (i=0;i<LibraryHead.TotalLibraryPages;i++)
  {                                    // Read page by page
      if (fread(&LibraryPageTable,sizeof(LibraryPageTable),1,fp)<1)
      {
         fclose(fp);
         return(FILEREAD);
      }

      if (fread(&MidPage,sizeof(Pages),1,fp)<1)
      {
         fclose(fp);
         return(FILEREAD);
      }
                                       // for read next version's file
      if (LibraryHead.PageParameterLength-sizeof(Pages)>0)
         fseek(fp,LibraryHead.PageParameterLength-sizeof(Pages),SEEK_CUR);

      if (LibraryPageTable.LibraryPageNumber==LibNumber)
      {
         DeleteStart=ftell(fp);
         DeleteStart-=LibraryHead.PageParameterLength+sizeof(LibraryPageTable);
      }
      else
         if (DeleteStart>=0)
         {
            DeleteSize=ftell(fp);
            DeleteSize-=LibraryHead.PageParameterLength+sizeof(LibraryPageTable);
            DeleteSize=DeleteSize-DeleteStart;
            break;
         }

      for (j=0;j<LibraryPageTable.TotalPageBox;j++)
      {                                // Read box by box
          if ((Result=FileLoadBox(fp,LibraryBoxLinkArray,&BoxLinkIndex,
               MidHPage,1))<OpOK)
          {
             fclose(fp);
             return(FILEREAD);
          }
      }
  }

  if (DeleteStart>=0)
  {
     fseek(fp,0,SEEK_SET);             // Write back file head
     LibraryHead.TotalLibraryPages--;
     if (fwrite(&LibraryHead,sizeof(LibraryHead),1,fp)<1)
     {
        fclose(fp);
        return(FILEWRITE);
     }
     fclose(fp);
     i=GeneralFileDelete(FileName,DeleteStart,DeleteSize);
  }
  else
     fclose(fp);

  if (DeleteStart>=0&&i>=0)
     ReturnOK();
  else
     return(-1);
#undef MidBox
}

int LibrarySave(char *FileName,int LibNumber)
{
  LibraryHeads LibraryHead;
  LibraryPageTables LibraryPageTable;
  LibraryBoxTables LibraryBoxTable;
  FILE *fp;
  HITEM HPage,HBox;
  long SavePageOffset;
  int i,Result;

  if (!GlobalGroupGetSign())
     ReturnOK();
  if (!access(FileName,0))             // Check existence of file
  {
     LibraryDelete(FileName,LibNumber);
     if ((fp=fopen(FileName,"r+b"))==NULL)
        return(FILEOPEN);
     fseek(fp,0,SEEK_SET);
     if (fread(&LibraryHead,sizeof(LibraryHead),1,fp)<1)
     {
        fclose(fp);
        return(FILEREAD);
     }
     fseek(fp,0,SEEK_END);
  }
  else                                 // Not exist, creat it
  {
     if ((fp=fopen(FileName,"wb"))==NULL)
        return(FILEOPEN);

     memset(&LibraryHead,0,sizeof(LibraryHead));
     strcpy(LibraryHead.LibrarySign,LIBRARYHEADSTRING);
     LibraryHead.Version=FILEVERSION;
     LibraryHead.TotalLibraryPages=0;
     LibraryHead.PageParameterLength=sizeof(Pages);

     if (fwrite(&LibraryHead,sizeof(LibraryHead),1,fp)<1)
     {
        fclose(fp);
        return(FILEWRITE);
     }
  }

  HPage=GlobalGroupGetPage();
  if (HPage)
  {
    long CurrentLibraryPosition;
    Pages *MidPage;

    memset(&LibraryPageTable,0,sizeof(LibraryPageTable));
    LibraryPageTable.TotalPageBox=0;
    SavePageOffset=ftell(fp);
    if (fwrite(&LibraryPageTable,sizeof(LibraryPageTable),1,fp)<1)
    {
       fclose(fp);
       return(FILEWRITE);
    }

    MidPage=HandleLock(ItemGetHandle(HPage));
    if (MidPage==NULL)
    {
       HandleUnlock(ItemGetHandle(HPage));
       fclose(fp);
       return(OUTOFMEMORY);
    }
    if (fwrite(MidPage,sizeof(Pages),1,fp)<1)
    {
       HandleUnlock(ItemGetHandle(HPage));
       fclose(fp);
       return(FILEWRITE);
    }
    else
       HandleUnlock(ItemGetHandle(HPage));

    i=0;
    while (i<GlobalGroupGetSumBox())   // Form the first box to the last one
    {
          TextBoxs *HBoxSetting;
          Wchar *HText;

          HBox=GlobalGroupGetBox(i);
          if ((Result=FileSaveBox(fp,HBox))<OpOK)
          {
             fclose(fp);
             return(Result);
          }
          else
          {
             i++;
             LibraryPageTable.TotalPageBox++;
          }
    } /*----- i -------*/

    CurrentLibraryPosition=ftell(fp);
    fseek(fp,SavePageOffset,SEEK_SET); // Write back page head
    LibraryPageTable.LibraryPageNumber=LibNumber;
    if (fwrite(&LibraryPageTable,sizeof(LibraryPageTable),1,fp)<1)
    {
       fclose(fp);
       return(FILEWRITE);
    }
    fseek(fp,CurrentLibraryPosition,SEEK_SET);

    LibraryHead.TotalLibraryPages++;
  }

  fseek(fp,0,SEEK_SET);                // Write back file head
  if (fwrite(&LibraryHead,sizeof(LibraryHead),1,fp)<1)
  {
     fclose(fp);
     return(FILEWRITE);
  }

  fclose(fp);
  ReturnOK();
}

int LibraryLoad(char *LibraryName,int LibNumber,HPAGE *FirstPage,
                  HBOX *FirstBox)
{
  LibraryHeads LibraryHead;
  LibraryPageTables LibraryPageTable;
  FILE *fp;
  HITEM HPage,HBox,MidHPage,MidHBox;
  Pages MidPage;
//  Boxs MidBox;                // ByHance
#define MidBox  TmpBuf
  TextBoxs *MidPBox;
  int i,j,BoxLinkIndex=0,Result,TmpFirstHBox;
  HANDLE HTextBoxLink;
  FileBoxLinks *LibraryBoxLinkArray;

  if ((fp=fopen(LibraryName,"rb"))==NULL)
     return(FILEOPEN);

  if (fread(&LibraryHead,sizeof(LibraryHead),1,fp)<1)
  {
     fclose(fp);
     return(FILEREAD);
  }

  if (strcmp(LibraryHead.LibrarySign,LIBRARYHEADSTRING)||LibraryHead.Version!=FILEVERSION)
  {
     fclose(fp);
     return(FILEFORMAT);
  }

  HTextBoxLink=HandleAlloc(1000*sizeof(FileBoxLinks),0);
  if (!HTextBoxLink)
  {
     fclose(fp);
     return(OUTOFMEMORY);
  }

  LibraryBoxLinkArray=(FileBoxLinks *)HandleLock(HTextBoxLink);
  if (LibraryBoxLinkArray==NULL)
  {
     HandleFree(HTextBoxLink);
     fclose(fp);
     return(OUTOFMEMORY);
  }

  for (i=0;i<LibraryHead.TotalLibraryPages;i++)
  {                                    // Read page by page
      if (fread(&LibraryPageTable,sizeof(LibraryPageTable),1,fp)<1)
      {
         HandleUnlock(HTextBoxLink);
         HandleFree(HTextBoxLink);
         fclose(fp);
         return(FILEREAD);
      }

      if (fread(&MidPage,sizeof(Pages),1,fp)<1)
      {
         HandleUnlock(HTextBoxLink);
         HandleFree(HTextBoxLink);
         fclose(fp);
         return(FILEREAD);
      }
                                       // for read next version's file
      if (LibraryHead.PageParameterLength-sizeof(Pages)>0)
         fseek(fp,LibraryHead.PageParameterLength-sizeof(Pages),SEEK_CUR);

      if (LibraryPageTable.LibraryPageNumber==LibNumber)
      {
         GlobalUnGroup();

         if (*FirstPage<=0)
         {
            j=MidPage.VirtualPage;
            MidPage.VirtualPage|=0x80; // For no initial box in page
            MidHPage=PageNew(&MidPage,i);
            MidPage.VirtualPage=j;
            if (!MidHPage)
            {
               HandleUnlock(HTextBoxLink);
               HandleFree(HTextBoxLink);
               fclose(fp);
               return(OUTOFMEMORY);
            }

            *FirstPage=HPage=MidHPage;
         }
         else
            MidHPage=*FirstPage;
      }
      else
         MidHPage=0;

      for (j=0;j<LibraryPageTable.TotalPageBox;j++)
      {                                // Read box by box
          if ((Result=FileLoadBox(fp,LibraryBoxLinkArray,&BoxLinkIndex,
               MidHPage,(LibraryPageTable.LibraryPageNumber!=LibNumber)))
               <OpOK)
          {
             HandleUnlock(HTextBoxLink);
             HandleFree(HTextBoxLink);
             fclose(fp);
             return(FILEREAD);
          }

          if (LibraryPageTable.LibraryPageNumber==LibNumber)
          {
             GlobalGroupSetBox(j,Result);
             GlobalGroupSetSumBox(j+1);
             if (j==0)
                TmpFirstHBox=Result;
          }
      } /*---- j -------*/
  }

  fclose(fp);

  for (i=0;i<BoxLinkIndex;i++)         // Deal with link
  {
      MidPBox=HandleLock(ItemGetHandle(LibraryBoxLinkArray[i].HConvertTextBox));
      if (!MidPBox)
      {
         HandleUnlock(HTextBoxLink);
         HandleFree(HTextBoxLink);
         return(OUTOFMEMORY);
      }
      if (TextBoxGetPrevLinkBox(MidPBox))
      {
         for (j=0;j<BoxLinkIndex;j++)
             if (TextBoxGetPrevLinkBox(MidPBox)==LibraryBoxLinkArray[j].HIndexTextBox)
                break;
         if (j<BoxLinkIndex)
            TextBoxSetPrevLinkBox(MidPBox,LibraryBoxLinkArray[j].HConvertTextBox);
         else
            TextBoxSetPrevLinkBox(MidPBox,0);
      }
      if (TextBoxGetNextLinkBox(MidPBox))
      {
         for (j=0;j<BoxLinkIndex;j++)
             if (TextBoxGetNextLinkBox(MidPBox)==LibraryBoxLinkArray[j].HIndexTextBox)
                break;
         if (j<BoxLinkIndex)
            TextBoxSetNextLinkBox(MidPBox,LibraryBoxLinkArray[j].HConvertTextBox);
         else
            TextBoxSetNextLinkBox(MidPBox,0);
      }
      HandleUnlock(ItemGetHandle(LibraryBoxLinkArray[i].HConvertTextBox));
  }

  HandleUnlock(HTextBoxLink);
  HandleFree(HTextBoxLink);

  MidHBox=TmpFirstHBox;
  while (MidHBox)
  {
    MidPBox=HandleLock(ItemGetHandle(MidHBox));
    if (MidPBox==NULL)
       return(OUTOFMEMORY);
    if (TextBoxGetBoxType(MidPBox)==TEXTBOX)
    {
       InitRL(MidPBox);
       if ((!TextBoxGetPrevLinkBox(MidPBox))
           &&TextBoxGetNextLinkBox(MidPBox))
          SetAllLinkBoxTextHandle(MidHBox);
    }
    HandleUnlock(ItemGetHandle(MidHBox));
    MidHBox=BoxGetNext(MidHBox);
  }

  if (GlobalBoxTool==IDX_SELECTBOX||GlobalBoxTool==IDX_ROTATE)
  {
     int HBoxDots;
     ORDINATETYPE BoxBorderLeft,BoxBorderTop,BoxBorderRight,BoxBorderBottom;
     ORDINATETYPE GroupLeft=32767,GroupTop=32767,GroupRight=-32767,
                  GroupBottom=-32767;
     ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];

     MidHBox=TmpFirstHBox;
     while (MidHBox)
     {
       MidPBox=HandleLock(ItemGetHandle(MidHBox));
       if (MidPBox==NULL)
          return(OUTOFMEMORY);
       BoxGetPolygonBorder(MidPBox,&HBoxDots,BoxXY);
       PolygonGetMinRectangle(HBoxDots,BoxXY,&BoxBorderLeft,&BoxBorderTop,
                              &BoxBorderRight,&BoxBorderBottom);
       if (GroupLeft>BoxBorderLeft)
          GroupLeft=BoxBorderLeft;
       if (GroupTop>BoxBorderTop)
          GroupTop=BoxBorderTop;
       if (GroupRight<BoxBorderRight)
          GroupRight=BoxBorderRight;
       if (GroupBottom<BoxBorderBottom)
          GroupBottom=BoxBorderBottom;
       if (TextBoxGetBoxType(MidPBox)==TEXTBOX
         ||TextBoxGetBoxType(MidPBox)==TABLEBOX)
       {
          if (!TextBoxGetPrevLinkBox(MidPBox))
             FormatAll(MidHBox);        //FormatText(MidHBox,0);
       }
       HandleUnlock(ItemGetHandle(MidHBox));
       MidHBox=BoxGetNext(MidHBox);
     }

     GlobalGroupSetLeft(GroupLeft);
     GlobalGroupSetTop(GroupTop);
     GlobalGroupSetWidth(GroupRight-GroupLeft);
     GlobalGroupSetHeight(GroupBottom-GroupTop);
     GlobalGroupSetRotateAngle(0);
     GlobalGroupSetRotateAxisX(0);
     GlobalGroupSetRotateAxisY(0);
     GlobalGroupSetSign();
     GlobalGroupSetPage(*FirstPage);
  }
  else
  {
     GlobalGroupSetSumBox(0);
     *FirstBox=TmpFirstHBox;
  }

  FileSetLoaded();
  FileSetModified();
  FileSetNotSaved();
  ReturnOK();
#undef MidBox
}
#endif     // UNUSED           //ByHance, 96,1.29

static int TextBlockExportText(FILE *fp,Wchar *Source,int Length)
{
  int i,j, col, autoCR;
  Wchar WriteChar,*TmpP;

  TmpP=Source;
  for (i=j=col=0;i<Length;i++)
  {
      WriteChar=*TmpP++;
      autoCR=0;
      if(col>=78)       // must LF now
      {                         // use Chinese_LF(0x8d,0x8a)
         if (fputc(0xd,fp)==EOF)
            return(FILEWRITE);
         if (fputc(0xa,fp)==EOF)
            return(FILEWRITE);
         col=0;
         autoCR=1;
      }

      if (GetPreCode(WriteChar)==0)    // English char
      {
         if(WriteChar==0xd && autoCR)
         {
             j++;  continue;
         }

         if (fputc(WriteChar,fp)==EOF && (int)WriteChar!=EOF)
            return(FILEWRITE);
         j++;  col++;

         if(WriteChar==0xd)
         {
            if (fputc(0xa,fp)==EOF)
               return(FILEWRITE);
            j++; col=0;
         }
      }
      else
      {
         if (GetPreCode(WriteChar)>=GetPreCode(0xa000))
         {                             // Chineses char
            if (fputc(((WriteChar&0xff00)>>8),fp)==EOF)
               return(FILEWRITE);
            j++;
            if (fputc((WriteChar&0xff),fp)==EOF)
               return(FILEWRITE);
            j++;
            col+=2;
         }
      }
  }
  return(j);
}

static int BoxExportText(FILE *fp,HTEXTBOX HBox)
{
  TextBoxs *pBox;
  Wchar *HText;

  pBox=HandleLock(ItemGetHandle(HBox));
  if (pBox==NULL)
     return(OUTOFMEMORY);

  switch (TextBoxGetBoxType(pBox))
  {
    case TEXTBOX:
    /*-------------- ????? ByHance ----------------*/
         // if (TextBoxGetTextLength(pBox)&&!TextBoxGetPrevLinkBox(pBox))
         if (TextBoxGetTextLength(pBox)>1&&!TextBoxGetPrevLinkBox(pBox))
         {
            int Result,i;
            HBOX InsertHBox;

            HText=HandleLock(TextBoxGetTextHandle(pBox));
            if (HText==NULL)
            {
               HandleUnlock(TextBoxGetTextHandle(pBox));
               HandleUnlock(ItemGetHandle(HBox));
               return(FILEWRITE);
            }
            if (TextBlockExportText(fp,HText,TextBoxGetTextLength(pBox))
                <0)
            {
               HandleUnlock(TextBoxGetTextHandle(pBox));
               HandleUnlock(ItemGetHandle(HBox));
               return(FILEWRITE);
            }
            i=0;
    /*-------------- ????? ByHance ----------------*/
            while (i<TextBoxGetTextLength(pBox))
            {
              InsertHBox=EditBufferSearchNextAttribute(HText,i,
                         TextBoxGetTextLength(pBox),INSERTBOX,&i);
              if (InsertHBox>0)
              {
                 if ((Result=BoxExportText(fp,InsertHBox))<OpOK)
                 {
                    HandleUnlock(TextBoxGetTextHandle(pBox));
                    return(Result);
                 }
                 i++;
              }
              else
                 break;
            }
            HandleUnlock(TextBoxGetTextHandle(pBox));
         }
         break;
    case TABLEBOX:
         {
            FormBoxs *FormBox;

            FormBox=(FormBoxs *)pBox;
    /*-------------- ????? ByHance ----------------*/
            if (TextBoxGetTextLength(pBox)&&!TableBoxGetPrevLinkBox(FormBox))
            {
               HText=HandleLock(TableBoxGetTextHandle(FormBox));
               if (HText==NULL)
               {
                  HandleUnlock(TextBoxGetTextHandle(pBox));
                  HandleUnlock(ItemGetHandle(HBox));
                  return(FILEWRITE);
               }
               if (TextBlockExportText(fp,HText,
                   TextBoxGetTextLength(pBox))<0)
               {
                  HandleUnlock(TextBoxGetTextHandle(pBox));
                  HandleUnlock(ItemGetHandle(HBox));
                  return(FILEWRITE);
               }
               HandleUnlock(TableBoxGetTextHandle(FormBox));
            }
         }
         break;
    default:
         break;
  }
  HandleUnlock(ItemGetHandle(HBox));
  ReturnOK();
}

int FileExportText(char *FileName)
{
  FILE *fp;
  HITEM HPage,HBox;
  int Result;

  if ((fp=fopen(FileName,"wb"))==NULL)
     return(FILEOPEN);

  HPage=ItemGetChild(GlobalPageHeadHandle);
  while (HPage)                        /* Form the first page to the last one */
  {
    HBox=PageGetBoxHead(HPage);
    while (HBox)                       /* Form the first box to the last one */
    {
      if ((Result=BoxExportText(fp,HBox))<0)
      {
         HandleUnlock(ItemGetHandle(HPage));
         fclose(fp);
         return(Result);
      }
      HBox=ItemGetNext(HBox);
    }
    HPage=ItemGetNext(HPage);
  }
  fclose(fp);
  ReturnOK();
}

int FileExportStory(char *FileName,HTEXTBOX HBox)
{
  FILE *fp;
  int Result;

  if ((fp=fopen(FileName,"wb"))==NULL)
     return(FILEOPEN);

  Result=BoxExportText(fp,GetFirstLinkBox(HBox));

  fclose(fp);
  ReturnOK();
}

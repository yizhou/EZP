/*-------------------------------------------------------------------
* Name: clpboard.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static HANDLE GlobalClipBoardHandle=0;
static int GlobalClipBoardDataLength=0,GlobalClipBoardReadLength=0;

#ifdef UNUSED
int ClipBoardGetTotalLength(int DataLength)
{
  return(DataLength+sizeof(ClipBoards));
}

int ClipBoardDelete(void)
{
  if (GlobalClipBoardHandle)
  {
     HandleFree(GlobalClipBoardHandle);
     GlobalClipBoardHandle=0;
     GlobalClipBoardReadLength=0;
  }
  ReturnOK();
}
#endif

void ClipBoardInsert(int DataLength)
{
  if (GlobalClipBoardHandle)
     HandleFree(GlobalClipBoardHandle);
  GlobalClipBoardDataLength=0;
  GlobalClipBoardHandle=HandleAlloc(DataLength,0);
}

int ClipBoardAppend(void *AppendData,int DataLength,char DataType)
{
  char *ClipBoardData;

  ClipBoardData=HandleLock(GlobalClipBoardHandle);
  if (ClipBoardData==NULL)
     return(OUTOFMEMORY);

  ((ClipBoards *)&ClipBoardData[GlobalClipBoardDataLength])->ClipBoardDataType=
                  DataType;
  ((ClipBoards *)&ClipBoardData[GlobalClipBoardDataLength])->ClipBoardDataLength=
                  DataLength;
  memcpy(&ClipBoardData[GlobalClipBoardDataLength+sizeof(ClipBoards)],
         AppendData,DataLength);
  GlobalClipBoardDataLength+=DataLength+sizeof(ClipBoards);
  HandleUnlock(GlobalClipBoardHandle);
  ReturnOK();
}

static void ClipBoardGet(void *GetClipboardData,int *DataLength,char *DataType)
{
  char *ClipBoardData;

  ClipBoardData=HandleLock(GlobalClipBoardHandle);
  if (ClipBoardData==NULL)
     return;
  *DataType=((ClipBoards *)&ClipBoardData[GlobalClipBoardReadLength])->ClipBoardDataType;
  *DataLength=((ClipBoards *)&ClipBoardData[GlobalClipBoardReadLength])->ClipBoardDataLength;
  memcpy(GetClipboardData,
         &ClipBoardData[GlobalClipBoardReadLength+sizeof(ClipBoards)],
         *DataLength);
  GlobalClipBoardReadLength+=*DataLength+sizeof(ClipBoards);
  HandleUnlock(GlobalClipBoardHandle);
}

void ClipBoardRead(void)
{
  GlobalClipBoardReadLength=0;
}

static int ClipBoardHasData(void)
{
  return(GlobalClipBoardReadLength<GlobalClipBoardDataLength);
}

static int ClipBoardSizeofBox(HBOX HBox)
{
  TextBoxs *TextBox;
  int Length;
  int *PolygonEdgeNumber;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  if (TextBoxGetBoxType(TextBox)==TEXTBOX)
  {
     Length=sizeof(ClipBoards)+sizeof(TextBoxs);
     if (!TextBoxGetPrevLinkBox(TextBox))
        Length+=sizeof(ClipBoards)+sizeof(Wchar)*TextBoxGetTextLength(TextBox);
  }
  else
  if ((TextBoxGetBoxType(TextBox)>=RECTANGLEPICTUREBOX)&&
      (TextBoxGetBoxType(TextBox)<=POLYGONPICTUREBOX))
  {
     Length=sizeof(ClipBoards)+sizeof(PictureBoxs);
     if (TextBoxGetBoxType(TextBox)==POLYGONPICTUREBOX)
     {
        PolygonEdgeNumber=HandleLock(PictureBoxGetBorderPolygon(TextBox));
        if (PolygonEdgeNumber!=NULL)
        {
           Length+=sizeof(ClipBoards)+sizeof(int)+
                   sizeof(ORDINATETYPE)*2*(*PolygonEdgeNumber);
           HandleUnlock(PictureBoxGetBorderPolygon(TextBox));
        }
     }
     if (PictureBoxGetPictureFileName(TextBox)[0])
        Length+=sizeof(ClipBoards)+sizeof(InsertImages);
  }
  HandleUnlock(ItemGetHandle(HBox));
  return(Length);
}

static int ClipBoardSizeofGroup(HANDLE *Group,int GroupNumber)
{
  int i;
  int Length,TmpLength;

  for (i=Length=0;i<GroupNumber;i++)
  {
      TmpLength=ClipBoardSizeofBox(Group[i]);
      if (TmpLength>0)
         Length+=TmpLength;
      else
         return(OUTOFMEMORY);
  }
  return(Length);
}

int ClipBoardInsertText(HTEXTBOX HBox,int BlockStart,int BlockEnd,char InsertSign)
/*  InsertSign: 0 -- Not first insert, 1 -- First insert  */
{
  Wchar *TextBlock;
  int Result,Length;
  TextBoxs *TextBox;

  Length=BlockEnd-BlockStart;
  if(Length<=0)                         // ByHance
     ReturnOK();

  if (InsertSign)
     ClipBoardInsert(Length*sizeof(Wchar)+sizeof(ClipBoards));

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  if (TextBoxGetTextHandle(TextBox))
  {
     TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
     if (TextBlock==NULL)
     {
        HandleUnlock(ItemGetHandle(HBox));
        return(OUTOFMEMORY);
     }
     Result=ClipBoardAppend(TextBlock+BlockStart,Length*sizeof(Wchar),TEXTDATA);

     HandleUnlock(ItemGetHandle(HBox));
     return(Result);
  }
  else
     ReturnOK();
}

int ClipBoardInsertImage(HPICTUREBOX HBox,char InsertSign)
/*  InsertSign: 0 -- Not first insert, 1 -- First insert  */
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  InsertImages InsertImage;

  PictureBox=HandleLock(ItemGetHandle(HBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  if (!PictureBoxGetPictureFileName(PictureBox)[0])
  {
     HandleUnlock(ItemGetHandle(HBox));
     return(0);
  }

  if (InsertSign)
     ClipBoardInsert(sizeof(ClipBoards)+sizeof(InsertImages));
  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
  strcpy(InsertImage.InsertFileName,PictureBoxGetPictureFileName(PictureBox));
  memcpy(&InsertImage.InsertPresent,TiffPresent,sizeof(*TiffPresent));
  HandleUnlock(ItemGetHandle(HBox));
  return( ClipBoardAppend(&InsertImage,sizeof(InsertImages),IMAGEDATA) );
}

int ClipBoardInsertBox(HBOX HBox,char InsertSign)
/*  InsertSign: 0 -- Not first insert, 1 -- First insert  */
{
  TextBoxs *TextBox;
  int Result;
  int *PolygonEdgeNumber;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (InsertSign)
  {
     Result=ClipBoardSizeofBox(HBox);
     if (Result<=0)
     {
        HandleUnlock(ItemGetHandle(HBox));
        return(Result);
     }
     else
        ClipBoardInsert(Result);
  }

  if (TextBoxGetBoxType(TextBox)==TEXTBOX)
  {
     Result=ClipBoardAppend(TextBox,sizeof(TextBoxs),BOXDATA);
     if (Result<0)
     {
        HandleUnlock(ItemGetHandle(HBox));
        return(Result);
     }
     if (!TextBoxGetPrevLinkBox(TextBox))
     {
        Result=ClipBoardInsertText(HBox,0,TextBoxGetTextLength(TextBox),0);
     }
  }
  else
     if ((TextBoxGetBoxType(TextBox)>=RECTANGLEPICTUREBOX)&&
         (TextBoxGetBoxType(TextBox)<=POLYGONPICTUREBOX))
     {
         Result=ClipBoardAppend(TextBox,sizeof(PictureBoxs),BOXDATA);
         if (Result<0)
         {
            HandleUnlock(ItemGetHandle(HBox));
            return(Result);
         }
         if (TextBoxGetBoxType(TextBox)==POLYGONPICTUREBOX)
         {
            PolygonEdgeNumber=HandleLock(PictureBoxGetBorderPolygon(TextBox));
            if (PolygonEdgeNumber!=NULL)
            {
               Result=sizeof(int)+sizeof(ORDINATETYPE)*2*(*PolygonEdgeNumber);
               ClipBoardAppend(PolygonEdgeNumber,Result,POLYGONDATA);
               HandleUnlock(PictureBoxGetBorderPolygon(TextBox));
            }
         }
     }
  HandleUnlock(ItemGetHandle(HBox));
  return(Result);
}

int ClipBoardInsertGroup(HANDLE *Group,int GroupNumber)
{
  int i,Result;

  i=ClipBoardSizeofGroup(Group,GroupNumber);
  if (i<=0) return(i);

  ClipBoardInsert(i);
  for (i=0;i<GroupNumber;i++)
  {
      Result=ClipBoardInsertBox(Group[i],0);
      if (Result<0)
         return(Result);
  }
  ReturnOK();
}

void ClipBoardGetDataInfomation(ClipBoards *ClipBoardDataInformation)
{
  char *ClipBoardData;

  if (!ClipBoardHasData())
  {
   err_exit:
     ClipBoardDataInformation->ClipBoardDataType=0;
     ClipBoardDataInformation->ClipBoardDataLength=0;
     return;
  }

  ClipBoardData=HandleLock(GlobalClipBoardHandle);
  if (ClipBoardData==NULL)
  {
     HandleUnlock(GlobalClipBoardHandle);
     goto err_exit;
  }

  memcpy(ClipBoardDataInformation,&ClipBoardData[GlobalClipBoardReadLength],
         sizeof(ClipBoards));
  HandleUnlock(GlobalClipBoardHandle);
  return;
}

int ClipBoardGetText(Wchar *Text)
{
  ClipBoards ClipBoardDataInformation;

  ClipBoardGetDataInfomation(&ClipBoardDataInformation);
  if ((ClipBoardDataInformation.ClipBoardDataLength==0)||
      (ClipBoardDataInformation.ClipBoardDataType!=TEXTDATA))
     return(0);
  else
  {
     ClipBoardGet((void *)Text,&ClipBoardDataInformation.ClipBoardDataLength,
                  &ClipBoardDataInformation.ClipBoardDataType);
     return(ClipBoardDataInformation.ClipBoardDataLength/sizeof(Wchar));
  }
}

int ClipBoardGetImage(InsertImages *InsertImage)
{
  ClipBoards ClipBoardDataInformation;

  ClipBoardGetDataInfomation(&ClipBoardDataInformation);
  if ((ClipBoardDataInformation.ClipBoardDataLength==0)||
      (ClipBoardDataInformation.ClipBoardDataType!=IMAGEDATA))
     return(0);
  else
  {
     ClipBoardGet(InsertImage,&ClipBoardDataInformation.ClipBoardDataLength,
                  &ClipBoardDataInformation.ClipBoardDataType);
     return(ClipBoardDataInformation.ClipBoardDataLength);
  }
}

  /*  PasteSign = 0:Paste to Page,  1: Paste to Text Box */
int ClipBoardGetBox(int PasteSign)
{
  ClipBoards ClipBoardDataInformation;
  HBOX InsertHBox;
//  Boxs TmpBox,*Box;
#define TmpBox        TmpBuf
  Boxs *Box;
  TextBoxs *TextBox;
  PictureBoxs *PictureBox;

  ClipBoardGetDataInfomation(&ClipBoardDataInformation);
  if( ClipBoardDataInformation.ClipBoardDataLength==0
  || ClipBoardDataInformation.ClipBoardDataType!=BOXDATA )
     return(0);

  Box=&TmpBox;
  ClipBoardGet(Box,&ClipBoardDataInformation.ClipBoardDataLength,
               &ClipBoardDataInformation.ClipBoardDataType);
  InsertHBox=BoxInsert(Box,GlobalCurrentPage);
  if (!InsertHBox)
     return(OUTOFMEMORY);
  if (TextBoxGetBoxType((TextBoxs *)Box)==POLYGONPICTUREBOX)
  {
     int *PolygonEdges,PolygonLeft,PolygonTop,PolygonRight,PolygonBottom,i;
     ORDINATETYPE PolygonEdgeData[2*MAXPOLYGONNUMBER+1];

     ClipBoardGetDataInfomation(&ClipBoardDataInformation);
     if ((ClipBoardDataInformation.ClipBoardDataLength==0)||
         (ClipBoardDataInformation.ClipBoardDataType!=POLYGONDATA))
        return(OUTOFMEMORY);
     ClipBoardGet(PolygonEdgeData,&ClipBoardDataInformation.ClipBoardDataLength,
                  &ClipBoardDataInformation.ClipBoardDataType);
     PictureBox=HandleLock(ItemGetHandle(InsertHBox));
     if (PictureBox==NULL)
        return(OUTOFMEMORY);
     PictureBoxSetBorderPolygon(PictureBox,HandleAlloc(
         sizeof(int)+2*sizeof(ORDINATETYPE)*MAXPOLYGONNUMBER,0));
     if (!PictureBoxGetBorderPolygon(PictureBox))
     {
        HandleUnlock(ItemGetHandle(InsertHBox));
        return(OUTOFMEMORY);
     }
     PolygonEdges=HandleLock(PictureBoxGetBorderPolygon(PictureBox));
     if (PolygonEdges==NULL)
     {
        HandleUnlock(ItemGetHandle(InsertHBox));
        return(OUTOFMEMORY);
     }
     memcpy(PolygonEdges,PolygonEdgeData,
            ClipBoardDataInformation.ClipBoardDataLength);
     if (PasteSign==GETTOTEXTBOX)
     {
        PictureBoxSetBoxLeft(PictureBox,0);
        PictureBoxSetBoxTop(PictureBox,0);
        PolygonGetMinRectangle(*PolygonEdges,&(PolygonEdges[1]),
                 &PolygonLeft,&PolygonTop,&PolygonRight,&PolygonBottom);
        for (i=0;i<*PolygonEdges;i++)
        {
            PolygonEdges[i*2+1]-=PolygonLeft;
            PolygonEdges[i*2+2]-=PolygonTop;
        }
     }
     HandleUnlock(PictureBoxGetBorderPolygon(PictureBox));
     HandleUnlock(ItemGetHandle(InsertHBox));
  }
  else
  if (TextBoxGetBoxType((TextBoxs *)Box)==TEXTBOX)
  {
     TextBox=HandleLock(ItemGetHandle(InsertHBox));
     if (TextBox==NULL)
        return(OUTOFMEMORY);

     TextBoxInitialLineTable(TextBox);
     TextBoxSetInvalidPolygons(TextBox,0);

     if ((TextBoxGetTextLength((TextBoxs *)Box)>0)&&
         !TextBoxGetPrevLinkBox((TextBoxs *)Box))
     {
        Wchar *TextBlock;

        ClipBoardGetDataInfomation(&ClipBoardDataInformation);
        if ((ClipBoardDataInformation.ClipBoardDataLength==0)||
            (ClipBoardDataInformation.ClipBoardDataType!=TEXTDATA))
        {
           TextBoxSetTextHandle(TextBox,0);
           TextBoxSetTextLength(TextBox,0);
           TextBoxSetBlockLength(TextBox,0);
           HandleUnlock(ItemGetHandle(InsertHBox));
           return(OUTOFMEMORY);
        }
        TextBoxSetTextHandle(TextBox,HandleAlloc(TextBoxGetBlockLength(TextBox)
                             *sizeof(Wchar),0));
        if (!TextBoxGetTextHandle(TextBox))
        {
           TextBoxSetTextLength(TextBox,0);
           TextBoxSetBlockLength(TextBox,0);
           HandleUnlock(ItemGetHandle(InsertHBox));
           return(OUTOFMEMORY);
        }
        TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
        ClipBoardGet(TextBlock,&ClipBoardDataInformation.ClipBoardDataLength,
                  &ClipBoardDataInformation.ClipBoardDataType);
        TextBlock[TextBoxGetTextLength(TextBox)]=0;
        HandleUnlock(TextBoxGetTextHandle(TextBox));
     }
     else
     {
        TextBoxSetTextHandle(TextBox,0);
        TextBoxSetTextLength(TextBox,0);
        TextBoxSetBlockLength(TextBox,0);
     }
     TextBoxSetPrevLinkBox(TextBox,0);
     TextBoxSetNextLinkBox(TextBox,0);
     InitRL(TextBox);
     FormatAll(InsertHBox);
     HandleUnlock(ItemGetHandle(InsertHBox));
  }
  else
  if ((TextBoxGetBoxType((TextBoxs *)Box)>=RECTANGLEPICTUREBOX)
      &&(TextBoxGetBoxType((TextBoxs *)Box)<=POLYGONPICTUREBOX))
  {
     PictureBox=HandleLock(ItemGetHandle(InsertHBox));
     if (PictureBox==NULL)
        return(OUTOFMEMORY);
     if (PictureBoxGetPictureFileName(PictureBox)[0])
     {
        ImageDescribes *TiffPresent;

        TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
        TiffPresent->ImageHandle=TiffPresent->ImageNewHandle=0;
        PictureBoxImportTiff(PictureBoxGetPictureFileName(PictureBox),
                             InsertHBox);
     }
     HandleUnlock(ItemGetHandle(InsertHBox));
  }

  if (PasteSign==GETTOTEXTBOX)
  {
     ItemSetNext(ItemGetPrev(InsertHBox),0);
     ItemSetPrev(InsertHBox,0);
     ItemSetNext(InsertHBox,0);
     ItemSetFather(InsertHBox,0);

     TextBox=HandleLock(ItemGetHandle(InsertHBox));
     if (TextBox==NULL)
        return(OUTOFMEMORY);
     TextBoxSetEmbodyBox(TextBox);
     HandleUnlock(ItemGetHandle(InsertHBox));
  }
  return(InsertHBox);
}

#ifdef UNUSED
int ClipBoardGetGroup(HANDLE *Group)
{
  int GroupNumbers;

  GroupNumbers=0;
  ClipBoardRead();
  while (ClipBoardHasData())
  {
    Group[GroupNumbers]=ClipBoardGetBox(GETTOPAGE);
    if (!Group[GroupNumbers])
       return(GroupNumbers);
    GroupNumbers++;
  }
  return(GroupNumbers);
}
#endif

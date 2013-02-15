/*-------------------------------------------------------------------
* Name: boxc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

//HBOX CurrentBox=0;
#define TextBoxIsInLink(HHBB1,HHBB2) TextBoxLinkCycle(HHBB1,HHBB2)

int PolygonGetMinRectangle(int PolygonNumber,ORDINATETYPE *PolygonEdges,
                           ORDINATETYPE *MinX,ORDINATETYPE *MinY,
                           ORDINATETYPE *MaxX,ORDINATETYPE *MaxY)
{
  int i;
  *MinX=*MinY=30000;
  *MaxX=*MaxY=-30000;
  for (i=0;i<PolygonNumber;i++)
  {
      if (PolygonEdges[2*i]<(*MinX))
         (*MinX)=PolygonEdges[2*i];

      if (PolygonEdges[2*i]>(*MaxX))
         (*MaxX)=PolygonEdges[2*i];

      if (PolygonEdges[2*i+1]<(*MinY))
         (*MinY)=PolygonEdges[2*i+1];

      if (PolygonEdges[2*i+1]>(*MaxY))
         (*MaxY)=PolygonEdges[2*i+1];
  }

  ReturnOK();
}

static void BoxPolygonMove(int BoxDots,ORDINATETYPE *BoxXY,PictureBoxs *Box)
{
  int i;
  for (i=0;i<BoxDots;i++)
  {
      BoxXY[2*i]+=PictureBoxGetBoxLeft(Box);
      BoxXY[2*i+1]+=PictureBoxGetBoxTop(Box);
  }
}

#ifdef UNUSED
void BoxPolygonUnMove(int BoxDots,ORDINATETYPE *BoxXY,PictureBoxs *Box)
{
  int i;
  for (i=0;i<BoxDots;i++)
  {
      BoxXY[2*i]-=PictureBoxGetBoxLeft(Box);
      BoxXY[2*i+1]-=PictureBoxGetBoxTop(Box);
  }
}

HBOX BoxSearchSort(ORDINATETYPE x,ORDINATETYPE y,HITEM PageItem)
{
  HITEM MidBox;
  TextBoxs *MidBoxPointer;
  ORDINATETYPE SearchX,SearchY;

  MidBox=PageGetBoxHead(PageItem);
  while (MidBox)                       /* Sort by x */
  {
    MidBoxPointer=HandleLock(ItemGetHandle(MidBox));
    SearchX=TextBoxGetBoxLeft(MidBoxPointer);
    HandleUnlock(ItemGetHandle(MidBox));
    if (x>SearchX)
       MidBox=ItemGetNext(MidBox);
    else
       break;
  }

  while (MidBox>0)                       /* When x sort, sort by y */
  {
    MidBoxPointer=HandleLock(ItemGetHandle(MidBox));
    SearchY=TextBoxGetBoxTop(MidBoxPointer);
    HandleUnlock(ItemGetHandle(MidBox));
    if (y>SearchY)
       MidBox=ItemGetNext(MidBox);
    else
       break;
  }
  return(MidBox);
}
#endif    // UNUSED

void BoxPolygonRotate(int BoxDots,ORDINATETYPE *BoxXY,PictureBoxs *Box)
{
  int i;
  ORDINATETYPE X,Y;

  if (PictureBoxGetRotateAngle(Box))
  {
     X=PictureBoxGetRotateAxisX(Box)+PictureBoxGetBoxLeft(Box);
     Y=PictureBoxGetRotateAxisY(Box)+PictureBoxGetBoxTop(Box);

     for (i=0;i<BoxDots;i++)
         Rotate(&BoxXY[2*i],&BoxXY[2*i+1],BoxXY[2*i],BoxXY[2*i+1],
                X,Y,PictureBoxGetRotateAngle(Box));
  }
}

void BoxPolygonToWindowXY(int BoxDots,ORDINATETYPE *BoxXY)
{
  int i;

  for (i=0;i<BoxDots;i++)
  {
      BoxXY[2*i]=UserXToWindowX(BoxXY[2*i]);
      BoxXY[2*i+1]=UserYToWindowY(BoxXY[2*i+1]);
  }
}

static void TextBoxGetPolygonBorder(TextBoxs *TextBox,
                           int *BoxDots,ORDINATETYPE *BoxXY)
{
  *BoxDots=4;
  BoxXY[0]=PictureBoxGetBoxLeft(TextBox);
  BoxXY[1]=PictureBoxGetBoxTop(TextBox);
  BoxXY[2]=PictureBoxGetBoxRight(TextBox);
  BoxXY[3]=BoxXY[1];     // PictureBoxGetBoxTop(TextBox);
  BoxXY[4]=BoxXY[2];     // PictureBoxGetBoxRight(TextBox);
  BoxXY[5]=PictureBoxGetBoxBottom(TextBox);
  BoxXY[6]=BoxXY[0];     // PictureBoxGetBoxLeft(TextBox);
  BoxXY[7]=BoxXY[5];     // PictureBoxGetBoxBottom(TextBox);
  BoxPolygonRotate(*BoxDots,BoxXY,(PictureBoxs *)TextBox);
}

static void PictureBoxGetPolygonBorder(PictureBoxs *ImageBox,
                           int *BoxDots,ORDINATETYPE *BoxXY)
{
  int CornerPoints;
  ORDINATETYPE CornerRadius;

  switch (PictureBoxGetBoxType(ImageBox))
  {
    case RECTANGLEPICTUREBOX:
         *BoxDots=4;
         BoxXY[0]=PictureBoxGetBoxLeft(ImageBox);
         BoxXY[1]=PictureBoxGetBoxTop(ImageBox);
         BoxXY[2]=PictureBoxGetBoxRight(ImageBox);
         BoxXY[3]=BoxXY[1];     // PictureBoxGetBoxTop(ImageBox);
         BoxXY[4]=BoxXY[2];     // PictureBoxGetBoxRight(ImageBox);
         BoxXY[5]=PictureBoxGetBoxBottom(ImageBox);
         BoxXY[6]=BoxXY[0];     // PictureBoxGetBoxLeft(ImageBox);
         BoxXY[7]=BoxXY[5];     // PictureBoxGetBoxBottom(ImageBox);
         BoxPolygonRotate(*BoxDots,BoxXY,ImageBox);
         break;
    case CORNERPICTUREBOX:
         CornerRadius=PictureBoxGetCornerRadius(ImageBox);
         *BoxDots=CornerPoints=0;
         if (CornerRadius>0)
            ArctoLine(PictureBoxGetBoxLeft(ImageBox)+CornerRadius,
                      PictureBoxGetBoxTop(ImageBox)+CornerRadius,
                      CornerRadius,CornerRadius,
                      180,270,&CornerPoints,(int *)BoxXY,COMPUTEANGLEINC);
         *BoxDots+=CornerPoints;
         BoxXY[2*(*BoxDots)]=PictureBoxGetBoxLeft(ImageBox)+CornerRadius;
         BoxXY[2*(*BoxDots)+1]=PictureBoxGetBoxTop(ImageBox);
         BoxXY[2*(*BoxDots)+2]=PictureBoxGetBoxRight(ImageBox)-CornerRadius;
         BoxXY[2*(*BoxDots)+3]=PictureBoxGetBoxTop(ImageBox);
         *BoxDots+=2;
         if (CornerRadius>0)
            ArctoLine(PictureBoxGetBoxRight(ImageBox)-CornerRadius,
                      PictureBoxGetBoxTop(ImageBox)+CornerRadius,
                      CornerRadius,CornerRadius,
                      270,360,&CornerPoints,(int *)&BoxXY[2*(*BoxDots)],COMPUTEANGLEINC);
         *BoxDots+=CornerPoints;
         BoxXY[2*(*BoxDots)]=PictureBoxGetBoxRight(ImageBox);
         BoxXY[2*(*BoxDots)+1]=PictureBoxGetBoxTop(ImageBox)+CornerRadius;
         BoxXY[2*(*BoxDots)+2]=PictureBoxGetBoxRight(ImageBox);
         BoxXY[2*(*BoxDots)+3]=PictureBoxGetBoxBottom(ImageBox)-CornerRadius;
         *BoxDots+=2;
         if (CornerRadius>0)
            ArctoLine(PictureBoxGetBoxRight(ImageBox)-CornerRadius,
                      PictureBoxGetBoxBottom(ImageBox)-CornerRadius,
                      CornerRadius,CornerRadius,
                      0,90,&CornerPoints,(int *)&BoxXY[2*(*BoxDots)],COMPUTEANGLEINC);
         *BoxDots+=CornerPoints;
         BoxXY[2*(*BoxDots)]=PictureBoxGetBoxRight(ImageBox)-CornerRadius;
         BoxXY[2*(*BoxDots)+1]=PictureBoxGetBoxBottom(ImageBox);
         BoxXY[2*(*BoxDots)+2]=PictureBoxGetBoxLeft(ImageBox)+CornerRadius;
         BoxXY[2*(*BoxDots)+3]=PictureBoxGetBoxBottom(ImageBox);
         *BoxDots+=2;
         if (CornerRadius>0)
            ArctoLine(PictureBoxGetBoxLeft(ImageBox)+CornerRadius,
                      PictureBoxGetBoxBottom(ImageBox)-CornerRadius,
                      CornerRadius,CornerRadius,
                      90,180,&CornerPoints,(int *)&BoxXY[2*(*BoxDots)],COMPUTEANGLEINC);
         *BoxDots+=CornerPoints;
         BoxXY[2*(*BoxDots)]=PictureBoxGetBoxLeft(ImageBox);
         BoxXY[2*(*BoxDots)+1]=PictureBoxGetBoxBottom(ImageBox)-CornerRadius;
         BoxXY[2*(*BoxDots)+2]=PictureBoxGetBoxLeft(ImageBox);
         BoxXY[2*(*BoxDots)+3]=PictureBoxGetBoxTop(ImageBox)+CornerRadius;
         *BoxDots+=2;
         BoxPolygonRotate(*BoxDots,BoxXY,ImageBox);
         break;
    case ELIPSEPICTUREBOX:
         ArctoLine((PictureBoxGetBoxLeft(ImageBox)+PictureBoxGetBoxRight(ImageBox))/2,
                   (PictureBoxGetBoxTop(ImageBox)+PictureBoxGetBoxBottom(ImageBox))/2,
                   (PictureBoxGetBoxRight(ImageBox)-PictureBoxGetBoxLeft(ImageBox))/2,
                   (PictureBoxGetBoxBottom(ImageBox)-PictureBoxGetBoxTop(ImageBox))/2,
                   0,360,BoxDots,(int *)BoxXY,COMPUTEANGLEINC);
         BoxPolygonRotate(*BoxDots,BoxXY,ImageBox);
         break;
    case POLYGONPICTUREBOX:
         PolygonGetBorderData(PictureBoxGetBorderPolygon(ImageBox),
                              BoxDots,BoxXY);
         BoxPolygonRotate(*BoxDots,BoxXY,ImageBox);
         break;
    default:
         break;
  }
}

void BoxGetPolygonBorder(Boxs *Box,int *BoxDots,ORDINATETYPE *BoxXY)
{
  if (TextBoxGetBoxType(((TextBoxs*)Box))==TEXTBOX
      ||TextBoxGetBoxType(((TextBoxs*)Box))==TABLEBOX
      ||TextBoxGetBoxType(((TextBoxs*)Box))==LINEBOX)
     TextBoxGetPolygonBorder((TextBoxs*)Box,BoxDots,BoxXY);
  else
     if (PictureBoxGetBoxType(Box)>=RECTANGLEPICTUREBOX&&
         PictureBoxGetBoxType(Box)<=POLYGONPICTUREBOX)
        PictureBoxGetPolygonBorder((PictureBoxs *)Box,BoxDots,BoxXY);
  return;
}

void BoxGetPolygonDrawBorder(Boxs *Box,int *BoxDots,ORDINATETYPE *BoxXY)
{
  int CornerPoints;
  ORDINATETYPE CornerRadius;
  PictureBoxs *ImageBox;

  switch(PictureBoxGetBoxType(Box))
  {
    case TEXTBOX:
    case TABLEBOX:
    case LINEBOX:
       TextBoxGetPolygonBorder((TextBoxs *)Box,BoxDots,BoxXY);
       break;
    case RECTANGLEPICTUREBOX:
    case POLYGONPICTUREBOX:
       PictureBoxGetPolygonBorder((PictureBoxs *)Box,BoxDots,BoxXY);
       break;
    case ELIPSEPICTUREBOX:
       ImageBox=(PictureBoxs *)Box;
       ArctoLine((PictureBoxGetBoxLeft(ImageBox)+PictureBoxGetBoxRight(ImageBox))/2,
                 (PictureBoxGetBoxTop(ImageBox)+PictureBoxGetBoxBottom(ImageBox))/2,
                 (PictureBoxGetBoxRight(ImageBox)-PictureBoxGetBoxLeft(ImageBox))/2,
                 (PictureBoxGetBoxBottom(ImageBox)-PictureBoxGetBoxTop(ImageBox))/2,
                 0,360,BoxDots,BoxXY,DRAWANGLEINC);
       BoxPolygonRotate(*BoxDots,BoxXY,ImageBox);
       break;
    case CORNERPICTUREBOX:
       ImageBox=(PictureBoxs *)Box;
       CornerRadius=PictureBoxGetCornerRadius(ImageBox);
       *BoxDots=CornerPoints=0;

       if (CornerRadius>0)
          ArctoLine(PictureBoxGetBoxLeft(ImageBox)+CornerRadius,
                    PictureBoxGetBoxTop(ImageBox)+CornerRadius,
                    CornerRadius,CornerRadius,
                    180,270,&CornerPoints,BoxXY,DRAWANGLEINC);
       *BoxDots+=CornerPoints;
       BoxXY[2*(*BoxDots)]=PictureBoxGetBoxLeft(ImageBox)+CornerRadius;
       BoxXY[2*(*BoxDots)+1]=PictureBoxGetBoxTop(ImageBox);
       BoxXY[2*(*BoxDots)+2]=PictureBoxGetBoxRight(ImageBox)-CornerRadius;
       BoxXY[2*(*BoxDots)+3]=PictureBoxGetBoxTop(ImageBox);
       *BoxDots+=2;

       if (CornerRadius>0)
          ArctoLine(PictureBoxGetBoxRight(ImageBox)-CornerRadius,
                    PictureBoxGetBoxTop(ImageBox)+CornerRadius,
                    CornerRadius,CornerRadius,
                    270,360,&CornerPoints,&BoxXY[2*(*BoxDots)],DRAWANGLEINC);
       *BoxDots+=CornerPoints;
       BoxXY[2*(*BoxDots)]=PictureBoxGetBoxRight(ImageBox);
       BoxXY[2*(*BoxDots)+1]=PictureBoxGetBoxTop(ImageBox)+CornerRadius;
       BoxXY[2*(*BoxDots)+2]=PictureBoxGetBoxRight(ImageBox);
       BoxXY[2*(*BoxDots)+3]=PictureBoxGetBoxBottom(ImageBox)-CornerRadius;
       *BoxDots+=2;

       if (CornerRadius>0)
          ArctoLine(PictureBoxGetBoxRight(ImageBox)-CornerRadius,
                    PictureBoxGetBoxBottom(ImageBox)-CornerRadius,
                    CornerRadius,CornerRadius,
                    0,90,&CornerPoints,&BoxXY[2*(*BoxDots)],DRAWANGLEINC);
       *BoxDots+=CornerPoints;
       BoxXY[2*(*BoxDots)]=PictureBoxGetBoxRight(ImageBox)-CornerRadius;
       BoxXY[2*(*BoxDots)+1]=PictureBoxGetBoxBottom(ImageBox);
       BoxXY[2*(*BoxDots)+2]=PictureBoxGetBoxLeft(ImageBox)+CornerRadius;
       BoxXY[2*(*BoxDots)+3]=PictureBoxGetBoxBottom(ImageBox);
       *BoxDots+=2;

       if (CornerRadius>0)
          ArctoLine(PictureBoxGetBoxLeft(ImageBox)+CornerRadius,
                    PictureBoxGetBoxBottom(ImageBox)-CornerRadius,
                    CornerRadius,CornerRadius,
                    90,180,&CornerPoints,&BoxXY[2*(*BoxDots)],DRAWANGLEINC);
       *BoxDots+=CornerPoints;
       BoxXY[2*(*BoxDots)]=PictureBoxGetBoxLeft(ImageBox);
       BoxXY[2*(*BoxDots)+1]=PictureBoxGetBoxBottom(ImageBox)-CornerRadius;
       BoxXY[2*(*BoxDots)+2]=PictureBoxGetBoxLeft(ImageBox);
       BoxXY[2*(*BoxDots)+3]=PictureBoxGetBoxTop(ImageBox)+CornerRadius;
       *BoxDots+=2;

       BoxPolygonRotate(*BoxDots,BoxXY,ImageBox);
       break;
  }  /*- end of switch -*/
  return;
}

static int FirstLinkBoxInOtherPage(HTEXTBOX BoxItem)
{
  HPAGE Page1,Page2;
  HTEXTBOX FirstLinkBox;

  if(BoxItem<=0)
     return(FALSE);

  FirstLinkBox=GetFirstLinkBox(BoxItem);
  if (FirstLinkBox==BoxItem || !FirstLinkBox)
     return(FALSE);
  Page1=ItemGetFather(BoxItem);
  Page2=ItemGetFather(FirstLinkBox);
  if (Page1!=Page2)
     return(TRUE);
  else
     return(FALSE);
}

HBOX TextBoxInsert(TextBoxs *InsertBox,HITEM PageItem)
{
  Items InsertBoxItem;
  HBOX InsertBoxHandle;
  HITEM MidBox;
  TextBoxs *MidBoxPointer;

  memset(&InsertBoxItem,0,sizeof(InsertBoxItem));

//  MidBox=0;
  MidBox=ItemAppend(&InsertBoxItem,PageItem);
                                       /* Insert to the latest */
  if (!MidBox)
     return(0);
  InsertBoxHandle=HandleAlloc(sizeof(TextBoxs),REVERSEDCLASS);
  if (!InsertBoxHandle)
  {
     ItemDelete(MidBox);
     return(0);
  }

  MidBoxPointer=HandleLock(InsertBoxHandle);
  if (MidBoxPointer==NULL)
  {
     ItemDelete(MidBox);
     HandleUnlock(InsertBoxHandle);
     HandleFree(InsertBoxHandle);
     return(0);
  }
  memcpy(MidBoxPointer,InsertBox,sizeof(TextBoxs));

  ItemSetHandle(MidBox,InsertBoxHandle);
  ItemSetChild(MidBox,0);
  FileSetModified();
  if (InsertBox->PrevLinkBox==0&&InsertBox->TextLength==0&&!FileIsBeenLoading())
  {
     unsigned short TmpStr[2];

     InitRL(MidBoxPointer);
     TmpStr[0]=' ';
     TextBoxInsertString(MidBox,0,TmpStr,1);
     TextBoxDeleteString(MidBox,0,1);
     FormatAll(MidBox);
  }
  HandleUnlock(InsertBoxHandle);
  return(MidBox);
}

HBOX TableBoxInsert(FormBoxs *InsertBox,HITEM PageItem)
{
  Items InsertBoxItem;
  HBOX InsertBoxHandle;
  HITEM MidBox;
  FormBoxs *MidBoxPointer;

  memset(&InsertBoxItem,0,sizeof(InsertBoxItem));

  //MidBox=0;
  MidBox=ItemAppend(&InsertBoxItem,PageItem);
                                       /* Insert to the latest */
  if (!MidBox)
     return(0);
  InsertBoxHandle=HandleAlloc(sizeof(FormBoxs),REVERSEDCLASS);
  if (!InsertBoxHandle)
  {
     ItemDelete(MidBox);
     return(0);
  }

  MidBoxPointer=HandleLock(InsertBoxHandle);
  if (MidBoxPointer==NULL)
  {
     ItemDelete(MidBox);
     HandleUnlock(InsertBoxHandle);
     HandleFree(InsertBoxHandle);
     return(0);
  }
  memcpy(MidBoxPointer,InsertBox,sizeof(FormBoxs));
  HandleUnlock(InsertBoxHandle);

  ItemSetHandle(MidBox,InsertBoxHandle);
  ItemSetChild(MidBox,0);
  FileSetModified();
  return(MidBox);
}

HBOX LineBoxInsert(LineBoxs *InsertBox,HITEM PageItem)
{
  Items InsertBoxItem;
  HBOX InsertBoxHandle;
  HITEM MidBox;
  LineBoxs *MidBoxPointer;

  memset(&InsertBoxItem,0,sizeof(InsertBoxItem));
//  MidBox=0;
  MidBox=ItemAppend(&InsertBoxItem,PageItem);
                                       /* Insert to the latest */
  if (!MidBox)
     return(0);
  InsertBoxHandle=HandleAlloc(sizeof(LineBoxs),0);
  if (!InsertBoxHandle)
  {
     ItemDelete(MidBox);
     return(0);
  }
  MidBoxPointer=HandleLock(InsertBoxHandle);
  if (MidBoxPointer==NULL)
  {
     ItemDelete(MidBox);
     HandleUnlock(InsertBoxHandle);
     HandleFree(InsertBoxHandle);
     return(0);
  }
  memcpy(MidBoxPointer,InsertBox,sizeof(LineBoxs));
  HandleUnlock(InsertBoxHandle);
  ItemSetHandle(MidBox,InsertBoxHandle);
  ItemSetChild(MidBox,0);
  FileSetModified();
  return(MidBox);
}

HBOX PictureBoxInsert(PictureBoxs *InsertBox,HITEM PageItem)
{
  Items InsertBoxItem;
  HBOX InsertBoxHandle;
  HITEM MidBox;
  PictureBoxs *MidBoxPointer;

  memset(&InsertBoxItem,0,sizeof(InsertBoxItem));
//  MidBox=0;
  MidBox=ItemAppend(&InsertBoxItem,PageItem);
                                       /* Insert to the latest */
  if (!MidBox)
     return(0);
  InsertBoxHandle=HandleAlloc(sizeof(PictureBoxs),0);
  if (!InsertBoxHandle)
  {
     ItemDelete(MidBox);
     return(0);
  }
  MidBoxPointer=HandleLock(InsertBoxHandle);
  if (MidBoxPointer==NULL)
  {
     ItemDelete(MidBox);
     HandleUnlock(InsertBoxHandle);
     HandleFree(InsertBoxHandle);
     return(0);
  }
  memcpy(MidBoxPointer,InsertBox,sizeof(PictureBoxs));
  HandleUnlock(InsertBoxHandle);
  ItemSetHandle(MidBox,InsertBoxHandle);
  ItemSetChild(MidBox,0);
  FileSetModified();
  return(MidBox);
}

HBOX BoxInsert(Boxs *InsertBox,HITEM PageItem)
{
  if ( (*InsertBox).TextBox.BoxType==TEXTBOX)
     return(TextBoxInsert( &(*InsertBox).TextBox,PageItem));

  if ( (*InsertBox).TextBox.BoxType==TABLEBOX)
     return(TableBoxInsert( &(*InsertBox).FormBox,PageItem));

  if ( (*InsertBox).TextBox.BoxType==LINEBOX)
     return(LineBoxInsert( &(*InsertBox).LineBox,PageItem));

  if (( (*InsertBox).TextBox.BoxType>=RECTANGLEPICTUREBOX)&&
      ( (*InsertBox).TextBox.BoxType<=POLYGONPICTUREBOX))
     return(PictureBoxInsert( &(*InsertBox).PictureBox,PageItem));

  return(0);
}

void BoxDelete(HITEM BoxItem)
{
  TextBoxs *TextBox;
  FormBoxs *FormBox;
  // LineBoxs *LineBox;
  PictureBoxs *PictureBox;
  char ThisBoxIsInsertBox;

  if(BoxItem<=0)
     return;
  TextBox=HandleLock(ItemGetHandle(BoxItem));
  if (TextBox==NULL)
     return;

  ThisBoxIsInsertBox=TextBoxIsEmbodyBox(TextBox);
  if (TextBoxGetBoxType(TextBox)==TEXTBOX)
  {
     if ((!TextBoxGetPrevLinkBox(TextBox))&&(!TextBoxGetNextLinkBox(TextBox)))
     {
        if (TextBoxGetTextHandle(TextBox))
        {
           TextBoxDeleteLineTable(TextBox);
           HandleFree(TextBoxGetTextHandle(TextBox));
        }
     }
     else
     {
        TextBoxs *PrevTextBox,*NextTextBox;

        if (TextBoxGetPrevLinkBox(TextBox))
        {
           PrevTextBox=HandleLock(ItemGetHandle(TextBoxGetPrevLinkBox(TextBox)));
           if (PrevTextBox==NULL)
           {
              HandleUnlock(ItemGetHandle(BoxItem));
              return;
           }
           TextBoxSetNextLinkBox(PrevTextBox,TextBoxGetNextLinkBox(TextBox));
           HandleUnlock(ItemGetHandle(TextBoxGetPrevLinkBox(TextBox)));
        }
        if (TextBoxGetNextLinkBox(TextBox))
        {
           NextTextBox=HandleLock(ItemGetHandle(TextBoxGetNextLinkBox(TextBox)));
           if (NextTextBox==NULL)
           {
              HandleUnlock(ItemGetHandle(BoxItem));
              return;
           }
           TextBoxSetPrevLinkBox(NextTextBox,TextBoxGetPrevLinkBox(TextBox));
           HandleUnlock(ItemGetHandle(TextBoxGetNextLinkBox(TextBox)));
        }
     }
  }
  else
  if (TextBoxGetBoxType(TextBox)==TABLEBOX)
  {
     FormBox=(FormBoxs*)TextBox;
     if (TableBoxGetTextHandle(FormBox))
     {
        TextBoxDeleteLineTable(TextBox);
        HandleFree(TableBoxGetTextHandle(FormBox));
        HandleFree(TableBoxGethCellTable(FormBox));
     }
  }
  else                              // When Box is LineBox, no action
  {
     PictureBox=(PictureBoxs *)TextBox;
     if((PictureBoxGetBoxType(PictureBox)==RECTANGLEPICTUREBOX)
     || (PictureBoxGetBoxType(PictureBox)==CORNERPICTUREBOX)
     || (PictureBoxGetBoxType(PictureBox)==ELIPSEPICTUREBOX))
        PictureBoxClearImage(BoxItem);
     else
     if (PictureBoxGetBoxType(PictureBox)==POLYGONPICTUREBOX)
     {
        PictureBoxClearImage(BoxItem);
        if (PictureBoxGetBorderPolygon(PictureBox))
           HandleFree(PictureBoxGetBorderPolygon(PictureBox));
     }
  }

  HandleUnlock(ItemGetHandle(BoxItem));
  HandleFree(ItemGetHandle(BoxItem));
  FileSetModified();
  if (!ThisBoxIsInsertBox)
     ItemDelete(BoxItem);
}

#ifdef UNUSED           // ByHance, 96,1.29
void BoxResize(HITEM BoxItem,ORDINATETYPE ResizeX,ORDINATETYPE ResizeY)
{
  TextBoxs *MidBox;

  if(BoxItem<=0)
     return;

  MidBox=HandleLock(ItemGetHandle(BoxItem));
  if (MidBox==NULL)
     return;
  if (BoxIsLocked(MidBox))
  {
     HandleUnlock(ItemGetHandle(BoxItem));
     return;
  }
  TextBoxSetBoxWidth(MidBox,ResizeX);
  TextBoxSetBoxHeight(MidBox,ResizeY);
  FileSetModified();
  HandleUnlock(ItemGetHandle(BoxItem));
}
#endif      // UNUSED           // ByHance, 96,1.29

void BoxMove(HITEM BoxItem,ORDINATETYPE MoveX,ORDINATETYPE MoveY)
{
  TextBoxs *MidBox;

  if(BoxItem<=0)
     return;

  MidBox=HandleLock(ItemGetHandle(BoxItem));
  if (MidBox==NULL)
     return;
  if (BoxIsLocked(MidBox))
  {
     HandleUnlock(ItemGetHandle(BoxItem));
     return;
  }

  UndoInsertBoxMove(TextBoxGetBoxLeft(MidBox),TextBoxGetBoxTop(MidBox));
  if (TextBoxGetBoxType(MidBox)==POLYGONPICTUREBOX)
  {
     PictureBoxs *MidPictureBox;
     int *PolygonEdges,i;

     MidPictureBox=(PictureBoxs *)MidBox;
     PolygonEdges=HandleLock(PictureBoxGetBorderPolygon(MidPictureBox));
     for (i=0;i<PolygonEdges[0];i++)
     {
         PolygonEdges[2*i+1]+=MoveX;
         PolygonEdges[2*i+2]+=MoveY;
     }
     HandleUnlock(PictureBoxGetBorderPolygon(MidPictureBox));
  }
  TextBoxSetBoxLeft(MidBox,TextBoxGetBoxLeft(MidBox)+MoveX);
  TextBoxSetBoxTop(MidBox,TextBoxGetBoxTop(MidBox)+MoveY);
  FileSetModified();
  HandleUnlock(ItemGetHandle(BoxItem));
}

void BoxSetBackward(HBOX HBox)
{
  if (ItemGetPrev(HBox))
     ItemSetFront(HBox,ItemGetPrev(ItemGetPrev(HBox)));
}

void BoxSetForward(HBOX HBox)
{
  if (ItemGetNext(HBox))
     ItemSetFront(HBox,ItemGetNext(HBox));
}

void BoxSetBackground(HBOX HBox)
{
  if (ItemGetPrev(HBox))
     ItemSetFront(HBox,0);
}

void BoxSetFront(HBOX HBox)
{
  if (ItemGetNext(HBox))
     ItemSetFront(HBox,ItemGetLastChild(ItemGetFather(HBox)));
}

void BoxGetRect(HBOX HBox,ORDINATETYPE* BoxLeft,ORDINATETYPE*BoxTop,
                ORDINATETYPE*BoxRight,ORDINATETYPE*BoxBottom)
{
  TextBoxs *MidBox;

  if(HBox<=0)
     return;

  MidBox=HandleLock(ItemGetHandle(HBox));
  if (MidBox!=NULL)
  {
     *BoxLeft=TextBoxGetBoxLeft(MidBox);
     *BoxTop=TextBoxGetBoxTop(MidBox);
     *BoxRight=*BoxLeft+TextBoxGetBoxWidth(MidBox);
     *BoxBottom=*BoxTop+TextBoxGetBoxHeight(MidBox);
     HandleUnlock(ItemGetHandle(HBox));
  }
  return;
}

static void TextBoxDraw(HBOX BoxItem)
{
  TextBoxs *BoxPointer;

  if(BoxItem<=0)
     return;
  BoxPointer=HandleLock(ItemGetHandle(BoxItem));
  if (BoxPointer==NULL)
     return;

  if( !TextBoxGetPrevLinkBox(BoxPointer)
  || PrintingSign                            //By zjh 1997.3.18  avoid losing Link box when printing
  || FirstLinkBoxInOtherPage(BoxItem) )
     TextBoxRedraw(BoxItem,0,30000,FALSE);

  if (GlobalTextBlockStart<GlobalTextBlockEnd && !PrintingSign)
  {
     if((!TextBoxGetPrevLinkBox(BoxPointer)&&TextBoxLinkCycle(BoxItem,GlobalBoxHeadHandle))
     || (FirstLinkBoxInOtherPage(BoxItem) && BoxItem==GlobalBoxHeadHandle) )
       DisplayBlock(BoxItem,GlobalTextBlockStart,GlobalTextBlockEnd);
  }

  HandleUnlock(ItemGetHandle(BoxItem));
  TextBoxDrawTail(BoxItem,0);             // ByHance, 95,12.6
}

static void PolygonGetBorderData(HANDLE PolygonBorderHandle,
                      int *PolygonDotNumber, ORDINATETYPE *PolygonDots)
{
  int *BoxBorder;

  if (PolygonBorderHandle<=0)
  {
     *PolygonDotNumber=0;
     return;
  }
  BoxBorder=HandleLock(PolygonBorderHandle);
  if (BoxBorder==NULL)
  {
     *PolygonDotNumber=0;
     return;
  }
  *PolygonDotNumber=*BoxBorder;
  memcpy(PolygonDots,BoxBorder+1,2*(*PolygonDotNumber)*sizeof(ORDINATETYPE));
  HandleUnlock(PolygonBorderHandle);
}

void XorPutCell(HBOX BoxItem,int DrawCell)
{
  ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
  int BoxDots,Left,Top,Right,Bottom;
  struct viewporttype TmpViewPort;
  int SaveColor;
  FormBoxs *BoxPointer;
  DC dc;

  if(BoxItem<=0)
     return;
  if (ItemGetFather(BoxItem)!=GlobalCurrentPage)
     return;
  BoxPointer=HandleLock(ItemGetHandle(BoxItem));
  if (BoxPointer==NULL)
     return;

  MouseHidden();
  getviewsettings(&TmpViewPort);
  WindowGetRect(1,&Left,&Top,&Right,&Bottom);
  setviewport(Left,Top,Right,Bottom,1);
  SaveColor=getcolor();

  if (TableBoxGetBoxType(BoxPointer)==TABLEBOX&&DrawCell>=0)
  {
     FBGetCellRect(BoxItem,DrawCell,&Left,&Top,&Right,&Bottom);
     Left+=TableBoxGetBoxLeft(BoxPointer);
     Right+=TableBoxGetBoxLeft(BoxPointer);
     Top+=TableBoxGetBoxTop(BoxPointer);
     Bottom+=TableBoxGetBoxTop(BoxPointer);
     BoxDots=4;

     BoxXY[0]=Left;
     BoxXY[1]=Top;
     BoxXY[2]=Right;
     BoxXY[3]=Top;
     BoxXY[4]=Right;
     BoxXY[5]=Bottom;
     BoxXY[6]=Left;
     BoxXY[7]=Bottom;

     if (TableBoxGetRotateAngle(BoxPointer))
        BoxPolygonRotate(BoxDots,BoxXY,(PictureBoxs *)BoxPointer);
     BoxPolygonToWindowXY(BoxDots,BoxXY);
     PolygonGetMinRectangle(BoxDots,BoxXY,&dc.left,&dc.top,&dc.right,&dc.bottom);
     setwritemode(XOR_PUT);
     setcolor(EGA_WHITE);
     FillPolygon((LPDC)&dc,(LPPOINT)BoxXY,BoxDots);
     setwritemode(COPY_PUT);
  }

  setcolor(SaveColor);
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  MouseShow();

  HandleUnlock(ItemGetHandle(BoxItem));
}

void TableBoxClear(HBOX BoxItem)
{
  ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
  short XY[2*MAXPOLYGONNUMBER];
  int i,BoxDots,Left,Top,Right,Bottom;
  struct viewporttype TmpViewPort;
  int SaveColor;
  FormBoxs *BoxPointer;

  if(BoxItem<=0)
     return;

  if (ItemGetFather(BoxItem)!=GlobalCurrentPage)
     return;
  BoxPointer=HandleLock(ItemGetHandle(BoxItem));
  if (BoxPointer==NULL)
     return;
  if (TableBoxGetBoxType(BoxPointer)!=TABLEBOX)
     return;

  MouseHidden();
  getviewsettings(&TmpViewPort);
  WindowGetRect(1,&Left,&Top,&Right,&Bottom);
  setviewport(Left,Top,Right,Bottom,1);
  SaveColor=getcolor();
  setfillstyle(1,EGA_WHITE);

  BoxGetPolygonDrawBorder((Boxs *)BoxPointer,&BoxDots,BoxXY);
  BoxPolygonToWindowXY(BoxDots,BoxXY);
  if (TableBoxGetRotateAngle(BoxPointer)) {
     for(i=0;i<2*BoxDots;i++) XY[i]=(short)BoxXY[i];
     // fillpoly(BoxDots,BoxXY);
     fillpoly(BoxDots,XY);
  } else
     bar(BoxXY[0],BoxXY[1],BoxXY[2],BoxXY[5]);

  setcolor(SaveColor);
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  MouseShow();

  HandleUnlock(ItemGetHandle(BoxItem));
}

void TableBoxDraw(HBOX BoxItem)
{
  struct viewporttype TmpViewPort;
  int Left,Top,Right,Bottom;
  int SaveColor;
  //unsigned short old_style;
  FormBoxs *BoxPointer;

  if(BoxItem<=0)
     return;

  if (ItemGetFather(BoxItem)!=GlobalCurrentPage)
     return;
  BoxPointer=HandleLock(ItemGetHandle(BoxItem));
  if (BoxPointer==NULL)
     return;

  if (!PrintingSign)
  {
      MouseHidden();
      getviewsettings(&TmpViewPort);
      WindowGetRect(1,&Left,&Top,&Right,&Bottom);
      setviewport(Left,Top,Right,Bottom,1);
      SaveColor=getcolor();
      setcolor(EGA_BLACK);
  }
  else
      SetDeviceColor(EGA_BLACK,1);      // set color as  Black

  if(TableBoxGetBoxType(BoxPointer)==TABLEBOX
  && TableBoxGetnumLines(BoxPointer)>0 && TableBoxGetnumCols(BoxPointer)>0)
  {
    #ifdef NOTCORRECT_BYHANCE
     if (!PrintingSign)
     {
       #ifdef __TURBOC__
            struct linesettingstype SaveLineStyle;
            getlinesettings(&SaveLineStyle);
            setlinestyle(4,0x5555,1);
       #else
            old_style=getlinestyle();
            setlinestyle(0x5555);
       #endif
     }
    #endif // NOTCORRECT_BYHANCE

     TextBoxRedraw(BoxItem,0,30000,FALSE);      // Dg Add in 1996,2
     FBDrawBorder(BoxItem);

    #ifdef NOTCORRECT_BYHANCE
     if (!PrintingSign)
     {
       #ifdef __TURBOC__
            setlinestyle(SaveLineStyle.linestyle,
                     SaveLineStyle.upattern,
                     SaveLineStyle.thickness);
       #else
            setlinestyle(old_style);
       #endif
     }
    #endif     // NOTCORRECT_BYHANCE
  }

  if (!PrintingSign)
  {
     if(BoxItem==GlobalBoxHeadHandle)
     {
       if(GlobalTableBlockStart>=0 && GlobalTableBlockEnd>=0)
         DisplayCellBlock(BoxItem,GlobalTableBlockStart,GlobalTableBlockEnd);

       if(GlobalTextBlockStart>=0 && GlobalTextBlockEnd>=0)
         DisplayBlock(BoxItem,GlobalTextBlockStart,GlobalTextBlockEnd);
     }

     setcolor(SaveColor);
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
  }

  HandleUnlock(ItemGetHandle(BoxItem));
}

static void LineBoxDraw(HBOX BoxItem)
{
  ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
  short XY[2*MAXPOLYGONNUMBER];
  int i,BoxDots,Left,Top,Right,Bottom;
  struct viewporttype TmpViewPort;
  int SaveColor;
  LineBoxs *BoxPointer;

  if(BoxItem<=0)
     return;
  if (ItemGetFather(BoxItem)!=GlobalCurrentPage)
     return;
  BoxPointer=HandleLock(ItemGetHandle(BoxItem));
  if (BoxPointer==NULL)
     return;

  if (!PrintingSign)
  {
     MouseHidden();
     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
     SaveColor=getcolor();
     setfillstyle(1,LineBoxGetBoxBackColor(BoxPointer));
  }
  else
      SetDeviceColor(EGA_BLACK,1);      // set color as  Black

  if (LineBoxGetBoxType(BoxPointer)==LINEBOX)
  {
     if (!PrintingSign&&!LineBoxGetBoxBorderType(BoxPointer)
         &&!LineBoxGetBoxArrowType(BoxPointer))
     {
        BoxGetPolygonDrawBorder((Boxs *)BoxPointer,&BoxDots,BoxXY);
        BoxPolygonToWindowXY(BoxDots,BoxXY);
        if (LineBoxGetRotateAngle(BoxPointer)) {
           for(i=0;i<2*BoxDots;i++) XY[i]=(short)BoxXY[i];
          // fillpoly(BoxDots,BoxXY);
           fillpoly(BoxDots,XY);
        } else
           bar(BoxXY[0],BoxXY[1],BoxXY[2],BoxXY[5]);
     }
     else
     {
        BoxXY[0]=LineBoxGetBoxLeft(BoxPointer);
        BoxXY[1]=BoxXY[3]=LineBoxGetBoxTop(BoxPointer)
                 +LineBoxGetBoxHeight(BoxPointer)/2;
        BoxXY[2]=LineBoxGetBoxRight(BoxPointer);

        BoxPolygonRotate(2,BoxXY,(PictureBoxs *)BoxPointer);
        BoxPolygonToWindowXY(2,BoxXY);
        WithWidthLine(&SysDc,BoxXY[0],BoxXY[1],BoxXY[2],BoxXY[3],
                       (UserYToWindowY(LineBoxGetBoxHeight(BoxPointer))
                       -UserYToWindowY(0))/2*2,
                       LineBoxGetBoxBorderType(BoxPointer),
                       LineBoxGetBoxArrowType(BoxPointer),0);
     }
  }

  if (!PrintingSign)
  {
     setcolor(SaveColor);
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
  }

  HandleUnlock(ItemGetHandle(BoxItem));
}

void TextBoxDrawTail(HBOX BoxItem, int Hidden)
{
  TextBoxs *BoxPointer;
  ORDINATETYPE BoxXY[2*4];
  int Left,Top,Right,Bottom;
  struct viewporttype TmpViewPort;
  int SaveColor;

  if (PrintingSign || ItemGetFather(BoxItem)!=GlobalCurrentPage)
     return;

  BoxPointer=HandleLock(ItemGetHandle(BoxItem));
  if (BoxPointer==NULL)
     return;
  if(  (TextBoxGetBoxType(BoxPointer)!=TEXTBOX
     && TextBoxGetBoxType(BoxPointer)!=TABLEBOX)
  ||TextBoxGetTextHandle(BoxPointer)==0
  ||TextBoxGetTextLength(BoxPointer)<=1 )
  {
     HandleUnlock(ItemGetHandle(BoxItem));
     return;
  }

  if (!TextBoxGetNextLinkBox(BoxPointer) && TextBoxIsFormatFull(BoxPointer))
  {                                 // draw the text box full sign
     #define FULLEDGELENGTH 8
     int x1,y1,x2,y2;

     MouseHidden();
     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
     SaveColor=getcolor();
     if(Hidden) {
        setwritemode(XOR_PUT);
        setcolor(EGA_WHITE);
     }
     else
        setcolor(EGA_BLACK);

     //x1=TextBoxGetBoxLeft(BoxPointer)+TextBoxGetBoxWidth(BoxPointer)-FULLEDGELENGTH*GlobalPageScale;
     x1=TextBoxGetBoxLeft(BoxPointer)+TextBoxGetBoxWidth(BoxPointer)
        -myWindowXToUserX(FULLEDGELENGTH);
     //y1=TextBoxGetBoxTop(BoxPointer)+TextBoxGetBoxHeight(BoxPointer)-FULLEDGELENGTH*GlobalPageScale;
     y1=TextBoxGetBoxTop(BoxPointer)+TextBoxGetBoxHeight(BoxPointer)
        -myWindowYToUserY(FULLEDGELENGTH);

     x2=TextBoxGetBoxLeft(BoxPointer)+TextBoxGetBoxWidth(BoxPointer);
     y2=TextBoxGetBoxTop(BoxPointer)+TextBoxGetBoxHeight(BoxPointer);
     BoxXY[0]=x1;     BoxXY[1]=y1;
     BoxXY[2]=x2;     BoxXY[3]=y1;
     BoxXY[4]=x2;     BoxXY[5]=y2;
     BoxXY[6]=x1;     BoxXY[7]=y2;
     BoxPolygonRotate(4,BoxXY,(PictureBoxs *)BoxPointer);
     BoxPolygonToWindowXY(4,BoxXY);
     // draw frame
     line(BoxXY[0],BoxXY[1],BoxXY[6],BoxXY[7]);
     line(BoxXY[6],BoxXY[7],BoxXY[4],BoxXY[5]);
     line(BoxXY[4],BoxXY[5],BoxXY[2],BoxXY[3]);
     line(BoxXY[2],BoxXY[3],BoxXY[0],BoxXY[1]);
     // draw cross line
     line(BoxXY[0],BoxXY[1],BoxXY[4],BoxXY[5]);
     line(BoxXY[6],BoxXY[7],BoxXY[2],BoxXY[3]);

     HandleUnlock(ItemGetHandle(BoxItem));
     setwritemode(COPY_PUT);
     setcolor(SaveColor);
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
  }
}

/*
   AttributeSign:
     Bit 0: Virtual / Real frame ( 0 / 1 )
         1: With / Without border rectangle
         2: OR / XOR put
         3: Do not / do fill
         4: Neglect relation between Virtual and XOR
 */
void BoxDrawBorder(HITEM BoxItem,char AttributeSign)
{
  TextBoxs *BoxPointer;
  ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
  int BoxDots,Left,Top,Right,Bottom,i;
  struct viewporttype TmpViewPort;
  int SaveColor;
  unsigned old_style;

  if(BoxItem<=0)
     return;
  BoxPointer=HandleLock(ItemGetHandle(BoxItem));
  if (BoxPointer==NULL)
     return;


  if (ItemGetFather(BoxItem)!=GlobalCurrentPage
      ||(TextBoxGetBoxType(BoxPointer)==LINEBOX&&PrintingSign))
  {
     HandleUnlock(ItemGetHandle(BoxItem));
     return;
  }



  if (PrintingSign)
  {
     if (TextBoxIsDisableFrame(BoxPointer))
     {
        HandleUnlock(ItemGetHandle(BoxItem));
        return;
     }
  }
  else
  {
     if (BoxCanNotEdit(BoxPointer))               //add by zjh 96.9.12 for module file
       {
        HandleUnlock(ItemGetHandle(BoxItem));
        return;
       }
     MouseHidden();
     getviewsettings(&TmpViewPort);
     old_style=getlinestyle();
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
     SaveColor=getcolor();

     if (IsDrawXORBorder(AttributeSign))
     {
        setwritemode(XOR_PUT);
        setcolor(EGA_WHITE);
     }
     else
        setcolor(EGA_BLACK);



     if (IsDrawVirtualBorder(AttributeSign))
     {
    #ifdef __TURBOC__
        struct linesettingstype SaveLineStyle;
        getlinesettings(&SaveLineStyle);
        if (IsDrawXORBorder(AttributeSign)&&(!(AttributeSign&16)))
           setlinestyle(1,0,1);
        else
           setlinestyle(4,0x5555,1);
    #else
        setlinestyle(0x5555);
    #endif
     }
  }


  BoxGetPolygonDrawBorder((Boxs *)BoxPointer,&BoxDots,BoxXY);

  if (BoxGetEditStatus(BoxPointer)==0
  &&BoxGetModuleStatus(BoxPointer)
  &&!PrintingSign)  //By zjh for module
  {
     setcolor(14);
  }

  if(TextBoxIsEmbodyBox(BoxPointer)
  && TextBoxGetBoxType(BoxPointer)==POLYGONPICTUREBOX)
     BoxPolygonMove(BoxDots,BoxXY,(PictureBoxs *)BoxPointer);

  BoxPolygonToWindowXY(BoxDots,BoxXY);

  if (!PrintingSign)
  {
     /*for (i=0;i<BoxDots;i++)
         BorderLine(&BoxXY[2*i],(Boxs *)BoxPointer);
     {
       ORDINATETYPE TmpBorder[2][2];

       TmpBorder[0][0]=BoxXY[2*BoxDots-2];
       TmpBorder[0][1]=BoxXY[2*BoxDots-1];
       TmpBorder[1][0]=BoxXY[0];
       TmpBorder[1][1]=BoxXY[1];
       BorderLine(TmpBorder,(Boxs *)BoxPointer);
     }*/

     if (TextBoxGetBoxType(BoxPointer)==LINEBOX)
     {
        if( TextBoxGetBoxHeight(BoxPointer)==1*SCALEMETER/72
           && (i=TextBoxGetRotateAngle(BoxPointer))!=0)     // it is real line
        {
            if( sin((double)i*PI/180) >= 0 )
            {
               line(BoxXY[2],BoxXY[3],BoxXY[6],BoxXY[7]);
            }
            else
            {
               line(BoxXY[0],BoxXY[1],BoxXY[4],BoxXY[5]);
            }
        }
        else
        if( abs(TextBoxGetBoxWidth(BoxPointer))>SLANTLINEWIDTH
        && abs(TextBoxGetBoxHeight(BoxPointer))>SLANTLINEWIDTH)
        {
            line(BoxXY[0],BoxXY[1],BoxXY[4],BoxXY[5]);
        }
        else
        if( abs(TextBoxGetBoxWidth(BoxPointer))<=SLANTLINEWIDTH )
        {               // vert. line
            i=(BoxXY[0]+BoxXY[2])/2;
            line(i,BoxXY[1],i,BoxXY[5]);
        }
        else
        if( abs(TextBoxGetBoxHeight(BoxPointer))!=1*SCALEMETER/72
        || sin(TextBoxGetRotateAngle(BoxPointer)*PI/180)==0)
        {               // hor. line
            i=(BoxXY[1]+BoxXY[7])/2;
            line(BoxXY[0],i,BoxXY[2],i);
        }
     }
     else
     {
          moveto(BoxXY[0],BoxXY[1]);
          for (i=1;i<BoxDots;i++)
              lineto(BoxXY[2*i],BoxXY[2*i+1]);
          lineto(BoxXY[0],BoxXY[1]);
          if (TextBoxGetBoxType(BoxPointer)>=RECTANGLEPICTUREBOX&&
              TextBoxGetBoxType(BoxPointer)<=POLYGONPICTUREBOX&&
              !PictureBoxGetPictureFileName(BoxPointer)[0])
          {
             //---- draw a Cross_Line ----
             switch(TextBoxGetBoxType(BoxPointer))
             {
               case RECTANGLEPICTUREBOX:
                    line(BoxXY[0],BoxXY[1],BoxXY[4],BoxXY[5]);
                    line(BoxXY[2],BoxXY[3],BoxXY[6],BoxXY[7]);
                    break;
             }  // switch
          } // picture box
     } // line box

 // exit_draw:
    #ifdef __TURBOC__
     if (IsDrawVirtualBorder(AttributeSign))
     {
        setlinestyle(SaveLineStyle.linestyle,
                     SaveLineStyle.upattern,
                     SaveLineStyle.thickness);
     }
    #else
         setlinestyle(old_style);
    #endif

     if (IsDrawXORBorder(AttributeSign))
        setwritemode(COPY_PUT);

     setcolor(SaveColor);
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
  }
  else
  {                     // printing now
     for (i=0;i<BoxDots-1;i++)
         WithWidthLine(&SysDc,BoxXY[2*i],BoxXY[2*i+1],BoxXY[2*i+2],BoxXY[2*i+3],
                       (UserYToWindowY(BoxPointer->BoxBorderWidth+2)
                       -UserYToWindowY(0))/2*2,BoxPointer->BoxBorderType,0,0);
  }

  HandleUnlock(ItemGetHandle(BoxItem));
}

void BoxDraw(HITEM BoxItem,char AttributeSign)
{
  TextBoxs *BoxPointer;

  if (BoxItem<=0)
     return;
  BoxPointer=HandleLock(ItemGetHandle(BoxItem));
  if (BoxPointer==NULL)
     return;
  if (ItemGetFather(BoxItem)!=GlobalCurrentPage
      ||(PrintingSign&&TextBoxIsDisablePrint(BoxPointer)))
  {
     HandleUnlock(ItemGetHandle(BoxItem));
     return;
  }

  if (!PrintingSign)            // ByHance, 96,2.1
     BoxDrawBorder(BoxItem,AttributeSign);

  if(!GlobalNotDisplay)                 // ByHance, 95,12.8
  {
      switch (TextBoxGetBoxType(BoxPointer))
      {
        case TEXTBOX:
             TextBoxDraw(BoxItem);
             break;
        case TABLEBOX:
             TableBoxDraw(BoxItem);
             break;
        case LINEBOX:
             LineBoxDraw(BoxItem);
             break;
        case RECTANGLEPICTUREBOX:
        case CORNERPICTUREBOX:
        case ELIPSEPICTUREBOX:
        case POLYGONPICTUREBOX:
             PictureBoxDisplayPicture(BoxItem);
             break;
      } // switch
  }

  HandleUnlock(ItemGetHandle(BoxItem));
}

void DefaultNewBox(Pages *NewPage,TextBoxs *NewBox)
{
  memset(NewBox,0,sizeof(*NewBox));
  NewBox->BoxLeft=NewPage->MarginLeft;
  NewBox->BoxTop=NewPage->MarginTop;
  NewBox->BoxWidth=NewPage->PageWidth-(NewPage->MarginLeft+NewPage->MarginRight);
  NewBox->BoxHeight=NewPage->PageHeight-(NewPage->MarginTop+NewPage->MarginBottom);
  NewBox->BoxColumn=NewPage->PageColumn;
  NewBox->ColumnDistant=NewPage->ColumnDistant;
  NewBox->TextDistantLeft=DEFAULTBOXTEXTDISTANT;
  NewBox->TextDistantTop=DEFAULTBOXTEXTDISTANT;
  NewBox->TextDistantRight=DEFAULTBOXTEXTDISTANT;
  NewBox->TextDistantBottom=DEFAULTBOXTEXTDISTANT;
}

static int XYInTextBox(ORDINATETYPE X,ORDINATETYPE Y,TextBoxs *Box)
{
  ORDINATETYPE BoxLeft,BoxTop,BoxRight,BoxBottom;
  int RotateAngle;
  ORDINATETYPE RotateAxisX,RotateAxisY;
  ORDINATETYPE TmpX,TmpY;

  BoxLeft=TextBoxGetBoxLeft(Box);
  BoxTop=TextBoxGetBoxTop(Box);
  BoxRight=BoxLeft+TextBoxGetBoxWidth(Box);
  BoxBottom=BoxTop+TextBoxGetBoxHeight(Box);
  RotateAngle=(-1)*TextBoxGetRotateAngle(Box);
  RotateAxisX=TextBoxGetRotateAxisX(Box)+BoxLeft;
  RotateAxisY=TextBoxGetRotateAxisY(Box)+BoxTop;
  Rotate(&TmpX,&TmpY,X,Y,RotateAxisX,RotateAxisY,RotateAngle);

  if ((TmpX>=BoxLeft)&&(TmpX<=BoxRight)&&(TmpY>=BoxTop)&&(TmpY<=BoxBottom))
     return(1);
  else
     return(0);
}

static int XYInPictureBox(ORDINATETYPE X,ORDINATETYPE Y,PictureBoxs *Box)
{
  ORDINATETYPE BoxLeft,BoxTop,BoxRight,BoxBottom;
  int RotateAngle;
  ORDINATETYPE RotateAxisX,RotateAxisY;
  ORDINATETYPE TmpX,TmpY;

  BoxLeft=PictureBoxGetBoxLeft(Box);
  BoxTop=PictureBoxGetBoxTop(Box);
  BoxRight=BoxLeft+PictureBoxGetBoxWidth(Box);
  BoxBottom=BoxTop+PictureBoxGetBoxHeight(Box);
  RotateAngle=(-1)*PictureBoxGetRotateAngle(Box);
  RotateAxisX=BoxLeft+PictureBoxGetRotateAxisX(Box);
  RotateAxisY=BoxTop+PictureBoxGetRotateAxisY(Box);
  Rotate(&TmpX,&TmpY,X,Y,RotateAxisX,RotateAxisY,RotateAngle);

  if ((TmpX>=BoxLeft)&&(TmpX<BoxRight)&&(TmpY>=BoxTop)&&(TmpY<BoxBottom))
     return(1);
  else
     return(0);
}

#ifdef OLD_VERSION_1
    #define XYInTableBox(X,Y,Box)   (XYInTextBox(X,Y,Box))
    #define XYInLineBox(X,Y,Box)    (XYInTextBox(X,Y,Box))
   /*---------------------------
   static int XYInTableBox(ORDINATETYPE X,ORDINATETYPE Y,TextBoxs *Box)
   {
     return(XYInTextBox(X,Y,Box));
   }
   static int XYInLineBox(ORDINATETYPE X,ORDINATETYPE Y,TextBoxs *Box)
   {
     return(XYInTextBox(X,Y,Box));
   }
   -----------------------------------*/
#endif

HBOX FindXYInBox(HPAGE Page,int MouseX,int MouseY)
{
  ORDINATETYPE BoxX,BoxY;
  HBOX MidHBox;
  TextBoxs *MidTextBox;

  if (Page<1)
     return(0);

  BoxX=WindowXToUserX(MouseX);
  BoxY=WindowYToUserY(MouseY);

  MidHBox=ItemGetLastChild(Page);
  while (MidHBox)
  {
    char MidBoxType;

    MidTextBox=HandleLock(ItemGetHandle(MidHBox));
    if (MidTextBox==NULL)
       return(0);
    MidBoxType=TextBoxGetBoxType(MidTextBox);

 #ifdef OLD_VERSION_1
    if (MidBoxType==TEXTBOX)
    {
       if (XYInTextBox(BoxX,BoxY,MidTextBox))
       {
          HandleUnlock(ItemGetHandle(MidHBox));
          break;
       }
    }
    if (MidBoxType==TABLEBOX)
    {
       if (XYInTableBox(BoxX,BoxY,MidTextBox))
       {
          HandleUnlock(ItemGetHandle(MidHBox));
          break;
       }
    }
    if (MidBoxType==LINEBOX)
    {
       if (XYInLineBox(BoxX,BoxY,MidTextBox))
       {
          HandleUnlock(ItemGetHandle(MidHBox));
          break;
       }
    }
    if (MidBoxType==RECTANGLEPICTUREBOX)
    {
       if (XYInPictureBox(BoxX,BoxY,(PictureBoxs *)MidTextBox))
       {
          HandleUnlock(ItemGetHandle(MidHBox));
          break;
       }
    }
    if (MidBoxType==CORNERPICTUREBOX)
    {
       if (XYInPictureBox(BoxX,BoxY,(PictureBoxs *)MidTextBox))
       {
          HandleUnlock(ItemGetHandle(MidHBox));
          break;
       }
    }
    if (MidBoxType==ELIPSEPICTUREBOX)
    {
       if (XYInPictureBox(BoxX,BoxY,(PictureBoxs *)MidTextBox))
       {
          HandleUnlock(ItemGetHandle(MidHBox));
          break;
       }
    }
    if (MidBoxType==POLYGONPICTUREBOX)
    {
       if (XYInPictureBox(BoxX,BoxY,(PictureBoxs *)MidTextBox))
       {
          HandleUnlock(ItemGetHandle(MidHBox));
          break;
       }
    }
 #else
    //if(MidBoxType==TEXTBOX || MidBoxType==TABLEBOX || MidBoxType==LINEBOX)
    if(MidBoxType<=LINEBOX)
    {
       if( !(MidBoxType==TEXTBOX && BoxGetEditStatus(MidTextBox)) )
       if (XYInTextBox(BoxX,BoxY,MidTextBox))
       {
          HandleUnlock(ItemGetHandle(MidHBox));
          break;
       }
    }
    else
    if (MidBoxType<=POLYGONPICTUREBOX)
    {
       if (XYInPictureBox(BoxX,BoxY,(PictureBoxs *)MidTextBox))
       {
          HandleUnlock(ItemGetHandle(MidHBox));
          break;
       }
    }
 #endif   // OLD_VERSION_1

    HandleUnlock(ItemGetHandle(MidHBox));
    MidHBox=ItemGetPrev(MidHBox);
  }     /* while */

  return(MidHBox);
}

void SetTextBoxTextCursor(HBOX HBox)
{
  TextBoxs *MidTextBox;

  if (HBox<=0)
     return;
  MidTextBox=HandleLock(ItemGetHandle(HBox));
  if (MidTextBox==NULL)
     return;

  if (TextBoxGetBoxType(MidTextBox)==TEXTBOX||
      TextBoxGetBoxType(MidTextBox)==TABLEBOX)
  {
     TextCursorSetRotate(
         UserXToWindowX(TextBoxGetRotateAxisX(MidTextBox)+TextBoxGetBoxLeft(MidTextBox)),
         UserYToWindowY(TextBoxGetRotateAxisY(MidTextBox)+TextBoxGetBoxTop(MidTextBox)),
         TextBoxGetRotateAngle(MidTextBox));
  }
  HandleUnlock(ItemGetHandle(HBox));
  return;
}

void SetNewCursor(void)
{
  int TmpX,TmpY;

  if (count)
  {
    TmpX=0;
  }
  if ((GlobalBoxHeadHandle>0)&&(BoxCanEditable(GlobalBoxHeadHandle))
      &&(ItemGetFather(GlobalBoxHeadHandle)==GlobalCurrentPage))
  {
     SetTextBoxTextCursor(GlobalBoxHeadHandle);
     if(!bAtLineFeed && !fInZoom)      // ByHance, 97,5.4
        CursorLocate(GlobalBoxHeadHandle,&GlobalBoxHeadHandle,
                  GlobalTextPosition,&TmpX,&TmpY);
     BoxIsModule("SetNewCursor",GlobalBoxHeadHandle);
  }
}

void BoxEnter(HBOX HBox)
{
  if (HBox>0&&BoxCanEditable(HBox))
     CurrentBoxSetEditable();
  else
     CurrentBoxSetNotEditable();
}

void BoxLeave(HBOX HBox)
{
  int SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;
  if (HBox>0&&BoxCanEditable(HBox))
  {
     TextCursorOff();

     if (BoxIsTableBox(HBox))                   // ByHance, 97,8.11
        CancelCellBlock(HBox);

     CancelBlock(HBox,&GlobalTextBlockStart,&GlobalTextBlockEnd);
     if (GlobalTextPosition)
     {
        UndoInsertCursorGoto(GlobalTextPosition);
        GlobalTextPosition=0;
     }
  }
  UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
}

int BoxSelect(HPAGE Page,int MouseX,int MouseY)
{
  HBOX SelectBox;
  int SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;
  SelectBox=FindXYInBox(Page,MouseX,MouseY);
  if (SelectBox!=GlobalBoxHeadHandle)
  {
     if (GlobalBoxTool==IDX_INPUTBOX)
     {
        BoxLeave(GlobalBoxHeadHandle);
        BoxEnter(SelectBox);
     }
     if (GlobalBoxHeadHandle>0)
     {
        if ((GlobalTextBlockStart<GlobalTextBlockEnd
            ||GlobalTableBlockStart>=0||GlobalTableBlockStart>=0)
            &&BoxCanEditable(GlobalBoxHeadHandle))
           CancelBlock(GlobalBoxHeadHandle,&GlobalTextBlockStart,&GlobalTextBlockEnd);
        BoxDrawBorder(GlobalBoxHeadHandle,DRAWXORBORDER|DRAWBORDERWITHRECATNGLE);
        BoxDrawBorder(GlobalBoxHeadHandle,DRAWVIRTUALBORDOR);
     }
     UndoInsertBoxSelect(GlobalBoxHeadHandle,GlobalTextPosition,GlobalTableCell);
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  if (!SelectBox)
  {
     GlobalBoxHeadHandle=0;
     return(0);                        // No box is selected
  }
  if (SelectBox!=GlobalBoxHeadHandle)
  {                                    // Select a new unselected box
     SetTextBoxTextCursor(SelectBox);
     GlobalBoxHeadHandle=SelectBox;
     BoxDrawBorder(GlobalBoxHeadHandle,DRAWBORDERWITHRECATNGLE);
     return(1);
  }
  return(2);                           // Select a selected box
}

static void SetTextInvalidArea(HTEXTBOX HTextBox,HBOX HInValidBox,
 //   int *InvalidPolygons,int *InvalidEdges,ORDINATETYPE *InvalidBoxXY)
   int *InvalidPolygons,short *InvalidEdges,short *InvalidBoxXY)
{
  Boxs *Box;
  TextBoxs *TextBox;
  ORDINATETYPE Left,Top,Right,Bottom;
  ORDINATETYPE BoxBorderLeft,BoxBorderTop,BoxBorderRight,BoxBorderBottom;
  ORDINATETYPE RotateAxisX,RotateAxisY;
  ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
  int BoxDots;
  int i;
  int RotateAngle;

  if(HInValidBox<=0 || HTextBox<=0)
     return;

  Box=HandleLock(ItemGetHandle(HInValidBox));
  if (Box==NULL) return;

  if (BoxGetModuleStatus((TextBoxs*)Box)||IsModule)            //By zjh for fra 9.12
    {
      HandleUnlock(ItemGetHandle(HInValidBox));
      return ;
    }

  BoxGetPolygonBorder(Box,&BoxDots,BoxXY);  // get InvalidBox's edge to BoxXY
  HandleUnlock(ItemGetHandle(HInValidBox));

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)  return;

  if (BoxGetModuleStatus(TextBox)||IsModule)            //By zjh for fra 9.12
    {
      HandleUnlock(ItemGetHandle(HTextBox));
      return ;
    }

  Left=TextBoxGetBoxLeft(TextBox);      // get Box's Rect to (left,top...)
  Top=TextBoxGetBoxTop(TextBox);
  Right=TextBoxGetBoxWidth(TextBox)+Left;
  Bottom=TextBoxGetBoxHeight(TextBox)+Top;

           // rotate InvalidBox's edge using Box's rotateAngle
  RotateAngle=TextBoxGetRotateAngle(TextBox);
  if (RotateAngle)
  {
     RotateAxisX=TextBoxGetRotateAxisX(TextBox)+TextBoxGetBoxLeft(TextBox);
     RotateAxisY=TextBoxGetRotateAxisY(TextBox)+TextBoxGetBoxTop(TextBox);
     for (i=0;i<BoxDots;i++)
         Rotate(&BoxXY[2*i],&BoxXY[2*i+1],BoxXY[2*i],BoxXY[2*i+1],
                RotateAxisX,RotateAxisY,-RotateAngle);
  }

  HandleUnlock(ItemGetHandle(HTextBox));

         // get InvalidBox's Rect
  PolygonGetMinRectangle(BoxDots,BoxXY,&BoxBorderLeft,&BoxBorderTop,
                         &BoxBorderRight,&BoxBorderBottom);

  if (RectangleIsInRectangle(Left,Top,Right,Bottom,BoxBorderLeft,
                             BoxBorderTop,BoxBorderRight,BoxBorderBottom))
  {
     int Sum,j,total;

     total=*InvalidPolygons;
     for (i=Sum=0;i<total;i++)
         Sum+=InvalidEdges[i];

     if(Sum+BoxDots>=5*MAXPOLYGONNUMBER)        // ByHance, 96,4.5
         return;

     InvalidEdges[total]=BoxDots;
     (*InvalidPolygons)++;

     total=2*Sum;
     for (j=0;j<BoxDots;j++)
     {
         InvalidBoxXY[total+2*j]=BoxXY[2*j]-Left;
         InvalidBoxXY[total+2*j+1]=BoxXY[2*j+1]-Top;
     }
  }
}

static void SetTextAllInvalidArea(HTEXTBOX HTextBox)
{
  HBOX MidHBox;
  TextBoxs *TextBox;
  int *InvalidPolygons;
  // int *InvalidEdges;
  // ORDINATETYPE *InvalidBoxXY;        // ByHance, 96,4.5
  short *InvalidBoxXY, *InvalidEdges;

  if(HTextBox<=0)
     return;
  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return;

  if (BoxGetModuleStatus(TextBox)||IsModule)            //By zjh for fra 9.12
    {
      HandleUnlock(ItemGetHandle(HTextBox));
      return ;
    }

  InvalidPolygons=&TextBoxGetInvalidPolygons(TextBox);
  InvalidEdges=TextBoxGetInvalidEdges(TextBox);
  InvalidBoxXY=TextBoxGetInvalidBoxXY(TextBox);
  *InvalidPolygons=0;

  MidHBox=BoxGetNext(HTextBox);
  while (MidHBox)
  {
     SetTextInvalidArea(HTextBox,MidHBox,InvalidPolygons,
                          InvalidEdges,InvalidBoxXY);
     MidHBox=BoxGetNext(MidHBox);
  }

  HandleUnlock(ItemGetHandle(HTextBox));
}

void BoxChange(HBOX HBox,HPAGE HPage)
{
  HBOX MidHBox,MidHBox2;
  Boxs *BoxPointer1,*BoxPointer2;
  int HBoxDots1,HBoxDots2;
  ORDINATETYPE BoxBorderLeft1,BoxBorderTop1,BoxBorderRight1,BoxBorderBottom1;
  ORDINATETYPE BoxBorderLeft2,BoxBorderTop2,BoxBorderRight2,BoxBorderBottom2;
  ORDINATETYPE BoxXY1[2*MAXPOLYGONNUMBER],BoxXY2[2*MAXPOLYGONNUMBER];

  if(HBox<=0 || HPage<=0)
     return;
  BoxPointer1=HandleLock(ItemGetHandle(HBox));
  if (BoxPointer1==NULL)
     return;

  BoxGetPolygonBorder(BoxPointer1,&HBoxDots1,BoxXY1);
  PolygonGetMinRectangle(HBoxDots1,BoxXY1,&BoxBorderLeft1,&BoxBorderTop1,
                         &BoxBorderRight1,&BoxBorderBottom1);

  MidHBox=ItemGetChild(HPage);     // try from first box in this page
  while (MidHBox>0)
  {
    BoxPointer2=HandleLock(ItemGetHandle(MidHBox));
    if (BoxPointer2==NULL)
    {
       HandleUnlock(ItemGetHandle(HBox));
       return;
    }
    if(TextBoxGetBoxType((TextBoxs *)BoxPointer2)==TEXTBOX)
    {
       BoxGetPolygonBorder(BoxPointer2,&HBoxDots2,BoxXY2);
       PolygonGetMinRectangle(HBoxDots2,BoxXY2,&BoxBorderLeft2,&BoxBorderTop2,
                              &BoxBorderRight2,&BoxBorderBottom2);
       if(RectangleIsInRectangle(BoxBorderLeft1,BoxBorderTop1,BoxBorderRight1,
             BoxBorderBottom1,BoxBorderLeft2,BoxBorderTop2,
             BoxBorderRight2,BoxBorderBottom2) )
       {
          SetTextAllInvalidArea(MidHBox);
          InitRL(BoxPointer2);

          if (TextBoxGetPrevLinkBox((TextBoxs *)BoxPointer2))
             MidHBox2=GetFirstLinkBox(MidHBox);
          else
             MidHBox2=MidHBox;
          FormatAll(MidHBox2);
       }
    }
    HandleUnlock(ItemGetHandle(MidHBox));

    MidHBox=BoxGetNext(MidHBox);
  }

  HandleUnlock(ItemGetHandle(HBox));
}

void BoxChangeFrom(HBOX HBox)
{
  HBOX MidHBox;

  MidHBox=HBox;
  while (MidHBox)
  {
    BoxChange(MidHBox,ItemGetFather(MidHBox));
    MidHBox=ItemGetNext(MidHBox);
  }
}

void BoxChangeAll(HPAGE HPage)
{
  if(HPage>0)
     BoxChangeFrom(ItemGetChild(HPage));
}

void BoxBar(TextBoxs *TextBox,int Left,int Top,int Right,int Bottom)
{
  ORDINATETYPE  Edges[8];
  short XY[8];
  int RotateAxisX,RotateAxisY,RotateAngle;
  struct viewporttype TmpViewPort;
  int WindowLeft,WindowTop,WindowRight,WindowBottom;
  int i;

  if (PrintingSign || TextBox==NULL)
     return;

  if (TextBoxGetBoxType(TextBox)==TEXTBOX||TextBoxGetBoxType(TextBox)==TABLEBOX)
  {
     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&WindowLeft,&WindowTop,&WindowRight,&WindowBottom);
     setviewport(WindowLeft,WindowTop,WindowRight,WindowBottom,1);
     Left+=TextBoxGetBoxLeft(TextBox);
     Right+=TextBoxGetBoxLeft(TextBox);
     Top+=TextBoxGetBoxTop(TextBox);
     Bottom+=TextBoxGetBoxTop(TextBox);

     Edges[0]=Left;
     Edges[1]=Top;
     Edges[2]=Right;
     Edges[3]=Top;
     Edges[4]=Right;
     Edges[5]=Bottom;
     Edges[6]=Left;
     Edges[7]=Bottom;

     RotateAngle=TextBoxGetRotateAngle(TextBox);

     if (RotateAngle)
     {
        RotateAxisX=TextBoxGetRotateAxisX(TextBox)+TextBoxGetBoxLeft(TextBox);
        RotateAxisY=TextBoxGetRotateAxisY(TextBox)+TextBoxGetBoxTop(TextBox);

        for (i=0;i<4;i++)
            Rotate((ORDINATETYPE *)&Edges[2*i],(ORDINATETYPE *)&Edges[2*i+1],
                   Edges[2*i],Edges[2*i+1],
                   RotateAxisX,RotateAxisY,RotateAngle);
     }

     for (i=0;i<4;i++)
     {
         Edges[2*i]=UserXToWindowX(Edges[2*i]);
         Edges[2*i+1]=UserYToWindowY(Edges[2*i+1]);
     }
//     setfillstyle(1,EGA_WHITE);
     setcolor(EGA_WHITE);
     if (RotateAngle) {
        // fillpoly(4,Edges);
        for(i=0;i<8;i++) XY[i]=(short)Edges[i];
        fillpoly(4,XY);
     } else
        bar(Edges[0],Edges[1],Edges[2],Edges[5]);
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
  }
}

void FindXYInCell(HBOX HTableBox,ORDINATETYPE BoxX,ORDINATETYPE BoxY,int *FindCell)
{
  if (BoxIsTableBox(HTableBox))
  {
     FormBoxs *FormBox;

     FormBox=HandleLock(ItemGetHandle(HTableBox));
     if (TableBoxGetRotateAngle(FormBox))
        Rotate(&BoxX,&BoxY,BoxX,BoxY,TableBoxGetRotateAxisX(FormBox),
               TableBoxGetRotateAxisY(FormBox),
               TableBoxGetRotateAngle(FormBox));
     BoxX-=TableBoxGetBoxLeft(FormBox);
     BoxY-=TableBoxGetBoxTop(FormBox);
     *FindCell=FBCellofXY(HTableBox,BoxX,BoxY);
     HandleUnlock(ItemGetHandle(HTableBox));
  }
}

void BoxSelectOrMove(HBOX HBox,unsigned long Mouse1,unsigned long Mouse2,
                    unsigned long MidMouse,int *BlockStart,int *BlockEnd)
{
  TextBoxs *TextBox;
  int MoveX,MoveY,x1,y1,x2,y2;
  HBOX HMidMouseInBox,HMouseInBox2;    // Suppose: HMouseInBox1 == HBox

  if(HBox<=0)
     return;

  TextBox=(TextBoxs *)HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return;

  TextCursorOff();
  if (TextBoxGetBoxType(TextBox)==TEXTBOX)
  {                                    // select text
     Mouse1=WindowToUserWindow(1,Mouse1);
     Mouse2=WindowToUserWindow(1,Mouse2);

   lbl_select_text:
     MidMouse=WindowToUserWindow(1,MidMouse);
                                       // Which box mouse point to
     MoveX=(short)MAKEHI(MidMouse);
     MoveY=(short)MAKELO(MidMouse);
     HMidMouseInBox=FindXYInBox(GlobalCurrentPage,MoveX,MoveY);
     if( !HMidMouseInBox || !BoxIsTextBox(HMidMouseInBox)
     || !TextBoxIsInLink(HBox,HMidMouseInBox) )
        HMidMouseInBox=HBox;

     x2=(short)MAKEHI(Mouse2);
     y2=(short)MAKELO(Mouse2);
     HMouseInBox2=FindXYInBox(GlobalCurrentPage,x2,y2);
     if( !HMouseInBox2 || !BoxIsTextBox(HMouseInBox2)
     || !TextBoxIsInLink(HBox,HMouseInBox2) )
        HMouseInBox2=HMidMouseInBox;

     x1=(short)MAKEHI(Mouse1);
     y1=(short)MAKELO(Mouse1);
     if (*BlockStart>=*BlockEnd)       // Both define block start and block end
     {
          BoxGetSelectBlock(HBox,x1,y1,BlockStart);
          BoxGetSelectBlock(HMouseInBox2,x2,y2,BlockEnd);
          GlobalTextPosition=*BlockEnd;
          if (*BlockStart>*BlockEnd)       // Case 1:
          {                                //      Mouse1
               int Tmp;                    //
                                           //        Mouse2 (MidMouse)
               Tmp=*BlockStart;            // Case 2:
               *BlockStart=*BlockEnd;      //      Mouse2 (MidMouse)
               *BlockEnd=Tmp;              //
          }                                //        Mouse1

          DisplayBlock(HBox,*BlockStart,*BlockEnd);
     }
     else
     {
          int TmpBlockEnd;
          int TmpLastBlock;

          BoxGetSelectBlock(HMidMouseInBox,MoveX,MoveY,&TmpLastBlock);
          BoxGetSelectBlock(HMouseInBox2,x2,y2,&TmpBlockEnd);

          if (TmpBlockEnd==*BlockStart||TmpBlockEnd==*BlockEnd)
          {
               HandleUnlock(ItemGetHandle(HBox));
               return;
          } else { GlobalTextPosition=TmpBlockEnd; }

          if (TmpBlockEnd>*BlockEnd)     // Only define block end
          {                              // BlockStart--BlockEnd < Mouse2
               if (TmpLastBlock>*BlockStart)
               {                           // Case 3:
                                           //      Mouse1
                                           //
                                           //        MidMouse
                                           //
                                           //          Mouse2
                  DisplayBlock(HBox,*BlockEnd,TmpBlockEnd);
               }
               else
               {                           // Case 4:
                                           //      MidMouse
                                           //
                                           //        Mouse1
                                           //
                                           //          Mouse2
                  DisplayBlock(HBox,*BlockStart,TmpBlockEnd);
                  *BlockStart=*BlockEnd;
               }
               *BlockEnd=TmpBlockEnd;
          }
          else                           // Only define block start

          if (TmpBlockEnd<*BlockStart)// Mouse2 < BlockStart--BlockEnd
          {
               if (TmpLastBlock<*BlockEnd)
               {                        // Case 5:
                                        //      Mouse2
                                        //
                                        //        MidMouse
                                        //
                                        //          Mouse1
                    DisplayBlock(HBox,TmpBlockEnd,*BlockStart);
               }
               else
               {                        // Case 6:
                                        //      Mouse2
                                        //
                                        //        Mouse1
                                        //
                                        //          MidMouse
                    DisplayBlock(HBox,TmpBlockEnd,*BlockEnd);
                    *BlockEnd=*BlockStart;
               }
               *BlockStart=TmpBlockEnd;
          }
          else                        // Define block start or block end
                                    // BlockStart < Mouse2 < BlockEnd
          if (TmpLastBlock<*BlockEnd)
          {                        // Case 7:
                                   //      MidMouse
                                   //
                                   //        Mouse2
                                   //
                                   //          Mouse1
               DisplayBlock(HBox,*BlockStart,TmpBlockEnd);
               *BlockStart=TmpBlockEnd;
          }
          else
          {                        // Case 8:
                                   //      Mouse1
                                   //
                                   //        Mouse2
                                   //
                                   //          MidMouse
               DisplayBlock(HBox,TmpBlockEnd,*BlockEnd);
               *BlockEnd=TmpBlockEnd;
          }
     }
     CursorLocate(HBox,&HBox,GlobalTextPosition,&MoveX,&MoveY);
  }
  else
  if (TextBoxGetBoxType(TextBox)==TABLEBOX)
  {                                 // select text or select table cell
     int StartCell,EndCell;

     Mouse1=WindowToUserWindow(1,Mouse1);
     Mouse2=WindowToUserWindow(1,Mouse2);
/**-------- ByHance
     FindXYInCell(HBox,WindowXToUserX((short)MAKEHI(Mouse1)),
                  WindowYToUserY((short)MAKELO(Mouse1)),&StartCell);
     FindXYInCell(HBox,WindowXToUserX((short)MAKEHI(Mouse2)),
                  WindowYToUserY((short)MAKELO(Mouse2)),&EndCell);
------*/
     x1=(short)MAKEHI(Mouse1);
     y1=(short)MAKELO(Mouse1);
     x2=(short)MAKEHI(Mouse2);
     y2=(short)MAKELO(Mouse2);
     FindXYInCell(HBox,WindowXToUserX(x1),
                  WindowYToUserY(y1),&StartCell);
     FindXYInCell(HBox,WindowXToUserX(x2),
                  WindowYToUserY(y2),&EndCell);
     if (StartCell>EndCell)
     {
        int Tmp;

        Tmp=StartCell;
        StartCell=EndCell;
        EndCell=Tmp;
     }

     if ((StartCell!=EndCell)&&(StartCell>=0)&&(EndCell>=0)&&
         (StartCell!=GlobalTableBlockStart||EndCell!=GlobalTableBlockEnd))
     {                              // Select Cell Block
        CancelBlock(HBox,&GlobalTextBlockStart,&GlobalTextBlockEnd); //97,8.6
        CancelCellBlock(HBox);
        GlobalTableBlockStart=StartCell;
        GlobalTableBlockEnd=EndCell;
        // *BlockStart=TableCellGetTextHead(HBox,StartCell);
        // *BlockEnd=TableCellGetTextHead(HBox,EndCell)+TableCellGetTextLength(HBox,EndCell);
        FindXYInCell(HBox,WindowXToUserX(x2),         // Byhance
                     WindowYToUserY(y2),&GlobalTableCell);
        if (GlobalTextPosition==StartCell)
           GlobalTextPosition=TableCellGetTextHead(HBox,StartCell);
        else
           GlobalTextPosition=TableCellGetTextHead(HBox,EndCell)+TableCellGetTextLength(HBox,EndCell);
        DisplayCellBlock(HBox,GlobalTableBlockStart,GlobalTableBlockEnd);
     }
     else
     if (StartCell==EndCell && GlobalTableBlockStart==GlobalTableBlockEnd)
     {                              // Select Text Block
        CancelCellBlock(HBox);          // ByHance, 97,8.11
        // StartCell=0;
        goto lbl_select_text;
     }
  }
  else                              // move image
  if( TextBoxGetBoxType(TextBox)>=RECTANGLEPICTUREBOX
   && TextBoxGetBoxType(TextBox)<=POLYGONPICTUREBOX)
  {
    /*----- modify ByHance, add:: "(short)" -----*/
     //MoveX=((short)MAKEHI(Mouse2)-(short)MAKEHI(MidMouse))*GlobalPageScale;
     //MoveY=((short)MAKELO(Mouse2)-(short)MAKELO(MidMouse))*GlobalPageScale;
     MoveX=myWindowXToUserX((short)MAKEHI(Mouse2)-(short)MAKEHI(MidMouse));
     MoveY=myWindowYToUserY((short)MAKELO(Mouse2)-(short)MAKELO(MidMouse));
     PictureBoxMovePicture(HBox,MoveX,MoveY);
  }

  HandleUnlock(ItemGetHandle(HBox));
} /* BoxSelectOrMove  */

int BoxIsPictureBox(HBOX HBox)
{
  PictureBoxs *PictureBox;
  char BoxType;

  if (HBox<=0)
     return(0);
  PictureBox=HandleLock(ItemGetHandle(HBox));
  if (PictureBox==NULL)
     return(0);
  BoxType=PictureBoxGetBoxType(PictureBox);
  HandleUnlock(ItemGetHandle(HBox));
  if (BoxType>=RECTANGLEPICTUREBOX&&BoxType<=POLYGONPICTUREBOX)
     return(BoxType-RECTANGLEPICTUREBOX+1);
  else
     return(0);
}

int BoxIsTextBox(HBOX HBox)
{
  TextBoxs *TextBox;
  char BoxType;

  if (HBox<=0)
     return(0);
  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(0);
  BoxType=TextBoxGetBoxType(TextBox);
  HandleUnlock(ItemGetHandle(HBox));
  if (BoxType==TEXTBOX)
     return(1);
  else
     return(0);
}

int BoxIsTableBox(HBOX HBox)
{
  TextBoxs *TextBox;
  char BoxType;

  if (HBox<=0)
     return(0);
  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(0);
  BoxType=TextBoxGetBoxType(TextBox);
  HandleUnlock(ItemGetHandle(HBox));
  if (BoxType==TABLEBOX)
     return(1);
  else
     return(0);
}

int BoxCanEditable(HBOX HBox)
{
  TextBoxs *TextBox;
  char BoxType;
  int  fCanEdit;

  if (HBox<=0)
     return(0);
  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(0);
  BoxType=TextBoxGetBoxType(TextBox);
  fCanEdit=!BoxGetEditStatus(TextBox);
  HandleUnlock(ItemGetHandle(HBox));
  if( (BoxType==TEXTBOX||BoxType==TABLEBOX) &&  fCanEdit)
     return(1);
  else
     return(0);
}

int BoxIsLineBox(HBOX HBox)
{
  TextBoxs *TextBox;
  char BoxType;

  if (HBox<=0)
     return(0);
  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(0);
  BoxType=TextBoxGetBoxType(TextBox);
  HandleUnlock(ItemGetHandle(HBox));
  if (BoxType==LINEBOX)
     return(1);
  else
     return(0);
}

#define TextBoxIsLinkHead(TT) (TextBoxGetPrevLinkBox(TT)==NULL)
#define TextBoxIsLinkTail(TT) (TextBoxGetNextLinkBox(TT)==NULL)

void SetAllLinkBoxTextHandle(HTEXTBOX HTextBox)
{
  HTEXTBOX MidHTextBox;
  TextBoxs *TextBox,*MidTextBox;
  HANDLE TextHandle;
  int TextLength;
  int TextBlockLength;

  if(HTextBox<=0)
     return;
  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return;

  TextHandle=TextBoxGetTextHandle(TextBox);
  TextLength=TextBoxGetTextLength(TextBox);
  TextBlockLength=TextBoxGetBlockLength(TextBox);

  MidHTextBox=TextBoxGetPrevLinkBox(TextBox);
  while (MidHTextBox)
  {
    HANDLE TmpHBox;

    MidTextBox=HandleLock(ItemGetHandle(MidHTextBox));
    if (MidTextBox==NULL)
    {
       HandleUnlock(ItemGetHandle(HTextBox));
       return;
    }
    TextBoxSetTextHandle(MidTextBox,TextHandle);
    TextBoxSetTextLength(MidTextBox,TextLength);
    TextBoxSetBlockLength(MidTextBox,TextBlockLength);
    TmpHBox=TextBoxGetPrevLinkBox(MidTextBox);
    HandleUnlock(ItemGetHandle(MidHTextBox));
    MidHTextBox=TmpHBox;
  }

  MidHTextBox=TextBoxGetNextLinkBox(TextBox);
  while (MidHTextBox)
  {
    HANDLE TmpHBox;

    MidTextBox=HandleLock(ItemGetHandle(MidHTextBox));
    if (MidTextBox==NULL)
    {
       HandleUnlock(ItemGetHandle(HTextBox));
       return;
    }
    TextBoxSetTextHandle(MidTextBox,TextHandle);
    TextBoxSetTextLength(MidTextBox,TextLength);
    TextBoxSetBlockLength(MidTextBox,TextBlockLength);
    TmpHBox=TextBoxGetNextLinkBox(MidTextBox);
    HandleUnlock(ItemGetHandle(MidHTextBox));
    MidHTextBox=TmpHBox;
  }

  HandleUnlock(ItemGetHandle(HTextBox));
}

void SetAllUnLinkBoxTextHandle(HTEXTBOX HTextBox)
{
  HTEXTBOX MidHTextBox;
  TextBoxs *TextBox,*MidTextBox;
  HANDLE TextHandle;
  int TextLength;
  int TextBlockLength;

  if(HTextBox<=0)
     return;
  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return;

  TextHandle=0;
  TextLength=0;
  TextBlockLength=0;

  MidHTextBox=HTextBox;
  //TextBoxGetNextLinkBox(TextBox);
  while (MidHTextBox)
  {
    HANDLE TmpHBox;

    MidTextBox=HandleLock(ItemGetHandle(MidHTextBox));
    if (MidTextBox==NULL)
    {
       HandleUnlock(ItemGetHandle(HTextBox));
       return;
    }
    TextBoxSetTextHandle(MidTextBox,TextHandle);
    TextBoxSetTextLength(MidTextBox,TextLength);
    TextBoxSetBlockLength(MidTextBox,TextBlockLength);
    TmpHBox=TextBoxGetNextLinkBox(MidTextBox);
    HandleUnlock(ItemGetHandle(MidHTextBox));
    MidHTextBox=TmpHBox;
  }

  HandleUnlock(ItemGetHandle(HTextBox));
}

int GetFirstLinkBox(HTEXTBOX HTextBox)
{
  TextBoxs *TextBox;
  HTEXTBOX HTextMidBox,TmpHBox;

  HTextMidBox=HTextBox;
  while (HTextMidBox>0)
  {
    TextBox=HandleLock(ItemGetHandle(HTextMidBox));
    if (TextBox==NULL)
       return(0);
    TmpHBox=TextBoxGetPrevLinkBox(TextBox);
    HandleUnlock(ItemGetHandle(HTextMidBox));
    if (!TmpHBox)
       break;
    else
       HTextMidBox=TmpHBox;
  }
  return(HTextMidBox);
}

int TextBoxInWindow(HTEXTBOX hBox)
{
  ORDINATETYPE BoxXY[10];
  int BoxDots;
  ORDINATETYPE BoxLeft,BoxTop,BoxRight,BoxBottom;
  int WindowLeft,WindowTop,WindowRight,WindowBottom;
  TextBoxs *TextBox;

  if(hBox<=0)
     return(FALSE);
  TextBox=HandleLock(ItemGetHandle(hBox));
  if (TextBox==NULL)
     return(FALSE);

  if (ItemGetFather(hBox)!=GlobalCurrentPage)
  {
     HandleUnlock(ItemGetHandle(hBox));
     return(FALSE);
  }

  if (PrintingSign)
  {
     Pages *MidPage;

     MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
     if (MidPage==NULL)
        return(0);
     WindowRight=PageGetPageWidth(MidPage);
     WindowBottom=PageGetPageHeight(MidPage);
     HandleUnlock(ItemGetHandle(GlobalCurrentPage));
  }
  else
  {
     WindowGetRect(1,&WindowLeft,&WindowTop,&WindowRight,&WindowBottom);
     WindowRight-=WindowLeft;
     WindowBottom-=WindowTop;
  }

  WindowLeft=0;
  WindowTop=0;
  BoxGetPolygonBorder((Boxs *)TextBox,&BoxDots,BoxXY);
  PolygonGetMinRectangle(BoxDots,BoxXY,&BoxLeft,&BoxTop,&BoxRight,&BoxBottom);
  BoxLeft=UserXToWindowX(BoxLeft);
  BoxTop=UserYToWindowY(BoxTop);
  BoxRight=UserXToWindowX(BoxRight);
  BoxBottom=UserYToWindowY(BoxBottom);
  HandleUnlock(ItemGetHandle(hBox));
  return(RectangleIsInRectangle(BoxLeft,BoxTop,BoxRight,BoxBottom,
                                WindowLeft,WindowTop,WindowRight,WindowBottom));
}

int TextBoxRectInWindow(HTEXTBOX hBox,int Left,int Top,int Right,int Bottom)
{
  int WindowLeft,WindowTop,WindowRight,WindowBottom;
  ORDINATETYPE AreaEdges[10];
  TextBoxs *TextBox;
  int RotateAngle,RotateAxisX,RotateAxisY;

  if(hBox<=0)
     return(FALSE);
  TextBox=HandleLock(ItemGetHandle(hBox));
  if (TextBox==NULL)
     return(FALSE);

  if (ItemGetFather(hBox)!=GlobalCurrentPage)
  {
     HandleUnlock(ItemGetHandle(hBox));
     return(FALSE);
  }

  if (PrintingSign)
  {
     if (GlobalRorate90)
     {
     WindowLeft=myDC.left;
     WindowTop=myDC.top;
     WindowRight=myDC.right;
     WindowBottom=myDC.bottom;
     }
     else
     {
     WindowLeft=SysDc.left;
     WindowTop=SysDc.top;
     WindowRight=SysDc.right;
     WindowBottom=SysDc.bottom;
     }
  }
  else
  {
     WindowGetRect(1,&WindowLeft,&WindowTop,&WindowRight,&WindowBottom);
     WindowRight-=WindowLeft;
     WindowBottom-=WindowTop;
     WindowLeft=0;
     WindowTop=0;
  }

  RotateAngle=TextBoxGetRotateAngle(TextBox);
  RotateAxisX=TextBoxGetRotateAxisX(TextBox)+TextBoxGetBoxLeft(TextBox);
  RotateAxisY=TextBoxGetRotateAxisY(TextBox)+TextBoxGetBoxTop(TextBox);
  Left=Left+TextBoxGetBoxLeft(TextBox);
  Top=Top+TextBoxGetBoxTop(TextBox);
  Right=Right+TextBoxGetBoxLeft(TextBox);
  Bottom=Bottom+TextBoxGetBoxTop(TextBox);

  if (RotateAngle)
  {
     Rotate(&AreaEdges[0],&AreaEdges[1],Left,Top,
            RotateAxisX,RotateAxisY,RotateAngle);
     Rotate(&AreaEdges[2],&AreaEdges[3],Right,Top,
            RotateAxisX,RotateAxisY,RotateAngle);
     Rotate(&AreaEdges[4],&AreaEdges[5],Right,Bottom,
            RotateAxisX,RotateAxisY,RotateAngle);
     Rotate(&AreaEdges[6],&AreaEdges[7],Left,Bottom,
            RotateAxisX,RotateAxisY,RotateAngle);
  }
  else
  {
     AreaEdges[0]=Left;
     AreaEdges[1]=Top;
     AreaEdges[2]=Right;
     AreaEdges[3]=Top;
     AreaEdges[4]=Right;
     AreaEdges[5]=Bottom;
     AreaEdges[6]=Left;
     AreaEdges[7]=Bottom;
  }
  PolygonGetMinRectangle(4,AreaEdges,(ORDINATETYPE *)&Left,
                         (ORDINATETYPE *)&Top,(ORDINATETYPE *)&Right,
                         (ORDINATETYPE *)&Bottom);
  Left=UserXToWindowX(Left);
  Top=UserYToWindowY(Top);
  Right=UserXToWindowX(Right);
  Bottom=UserYToWindowY(Bottom);
  HandleUnlock(ItemGetHandle(hBox));
  return(RectangleIsInRectangle(WindowLeft,WindowTop,WindowRight,WindowBottom,
                                Left,Top,Right,Bottom));
}

int TextBoxLinkCycle(HTEXTBOX HPrevLinkBox,HTEXTBOX HNextLinkBox)
{
  HPrevLinkBox=GetFirstLinkBox(HPrevLinkBox);
  HNextLinkBox=GetFirstLinkBox(HNextLinkBox);
  return(HPrevLinkBox==HNextLinkBox);
}

void DrawBoxInBox(HBOX insertBox,HBOX HBox,int x,int y)
{
  TextBoxs *ChildBox,*FatherBox;
  int Left,Top,RotateAngle,RotateAxisX,RotateAxisY;

  if(HBox<=0||insertBox<=0)
     return;
  FatherBox=HandleLock(ItemGetHandle(HBox));
  if (FatherBox==NULL)
     return;

  Left=TextBoxGetBoxLeft(FatherBox);
  Top=TextBoxGetBoxTop(FatherBox);
  RotateAngle=TextBoxGetRotateAngle(FatherBox);
  RotateAxisX=TextBoxGetRotateAxisX(FatherBox);
  RotateAxisY=TextBoxGetRotateAxisY(FatherBox);
  HandleUnlock(ItemGetHandle(HBox));

  ChildBox=HandleLock(ItemGetHandle(insertBox));
  if (ChildBox==NULL)
     return;

  TextBoxSetBoxLeft(ChildBox,Left+x);
  TextBoxSetBoxTop(ChildBox,Top+y);
  TextBoxSetRotateAngle(ChildBox,RotateAngle);
  TextBoxSetRotateAxisX(ChildBox,RotateAxisX-x);
  TextBoxSetRotateAxisY(ChildBox,RotateAxisY-y);
  HandleUnlock(ItemGetHandle(insertBox));

  ItemSetFather(insertBox,ItemGetFather(HBox));
  BoxDraw(insertBox,GlobalBoxHeadHandle==HBox);
}

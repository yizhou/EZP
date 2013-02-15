/*-------------------------------------------------------------------
* Name: itemsc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

int ItemConstruct(void)
{
  HITEM MidItem;

  for (MidItem=1;MidItem<MAXITEMS;MidItem++)
      if (ItemCanUse(MidItem))
         break;

  if (MidItem>=MAXITEMS)
     Error(TOOMANYITEMS);
  return(MidItem);
}

int ItemDestruct(HITEM ItemNumber)
{
  if(ItemNumber>MAXITEMS)
     return(TOOMANYITEMS);
  ReturnOK();
}

HITEM ItemGetLastChild(HITEM ItemNumber)
{
  HITEM MidItem;

  if(ItemNumber>MAXITEMS)
     return(TOOMANYITEMS);
  MidItem=ItemGetChild(ItemNumber);
  if (MidItem==0)
     return(0);

  while (!ItemIsNextless(MidItem))
    MidItem=ItemGetNext(MidItem);

  return(MidItem);
}

HITEM ItemInsert(Items *InsertItem,HITEM FatherItem,HITEM PrevItem)
{
  short Result;
  HITEM ItemNumber;

  if ((Result=ItemConstruct())<OpOK)
     Error(Result);
  else
     ItemNumber=Result;

  memcpy(&DataofItems[ItemNumber],InsertItem,sizeof(Items));
  ItemSetFather(ItemNumber,FatherItem);
  ItemSetPrev(ItemNumber,PrevItem);
  if (PrevItem!=0)
  {
     ItemSetNext(ItemNumber,ItemGetNext(PrevItem));
     ItemSetNext(PrevItem,ItemNumber);
  }
  else
  {
     ItemSetNext(ItemNumber,ItemGetChild(FatherItem));
     ItemSetChild(FatherItem,ItemNumber);
  }
  if (ItemGetNext(ItemNumber))
     ItemSetPrev(ItemGetNext(ItemNumber),ItemNumber);
  return(ItemNumber);
}

HITEM ItemAppend(Items *AppendItem,HITEM FatherItem)
{
  return(ItemInsert(AppendItem,FatherItem,ItemGetLastChild(FatherItem)));
}

void ItemDelete(HITEM ItemNumber)
{
  short Result;
  HITEM MidItem,NextItem;

  if ((Result=ItemDestruct(ItemNumber))!=OpOK)
     return;     //Error(Result);

  if (!ItemIsChildless(ItemNumber))
  {
     MidItem=ItemGetChild(ItemNumber);
     while (MidItem)
     {
       NextItem=ItemGetNext(MidItem);
       ItemDelete(MidItem);
       MidItem=NextItem;
     }
  }
  if (ItemGetPrev(ItemNumber)==0)
     ItemSetChild(ItemGetFather(ItemNumber),ItemGetNext(ItemNumber));
  else
     ItemSetNext(ItemGetPrev(ItemNumber),ItemGetNext(ItemNumber));
  if (ItemGetNext(ItemNumber)!=0)
     ItemSetPrev(ItemGetNext(ItemNumber),ItemGetPrev(ItemNumber));

  ItemSetCanUse(ItemNumber);
}

HITEM ItemSearchbyHandle(HANDLE Handle)
{
  int i;
  for (i=1;i<MAXITEMS;i++)
      if (ItemGetHandle(i)==Handle)
         return(i);

  return(0);
}

HITEM ItemSetFront(HITEM ItemNumber,HITEM PrevItem)
{
  if(ItemNumber>MAXITEMS)
     return(TOOMANYITEMS);
  if(PrevItem>MAXITEMS)
     return(TOOMANYITEMS);
  if (ItemNumber==0)
     return(0);

  if (ItemGetPrev(ItemNumber)==0)
     ItemSetChild(ItemGetFather(ItemNumber),ItemGetNext(ItemNumber));
  else
     ItemSetNext(ItemGetPrev(ItemNumber),ItemGetNext(ItemNumber));
  if (ItemGetNext(ItemNumber)!=0)
     ItemSetPrev(ItemGetNext(ItemNumber),ItemGetPrev(ItemNumber));

  ItemSetPrev(ItemNumber,PrevItem);
  if (PrevItem!=0)
  {
     ItemSetNext(ItemNumber,ItemGetNext(PrevItem));
     ItemSetNext(PrevItem,ItemNumber);
  }
  else
  {
     ItemSetNext(ItemNumber,ItemGetChild(ItemGetFather(ItemNumber)));
     ItemSetChild(ItemGetFather(ItemNumber),ItemNumber);
  }
  if (ItemGetNext(ItemNumber)!=0)
     ItemSetPrev(ItemGetNext(ItemNumber),ItemNumber);

  return(ItemNumber);
}

int ItemInitial(void)
{
  GlobalItemsHandle=HandleAlloc(sizeof(Items)*MAXITEMS,0);
  if (!GlobalItemsHandle)
     return(OUTOFMEMORY);
  DataofItems=HandleLock(GlobalItemsHandle);
  if (DataofItems==NULL)
  {
     HandleFree(GlobalItemsHandle);
     GlobalItemsHandle=0;
     return(OUTOFMEMORY);
  }
  memset(DataofItems,0,sizeof(DataofItems[0])*MAXITEMS);
  GlobalPageHeadHandle=1;              // Preset a Page head or file head
  ItemSetPrev(GlobalPageHeadHandle,-1);// Only for setting a sign USED
  ItemSetNext(GlobalPageHeadHandle,-1);
  ReturnOK();
}

void ItemFinish(void)
{
  if (GlobalItemsHandle)
  {
     HandleUnlock(GlobalItemsHandle);
     HandleFree(GlobalItemsHandle);
  }
}

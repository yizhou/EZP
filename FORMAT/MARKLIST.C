/*-------------------------------------------------------------------
* Name: marklist.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"
#include <assert.h>

/*------
#undef assert
#define assert(p) 0
-------*/

/*
 * Add an element to the linked list of formatted elements.
 * return a pointer to the current (end) position in the list.
 */
Pmark_rec AddEle(Pmark_rec *elistp,Pmark_rec current,Pmark_rec eptr)
{
        if (eptr == NULL)
             return(current);

        /*
         * Add object to either the head of the list for a new list,
         * or at the end after the current pointer.
         */
        if (*elistp == NULL)
        {                       // adding at head
                *elistp = eptr;
                (*elistp)->next = NULL;
                (*elistp)->prev = NULL;
        }
        else if (current!=NULL)
        {
                eptr->prev = current;
                eptr->next=current->next;
                if (current->next)
                        current->next->prev=eptr;
                current->next = eptr;
        }
        else     //current==NULL insert at head
        {
                eptr->prev = NULL;
                eptr->next=*elistp;
                if (*elistp)
                        (*elistp)->prev=eptr;
                *elistp=eptr;
        }
        return(eptr);
}

/*
 * Free up the passed linked list of formatted elements, freeing
 * all memory associates with each element.
 */
void FreeMarkList(Pmark_rec list)
{
        Pmark_rec current;
        Pmark_rec eptr;

        current = list;
        while (current != NULL)
        {
                eptr = current;
                current = current->next;
                eptr->next = NULL;
                free((char *)eptr);
        }
}

void FreeMarks(Pmark_rec *listhead,Pmark_rec list,Pmark_rec listend, BOOL bEraseBk)
{
        Pmark_rec current,eptr,plist,nlist;

        //assert(listhead);
        //assert(*listhead);

        if(listhead==NULL || *listhead==NULL || list==NULL)
           return;      // ByHance, 96,2.2

        current = list;
        plist=list->prev;

        if (listend==NULL)   nlist=NULL;
        else  nlist=listend->next;

  #ifdef MERGEBYHance
        //undisplay the mark
        while (/*(current != NULL)||*/ (current != nlist) )
        {
                assert(current!=NULL);
                eptr = current;
                EraseMark(current, bEraseBk);
                current = current->next;
        }

        current = list;
        while (/*(current != NULL)||*/(current != nlist))
        {
                assert(current!=NULL);
                eptr = current;
                current = current->next;
                eptr->next = NULL;
                free((char *)eptr);
        }
  #else
        //undisplay the mark                    //ByHance, 95,12.5
        while( current != nlist )
        {
                if(current==NULL)
                    break;

                eptr = current;
                current = current->next;

                //if(!GlobalNotDisplay)
                EraseMark(eptr, bEraseBk);
                eptr->next = NULL;
                free((char *)eptr);
        }
  #endif

        //assert(current==nlist);            //ByHance, 96,2.2

        //---- change link -----
        if (plist!=NULL)
        {
            plist->next=current;
            if (current!=NULL)
               current->prev=plist;
        }
        else
        {
            *listhead=current;
            if (current!=NULL)
                current->prev=NULL;
        }
}
#ifdef UNUSE
void PrintMarkList(Pmark_rec list)
{
    Pmark_rec current;
    Pmark_rec eptr;

    current = list;
    while (current != NULL)
    {
            eptr = current;
            current = current->next;
            fprintf(stderr, "char pos %ld placed at%d %d totol %d \n",
                    eptr->start_pos,eptr->x,eptr->y,eptr->edata_len);
    }
}

void PrintRegion2MarkList(PTextBoxs hw)
{
    Pmark_rec eptr;
    int i;
    for (i=0; i<hw->numRg; i++)
    {
       eptr=hw->region2mark[i];
       if ( eptr== NULL)
               fprintf(stderr, "error region  element %d in  array",
                       i);
       else
               fprintf(stderr, "char pos %ld placed at%d %d totol %d region %d\n",
                       eptr->start_pos,eptr->x,eptr->y,
                       eptr->edata_len,eptr->regno);
    }
}

/*
 * Contruct and return an array of pointers into the element list that
 * indexes the elements by region  number.

 * Note, region containing only while space will have NULL pointers
 * into the element list.
 */
void MakeRegionList(Pmark_rec elist)
{
     int num;
     Pmark_rec eptr;
     Pmark_rec *ll=NULL;
     HTEXTBOX hBox=0;
     PTextBoxs pBox;

    /* fill in pointers to beginning of the lines */
     eptr = elist;
     while (eptr != NULL)
     {
           if (eptr->hBox!=hBox)
           {
              if (hBox!=0)
                 HandleUnlock(ItemGetHandle(hBox));

              hBox=eptr->hBox;
              pBox=HandleLock(ItemGetHandle(hBox));
              if (pBox==NULL)
              {
                 HandleUnlock(ItemGetHandle(hBox));
                 //assert(0);
                 return;
              }

                  /* init the index array */
              ll=&pBox->region2mark[0];
              //for (i=0; i<MAXREGIONNUM; i++)  ll[i] = NULL; //ByHance
              memset((void *)ll,0,sizeof(ll[0])*MAXVIRTUALREGIONNUM);
           } //end of if change box

           num=eptr->regno;
           if(num>=MAXREGIONNUM)
           {
                //fprintf(stderr,"Region exceed Limit");
                break;
           }

           if(ll[num] == NULL)
                 ll[num] = eptr;

           eptr = eptr->next;
     }

     if (hBox!=0)
        HandleUnlock(ItemGetHandle(hBox));
}
#endif

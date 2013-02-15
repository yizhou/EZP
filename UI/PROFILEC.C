/* --------------------------------------------------------------------- */
/* ÎÄ¼þÃû:             Profile.c                                         */
/* --------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum{SET,GET,DELETE,REM,INT,STRING};
enum{NO,SECTIONFOUND,ENTRYFOUND,OK};
static char backname[]={"c:\\ezp\\ezpini.bak"};

static void ReadALine(FILE *fr,char *lbuf)
{
    long lcount;
    int lchar;

    lcount=0;
    lbuf[0]=0;
    while ((lchar=getc(fr))!=EOF)
    {
        if ((lchar<0x21) && (lcount==0)) continue;  /* Skip control char*/
        if (lchar=='\n') return;
        lbuf[lcount]=lchar;
        lcount++;
        lbuf[lcount]=0;
    }
}

static int Profile( char *fname, char *section,char *entry, char *stbuf,
                int  *val, char act, char mode)
{
     FILE *fr,*fw;
    #define LINEBUFFERLENGTH 1024
     char lbuf[LINEBUFFERLENGTH];
     long count;
     char MODE=NO;
     char NeedCreatSection=OK;
     char NeedCreatEntry=OK;

    if (act==SET)
      {
        remove(backname);
        rename(fname,backname);
        if ((fr=fopen(backname,"rt"))==NULL)
          {
            fclose(fopen(backname,"wt"));
            fr=fopen(backname,"rt");
          }
        if ((fw=fopen(fname,"wt"))==NULL)
          {
            fclose(fr);
            return (1);
          }
      }
    else
        if ((fr=fopen(fname,"rt"))==NULL) return (1);

    while (1)
    {
        /* Search Section */
        ReadALine(fr,lbuf);
        if (strlen(lbuf)==0) break;

        if (lbuf[0]=='[')
        {
            if (act==SET)
            {
                  fprintf(fw,"\n");
                  if(MODE==SECTIONFOUND)
                  {        /* creat section,entry */
                      fprintf(fw,"%s=",entry);
                      if (mode==INT) fprintf(fw,"%ld\n",*val);
                        else fprintf(fw,"%s\n",stbuf);
                      NeedCreatSection=NO;
                      NeedCreatEntry=NO;
                  }
                  fprintf(fw,"%s\n",lbuf);
            }

            count=0;
            while (lbuf[count]!=']') count++;
            if (memcmp (section,lbuf+1,count-1)==0)
            {
               MODE=SECTIONFOUND;
               NeedCreatSection=NO;
            } else MODE=NO;

            continue;
        }

        if (MODE==SECTIONFOUND)
        {
            count=0;
            while (lbuf[count]!='=') count++;
            if ((memcmp(lbuf,entry,count)==0) && (strlen(entry)==count))
                MODE=ENTRYFOUND;
        }

        if (MODE==ENTRYFOUND)
        {
            switch (act)
            {
                case SET:
                    fprintf(fw,"%s=",entry);
                    if (mode==STRING) fprintf(fw,"%s\n",stbuf);
                      else fprintf(fw,"%ld\n",*val);
                    NeedCreatEntry=NO;
                    break;
                case GET:
                    if (mode!=STRING)
                    {
                        if ((lbuf[count+2]=='x') || (lbuf[count+2]=='X'))
                          sscanf(lbuf+count+3,"%lx",val);
                        else sscanf(lbuf+count+1,"%ld",val);
                    }
                    else strcpy(stbuf,lbuf+count+1);
                    fclose(fr);
                    return (0);
                default:
                  break;
            }  /*-- end switch --*/

            MODE=NO;
        }
        else         /*---- entry not found ----*/
        if (act==SET) fprintf(fw,"%s\n",lbuf);
    }

    fclose(fr);

    if (act==SET)
    {
        if(NeedCreatSection==OK) fprintf(fw,"\n[%s]\n",section);
        if(NeedCreatEntry==OK)
        {
          fprintf(fw,"%s=",entry);
          if (mode==INT) fprintf(fw,"%ld\n",*val);
            else fprintf(fw,"%s\n",stbuf);
        }
        fclose(fw);
        remove("profile.pnd");
    }

    return (1);         /*-- not found --*/
}

int GetProfileString( char *fname, char *section,
   char *entry, char *stbuf, char *stdefault)
  {
    if (Profile(fname,section,entry,stbuf,0,GET,STRING)!=0)
      {
        strcpy(stbuf,stdefault);
        return (1);
      }
    return (0);
  }

int GetProfileInt( char *fname, char *section,
   char *entry, int *val, int valdefault)
  {
    if (Profile(fname,section,entry,"",val,GET,INT)!=0)
      {
        *val=valdefault;
        return(1);
      }
    return(0);

  }

int SetProfileString( char *fname, char *section,
   char *entry, char *stbuf)
  {
    return (Profile(fname,section,entry,stbuf,0,SET,STRING));
  }

int SetProfileInt( char *fname, char *section,
   char *entry, int val)
  {
    return(Profile(fname,section,entry,"",&val,SET,INT));
  }

#ifdef DEBUG
void main(int argv,char *argc[])
  {
     char *fname;
     char *section;
     char *entry;
     char *stbuf;
     char *stdefault;
     int *val;
     long valdefault;
     char act;
     char mode;
     long vall;
     char buf[128];

    SetProfileString("redps.cfg","SysInfo","Typeset","123");
    GetProfileString("redps.cfg","SysInfo","Typeset",buf,"3");
    return;

    if (argv<2)
      {
        fprintf(stdout,"Profile settter , use as: Profile S/G I/S FNAME SEC ENT ST/VAL ");
        return;
      }
    fprintf(stdout,"----------------------------------------");
    fname=argc[3];
    section=argc[4];
    entry=argc[5];
    stbuf=argc[6];
    stdefault=argc[6];
    if (argc[1][0]=='s') act=SET;
      else act=GET;
    if (argc[2][0]=='s') mode=STRING;
      else mode=INT;

    if (act==SET)
      {
        fprintf(stdout,"\n");
        if (mode==INT)
          {
            vall=atol(argc[6]);
            val=&vall;
            SetProfileInt(fname,section,entry,*val);
          }
        else
          {
            SetProfileString(fname,section,entry,stbuf);
          }
      }
    else
      {
        if (mode==INT)
          {
            valdefault=atol(argc[6]);
            GetProfileInt(fname,section,entry,val,valdefault);
            fprintf(stdout,"Result %s=%ld\n",entry,*val);
          }
        else
          {
            stbuf=buf;
            GetProfileString(fname,section,entry,stbuf,stdefault);
            fprintf(stdout,"Result %s=%s\n",entry,stbuf);
          }
      }
  }
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char helptag[32] = "REDTEK HELP 96V1\n\x1a";
char h[32];

typedef struct {
     short BackIndex;
     short NextIndex;
     long StartOff;
     short length;
} HelpIndex;

#define MAXTOPIC        4096
HelpIndex hindex[MAXTOPIC];

void Panic(char *str)
{
  printf("Fatal error: %s\n",str);
  exit (-1);
}

main(int argc,char *argv[])
{
    int i,j,k,cc;
    int now,back,last;

    FILE *fs;

    if (argc<2) {
       printf("Usage: help <hlpname.rhp>  topicnum \n" );
       exit(0);
    }

    fs = fopen(argv[1],"rb");
    if (NULL==fs) {
	  printf("Fail to open %s\n",argv[1]);
	  exit(1);
    }

    sscanf(argv[2],"%d",&now);

    fread(h,1,32,fs);
    if (strcmp(h,helptag) != 0)
	Panic("Wrong help file !");

    fread(hindex,1,sizeof(hindex),fs);
    if (hindex[now].StartOff<0)
	 exit(0);

    fseek(fs,hindex[now].StartOff,SEEK_CUR);
    printf("now = %d back = %d, next = %d\n",now,hindex[now].BackIndex,
	   hindex[now].NextIndex);
    printf("Help Text:\n");
    for (i=0;i<hindex[now].length;i++)
       putchar(getc(fs));

    fclose(fs);
}



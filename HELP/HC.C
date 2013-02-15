#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char helptag[32] = "REDTEK HELP 96V1\n\x1a";

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
    int last,next,back,thisone;
    FILE *fs,*ft;
    char str[80];

    if (argc<2) {
       printf("Usage: helpmk  <helpname.txt> <hlpname.rhp>\n" );
       exit(0);
    }

    for (i=0;i<MAXTOPIC;i++) {
	   hindex[i].BackIndex = hindex[i].NextIndex =  -1;
	   hindex[i].StartOff = -1;
	   hindex[i].length = 0;
    }

    fs = fopen(argv[1],"rb");
    if (NULL==fs) {
	  printf("Fail to open %s\n",argv[1]);
	  exit(1);
    }

    last = 0;
    while((cc =getc(fs))!=EOF) {
	  if (cc == '~')  {
	      cc = getc(fs);
              if (cc == '~') {
                 fgets(str,80,fs);
                 sscanf(str,"%d %d %d",&thisone,&back,&next);
                 if (thisone <0 || thisone >=MAXTOPIC)
			  Panic("Wrong pointer !");
		 if (back <0 || back >=MAXTOPIC)
			  Panic("Wrong pointer !");
                 if (next <0 || next >=MAXTOPIC)
                          Panic("Wrong pointer !");
                 hindex[thisone].BackIndex = back;
		 hindex[thisone].NextIndex = next;
		 hindex[thisone].StartOff = ftell(fs);
		 hindex[last].length = ftell(fs) - hindex[last].StartOff - strlen(str)-2;
		 last = thisone;
	      }
	  }
    }
    fseek(fs,0,SEEK_SET);

    ft = fopen(argv[2],"wb");
    if (NULL==ft) {
	  printf("Fail to open %s\n",argv[2]);
	  exit(1);
    }
    fwrite(helptag,1,32,ft);
    fwrite(hindex,1,sizeof(hindex),ft);
    while ((cc = getc(fs))!=EOF) putc(cc,ft);
    fclose(fs);
    fclose(ft);
}



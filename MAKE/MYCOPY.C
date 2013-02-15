#include <stdio.h>
static int mycopy(char *nas,char *nad)
{
  FILE *fp,*fp1;
  unsigned char buff[1030];
  int len=1024;
  fp=fopen(nas,"rb");
  if (fp==NULL) return -1;
  fp1=fopen(nad,"rb+");
  if (fp1==NULL)
  {
   fclose(fp);
   return -1;
  }
  while (len==1024&&(!feof(fp)))
  {
    len=fread(buff,1,1024,fp);
    if (len) fwrite(buff,1,len,fp1);
  }
  fclose(fp);
  fclose(fp1);
  return 0;
}
main(int nn,char *p[])
{
if (nn>2)
  {
    mycopy(p[1],p[2]);
  }
  else
   printf("\nUsage: mycopy file1 file 2");
}
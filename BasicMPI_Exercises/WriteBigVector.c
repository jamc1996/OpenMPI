#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int length = 10000;
  int c;
  int i;
  FILE *fp;
  srand48(333);

  while ((c = getopt(argc,argv,"n:")) != -1)
  {
    switch(c)
    {
      case 'n':
	length = atoi(optarg);
        break;
      case '?':
	printf("Invalid Option\n");
	return 1;
    }
  }
  
  fp = fopen("BigVector.txt","w");
  fprintf(fp,"%d\n",length);
  for(i=0;i<length;i++)
  {
    fprintf(fp,"%lf\n",drand48());
  }
  fclose(fp);
  return 0;
}

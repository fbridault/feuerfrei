#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  int nbVal = argc - 4;
  int nbValIntermediaires;
  int v,i,j;
  int sizex, sizey;
  double valx, incx, tmpx;
  FILE *f;
  
  if(argc < 6){
    printf("Usage : genIES <iesfile> sizex sizey val1 ... val\n");
    return 1;
  }
  sizex = atoi(argv[2]); sizey = atoi(argv[3]);
  if(sizex <= 0 || sizey <= 0){
    printf("Wrong size\n");
    return 2;
  }
  
  if( ( (sizex+1) % nbVal ) ){
    printf("sizex must be a multiple of the number of values - 1\n");
    return 3;
  }
  
  if((f=fopen(argv[1],"wt"))== NULL){
    printf("Cannot open file %s\n", argv[1]);
    return 4;
  }
  
  fprintf(f,"SPV1.0\n#NBZENITH %d\n#NBAZIMUTH %d\n",sizex,sizey);
  nbValIntermediaires = sizex / (nbVal-1);
  for( v=4; v < argc-1; v++ )
    {
      incx = (atof(argv[v+1]) - atof(argv[v])) / (double)(nbValIntermediaires);
      valx = atof(argv[v]);
      for( i=0 ; i < nbValIntermediaires ; i++){
	for( j=0 ; j < sizey ; j++)
	  fprintf(f,"%f ",valx);
	valx+=incx;
	fprintf(f,"\n");
      }
    }
  for( j=0 ; j < sizey ; j++)
    fprintf(f,"%f ",atof(argv[argc-1]));
  return 0;
}

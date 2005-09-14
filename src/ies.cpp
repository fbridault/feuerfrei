#include "ies.hpp"

#include <stdlib.h>

void IES::test()
{
  int z,a;
  float *pintensite;  

  printf("nbazimut : %d  nbzenith : %d\n",nbazimut,nbzenith);

  pintensite=intensites;
  for(z=0;z<nbzenith;z++)
    for(a=0;a<nbazimut;a++){
      printf("%f ",*pintensite);
      pintensite++;
    }
  printf("\n");
}

IES::IES(const char* const filename)
{
  char extension[4],chaine[255];
  int lg,z,a;
  //  int test;
  float *pintensite,tmp;
  FILE *f;

  cout << "Chargement fichier IES : " << filename << " ... ";

  // Chargement de l'extension du fichier (3 derniers caracteres)
  lg=strlen(filename);
  if(lg>2){
    strcpy(extension,filename+lg-3);
    //cout << "extension du fichier : " << extension << endl;
    if((!strcmp(extension,"IES")) || (!strcmp(extension,"ies"))){
      if(!(f=fopen(filename,"r")))
	cout << "Erreur lors de l'ouverture du fichier " << filename << endl;
      else{
	fscanf(f,"%s",chaine);
	if(!strcmp(chaine,"SPV1.0")){
	  fscanf(f,"%*s%d%*s%d",&nbzenith,&nbazimut);
	  nbazimut--;// la derniere colonne n'est pas lue
	  if((nbzenith>0) && (nbazimut>0)){
	    if((intensites=(float*)malloc(nbzenith*nbazimut*sizeof(float)))){
	      pintensite=intensites;
// 	      test=true;
	      for(z=0;z<nbzenith;z++){
		for(a=0;a<nbazimut;a++){
		  fscanf(f,"%f",pintensite++);
		  //printf("z=%d a=%d lu : %f\n",z,a,*(pintensite-1));
		}
		fscanf(f,"%f",&tmp);// la derniere colonne n'est pas lue
// 		if(*(pintensite-nbazimut)!=*(pintensite-1))
// 		  test=false;
	      }
// 	      if(test)
// 		printf("memes donnees sur la premiere et la derniere colonne\n");
	      
	      cout << "taille : " << nbazimut << "x"<< nbzenith << endl;
	      lazimut = 2*M_PI/(nbazimut-1);
	      lazimutTEX = 2*M_PI/nbazimut;
	      lzenith = M_PI/(nbzenith-1);
	      lzenithTEX = M_PI/nbzenith;
	      denom = lazimut*lzenith;	      
	    }
	    else
	      cout << "Erreur d'allocation des texels" << endl;
	  }
	  else
	    cout << "Decoupage zenith/azimut incorrect : nbzenith=" << nbzenith << " nbazimut=" << nbazimut << endl;
	}
	else
	  cout << "Format du fichier IES non reconnu" << endl;
	fclose(f);
      }
    }
    else
      cout << "Extension du fichier ies non reconnue : " << extension << endl;
  }
  else
    cout << "Nom de fichier IES non reconnu" << endl;

  texture = new Texture(getNbazimut(),getNbzenith(),getIntensites());
}

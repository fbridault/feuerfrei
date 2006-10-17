#include "ies.hpp"

#include <fstream>

IES::IES(const char* const filename)
{
  char extension[4],chaine[255];
  uint lg,z,a;
  //  int test;
  float *pintensite;

  // Chargement de l'extension du fichier (3 derniers caracteres)
  lg=strlen(filename);
  if(lg>2){
    strcpy(extension,filename+lg-3);
    //cout << "extension du fichier : " << extension << endl;
    if((!strcmp(extension,"IES")) || (!strcmp(extension,"ies"))){
      ifstream iesFile(filename, ios::in);
      cout << "Chargement fichier IES : " << filename << " ... ";
      if (!iesFile.is_open ()){
	cerr << "Erreur !! " << endl;
	throw (ios::failure ("Open error"));
	return;
      }
      iesFile >> chaine;
      if(!strcmp(chaine,"SPV1.0")){
	iesFile >> chaine >> m_nbzenith >> chaine >> m_nbazimut;
	//m_nbazimut--;// la derniere colonne n'est pas lue
	if((m_nbzenith>0) && (m_nbazimut>0)){
	  if((m_intensites=new float[m_nbzenith*m_nbazimut])){
	    pintensite=m_intensites;
	    // 	      test=true;
	    for(z = 0 ; z < m_nbzenith ; z++){
	      for(a = 0 ; a < m_nbazimut ; a++){
		iesFile >> *(pintensite++);
		//printf("z=%d a=%d lu : %lf\n",z,a,*(pintensite-1));
	      }
	      // iesFile >> tmp;
	      // la derniere colonne n'est pas lue
	      // 		if(*(pintensite-nbazimut)!=*(pintensite-1))
	      // 		  test=false;
	    }
	    // 	      if(test)
	    // 		printf("memes donnees sur la premiere et la derniere colonne\n");
	    
	    cout << "taille : " << m_nbazimut << "x"<< m_nbzenith << endl;
	    m_lazimut = 2*M_PI/(m_nbazimut-1);
	    m_lazimutTEX = 2*M_PI/m_nbazimut;
	    m_lzenith = M_PI/(m_nbzenith-1);
	    m_lzenithTEX = M_PI/m_nbzenith;
	    m_denom = m_lazimut*m_lzenith;	      
	  }
	  else
	    cout << "Erreur d'allocation des texels" << endl;
	}
	else
	  cout << "Decoupage zenith/azimut incorrect : nbzenith=" << m_nbzenith << " nbazimut=" << m_nbazimut << endl;
      }
      else
	cout << "Format du fichier IES non reconnu" << endl;
      iesFile.close();
      
    }
    else
      cout << "Extension du fichier ies non reconnue : " << extension << endl;
  }
  else
    cout << "Nom de fichier IES non reconnu" << endl;
}

void IES::test()
{
  uint z,a;
  float *pintensite;  

  cout << "nbazimut : " << m_nbazimut << " nbzenith : " << m_nbzenith << endl;

  pintensite=m_intensites;
  for(z = 0; z < m_nbzenith ; z++)
    for(a = 0; a < m_nbazimut ; a++){
      printf("%f ",*pintensite);
      pintensite++;
    }
  cout << endl;
}

IES::~IES()
{
  delete [] m_intensites;
}

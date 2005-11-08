#include "OBJReader.hpp"

#include <fstream>

COBJReader::COBJReader (const char *filename, CScene* scene, CObject* object, bool detached)
{
  char lettre,drop;
  char buffer[255];
  int coord_textures = 0, normales = 0;
  bool objectWSV = false;
  float x, y, z;
  int a, b, c, d, an, bn, cn, dn, at, bt, ct, dt, w, matIndex=0;
  int valeurLues;
  bool alreadyOneObject = false;
  bool objectsAttributesSet=0;
  int nbVertex=0, nbNormals=0, nbTexCoords=0;
  int nbObjectVertex=0, nbObjectNormals=0, nbObjectTexCoords=0;
  CObject* currentObject=NULL;
  bool importSingleObject = (object != NULL);
  
  CPoint offset(0,0,0);
  //CPoint offset(-19,2.2,8);
  //CPoint offset(-10,0.2,16);
  
  AS_ERROR(chdir("./scenes"),"chdir scenes dans COBJReader");
  ifstream objFile(filename, ios::in);
  if (!objFile.is_open ()){
    throw (ios::failure ("Open error"));
    return;
  }
  
  while (!objFile.eof())
    {
      objFile >> lettre;
      
      switch (lettre)
	{
	default:
	  objFile.getline(buffer, sizeof (buffer));
	  break;
	case 'g':
	  objFile >> buffer;
	  if(importSingleObject){
	    /* Nouvel objet */
	    if(alreadyOneObject){
	      cerr << "Ne peut pas ajouter plus d'un objet OBJ dans un CObject" << endl;
	      cerr << "Chargement du fichier " << filename << " interrompu" << endl;
	      return;
	    }
	    currentObject = object;
	    alreadyOneObject = true;
	  }else
	    currentObject = new CObject(scene,&offset);
	    
	  if (!strncmp (buffer, "WSV", 3))
	    scene->addObject(currentObject, true);
	  else
	    scene->addObject(currentObject, false);
	  objectsAttributesSet = false;
	  nbObjectVertex = nbVertex;
	  nbObjectNormals = nbNormals;
	  nbObjectTexCoords = nbTexCoords;
	  break;
	case '\n':
	  break;
	case 'm':
	  /* Définition d'un matriau pour l'objet courant */
	  objFile >> buffer >> buffer;
	  this->importMaterial ((const char *) buffer, scene);
	  break;
	case 'v':	  
	  objFile.get(lettre);
	  switch (lettre)
	    {
	    default:
	      break;
	    case ' ':
	      objFile >> x >> y >> z; 
	      currentObject->addVertex(new CPoint(x, y, z));
	      normales = coord_textures = 0;
	      nbVertex++;
	      break;
	    case 'n':
	      objFile >> x >> y >> z;	      
	      currentObject->addNormal (new CVector (x, y, z));
	      normales = 1;
	      nbNormals++;
	      break;
	    case 't':
	      objFile >> x >> y;	      
	      currentObject->addTexCoord (new CPoint (x, y, 0));
	      coord_textures = 2;
	      nbTexCoords++;
	      break;
	    }
	  break;
	case 'f':
	  if(!objectsAttributesSet){
	    objectsAttributesSet = true;
	    currentObject->setAttributes(coord_textures + normales);
	  }
	  switch (coord_textures + normales)
	    {
	    case 0:
	      objFile >> a >> b >> c;// >> d;
	      
	      // if (valeurLues < 3){
// 		cout << "Erreur de chargement : Le fichier "
// 		     << filename
// 		     << " contient des erreurs d'indexation de points.\n";
// 		return;
// 	      }
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex),
				      new CIndex(b - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex),
				      new CIndex(c - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex));
	      
	      // if (valeurLues > 3)
// 		cout << "problème: facette non triangulaire !!!" << endl;
	      
	      break;
	    case 1:
	      objFile >> a >> drop >> drop >> an;
	      objFile >> b >> drop >> drop >> bn;
	      objFile >> c >> drop >> drop >> cn;
	      //	      objFile >> d >> drop >> drop >> dn;
	     //  valeurLues = fscanf (pfichier_obj, " %d//%d %d//%d %d//%d %d//%d", &a,
// 				   &an, &b, &bn, &c, &cn, &d, &dn);
// 	      if (valeurLues < 6)
// 		{
// 		  cout << "Erreur de chargement : Le fichier " << filename
// 		       << " contient des erreurs d'indexation de points.\n";
// 		  return;
// 		}
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1,
						 an - nbObjectNormals - 1,
						 UNDEFINED, matIndex),
				      new CIndex(b - nbObjectVertex - 1,
						 bn - nbObjectNormals - 1,
						 UNDEFINED, matIndex),
				      new CIndex(c - nbObjectVertex - 1,
						 cn - nbObjectNormals - 1,
						 UNDEFINED, matIndex));
	    //   if (valeurLues > 6)
// 		  cout << "problème: facette non triangulaire !!!" << endl;
	      
	      break;
	    case 2:
	      objFile >> a >> drop >> w;
	      objFile >> b >> drop >> w;
	      objFile >> c >> drop >> w;
	      //	      objFile >> d >> drop >> w;
//	      valeurLues = fscanf (pfichier_obj, " %d/%d %d/%d %d/%d %d/%d", &a, &w,
// 				   &b, &w, &c, &w, &d, &w);
// 	      if (valeurLues < 6)
// 		{
// 		  cout << "Erreur de chargement : Le fichier " << filename
// 		       << " contient des erreurs d'indexation de points.\n";
// 		  return;
// 		}
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1, 
						 an - nbObjectNormals - 1,
						 UNDEFINED, matIndex),
				      new CIndex(b - nbObjectVertex - 1, 
						 bn - nbObjectNormals - 1, 
						 UNDEFINED, matIndex),
				      new CIndex(c - nbObjectVertex - 1, 
						 cn - nbObjectNormals - 1, 
						 UNDEFINED, matIndex));
	      
	      // if (valeurLues > 6)
// 		cout << "problème: facette non triangulaire !!!" << endl;
 	      break;
	    case 3:
	      objFile >> a >> drop >> at >> drop >> an;
	      objFile >> b >> drop >> bt >> drop >> bn;
	      objFile >> c >> drop >> ct >> drop >> cn;
	      //	      objFile >> d >> drop >> dt >> drop >> dn;
	      // valeurLues = fscanf (pfichier_obj,
// 				   " %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &a, &at,
// 				   &an, &b, &bt, &bn, &c, &ct, &cn, &d, &dt, &dn);
	      // if (valeurLues < 9)
// 		{
// 		  cout << "Erreur de chargement : Le fichier " << filename
// 		       << " contient des erreurs d'indexation de points.\n";
// 		  return;
// 		}
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1, 
						 an - nbObjectNormals - 1,
						 at - nbObjectTexCoords - 1, matIndex),
				      new CIndex(b - nbObjectVertex - 1, 
						 bn - nbObjectNormals - 1, 
						 bt - nbObjectTexCoords - 1, matIndex),
				      new CIndex(c - nbObjectVertex - 1, 
						 cn - nbObjectNormals - 1, 
						 ct - nbObjectTexCoords - 1, matIndex));
	      
	      // if (valeurLues > 9)
// 		cout << "problème: facette non triangulaire !!!" << endl;
	      break;
	      
	    default:
	      cout << "Erreur de chargement : Le fichier " << filename <<
		" contient des erreurs d'indexation de points.\n";
	      return;
	      break;
	    }
	  break;
	case 'u':
	  objFile >> buffer >> buffer;
	  matIndex = scene->setMaterialToObjectByName(buffer,currentObject);
	}
    }
  objFile.close ();
  chdir("..");
}

void
COBJReader::importMaterial (const char *filename, CScene *scene)
{
  char lettre, lettre2;
  char buffer[255];
  Texture *nouvelle_texture = NULL;
  string name_nouvelle_matiere;
  
  ifstream matFile(filename, ios::in);
  if (!matFile.is_open ()){
    throw (ios::failure ("Open error"));
    return;
  }
  
  double Kd[3], Ka[3], Ks[3], alpha, shini;
  
  while (!matFile.eof())
    {
      matFile >> lettre;
      
      switch (lettre)
	{
	case 'K':
	  matFile >> lettre2;
	  
	  switch (lettre2)
	    {
	    case 'd':
	      {
		float R, G, B;
		matFile >> R >> G >> B;
		Kd[0] = R;
		Kd[1] = G;
		Kd[2] = B;
	      }
	      break;
	    case 'a':
	      {
		float R, G, B;
		matFile >> R >> G >> B;
		Ka[0] = R;
		Ka[1] = G;
		Ka[2] = B;
	      }
	      break;
	    case 's':
	      {
		float R, G, B;
		matFile >> R >> G >> B;
		Ks[0] = R;
		Ks[1] = G;
		Ks[2] = B;
	      }
	      break;
	    default:
	      break;
	    }
	  break;
	case 'N':
	  matFile >> lettre;
	  switch (lettre)
	    {
	    case 's':
	      matFile >> shini;
	      shini = (shini / 1000) * 128;
	      break;
	    default:
	      break;
	    }
	  break;
	case 'd':
	  matFile >> alpha;
	  break;
	case 'n':
	  Ks[0] = Ks[1] = Ks[2] = Kd[0] = Kd[1] = Kd[2] = Ka[0] = Ka[1] = Ka[2] = 0.0;
	  alpha = 1.0;
	  shini = 0.0;
	  matFile >> buffer >> buffer;
	  name_nouvelle_matiere = buffer;
	  //cout << name_nouvelle_matiere << endl;
	  break;
	case 'i':		// considéré comme le dernier, le matériau est créé
	  scene->addMaterial(new CMaterial (name_nouvelle_matiere, Ka, Kd, Ks, shini, nouvelle_texture)); //,alpha);
	  nouvelle_texture = NULL;
	  break;
	case 'm':		//map_K?
	  matFile >> buffer >> buffer;
	  //nouvelle_texture = new Texture (buffer);
	  nouvelle_texture = new Texture (wxString(buffer, wxConvUTF8));
	  break;
	default:
	  matFile.getline(buffer, sizeof (buffer));
	  break;
	}
    }
  matFile.close();
}

COBJReader::~COBJReader()
{
}

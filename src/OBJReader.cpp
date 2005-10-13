// OBJReader.cpp: implementation of the COBJReader class.
//
//////////////////////////////////////////////////////////////////////

#include "OBJReader.hpp"

#include "unistd.h"

/**
 * Fonction interface permettant l'import d'un fichier OBJ dans une sc√®ne.
 *
 * @param filename nom du fichier OBJ √† importer.
 * @param sceneToImportInto pointeur vers la sc√®ne dans laquelle importer le fichier OBJ.
 */
void
importOBJFile2Scene (const char *filename, CScene * sceneToImportInto)
{
  COBJReader import (filename, sceneToImportInto);
}

/**
 * Fonction interface permettant l'import d'un fichier OBJ dans une sc√®ne.
 *
 * @param filename nom du fichier OBJ √† importer.
 * @param sceneToImportInto pointeur vers la sc√®ne dans laquelle importer le fichier OBJ.
 */
void
importOBJFile2Scene (const char *filename, CObject * objectToImportInto)
{
  COBJReader import (filename, objectToImportInto);
}

COBJReader::COBJReader (const char *filename, CObject* currentObject)
{
  char lettre;
  char buffer[255];
  int coord_textures = 0, normales = 0;
  double coeff[3] = { .8, .8, .8 };
  bool objectWSV = false;
  float x, y, z;
  int a, b, c, d, an, bn, cn, dn, at, bt, ct, dt, w, matIndex;
  int valeurLues;
  bool alreadyOneObject = false;
  bool objectsAttributesSet;
  int nbVertex=0, nbNormals=0, nbTexCoords=0;
  int nbObjectVertex=0, nbObjectNormals=0, nbObjectTexCoords=0;
  
  CMaterial *materiau_base = new CMaterial (NULL, coeff, NULL, 0);
	
  AS_ERROR(chdir("./scenes"),"chdir scenes dans COBJReader");
  FILE *pfichier_obj = fopen (filename, "r");
  if (!pfichier_obj){
    cerr << "fichier de scËne " << filename << " n'existe pas" << endl;
    return;
  }
  
  while (!feof (pfichier_obj))
    {
      lettre = fgetc (pfichier_obj);

      switch (lettre)
	{
	default:
	  fgets (buffer, sizeof (buffer), pfichier_obj);
	  break;
	case 'g':
	  /* Nouvel objet */
	  if(alreadyOneObject){
	    cerr << "Ne peut pas ajouter plus d'un objet OBJ dans un objet de la scËne" << endl;
	    cerr << "Chargement du fichier " << filename << " interrompu" << endl;
	    return;
	  }
	  alreadyOneObject = true;
	  fscanf (pfichier_obj, " %s", buffer);
	  if (!strncmp (buffer, "WSV", 3))
	    objectWSV = true;
	  else
	    objectWSV = false;
	  objectsAttributesSet = false;
	  nbObjectVertex = nbVertex;
	  nbObjectNormals = nbNormals;
	  nbObjectTexCoords = nbTexCoords;
	  break;
	case '\n':
	  break;
	case 'm':
	  /* DÈfinition d'un matÈriau pour l'objet courant */
	  fscanf (pfichier_obj, " %s %s", buffer, buffer);
	  this->importMaterial ((const char *) buffer);
	  
	  break;
	case 'v':
	  lettre = fgetc (pfichier_obj);
	  switch (lettre)
	    {
	    default:
	      break;
	    case ' ':
	      fscanf (pfichier_obj, "%f %f %f", &x, &y, &z);
	      currentObject->addVertex(new CPoint(x, y, z));
	      normales = coord_textures = 0;
	      nbVertex++;
	      break;
	    case 'n':
	      fscanf (pfichier_obj, "%f %f %f", &x, &y, &z);
	      currentObject->addNormal (new CVector (x, y, z));
	      normales = 1;
	      nbNormals++;
	      break;
	    case 't':
	      fscanf (pfichier_obj, "%f %f", &x, &y);
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
	      valeurLues = fscanf (pfichier_obj, " %d %d %d %d", &a, &b, &c, &d);
	      if (valeurLues < 3){
		cout << "Erreur de chargement : Le fichier "
		     << filename
		     << " contient des erreurs d'indexation de points.\n";
		return;
	      }
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex),
				      new CIndex(b - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex),
				      new CIndex(c - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex));
	      
	      if (valeurLues > 3)
		cout << "problËme: facette non triangulaire !!!" << endl;
	      
	      break;
	    case 1:
	      valeurLues = fscanf (pfichier_obj, " %d//%d %d//%d %d//%d %d//%d", &a,
				   &an, &b, &bn, &c, &cn, &d, &dn);
	      if (valeurLues < 6)
		{
		  cout << "Erreur de chargement : Le fichier " << filename
		       << " contient des erreurs d'indexation de points.\n";
		  return;
		}
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1,
						 an - nbObjectNormals - 1,
						 UNDEFINED, matIndex),
				      new CIndex(b - nbObjectVertex - 1,
						 bn - nbObjectNormals - 1,
						 UNDEFINED, matIndex),
				      new CIndex(c - nbObjectVertex - 1,
						 cn - nbObjectNormals - 1,
						 UNDEFINED, matIndex));
	      if (valeurLues > 6)
		  cout << "problËme: facette non triangulaire !!!" << endl;
	      
	      break;
	    case 2:
	      valeurLues = fscanf (pfichier_obj, " %d/%d %d/%d %d/%d %d/%d", &a, &w,
				   &b, &w, &c, &w, &d, &w);
	      if (valeurLues < 6)
		{
		  cout << "Erreur de chargement : Le fichier " << filename
		       << " contient des erreurs d'indexation de points.\n";
		  return;
		}
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1, 
						 an - nbObjectNormals - 1,
						 UNDEFINED, matIndex),
				      new CIndex(b - nbObjectVertex - 1, 
						 bn - nbObjectNormals - 1, 
						 UNDEFINED, matIndex),
				      new CIndex(c - nbObjectVertex - 1, 
						 cn - nbObjectNormals - 1, 
						 UNDEFINED, matIndex));
	      
	      if (valeurLues > 6)
		cout << "problËme: facette non triangulaire !!!" << endl;
	      break;
	    case 3:
	      valeurLues = fscanf (pfichier_obj,
				   " %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &a, &at,
				   &an, &b, &bt, &bn, &c, &ct, &cn, &d, &dt, &dn);
	      if (valeurLues < 9)
		{
		  cout << "Erreur de chargement : Le fichier " << filename
		       << " contient des erreurs d'indexation de points.\n";
		  return;
		}
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1, 
						 an - nbObjectNormals - 1,
						 at - nbObjectTexCoords - 1, matIndex),
				      new CIndex(b - nbObjectVertex - 1, 
						 bn - nbObjectNormals - 1, 
						 bt - nbObjectTexCoords - 1, matIndex),
				      new CIndex(c - nbObjectVertex - 1, 
						 cn - nbObjectNormals - 1, 
						 ct - nbObjectTexCoords - 1, matIndex));
	      
	      if (valeurLues > 9)
		cout << "problËme: facette non triangulaire !!!" << endl;
	      break;
	      
	    default:
	      cout << "Erreur de chargement : Le fichier " << filename <<
		" contient des erreurs d'indexation de points.\n";
	      return;
	      break;
	    }
	  break;
	case 'u':
	  fscanf (pfichier_obj, " %s %s", buffer, buffer);
	  matiter = matlist.begin ();

	  for (miter = mlist.begin ();
	       (miter != mlist.end ())
	       && (strcmp ((const char *) buffer, (*miter).c_str ()) != 0);
	       miter++)
	    {
	      matiter++;
	    }
	  if (miter != mlist.end ())
	    {
	      matIndex = currentObject->addMaterial(*matiter);
	    }
	  else
	    {
	      cout << "Erreur de chargement : material " << buffer << " inconnu.\n" << endl;
	      matIndex = currentObject->addMaterial(materiau_base);
	    }
	  break;
	}
    }
  fclose (pfichier_obj);
  chdir("..");
}

COBJReader::COBJReader (const char *filename, CScene* s)
{
  char lettre;
  char buffer[255];
  int coord_textures = 0, normales = 0;
  double coeff[3] = { .8, .8, .8 };
  bool objectWSV = false;
  float x, y, z;
  int a, b, c, d, an, bn, cn, dn, at, bt, ct, dt, w, matIndex;
  int valeurLues;
  CObject *currentObject;
  bool objectsAttributesSet;
  int nbVertex=0, nbNormals=0, nbTexCoords=0;
  int nbObjectVertex=0, nbObjectNormals=0, nbObjectTexCoords=0;
  CPoint offset(0,0,0);
  //CPoint offset(-19,2.2,8);
  //CPoint offset(-10,0.2,16);
  CMaterial *materiau_base = new CMaterial (NULL, coeff, NULL, 0);
	
  AS_ERROR(chdir("./scenes"),"chdir scenes dans COBJReader");
  FILE *pfichier_obj = fopen (filename, "r");
  if (!pfichier_obj){
    cerr << "fichier de scËne " << filename << " n'existe pas" << endl;
    return;
  }
  
  while (!feof (pfichier_obj))
    {
      lettre = fgetc (pfichier_obj);

      switch (lettre)
	{
	default:
	  fgets (buffer, sizeof (buffer), pfichier_obj);
	  break;
	case 'g':
	  /* Nouvel objet */
	  fscanf (pfichier_obj, " %s", buffer);
	  
	  currentObject = new CObject(&offset);
	  
	  if (!strncmp (buffer, "WSV", 3))
	    s->addObject(currentObject, true);
	  else
	    s->addObject(currentObject, false);

	  objectsAttributesSet = false;
	  nbObjectVertex = nbVertex;
	  nbObjectNormals = nbNormals;
	  nbObjectTexCoords = nbTexCoords;
	  break;
	case '\n':
	  break;
	case 'm':
	  /* DÈfinition d'un matÈriau pour l'objet courant */
	  fscanf (pfichier_obj, " %s %s", buffer, buffer);
	  this->importMaterial ((const char *) buffer);
	  
	  break;
	case 'v':
	  lettre = fgetc (pfichier_obj);
	  switch (lettre)
	    {
	    default:
	      break;
	    case ' ':
	      fscanf (pfichier_obj, "%f %f %f", &x, &y, &z);
	      currentObject->addVertex(new CPoint(x, y, z));
	      normales = coord_textures = 0;
	      nbVertex++;
	      break;
	    case 'n':
	      fscanf (pfichier_obj, "%f %f %f", &x, &y, &z);
	      currentObject->addNormal (new CVector (x, y, z));
	      normales = 1;
	      nbNormals++;
	      break;
	    case 't':
	      fscanf (pfichier_obj, "%f %f", &x, &y);
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
	      valeurLues = fscanf (pfichier_obj, " %d %d %d %d", &a, &b, &c, &d);
	      if (valeurLues < 3){
		cout << "Erreur de chargement : Le fichier "
		     << filename
		     << " contient des erreurs d'indexation de points.\n";
		return;
	      }
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex),
				      new CIndex(b - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex),
				      new CIndex(c - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex));
	      
	      if (valeurLues > 3)
		cout << "problËme: facette non triangulaire !!!" << endl;
	      
	      break;
	    case 1:
	      valeurLues = fscanf (pfichier_obj, " %d//%d %d//%d %d//%d %d//%d", &a,
				   &an, &b, &bn, &c, &cn, &d, &dn);
	      if (valeurLues < 6)
		{
		  cout << "Erreur de chargement : Le fichier " << filename
		       << " contient des erreurs d'indexation de points.\n";
		  return;
		}
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1, 
						 an - nbObjectNormals - 1,
						 UNDEFINED, matIndex),
				      new CIndex(b - nbObjectVertex - 1, 
						 bn - nbObjectNormals - 1, 
						 UNDEFINED, matIndex),
				      new CIndex(c - nbObjectVertex - 1, 
						 cn - nbObjectNormals - 1, 
						 UNDEFINED, matIndex));
	      
	      if (valeurLues > 6)
		  cout << "problËme: facette non triangulaire !!!" << endl;
	      
	      break;
	    case 2:
	      valeurLues = fscanf (pfichier_obj, " %d/%d %d/%d %d/%d %d/%d", &a, &w,
				   &b, &w, &c, &w, &d, &w);
	      if (valeurLues < 6)
		{
		  cout << "Erreur de chargement : Le fichier " << filename
		       << " contient des erreurs d'indexation de points.\n";
		  return;
		}
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1, 
						 an - nbObjectNormals - 1,
						 UNDEFINED, matIndex),
				      new CIndex(b - nbObjectVertex - 1, 
						 bn - nbObjectNormals - 1, 
						 UNDEFINED, matIndex),
				      new CIndex(c - nbObjectVertex - 1, 
						 cn - nbObjectNormals - 1, 
						 UNDEFINED, matIndex));
	      
	      if (valeurLues > 6)
		cout << "problËme: facette non triangulaire !!!" << endl;
	      break;
	    case 3:
	      valeurLues = fscanf (pfichier_obj,
				   " %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &a, &at,
				   &an, &b, &bt, &bn, &c, &ct, &cn, &d, &dt, &dn);
	      if (valeurLues < 9)
		{
		  cout << "Erreur de chargement : Le fichier " << filename
		       << " contient des erreurs d'indexation de points.\n";
		  return;
		}
	      currentObject->addFacet(new CIndex(a - nbObjectVertex - 1, 
						 an - nbObjectNormals - 1,
						 at - nbObjectTexCoords - 1, matIndex),
				      new CIndex(b - nbObjectVertex - 1, 
						 bn - nbObjectNormals - 1, 
						 bt - nbObjectTexCoords - 1, matIndex),
				      new CIndex(c - nbObjectVertex - 1, 
						 cn - nbObjectNormals - 1, 
						 ct - nbObjectTexCoords - 1, matIndex));
	      
	      if (valeurLues > 9)
		cout << "problËme: facette non triangulaire !!!" << endl;
	      break;
	      
	    default:
	      cout << "Erreur de chargement : Le fichier " << filename <<
		" contient des erreurs d'indexation de points.\n";
	      return;
	      break;
	    }
	  break;
	case 'u':
	  fscanf (pfichier_obj, " %s %s", buffer, buffer);
	  matiter = matlist.begin ();
	  for (miter = mlist.begin ();
	       (miter != mlist.end ())
	       && (strcmp ((const char *) buffer, (*miter).c_str ()) != 0);
	       miter++)
	    {
	      matiter++;
	    }
	  if (miter != mlist.end ())
	    {
	      matIndex = currentObject->addMaterial(*matiter);
	    }
	  else
	    {
	      cout << "Erreur de chargement : material " << buffer <<
		" unknown.\n";
	      matIndex = currentObject->addMaterial(materiau_base);
	    }
	  break;
	}
    }
  fclose (pfichier_obj);
  chdir("..");
}

void
COBJReader::importMaterial (const char *filename)
{
  char lettre, lettre2;
  char buffer[255];
  FILE *pfichier_mtl = fopen (filename, "r");
  CMaterial *nouvelle_matiere;
  Texture *nouvelle_texture = NULL;
  string name_nouvelle_matiere;
  if (!pfichier_mtl)
    return;

  double Kd[3], Ka[3], Ks[3], alpha, shini;

  while (!feof (pfichier_mtl))
    {
      lettre = fgetc (pfichier_mtl);

      switch (lettre)
	{
	case 'K':
	  lettre2 = fgetc (pfichier_mtl);

	  switch (lettre2)
	    {
	    case 'd':
	      {
		float R, G, B;
		fscanf (pfichier_mtl, "%f %f %f ", &R, &G, &B);
		//cout << R << " " << G << " " << B << endl;
		Kd[0] = R;
		Kd[1] = G;
		Kd[2] = B;
	      }
	      break;
	    case 'a':
	      {
		float R, G, B;
		fscanf (pfichier_mtl, "%f %f %f ", &R, &G, &B);
		//cout << R << " " << G << " " << B << endl;
		Ka[0] = R;
		Ka[1] = G;
		Ka[2] = B;
	      }
	      break;
	    case 's':
	      {
		float R, G, B;
		fscanf (pfichier_mtl, "%f %f %f ", &R, &G, &B);
		//cout << R << " " << G << " " << B << endl;
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
	  lettre = fgetc (pfichier_mtl);
	  switch (lettre)
	    {
	    case 's':
	      fscanf (pfichier_mtl, "%lf ", &shini);
	      shini = (shini / 1000) * 128;
	      break;
	    default:
	      break;
	    }
	  break;
	case 'd':
	  fscanf (pfichier_mtl, "  %lf  ", &alpha);
	  break;
	case 'n':
	  Ks[0] = Ks[1] = Ks[2] = Kd[0] = Kd[1] = Kd[2] = Ka[0] = Ka[1] =
	    Ka[2] = 0.0;
	  alpha = 1.0;
	  shini = 0.0;
	  fscanf (pfichier_mtl, " %s %s ", buffer, buffer);
	  name_nouvelle_matiere = buffer;
	  //cout << name_nouvelle_matiere << endl;
	  break;
	case 'i':		// considÈrÈ comme le dernier, le matÈriau est crÈÈ
	  nouvelle_matiere = new CMaterial (Ka, Kd, Ks, shini, nouvelle_texture);	//,alpha);
	  mlist.push_back (name_nouvelle_matiere);
	  matlist.push_back (nouvelle_matiere);
	  nouvelle_texture = NULL;
	  //cout << "MatÈriau ajoutÈ" << endl;
	  break;
	case 'm':		//map_K?
	  fscanf (pfichier_mtl, " %s %s ", buffer, buffer);
	  nouvelle_texture = new Texture (buffer);
	  break;
	default:
	  fgets (buffer, sizeof (buffer), pfichier_mtl);
	  break;
	}
    }
  fclose (pfichier_mtl);
}

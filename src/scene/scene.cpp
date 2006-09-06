#include "scene.hpp"

#include <fstream>

Scene::Scene (const char* const fileName, FireSource **flames, int nbFlames)
{  
  m_flames = flames;
  m_nbFlames = nbFlames;
  
  addMaterial(new Material());
  cerr << "Chargement de la scène " << fileName << endl;
  importOBJ(fileName);
}

void Scene::createDisplayLists(void)
{
  m_displayLists[0] = glGenLists(NB_DISPLAY_LISTS);
  for(int i=1; i<8; i++)
    m_displayLists[i] = m_displayLists[0] + i;
  
  glNewList(m_displayLists[0],GL_COMPILE);
  for (vector<Object*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(ALL,true);
  glEndList();
  
  glNewList(m_displayLists[1],GL_COMPILE);
  for (vector<Object*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(TEXTURED,true);
  glEndList();
  
  glNewList(m_displayLists[2],GL_COMPILE);
  for (vector<Object*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(FLAT,false);
  glEndList();  
  
  glNewList(m_displayLists[3],GL_COMPILE); 
  for (vector<Object*>::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->draw(TEXTURED,true);
  glEndList();  
  
  /* Création de la display list des objets qui projettent des ombres sans les textures */
  /* Ce qui permet d'aller plus vite lors de la génération des shadow volumes */
  glNewList(m_displayLists[4],GL_COMPILE); 
  for (vector<Object*>::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->draw(FLAT,false);
  glEndList();	
  
  glNewList (m_displayLists[7], GL_COMPILE);
  for (vector < Object * >::iterator objectsArrayIterator = m_objectsArray.begin ();
       objectsArrayIterator != m_objectsArray.end ();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw (AMBIENT,false);
  glEndList ();
  
  /* Création de la display list des objets qui projettent des ombres */
  glNewList (m_displayLists[5], GL_COMPILE);
  for (vector < Object * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
      (*objectsArrayIteratorWSV)->draw (ALL,true);
  glEndList ();
  
  /* Création de la display list des objets qui projettent des ombres sans les textures */
  /* Ce qui permet d'aller plus vite lors de la génération des shadow volumes */
  glNewList (m_displayLists[6], GL_COMPILE);  
  for (vector < Object * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->draw (AMBIENT,false);
  glEndList ();
  
  cout << "Terminé" << endl;
  cout << "*******************************************" << endl;
  cout << "Statistiques sur la scène :" << endl;
  cout << getObjectsCount() << " objets" << endl;
  cout << getPolygonsCount() << " polygones" << endl;
  cout << getVertexCount() << " vertex" << endl;
  cout << "*******************************************" << endl;
}

Scene::~Scene ()
{
  for (vector < Object * >::iterator objectsArrayIterator = m_objectsArray.begin ();
       objectsArrayIterator != m_objectsArray.end ();
       objectsArrayIterator++)
    delete (*objectsArrayIterator);
  m_objectsArray.clear ();
  
  for (vector < Object * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    delete (*objectsArrayIteratorWSV);
  m_objectsArrayWSV.clear ();

  for (vector < Source * >::iterator lightSourcesIterator = m_lightSourcesArray.begin ();
       lightSourcesIterator != m_lightSourcesArray.end (); lightSourcesIterator++)
    delete (*lightSourcesIterator);
  m_lightSourcesArray.clear ();

  for (vector < Material * >::iterator materialArrayIterator = m_materialArray.begin ();
       materialArrayIterator != m_materialArray.end (); materialArrayIterator++)
    delete (*materialArrayIterator);
  m_materialArray.clear ();
  
  glDeleteLists(m_displayLists[0],NB_DISPLAY_LISTS);
}

void Scene::getSceneAbsolutePath(const char* const fileName)
{
  bool found=false;
  
  cerr << fileName << endl;
  if(strlen(fileName) > 255)
    cerr << "Dir string too long" << endl;
  
  for( int i=strlen(fileName)-1 ; i >= 0 ; i--)
    {
      if(found)
	m_currentDir[i]=fileName[i];
      else
	if(fileName[i] == '/'){
	  found=true;
	  m_currentDir[i+1]='\0';
	  m_currentDir[i]=fileName[i];
	}
    }
}

bool Scene::importOBJ(const char* fileName, Object* object, bool detached, const char* objName)
{
  char lettre,drop;
  char buffer[255];
  int coord_textures = 0, normales = 0;
  double x, y, z;
  int a, b, c, d, an, bn, cn, dn, at, bt, ct, dt, w, matIndex=0;
  int valeurLues;
  bool alreadyOneObject = false, skip = false;
  bool objectsAttributesSet=0;
  int nbVertex=0, nbNormals=0, nbTexCoords=0;
  int nbObjectVertex=0, nbObjectNormals=0, nbObjectTexCoords=0;
  Object* currentObject=NULL;  
  bool importSingleObject = (object != NULL);
  bool lookForSpecificObject = (objName != NULL);
  
  getSceneAbsolutePath(fileName);
  
  ifstream objFile(fileName, ios::in);
  if (!objFile.is_open ()){
    throw (ios::failure ("Open scene error"));
    return false;
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
	      objFile.close ();
	      return (currentObject != NULL);
	    }else{
	      if(lookForSpecificObject){
		if(!strcmp(buffer,objName)){
		  cerr << "import de " << objName << endl;
		  currentObject = object;
		  alreadyOneObject = true;
		  skip = false;
		}else
		  skip = true;
	      }else{
		currentObject = object;
		alreadyOneObject = true;
	      }
	    }
	  }else
	    currentObject = new Object(this);
	  
	  if(!detached)
	    if (!strncmp (buffer, "WSV", 3))
	      addObject(currentObject, true);
	    else
	      addObject(currentObject, false);
	  objectsAttributesSet = false;
	  nbObjectVertex = nbVertex;
	  nbObjectNormals = nbNormals;
	  nbObjectTexCoords = nbTexCoords;
	  break;
	case '\n':
	  break;
	case 'm':
	  /* Définition des matériaux */
	  objFile >> buffer >> buffer;
	  /* La définition des matériaux est évitée si l'on importe qu'un seul objet */
	  if(!lookForSpecificObject)
	    this->importMTL ((const char *) buffer);
	  break;
	case 'v':	  
	  objFile.get(lettre);
	  switch (lettre)
	    {
	    default:
	      break;
	    case ' ':
	      objFile >> x >> y >> z;
	      if(!skip){
		currentObject->addVertex(new Point(x, y, z));
		normales = coord_textures = 0;
	      }
	      nbVertex++;
	      break;
	    case 'n':
	      objFile >> x >> y >> z;	
	      if(!skip){      
		currentObject->addNormal (new Vector(x, y, z));
		normales = 1;
	      }
	      nbNormals++;
	      break;
	    case 't':
	      objFile >> x >> y;
	      if(!skip){	      
		currentObject->addTexCoord (new Point(x, y, 0));
		coord_textures = 2;
	      }
	      nbTexCoords++;
	      break;
	    }
	  break;
	case 'f':
	  if(skip){
	    objFile.getline(buffer, sizeof (buffer));
	    break;
	  }
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
// 		     << fileName
// 		     << " contient des erreurs d'indexation de points.\n";
// 		return;
// 	      }
	      currentObject->addFacet(new PointIndices(a - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex),
				      new PointIndices(b - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex),
				      new PointIndices(c - nbObjectVertex - 1, UNDEFINED, UNDEFINED, matIndex));
	      
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
// 		  cout << "Erreur de chargement : Le fichier " << fileName
// 		       << " contient des erreurs d'indexation de points.\n";
// 		  return;
// 		}
	      currentObject->addFacet(new PointIndices(a - nbObjectVertex - 1,
						 an - nbObjectNormals - 1,
						 UNDEFINED, matIndex),
				      new PointIndices(b - nbObjectVertex - 1,
						 bn - nbObjectNormals - 1,
						 UNDEFINED, matIndex),
				      new PointIndices(c - nbObjectVertex - 1,
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
// 		  cout << "Erreur de chargement : Le fichier " << fileName
// 		       << " contient des erreurs d'indexation de points.\n";
// 		  return;
// 		}
	      currentObject->addFacet(new PointIndices(a - nbObjectVertex - 1, 
						 an - nbObjectNormals - 1,
						 UNDEFINED, matIndex),
				      new PointIndices(b - nbObjectVertex - 1, 
						 bn - nbObjectNormals - 1, 
						 UNDEFINED, matIndex),
				      new PointIndices(c - nbObjectVertex - 1, 
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
// 		  cout << "Erreur de chargement : Le fichier " << fileName
// 		       << " contient des erreurs d'indexation de points.\n";
// 		  return;
// 		}
	      currentObject->addFacet(new PointIndices(a - nbObjectVertex - 1,
						 an - nbObjectNormals - 1,
						 at - nbObjectTexCoords - 1, matIndex),
				      new PointIndices(b - nbObjectVertex - 1,
						 bn - nbObjectNormals - 1,
						 bt - nbObjectTexCoords - 1, matIndex),
				      new PointIndices(c - nbObjectVertex - 1,
						 cn - nbObjectNormals - 1,
						 ct - nbObjectTexCoords - 1, matIndex));
	      
	      // if (valeurLues > 9)
// 		cout << "problème: facette non triangulaire !!!" << endl;
	      break;
	      
	    default:
	      cout << "Erreur de chargement : Le fichier " << fileName <<
		" contient des erreurs d'indexation de points.\n";
	      return false;
	      break;
	    }
	  break;
	case 'u':
	  objFile >> buffer >> buffer;
	  matIndex = getMaterialIndexByName(buffer);
	}
    }
  objFile.close ();
	      
  if(importSingleObject)
    return (currentObject != NULL);
  else
    return true;  
}

void Scene::getObjectsNameFromOBJ(const char* fileName, list<string> &objectsList, const char* prefix)
{
  char lettre;
  char buffer[255];
  string objName;
  
  int len = strlen(prefix);
  
  ifstream objFile(fileName, ios::in);
  if (!objFile.is_open ()){
    throw (ios::failure ("Open error"));
    return;
  }
  while (!objFile.eof())
    {
      objFile >> lettre;
      
      switch (lettre)
	{
	case 'g':
	  objFile >> buffer;
	  if(!strncmp(prefix,buffer,len)){
	    objName = buffer;
	    objectsList.push_back(objName);
	  }
	  break;
	default:
	  objFile.getline(buffer, sizeof (buffer));
	  break;
	}
    }
  objFile.close ();
}

bool Scene::getMTLFileNameFromOBJ(const char* fileName, char* mtlName)
{
  char lettre;
  char buffer[255];
  
  getSceneAbsolutePath(fileName);
  ifstream objFile(fileName, ios::in);
  if (!objFile.is_open ()){
    throw (ios::failure ("Open error"));
    return false;
  }
  while (!objFile.eof())
    {
      objFile >> lettre;
      switch (lettre)
	{
	case 'm':
	  /* On évite la définition des matériaux, elle doit être faite au prélable */
	  objFile >> buffer >> buffer;
	  
	  strcpy(mtlName,buffer);
	  objFile.close ();
	  return true;
	  break;
	default:
	  objFile.getline(buffer, sizeof (buffer));
	  break;
	}
    }
  objFile.close ();
  
  return false;
}

void Scene::importMTL(const char* fileName)
{
  char lettre, lettre2;
  char buffer[255];
  char texturePath[512];
  Texture *nouvelle_texture = NULL;
  string name_nouvelle_matiere;
  
  strcpy(buffer,m_currentDir);
  strcat(buffer,fileName);
  ifstream matFile(buffer, ios::in);
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
		double R, G, B;
		matFile >> R >> G >> B;
		Kd[0] = R;
		Kd[1] = G;
		Kd[2] = B;
	      }
	      break;
	    case 'a':
	      {
		double R, G, B;
		matFile >> R >> G >> B;
		Ka[0] = R;
		Ka[1] = G;
		Ka[2] = B;
	      }
	      break;
	    case 's':
	      {
		double R, G, B;
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
	  addMaterial(new Material (name_nouvelle_matiere, Ka, Kd, Ks, shini, nouvelle_texture)); //,alpha);
	  cerr << "ajout matériau :" << name_nouvelle_matiere << endl;
	  nouvelle_texture = NULL;
	  break;
	case 'm':		//map_K?
	  matFile >> buffer >> buffer;
	  //nouvelle_texture = new Texture (buffer);
	  strcpy(texturePath, m_currentDir);
	  strcat(texturePath,buffer);
	  nouvelle_texture = new Texture (wxString(texturePath, wxConvUTF8));
	  break;
	default:
	  matFile.getline(buffer, sizeof (buffer));
	  break;
	}
    }
  matFile.close();
}

int Scene::getVertexCount()
{
  int nb=0;
  for (vector < Object * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    nb += (*objectsArrayIteratorWSV)->getVertexArraySize();

  for (vector < Object * >::iterator objectsArrayIterator = m_objectsArray.begin ();
       objectsArrayIterator != m_objectsArray.end ();
       objectsArrayIterator++)
    nb += (*objectsArrayIterator)->getVertexArraySize();
  
  return nb;
}

int Scene::getPolygonsCount()
{
  int nb=0;
  for (vector < Object * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    nb += (*objectsArrayIteratorWSV)->getPolygonsCount();

  for (vector < Object * >::iterator objectsArrayIterator = m_objectsArray.begin ();
       objectsArrayIterator != m_objectsArray.end ();
       objectsArrayIterator++)
    nb += (*objectsArrayIterator)->getPolygonsCount();
  
  return nb;
}

int Scene::getMaterialIndexByName(const char *name)
{ 
  int index=0;
  for (vector<Material*>::iterator materialArrayIterator = m_materialArray.begin ();
       materialArrayIterator != m_materialArray.end ();
       materialArrayIterator++)
    {
      if (!strcmp (name, (*materialArrayIterator)->getName()->c_str ()))
	return index;
      
      index++;
    } 
  cerr << "Error loading unknown material " << name << endl;
  return getMaterialIndexByName("default");
}

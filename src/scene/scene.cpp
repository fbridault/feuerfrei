#include "scene.hpp"

#include <fstream>

#include "material.hpp"

Scene::Scene (const char* const fileName, FireSource **flames, int nbFlames)
{  
  m_flames = flames;
  m_nbFlames = nbFlames;
  
  addMaterial(new Material(this));
  cerr << "Chargement de la scène " << fileName << endl;
  importOBJ(fileName);
  m_boundingSpheresMode=false;
  //sortTransparentObjects();
}

// void Scene::sortTransparentObjects()
// {  
//   int i;
//   int size;
//   Object* object;
  
//   size = m_objectsArray.size(); i=0;
//   for (vector<Object*>::iterator objectsArrayIterator = m_objectsArray.begin();
//        i < size; i++,objectsArrayIterator++)
//     if ((*objectsArrayIterator)->isTransparent ())
//       {
// 	object = *objectsArrayIterator;
// 	objectsArrayIterator =  m_objectsArray.erase(objectsArrayIterator); 
// 	m_objectsArray.push_back(object);
//       }
  
//   size = m_objectsArrayWSV.size(); i=0;
//   for (vector<Object*>::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin();
//        i < size; i++,objectsArrayIteratorWSV++)
//     if ((*objectsArrayIteratorWSV)->isTransparent ())
//       {
// 	object = *objectsArrayIteratorWSV;
// 	objectsArrayIteratorWSV =  m_objectsArrayWSV.erase(objectsArrayIteratorWSV); 
// 	m_objectsArrayWSV.push_back(object);
//       }  
// }

void Scene::computeVisibility(const Camera &view)
{
  for (vector<Object*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->computeVisibility(view);
  for (vector<Object*>::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->computeVisibility(view);
}

void Scene::createVBOs(void)
{
  for (vector<Object*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++){
    (*objectsArrayIterator)->buildVBO();
    (*objectsArrayIterator)->buildBoundingSpheres();
  }
  for (vector<Object*>::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end();
       objectsArrayIteratorWSV++){
    (*objectsArrayIteratorWSV)->buildVBO();
    (*objectsArrayIteratorWSV)->buildBoundingSpheres();
  }
  
  cout << "Terminé" << endl;
  cout << "*******************************************" << endl;
  cout << "Statistiques sur la scène :" << endl;
  cout << getObjectsCount() << " objets" << endl;
  cout << getPolygonsCount() << " polygones" << endl;
  cout << getVertexCount() << " vertex" << endl;
  cout << "*******************************************" << endl;
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
  
  for (vector < Texture * >::iterator texturesArrayIterator = m_texturesArray.begin ();
       texturesArrayIterator != m_texturesArray.end (); texturesArrayIterator++)
    delete (*texturesArrayIterator);
  m_texturesArray.clear ();
  
//   glDeleteLists(m_displayLists[0],NB_DISPLAY_LISTS);
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

bool Scene::importOBJ(const char* fileName, Object* object, const char* objName)
{
  bool alreadyOneObject = false, skip = false;
  bool objectsAttributesSet=false;
  bool importSingleObject = (object != NULL);
  bool lookForSpecificObject = (objName != NULL);
  /* Indique qu'un maillage ou un objet a été créé, utile pour ajouter les informations géométriques une fois */
  /* qu'elles sont toutes lues car les objets sont crées auparavant. */
  bool meshCreated=false, objectCreated=false;

  char lettre,drop;
  char buffer[255];
  uint coord_textures = 0, normales = 0;
  int nbVertex=0, nbNormals=0, nbTexCoords=0;
  int nbObjectVertex=0, nbObjectNormals=0, nbObjectTexCoords=0;
  int a, b, c, an, bn, cn, at, bt, ct, matIndex=0;
  double x, y, z;
  
  Vertex currentVertex;
  Object* currentObject=NULL;
  Mesh* currentMesh=NULL;
  
  /**<Liste des normales de l'objet */
  vector < Vector >normalsVector;
  /**<Liste des coordonnées de textures de l'objet */
  vector < Point  >texCoordsVector;
  /**<Liste des indices des normales des facettes */
  vector < GLuint >normalsIndexVector;  
  /**<Liste des indices des coordonnées de textures des facettes */
  vector < GLuint >texCoordsIndexVector;
  
  getSceneAbsolutePath(fileName);
  
  ifstream objFile(fileName, ios::in);
  if (!objFile.is_open ()){
    throw (ios::failure ("Open scene error"));
    return false;
  }
  
  while (!objFile.eof())
    {
      objFile >> lettre;
      if(objFile.eof()) break;
      switch (lettre)
	{
	default:
	  objFile.getline(buffer, sizeof (buffer));
	  break;
	case 'g':
	  /* Définition d'un nouvel objet. */
	  /* Un nouveau matériau est appliqué, nous devons donc créer un nouveau Mesh. */
	  /* Cependant, on commence d'abord par valider les données du Mesh précédent. */
	  if(meshCreated){
	    /* On valide les données du dernier Mesh. */
	    currentMesh->setUVsAndNormals(normalsVector, normalsIndexVector, texCoordsVector, texCoordsIndexVector);
	    normalsIndexVector.clear();
	    texCoordsIndexVector.clear();
	    meshCreated = false;
	  }
	  if(objectCreated){
	    normalsVector.clear();
	    texCoordsVector.clear();
	  }
  
	  objFile >> buffer;
	  if(importSingleObject){
	    /* On importe un seul objet. */
	    if(alreadyOneObject){
	      /* Un objet a déjà été importé, inutile de continuer. */
	      objFile.close ();
	      return true;
	    }else{
	      if(lookForSpecificObject){
		/* On recherche un objet en particulier. */
		if(!strcmp(buffer,objName)){
		  /* Objet trouvé ! */
		  cerr << "import of " << objName << endl;
		  currentObject = object;
		  alreadyOneObject = true;
		  objectCreated = true;
		  skip = false;
		}else
		  skip = true;
	      }else{
		/* Sinon on prend le premier objet dans le fichier. */
		currentObject = object;
		alreadyOneObject = true;
		objectCreated = true;
	      }
	    }
	  }else{
	    /* On est en train d'importer tous les objets. */
	    currentObject = new Object(this);
	    objectCreated = true;
	    
	    if (!strncmp (buffer, "WSV", 3))
	      addObject(currentObject, true);
	    else
	      addObject(currentObject, false);
	  }
	  nbObjectVertex = nbVertex;
	  nbObjectNormals = nbNormals;
	  nbObjectTexCoords = nbTexCoords;
	  break;
	case 'u':
	  /* Un nouveau matériau est appliqué, nous devons donc créer un nouveau Mesh. */
	  /* Cependant, on commence d'abord par valider les données du Mesh précédent. */
	  if(meshCreated){
	    /* On valide les données du dernier Mesh. */
	    currentMesh->setUVsAndNormals(normalsVector, normalsIndexVector, texCoordsVector, texCoordsIndexVector);	    
	    normalsIndexVector.clear();
	    texCoordsIndexVector.clear();
	  }
  
	  /* Création du nouveau mesh. */
	  objFile >> buffer >> buffer;
	  if(!skip){
	    matIndex = getMaterialIndexByName(buffer);
	    currentMesh = new Mesh(this, matIndex, currentObject);
	    currentObject->addMesh(currentMesh);
	    meshCreated = true;
	    objectsAttributesSet = false;
	  }
	  break;
	case 'm':
	  /* Définition des matériaux */
	  objFile >> buffer >> buffer;
	  /* La définition des matériaux est évitée si l'on importe qu'un seul objet. */
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
	      objFile >> currentVertex.x >> currentVertex.y >> currentVertex.z;
	      if(!skip)
		currentObject->addVertex(currentVertex);
	      nbVertex++;
	      break;
	    case 'n':
	      objFile >> x >> y >> z;
	      if(!skip)
		normalsVector.push_back(Vector(x, y, z));
	      nbNormals++;
	      break;
	    case 't':
	      objFile >> x >> y;
	      if(!skip)
		/* On inverse la coordonnée y */
		texCoordsVector.push_back(Point(x, -y, 0));
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
	    if(!normalsVector.empty())
	      normales = 1;
	    else
	      normales = 0;
	    if(!texCoordsVector.empty())
	      coord_textures = 2;
	    else
	      coord_textures = 0;
	    currentMesh->setAttributes(coord_textures + normales);
	    currentObject->allocHashTable();
	  }
	  switch (coord_textures + normales)
	    {
	    case 0:
	      objFile >> a >> b >> c;
	      currentMesh->addIndex(a - nbObjectVertex - 1);
	      currentMesh->addIndex(b - nbObjectVertex - 1);
	      currentMesh->addIndex(c - nbObjectVertex - 1);
	      break;
	    case 1:
	      objFile >> a >> drop >> drop >> an;
	      objFile >> b >> drop >> drop >> bn;
	      objFile >> c >> drop >> drop >> cn;
	      currentMesh->addIndex(a - nbObjectVertex - 1);
	      currentMesh->addIndex(b - nbObjectVertex - 1);
	      currentMesh->addIndex(c - nbObjectVertex - 1);
	      
	      normalsIndexVector.push_back(an - nbObjectNormals - 1);
	      normalsIndexVector.push_back(bn - nbObjectNormals - 1);
	      normalsIndexVector.push_back(cn - nbObjectNormals - 1);
	      break;
	    case 2:
	      objFile >> a >> drop >> an;
	      objFile >> b >> drop >> bn;
	      objFile >> c >> drop >> cn;
	      currentMesh->addIndex(a - nbObjectVertex - 1);
	      currentMesh->addIndex(b - nbObjectVertex - 1);
	      currentMesh->addIndex(c - nbObjectVertex - 1);
	      
	      normalsIndexVector.push_back(an - nbObjectNormals - 1);
	      normalsIndexVector.push_back(bn - nbObjectNormals - 1);
	      normalsIndexVector.push_back(cn - nbObjectNormals - 1);
 	      break;
	    case 3:
	      objFile >> a >> drop >> at >> drop >> an;
	      objFile >> b >> drop >> bt >> drop >> bn;
	      objFile >> c >> drop >> ct >> drop >> cn;
	      currentMesh->addIndex(a - nbObjectVertex - 1);
	      currentMesh->addIndex(b - nbObjectVertex - 1);
	      currentMesh->addIndex(c - nbObjectVertex - 1);
	      
	      normalsIndexVector.push_back(an - nbObjectNormals - 1);
	      normalsIndexVector.push_back(bn - nbObjectNormals - 1);
	      normalsIndexVector.push_back(cn - nbObjectNormals - 1);
	      
	      texCoordsIndexVector.push_back(at - nbObjectTexCoords - 1);
	      texCoordsIndexVector.push_back(bt - nbObjectTexCoords - 1);
	      texCoordsIndexVector.push_back(ct - nbObjectTexCoords - 1);
	      break;	      
	    default:
	      cout << "Erreur de chargement : Le fichier " << fileName << " contient des erreurs d'indexation de points.\n";
	      return false;
	      break;
	    }
	  break;
	case '\n':
	case ' ':
	  break;
	}
    }
  objFile.close ();
  if(meshCreated){
    /* On valide les données du dernier Mesh. */
    currentMesh->setUVsAndNormals(normalsVector, normalsIndexVector, texCoordsVector, texCoordsIndexVector);
    normalsIndexVector.clear();
    texCoordsIndexVector.clear();
  }
  if(objectCreated){
    normalsVector.clear();
    texCoordsVector.clear();
  }
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
  int nouvelle_texture = -1;
  string name_nouvelle_matiere;
  bool newMat=false;
  
  strcpy(buffer,m_currentDir);
  strcat(buffer,fileName);
  ifstream matFile(buffer, ios::in);
  if (!matFile.is_open ()){
    throw (ios::failure ("Open error"));
    return;
  }
  
  double Kd[3], Ka[3], Ks[3], alpha, shini;
  
  /* Problème: selon l'exportateur, les champs ne sont pas écrits dans le même ordre */
  /* On est donc sûr d'avoir lu tous les champs d'un matériau que lorsque l'on arrive */
  /* au matériau suivant ou à la fin du fichier/ */
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
	  /* On ajoute donc d'abord le matériau précédemment trouvé */
	  if(newMat){
	    addMaterial(new Material (this, name_nouvelle_matiere, Ka, Kd, Ks, shini, nouvelle_texture)); //,alpha);
	    nouvelle_texture = -1;
	    newMat=false;
	  }
	  Ks[0] = Ks[1] = Ks[2] = Kd[0] = Kd[1] = Kd[2] = Ka[0] = Ka[1] = Ka[2] = 0.0;
	  alpha = 1.0;
	  shini = 0.0;
	  matFile >> buffer >> buffer;
	  name_nouvelle_matiere = buffer;
	  newMat = true;
	  break;
	case 'i':		// ignoré pour le moment
	  break;
	case 'm':		//map_K?
	  matFile >> buffer >> buffer;
	  //nouvelle_texture = new Texture (buffer);
	  strcpy(texturePath, m_currentDir);
	  strcat(texturePath,buffer);
	  if( (nouvelle_texture = searchTextureIndexByName(texturePath)) == -1) {
	    nouvelle_texture = addTexture(new Texture (wxString(texturePath, wxConvUTF8)));
	  }
	  break;
	default:
	  matFile.getline(buffer, sizeof (buffer));
	  break;
	}
    }
  /* On ajoute enfin le dernier matériau trouvé */
  if(newMat){
    addMaterial(new Material (this, name_nouvelle_matiere, Ka, Kd, Ks, shini, nouvelle_texture)); //,alpha);
    nouvelle_texture = -1;
    newMat=false;
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
      if ( !(*materialArrayIterator)->getName()->compare (name) ){
	return index;
      }
      
      index++;
    } 
  cerr << "Error loading unknown material " << name << endl;
  return getMaterialIndexByName("default");
}

int Scene::searchTextureIndexByName(const char *name)
{ 
  int index=0;
  for (vector<Texture*>::iterator texturesArrayIterator = m_texturesArray.begin ();
       texturesArrayIterator != m_texturesArray.end ();
       texturesArrayIterator++)
    {
      if ( !strcmp((*texturesArrayIterator)->getName().fn_str(), name) ) {
	return index;
      }
      index++;
    }
  return -1;
}

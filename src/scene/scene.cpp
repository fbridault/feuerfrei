#include "scene.hpp"

#include <fstream>

#include "material.hpp"

Scene::Scene (const char* const fileName, vector <Luminary *> *luminaries, vector <FireSource *> *flames)
{
  m_luminaries = luminaries;
  m_flames = flames;
  
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

void Scene::computeBoundingBox(Point& max, Point& min)
{ 
  Point ptMax(FLT_MIN, FLT_MIN, FLT_MIN), ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
  Point objMax, objMin;
  
  for (vector<Object*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++){
    (*objectsArrayIterator)->getBoundingBox(objMax, objMin);
    /* Calcul du max */
    if ( objMax.x > ptMax.x)
      ptMax.x = objMax.x;
    if ( objMax.y > ptMax.y)
      ptMax.y = objMax.y;
    if ( objMax.z > ptMax.z)
      ptMax.z = objMax.z;
    /* Calcul du min */
    if ( objMin.x < ptMin.x)
      ptMin.x = objMin.x;
    if ( objMin.y < ptMin.y)
      ptMin.y = objMin.y;
    if ( objMin.z < ptMin.z)
      ptMin.z = objMin.z;
  }
  for (vector<Object*>::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end();
       objectsArrayIteratorWSV++){
    (*objectsArrayIteratorWSV)->getBoundingBox(objMax, objMin);
    /* Calcul du max */
    if ( objMax.x > ptMax.x)
      ptMax.x = objMax.x;
    if ( objMax.y > ptMax.y)
      ptMax.y = objMax.y;
    if ( objMax.z > ptMax.z)
      ptMax.z = objMax.z;
    /* Calcul du min */
    if ( objMin.x < ptMin.x)
      ptMin.x = objMin.x;
    if ( objMin.y < ptMin.y)
      ptMin.y = objMin.y;
    if ( objMin.z < ptMin.z)
      ptMin.z = objMin.z;
  }
  max = ptMax; min = ptMin;
}

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
  for (vector < FireSource* >::const_iterator flamesIterator = m_flames->begin ();
       flamesIterator != m_flames->end (); flamesIterator++)
    (*flamesIterator)->computeVisibility(view);
  // On trie les flammes en fonction de leur distance pour éviter les problèmes
  // de transparence avec le glow
  sort(m_flames->begin(),m_flames->end(),FireSource::cmp);
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
}

void Scene::getSceneAbsolutePath(const char* const fileName)
{
  bool found=false;
  
  cerr << fileName << endl;
  if(strlen(fileName) > 255)
    cerr << "Dir string too long" << endl;
  
  /* On parcourt la chaîne en partant de la fin */
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

bool Scene::importOBJ(const char* fileName, list <Object*>* const objectsList,
		      list<Wick*>* const wicksList, const char* prefix)
{
  bool skip = false;
  bool objectsAttributesSet=false;
  bool storeObjectsInList = (objectsList != NULL), storeWicksInList = (wicksList != NULL);
  bool lookForSpecificObjects = (prefix != NULL);
  uint prefixlen;
  /* Indique qu'un maillage ou un objet a été créé, utile pour ajouter les informations géométriques une fois */
  /* qu'elles sont toutes lues car les objets sont crées auparavant. */
  bool meshCreated=false, objectCreated=false;
  bool firstMesh = false;
  char lettre,dump;
  char buffer[255];
  uint coord_textures = 0, normales = 0;
  int nbVertex=0, nbNormals=0, nbTexCoords=0;
  int nbObjectVertex=0, nbObjectNormals=0, nbObjectTexCoords=0;
  int a, b, c, an, bn, cn, at, bt, ct, matIndex=0;
  float x, y, z;
  
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
  
  if(lookForSpecificObjects) prefixlen = strlen(prefix);
  getSceneAbsolutePath(fileName);
  
  ifstream objFile(fileName, ios::in);
  if (!objFile.is_open ()){
    cerr << "Can't open file " << fileName << endl;
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

	  if(storeObjectsInList || storeWicksInList){
	    if(lookForSpecificObjects){
	      /* On recherche un objet en particulier. */
	      if(!strncmp(buffer,prefix,prefixlen)){
		/* Objet trouvé ! */
		if(storeObjectsInList){
		  currentObject = new Object(this);
		  objectsList->push_back(currentObject);
		}else{
		  currentObject = new Wick(this);
		  wicksList->push_back((Wick* )currentObject);
		}
		objectCreated = true;
		firstMesh = true;
		skip = false;
	      }else
		skip = true;
	    }else{
	      /* Sinon on prend tous les objets dans le fichier. */
	      if(storeObjectsInList){
		currentObject = new Object(this);
		objectsList->push_back(currentObject);
	      }else{
		currentObject = new Wick(this);
		wicksList->push_back((Wick *)currentObject);
	      }
	      objectCreated = true;
	      firstMesh = true;
	    }
	  }
	  else{
	    currentObject = new Object(this);
	    objectCreated = true;
	    firstMesh = true;
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
	    firstMesh = false;
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
	  if(!lookForSpecificObjects)
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
	    if(firstMesh){
	      /* On ne doit allouer la table de hachage qu'une seule fois */
	      currentObject->allocHashTable();
	      /* On initialise la table de hachage */
	      currentObject->initHashTable();
	    }
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
	      objFile >> a >> dump >> dump >> an;
	      objFile >> b >> dump >> dump >> bn;
	      objFile >> c >> dump >> dump >> cn;
	      currentMesh->addIndex(a - nbObjectVertex - 1);
	      currentMesh->addIndex(b - nbObjectVertex - 1);
	      currentMesh->addIndex(c - nbObjectVertex - 1);
	      
	      normalsIndexVector.push_back(an - nbObjectNormals - 1);
	      normalsIndexVector.push_back(bn - nbObjectNormals - 1);
	      normalsIndexVector.push_back(cn - nbObjectNormals - 1);
	      break;
	    case 2:
	      objFile >> a >> dump >> an;
	      objFile >> b >> dump >> bn;
	      objFile >> c >> dump >> cn;
	      currentMesh->addIndex(a - nbObjectVertex - 1);
	      currentMesh->addIndex(b - nbObjectVertex - 1);
	      currentMesh->addIndex(c - nbObjectVertex - 1);
	      
	      normalsIndexVector.push_back(an - nbObjectNormals - 1);
	      normalsIndexVector.push_back(bn - nbObjectNormals - 1);
	      normalsIndexVector.push_back(cn - nbObjectNormals - 1);
 	      break;
	    case 3:
	      objFile >> a >> dump >> at >> dump >> an;
	      objFile >> b >> dump >> bt >> dump >> bn;
	      objFile >> c >> dump >> ct >> dump >> cn;
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
  if(storeObjectsInList)
    return (objectsList->size() > 0);
  else
    if(storeWicksInList)
      return (wicksList->size() > 0);
  else
    return true;
}

bool Scene::getMTLFileNameFromOBJ(const char* fileName, char* mtlName)
{
  char lettre;
  char buffer[255];
  
  getSceneAbsolutePath(fileName);
  ifstream objFile(fileName, ios::in);
  if (!objFile.is_open ()){
    cerr << "Can't open file " << fileName << endl;
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
    cerr << "Can't open file " << fileName << endl;
    throw (ios::failure ("Open error"));
    return;
  }
  
  float Kd[3], Ka[3], Ks[3], alpha, shini;
  
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

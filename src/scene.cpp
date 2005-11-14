#include "scene.hpp"

#define ALL      0
#define TEXTURED 1
#define FLAT    -1

/* Construction/Destruction */
CScene::CScene (const char* const filename)
{  
  double coeff[3] = { .8, .8, .8 };
  
  addMaterial(new CMaterial("default",NULL, coeff, NULL, 0));
  cerr << "Chargement de la scène " << filename << endl;
  COBJReader objReader(filename, this);
}

void CScene::loadObject(const char *filename, CObject* const newObj, bool detached)
{ 
  cerr << "Chargement de l'objet " << filename << endl;
  COBJReader import(filename, this, newObj, detached);
}

void CScene::createDisplayLists(void)
{
  glNewList(SCENE_OBJECTS,GL_COMPILE);
  for (vector<CObject*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(ALL,true);
  glEndList();
  
  glNewList(SCENE_OBJECTS_TEX,GL_COMPILE);
  for (vector<CObject*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(TEXTURED,true);
  glEndList();
  
  glNewList(SCENE_OBJECTS_WTEX,GL_COMPILE);
  for (vector<CObject*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(FLAT,false);
  glEndList();  
  
  glNewList(SCENE_OBJECTS_WSV_TEX,GL_COMPILE); 
  for (vector<CObject*>::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->draw(TEXTURED,true);
  glEndList();  
  
  /* Création de la display list des objets qui projettent des ombres sans les textures */
  /* Ce qui permet d'aller plus vite lors de la génération des shadow volumes */
  glNewList(SCENE_OBJECTS_WSV_WTEX,GL_COMPILE); 
  for (vector<CObject*>::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->draw(FLAT,false);
  glEndList();	
  
  glNewList (SCENE_OBJECTS_WT, GL_COMPILE);
  for (vector < CObject * >::iterator objectsArrayIterator = m_objectsArray.begin ();
       objectsArrayIterator != m_objectsArray.end ();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw (ALL,false);
  glEndList ();
  
  /* Création de la display list des objets qui projettent des ombres */
  glNewList (SCENE_OBJECTS_WSV, GL_COMPILE);
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
      (*objectsArrayIteratorWSV)->draw (ALL,true);
  glEndList ();
  
  /* Création de la display list des objets qui projettent des ombres sans les textures */
  /* Ce qui permet d'aller plus vite lors de la génération des shadow volumes */
  glNewList (SCENE_OBJECTS_WSV_WT, GL_COMPILE);  
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->draw (ALL,false);
  glEndList ();
  
  cout << "Terminé" << endl;
  cout << "*******************************************" << endl;
  cout << "Statistiques sur la scène :" << endl;
  cout << getObjectsCount() << " objets" << endl;
  cout << getPolygonsCount() << " polygones" << endl;
  cout << getVertexCount() << " vertex" << endl;
  cout << "*******************************************" << endl;
}

CScene::~CScene ()
{
  for (vector < CObject * >::iterator objectsArrayIterator = m_objectsArray.begin ();
       objectsArrayIterator != m_objectsArray.end ();
       objectsArrayIterator++)
    delete (*objectsArrayIterator);
  m_objectsArray.clear ();
  
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    delete (*objectsArrayIteratorWSV);
  m_objectsArrayWSV.clear ();

  for (vector < CSource * >::iterator lightSourcesIterator = m_lightSourcesArray.begin ();
       lightSourcesIterator != m_lightSourcesArray.end (); lightSourcesIterator++)
    delete (*lightSourcesIterator);
  m_lightSourcesArray.clear ();

  for (vector < CMaterial * >::iterator materialArrayIterator = m_materialArray.begin ();
       materialArrayIterator != m_materialArray.end (); materialArrayIterator++)
    delete (*materialArrayIterator);
  m_materialArray.clear ();
}

int CScene::getVertexCount()
{
  int nb=0;
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    nb += (*objectsArrayIteratorWSV)->getVertexArraySize();

  for (vector < CObject * >::iterator objectsArrayIterator = m_objectsArray.begin ();
       objectsArrayIterator != m_objectsArray.end ();
       objectsArrayIterator++)
    nb += (*objectsArrayIterator)->getVertexArraySize();
  
  return nb;
}

int CScene::getPolygonsCount()
{
  int nb=0;
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    nb += (*objectsArrayIteratorWSV)->getPolygonsCount();

  for (vector < CObject * >::iterator objectsArrayIterator = m_objectsArray.begin ();
       objectsArrayIterator != m_objectsArray.end ();
       objectsArrayIterator++)
    nb += (*objectsArrayIterator)->getPolygonsCount();
  
  return nb;
}

int CScene::getMaterialIndexByName(const char *name)
{ 
  int index=0;
  for (vector<CMaterial*>::iterator materialArrayIterator = m_materialArray.begin ();
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

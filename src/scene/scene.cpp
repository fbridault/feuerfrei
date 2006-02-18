#include "scene.hpp"

/* Construction/Destruction */
CScene::CScene (const char* const filename, Flame **flames, int nbFlames)
{  
  m_flames = flames;
  m_nbFlames = nbFlames;
  
  addMaterial(new CMaterial());
  cerr << "Chargement de la scène " << filename << endl;
  COBJReader objReader(filename, *this);
}

void CScene::loadObject(const char *filename, CObject* const newObj, bool detached)
{ 
  cerr << "Chargement de l'objet " << filename << endl;
  COBJReader import(filename, *this, newObj, detached);
}

void CScene::createDisplayLists(void)
{
  m_displayLists[0] = glGenLists(NB_DISPLAY_LISTS);
  for(int i=1; i<8; i++)
    m_displayLists[i] = m_displayLists[0] + i;
  
  glNewList(m_displayLists[0],GL_COMPILE);
  for (vector<CObject*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(ALL,true);
  glEndList();
  
  glNewList(m_displayLists[1],GL_COMPILE);
  for (vector<CObject*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(TEXTURED,true);
  glEndList();
  
  glNewList(m_displayLists[2],GL_COMPILE);
  for (vector<CObject*>::iterator objectsArrayIterator = m_objectsArray.begin();
       objectsArrayIterator != m_objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(FLAT,false);
  glEndList();  
  
  glNewList(m_displayLists[3],GL_COMPILE); 
  for (vector<CObject*>::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->draw(TEXTURED,true);
  glEndList();  
  
  /* Création de la display list des objets qui projettent des ombres sans les textures */
  /* Ce qui permet d'aller plus vite lors de la génération des shadow volumes */
  glNewList(m_displayLists[4],GL_COMPILE); 
  for (vector<CObject*>::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->draw(FLAT,false);
  glEndList();	
  
  glNewList (m_displayLists[7], GL_COMPILE);
  for (vector < CObject * >::iterator objectsArrayIterator = m_objectsArray.begin ();
       objectsArrayIterator != m_objectsArray.end ();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw (AMBIENT,false);
  glEndList ();
  
  /* Création de la display list des objets qui projettent des ombres */
  glNewList (m_displayLists[5], GL_COMPILE);
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != m_objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
      (*objectsArrayIteratorWSV)->draw (ALL,true);
  glEndList ();
  
  /* Création de la display list des objets qui projettent des ombres sans les textures */
  /* Ce qui permet d'aller plus vite lors de la génération des shadow volumes */
  glNewList (m_displayLists[6], GL_COMPILE);  
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = m_objectsArrayWSV.begin ();
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
  
  glDeleteLists(m_displayLists[0],NB_DISPLAY_LISTS);
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

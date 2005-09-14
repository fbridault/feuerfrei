#include "scene.hpp"

#define ALL      0
#define TEXTURED 1
#define FLAT    -1

/* Construction/Destruction */
CScene::CScene (const char* const filename, Flame **flammes, int nb_flammes)
{
  cout << "Chargement de la scène...";
  importOBJFile2Scene (filename, this);
  
  glNewList(SCENE_OBJECTS,GL_COMPILE);
  for (vector<CObject*>::iterator objectsArrayIterator = objectsArray.begin();
       objectsArrayIterator != objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(ALL,true);
  for (int f = 0; f < nb_flammes; f++)
    flammes[f]->getLuminaire()->draw(ALL,true);
  glEndList();
  
  cout << "SCENE_OBJECTS_TEX" << endl;
  glNewList(SCENE_OBJECTS_TEX,GL_COMPILE);
  for (vector<CObject*>::iterator objectsArrayIterator = objectsArray.begin();
       objectsArrayIterator != objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(TEXTURED,true);
  glEndList();
  
  cout << "SCENE_OBJECTS_WTEX" << endl;
  glNewList(SCENE_OBJECTS_WTEX,GL_COMPILE);
  for (vector<CObject*>::iterator objectsArrayIterator = objectsArray.begin();
       objectsArrayIterator != objectsArray.end();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw(FLAT,false);
  for (int f = 0; f < nb_flammes; f++)
    flammes[f]->getLuminaire()->draw(FLAT,false);
  glEndList();
  
  cout << "SCENE_OBJECTS_WSV_TEX" << endl;
  glNewList(SCENE_OBJECTS_WSV_TEX,GL_COMPILE); 
  for (vector<CObject*>::iterator objectsArrayIteratorWSV = objectsArrayWSV.begin();
       objectsArrayIteratorWSV != objectsArrayWSV.end();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->draw(TEXTURED,true);
  glEndList();
  
  /* Création de la display list des objets qui projettent des ombres sans les textures */
  /* Ce qui permet d'aller plus vite lors de la génération des shadow volumes */
  cout << "SCENE_OBJECTS_WSV_WTEX" << endl;
  glNewList(SCENE_OBJECTS_WSV_WTEX,GL_COMPILE); 
  for (vector<CObject*>::iterator objectsArrayIteratorWSV = objectsArrayWSV.begin();
       objectsArrayIteratorWSV != objectsArrayWSV.end();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->draw(FLAT,false);
  for (int f = 0; f < nb_flammes; f++)
    flammes[f]->getLuminaire()->draw(FLAT,false);
  glEndList();	

  cout << "SCENE_OBJECTS_WT" << endl;
  glNewList (SCENE_OBJECTS_WT, GL_COMPILE);
  for (vector < CObject * >::iterator objectsArrayIterator = objectsArray.begin ();
       objectsArrayIterator != objectsArray.end ();
       objectsArrayIterator++)
    (*objectsArrayIterator)->draw (ALL,false);
  for (int f = 0; f < nb_flammes; f++)
    flammes[f]->getLuminaire()->draw(ALL,false);
  glEndList ();

  cout << "SCENE_OBJECTS_WSV" << endl;
  /* Création de la display list des objets qui projettent des ombres */
  glNewList (SCENE_OBJECTS_WSV, GL_COMPILE);
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
      (*objectsArrayIteratorWSV)->draw (ALL,true);
  for (int f = 0; f < nb_flammes; f++)
    flammes[f]->getLuminaire()->draw(ALL,true);
  glEndList ();
  
  /* Création de la display list des objets qui projettent des ombres sans les textures */
  /* Ce qui permet d'aller plus vite lors de la génération des shadow volumes */
  cout << "SCENE_OBJECTS_WSV_WT" << endl;
  glNewList (SCENE_OBJECTS_WSV_WT, GL_COMPILE);  
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    (*objectsArrayIteratorWSV)->draw (ALL,false);
  for (int f = 0; f < nb_flammes; f++)
    flammes[f]->getLuminaire()->draw(ALL,false);
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
  for (vector < CObject * >::iterator objectsArrayIterator = objectsArray.begin ();
       objectsArrayIterator != objectsArray.end ();
       objectsArrayIterator++)
    delete (*objectsArrayIterator);
  objectsArray.clear ();
  
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    delete (*objectsArrayIteratorWSV);
  objectsArrayWSV.clear ();

  for (vector < CSource * >::iterator lightSourcesIterator = lightSourcesArray.begin ();
       lightSourcesIterator != lightSourcesArray.end (); lightSourcesIterator++)
    delete (*lightSourcesIterator);
  lightSourcesArray.clear ();
}

int CScene::getVertexCount()
{
  int nb=0;
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    nb += (*objectsArrayIteratorWSV)->getVertexArraySize();

  for (vector < CObject * >::iterator objectsArrayIterator = objectsArray.begin ();
       objectsArrayIterator != objectsArray.end ();
       objectsArrayIterator++)
    nb += (*objectsArrayIterator)->getVertexArraySize();
  
  return nb;
}


int CScene::getPolygonsCount()
{
  int nb=0;
  for (vector < CObject * >::iterator objectsArrayIteratorWSV = objectsArrayWSV.begin ();
       objectsArrayIteratorWSV != objectsArrayWSV.end ();
       objectsArrayIteratorWSV++)
    nb += (*objectsArrayIteratorWSV)->getPolygonsCount();

  for (vector < CObject * >::iterator objectsArrayIterator = objectsArray.begin ();
       objectsArrayIterator != objectsArray.end ();
       objectsArrayIterator++)
    nb += (*objectsArrayIterator)->getPolygonsCount();
  
  return nb;
}

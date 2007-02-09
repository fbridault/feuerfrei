#include "object.hpp"

#include "scene.hpp"

Object::Object(Scene *scene)
{
  m_nbVertex = 0;
  m_nbNormals = 0;
  m_nbTexCoords = 0;
  m_scene = scene;
}

Object::~Object ()
{
  delete [] m_vertexArray;
  if(m_nbNormals) delete [] m_normalsArray;
  if(m_nbTexCoords) delete [] m_texCoordsArray;
  
  for (list <Mesh* >::iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    delete (*meshesListIterator);
  m_meshesList.clear ();
}

void Object::getBoundingBox (Point & max, Point & min)
{
  Point ptMax(DBL_MIN, DBL_MIN, DBL_MIN), ptMin(DBL_MAX, DBL_MAX, DBL_MAX);
  /* Création de la bounding box */
  for (int i=0; i < m_nbVertex; i+=3)
    {
      /* Clacul du max */
      if ( m_vertexArray[i] > ptMax.x)
	ptMax.x = m_vertexArray[i];
      if ( m_vertexArray[i+1] > ptMax.y)
	ptMax.y = m_vertexArray[i+1];
      if ( m_vertexArray[i+2] > ptMax.z)
	ptMax.z = m_vertexArray[i+2];
      /* Calcul du min */
      if ( m_vertexArray[i] < ptMin.x)
	ptMin.x = m_vertexArray[i];
      if ( m_vertexArray[i+1] < ptMin.y)
	ptMin.y = m_vertexArray[i+1];
      if ( m_vertexArray[i+2] < ptMin.z)
	ptMin.z = m_vertexArray[i+2];
  }
  
  max = ptMax; min = ptMin;
}

uint Object::getPolygonsCount ()
{
  uint count=0;
  for (list <Mesh* >::iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    count += (*meshesListIterator)->getPolygonsCount();
  return count;
}

void Object::draw (char drawCode, bool tex)
{
  /* On initialise le dernier matériau au premier de la liste, le matériau par défaut */
  uint lastMaterialIndex=0;
  
  /* Parcours de la liste des meshes */
  for (list <Mesh* >::iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    (*meshesListIterator)->draw(drawCode, tex, lastMaterialIndex);
  
  /* On désactive l'unité de texture le cas échéant */
  if(m_scene->getMaterial(lastMaterialIndex)->hasDiffuseTexture() && tex){
    glDisable(GL_TEXTURE_2D);
  }
}

/********************************************* Mesh Definition *********************************************/
Mesh::Mesh (Scene* const scene, uint materialIndex, Object* parent)
{
  m_scene = scene;
  m_attributes = 0;
  m_materialIndex = materialIndex;
  m_nbVertexIndex = 0;
  m_nbNormalsIndex = 0;
  m_nbTexCoordsIndex = 0;    
  m_parent = parent;
}

Mesh::~Mesh ()
{
  delete [] m_vertexIndexArray;
  if(m_nbNormalsIndex) delete [] m_normalsIndexArray;
  if(m_nbTexCoordsIndex) delete [] m_texCoordsIndexArray;
}

void Mesh::draw (char drawCode, bool tex, uint &lastMaterialIndex)
{  
  uint vertexIndex, normalIndex, texCoordIndex;
  
  if(drawCode == TEXTURED){
    /* Ne dessiner que si il y a une texture */
    if(!m_scene->getMaterial(m_materialIndex)->hasDiffuseTexture())
      return;
  }else
    if(drawCode == FLAT){
      /* Ne dessiner que si il n'y a pas de texture */
      if(m_scene->getMaterial(m_materialIndex)->hasDiffuseTexture())
	return;
    }
  
  if(drawCode == AMBIENT)
    /* Dessiner avec le matériau par défaut (pour tester les zones d'ombres par exemple) */
    m_scene->getMaterial(0)->apply();
  else
    if( m_materialIndex != lastMaterialIndex){
      if(m_scene->getMaterial(m_materialIndex)->hasDiffuseTexture() && tex){
	/* Inutile de réactiver l'unité de texture si le matériau précédent en avait une */
	if(!m_scene->getMaterial(lastMaterialIndex)->hasDiffuseTexture()){
	  glActiveTextureARB(GL_TEXTURE0_ARB);
	  glEnable(GL_TEXTURE_2D);
	  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	m_scene->getMaterial(m_materialIndex)->getDiffuseTexture()->bind();
      }else
	if(m_scene->getMaterial(lastMaterialIndex)->hasDiffuseTexture() && tex){
	  /* Pas de texture pour le matériau courant, on désactive l'unité de texture car le matériau précédent en avait une */
	  glDisable(GL_TEXTURE_2D);
	}
      m_scene->getMaterial(m_materialIndex)->apply();
    }else
      cerr << ">>> Mat identique" << endl;
  
  glBegin (GL_TRIANGLES);
  switch(m_attributes){
  case 0:
    cerr << "Cas non pris en compte pour l'instant" << endl;
    break;
  case 1:
  case 2:
    for (uint i=0; i < m_nbVertexIndex; i++){
      normalIndex = m_normalsIndexArray[i]*3;
      vertexIndex = m_vertexIndexArray[i]*3;
      glNormal3d (m_parent->getNormal(normalIndex), m_parent->getNormal(normalIndex+1),
		  m_parent->getNormal(normalIndex+2));
      glVertex3d (m_parent->getVertex(vertexIndex), m_parent->getVertex(vertexIndex+1), 
		  m_parent->getVertex(vertexIndex+2) );
    }
    break;
  case 3:
    for (uint i=0; i < m_nbVertexIndex; i++){
      texCoordIndex = m_texCoordsIndexArray[i]*2;
      normalIndex = m_normalsIndexArray[i]*3;
      vertexIndex = m_vertexIndexArray[i]*3;
      glTexCoord2d ( m_parent->getTexCoord(texCoordIndex), m_parent->getTexCoord(texCoordIndex+1) );
      glNormal3d (m_parent->getNormal(normalIndex), m_parent->getNormal(normalIndex+1),
		  m_parent->getNormal(normalIndex+2));
      glVertex3d (m_parent->getVertex(vertexIndex), m_parent->getVertex(vertexIndex+1), 
		  m_parent->getVertex(vertexIndex+2));
    }
    break;
  }
  glEnd ();
  lastMaterialIndex = m_materialIndex;
}

const bool Mesh::isTransparent ()
{ 
  if(m_scene->getMaterial(m_materialIndex)->hasDiffuseTexture())
    if( m_scene->getMaterial(m_materialIndex)->isTransparent())
      return true;
  return false;
}

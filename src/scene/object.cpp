#include "object.hpp"

#include "scene.hpp"

Object::Object(Scene *scene)
{
  m_nbVertex = 0;
  m_nbNormals = 0;
  m_nbTexCoords = 0;
  m_scene = scene;
  m_attributes = 0;
  glGenBuffers(3, m_bufferID);
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
  glDeleteBuffers(m_attributes, m_bufferID);
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

void Object::buildVBOs()
{
  /* Détermination du type de données décrites à partir des maillages */
  for (list <Mesh* >::iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    if( (*meshesListIterator)->getAttributes() > m_attributes)
      m_attributes = (*meshesListIterator)->getAttributes();
  
  glGenBuffers(m_attributes, m_bufferID);
  
  switch(m_attributes){
  case 3:
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferID[2]);
    glBufferData(GL_ARRAY_BUFFER, m_nbTexCoords*sizeof(GLfloat), m_texCoordsArray, GL_STATIC_DRAW);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
  case 2: 
  case 1:
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferID[1]);
    glBufferData(GL_ARRAY_BUFFER, m_nbNormals*sizeof(GLfloat), m_normalsArray, GL_STATIC_DRAW);
    glNormalPointer(GL_FLOAT, 0, 0);
  case 0:
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferID[0]);
    glBufferData(GL_ARRAY_BUFFER, m_nbVertex*sizeof(GLfloat), m_vertexArray, GL_STATIC_DRAW);
    glVertexPointer(3, GL_FLOAT, 0, 0);
  }
  for (list <Mesh* >::iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    (*meshesListIterator)->buildVBOs();	
  glBindBuffer(GL_ARRAY_BUFFER, 0 );
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
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/********************************************* Mesh Definition *********************************************/
Mesh::Mesh (Scene* const scene, uint materialIndex, Object* parent)
{
  m_scene = scene;
  m_attributes = 0;
  m_materialIndex = materialIndex;
  m_nbIndex = 0;
  m_parent = parent;
}

Mesh::~Mesh ()
{
  switch(m_attributes){
  case 3: delete [] m_texCoordsIndexArray;
  case 2: delete [] m_normalsIndexArray;
  case 1:
  case 0: delete [] m_vertexIndexArray;
  }
  glDeleteBuffers(m_attributes, m_bufferID);
}

void Mesh::buildVBOs()
{
  glGenBuffers(m_attributes, m_bufferID);
  
  switch(m_attributes){
  case 3:
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nbIndex*sizeof(GLuint), m_texCoordsIndexArray, GL_STATIC_DRAW);
  case 2: 
  case 1:
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nbIndex*sizeof(GLuint), m_normalsIndexArray, GL_STATIC_DRAW);
  case 0:
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nbIndex*sizeof(GLuint), m_vertexIndexArray, GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void Mesh::draw (char drawCode, bool tex, uint &lastMaterialIndex)
{  
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
  
  switch(m_attributes){
  case 3:
    m_parent->bindVBO(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID[2]);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  case 2: 
  case 1:
    m_parent->bindVBO(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID[1]);
    //glEnableClientState(GL_NORMAL_ARRAY);
  case 0:
    m_parent->bindVBO(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawElements(GL_TRIANGLES, m_nbIndex, GL_UNSIGNED_INT, 0);
  }
  switch(m_attributes){
  case 0: glDisableClientState(GL_VERTEX_ARRAY);
    //case 1: 
    //  case 2: glDisableClientState(GL_NORMAL_ARRAY);
    //  case 3: glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }
  lastMaterialIndex = m_materialIndex;
}

void Mesh::drawImmediate (char drawCode, bool tex, uint &lastMaterialIndex)
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
    for (uint i=0; i < m_nbIndex; i++){
      normalIndex = m_normalsIndexArray[i]*3;
      vertexIndex = m_vertexIndexArray[i]*3;
      glNormal3d (m_parent->getNormal(normalIndex), m_parent->getNormal(normalIndex+1),
		  m_parent->getNormal(normalIndex+2));
      glVertex3d (m_parent->getVertex(vertexIndex), m_parent->getVertex(vertexIndex+1), 
		  m_parent->getVertex(vertexIndex+2) );
    }
    break;
  case 3:
    for (uint i=0; i < m_nbIndex; i++){
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

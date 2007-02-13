#include "object.hpp"

#include "scene.hpp"

Object::Object(Scene *scene)
{
  m_scene = scene;
  m_attributes = 0;
  glGenBuffers(1, &m_bufferID);
}

Object::~Object ()
{
  m_vertexArray.clear();
  
  for (list <Mesh* >::iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    delete (*meshesListIterator);
  m_meshesList.clear ();
  glDeleteBuffers(1, &m_bufferID);
  delete [] m_hashTable;
}

void Object::getBoundingBox (Point & max, Point & min)
{
  Point ptMax(DBL_MIN, DBL_MIN, DBL_MIN), ptMin(DBL_MAX, DBL_MAX, DBL_MAX);
  /* Création de la bounding box */
  
  for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
       vertexIterator != m_vertexArray.end (); vertexIterator++)
    {
      /* Calcul du max */
      if ( vertexIterator->x > ptMax.x)
	ptMax.x = vertexIterator->x;
      if ( vertexIterator->y > ptMax.y)
	ptMax.y = vertexIterator->y;
      if ( vertexIterator->z > ptMax.z)
	ptMax.z = vertexIterator->z;
      /* Calcul du min */
      if ( vertexIterator->x < ptMin.x)
	ptMin.x = vertexIterator->x;
      if ( vertexIterator->y < ptMin.y)
	ptMin.y = vertexIterator->y;
      if ( vertexIterator->z < ptMin.z)
	ptMin.z = vertexIterator->z;
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
  
  glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
  glBufferData(GL_ARRAY_BUFFER, m_vertexArray.size()*sizeof(Vertex), &m_vertexArray[0], GL_STATIC_DRAW);
    
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
}

/********************************************* Mesh Definition *********************************************/
Mesh::Mesh (Scene* const scene, uint materialIndex, Object* parent)
{
  m_scene = scene;
  m_attributes = 0;
  m_materialIndex = materialIndex;
  m_parent = parent;
  glGenBuffers(1, &m_bufferID);
}

Mesh::~Mesh ()
{
  m_indexArray.clear();  
  glDeleteBuffers(1, &m_bufferID);
}

void Mesh::buildVBOs(){
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexArray.size()*sizeof(GLuint), &m_indexArray[0], GL_STATIC_DRAW);
  
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
    }
  
  m_parent->bindVBO();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
  
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY); 

  glDrawElements(GL_TRIANGLES, m_indexArray.size(), GL_UNSIGNED_INT, 0);
  
  glDisableClientState(GL_VERTEX_ARRAY); 
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
  glBindBuffer(GL_ARRAY_BUFFER, 0 );
  lastMaterialIndex = m_materialIndex;
}

const bool Mesh::isTransparent ()
{ 
  if(m_scene->getMaterial(m_materialIndex)->hasDiffuseTexture())
    if( m_scene->getMaterial(m_materialIndex)->isTransparent())
      return true;
  return false;
}

void Mesh::setUVsAndNormals(vector < Vector > &normalsVector,   vector < GLuint > &normalsIndexVector, 
			    vector < Point >  &texCoordsVector, vector < GLuint > &texCoordsIndexVector)
{
  Vertex v;
  Vector normal, texCoord;
  
  if(!m_attributes)
    {
      for (uint i = 0; i < m_indexArray.size(); i++)
	m_parent->setVertex( m_indexArray[i], 0, 0, 0, 0, 0);
    }else{    
    /* On initialise la table de hachage */
    m_parent->initHashTable();
    if(m_attributes < 3)
      for (uint i = 0; i < m_indexArray.size(); i++)
	{
	  normal = normalsVector[normalsIndexVector[i]];
	  if( m_parent->findRefInHashTable( m_indexArray[i], v) )
	    /* Le point courant a déjà été référencé auparavant dans le tableau d'indices */
	    {
	      if ( normal.x == v.nx && normal.y == v.ny && normal.z == v.nz )
		/* La normale du point courant est identique à la précédente référence, il n'y a donc rien à faire */
		continue;
	      else {
		v = m_parent->getVertex(m_indexArray[i]);
		m_parent->addVertex( v );
		/* Le nouveau point est placé en dernier, on récupère son index et on le stocke */
		m_indexArray[i] = m_parent->getVertexArraySize()-1;
	      }
	    }
	  /* On affecte les coordonnées de texture et de normale au point courant */
	  m_parent->setVertex( m_indexArray[i], 0, 0, normal.x, normal.y, normal.z);
	}
    else      
      for (uint i = 0; i < m_indexArray.size(); i++)
	{
	  normal = normalsVector[normalsIndexVector[i]];
	  texCoord = texCoordsVector[texCoordsIndexVector[i]];
	  if( m_parent->findRefInHashTable( m_indexArray[i], v) )
	    /* Le point courant a déjà été référencé auparavant dans le tableau d'indices */
	    {
	      if ( texCoord.x == v.u && texCoord.y == v.v && normal.x == v.nx && normal.y == v.ny && normal.z == v.nz )
		/* La normale et les coordonnées de texture du point courant sont identiques à la précédente référence, il n'y a donc rien à faire */
		continue;
	      else{
		v = m_parent->getVertex(m_indexArray[i]);
		m_parent->addVertex( v );
		/* Le nouveau point est placé en dernier, on récupère son index et on le stocke */
		m_indexArray[i] = m_parent->getVertexArraySize()-1;
	      }
	    }
	  /* On affecte les coordonnées de texture et de normale au point courant */
	  m_parent->setVertex( m_indexArray[i], texCoord.x, texCoord.y, normal.x, normal.y, normal.z);
	}
  }
}

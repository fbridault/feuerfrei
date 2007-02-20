#include "object.hpp"

#include "scene.hpp"
#include "material.hpp"

extern uint g_objectCount;

Object::Object(const Scene* const scene)
{
  m_scene = scene;
  m_attributes = 0;
  glGenBuffers(1, &m_bufferID);
  m_hashTable = NULL;
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

void Object::buildBoundingSpheres ()
{
  /* On réalloue la table de hachage car le tableau peut contenir des nouveaux points depuis l'appel à setUVsAndNormals() */
  allocHashTable();
  for (list <Mesh* >::iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    (*meshesListIterator)->buildBoundingSphere();
}

void Object::drawBoundingSpheres ()
{
  m_scene->getMaterial(0)->apply();
  for (list <Mesh* >::iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    (*meshesListIterator)->drawBoundingSphere();
}

void Object::computeVisibility(const Camera &view)
{
  for (list <Mesh* >::iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    (*meshesListIterator)->computeVisibility(view);
}

uint Object::getPolygonsCount () const
{
  uint count=0;
  for (list <Mesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    count += (*meshesListIterator)->getPolygonsCount();
  return count;
}

void Object::buildVBO()
{
  /* Détermination du type de données décrites à partir des maillages */
  for (list <Mesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    if( (*meshesListIterator)->getAttributes() > m_attributes)
      m_attributes = (*meshesListIterator)->getAttributes();
  
  glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
  glBufferData(GL_ARRAY_BUFFER, m_vertexArray.size()*sizeof(Vertex), &m_vertexArray[0], GL_STATIC_DRAW);
    
  for (list <Mesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
       meshesListIterator != m_meshesList.end ();
       meshesListIterator++)
    (*meshesListIterator)->buildVBO();	
  glBindBuffer(GL_ARRAY_BUFFER, 0 );
}

void Object::draw (char drawCode, bool tex, bool boundingSpheres) const
{
  /* On initialise le dernier matériau au premier de la liste, le matériau par défaut */
  uint lastMaterialIndex=0;
  
  if(boundingSpheres){
    m_scene->getMaterial(0)->apply();
    for (list <Mesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
	 meshesListIterator != m_meshesList.end ();
	 meshesListIterator++)
      (*meshesListIterator)->drawBoundingSphere();
  }else{
    if(drawCode == AMBIENT)
      /* Dessiner avec le matériau par défaut (pour tester les zones d'ombres par exemple) */
      m_scene->getMaterial(0)->apply();
    
    /* Parcours de la liste des meshes */
    for (list <Mesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
	 meshesListIterator != m_meshesList.end ();
	 meshesListIterator++)
      (*meshesListIterator)->draw(drawCode, tex, lastMaterialIndex);
    
    /* On désactive l'unité de texture le cas échéant */
    if(m_scene->getMaterial(lastMaterialIndex)->hasDiffuseTexture() && tex){
      glDisable(GL_TEXTURE_2D);
    }
  }
}

/********************************************* Mesh Definition *********************************************/
Mesh::Mesh (const Scene* const scene, uint materialIndex, Object* parent)
{
  m_scene = scene;
  m_attributes = 0;
  m_materialIndex = materialIndex;
  m_parent = parent;
  m_visibility = true;
  m_radius=0;
  glGenBuffers(1, &m_bufferID);
}

Mesh::~Mesh ()
{
  m_indexArray.clear();  
  glDeleteBuffers(1, &m_bufferID);
}

void Mesh::buildVBO() const
{  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexArray.size()*sizeof(GLuint), &m_indexArray[0], GL_STATIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void Mesh::buildBoundingSphere ()
{
  double dist;
  Vertex v;
  uint n=0;
  
  /* On initialise la table de hachage */
  m_parent->initHashTable();
  
  for (uint i = 0; i < m_indexArray.size(); i++)
    {
      /* Il ne faut prendre un même point qu'une seule fois en compte. */
      if( !m_parent->findRefInHashTable( m_indexArray[i] ) )
	{
	  v = m_parent->getVertex(m_indexArray[i]);
	  m_parent->addRefInHashTable( m_indexArray[i], i );
	  m_centre = (m_centre*n + Point(v.x, v.y, v.z)) / (double)(n+1);	  
	  n++;
	}
    }
  
  for (vector < GLuint >::iterator indexIterator = m_indexArray.begin ();
       indexIterator != m_indexArray.end ();  indexIterator++)
    {      
      Point p;
      v=m_parent->getVertex(*indexIterator);
      p=Point(v.x, v.y, v.z);
      dist=p.squaredDistanceFrom(m_centre);
      if( dist > m_radius)
	m_radius = dist;
    }
  m_radius = sqrt(m_radius);
  //cerr << "sphere de centre " << m_centre << " et de rayon " << m_radius << endl;
}

void Mesh::drawBoundingSphere()
{
  glPushMatrix();
  glTranslatef(m_centre.x, m_centre.y, m_centre.z);
  glColor3d(1.0,0.0,0.0);
  GraphicsFn::SolidSphere(m_radius, 20, 20);
  glPopMatrix();
}

void Mesh::computeVisibility(const Camera &view)
{
  uint i;
  const double *plan;

  // Centre dans le frustrum ?
//   for( p = 0; p < 6; p++ )
//     if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0 ){
//       m_visibility = false;
//       return;
//     }

  // Sphère dans le frustrum ?
  for( i = 0; i < 6; i++ ){
    plan=view.getFrustum(i);
    if( plan[0] * m_centre.x + plan[1] * m_centre.y + plan[2] * m_centre.z + plan[3] <= -m_radius ){
      m_visibility = false;
      return;
    }
  }
//   g_objectCount++;
  m_visibility = true;
  return;
}

void Mesh::draw (char drawCode, bool tex, uint &lastMaterialIndex) const
{  
  if(!m_visibility)
    return;
  
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
  
  if(drawCode != AMBIENT)
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

const bool Mesh::isTransparent () const
{ 
  if(m_scene->getMaterial(m_materialIndex)->hasDiffuseTexture())
    if( m_scene->getMaterial(m_materialIndex)->isTransparent())
      return true;
  return false;
}

void Mesh::setUVsAndNormals(const vector < Vector > &normalsVector, const vector < GLuint > &normalsIndexVector, 
			    const vector < Point >  &texCoordsVector, const vector < GLuint > &texCoordsIndexVector)
{
  Vertex v;
  Vector normal, texCoord;
  uint dup=0,nondup=0;

  //   cerr << " Over " << m_indexArray.size() << " vertices, ";
  if(!m_attributes)
    for (uint i = 0; i < m_indexArray.size(); i++)
      m_parent->setVertex( m_indexArray[i], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  else
    if(m_attributes < 3)
      for (uint i = 0; i < m_indexArray.size(); i++)
	{
	  normal = normalsVector[normalsIndexVector[i]];
	  if( m_parent->findRefInHashTable( m_indexArray[i] ) )
	    /* Le point courant a déjà été référencé auparavant dans le tableau d'indices */
	    {
	      v = m_parent->getVertex(m_indexArray[i]);
	      if ( ((float)normal.x) == v.nx && ((float)normal.y) == v.ny && ((float)normal.z) == v.nz ){
		nondup++;
		/* La normale du point courant est identique à la précédente référence, il n'y a donc rien à faire */
		continue;
	      }else {
		m_parent->addVertex( v );
		/* Le nouveau point est placé en dernier, on récupère son index et on le stocke */
		m_indexArray[i] = m_parent->getVertexArraySize()-1;
		dup++;
	      }
	    }
	  else
	    m_parent->addRefInHashTable( m_indexArray[i], i);
	  /* On affecte les coordonnées de texture et de normale au point courant */
	  m_parent->setVertex( m_indexArray[i], 0.0f, 0.0f, normal.x, normal.y, normal.z);
	}
    else      
      for (uint i = 0; i < m_indexArray.size(); i++)
	{
	  normal = normalsVector[normalsIndexVector[i]];
	  texCoord = texCoordsVector[texCoordsIndexVector[i]];
	  
	  if( m_parent->findRefInHashTable( m_indexArray[i] ) )
	    /* Le point courant a déjà été référencé auparavant dans le tableau d'indices */
	    {	
	      v = m_parent->getVertex(m_indexArray[i]);
	      if ( ((float)texCoord.x) == v.u && ((float)texCoord.y) == v.v && 
		   ((float)normal.x) == v.nx && ((float)normal.y) == v.ny && ((float)normal.z) == v.nz ){
		nondup++;
		/* La normale et les coordonnées de texture du point courant sont identiques à la précédente référence, il n'y a donc rien à faire */
		continue;
	      }else{
		m_parent->addVertex( v );
		/* Le nouveau point est placé en dernier, on récupère son index et on le stocke */
		m_indexArray[i] = m_parent->getVertexArraySize()-1;
		dup++;	  
	      }
	    }
	  else
	    m_parent->addRefInHashTable( m_indexArray[i], i);
	  /* On affecte les coordonnées de texture et de normale au point courant */
	  m_parent->setVertex( m_indexArray[i], texCoord.x, texCoord.y, normal.x, normal.y, normal.z);
	}
//   cerr << dup << " have been duplicated, " << nondup << " untouched" << endl;
}

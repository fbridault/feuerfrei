#include "object.hpp"

#include "scene.hpp"

Object::~Object ()
{
  for (vector < Point * >::iterator vertexArrayIterator = m_vertexArray.begin (); 
       vertexArrayIterator != m_vertexArray.end ();
       vertexArrayIterator++)
    delete (*vertexArrayIterator);
  m_vertexArray.clear ();
  
  for (vector < Vector * >::iterator normalsArrayIterator = m_normalsArray.begin (); 
       normalsArrayIterator != m_normalsArray.end ();
       normalsArrayIterator++)
    delete (*normalsArrayIterator);
  m_normalsArray.clear ();

  for (vector < Point * >::iterator texCoordsArrayIterator = m_texCoordsArray.begin (); 
       texCoordsArrayIterator != m_texCoordsArray.end ();
       texCoordsArrayIterator++)
    delete (*texCoordsArrayIterator);
  m_texCoordsArray.clear ();

  for (vector < PointIndices * >::iterator vertexIndexArrayIterator = m_vertexIndexArray.begin ();
       vertexIndexArrayIterator != m_vertexIndexArray.end (); 
       vertexIndexArrayIterator++)
    delete (*vertexIndexArrayIterator);
  m_vertexIndexArray.clear ();
}

void
Object::getBoundingBox (Point & max, Point & min)
{
  Point ptMax(DBL_MIN, DBL_MIN, DBL_MIN), ptMin(DBL_MAX, DBL_MAX, DBL_MAX);
  /* Création de la bounding box */
  for (vector < Point * >::iterator vertexArrayIterator = m_vertexArray.begin (); 
       vertexArrayIterator != m_vertexArray.end ();
       vertexArrayIterator++){
    /* Calcul du max */
    if ((*vertexArrayIterator)->x > ptMax.x)
      ptMax.x = (*vertexArrayIterator)->x;
    if ((*vertexArrayIterator)->y > ptMax.y)
      ptMax.y = (*vertexArrayIterator)->y;
    if ((*vertexArrayIterator)->z > ptMax.z)
      ptMax.z = (*vertexArrayIterator)->z;
    /* Calcul du min */
    if ((*vertexArrayIterator)->x < ptMin.x)
      ptMin.x = (*vertexArrayIterator)->x;
    if ((*vertexArrayIterator)->y < ptMin.y)
      ptMin.y = (*vertexArrayIterator)->y;
    if ((*vertexArrayIterator)->z < ptMin.z)
      ptMin.z = (*vertexArrayIterator)->z;
  }
  
  max = ptMax; min = ptMin;
}

void
Object::checkAndApplyMaterial(int currentMaterialIndex, bool tex)
{
  if(currentMaterialIndex != m_lastMaterialIndex){
    if(m_scene->getMaterial(currentMaterialIndex)->hasDiffuseTexture() && tex){
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_scene->getMaterial(currentMaterialIndex)->getDiffuseTexture());
      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }else
      glDisable(GL_TEXTURE_2D);
    m_scene->getMaterial(currentMaterialIndex)->apply();
    m_lastMaterialIndex = currentMaterialIndex;
  }
}

void
Object::draw (char drawCode, bool tex)
{  
  PointIndices *index;
  int vertexCount=0;
  m_lastMaterialIndex=-1;
  
  switch(m_attributes){
  case 0:
    cerr << "Cas non pris en compte pour l'instant" << endl;
    break;
  case 1:
    for (vector < PointIndices * >::iterator vertexIndexArrayIterator = m_vertexIndexArray.begin ();
	 vertexIndexArrayIterator != m_vertexIndexArray.end ();
	 vertexIndexArrayIterator++){
      index = *vertexIndexArrayIterator;
      
      if(drawCode == TEXTURED){
	/* Ne dessiner que si il y a une texture */
	if(!m_scene->getMaterial(index->vm)->hasDiffuseTexture())
	  continue;
      }else
	if(drawCode == FLAT)
	  /* Ne dessiner que si il n'y a pas de texture */
	  if(m_scene->getMaterial(index->vm)->hasDiffuseTexture())
	    continue;
	  
      if(!vertexCount){
	if(drawCode == AMBIENT)
	  checkAndApplyMaterial(0,tex);
	else
	  checkAndApplyMaterial(index->vm,tex);
	  
	glBegin (GL_POLYGON);
      }
      glNormal3f (m_normalsArray[index->vn]->x, m_normalsArray[index->vn]->y, 
		  m_normalsArray[index->vn]->z);
      glVertex3d (m_vertexArray[index->v]->x, m_vertexArray[index->v]->y, 
		  m_vertexArray[index->v]->z);
      vertexCount++;
      if(vertexCount==3){
	glEnd ();
	vertexCount=0;
      }
    }
    
    break;
  case 2:
    for (vector < PointIndices * >::iterator vertexIndexArrayIterator = m_vertexIndexArray.begin ();
	 vertexIndexArrayIterator != m_vertexIndexArray.end ();
	 vertexIndexArrayIterator++){
      index = *vertexIndexArrayIterator;

     if(drawCode == TEXTURED){
	/* Ne dessiner que si il y a une texture */
	if(!m_scene->getMaterial(index->vm)->hasDiffuseTexture())
	  continue;
      }else
	if(drawCode == FLAT)
	  /* Ne dessiner que si il n'y a pas de texture */
	  if(m_scene->getMaterial(index->vm)->hasDiffuseTexture())
	    continue;

      if(!vertexCount){
	if(drawCode == AMBIENT)
	  checkAndApplyMaterial(0,tex);
	else
	  checkAndApplyMaterial(index->vm,tex);
	glBegin (GL_POLYGON);
      }
      
      glNormal3f (m_normalsArray[index->vn]->x, m_normalsArray[index->vn]->y, 
		  m_normalsArray[index->vn]->z);
      glVertex3d (m_vertexArray[index->v]->x, m_vertexArray[index->v]->y, 
		  m_vertexArray[index->v]->z);
      vertexCount++;
      if(vertexCount==3){
	glEnd ();
	vertexCount=0;
      }
    }
    break;
  case 3:
    for (vector < PointIndices * >::iterator vertexIndexArrayIterator = m_vertexIndexArray.begin ();
	 vertexIndexArrayIterator != m_vertexIndexArray.end ();
	 vertexIndexArrayIterator++){
      index = *vertexIndexArrayIterator;

     if(drawCode == TEXTURED){
	/* Ne dessiner que si il y a une texture */
	if(!m_scene->getMaterial(index->vm)->hasDiffuseTexture())
	  continue;
      }else
	if(drawCode == FLAT)
	  /* Ne dessiner que si il n'y a pas de texture */
	  if(m_scene->getMaterial(index->vm)->hasDiffuseTexture())
	    continue;

      if(!vertexCount){
	if(drawCode == AMBIENT)
	  checkAndApplyMaterial(0,tex);
	else
	  checkAndApplyMaterial(index->vm,tex);
	glBegin (GL_POLYGON);
      }
      
      glTexCoord2f ( m_texCoordsArray[index->vt]->x, m_texCoordsArray[index->vt]->y );
      glNormal3f (m_normalsArray[index->vn]->x, m_normalsArray[index->vn]->y, 
		  m_normalsArray[index->vn]->z);
      glVertex3d (m_vertexArray[index->v]->x, m_vertexArray[index->v]->y, 
		  m_vertexArray[index->v]->z);
      vertexCount++;
      if(vertexCount==3){
	glEnd ();
	vertexCount=0;
      }
    }
    break;
  }
}

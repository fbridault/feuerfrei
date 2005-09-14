/***************************************************************************
 *            object.cpp
 *
 *  Mon Apr  4 10:38:28 2005
 *  Copyright  2005  Flavien Bridault
 *  bridault@lil.univ-littoral.fr
 ****************************************************************************/
#include "object.hpp"

CObject::~CObject ()
{
  for (vector < CPoint * >::iterator vertexArrayIterator = vertexArray.begin (); 
       vertexArrayIterator != vertexArray.end ();
       vertexArrayIterator++)
    delete (*vertexArrayIterator);
  vertexArray.clear ();
  
  for (vector < CVector * >::iterator normalsArrayIterator = normalsArray.begin (); 
       normalsArrayIterator != normalsArray.end ();
       normalsArrayIterator++)
    delete (*normalsArrayIterator);
  normalsArray.clear ();

  for (vector < CPoint * >::iterator texCoordsArrayIterator = texCoordsArray.begin (); 
       texCoordsArrayIterator != texCoordsArray.end ();
       texCoordsArrayIterator++)
    delete (*texCoordsArrayIterator);
  texCoordsArray.clear ();

  for (vector < CIndex * >::iterator vertexIndexArrayIterator = vertexIndexArray.begin ();
       vertexIndexArrayIterator != vertexIndexArray.end (); 
       vertexIndexArrayIterator++)
    delete (*vertexIndexArrayIterator);
  vertexIndexArray.clear ();
  
  for (vector < CMaterial * >::iterator materialArrayIterator = materialArray.begin (); 
       materialArrayIterator != materialArray.end ();
       materialArrayIterator++)
    delete (*materialArrayIterator);
  materialArray.clear ();
}

void
CObject::getBoundingBox (CPoint & max, CPoint & min)
{
  CPoint ptMax(DBL_MIN, DBL_MIN, DBL_MIN), ptMin(DBL_MAX, DBL_MAX, DBL_MAX);
  /* Création de la bounding box */
  for (vector < CPoint * >::iterator vertexArrayIterator = vertexArray.begin (); 
       vertexArrayIterator != vertexArray.end ();
       vertexArrayIterator++){
    /* Calcul du max */
    if ((*vertexArrayIterator)->getX () > ptMax.getX ())
      ptMax.setX ((*vertexArrayIterator)->getX ());
    if ((*vertexArrayIterator)->getY () > ptMax.getY ())
      ptMax.setY ((*vertexArrayIterator)->getY ());
    if ((*vertexArrayIterator)->getZ () > ptMax.getZ ())
      ptMax.setZ ((*vertexArrayIterator)->getZ ());
    /* Calcul du min */
    if ((*vertexArrayIterator)->getX () < ptMin.getX ())
      ptMin.setX ((*vertexArrayIterator)->getX ());
    if ((*vertexArrayIterator)->getY () < ptMin.getY ())
      ptMin.setY ((*vertexArrayIterator)->getY ());
    if ((*vertexArrayIterator)->getZ () < ptMin.getZ ())
      ptMin.setZ ((*vertexArrayIterator)->getZ ());
  }
  
  max = ptMax; min = ptMin;
}

void
CObject::checkAndApplyMaterial(int currentMaterialIndex, bool tex)
{
  if(currentMaterialIndex != lastMaterialIndex){
    if(materialArray[currentMaterialIndex]->hasDiffuseTexture() && tex){
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, materialArray[currentMaterialIndex]->getDiffuseTexture());
      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }else
      glDisable(GL_TEXTURE_2D);
    materialArray[currentMaterialIndex]->apply();
    lastMaterialIndex = currentMaterialIndex;
  }

}

void
CObject::draw (char drawCode, bool tex)
{  
  CIndex *index;
  int vertexCount=0;
  lastMaterialIndex=-1;
  
  switch(attributes){
  case 0:
    cerr << "Cas non pris en compte pour l'instant" << endl;
    break;
  case 1:
    for (vector < CIndex * >::iterator vertexIndexArrayIterator = vertexIndexArray.begin ();
	 vertexIndexArrayIterator != vertexIndexArray.end ();
	 vertexIndexArrayIterator++){
      index = *vertexIndexArrayIterator;
      
      if(drawCode > 0){
	/* Ne dessiner que si il y a une texture */
	if(!materialArray[index->vm]->hasDiffuseTexture())
	  continue;
      }else
	if(drawCode < 0)
	  /* Ne dessiner que si il n'y a pas de texture */
	  if(materialArray[index->vm]->hasDiffuseTexture())
	    continue;
      
      if(!vertexCount){
	checkAndApplyMaterial(index->vm,tex);
	glBegin (GL_POLYGON);
      }
      
      glNormal3f (normalsArray[index->vn]->getX (), normalsArray[index->vn]->getY (), 
		  normalsArray[index->vn]->getZ ());
      glVertex3f (vertexArray[index->v]->getX (), vertexArray[index->v]->getY (), 
		  vertexArray[index->v]->getZ ());
      vertexCount++;
      if(vertexCount==3){
	glEnd ();
	vertexCount=0;
      }
    }
    
    break;
  case 2:
    for (vector < CIndex * >::iterator vertexIndexArrayIterator = vertexIndexArray.begin ();
	 vertexIndexArrayIterator != vertexIndexArray.end ();
	 vertexIndexArrayIterator++){
      index = *vertexIndexArrayIterator;

     if(drawCode > 0){
	/* Ne dessiner que si il y a une texture */
	if(!materialArray[index->vm]->hasDiffuseTexture())
	  continue;
      }else
	if(drawCode < 0)
	  /* Ne dessiner que si il n'y a pas de texture */
	  if(materialArray[index->vm]->hasDiffuseTexture())
	    continue;

      if(!vertexCount){
	checkAndApplyMaterial(index->vm,tex);
	glBegin (GL_POLYGON);
      }
      
      glNormal3f (normalsArray[index->vn]->getX (), normalsArray[index->vn]->getY (), 
		  normalsArray[index->vn]->getZ ());
      glVertex3f (vertexArray[index->v]->getX (), vertexArray[index->v]->getY (), 
		  vertexArray[index->v]->getZ ());
      vertexCount++;
      if(vertexCount==3){
	glEnd ();
	vertexCount=0;
      }
    }
    break;
  case 3:
    for (vector < CIndex * >::iterator vertexIndexArrayIterator = vertexIndexArray.begin ();
	 vertexIndexArrayIterator != vertexIndexArray.end ();
	 vertexIndexArrayIterator++){
      index = *vertexIndexArrayIterator;

     if(drawCode > 0){
	/* Ne dessiner que si il y a une texture */
	if(!materialArray[index->vm]->hasDiffuseTexture())
	  continue;
      }else
	if(drawCode < 0)
	  /* Ne dessiner que si il n'y a pas de texture */
	  if(materialArray[index->vm]->hasDiffuseTexture())
	    continue;

      if(!vertexCount){
	checkAndApplyMaterial(index->vm,tex);
	glBegin (GL_POLYGON);
      }
      
      glTexCoord2f ( texCoordsArray[index->vt]->getX(), texCoordsArray[index->vt]->getY() );
      glNormal3f (normalsArray[index->vn]->getX (), normalsArray[index->vn]->getY (), 
		  normalsArray[index->vn]->getZ ());
      glVertex3f (vertexArray[index->v]->getX (), vertexArray[index->v]->getY (), 
		  vertexArray[index->v]->getZ ());
      vertexCount++;
      if(vertexCount==3){
	glEnd ();
	vertexCount=0;
      }
    }
    break;
  }
}

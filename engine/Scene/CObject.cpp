#include "CObject.hpp"

#include "CScene.hpp"
#include "CMaterial.hpp"
#include "../Utility/CRefTable.hpp"

#include <values.h>

/**************************************************************************************************/
/**														 DEFINITION DE L'INTERFACE ISCENEITEM												*/
/**************************************************************************************************/

ISceneItem::ISceneItem(CPoint const& a_rPosition) :
	m_oPosition(a_rPosition), m_oScale(1.f,1.f,1.f), m_bSelected(false)
{
	m_uiGlName = CScene::glNameCounter++;
}

/**************************************************************************************************/
/**														 DEFINITION DE LA CLASSE OBJECT 											 	    			*/
/**************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
CObject::CObject(CScene& a_rScene) :
	ISceneItem(CPoint(0,0,0)),
	m_bBuilt(false),
	m_rScene(a_rScene),
	m_attributes(0)
{
	glGenBuffers(1, &m_bufferID);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
CObject::~CObject ()
{
	m_vertexArray.clear();

	for (vector <CMesh* >::iterator meshesListIterator = m_meshesList.begin ();
	     meshesListIterator != m_meshesList.end ();
	     meshesListIterator++)
		delete (*meshesListIterator);
	m_meshesList.clear ();
	glDeleteBuffers(1, &m_bufferID);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CObject::buildBoundingBox ()
{
	CPoint ptMax(-FLT_MAX, -FLT_MAX, -FLT_MAX), ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
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
	m_max = ptMax;
	m_min = ptMin;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CObject::HardScale (float scaleFactor)
{
	for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
	     vertexIterator != m_vertexArray.end (); vertexIterator++)
	{
		vertexIterator->x = vertexIterator->x * scaleFactor;
		vertexIterator->y = vertexIterator->y * scaleFactor;
		vertexIterator->z = vertexIterator->z * scaleFactor;
	}
	m_max = m_max * scaleFactor;
	m_min = m_min * scaleFactor;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CObject::HardTranslate (const CPoint& offset)
{
	for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
	     vertexIterator != m_vertexArray.end (); vertexIterator++)
	{
		vertexIterator->x = vertexIterator->x + offset.x;
		vertexIterator->y = vertexIterator->y + offset.y;
		vertexIterator->z = vertexIterator->z + offset.z;
	}
	m_max = m_max + offset;
	m_min = m_min + offset;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CObject::buildBoundingSpheres ()
{
	CRefTable refTable(getVertexArraySize());

	for (vector <CMesh* >::iterator meshesListIterator = m_meshesList.begin ();
	     meshesListIterator != m_meshesList.end ();
	     meshesListIterator++)
		(*meshesListIterator)->buildBoundingSphere(refTable);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CObject::drawBoundingSpheres ()
{
	m_rScene.getMaterial(0)->apply();
	for (vector <CMesh* >::iterator meshesListIterator = m_meshesList.begin ();
	     meshesListIterator != m_meshesList.end ();
	     meshesListIterator++)
		(*meshesListIterator)->drawBoundingSphere();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CObject::computeVisibility(const CCamera &view)
{
	for (vector <CMesh* >::iterator meshesListIterator = m_meshesList.begin ();
	     meshesListIterator != m_meshesList.end ();
	     meshesListIterator++)
		(*meshesListIterator)->computeVisibility(view);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
uint CObject::getPolygonsCount () const
{
	uint count=0;
	for (vector <CMesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
	     meshesListIterator != m_meshesList.end ();
	     meshesListIterator++)
		count += (*meshesListIterator)->getPolygonsCount();
	return count;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CObject::buildVBO()
{
	assert(m_bBuilt == false);

	/* Détermination du type de données décrites à partir des maillages */
	for (vector <CMesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
	     meshesListIterator != m_meshesList.end ();
	     meshesListIterator++)
		if ( (*meshesListIterator)->getAttributes() > m_attributes)
			m_attributes = (*meshesListIterator)->getAttributes();

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexArray.size()*sizeof(Vertex), &m_vertexArray[0], GL_STATIC_DRAW);

	for (vector <CMesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
	     meshesListIterator != m_meshesList.end ();
	     meshesListIterator++)
		(*meshesListIterator)->buildVBO();
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
	m_bBuilt = true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CObject::draw (char drawCode, bool tex, bool boundingSpheres) const
{
	assert(m_bBuilt == true);

	/* On initialise le dernier matériau au premier de la liste, le matériau par défaut */
	uint lastMaterialIndex=0;

	if (boundingSpheres)
	{
		m_rScene.getMaterial(0)->apply();
		for (vector <CMesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
		     meshesListIterator != m_meshesList.end ();
		     meshesListIterator++)
			(*meshesListIterator)->drawBoundingSphere();
	}
	else
	{
		if (drawCode == AMBIENT)
			/* Dessiner avec le matériau par défaut (pour tester les zones d'ombres par exemple) */
			m_rScene.getMaterial(0)->apply();

		CPoint const& rPosition = GetPosition();
		CPoint const& rScale = GetScale();
		glPushMatrix();
		glTranslatef(rPosition.x,rPosition.y, rPosition.z);
		glScalef(rScale.x, rScale.y, rScale.z);
		/* Parcours de la liste des meshes */
		for (vector <CMesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
		     meshesListIterator != m_meshesList.end ();
		     meshesListIterator++)
			(*meshesListIterator)->draw(drawCode, tex, lastMaterialIndex);
		glPopMatrix();

		if (drawCode != AMBIENT)
			/* On désactive l'unité de texture le cas échéant */
			if (m_rScene.getMaterial(lastMaterialIndex)->hasDiffuseTexture() && tex)
			{
				glDisable(GL_TEXTURE_2D);
			}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CObject::DrawForSelection () const
{
	m_rScene.getMaterial(0)->apply();

	CPoint const& rPosition = GetPosition();
	CPoint const& rScale = GetScale();
	glPushMatrix();
	glTranslatef(rPosition.x,rPosition.y, rPosition.z);
	glScalef(rScale.x, rScale.y, rScale.z);
	glPushName(GetItemName());
	/* Parcours de la liste des meshes */
	for (vector <CMesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
	     meshesListIterator != m_meshesList.end ();
	     meshesListIterator++)
		(*meshesListIterator)->drawForSelection();
	glPopName();
	glPopMatrix();
}


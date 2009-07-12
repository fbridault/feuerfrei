#include "CObject.hpp"

#include "CScene.hpp"
#include "CMaterial.hpp"
#include "../Utility/CRefTable.hpp"

#include <values.h>

/*********************************************************************************************************************/
/**		Interface ISceneItem            	 														   			 */
/*********************************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
ISceneItem::ISceneItem(NRenderType const& a_nRenderType) :
	m_bSelected(false),
	m_bVisible(true),
	m_nRenderType(a_nRenderType)
{
	m_uiGlName = CScene::glNameCounter++;
}

/*********************************************************************************************************************/
/**		Class CObject          					  	 													   			 */
/*********************************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CObject::CObject(CScene& a_rScene, CTransform& a_rTransform) :
	ISceneItem(NRenderType::eNormal),
	m_bBuilt(false),
	m_rScene(a_rScene),
	m_rTransform(a_rTransform),
	m_attributes(0),
	m_oBBMin(FLT_MAX, FLT_MAX, FLT_MAX),
 	m_oBBMax(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{
	glGenBuffers(1, &m_bufferID);
	m_rTransform.AddChild(this);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CObject::~CObject ()
{
	m_vertexArray.clear();

	ForEachIter(itMesh, CMeshesVector, m_meshesList)
	{
		delete (*itMesh);
	}
	m_meshesList.clear ();
	glDeleteBuffers(1, &m_bufferID);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CObject::HardScale (float a_fScaleFactor)
{
	for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
	     vertexIterator != m_vertexArray.end (); vertexIterator++)
	{
		vertexIterator->x = vertexIterator->x * a_fScaleFactor;
		vertexIterator->y = vertexIterator->y * a_fScaleFactor;
		vertexIterator->z = vertexIterator->z * a_fScaleFactor;
	}
	m_oBBMax = m_oBBMax * a_fScaleFactor;
	m_oBBMin = m_oBBMin * a_fScaleFactor;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CObject::HardTranslate (const CPoint& a_rOffset)
{
	for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
	     vertexIterator != m_vertexArray.end (); vertexIterator++)
	{
		vertexIterator->x = vertexIterator->x + a_rOffset.x;
		vertexIterator->y = vertexIterator->y + a_rOffset.y;
		vertexIterator->z = vertexIterator->z + a_rOffset.z;
	}
	m_oBBMax = m_oBBMax + a_rOffset;
	m_oBBMin = m_oBBMin + a_rOffset;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CObject::BuildBoundingSpheres ()
{
	CRefTable refTable(GetVertexArraySize());

	ForEachIter(itMesh, CMeshesVector, m_meshesList)
	{
		(*itMesh)->buildBoundingSphere(refTable);
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CObject::drawBoundingSpheres ()
{
	m_rScene.GetMaterial(0).apply();
	ForEachIterC(itMesh, CMeshesVector, m_meshesList)
	{
		(*itMesh)->drawBoundingSphere();
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CObject::ComputeVisibility(const CCamera &a_rView)
{
	bool bVisible = false;

	// TODO: Put Meshes in scene graph/render list !!!
	ForEachIter(itMesh, CMeshesVector, m_meshesList)
	{
		bVisible |= (*itMesh)->computeVisibility(a_rView);
	}
	SetVisibility(bVisible);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
uint CObject::GetPolygonsCount () const
{
	uint count=0;
	ForEachIterC(itMesh, CMeshesVector, m_meshesList)
	{
		count += (*itMesh)->GetPolygonsCount();
	}
	return count;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CObject::BuildVBO()
{
	assert(m_bBuilt == false);

	/* Détermination du type de données décrites à partir des maillages */
	ForEachIterC(itMesh, CMeshesVector, m_meshesList)
	{
		if((*itMesh)->GetAttributes() > m_attributes)
			m_attributes = (*itMesh)->GetAttributes();
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexArray.size()*sizeof(Vertex), &m_vertexArray[0], GL_STATIC_DRAW);

	ForEachIterC(itMesh, CMeshesVector, m_meshesList)
	{
		(*itMesh)->BuildVBO();
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
	m_bBuilt = true;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CObject::Render () const
{
	assert(m_bBuilt == true);

	CDrawState &rDrawState = CDrawState::GetInstance();
	NShadingType const& nShadingType = rDrawState.GetShadingType();
	NDrawType const& nDrawType = rDrawState.GetDrawType();

	if (nDrawType == NDrawType::eBoundingSphere)
	{
		m_rScene.GetMaterial(0).apply();
		ForEachIterC(itMesh, CMeshesVector, m_meshesList)
		{
			(*itMesh)->drawBoundingSphere();
		}
	}
	else
	{
		if (nDrawType == NDrawType::eSelection)
		{
			glPushName(GetItemName());
			assert(nShadingType == NShadingType::eAmbient);
		}

		if (nShadingType == NShadingType::eAmbient)
		{
			/* Dessiner avec le matériau par défaut (pour tester les zones d'ombres par exemple) */
			m_rScene.GetMaterial(0).apply();
		}

		/* Parcours de la liste des meshes */
		ForEachIterC(ItMesh, CMeshesVector, m_meshesList)
		{
			(*ItMesh)->Render();
		}

		if (nDrawType == NDrawType::eSelection)
		{
			glPopName();
		}
	}
}



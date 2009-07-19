#include "CTransform.hpp"
#include "CRenderList.hpp"

/*********************************************************************************************************************/
/**		Class CTransform	          			  	 																 */
/*********************************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CTransform::CTransform() :
	m_pParentTransform(NULL)
{
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CTransform::CTransform(CPoint const& a_rPosition) :
	m_oMatrix(a_rPosition),
	m_pParentTransform(NULL)
{
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CTransform::CTransform(CPoint const& a_rPosition, CPoint const& a_rScale) :
	m_oMatrix(a_rPosition, a_rScale),
	m_pParentTransform(NULL)
{
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CTransform::CTransform(CTransform const& a_rTransform) :
	m_oMatrix(a_rTransform.m_oMatrix),
	m_pParentTransform(NULL)
{
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CTransform::CTransform(CMatrix const& a_rMatrix) :
	m_oMatrix(a_rMatrix),
	m_pParentTransform(NULL)
{
}

CTransform::~CTransform()
{
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CTransform::SetPosition(CPoint const& a_rPosition)
{
	m_oMatrix.SetTranslation(a_rPosition);

	CVector rDir = a_rPosition - GetLocalPosition();
	ForEachIter(it, CObjectsList, m_lpObjects)
	{
		(*it)->Move(rDir);
	}
}

//---------------------------------------------------------------------------------------------------------------------
// Move item at given position
//---------------------------------------------------------------------------------------------------------------------
void CTransform::Move (CVector const& a_rDir)
{
	m_oMatrix.Translate(a_rDir);

	ForEachIter(it, CObjectsList, m_lpObjects)
	{
		(*it)->Move(a_rDir);
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CTransform::Accumulate(IRenderList& a_rRenderList) const
{
	Push();
	ForEachIterC(it, CTransformsList, m_lpTransforms)
	{
		(*it)->Accumulate(a_rRenderList);
	}
	ForEachIterC(it, CObjectsList, m_lpObjects)
	{
		ISceneItem *pItem = *it;
		assert(pItem != NULL);

		a_rRenderList.CheckAndAddItem(*pItem);
	}
	Pop();
}

void CTransform::Clear()
{
	ForEachIter(it, CTransformsList, m_lpTransforms)
	{
		CTransform *pTransform = *it;
		pTransform->Clear();
		delete pTransform;
	}
	m_lpTransforms.clear();
	m_lpObjects.clear();
	// On ne delete pas les objets pour l'instant, la scène s'en occupe
//	ForEachIter(it, CObjectsList, m_lpObjects)
//	{
//		ISceneItem *pItem = *it;
//		it = m_lpObjects.erase(it);
//		delete pItem;
//	}
}

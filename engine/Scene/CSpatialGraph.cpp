#include "CSpatialGraph.hpp"

#include <values.h>
#include "CDrawState.hpp"
#include "CScene.hpp"

/*********************************************************************************************************************/
/**		Class CSpatialGraph          			  	 													   			 */
/*********************************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CSpatialGraph::CSpatialGraph() :
	m_oBBMin(FLT_MAX, FLT_MAX, FLT_MAX),
	m_oBBMax(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{
    CVisibilityState &rVisibilityState = CVisibilityState::GetInstance();
	m_uiVisibilityStateIndex = rVisibilityState.RegisterSpatialGraph(rThis);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CSpatialGraph::~CSpatialGraph()
{
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CSpatialGraph::AddTransform(CTransform* const a_spTransform)
{
	assert(a_spTransform != NULL);
	m_oRootTransform.AddChild(a_spTransform);

	// Adjust bounding box
	CPoint objMax, objMin;
	CTransform::CObjectsList const& lpObjects = a_spTransform->GetObjects();

	//assert(lpObjects.empty() == false);
	ForEachIterC(itObject, CTransform::CObjectsList, lpObjects)
	{
		(*itObject)->GetBoundingBox(objMax, objMin);

		if ( objMax.x > m_oBBMax.x)
			m_oBBMax.x = objMax.x;
		if ( objMax.y > m_oBBMax.y)
			m_oBBMax.y = objMax.y;
		if ( objMax.z > m_oBBMax.z)
			m_oBBMax.z = objMax.z;

		if ( objMin.x < m_oBBMin.x)
			m_oBBMin.x = objMin.x;
		if ( objMin.y < m_oBBMin.y)
			m_oBBMin.y = objMin.y;
		if ( objMin.z < m_oBBMin.z)
			m_oBBMin.z = objMin.z;
	}
}

//---------------------------------------------------------------------------------------------------------------------
// TODO: Fix the problem of Hard{Translate,Scale} not needed in ISceneItem
//---------------------------------------------------------------------------------------------------------------------
/*void CSpatialGraph::Normalize()
{
	// Center scene on 0,0,0
	CPoint offset = CPoint(0,0,0) - ((ptMax + ptMin)/2.0f);
	ForEachIterC(itTransform, CTransformsList, m_lpTransforms)
	{
		CTransform::CObjectsList const& lpObjects = (*itTransform)->GetObjects();

		ForEachIterC(itObject, CTransform::CObjectsList, lpObjects)
		{
			(*itObject)->HardTranslate(offset);
		}
	}
	m_oBBMax = ptMax + offset;
	m_oBBMin = ptMin + offset;

	// Normalize scene
	float invNormMax = 1.0f/((ptMax - ptMin).max());

	ForEachIterC(itTransform, CTransformsList, m_lpTransforms)
	{
		CTransform::CObjectsList const& lpObjects = (*itTransform)->GetObjects();

		ForEachIterC(itObject, CTransform::CObjectsList, lpObjects)
		{
			(*itObject)->HardScale(invNormMax);
		}
	}
	m_oBBMax = ptMax * invNormMax;
	m_oBBMin = ptMin * invNormMax;
}*/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CSpatialGraph::ComputeVisibility()
{
	CCamera &rCamera = CCamera::GetInstance();

	CTransform::CObjectsList const& lpObjects = m_oRootTransform.GetObjects();

	ForEachIterC(itObject, CTransform::CObjectsList, lpObjects)
	{
		(*itObject)->ComputeVisibility(rCamera);
	}

	// Notify visibility state that we are updated
	CVisibilityState &rVisibilityState = CVisibilityState::GetInstance();
	rVisibilityState.VisibilityUpdated(m_uiVisibilityStateIndex);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
bool CSpatialGraph::IsUpdateVisibilityNeeded() const
{
	CVisibilityState const& rVisibilityState = CVisibilityState::GetInstance();
	return rVisibilityState.IsUpdateVisibilityNeeded(m_uiVisibilityStateIndex);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
//void CSpatialGraph::AppendSpatialGraph(CSpatialGraph const& a_rGraph, CTransform& a_rTransform)
//{
//	ForEachIterC(itTransform, CTransformsList, a_rGraph.m_lpTransforms)
//	{
//		CTransform *pTransform = new CTransform(**itTransform);
//		m_lpTransforms.push_back(pTransform);
//	}
//	a_rTransform.AddChild(*a_rGraph.m_lpTransforms.begin());
//}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CSpatialGraph::Clear()
{
//	m_oRootTransform.Clear();
}

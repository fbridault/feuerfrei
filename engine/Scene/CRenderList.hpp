#ifndef CRENDERLIST_H
#define CRENDERLIST_H

#include <list>

#include "CTransform.hpp"
#include "CObject.hpp"

class CSpatialGraph;

class IRenderList
{
protected:
	typedef list<ISceneItem const*> CItemsList;
	typedef list<CTransform> CTransformsList;

public:

//---------------------------------------------------------------------------------------------------------------------
//  Public methods
//---------------------------------------------------------------------------------------------------------------------

	void Accumulate(CSpatialGraph& a_rGraph);

	/** Add an item to the list */
	void CheckAndAddItem(ISceneItem& a_rSceneItem);

//---------------------------------------------------------------------------------------------------------------------
//  Virtual methods
//---------------------------------------------------------------------------------------------------------------------

	virtual void Render() const = 0;

private:

	virtual bool CheckItem(ISceneItem& a_rSceneItem) = 0;

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

protected:
	/** List containing all items to be rendered. */
	CItemsList m_lpItems;

	/** List containing local transforms of items listed above. */
	CTransformsList m_lTransforms;
};

class CRenderList : public IRenderList
{
public:
	CRenderList(NRenderType a_nRenderType) : m_nRenderType(a_nRenderType) {}
	virtual ~CRenderList() {}

//---------------------------------------------------------------------------------------------------------------------
//  Inherited methods
//---------------------------------------------------------------------------------------------------------------------

	virtual void Render() const;

private:

	virtual bool CheckItem(ISceneItem& a_rSceneItem)
	{
		return (a_rSceneItem.GetType() == m_nRenderType);
	}


//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

	NRenderType m_nRenderType;

	bool m_bIsUpdated;
};

//template<>
//void CRenderList<NRenderType::eNormal>::Render() const
//{
// 	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
//  	glEnableClientState(GL_VERTEX_ARRAY);
//
//	ForEachIter(it, CItemsList, m_lpItems)
//	{
//		(*it)->Render();
//	}
//
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
//	glBindBuffer(GL_ARRAY_BUFFER, 0 );
//}

#endif // CRENDERLIST_H

#ifndef CSPATIALGRAPH_H
#define CSPATIALGRAPH_H

#include "../Maths/CPoint.hpp"
#include "../Utility/SmartPtr.hpp"
#include "CTransform.hpp"
#include <list>


/*********************************************************************************************************************/
/**		Class CSpatialGraph          			  	 													   			 */
/*********************************************************************************************************************/
class CSpatialGraph
{
public:

	CSpatialGraph();
	~CSpatialGraph();

//---------------------------------------------------------------------------------------------------------------------
//  Specific methods
//---------------------------------------------------------------------------------------------------------------------

	/** Add a transform in the graph.
	 * @param newObj Pointer.
	 */
	void AddTransform(CTransform* const a_pTransform);

	/** Retrieve bounding box */
	void GetBoundingBox(CPoint& a_rMin, CPoint& a_rMax)
	{
		a_rMin = m_oBBMin;
		a_rMax = m_oBBMax;
	}

	/** Initiate visibility computing on all objects of the graph */
	void ComputeVisibility();

	bool IsUpdateVisibilityNeeded() const;

	CTransform const& GetRootTransform() const
	{
		return m_oRootTransform;
	}

	/** Remove all tranforms and objects */
	void Clear();

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

protected:

	uint m_uiVisibilityStateIndex;

	/** Root transform du graph */
	CTransform m_oRootTransform;

private:

	/** Englobant */
	CPoint m_oBBMin, m_oBBMax;
};

#endif // SPATIALGRAPH_H

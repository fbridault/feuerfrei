#ifndef DRAWSTATE_H
#define DRAWSTATE_H

#include "../Utility/ISingleton.hpp"
#include "../Utility/Numerable.hpp"
#include <values.h>

//*********************************************************************************************************************
// Filter according to shading applied on objects
//*********************************************************************************************************************
struct _NShadingFilter
{
	enum EValue
	{
		eFlat,
		eTextured,
		eAll,

		_NbValues
	};
};
DeclareNumerable(_NShadingFilter, NShadingFilter);

//*********************************************************************************************************************
// Modify shading on objects
//*********************************************************************************************************************
struct _NShadingType
{
	enum EValue
	{
		eNormal,
		eNoTexture,
		eAmbient,

		_NbValues
	};
};
DeclareNumerable(_NShadingType, NShadingType);

//*********************************************************************************************************************
// Draw type
//*********************************************************************************************************************
struct _NDrawType
{
	enum EValue
	{
		eObject,
		eBoundingSphere,
		eSelection,

		_NbValues
	};
};
DeclareNumerable(_NDrawType, NDrawType);

/*********************************************************************************************************************/
/**		Class CDrawState	          			  	 																 */
/*********************************************************************************************************************/
class CDrawState : public ITSingleton<CDrawState>
{
	friend class ITSingleton<CDrawState>;

private:
	/**
	 * Constructeur par défaut.
	 */
	CDrawState() : m_uiLastMaterialIndex(INT_MAX) {};
	/** Destructeur par d&eacute;faut. */
	~CDrawState();

public:

	NShadingFilter const& GetShadingFilter() const { return m_nShadingFilter; };
	void SetShadingFilter(NShadingFilter const& a_nShadingFilter) { m_nShadingFilter = a_nShadingFilter; }

	NShadingType const& GetShadingType() const { return m_nShadingType; };
	void SetShadingType(NShadingType const& a_nShadingType) { m_nShadingType = a_nShadingType; }

	NDrawType const& GetDrawType() const { return m_nDrawType; };
	void SetDrawType(NDrawType const& a_nDrawType) { m_nDrawType = a_nDrawType; }

	uint GetLastMaterialIndex() { return m_uiLastMaterialIndex; };
	void SetLastMaterialIndex(uint a_uiLastMaterialIndex)
	{
		m_uiLastMaterialIndex = a_uiLastMaterialIndex;
	}

private:

	NShadingFilter m_nShadingFilter;
	NShadingType m_nShadingType;
	NDrawType m_nDrawType;
	uint m_uiLastMaterialIndex;
};

#include "../Scene/CSpatialGraph.hpp"
#include <vector>

/*********************************************************************************************************************/
/**		Class CVisibilityState         			  	 																 */
/*********************************************************************************************************************/
class CVisibilityState : public ITSingleton<CVisibilityState>
{
	friend class ITSingleton<CVisibilityState>;

private:
	/**
	 * Constructeur par défaut.
	 */
	CVisibilityState() : m_bUpdateVisibility(true) {};
	/** Destructeur par d&eacute;faut. */
	~CVisibilityState();

public:

	bool IsUpdateVisibilityNeeded(uint a_uiIndex) const
	{
		return !m_vbVisibilityUpdated[a_uiIndex] && m_bUpdateVisibility;
	};

	void TriggerUpdateVisibility()
	{
		m_bUpdateVisibility = true;
		ForEachIter(it, CBoolVector, m_vbVisibilityUpdated)
		{
			*it = false;
		}
	}

	void VisibilityUpdated(uint a_uiIndex)
	{
		// We are not supposed to update visibility if nothing was asked
		assert(m_bUpdateVisibility == true);

		m_vbVisibilityUpdated[a_uiIndex] = true;
		ForEachIterC(it, CBoolVector, m_vbVisibilityUpdated)
		{
			if( *it == false)
				return;
		}
		m_bUpdateVisibility = false;
	};

	uint RegisterSpatialGraph(CSpatialGraph const& a_rSpatialGraph)
	{
		m_vrSpatialGraphs.push_back(&a_rSpatialGraph);

		uint uiGraphIndex = m_vbVisibilityUpdated.size();
		m_vbVisibilityUpdated.push_back(true);
		return uiGraphIndex;
	}

private:

	bool m_bUpdateVisibility;
	typedef vector<CSpatialGraph const*> CSpatialGraphVector;
	CSpatialGraphVector m_vrSpatialGraphs;
	typedef vector<bool> CBoolVector;
	CBoolVector m_vbVisibilityUpdated;
};

#endif // DRAWSTATE_H

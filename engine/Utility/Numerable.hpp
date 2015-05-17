#ifndef NUMERABLE_H
#define NUMERABLE_H

#include "../Common.hpp"

template <typename t_object>
class CTNumerable : public t_object
{
private:
	typedef t_object CType;

public:
	CTNumerable() : m_eVal(CType::_NbValues){};

	CTNumerable(CTNumerable const& a_rSource)
	{
		assert(a_rSource.m_eVal < CType::_NbValues);
		m_eVal = a_rSource.m_eVal;
	}

	CTNumerable(enum CType::EValue const& a_rSource)
	{
		assert(a_rSource < CType::_NbValues);
		m_eVal = a_rSource;
	}

	explicit CTNumerable(uint a_uiSource)
	{
		assert(a_uiSource < CType::_NbValues);
		m_eVal = a_uiSource;
	}

	~CTNumerable() {};

	CTNumerable<CType>& operator=(CTNumerable<CType> const& a_rSource)
	{
		assert(a_rSource.m_eVal < CType::_NbValues);
		m_eVal = a_rSource.m_eVal;
		return *this;
	}

	CTNumerable<CType>& operator=(uint a_uiSource)
	{
		assert(a_uiSource <	CType::_NbValues);
		m_eVal = a_uiSource;
		return *this;
	}

	bool operator==(enum CType::EValue const& a_rSource) const
	{
		assert(a_rSource < CType::_NbValues);
		return m_eVal == a_rSource;
	}

	bool operator==(CTNumerable<CType> const& a_nSource) const
	{
		assert(a_nSource.m_eVal < CType::_NbValues);
		return m_eVal == a_nSource.m_eVal;
	}

	bool operator!=(enum CType::EValue const& a_rSource) const
	{
		return !(rThis == a_rSource);
	}

	bool operator!=(CTNumerable<CType> const& a_nSource) const
	{
		return !(rThis == a_nSource);
	}

	// Allow conversion to int, dangerous !!!
	operator int() const
	{
		assert(m_eVal < CType::_NbValues);
		return m_eVal;
	}

private:

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

	int m_eVal;
};

#define DeclareNumerable(x_Enum, x_Class)\
	typedef CTNumerable<x_Enum> x_Class;


#endif // NUMERABLE_H

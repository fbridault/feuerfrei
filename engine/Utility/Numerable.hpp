#ifndef NUMERABLE_H
#define NUMERABLE_H

template <typename t_object>
class CTNumerable : public t_object
{
private:
	typedef t_object CType;

public:
	CTNumerable() {};
	~CTNumerable() {};

	CTNumerable<t_object>& operator=(CTNumerable<t_object> const& a_rSource)
	{
		eVal = a_rSource.eVal;
		return *this;
	}

	CTNumerable<t_object>& operator=(uint a_uiSource)
	{
		assert(a_uiSource <	CType::_NbValues);
		eVal = a_uiSource;
		return *this;
	}

	bool operator==(uint a_uiSource) const
	{
		assert(a_uiSource <	CType::_NbValues);
		return eVal == a_uiSource;
	}

private:
	uint eVal;
};

#define DeclareNumerable(x_Enum, x_Class)\
	typedef CTNumerable<struct  x_Enum> x_Class;


#endif // NUMERABLE_H

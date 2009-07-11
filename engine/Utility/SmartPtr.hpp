#if !defined(SMARTPTR_H)
#define SMARTPTR_H



template<class t_Class>
class SmartPtr
{
	typedef t_Class CType;

public:

	SmartPtr() : m_pObject(NULL), m_uiRefCount(0) {}
	~SmartPtr()
	{
		// Ensure no one still references the pointer
		assert(m_pObject == NULL);
		assert(m_uiRefCount == 0);
	}

	SmartPtr& operator=(CType const* a_pObject)
	{
		if(a_pObject == NULL)
		{
			m_uiRefCount--;
			if(m_uiRefCount == 0)
			{
				delete m_pObject;
			}
		}
		else
		{
			m_pObject = a_pObject;
		}
		return rThis;
	}

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

private:

	CType* m_pObject;
	uint m_uiRefCount;
};

#endif //

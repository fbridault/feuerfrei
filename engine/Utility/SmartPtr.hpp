#if !defined(SMARTPTR_H)
#define SMARTPTR_H

template<class t_Class> class CSmartPtr;

class CSmartPtrRefCounter
{
	template<class t_Class> friend class CSmartPtr;

private:

	CSmartPtrRefCounter() : m_uiRefCount(1) {}

	void Increase() {  m_uiRefCount++; }
	bool Decrease() {  assert(m_uiRefCount > 0); m_uiRefCount--; return (m_uiRefCount > 0); }

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

private:

	uint m_uiRefCount;
};

template<class t_Class>
class CSmartPtr
{
	typedef t_Class CType;

public:

	CSmartPtr() : m_pObject(NULL), m_pRefCounter(NULL) {}

	CSmartPtr(CType* a_pObject) : m_pObject(a_pObject)
	{
		// Allocate a new ref counter
		m_pRefCounter = new CSmartPtrRefCounter();
	}

	~CSmartPtr()
	{
		if(m_pRefCounter != NULL)
		{
			if(m_pRefCounter->Decrease() == false)
			{
				delete m_pObject;
				delete m_pRefCounter;
			}
		}
	}

	CSmartPtr& operator=(CSmartPtr const* a_spObject)
	{
		if(a_spObject == NULL)
		{
			// If we do have a pointer
			if(m_pRefCounter != NULL)
			{
				// Decrease the ref counter and test if we are the last
				if(m_pRefCounter->Decrease() == false)
				{
					delete m_pObject;
					delete m_pRefCounter;
					m_pObject = NULL;
					m_pRefCounter = NULL;
				}
			}
		}
		else
		{
			// Set new pointed object
			m_pObject = a_spObject.m_pObject;

			// If we do have a pointer, decrease ref counter before setting the new one
			if(m_pRefCounter != NULL)
			{
				m_pRefCounter->Decrease();
			}
			m_pRefCounter = a_spObject.m_pRefCounter;
		}
		return rThis;
	}

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

private:

	CType* m_pObject;
	CSmartPtrRefCounter* m_pRefCounter;
};

#endif //

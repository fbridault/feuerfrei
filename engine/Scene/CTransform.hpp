#ifndef TRANSFORM_H
#define TRANSFORM_H

class CTransform;
class IRenderList;

#include "CObject.hpp"
#include "../Maths/CMatrix.hpp"

/*********************************************************************************************************************/
/**		Class CTransform          			  	 														   			 */
/*********************************************************************************************************************/
class CTransform
{
public:
	typedef list<ISceneItem *> CObjectsList;
	typedef list<CTransform *> CTransformsList;

	CTransform();
	CTransform(CPoint const& a_rPosition);
	CTransform(CPoint const& a_rPosition, CPoint const& a_rScale);
	CTransform(CTransform const& a_rTransform);
	CTransform(CMatrix const& a_rMatrix);

	~CTransform() {};

	/** Get item position */
	CPoint GetLocalPosition () const
	{
		return m_oMatrix.GetTranslation();
	}
	/** Get item position in world space - recursive */
	// TODO: Improve with a matrix cache !
	CPoint GetWorldPosition () const
	{
		if(m_pParentTransform == NULL)
		{
			return m_oMatrix.GetTranslation();
		}
		else
		{
			return (m_oMatrix.GetTranslation() + m_pParentTransform->GetWorldPosition());
		}
	}

	/** Get/Set item position */
	void GetLocalPosition (float &x, float &y, float &z) const
	{
		CPoint oPoint = m_oMatrix.GetTranslation();
		x = oPoint.x;
		y = oPoint.y;
		z = oPoint.z;
	}
	void SetPosition (CPoint const& a_rPosition);

	/** Move item at given position */
	void Move (CVector const& a_rDir);

	/** Get item scale factor */
	CPoint GetScale () const
	{
		return m_oMatrix.GetScale();
	}
	/** Set item scale factor */
	void SetScale (CPoint const& a_rScale)
	{
		assert(false);
//		m_oScale = a_rScale;
	}

	/** Accumulate children of this transform into the given rendering list */
	void Accumulate(IRenderList& a_rRenderList) const;

	void Push() const
	{
		glPushMatrix();
		glMultMatrixf(m_oMatrix);
	}

	void Pop() const
	{
		glPopMatrix();
	}

	CObjectsList const& GetObjects() const
	{
		return m_lpObjects;
	}

	uint GetNumObjects() const
	{
		return m_lpObjects.size();
	}

	CObjectsList& GrabObjects()
	{
		return m_lpObjects;
	}

	CTransformsList const& GetTransforms() const
	{
		return m_lpTransforms;
	}

	void AddChild(ISceneItem* const a_pObject)
	{
		assert(a_pObject != NULL);
		m_lpObjects.push_back(a_pObject);
	}

	void AddChild(CTransform* const a_pTransform)
	{
		assert(a_pTransform != NULL);
		m_lpTransforms.push_back(a_pTransform);
		a_pTransform->AddParent(this);
	}

private:

	void AddParent(CTransform* const a_pTransform)
	{
		assert(a_pTransform != NULL);
		m_pParentTransform = a_pTransform;
	}

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

	CMatrix m_oMatrix;

	CObjectsList m_lpObjects;
	CTransformsList m_lpTransforms;
	CTransform* m_pParentTransform;
};

#endif // TRANSFORM_H

#include "CRenderList.hpp"
#include "CObject.hpp"
#include "CSpatialGraph.hpp"

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IRenderList::Accumulate(CSpatialGraph& a_rGraph)
{
	{
		// First clear all the lists
		m_lpItems.clear();
		m_lTransforms.clear();

		// Reset the matrix transform cache
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Then accumulate graph
		CTransform const& rTransform = a_rGraph.GetRootTransform();
		rTransform.Accumulate(rThis);

		glPopMatrix();
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IRenderList::CheckAndAddItem(ISceneItem& a_rSceneItem)
{
	if( CheckItem(a_rSceneItem) && a_rSceneItem.IsVisible() )
	{
		m_lpItems.push_back(&a_rSceneItem);

		// At the moment, rely on OpenGL push/pop matrix
		GLfloat f44Matrix[16];
		glGetFloatv (GL_MODELVIEW_MATRIX, f44Matrix);
		CMatrix oMatrix(f44Matrix);

		m_lTransforms.push_back(CTransform(oMatrix));
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CRenderList::Render() const
{
	if(m_nRenderType == NRenderType::eNormal)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
	}

	glMatrixMode(GL_MODELVIEW);
	CTransformsList::const_iterator itTransform = m_lTransforms.begin();
	ForEachIterC(it, CItemsList, m_lpItems)
	{
		CTransform const& rTransform = (*itTransform);
		rTransform.Push();

		(*it)->Render();

		rTransform.Pop();
		itTransform++;
	}

	if(m_nRenderType == NRenderType::eNormal)
	{
		GLint iTex2DEnable;
		glGetIntegerv(GL_TEXTURE_2D, &iTex2DEnable);
		if(iTex2DEnable == GL_TRUE)
		{
			glDisable(GL_TEXTURE_2D);
		}

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
		glBindBuffer(GL_ARRAY_BUFFER, 0 );
	}
}

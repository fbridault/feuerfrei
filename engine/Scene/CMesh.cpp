#include "CMesh.hpp"
#include "../Utility/CRefTable.hpp"
#include "CObject.hpp"
#include "CScene.hpp"
#include "CMaterial.hpp"

extern uint g_objectCount;

/**************************************************************************************************/
/**							DEFINITION DE LA CLASSE MESH 					 	    			  */
/**************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CMesh::CMesh (CScene& a_rScene, uint a_uiMaterial, CObject& a_rParent) :
		m_rScene(a_rScene), m_rParent(a_rParent), m_uiMaterial(a_uiMaterial), m_attributes(0)
{
	glGenBuffers(1, &m_bufferID);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CMesh::~CMesh ()
{
	m_indexArray.clear();
	glDeleteBuffers(1, &m_bufferID);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CMesh::BuildVBO() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexArray.size()*sizeof(GLuint), &m_indexArray[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CMesh::buildBoundingSphere (CRefTable& refTable)
{
	float dist;
	Vertex v;
	uint n=0;
	refTable.reinit();

	CPoint& rCenter = m_boundingSphere.GrabCentre();
	CPoint oCenter;

	for (uint i = 0; i < m_indexArray.size(); i++)
	{
		/* Take each point only once. */
		if ( !refTable.findRef( m_indexArray[i] ) )
		{
			v = m_rParent.GetVertex(m_indexArray[i]);
			refTable.addRef( m_indexArray[i] );
			oCenter = (oCenter*n + CPoint(v.x, v.y, v.z)) / (float)(n+1);
			n++;
		}
	}
	/* Bounding sphere center may already contains translation in world */
	rCenter = rCenter + oCenter;

	float& rRadius = m_boundingSphere.GrabRadius();
	for (vector < GLuint >::iterator indexIterator = m_indexArray.begin ();
	     indexIterator != m_indexArray.end ();  indexIterator++)
	{
		CPoint p;
		v=m_rParent.GetVertex(*indexIterator);
		p=CPoint(v.x, v.y, v.z);
		dist=p.squaredDistanceFrom(rCenter);
		if ( dist > rRadius)
			rRadius = dist;
	}
	rRadius = sqrt(rRadius);
	//cerr << "sphere de centre " << m_centre << " et de rayon " << m_radius << endl;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CMesh::drawBoundingSphere()
{
	m_boundingSphere.draw();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
bool CMesh::computeVisibility(const CCamera &view)
{
	return m_boundingSphere.isVisible(view);
	//if(m_visibility) g_objectCount++;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CMesh::Render() const
{
	CDrawState &rDrawState = CDrawState::GetInstance();
	NShadingFilter const& nShadingFilter = rDrawState.GetShadingFilter();
	NShadingType const& nShadingType = rDrawState.GetShadingType();
	uint a_uiLastMaterialIndex = rDrawState.GetLastMaterialIndex();

	CMaterial const& rMaterial = m_rScene.GetMaterial(m_uiMaterial);

	if (nShadingFilter == NShadingFilter::eTextured)
	{
		/* Ne dessiner que si il y a une texture */
		if (!rMaterial.hasDiffuseTexture())
			return;
	}
	else
		if (nShadingFilter == NShadingFilter::eFlat)
		{
			/* Ne dessiner que si il n'y a pas de texture */
			if (rMaterial.hasDiffuseTexture())
				return;
		}

	if (nShadingType != NShadingType::eAmbient)
		if ( m_uiMaterial != a_uiLastMaterialIndex)
		{
			CMaterial const& rLastMaterial = m_rScene.GetMaterial(a_uiLastMaterialIndex);

			if ( rMaterial.hasDiffuseTexture() && nShadingType == NShadingType::eNormal)
			{
				/* Inutile de réactiver l'unité de texture si le matériau précédent en avait une */
				/* TODO : state cache !!! */

				GLint iTex2DEnable;
				glGetIntegerv(GL_TEXTURE_2D, &iTex2DEnable);
				if(iTex2DEnable == GL_FALSE)
				{
//				}
//				if (!rLastMaterial.hasDiffuseTexture())
//				{
					glEnable(GL_TEXTURE_2D);
					glActiveTexture(GL_TEXTURE0+OBJECT_TEX_UNIT);
					glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
				}

				ITexture const& rTexture = rMaterial.GetDiffuseTexture();
				rTexture.bind();
			}
			else
				if ( rLastMaterial.hasDiffuseTexture() && nShadingType == NShadingType::eNormal)
				{
					/* Pas de texture pour le matériau courant, on désactive l'unité de texture car le matériau précédent en avait une */
					glDisable(GL_TEXTURE_2D);
				}
			rMaterial.apply();
		}

	m_rParent.bindVBO();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);

	glDrawElements(GL_TRIANGLES, m_indexArray.size(), GL_UNSIGNED_INT, 0);

	rDrawState.SetLastMaterialIndex(m_uiMaterial);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
const bool CMesh::isTransparent () const
{
	if (m_rScene.GetMaterial(m_uiMaterial).hasDiffuseTexture())
		if ( m_rScene.GetMaterial(m_uiMaterial).isTransparent())
			return true;
	return false;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
float CMesh::GetArea() const
{
	float area=0.0f;

	for (uint i = 0; i < m_indexArray.size(); i+=3)
	{
		Vertex V1 = m_rParent.GetVertex(m_indexArray[i]);
		Vertex V2 = m_rParent.GetVertex(m_indexArray[i+1]);
		Vertex V3 = m_rParent.GetVertex(m_indexArray[i+2]);
		CPoint P1(V1.x,V1.y,V1.z);
		CPoint P2(V2.x,V2.y,V2.z);
		CPoint P3(V3.x,V3.y,V3.z);

		area += CPoint::GetTriangleArea(P1,P2,P3);
	}
	return area;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CMesh::GetTriangle(uint iTriangle, CPoint &P1, CPoint &P2, CPoint &P3) const
{
	Vertex V1 = m_rParent.GetVertex(m_indexArray[iTriangle*3]);
	Vertex V2 = m_rParent.GetVertex(m_indexArray[iTriangle*3+1]);
	Vertex V3 = m_rParent.GetVertex(m_indexArray[iTriangle*3+2]);

	P1.x=V1.x;
	P1.y=V1.y;
	P1.z=V1.z;
	P2.x=V2.x;
	P2.y=V2.y;
	P2.z=V2.z;
	P3.x=V3.x;
	P3.y=V3.y;
	P3.z=V3.z;

	/** Prise en compte de la translation */
	CTransform const& rTransform = m_rParent.GetTransform();
	P1 += rTransform.GetLocalPosition();
	P2 += rTransform.GetLocalPosition();
	P3 += rTransform.GetLocalPosition();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CMesh::GetTriangle(uint iTriangle, CPoint &P1, CPoint &P2, CPoint &P3, CVector& normal) const
{
	Vertex V1 = m_rParent.GetVertex(m_indexArray[iTriangle*3]);
	Vertex V2 = m_rParent.GetVertex(m_indexArray[iTriangle*3+1]);
	Vertex V3 = m_rParent.GetVertex(m_indexArray[iTriangle*3+2]);
	CVector N(V1.nx,V1.ny,V1.nz);

	P1.x=V1.x;
	P1.y=V1.y;
	P1.z=V1.z;
	P2.x=V2.x;
	P2.y=V2.y;
	P2.z=V2.z;
	P3.x=V3.x;
	P3.y=V3.y;
	P3.z=V3.z;
	/** Prise en compte de la translation */
	CTransform const& rTransform = m_rParent.GetTransform();
	P1 += rTransform.GetLocalPosition();
	P2 += rTransform.GetLocalPosition();
	P3 += rTransform.GetLocalPosition();

	normal = CVector(P2-P1)^CVector(P3-P1);
	normal.normalize();

	if (normal*N < 0.0)
		normal = CVector(0.0,0.0,0.0) - normal;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CVector CMesh::generateRandomRayHemisphere(uint iTriangle) const
{
	float r1,r2;
	float gamma;
	CPoint P1,P2,P3;
	CVector rayDir,dir,normal;
	CVector iAxis,jAxis;

	GetTriangle(iTriangle,P1,P2,P3,normal);

	r1 = rand()/(float)RAND_MAX;
	r2 = rand()/(float)RAND_MAX;

	gamma=sqrt(1-r2*r2);

	dir.x = cos ( 2*M_PI*r1 ) * gamma;
	dir.y = sin ( 2*M_PI*r1 ) * gamma;
	dir.z = r2;

	jAxis=(normal^normal.GetMinCoord()).normalize();
	iAxis=( jAxis^normal).normalize();

	rayDir.x = iAxis.x*dir.x + jAxis.x*dir.y + normal.x*dir.z;
	rayDir.y = iAxis.y*dir.x + jAxis.y*dir.y + normal.y*dir.z;
	rayDir.z = iAxis.z*dir.x + jAxis.z*dir.y + normal.z*dir.z;

	return rayDir;
}


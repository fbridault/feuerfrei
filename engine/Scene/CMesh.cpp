#include "CMesh.hpp"
#include "../Utility/CRefTable.hpp"
#include "CObject.hpp"
#include "CScene.hpp"
#include "CMaterial.hpp"

/**************************************************************************************************/
/**														 DEFINITION DE LA CLASSE MESH 												 	    			*/
/**************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
CMesh::CMesh (CScene* const a_pScene, uint a_uiMaterial, CObject* a_pParent) :
		m_pScene(a_pScene), m_pParent(a_pParent), m_uiMaterial(a_uiMaterial), m_attributes(0), m_visibility(true)
{
	glGenBuffers(1, &m_bufferID);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
CMesh::~CMesh ()
{
	m_indexArray.clear();
	glDeleteBuffers(1, &m_bufferID);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CMesh::buildVBO() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexArray.size()*sizeof(GLuint), &m_indexArray[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CMesh::buildBoundingSphere (CRefTable& refTable)
{
	float dist;
	Vertex v;
	uint n=0;
	refTable.reinit();

	for (uint i = 0; i < m_indexArray.size(); i++)
	{
		/* Il ne faut prendre un même point qu'une seule fois en compte. */
		if ( !refTable.findRef( m_indexArray[i] ) )
		{
			v = m_pParent->getVertex(m_indexArray[i]);
			refTable.addRef( m_indexArray[i] );
			m_boundingSphere.centre = (m_boundingSphere.centre*n + CPoint(v.x, v.y, v.z)) / (float)(n+1);
			n++;
		}
	}

	for (vector < GLuint >::iterator indexIterator = m_indexArray.begin ();
	     indexIterator != m_indexArray.end ();  indexIterator++)
	{
		CPoint p;
		v=m_pParent->getVertex(*indexIterator);
		p=CPoint(v.x, v.y, v.z);
		dist=p.squaredDistanceFrom(m_boundingSphere.centre);
		if ( dist > m_boundingSphere.radius)
			m_boundingSphere.radius = dist;
	}
	m_boundingSphere.radius = sqrt(m_boundingSphere.radius);
	//cerr << "sphere de centre " << m_centre << " et de rayon " << m_radius << endl;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CMesh::drawBoundingSphere()
{
	m_boundingSphere.draw();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CMesh::computeVisibility(const CCamera &view)
{
	m_visibility = m_boundingSphere.isVisible(view);
//  if(m_visibility) g_objectCount++;
	return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CMesh::draw (char drawCode, bool tex, uint &lastMaterialIndex) const
{
	if (!m_visibility)
		return;

	if (drawCode == TEXTURED)
	{
		/* Ne dessiner que si il y a une texture */
		if (!m_pScene->getMaterial(m_uiMaterial)->hasDiffuseTexture())
			return;
	}
	else
		if (drawCode == FLAT)
		{
			/* Ne dessiner que si il n'y a pas de texture */
			if (m_pScene->getMaterial(m_uiMaterial)->hasDiffuseTexture())
				return;
		}

	if (drawCode != AMBIENT)
		if ( m_uiMaterial != lastMaterialIndex)
		{
			if (m_pScene->getMaterial(m_uiMaterial)->hasDiffuseTexture() && tex)
			{
				/* Inutile de réactiver l'unité de texture si le matériau précédent en avait une */
				if (!m_pScene->getMaterial(lastMaterialIndex)->hasDiffuseTexture())
				{
					glActiveTexture(GL_TEXTURE0+OBJECT_TEX_UNIT);
					glEnable(GL_TEXTURE_2D);
					glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
				}
				m_pScene->getMaterial(m_uiMaterial)->getDiffuseTexture()->bind();
			}
			else
				if (m_pScene->getMaterial(lastMaterialIndex)->hasDiffuseTexture() && tex)
				{
					/* Pas de texture pour le matériau courant, on désactive l'unité de texture car le matériau précédent en avait une */
					glDisable(GL_TEXTURE_2D);
				}
			m_pScene->getMaterial(m_uiMaterial)->apply();
		}

	m_pParent->bindVBO();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);

	glDrawElements(GL_TRIANGLES, m_indexArray.size(), GL_UNSIGNED_INT, 0);

	lastMaterialIndex = m_uiMaterial;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CMesh::drawForSelection () const
{
	if (!m_visibility)
		return;

	m_pParent->bindVBO();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);

	glDrawElements(GL_TRIANGLES, m_indexArray.size(), GL_UNSIGNED_INT, 0);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
const bool CMesh::isTransparent () const
{
	if (m_pScene->getMaterial(m_uiMaterial)->hasDiffuseTexture())
		if ( m_pScene->getMaterial(m_uiMaterial)->isTransparent())
			return true;
	return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
float CMesh::getArea() const
{
	float area=0.0f;

	for (uint i = 0; i < m_indexArray.size(); i+=3)
	{
		Vertex V1 = m_pParent->getVertex(m_indexArray[i]);
		Vertex V2 = m_pParent->getVertex(m_indexArray[i+1]);
		Vertex V3 = m_pParent->getVertex(m_indexArray[i+2]);
		CPoint P1(V1.x,V1.y,V1.z);
		CPoint P2(V2.x,V2.y,V2.z);
		CPoint P3(V3.x,V3.y,V3.z);

		area += CPoint::getTriangleArea(P1,P2,P3);
	}
	return area;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CMesh::getTriangle(uint iTriangle, CPoint &P1, CPoint &P2, CPoint &P3) const
{
	Vertex V1 = m_pParent->getVertex(m_indexArray[iTriangle*3]);
	Vertex V2 = m_pParent->getVertex(m_indexArray[iTriangle*3+1]);
	Vertex V3 = m_pParent->getVertex(m_indexArray[iTriangle*3+2]);

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
	P1 += m_pParent->getPosition();
	P2 += m_pParent->getPosition();
	P3 += m_pParent->getPosition();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CMesh::getTriangle(uint iTriangle, CPoint &P1, CPoint &P2, CPoint &P3, CVector& normal) const
{
	Vertex V1 = m_pParent->getVertex(m_indexArray[iTriangle*3]);
	Vertex V2 = m_pParent->getVertex(m_indexArray[iTriangle*3+1]);
	Vertex V3 = m_pParent->getVertex(m_indexArray[iTriangle*3+2]);
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
	P1 += m_pParent->getPosition();
	P2 += m_pParent->getPosition();
	P3 += m_pParent->getPosition();

	normal = CVector(P2-P1)^CVector(P3-P1);
	normal.normalize();

	if (normal*N < 0.0)
		normal = CVector(0.0,0.0,0.0) - normal;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
CVector CMesh::generateRandomRayHemisphere(uint iTriangle) const
{
	float r1,r2;
	float gamma;
	CPoint P1,P2,P3;
	CVector rayDir,dir,normal;
	CVector iAxis,jAxis;

	getTriangle(iTriangle,P1,P2,P3,normal);

	r1 = rand()/(float)RAND_MAX;
	r2 = rand()/(float)RAND_MAX;

	gamma=sqrt(1-r2*r2);

	dir.x = cos ( 2*M_PI*r1 ) * gamma;
	dir.y = sin ( 2*M_PI*r1 ) * gamma;
	dir.z = r2;

	jAxis=(normal^normal.getMinCoord()).normalize();
	iAxis=( jAxis^normal).normalize();

	rayDir.x = iAxis.x*dir.x + jAxis.x*dir.y + normal.x*dir.z;
	rayDir.y = iAxis.y*dir.x + jAxis.y*dir.y + normal.y*dir.z;
	rayDir.z = iAxis.z*dir.x + jAxis.z*dir.y + normal.z*dir.z;

	return rayDir;
}


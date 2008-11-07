#include "CScene.hpp"

#include <values.h>

#include "CMaterial.hpp"
#include "../Utility/UObjImporter.hpp"


uint CScene::glNameCounter = 0;

void CScene::init (const string& a_strFileName)
{
	m_boundingSpheresMode = false;
	m_selectedItem        = NULL;

	addMaterial(new CMaterial(rThis));

	cout << "Chargement de la scène " << a_strFileName << endl;
	UObjImporter::import(rThis, a_strFileName, m_objectsArray);
}


CScene::~CScene ()
{
	for (vector < CObject * >::iterator objectsArrayIterator = m_objectsArray.begin ();
	        objectsArrayIterator != m_objectsArray.end (); objectsArrayIterator++)
		delete (*objectsArrayIterator);
	m_objectsArray.clear ();

	for (vector < ILight * >::iterator lightSourcesIterator = m_lightSourcesArray.begin ();
	        lightSourcesIterator != m_lightSourcesArray.end (); lightSourcesIterator++)
		delete (*lightSourcesIterator);
	m_lightSourcesArray.clear ();

	for (vector < CMaterial * >::iterator materialArrayIterator = m_materialArray.begin ();
	        materialArrayIterator != m_materialArray.end (); materialArrayIterator++)
		delete (*materialArrayIterator);
	m_materialArray.clear ();

	for (vector < CBitmapTexture * >::iterator texturesArrayIterator = m_texturesArray.begin ();
	        texturesArrayIterator != m_texturesArray.end (); texturesArrayIterator++)
		delete (*texturesArrayIterator);
	m_texturesArray.clear ();
}


void CScene::postInit(bool a_bNormalize)
{
	cout << "Building VBOs..." << endl;
	for (vector<CObject*>::iterator objectsArrayIterator = m_objectsArray.begin();
	        objectsArrayIterator != m_objectsArray.end();
	        objectsArrayIterator++)
	{
		(*objectsArrayIterator)->buildVBO();
		(*objectsArrayIterator)->buildBoundingSpheres();
	}

	computeBoundingBox(a_bNormalize);

	cout << "Terminé" << endl;
	cout << "*******************************************" << endl;
	cout << "Statistiques sur la scène :" << endl;
	cout << getCObjectsCount() << " objets" << endl;
	cout << getPolygonsCount() << " polygones" << endl;
	cout << getVertexCount() << " vertex" << endl;
	cout << "*******************************************" << endl;
}


void CScene::computeBoundingBox(bool a_bNormalize)
{
	CPoint ptMax(-FLT_MAX, -FLT_MAX, -FLT_MAX), ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
	CPoint objMax, objMin;

	for (vector<CObject*>::iterator objectsArrayIterator = m_objectsArray.begin();
	        objectsArrayIterator != m_objectsArray.end(); objectsArrayIterator++)
	{
		(*objectsArrayIterator)->buildBoundingBox();
		(*objectsArrayIterator)->getBoundingBox(objMax, objMin);
		/* Calcul du max */
		if ( objMax.x > ptMax.x)
			ptMax.x = objMax.x;
		if ( objMax.y > ptMax.y)
			ptMax.y = objMax.y;
		if ( objMax.z > ptMax.z)
			ptMax.z = objMax.z;
		/* Calcul du min */
		if ( objMin.x < ptMin.x)
			ptMin.x = objMin.x;
		if ( objMin.y < ptMin.y)
			ptMin.y = objMin.y;
		if ( objMin.z < ptMin.z)
			ptMin.z = objMin.z;
	}

	if (a_bNormalize)
	{
		/** Normalisation */
		CPoint offset = CPoint(0,0,0) - ((ptMax + ptMin)/2.0f);
		float invNormMax = 1.0f/((ptMax - ptMin).max());

		for (vector<CObject*>::iterator objectsArrayIterator = m_objectsArray.begin();
		        objectsArrayIterator != m_objectsArray.end();
		        objectsArrayIterator++)
		{
			(*objectsArrayIterator)->scale(invNormMax,offset);
		}

		m_max = (ptMax+offset) * invNormMax;
		m_min = (ptMin+offset) * invNormMax;
	}
}


void CScene::computeVisibility(const CCamera &view)
{
	for (vector<CObject*>::iterator objectsArrayIterator = m_objectsArray.begin();
	        objectsArrayIterator != m_objectsArray.end();
	        objectsArrayIterator++)
		(*objectsArrayIterator)->computeVisibility(view);
}


uint CScene::getPolygonsCount()
{
	int nb=0;

	for (vector < CObject * >::iterator objectsArrayIterator = m_objectsArray.begin ();
	        objectsArrayIterator != m_objectsArray.end ();
	        objectsArrayIterator++)
		nb += (*objectsArrayIterator)->getPolygonsCount();

	return nb;
}


uint CScene::getVertexCount()
{
	int nb=0;

	for (vector < CObject * >::iterator objectsArrayIterator = m_objectsArray.begin ();
	        objectsArrayIterator != m_objectsArray.end ();
	        objectsArrayIterator++)
		nb += (*objectsArrayIterator)->getVertexArraySize();

	return nb;
}


int CScene::getMaterialIndexByName(const string& name)
{
	int index=0;
	for (vector<CMaterial*>::iterator materialArrayIterator = m_materialArray.begin ();
	        materialArrayIterator != m_materialArray.end ();
	        materialArrayIterator++)
	{
		if ( !(*materialArrayIterator)->getName().compare (name) )
		{
			return index;
		}

		index++;
	}
	cerr << "Error loading unknown material " << name << endl;
	return getMaterialIndexByName("default");
}


int CScene::searchTextureIndexByName(CharCPtrC name)
{
	int index=0;
	for (vector<CBitmapTexture*>::iterator texturesArrayIterator = m_texturesArray.begin ();
	        texturesArrayIterator != m_texturesArray.end ();
	        texturesArrayIterator++)
	{
		if ( !strcmp((*texturesArrayIterator)->getName(), name) )
		{
			return index;
		}
		index++;
	}
	return -1;
}


void CScene::moveSelectedItem(float x, float y, float z, float oldX, float oldY)
{
	/** Déplacement d'une source si il y a eu sélection */
	GLdouble projMatrix[16], modelMatrix[16];
	GLint viewport[4];
	GLdouble v1[3], v2[3];

	glGetIntegerv(GL_VIEWPORT,viewport);
	glGetDoublev (GL_MODELVIEW_MATRIX,  modelMatrix);
	glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);

	if ( gluUnProject( x, y, z, modelMatrix, projMatrix, viewport, &v1[0], &v1[1], &v1[2]) == GL_FALSE )
		cerr << "gluUnProject failed" << endl;
	if ( gluUnProject( oldX, oldY, z, modelMatrix, projMatrix, viewport, &v2[0], &v2[1], &v2[2]) == GL_FALSE )
		cerr << "gluUnProject failed" << endl;

	m_selectedItem->move(v1[0]-v2[0], v1[1]-v2[1], v1[2]-v2[2]);
}


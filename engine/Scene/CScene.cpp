#include "CScene.hpp"


#include "CMaterial.hpp"
#include "../Utility/UObjImporter.hpp"


uint CScene::glNameCounter = 0;

/*********************************************************************************************************************/
/**		Class CScene          					  	 													   			 */
/*********************************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CScene::CScene() : m_pSelectedItem(NULL)
{
	addMaterial(new CMaterial(rThis));
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CScene::~CScene ()
{
	ForEachIter(itObject, CObjectsVector, m_vpObjects)			delete (*itObject);
	ForEachIter(itLight, CLightsVector, m_vpLights)				delete (*itLight);
	ForEachIter(itMaterial, CMaterialsVector, m_vpMaterials)	delete (*itMaterial);
	ForEachIter(itTexture, CTexturesVector, m_vpTextures) 		delete (*itTexture);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CScene::postInit(bool a_bNormalize)
{
	cout << "Building VBOs..." << endl;
	ForEachIter(itObject, CObjectsVector, m_vpObjects)
	{
		(*itObject)->BuildVBO();
		(*itObject)->BuildBoundingSpheres();
	}

	cout << "Terminé" << endl;
	cout << "*******************************************" << endl;
	cout << "Statistiques sur la scène :" << endl;
	cout << GetObjectsCount() << " objets" << endl;
	cout << GetPolygonsCount() << " polygones" << endl;
	cout << GetVertexCount() << " vertex" << endl;
	cout << "*******************************************" << endl;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
uint CScene::GetPolygonsCount()
{
	int nb=0;

	ForEachIterC(itObject, CObjectsVector, m_vpObjects)
	{
		nb += (*itObject)->GetPolygonsCount();
	}
	return nb;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
uint CScene::GetVertexCount()
{
	int nb=0;

	ForEachIterC(itObject, CObjectsVector, m_vpObjects)
	{
		nb += (*itObject)->GetVertexArraySize();
	}
	return nb;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
int CScene::GetMaterialIndexByName(const string& name)
{
	int index=0;

	ForEachIterC(itMaterial, CMaterialsVector, m_vpMaterials)
	{
		if ( !(*itMaterial)->GetName().compare (name) )
		{
			return index;
		}

		index++;
	}
	cerr << "Error loading unknown material " << name << endl;
	return GetMaterialIndexByName("default");
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
int CScene::searchTextureIndexByName(CharCPtrC name)
{
	int index=0;

	ForEachIterC(itTexture, CTexturesVector, m_vpTextures)
	{
		if ( !strcmp((*itTexture)->GetName(), name) )
		{
			return index;
		}
		index++;
	}
	return -1;
}


//---------------------------------------------------------------------------------------------------------------------
//  TODO
//---------------------------------------------------------------------------------------------------------------------
/*void CScene::moveSelectedItem(float x, float y, float z, float oldX, float oldY)
{
	assert (m_pSelectedItem != NULL);
	if(m_pSelectedItem->HasTransform())
	{
		// Déplacement d'une source si il y a eu sélection
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

		CVector oDir(v1[0]-v2[0], v1[1]-v2[1], v1[2]-v2[2]);

		CTransform &rTransform = *m_pSelectedItem->GetTransform();
		rTransform.Move(oDir);
	}
}*/

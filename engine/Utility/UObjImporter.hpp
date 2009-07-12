#ifndef OBJ_IMPORTER_H
#define OBJ_IMPORTER_H

class CObject;
class CMesh;
class CScene;
class CVector;
class CPoint;
class CRefTable;

#include "../Common.hpp"
#include "../Scene/CScene.hpp"
#include "../Scene/CMaterial.hpp"
#include "../Scene/CSpatialGraph.hpp"
#include "../Utility/CRefTable.hpp"
#include "string.h"

#include <string>
#include <vector>
#include <list>

#include <fstream>
#include <sstream>

class UObjImporter
{
private:
	UObjImporter() {};
public:

	/** Lit un fichier OBJ pass&eacute; en param&egrave;tres et l'importe dans la sc&egrave;ne.
	 * Si object est non nul, les objets seront ne seront pas ajoutés dans la liste des objets de la scène,
	 * mais dans la liste object. Ceci est par exemple utilisé pour les luminaires des flammes qui sont stockés
	 * dans FireSource et non dans CScene. Si objName est non nul, on cherchera à importer seulement les objets
	 * commençant portant ce nom, sinon tous les objets contenu dans le fichier sont importés.
	 *
	 * @param fileName nom du fichier OBJ &agrave; importer.
	 * @param a_rvpObjects Liste d'objets dans laquelle importer le fichier.
	 * @param a_szPrefix Préfixe servant de filtre pour les noms des objets.
	 *
	 * @return false si l'import a échoué.
	 */

	template<class t_Object>
	static bool import(	CScene& a_rScene,
						const string& a_rSceneName,
						CSpatialGraph* const a_pSpatialGraph=NULL,
						CTransform* const a_pTransform=NULL,
						const char* a_szPrefix=NULL)
	{
		bool bSkip = false;
		bool bObjectsAttributesSet=false;
		bool bImportSpecificObjects = (a_szPrefix != NULL);
		bool bAttachTransform = (a_pTransform != NULL);
		if(bAttachTransform)
		{
			assert(a_pSpatialGraph == NULL);
		}
		else
		{
			assert(a_pSpatialGraph != NULL);
		}

		/* Indique qu'un maillage ou un objet a été créé, utile pour ajouter les informations géométriques une fois */
		/* qu'elles sont toutes lues car les objets sont créés auparavant. */
		bool bMeshCreated=false, bObjectCreated=false;
		bool bFirstMesh = false;

		uint a_szPrefixlen=0;
		char lettre,dump;
		string buffer;
		uint coord_textures = 0, normales = 0;
		int nbVertex=0, nbNormals=0, nbTexCoords=0;
		int nbObjectVertex=0, nbObjectNormals=0, nbObjectTexCoords=0, nbObjectGroupVertex=0;
		int matIndex=0;
		int vindices[4], nindices[4], tindices[4];
		uint i=0,j=0;
		float x, y, z;
		string fileName=a_rSceneName;

		/** On lit la ligne en une seule fois pour la parser ensuite */
		std::string ligne;
		std::istringstream flig;

		Vertex currentVertex;
		t_Object* currentObject=NULL;
		CMesh* currentMesh=NULL;
		/** Table utilisée pour noter les références des vertex lors du regroupement */
		CRefTable *refTable=NULL;

		typedef vector <CPoint> CPointVector;
		typedef vector <CVector> CVectorVector;
		typedef vector <GLuint> CUIntVector;

		CPointVector worldPointsVector;
		/** Liste des normales de l'objet */
		CVectorVector normalsVector, worldNormalsVector;
		/** Liste des coordonnées de textures de l'objet */
		CPointVector texCoordsVector, worldTexCoordsVector;
		/** Liste des indices des normales des facettes */
		CUIntVector normalsIndexVector;
		/** Liste des indices des coordonnées de textures des facettes */
		CUIntVector texCoordsIndexVector;


		if (bImportSpecificObjects) a_szPrefixlen = strlen(a_szPrefix);
		GetSceneAbsolutePath(fileName);

		ifstream objFile(fileName.c_str(), ios::in);
		if (!objFile.is_open ())
		{
			cerr << "Can't open file " << fileName << endl;
			throw (ios::failure ("Open scene error"));
			return false;
		}

		while (!objFile.eof())
		{
			objFile >> lettre;
			if (objFile.eof()) break;
			switch (lettre)
			{
				default:
					getline(objFile,buffer);
					break;
				case 'o':
					/* Définition d'un nouvel objet. */
					/* Un nouveau matériau est appliqué, nous devons donc créer un nouveau CMesh. */
					/* Cependant, on commence d'abord par valider les données du CMesh précédent. */
					if (bMeshCreated)
					{
						/* On valide les données du dernier CMesh. */
						nbObjectGroupVertex+=setUVsAndNormals(	*currentMesh, *currentObject, *refTable,
																normalsVector, normalsIndexVector,
																texCoordsVector, texCoordsIndexVector);
						normalsIndexVector.clear();
						texCoordsIndexVector.clear();
						bMeshCreated = false;
					}

					objFile >> buffer;

					if (bImportSpecificObjects)
					{
						/* On recherche un objet en particulier. */
						if (!buffer.compare(0,a_szPrefixlen,a_szPrefix))
						{
							/* Objet trouvé ! */
							CTransform *pTransform = NULL;
							if(bAttachTransform)
							{
								pTransform = a_pTransform;
							}
							else
							{
								pTransform = new CTransform();
								a_pSpatialGraph->AddTransform(pTransform);
							}
							currentObject = new t_Object(a_rScene, *pTransform);
							a_rScene.addObject(currentObject);
							bObjectCreated = true;
							bFirstMesh = true;
							bSkip = false;
						}
						else
							bSkip = true;
					}
					else
					{
						/* Sinon on prend tous les objets dans le fichier. */
						CTransform *pTransform = NULL;
						if(bAttachTransform)
						{
							pTransform = a_pTransform;
						}
						else
						{
							pTransform = new CTransform();
							a_pSpatialGraph->AddTransform(pTransform);
						}
						currentObject = new t_Object(a_rScene, *pTransform);
						a_rScene.addObject(currentObject);
						bObjectCreated = true;
						bFirstMesh = true;
					}

					normalsVector.clear();
					texCoordsVector.clear();
					nbObjectGroupVertex=0;
					nbObjectVertex = nbVertex;
					nbObjectNormals = nbNormals;
					nbObjectTexCoords = nbTexCoords;
					break;

				case 'g':
					// Si on cherche des objets spécifiques on ne traite pas les groupes.
					assert(bImportSpecificObjects == false);
					/* Définition d'un nouvel objet faisant parti d'un groupe. */
					/* Un nouveau matériau est appliqué, nous devons donc créer un nouveau CMesh. */
					/* Cependant, on commence d'abord par valider les données du CMesh précédent. */
					if (bMeshCreated)
					{
						/* On valide les données du dernier CMesh. */
						nbObjectGroupVertex+=setUVsAndNormals(	*currentMesh, *currentObject, *refTable,
																normalsVector, normalsIndexVector,
																texCoordsVector, texCoordsIndexVector);
						normalsIndexVector.clear();
						texCoordsIndexVector.clear();
						bMeshCreated = false;
					}
					objFile >> buffer;

					if (!bObjectCreated)
					{
						/* On prend tous les objets dans le fichier. */
						CTransform *pTransform = NULL;
						if(bAttachTransform)
						{
							pTransform = a_pTransform;
						}
						else
						{
							pTransform = new CTransform();
							a_pSpatialGraph->AddTransform(pTransform);
						}
						currentObject = new t_Object(a_rScene, *pTransform);
						a_rScene.addObject(currentObject);
						bObjectCreated = true;
						bFirstMesh = true;
					}
					break;
				case 'u':
					/* Un nouveau matériau est appliqué, nous devons donc créer un nouveau CMesh. */
					/* Cependant, on commence d'abord par valider les données du CMesh précédent. */
					if (bMeshCreated)
					{
						/* On valide les données du dernier CMesh. */
						setUVsAndNormals(*currentMesh, *currentObject, *refTable,
						                 normalsVector, normalsIndexVector, texCoordsVector, texCoordsIndexVector);
						normalsIndexVector.clear();
						texCoordsIndexVector.clear();
						bFirstMesh = false;
					}

					/* Création du nouveau mesh. */
					objFile >> buffer >> buffer;
					if (!bSkip)
					{
						matIndex = a_rScene.GetMaterialIndexByName(buffer);
						assert(currentObject != NULL);
						currentMesh = new CMesh(a_rScene, matIndex, *currentObject);
						currentObject->AddMesh(currentMesh);
						bMeshCreated = true;
						bObjectsAttributesSet = false;
					}
					break;
				case 'm':
					/* Définition des matériaux */
					objFile >> buffer >> buffer;
					/* La définition des matériaux est évitée si l'on importe qu'un seul objet. */
					if (!bImportSpecificObjects)
						importMTL (a_rScene, buffer);
					break;
				case 'v':
					objFile.get(lettre);
					switch (lettre)
					{
						default:
							break;
						case ' ':
							objFile >> currentVertex.x >> currentVertex.y >> currentVertex.z;
							if (!bSkip)
								currentObject->addVertex(currentVertex);
							worldPointsVector.push_back(CPoint(x,y,z));
							nbVertex++;
							break;
						case 'n':
							objFile >> x >> y >> z;
							if (!bSkip)
								normalsVector.push_back(CVector(x, y, z));
							worldNormalsVector.push_back(CVector(x, y, z));
							nbNormals++;
							break;
						case 't':
							objFile >> x >> y;
							if (!bSkip)
								/* On inverse la coordonnée y */
								texCoordsVector.push_back(CPoint(x, y, 0));
							worldTexCoordsVector.push_back(CPoint(x, y, 0));
							nbTexCoords++;
							break;
					}
					break;
				case 'f':
					if (bSkip)
					{
						getline(objFile,buffer);
						break;
					}
					/** Pour prendre en compte les fichiers véreux ou usemtl n'est pas écrit */
					if (!bMeshCreated && !bSkip)
					{
						matIndex = a_rScene.GetMaterialIndexByName("default");
						assert(currentObject != NULL);
						currentMesh = new CMesh(a_rScene, matIndex, *currentObject);
						currentObject->AddMesh(currentMesh);
						bMeshCreated = true;
						bObjectsAttributesSet = false;
					}

					getline(objFile, ligne);
					flig.str(ligne);

					/** On regarde si les attributs de l'objet ont été affectés ou non */
					/** Ceci est en pratique réalisé sur la première face puisqu'on ne */
					/** peut pas le déterminer avant */
					if (!bObjectsAttributesSet)
					{
						i=j=0;
						bObjectsAttributesSet = true;

						GetObjectAttributesSet(ligne,normales,coord_textures);

						currentMesh->setAttributes(coord_textures + normales);
						if (bFirstMesh)
						{
							/* On initialise la table de réf */
							if (refTable)
								delete refTable;
							refTable = new CRefTable(currentObject->GetVertexArraySize());
						}
					}

					i=0;
					switch (coord_textures + normales)
					{
						case 0:
							while (flig >> vindices[i])
							{
								if (vindices[i] > nbObjectVertex)
									currentMesh->addIndex(vindices[i] - nbObjectVertex - 1 + nbObjectGroupVertex );
								else
									cerr << "(EE) point index " << vindices[i] << " " << nbObjectVertex << endl;
								i++;
							}
							assert(i<4);
							break;

						case 1:
							while (flig >> vindices[i] >> dump >> dump >> nindices[i])
							{
								if (vindices[i] > nbObjectVertex)
									currentMesh->addIndex(vindices[i] - nbObjectVertex - 1 + nbObjectGroupVertex );
								else
									cerr << "(EE) point index " << vindices[i] << " " << nbObjectVertex << endl;

								if (nindices[i] > nbObjectNormals)
									normalsIndexVector.push_back(nindices[i] - nbObjectNormals - 1);
								else
								{
									normalsVector.push_back( worldNormalsVector[nindices[i]-1] );
									normalsIndexVector.push_back(normalsVector.size()-1);
								}
								i++;
							}
							assert(i<4);
							break;

						case 2:
							while (flig >> vindices[i] >> dump >> nindices[i])
							{
								if (vindices[i] > nbObjectVertex)
									currentMesh->addIndex(vindices[i] - nbObjectVertex - 1 + nbObjectGroupVertex );
								else
									cerr << "(EE) point index " << vindices[i] << " " << nbObjectVertex << endl;

								if (nindices[i] > nbObjectNormals)
									normalsIndexVector.push_back(nindices[i] - nbObjectNormals - 1);
								else
								{
									normalsVector.push_back( worldNormalsVector[nindices[i]-1] );
									normalsIndexVector.push_back(normalsVector.size()-1);
								}
								i++;
							}
							assert(i<4);
							break;

						case 3:
							while (flig >> vindices[i] >> dump >> tindices[i] >> dump >> nindices[i])
							{
								if (vindices[i] > nbObjectVertex)
									currentMesh->addIndex(vindices[i] - nbObjectVertex - 1 + nbObjectGroupVertex );
								else
									cerr << "(EE) point index " << vindices[i] << " " << nbObjectVertex << endl;

								if (nindices[i] > nbObjectNormals)
									normalsIndexVector.push_back(nindices[i] - nbObjectNormals - 1);
								else
								{
									normalsVector.push_back( worldNormalsVector[nindices[i]-1] );
									normalsIndexVector.push_back(normalsVector.size()-1);
								}
								if (tindices[i] > nbObjectTexCoords)
									texCoordsIndexVector.push_back(tindices[i] - nbObjectTexCoords - 1);
								else
								{
									texCoordsVector.push_back( worldTexCoordsVector[tindices[i]-1] );
									texCoordsIndexVector.push_back(texCoordsVector.size()-1);
								}
								i++;
							}
							assert(i<4);
							break;

						default:
							cerr << "Erreur de chargement : Le fichier " << fileName << " contient des erreurs d'indexation de points.\n";
							return false;
							break;
					}
					/** Important, ne pas oublier de nettoyer l'input stream !!! */
					flig.clear();
					break;
				case '\n':
				case ' ':
					break;
			}
		}
		objFile.close ();
		if (bMeshCreated)
		{
			/* On valide les données du dernier CMesh. */
			nbObjectGroupVertex+=setUVsAndNormals(	*currentMesh,
													*currentObject,
													*refTable,
													normalsVector,
													normalsIndexVector,
													texCoordsVector,
													texCoordsIndexVector);
			normalsIndexVector.clear();
			texCoordsIndexVector.clear();
		}

		return (currentObject != NULL);
	}
	/** Lit un fichier OBJ pass&eacute; en param&egrave;tre et cherche le nom du fichier MTL.
	 *
	 * @param fileName Nom du fichier OBJ à lire.
	 * @param mtlName Nom du fichier récupéré, la chaîne doit être allouée au préalable.
	 *
	 * @return true si trouve un fichier MTL
	 */
	static bool GetMTLFileNameFromOBJ(const string& fileName, string& mtlName);

	/** Lit un fichier MTL pass&eacute; en param&egrave;tres et importe les matériaux qu'il contient
	 * dans la scène.
	 *
	 * @param fileName nom du fichier OBJ &agrave; importer.
	 */
	static void importMTL(CScene& a_rScene, const string& fileName);

private:

	/** Fonctions retournant le chemin absolu d'un fichier par rapport au chemin courant.
	 * @param fileName Nom du fichier à traiter.
	 */
	static void GetSceneAbsolutePath(string& a_rSceneName);

	/** Détermine les attributs fixés pour un objet pendant l'import */
	static void GetObjectAttributesSet(const string& str, uint& normals, uint& texCoords);

	/** Fusion des trois tableaux en un seul tableau.
	 * Le format OBJ gère trois tableaux d'indices, alors qu'en OpenGL il n'y a qu'un seul tableau d'indice.
	 * L'algorithme présent dans cette fonction réalise la fusion des ces trois tableaux en un seul.<br>
	 * Le principe est le suivant : pour chaque indice du tableau de points, on regarde si il a déjà été référencé
	 * auparavant dans le tableau. Si c'est le cas, alors on regarde si ses normales et ses coordonnées de texture
	 * (pas les indices mais les coordonnées réelles, car certains exportateurs en OBJ ont tendance à dupliquer les
	 * normales et les coordonnées de textures) sont égales à celle de la première référence. Dans l'affirmative, il n'y
	 * a rien à faire, l'indice du point est correct. Dans le cas contraire, il est nécessaire de dupliquer le point dans
	 * le tableau de point de l'objet, d'affecter les coordonnées de texture et la normale, et enfin de modifier l'indice
	 * du point courant en conséquence. Enfin dans le cas où le point n'a pas encore été référencé dans le tableau, on
	 * mémorise cette référence dans une table utilisée pour le premier cas, et ensuite on affecte les coordonnées
	 * de texture et la normale.
	 *
	 * @param normalsVector Vecteur des normales.
	 * @param normalsIndexVector Vecteur des indices des normales.
	 * @param texCoordsVector Vecteur des coordonnées de texture.
	 * @param texCoordsIndexVector Vecteur des indices des coordonnées de texture.
	 * @return Nombre de vertices dupliqués.
	 */
	static uint setUVsAndNormals(CMesh& mesh, CObject& parentObject, CRefTable& refTable,
	                             const vector < CVector > &normalsVector, const vector < GLuint > &normalsIndexVector,
	                             const vector < CPoint >  &texCoordsVector, const vector < GLuint > &texCoordsIndexVector);

	/** Chaîne de caractère contenant le chemin courant. Elle est utilisée dans les fonctions d'import pour
	 * parcourir les différents répertoires (scenes, textures, ...).
	 */
	static string m_currentDir;
};

#endif

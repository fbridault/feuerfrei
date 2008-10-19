#include "CObjImporter.hpp"

#include "../Scene/CScene.hpp"
#include "../Scene/CMaterial.hpp"
#include "../Utility/CRefTable.hpp"
#include <string.h>

#include <fstream>
#include <sstream>

string ObjImporter::m_currentDir;

bool ObjImporter::import(CScene* const scene, const string& sceneName, vector <CObject*> &objectsList, const char* prefix)
{
	bool skip = false;
	bool objectsAttributesSet=false;
	bool lookForSpecificCObjects = (prefix != NULL);
	uint prefixlen=0;
	/* Indique qu'un maillage ou un objet a été créé, utile pour ajouter les informations géométriques une fois */
	/* qu'elles sont toutes lues car les objets sont crées auparavant. */
	bool meshCreated=false, objectCreated=false;
	bool firstCMesh = false;
	char lettre,dump;
	string buffer;
	uint coord_textures = 0, normales = 0;
	int nbVertex=0, nbNormals=0, nbTexCoords=0;
	int nbCObjectVertex=0, nbCObjectNormals=0, nbCObjectTexCoords=0, nbCObjectGroupVertex=0;
	int matIndex=0;
	int vindices[4], nindices[4], tindices[4];
	uint i=0,j=0;
	float x, y, z;
	string fileName=sceneName;

	/** On lit la ligne en une seule fois pour la parser ensuite */
	std::string ligne;
	std::istringstream flig;

	Vertex currentVertex;
	CObject* currentCObject=NULL;
	CMesh* currentCMesh=NULL;
	/** Table utilisée pour noter les références des vertex lors du regroupement */
	CRefTable *refTable=NULL;


	vector < CPoint >worldPointsVector;
	/** Liste des normales de l'objet */
	vector < CVector >normalsVector, worldNormalsVector;
	/** Liste des coordonnées de textures de l'objet */
	vector < CPoint  >texCoordsVector, worldTexCoordsVector;
	/** Liste des indices des normales des facettes */
	vector < GLuint >normalsIndexVector;
	/** Liste des indices des coordonnées de textures des facettes */
	vector < GLuint >texCoordsIndexVector;


	if (lookForSpecificCObjects) prefixlen = strlen(prefix);
	getSceneAbsolutePath(fileName);

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
				if (meshCreated)
				{
					/* On valide les données du dernier CMesh. */
					nbCObjectGroupVertex+=setUVsAndNormals(*currentCMesh, *currentCObject, *refTable,
					                                      normalsVector, normalsIndexVector, texCoordsVector, texCoordsIndexVector);
					normalsIndexVector.clear();
					texCoordsIndexVector.clear();
					meshCreated = false;
				}

				objFile >> buffer;

				if (lookForSpecificCObjects)
				{
					/* On recherche un objet en particulier. */
					if (!buffer.compare(0,prefixlen,prefix))
					{
						/* Objet trouvé ! */
						currentCObject = new CObject(scene);
						objectsList.push_back(currentCObject);
						objectCreated = true;
						firstCMesh = true;
						skip = false;
					}
					else
						skip = true;
				}
				else
				{
					/* Sinon on prend tous les objets dans le fichier. */
					currentCObject = new CObject(scene);
					objectsList.push_back(currentCObject);
					objectCreated = true;
					firstCMesh = true;
				}

				normalsVector.clear();
				texCoordsVector.clear();
				nbCObjectGroupVertex=0;
				nbCObjectVertex = nbVertex;
				nbCObjectNormals = nbNormals;
				nbCObjectTexCoords = nbTexCoords;
				break;

			case 'g':
				/* Définition d'un nouvel objet faisant parti d'un groupe. */
				/* Un nouveau matériau est appliqué, nous devons donc créer un nouveau CMesh. */
				/* Cependant, on commence d'abord par valider les données du CMesh précédent. */
				if (meshCreated)
				{
					/* On valide les données du dernier CMesh. */
					nbCObjectGroupVertex+=setUVsAndNormals(*currentCMesh, *currentCObject, *refTable,
					                                      normalsVector, normalsIndexVector, texCoordsVector, texCoordsIndexVector);
					normalsIndexVector.clear();
					texCoordsIndexVector.clear();
					meshCreated = false;
				}
				objFile >> buffer;

				if (!objectCreated)
				{
					/* On prend tous les objets dans le fichier. */
					currentCObject = new CObject(scene);
					objectsList.push_back(currentCObject);
					objectCreated = true;
					firstCMesh = true;
				}
				break;
			case 'u':
				/* Un nouveau matériau est appliqué, nous devons donc créer un nouveau CMesh. */
				/* Cependant, on commence d'abord par valider les données du CMesh précédent. */
				if (meshCreated)
				{
					/* On valide les données du dernier CMesh. */
					setUVsAndNormals(*currentCMesh, *currentCObject, *refTable,
					                 normalsVector, normalsIndexVector, texCoordsVector, texCoordsIndexVector);
					normalsIndexVector.clear();
					texCoordsIndexVector.clear();
					firstCMesh = false;
				}

				/* Création du nouveau mesh. */
				objFile >> buffer >> buffer;
				if (!skip)
				{
					matIndex = scene->getMaterialIndexByName(buffer);
					currentCMesh = new CMesh(scene, matIndex, currentCObject);
					currentCObject->addCMesh(currentCMesh);
					meshCreated = true;
					objectsAttributesSet = false;
				}
				break;
			case 'm':
				/* Définition des matériaux */
				objFile >> buffer >> buffer;
				/* La définition des matériaux est évitée si l'on importe qu'un seul objet. */
				if (!lookForSpecificCObjects)
					importMTL (scene, buffer);
				break;
			case 'v':
				objFile.get(lettre);
				switch (lettre)
				{
					default:
						break;
					case ' ':
						objFile >> currentVertex.x >> currentVertex.y >> currentVertex.z;
						if (!skip)
							currentCObject->addVertex(currentVertex);
						worldPointsVector.push_back(CPoint(x,y,z));
						nbVertex++;
						break;
					case 'n':
						objFile >> x >> y >> z;
						if (!skip)
							normalsVector.push_back(CVector(x, y, z));
						worldNormalsVector.push_back(CVector(x, y, z));
						nbNormals++;
						break;
					case 't':
						objFile >> x >> y;
						if (!skip)
							/* On inverse la coordonnée y */
							texCoordsVector.push_back(CPoint(x, -y, 0));
						worldTexCoordsVector.push_back(CPoint(x, -y, 0));
						nbTexCoords++;
						break;
				}
				break;
			case 'f':
				if (skip)
				{
					getline(objFile,buffer);
					break;
				}
				/** Pour prendre en compte les fichiers véreux ou usemtl n'est pas écrit */
				if (!meshCreated && !skip)
				{
					matIndex = scene->getMaterialIndexByName("default");
					currentCMesh = new CMesh(scene, matIndex, currentCObject);
					currentCObject->addCMesh(currentCMesh);
					meshCreated = true;
					objectsAttributesSet = false;
				}

				getline(objFile, ligne);
				flig.str(ligne);

				/** On regarde si les attributs de l'objet ont été affectés ou non */
				/** Ceci est en pratique réalisé sur la première face puisqu'on ne */
				/** peut pas le déterminer avant */
				if (!objectsAttributesSet)
				{
					i=j=0;
					objectsAttributesSet = true;

					getObjectAttributesSet(ligne,normales,coord_textures);

					currentCMesh->setAttributes(coord_textures + normales);
					if (firstCMesh)
					{
						/* On initialise la table de réf */
						if (refTable)
							delete refTable;
						refTable = new CRefTable(currentCObject->getVertexArraySize());
					}
				}

				i=0;
				switch (coord_textures + normales)
				{
					case 0:
						while (flig >> vindices[i])
						{
							if (vindices[i] > nbCObjectVertex)
								currentCMesh->addIndex(vindices[i] - nbCObjectVertex - 1 + nbCObjectGroupVertex );
							else
								cerr << "(EE) point index " << vindices[i] << " " << nbCObjectVertex << endl;
							i++;
						}
						assert(i<4);
						break;

					case 1:
						while (flig >> vindices[i] >> dump >> dump >> nindices[i])
						{
							if (vindices[i] > nbCObjectVertex)
								currentCMesh->addIndex(vindices[i] - nbCObjectVertex - 1 + nbCObjectGroupVertex );
							else
								cerr << "(EE) point index " << vindices[i] << " " << nbCObjectVertex << endl;

							if (nindices[i] > nbCObjectNormals)
								normalsIndexVector.push_back(nindices[i] - nbCObjectNormals - 1);
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
							if (vindices[i] > nbCObjectVertex)
								currentCMesh->addIndex(vindices[i] - nbCObjectVertex - 1 + nbCObjectGroupVertex );
							else
								cerr << "(EE) point index " << vindices[i] << " " << nbCObjectVertex << endl;

							if (nindices[i] > nbCObjectNormals)
								normalsIndexVector.push_back(nindices[i] - nbCObjectNormals - 1);
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
							if (vindices[i] > nbCObjectVertex)
								currentCMesh->addIndex(vindices[i] - nbCObjectVertex - 1 + nbCObjectGroupVertex );
							else
								cerr << "(EE) point index " << vindices[i] << " " << nbCObjectVertex << endl;

							if (nindices[i] > nbCObjectNormals)
								normalsIndexVector.push_back(nindices[i] - nbCObjectNormals - 1);
							else
							{
								normalsVector.push_back( worldNormalsVector[nindices[i]-1] );
								normalsIndexVector.push_back(normalsVector.size()-1);
							}
							if (tindices[i] > nbCObjectTexCoords)
								texCoordsIndexVector.push_back(tindices[i] - nbCObjectTexCoords - 1);
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
	if (meshCreated)
	{
		/* On valide les données du dernier CMesh. */
		nbCObjectGroupVertex+=setUVsAndNormals(*currentCMesh, *currentCObject, *refTable,
		                                      normalsVector, normalsIndexVector, texCoordsVector, texCoordsIndexVector);
		normalsIndexVector.clear();
		texCoordsIndexVector.clear();
	}

	return (objectsList.size() > 0);
}


void ObjImporter::getObjectAttributesSet(const string& str, uint& normals, uint& texCoords)
{
	uint i=0,j=0;

	/** On regarde le nombre de slashs pour déterminer les attributs */
	while ( (i=str.find("//",i+2)) != string::npos ) j++;
	if (j==3)
	{
		normals = 1;
		texCoords = 0;
	}
	else
	{
		i=j=0;
		while ( (i=str.find("/",i+1)) != string::npos ) j++;
		if (j==3)
		{
			normals = 1;
			texCoords = 0;
		}
		else
			if (j==6)
			{
				normals = 1;
				texCoords = 2;
			}
			else
			{
				normals = 0;
				texCoords = 0;
			}
	}
}


bool ObjImporter::getMTLFileNameFromOBJ(const string& sceneName, string& mtlName)
{
	char lettre;
	string buffer;
	string fileName=sceneName;

	getSceneAbsolutePath(fileName);
	ifstream objFile(fileName.c_str(), ios::in);
	if (!objFile.is_open ())
	{
		cerr << "Can't open file " << fileName << endl;
		throw (ios::failure ("Open error"));
		return false;
	}
	while (!objFile.eof())
	{
		objFile >> lettre;
		switch (lettre)
		{
			case 'm':
				/* On évite la définition des matériaux, elle doit être faite au prélable */
				objFile >> buffer >> mtlName;
				objFile.close ();
				return true;
				break;
			default:
				getline(objFile,buffer);
				break;
		}
	}
	objFile.close ();

	return false;
}


void ObjImporter::importMTL(CScene* const scene, const string& fileName)
{
	char lettre, lettre2;
	string buffer;
	string texturePath;
	int nouvelle_texture = -1;
	string name_nouvelle_matiere;
	bool newMat=false;

	buffer.append(m_currentDir);
	buffer.append(fileName);

	ifstream matFile(buffer.c_str(), ios::in);
	if (!matFile.is_open ())
	{
		cerr << "Can't open file " << fileName << endl;
		throw (ios::failure ("Open error"));
		return;
	}

	float Kd[3], Ka[3], Ks[3], alpha, shini;

	/* Problème: selon l'exportateur, les champs ne sont pas écrits dans le même ordre */
	/* On est donc sûr d'avoir lu tous les champs d'un matériau que lorsque l'on arrive */
	/* au matériau suivant ou à la fin du fichier/ */
	while (!matFile.eof())
	{
		matFile >> lettre;

		switch (lettre)
		{
			case 'K':
				matFile >> lettre2;

				switch (lettre2)
				{
					case 'd':
						{
							float R, G, B;
							matFile >> R >> G >> B;
							Kd[0] = R;
							Kd[1] = G;
							Kd[2] = B;
						}
						break;
					case 'a':
						{
							float R, G, B;
							matFile >> R >> G >> B;
							Ka[0] = R;
							Ka[1] = G;
							Ka[2] = B;
						}
						break;
					case 's':
						{
							float R, G, B;
							matFile >> R >> G >> B;
							Ks[0] = R;
							Ks[1] = G;
							Ks[2] = B;
						}
						break;
					default:
						break;
				}
				break;
			case 'N':
				matFile >> lettre;
				switch (lettre)
				{
					case 's':
						matFile >> shini;
						shini = (shini / 1000) * 128;
						break;
					default:
						break;
				}
				break;
			case 'd':
				matFile >> alpha;
				break;
			case 'n':
				/* On ajoute donc d'abord le matériau précédemment trouvé */
				if (newMat)
				{
					scene->addMaterial(new CMaterial (scene, name_nouvelle_matiere, Ka, Kd, Ks, shini, nouvelle_texture)); //,alpha);
					nouvelle_texture = -1;
					newMat=false;
				}
				Ks[0] = Ks[1] = Ks[2] = Kd[0] = Kd[1] = Kd[2] = Ka[0] = Ka[1] = Ka[2] = 0.0;
				alpha = 1.0;
				shini = 0.0;
				matFile >> buffer >> buffer;
				name_nouvelle_matiere = buffer;
				newMat = true;
				break;
			case 'i':		// ignoré pour le moment
				break;
			case 'm':		//map_K?
				matFile >> buffer >> buffer;
				texturePath.clear();
				texturePath.append(m_currentDir);
				texturePath.append(buffer);

				if ( (nouvelle_texture = scene->searchTextureIndexByName(texturePath)) == -1)
				{
					nouvelle_texture = scene->addTexture(new CBitmapTexture (texturePath));
				}
				break;
			default:
				getline(matFile,buffer);
				break;
		}
	}
	/* On ajoute enfin le dernier matériau trouvé */
	if (newMat)
	{
		scene->addMaterial(new CMaterial (scene, name_nouvelle_matiere, Ka, Kd, Ks, shini, nouvelle_texture)); //,alpha);
		nouvelle_texture = -1;
		newMat=false;
	}
	matFile.close();
}


void ObjImporter::getSceneAbsolutePath(string& fileName)
{
	uint i = fileName.find_last_of('/');

	if ( i != string::npos )
		m_currentDir = fileName.substr(0,i+1);
	else
		cerr << "(EE) Error: could not find absolute path name" << endl;
}


uint ObjImporter::setUVsAndNormals(CMesh& mesh, CObject& parentObject, CRefTable& refTable,
                                   const vector < CVector > &normalsVector, const vector < GLuint > &normalsIndexVector,
                                   const vector < CPoint >  &texCoordsVector, const vector < GLuint > &texCoordsIndexVector)
{
	Vertex v;
	CVector normal, texCoord;
	uint dup=0,nondup=0;
	bool adup;

//  cerr << " Over " << mesh.getIndexesCount() << " vertices, ";
	if (!mesh.getAttributes())
		for (uint i = 0; i < mesh.getIndexesCount(); i++)
			parentObject.setVertex( mesh.getIndex(i), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	else
		for (uint i = 0; i < mesh.getIndexesCount(); i++)
		{
			normal = normalsVector[normalsIndexVector[i]];
			if (mesh.getAttributes() == 3)
				texCoord = texCoordsVector[texCoordsIndexVector[i]];

			if ( refTable.findRef( mesh.getIndex(i) ) )
			{
				/* Le point courant a déjà été référencé auparavant dans le tableau d'indices */
				v = parentObject.getVertex(mesh.getIndex(i));
				if ( ((float)texCoord.x) == v.u && ((float)texCoord.y) == v.v &&
				     ((float)normal.x) == v.nx && ((float)normal.y) == v.ny && ((float)normal.z) == v.nz )
				{
					/* La normale et les coordonnées de texture du point courant sont identiques à la précédente */
					/* référence, il n'y a donc rien à faire */
					nondup++;
				}
				else
				{
					adup=true;
					/* On regarde si le point a été dupliqué auparavant */
					if ( refTable.hasDupRefs( mesh.getIndex(i) ) )
					{
						list<int> refList = refTable.getDupRefs( mesh.getIndex(i) );

						for (list<int>::iterator it = refList.begin(); it!=refList.end(); it++)
						{
							v = parentObject.getVertex( *it );
							if ( ((float)texCoord.x) == v.u && ((float)texCoord.y) == v.v &&
							     ((float)normal.x) == v.nx && ((float)normal.y) == v.ny && ((float)normal.z) == v.nz ){
								nondup++;
								/* Le point courant a déjà été référencé auparavant dans les vertices dupliqués */
								mesh.setIndex(i, *it );
								adup=false;
								break;
							}
						}
					}

					if (adup)
					{
						parentObject.addVertex( v );
						//cerr << v.x << " " << v.y << " " << v.z << " " << v.nx << " " << v.ny << " " << v.nz << endl;
						/* On mémorise la duplication du vertex */
						refTable.addDupRef( mesh.getIndex(i), parentObject.getVertexArraySize()-1 );
						/* Le nouveau point est placé en dernier, on récupère son index et on le stocke */
						mesh.setIndex(i,parentObject.getVertexArraySize()-1);
						/* On affecte les coordonnées de texture et de normale au point courant */
						parentObject.setVertex( mesh.getIndex(i), texCoord.x, texCoord.y, normal.x, normal.y, normal.z);
						dup++;
					}
				}
			}
			else
			{
				/* On affecte les coordonnées de texture et de normale au point courant */
				parentObject.setVertex( mesh.getIndex(i), texCoord.x, texCoord.y, normal.x, normal.y, normal.z);
				refTable.addRef( mesh.getIndex(i));
			}
		}
	return dup;
//   cerr << dup << " have been duplicated, " << nondup << " untouched" << endl;
}

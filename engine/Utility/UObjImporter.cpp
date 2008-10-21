#include "UObjImporter.hpp"

#include <string.h>


string UObjImporter::m_currentDir;

void UObjImporter::getObjectAttributesSet(const string& str, uint& normals, uint& texCoords)
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


bool UObjImporter::getMTLFileNameFromOBJ(const string& sceneName, string& mtlName)
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


void UObjImporter::importMTL(CScene* const scene, const string& fileName)
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


void UObjImporter::getSceneAbsolutePath(string& fileName)
{
	uint i = fileName.find_last_of('/');

	if ( i != string::npos )
		m_currentDir = fileName.substr(0,i+1);
	else
		cerr << "(EE) Error: could not find absolute path name" << endl;
}


uint UObjImporter::setUVsAndNormals(CMesh& mesh, CObject& parentObject, CRefTable& refTable,
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
							     ((float)normal.x) == v.nx && ((float)normal.y) == v.ny && ((float)normal.z) == v.nz )
							{
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

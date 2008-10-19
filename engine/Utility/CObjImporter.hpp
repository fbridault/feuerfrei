#ifndef OBJ_IMPORTER_H
#define OBJ_IMPORTER_H

class CObject;
class CMesh;
class CScene;
class CVector;
class CPoint;
class CRefTable;

#include "../Common.hpp"

#include <string>
#include <vector>
#include <list>

using namespace std;

class ObjImporter
{
private:
	ObjImporter() {};
public:

	/** Lit un fichier OBJ pass&eacute; en param&egrave;tres et l'importe dans la sc&egrave;ne.
	 * Si object est non nul, les objets seront ne seront pas ajoutés dans la liste des objets de la scène,
	 * mais dans la liste object. Ceci est par exemple utilisé pour les luminaires des flammes qui sont stockés
	 * dans FireSource et non dans CScene. Si objName est non nul, on cherchera à importer seulement les objets
	 * commençant portant ce nom, sinon tous les objets contenu dans le fichier sont importés.
	 *
	 * @param fileName nom du fichier OBJ &agrave; importer.
	 * @param objectsList Liste d'objets dans laquelle importer le fichier.
	 * @param wicksList Optionnel, liste de mèches dans laquelle importer le fichier.
	 * @param prefix Préfixe servant de filtre pour les noms des objets.
	 *
	 * @return false si l'import a échoué.
	 */
	static bool import(CScene* const scene, const string& sceneName, vector<CObject*> &objectsList, const char* prefix=NULL);

private:

	/** Lit un fichier OBJ pass&eacute; en param&egrave;tre et cherche le nom du fichier MTL.
	 *
	 * @param fileName Nom du fichier OBJ à lire.
	 * @param mtlName Nom du fichier récupéré, la chaîne doit être allouée au préalable.
	 *
	 * @return true si trouve un fichier MTL
	 */
	static bool getMTLFileNameFromOBJ(const string& fileName, string& mtlName);

	/** Lit un fichier MTL pass&eacute; en param&egrave;tres et importe les matériaux qu'il contient
	 * dans la scène.
	 *
	 * @param fileName nom du fichier OBJ &agrave; importer.
	 */
	static void importMTL(CScene* const scene, const string& fileName);

	/** Fonctions retournant le chemin absolu d'un fichier par rapport au chemin courant.
	 * @param fileName Nom du fichier à traiter.
	 */
	static void getSceneAbsolutePath(string& sceneName);

	/** Détermine les attributs fixés pour un objet pendant l'import */
	static void getObjectAttributesSet(const string& str, uint& normals, uint& texCoords);

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

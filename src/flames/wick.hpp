#ifndef WICK_H
#define WICK_H
/***************************************************************************
 *            CWick.hpp
 *
 *  Tue Mar 29 16:23:15 2005
 *  Copyright  2005  Flavien Bridault
 *  bridault@lil.univ-littoral.fr
 ****************************************************************************/
class CWick;

#include <engine/Scene/CObject.hpp>
#include "leadSkeleton.hpp"

#define WICK_NAME_PREFIX "Wick"

class Solveur;
class CScene;

/** Classe représentant une mèche longiligne de flamme.<br>
 * Le constructeur prend en entrée un fichier OBJ. L'objet est découpé en nb_lead_skeletons
 * partitions. Une racine de squelette périphérique est placée au centre de chaque partition.
 * Deux racines sont également ajoutées aux extrémités de la mèche.<br>
 * L'ensemble de ces points est stocké dans la variable membre m_leadCPointsArray et sera utilisé
 * par la flamme pour générer tous ses squelettes.
 */
class CWick : public CObject
{
private:
	/**<Liste des points qui vont servir à  créer les squelettes guides */
	GLuint m_wickDisplayList;
	GLuint m_boxesDisplayList;

public:
	/** Constructeur de mèche.
	 * @param scene CPointeur sur la scène.
	 */
	CWick(CScene& a_rScene) : CObject(a_rScene) {};

	virtual ~CWick();

	/* Construction d'une mèche ponctuelle (bougie)
	 * @param scene CPointeur sur la scène.
	 * @param position Position de la mèche dans l'espace.
	 * @param leadSkeletons Vecteur des squelettes guides.
	 */
	uint buildPointFDF(FlameConfig const& flameConfig, vector< LeadSkeleton * >& leadSkeletons, Field3D* const field);

	/* Construction de la mèche
	 * @param scene CPointeur sur la scène.
	 * @param position Position de la mèche dans l'espace.
	 * @param leadSkeletons Vecteur des squelettes guides.
	 */
	uint buildFDF(FlameConfig const& flameConfig, vector< LeadSkeleton * >& leadSkeletons, Field3D* const field);

//   void buildFDF(Field3D* const m_field);
	/** Affiche la mèche
	 * @param displayBoxes Affiche ou non le partitionnement de la mèche.
	 */
	virtual void drawWick(bool displayBoxes) const
	{
		draw(ALL,true);
		if (displayBoxes)
			glCallList(m_boxesDisplayList);
	};

	bool checkPointsInVoxel(const CPoint& h, uint i, uint j, uint k)
	{
		for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
		     vertexIterator != m_vertexArray.end (); vertexIterator++)
			if ( h.x*(i-1) < vertexIterator->x && vertexIterator->x < h.x*i &&
			     h.y*(j-1) < vertexIterator->y && vertexIterator->y < h.y*j &&
			     h.z*(k-1) < vertexIterator->z && vertexIterator->z < h.z*k )
				return true;
		return false;
	};

};

#endif

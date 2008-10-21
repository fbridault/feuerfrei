#ifndef REFTABLE_H
#define REFTABLE_H

#include <list>

/** Table de référence permettant, lors de la reconstruction des index des normales et des coordonnées de texture
 * d'un mesh, de stocker les références faite à un indice d'un point. Seule une référence est stockée pour chaque point
 * identique. Ce point est ensuite utilisé pour comparaison.
 */
class CRefTable
{
public:
	CRefTable(uint size) : newRefsIndex(size)
	{
		m_lookupTable.assign(size,false);
		m_dupRefs.resize(size);
	};

	void reinit()
	{
		m_lookupTable.assign(m_lookupTable.size(),false);
	};

	/** Ajout d'un indice dans la table de hachage.
	 * @param i indice du point dans le tableau de points.
	 * @param ref indice de la référence du point dans le tableau d'indice du mesh courant.
	 */
	void addRef(uint i)
	{
		assert( i < m_lookupTable.size());
		m_lookupTable[i] = true;
	};

	/** Ajout d'un indice dans la table de hachage.
	 * @param i indice du point dans le tableau de points.
	 * @param ref indice de la référence du point dans le tableau d'indice du mesh courant.
	 */
	void addDupRef(uint i, uint ref)
	{
		assert( i < m_dupRefs.size());
		m_dupRefs[i].push_back(ref);
	};

	/** Recherche d'un indice dans la table de hachage. *
	 * @param i indice recherché
	 */
	bool findRef(uint i) const
	{
		return (m_lookupTable[i]);
	}

	bool hasDupRefs(uint i) const
	{
		assert( i < m_dupRefs.size());
		return !m_dupRefs[i].empty();
	}

	const list<int>& getDupRefs(uint i)
	{
		assert( i < m_dupRefs.size());
		return m_dupRefs[i];
	}

private:
	/** Contient les indices des points dans le tableau d'indice du mesh courant */
	vector<bool> m_lookupTable;
	/** Contient les indices des points dupliqués dans le tableau de vertices */
	vector< list<int> > m_dupRefs;

	uint newRefsIndex;
};
#endif

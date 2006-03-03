#ifndef WICK_H
#define WICK_H
/***************************************************************************
 *            Wick.hpp
 *
 *  Tue Mar 29 16:23:15 2005
 *  Copyright  2005  Flavien Bridault
 *  bridault@lil.univ-littoral.fr
 ****************************************************************************/
class Wick;

#include "../scene/material.hpp"
#include "../scene/object.hpp"

class CObject;
class Solveur;
class CScene;

/** Classe représentant une mèche longiligne de flamme.<br>
 * Le constructeur prend en entrée un fichier OBJ. L'objet est découpé en nb_lead_skeletons
 * partitions. Une racine de squelette périphérique est placée au centre de chaque partition.
 * Deux racines sont également ajoutées aux extrémités de la mèche.<br>
 * L'ensemble de ces points est stocké dans la variable membre m_leadPointsArray est sera utilisé
 * par la flamme pour générer tous ses squelettes.
 */
class Wick : public CObject
{
private:
  /**<Liste des points qui vont servir à  créer les squelettes guides */
  vector < CPoint * >m_leadPointsArray;
  GLuint m_wickDisplayList;
  
public:
  /** Constructeur de mèche	
   * @param filename nom du fichier de scène où est stockée la mèche
   */
  Wick(const char *filename, int nb_lead_squelettes, CScene *scene, CPoint& offset);
  virtual ~Wick();
  	
  virtual void drawWick()
  {
    glCallList(m_wickDisplayList);
  };
  /**
   * Lecture du nombre de points des squelettes guides.
   */
  virtual int getLeadPointsArraySize () const
  {
    return m_leadPointsArray.size ();
  };
  /**
   * Lecture d'un point sp&eacute;cifique des squelettes guides.
   * @param index indice du point &agrave; obtenir.
   * @return Un pointeur vers le point recherch&eacute;.
   */
  virtual const CPoint *getLeadPoint(int index) const
  {
    return (m_leadPointsArray[index]);
  };
  virtual vector < CPoint * > *getLeadPointsArray ()
  {
    return ( &m_leadPointsArray );
  };
};

#endif

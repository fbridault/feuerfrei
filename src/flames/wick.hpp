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

class Object;
class Solveur;
class Scene;

class WickPoint
{
public:
  WickPoint(Point& pt, double u) : m_pt(pt), m_u(u) {};
  virtual ~WickPoint() {};
  Point m_pt;
  double m_u;
};

/** Classe représentant une mèche longiligne de flamme.<br>
 * Le constructeur prend en entrée un fichier OBJ. L'objet est découpé en nb_lead_skeletons
 * partitions. Une racine de squelette périphérique est placée au centre de chaque partition.
 * Deux racines sont également ajoutées aux extrémités de la mèche.<br>
 * L'ensemble de ces points est stocké dans la variable membre m_leadPointsArray est sera utilisé
 * par la flamme pour générer tous ses squelettes.
 */
class Wick : public Object
{
private:
  /**<Liste des points qui vont servir à  créer les squelettes guides */
  vector < WickPoint * >m_leadPointsArray;
  GLuint m_wickDisplayList;
  GLuint m_boxesDisplayList;
  
public:
  /** Constructeur de mèche	
   * @param filename nom du fichier de scène où est stockée la mèche
   */
  Wick(const char *wickFileName, int nb_lead_squelettes, Scene *scene, Point& offset, const char*wickName=NULL);
  virtual ~Wick();
  	
  /** Affiche la mèche
   * @param displayBoxes affiche ou non le partitionnement de la mèche
   */
  virtual void drawWick(bool displayBoxes)
  {
    glCallList(m_wickDisplayList);
    if(displayBoxes)
      glCallList(m_boxesDisplayList);
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
  virtual const WickPoint *getLeadPoint(int index) const
  {
    return (m_leadPointsArray[index]);
  };
  virtual vector < WickPoint * > *getLeadPointsArray ()
  {
    return ( &m_leadPointsArray );
  };
};

#endif

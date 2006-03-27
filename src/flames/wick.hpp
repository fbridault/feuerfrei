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

/** Classe repr�sentant une m�che longiligne de flamme.<br>
 * Le constructeur prend en entr�e un fichier OBJ. L'objet est d�coup� en nb_lead_skeletons
 * partitions. Une racine de squelette p�riph�rique est plac�e au centre de chaque partition.
 * Deux racines sont �galement ajout�es aux extr�mit�s de la m�che.<br>
 * L'ensemble de ces points est stock� dans la variable membre m_leadPointsArray est sera utilis�
 * par la flamme pour g�n�rer tous ses squelettes.
 */
class Wick : public Object
{
private:
  /**<Liste des points qui vont servir � cr�er les squelettes guides */
  vector < WickPoint * >m_leadPointsArray;
  GLuint m_wickDisplayList;
  GLuint m_boxesDisplayList;
  
public:
  /** Constructeur de m�che	
   * @param filename nom du fichier de sc�ne o� est stock�e la m�che
   */
  Wick(const char *wickFileName, int nb_lead_squelettes, Scene *scene, Point& offset, const char*wickName=NULL);
  virtual ~Wick();
  	
  /** Affiche la m�che
   * @param displayBoxes affiche ou non le partitionnement de la m�che
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

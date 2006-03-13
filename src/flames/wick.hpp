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
  vector < Point * >m_leadPointsArray;
  GLuint m_wickDisplayList;
  
public:
  /** Constructeur de m�che	
   * @param filename nom du fichier de sc�ne o� est stock�e la m�che
   */
  Wick(const char *wickFileName, int nb_lead_squelettes, Scene *scene, Point& offset, const char*wickName=NULL);
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
  virtual const Point *getLeadPoint(int index) const
  {
    return (m_leadPointsArray[index]);
  };
  virtual vector < Point * > *getLeadPointsArray ()
  {
    return ( &m_leadPointsArray );
  };
};

#endif

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

/** Classe repr�sentant un point d'origine d'un squelette guide sur une m�che. Elle est donc
 * compos�e de la position du point dans l'espace et d'une valeur u qui est utilis�e pour
 * valeur d'entr�e de la fonction de distribution de carburant F(u).
 */
class WickPoint
{
public:
  /** Constructeur par d�faut.
   * @param pt Position dans l'espace.
   * @param u Valeur u.
   */
  WickPoint(Point& pt, double u) : m_pt(pt), m_u(u) {};
  
  /** Destructeur */
  virtual ~WickPoint() {};
  
  /** Position dans l'espace */
  Point m_pt;
  /** Valeur d'entr�e de la fonction de distribution de carburant F(u) */
  double m_u;
};

/** Classe repr�sentant une m�che longiligne de flamme.<br>
 * Le constructeur prend en entr�e un fichier OBJ. L'objet est d�coup� en nb_lead_skeletons
 * partitions. Une racine de squelette p�riph�rique est plac�e au centre de chaque partition.
 * Deux racines sont �galement ajout�es aux extr�mit�s de la m�che.<br>
 * L'ensemble de ces points est stock� dans la variable membre m_leadPointsArray et sera utilis�
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
  /** Constructeur de m�che.
   * @param wickFileName nom du fichier de sc�ne o� est stock�e la m�che.
   * @param nb_lead_squelettes Nombre de squelettes guides � placer sur la m�che.
   * @param scene Pointeur sur la sc�ne.
   * @param position Position de la m�che dans l'espace.
   * @param wickName Optionnel, nom de l'objet dans le fichier OBJ.
   */
  Wick(const char *wickFileName, int nb_lead_squelettes, Scene *scene, Point& position, const char*wickName=NULL);
  virtual ~Wick();
  	
  /** Affiche la m�che
   * @param displayBoxes Affiche ou non le partitionnement de la m�che.
   */
  virtual void drawWick(bool displayBoxes)
  {
    glCallList(m_wickDisplayList);
    if(displayBoxes)
      glCallList(m_boxesDisplayList);
  };
  
  /** Lecture du nombre de points des squelettes guides.
   * @return Nombre de points
   */
  virtual int getLeadPointsArraySize () const
  {
    return m_leadPointsArray.size ();
  };
  
  /** Lecture d'un point sp&eacute;cifique des squelettes guides.
   * @param index indice du point &agrave; obtenir.
   * @return Un pointeur vers le point recherch&eacute;.
   */
  virtual const WickPoint *getLeadPoint(int index) const
  {
    return (m_leadPointsArray[index]);
  };
  
  /** Lecture du tableau des squelettes guides.
   * @return Un pointeur vers le tableau recherch&eacute;.
   */
  virtual vector < WickPoint * > *getLeadPointsArray ()
  {
    return ( &m_leadPointsArray );
  };
};

#endif

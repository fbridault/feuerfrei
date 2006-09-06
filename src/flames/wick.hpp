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

/** Classe représentant un point d'origine d'un squelette guide sur une mèche. Elle est donc
 * composée de la position du point dans l'espace et d'une valeur u qui est utilisée pour
 * valeur d'entrée de la fonction de distribution de carburant F(u).
 */
class WickPoint
{
public:
  /** Constructeur par défaut.
   * @param pt Position dans l'espace.
   * @param u Valeur u.
   */
  WickPoint(Point& pt, double u) : m_pt(pt), m_u(u) {};
  
  /** Destructeur */
  virtual ~WickPoint() {};
  
  /** Position dans l'espace */
  Point m_pt;
  /** Valeur d'entrée de la fonction de distribution de carburant F(u) */
  double m_u;
};

/** Classe représentant une mèche longiligne de flamme.<br>
 * Le constructeur prend en entrée un fichier OBJ. L'objet est découpé en nb_lead_skeletons
 * partitions. Une racine de squelette périphérique est placée au centre de chaque partition.
 * Deux racines sont également ajoutées aux extrémités de la mèche.<br>
 * L'ensemble de ces points est stocké dans la variable membre m_leadPointsArray et sera utilisé
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
  /** Constructeur de mèche.
   * @param wickFileName nom du fichier de scène où est stockée la mèche.
   * @param nb_lead_squelettes Nombre de squelettes guides à placer sur la mèche.
   * @param scene Pointeur sur la scène.
   * @param position Position de la mèche dans l'espace.
   * @param wickName Optionnel, nom de l'objet dans le fichier OBJ.
   */
  Wick(const char *wickFileName, int nb_lead_squelettes, Scene *scene, Point& position, const char*wickName=NULL);
  virtual ~Wick();
  	
  /** Affiche la mèche
   * @param displayBoxes Affiche ou non le partitionnement de la mèche.
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

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

#include "../scene/object.hpp"
#include "leadSkeleton.hpp"

class Object;
class Solveur;
class Scene;

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
  GLuint m_wickDisplayList;
  GLuint m_boxesDisplayList;
  
public:
  /** Constructeur de m�che.
   * @param scene Pointeur sur la sc�ne.
   */
  Wick(Scene* const scene);
  virtual ~Wick();
  
  /* Construction d'une m�che ponctuelle (bougie)
   * @param scene Pointeur sur la sc�ne.
   * @param position Position de la m�che dans l'espace.
   * @param leadSkeletons Vecteur des squelettes guides.
   */
  uint buildPointFDF(const FlameConfig& flameConfig, vector< LeadSkeleton * >& leadSkeletons, Field3D* const field);
  
  /* Construction de la m�che
   * @param scene Pointeur sur la sc�ne.
   * @param position Position de la m�che dans l'espace.
   * @param leadSkeletons Vecteur des squelettes guides.
   */
  uint buildFDF(const FlameConfig& flameConfig, vector< LeadSkeleton * >& leadSkeletons, Field3D* const field);
  
  /** Affiche la m�che
   * @param displayBoxes Affiche ou non le partitionnement de la m�che.
   */
  virtual void drawWick(bool displayBoxes) const
  {
    draw(ALL,true);
    if(displayBoxes)
      glCallList(m_boxesDisplayList);
  };

};

#endif

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
   * @param wickFileName nom du fichier de sc�ne o� est stock�e la m�che.
   * @param nb_lead_squelettes Nombre de squelettes guides � placer sur la m�che.
   * @param scene Pointeur sur la sc�ne.
   * @param position Position de la m�che dans l'espace.
   * @param leadSkeletons Vecteur des squelettes guides.
   * @param wickName Optionnel, nom de l'objet dans le fichier OBJ.
   */
  Wick(const char *wickFileName, FlameConfig* flameConfig, Scene *scene, 
       vector< LeadSkeleton * >& leadSkeletons, Field3D *solver, const char*wickName=NULL);
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
};

#endif

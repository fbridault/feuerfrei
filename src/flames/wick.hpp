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
  GLuint m_wickDisplayList;
  GLuint m_boxesDisplayList;
  
public:
  /** Constructeur de mèche.
   * @param scene Pointeur sur la scène.
   */
  Wick(Scene* const scene);
  virtual ~Wick();
  
  /* Construction d'une mèche ponctuelle (bougie)
   * @param scene Pointeur sur la scène.
   * @param position Position de la mèche dans l'espace.
   * @param leadSkeletons Vecteur des squelettes guides.
   */
  uint buildPointFDF(const FlameConfig& flameConfig, vector< LeadSkeleton * >& leadSkeletons, Field3D* const field);
  
  /* Construction de la mèche
   * @param scene Pointeur sur la scène.
   * @param position Position de la mèche dans l'espace.
   * @param leadSkeletons Vecteur des squelettes guides.
   */
  uint buildFDF(const FlameConfig& flameConfig, vector< LeadSkeleton * >& leadSkeletons, Field3D* const field);
  
  /** Affiche la mèche
   * @param displayBoxes Affiche ou non le partitionnement de la mèche.
   */
  virtual void drawWick(bool displayBoxes) const
  {
    draw(ALL,true);
    if(displayBoxes)
      glCallList(m_boxesDisplayList);
  };

};

#endif

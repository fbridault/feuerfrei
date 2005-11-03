#if !defined(IES_H)
#define IES_H

#include "header.h"
#include "texture.hpp"

#include <vector>

/** Classe permettant de charger et d'exploiter un solide photométrique 
 * stocké sous la forme d'un fichier IES 
 */
class IES
{
public:
  /** Constructeur par défaut
   * @param filename Nom du fichier IES à charger
   */
  IES(const char* const filename);

  /** Affiche les valeurs de luminances du solide */
  void test();

  /** Retourne le nombre de valeurs en zénithal */
  const int getNbzenith() const {return nbzenith;};

  /** Retourne le nombre de valeurs en azimuthal */
  const int getNbazimut() const {return nbazimut;};

  /** Retourne le nombre d'intensités total (zénithal*azimuthal) */
  const float* getIntensites() const {return intensites;};
  
  
  const float getLazimut() const {return lazimut;};

  const float getLzenith() const {return lzenith;};

  const float getLazimutTEX() const {return lazimutTEX;};

  const float getLzenithTEX() const {return lzenithTEX;};

  const float getDenom() const {return denom;};

  /** Retourne une texture correspondant au fichier IES */
  Texture* getTexture() const {return texture;};

private:
  /** Nombre de valeurs en zénithal et en azimuthal */
  int nbzenith,nbazimut;
  /** Tableau contenant les intensités lumineuses */
  float *intensites;
  
  float lazimut,lazimutTEX,lzenith,lzenithTEX,denom;
  /** Pointeur sur la texture correspondant au fichier IES */
  Texture *texture;
};

/** Classe permettant de gérer une liste de fichiers IES.<br>
 * Utilisée par la classe SolidePhotometrique pour gérer plusieurs solides à la fois.
 * Elle propose de sélectionner un seul fichier IES à la fois et ensuite de faire "tourner"
 * les fichiers à l'aide de la méthode swap(). Un pointeur sur le fichier actuellement 
 * sélectionné peut être récupéré à l'aide de getCurrentIESfile().
 */
class IESList
{
public:
  /** Constructeur par défaut */
  IESList()
  {
    currentIndex=0;
  };
  
  /** Ajout d'un fichier IES dans la liste
   * @param filename Nom du fichier IES à charger
   */
  void addIESFile (const char* const filename){
    IESArray.push_back(new IES(filename));
  };
  
  /** Récupération d'un fichier IES dans la liste
   * @param n Indice du fichier IES
   * @return Pointeur vers le fichier IES
   */
  const IES* getIESfile (int n) const{
    return IESArray[n];
  };
  
  /** Récupération du fichier IES actuellement sélectionné dans la liste
   * @return Pointeur vers le fichier IES
   */
  const IES* getCurrentIESfile () const{
    return IESArray[currentIndex];
  };
  
  /** Récupération de la taille de la liste
   * @return Taille
   */
  const int getArraySize () const
  {
    return IESArray.size ();
  };
  
  /** Méthode permettant de sélectionner le fichier IES suivant dans la liste */
  void swap(void)
  {
    if(currentIndex == IESArray.size () - 1 )
      currentIndex=0;
    else
      currentIndex++;
  };
  
private:
  /** Vecteur contenant les fichiers IES */
  vector < IES *>IESArray;
  /** Indice du fichier actuellement sélectionné */
  unsigned int currentIndex;
};

#endif

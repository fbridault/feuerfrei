#if !defined(IES_H)
#define IES_H

#include "header.h"
#include "texture.hpp"

#include <vector>

/** Classe permettant de charger et d'exploiter un solide photom�trique 
 * stock� sous la forme d'un fichier IES 
 */
class IES
{
public:
  /** Constructeur par d�faut
   * @param filename Nom du fichier IES � charger
   */
  IES(const char* const filename);

  /** Affiche les valeurs de luminances du solide */
  void test();

  /** Retourne le nombre de valeurs en z�nithal */
  const int getNbzenith() const {return nbzenith;};

  /** Retourne le nombre de valeurs en azimuthal */
  const int getNbazimut() const {return nbazimut;};

  /** Retourne le nombre d'intensit�s total (z�nithal*azimuthal) */
  const float* getIntensites() const {return intensites;};
  
  
  const float getLazimut() const {return lazimut;};

  const float getLzenith() const {return lzenith;};

  const float getLazimutTEX() const {return lazimutTEX;};

  const float getLzenithTEX() const {return lzenithTEX;};

  const float getDenom() const {return denom;};

  /** Retourne une texture correspondant au fichier IES */
  Texture* getTexture() const {return texture;};

private:
  /** Nombre de valeurs en z�nithal et en azimuthal */
  int nbzenith,nbazimut;
  /** Tableau contenant les intensit�s lumineuses */
  float *intensites;
  
  float lazimut,lazimutTEX,lzenith,lzenithTEX,denom;
  /** Pointeur sur la texture correspondant au fichier IES */
  Texture *texture;
};

/** Classe permettant de g�rer une liste de fichiers IES.<br>
 * Utilis�e par la classe SolidePhotometrique pour g�rer plusieurs solides � la fois.
 * Elle propose de s�lectionner un seul fichier IES � la fois et ensuite de faire "tourner"
 * les fichiers � l'aide de la m�thode swap(). Un pointeur sur le fichier actuellement 
 * s�lectionn� peut �tre r�cup�r� � l'aide de getCurrentIESfile().
 */
class IESList
{
public:
  /** Constructeur par d�faut */
  IESList()
  {
    currentIndex=0;
  };
  
  /** Ajout d'un fichier IES dans la liste
   * @param filename Nom du fichier IES � charger
   */
  void addIESFile (const char* const filename){
    IESArray.push_back(new IES(filename));
  };
  
  /** R�cup�ration d'un fichier IES dans la liste
   * @param n Indice du fichier IES
   * @return Pointeur vers le fichier IES
   */
  const IES* getIESfile (int n) const{
    return IESArray[n];
  };
  
  /** R�cup�ration du fichier IES actuellement s�lectionn� dans la liste
   * @return Pointeur vers le fichier IES
   */
  const IES* getCurrentIESfile () const{
    return IESArray[currentIndex];
  };
  
  /** R�cup�ration de la taille de la liste
   * @return Taille
   */
  const int getArraySize () const
  {
    return IESArray.size ();
  };
  
  /** M�thode permettant de s�lectionner le fichier IES suivant dans la liste */
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
  /** Indice du fichier actuellement s�lectionn� */
  unsigned int currentIndex;
};

#endif

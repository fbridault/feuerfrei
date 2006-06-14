#if !defined(IES_H)
#define IES_H

#include "flames.hpp"
#include "../scene/texture.hpp"

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
  virtual ~IES();
  /** Affiche les valeurs de luminances du solide */
  void test();
  
  /** Retourne le nombre de valeurs en z�nithal */
  const int getNbzenith() const {return m_nbzenith;};
  
  /** Retourne le nombre de valeurs en azimuthal */
  const int getNbazimut() const {return m_nbazimut;};
  
  /** Retourne le nombre d'intensit�s total (z�nithal*azimuthal) */
  const float* getIntensites() const {return m_intensites;};
  
  const double getLazimut() const {return m_lazimut;};
  
  const double getLzenith() const {return m_lzenith;};
  
  const double getLazimutTEX() const {return m_lazimutTEX;};
  
  const double getLzenithTEX() const {return m_lzenithTEX;};
  
  const double getDenom() const {return m_denom;};
  
  /** Retourne une texture correspondant au fichier IES */
  Texture* getTexture() const {return m_texture;};
  
private:
  /** Nombre de valeurs en z�nithal et en azimuthal */
  int m_nbzenith,m_nbazimut;
  /** Tableau contenant les intensit�s lumineuses */
  float *m_intensites;
  
  double m_lazimut,m_lazimutTEX,m_lzenith,m_lzenithTEX,m_denom;
  /** Pointeur sur la texture correspondant au fichier IES */
  Texture *m_texture;
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
    m_currentIndex=0;
  };
  
  ~IESList();
  /** Ajout d'un fichier IES dans la liste
   * @param filename Nom du fichier IES � charger
   */
  void addIESFile (const char* const filename){
    m_IESArray.push_back(new IES(filename));
  };
  
  /** R�cup�ration d'un fichier IES dans la liste
   * @param n Indice du fichier IES
   * @return Pointeur vers le fichier IES
   */
  const IES* getIESfile (int n) const{
    return m_IESArray[n];
  };
  
  /** R�cup�ration du fichier IES actuellement s�lectionn� dans la liste
   * @return Pointeur vers le fichier IES
   */
  const IES* getCurrentIESfile () const{
    return m_IESArray[m_currentIndex];
  };
  
  /** R�cup�ration de la taille de la liste
   * @return Taille
   */
  const int getArraySize () const
  {
    return m_IESArray.size ();
  };
  
  /** M�thode permettant de s�lectionner le fichier IES suivant dans la liste */
  void swap(void)
  {
    if(m_currentIndex == m_IESArray.size () - 1 )
      m_currentIndex=0;
    else
      m_currentIndex++;
  };
  
private:
  /** Vecteur contenant les fichiers IES */
  vector < IES *>m_IESArray;
  /** Indice du fichier actuellement s�lectionn� */
  uint m_currentIndex;
};

#endif

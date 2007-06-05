#if !defined(IES_H)
#define IES_H

#include "flames.hpp"

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
  const uint getNbzenith() const {return m_nbzenith;};
  
  /** Retourne le nombre de valeurs en azimuthal */
  const uint getNbazimut() const {return m_nbazimut;};
  
  /** Retourne le nombre d'intensit�s total (z�nithal*azimuthal) */
  float* getIntensities() const {return m_intensites;};
  
  const float getLazimut() const {return m_lazimut;};
  
  const float getLzenith() const {return m_lzenith;};
  
  const float getLazimutTEX() const {return m_lazimutTEX;};
  
  const float getLzenithTEX() const {return m_lzenithTEX;};
  
  const float getDenom() const {return m_denom;};
    
private:
  /** Nombre de valeurs en z�nithal et en azimuthal */
  uint m_nbzenith,m_nbazimut;
  /** Tableau contenant les intensit�s lumineuses */
  float *m_intensites;
  
  /** Intervalle en valeur d'angle entre chaque valeur sur l'azimuth */
  float m_lazimut,m_lazimutTEX;
  /** Intervalle en valeur d'angle entre chaque valeur sur le zenith */
  float m_lzenith,m_lzenithTEX;
  float m_denom;
};

#endif

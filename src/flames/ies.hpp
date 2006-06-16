#if !defined(IES_H)
#define IES_H

#include "flames.hpp"

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
  virtual ~IES();
  /** Affiche les valeurs de luminances du solide */
  void test();
  
  /** Retourne le nombre de valeurs en zénithal */
  const uint getNbzenith() const {return m_nbzenith;};
  
  /** Retourne le nombre de valeurs en azimuthal */
  const uint getNbazimut() const {return m_nbazimut;};
  
  /** Retourne le nombre d'intensités total (zénithal*azimuthal) */
  float* getIntensities() const {return m_intensites;};
  
  const double getLazimut() const {return m_lazimut;};
  
  const double getLzenith() const {return m_lzenith;};
  
  const double getLazimutTEX() const {return m_lazimutTEX;};
  
  const double getLzenithTEX() const {return m_lzenithTEX;};
  
  const double getDenom() const {return m_denom;};
    
private:
  /** Nombre de valeurs en zénithal et en azimuthal */
  uint m_nbzenith,m_nbazimut;
  /** Tableau contenant les intensités lumineuses */
  float *m_intensites;
  
  /** Intervalle en valeur d'angle entre chaque valeur sur l'azimuth */
  double m_lazimut,m_lazimutTEX;
  /** Intervalle en valeur d'angle entre chaque valeur sur le zenith */
  double m_lzenith,m_lzenithTEX;
  double m_denom;
};

#endif

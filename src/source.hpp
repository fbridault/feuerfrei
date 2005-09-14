/* Source.h: interface for the Source class. */

#ifndef SOURCE_H
#define SOURCE_H

class CSource;

#include "intensity.hpp"
#include "vector.hpp"

#include <vector>

using namespace std;

typedef enum
{ POINTLIGHT, SPOTLIGHT, DIRECTIONNALLIGHT, AMBIENTLIGHT } lightCategoryType;

/** 
 * Classe de base repr&eacute;sentant une source lumineuse. 
 * Une source lumineuse peut &ecirc;tre de cinq types diff&eacute;rents (pointLight, areaLight, etc.) et 
 * chaque type poss&egrave;de ses attributs propres pour d&eacute;finir compl&egrave;tement une source.
 *
 * @author	Christophe Cassagnab&egrave;re modifi&eacute; par Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 * @see CMaterial, CIntensity
 */
class CSource
{
private:
  CIntensity lightIntensity;
  CPoint *position;
  CVector *direction;

  double aperture;
  lightCategoryType lightCategory;
  int sampling;

public:
  /**
   * Constructeur par d&eacute;faut.
   * Par d&eacute;faut la source cr&eacute;e est de type pointLight et n'a pas de g&eacute;om&eacute;trie associ&eacute;e.
   */
    CSource (const CIntensity & I)
  {
    lightIntensity = I;
    lightCategory = POINTLIGHT;
    position = NULL;
    direction = NULL;
  };
  /**
   * Constructeur param&eacute;trique pour une source de type pointLight.
   * @param P position de la source.
   * @param I intensit&eacute; lumineuse de la source.
   */
  CSource (const CPoint & P, const CIntensity & I)
  {
    lightIntensity = I;
    lightCategory = POINTLIGHT;
    position = new CPoint (P);
    direction = NULL;
  };

  /**
   * Destructeur par d&eacute;faut.
   */
  ~CSource ()
  {
    if (position != NULL)
      delete[]position;
    if (direction != NULL)
      delete[]direction;
  };


  /**
   * Op&eacute;rateur d'affectation.
   */
  CSource & operator= (const CSource & aSource);

  /**
   * Lecture de l'intensit&eacute; lumineuse de la source.
   */
  const CIntensity & getPureIntensity () const
  {
    return lightIntensity;
  };

};
#endif

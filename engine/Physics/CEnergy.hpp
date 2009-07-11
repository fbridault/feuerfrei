#ifndef ENERGY_H
#define ENERGY_H

class CEnergy;

#include <stdlib.h>

#define COMPOSANTES             4
#define RED                     0
#define GREEN                   1
#define BLUE                    2
#define ALPHA                   3

/**
 * Classe pour la représentation d'une intensité lumineuse.
 * Une intensité est définie par un ensemble de valeurs
 * d'intensités pour certaines longueurs d'ondes particulières.
 *
 * @author	Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 */
class CEnergy
{
private:
  float color[COMPOSANTES];
  /**< Tableaux d'intensité. Chaque longueur d'onde
   * a une intensité lumineuse propre de type réelle.
   * Une intensité lumineuse est la combinaison de ces
   * longueur d'ondes. Le système se base pour l'instant
   * sur le code RVB.*/

public:
  /**
   * Constructeur par défaut. Crée une intensité
   * lumineuse nulle.
   */
  CEnergy (){
    for (int i=0;i<COMPOSANTES; i++)
      color[i]=0.0;
  }
  /**
   * Constructeur par recopie.
   */
  CEnergy (const CEnergy& I){
    for (int i=0; i<COMPOSANTES; i++)
      color[i]=I.color[i];
  }
  /**
   * Constructeur paramétrique.
   * @param c pointeur vers un tableau de <CODE>COMPOSANTES</CODE>
   * éléments de type <CODE>float</CODE>
   */
  CEnergy (float r, float g, float b){
    color[RED] = r;
    color[GREEN] = g;
    color[BLUE] = b;
    color[ALPHA] = 1.0;
  }
  /**
   * Constructeur paramétrique.
   * @param c pointeur vers un tableau de <CODE>COMPOSANTES</CODE>
   * éléments de type <CODE>float</CODE>
   */
  CEnergy (float c[COMPOSANTES]){
    for (int i=0; i<COMPOSANTES; i++)
      color[i]=c[i];
  }
  /**
   * Destructeur par défaut.
   */
  ~CEnergy(){

  };

  /**
   * Opérateur d'addition d'intensité lumineuses.
   * Chacunes des composantes de chaque intensité sont
   * additionnées une à une.
   * Seules les intensités supérieures à 0
   * sont prises en compte.
   */
  CEnergy operator+(const CEnergy& I) const
  {
    float result[COMPOSANTES];
    for (int i=0;i<COMPOSANTES;i++)
      result[i]= color[i] + I.color[i];
    CEnergy CEnergy(result);
    return CEnergy;
  }
  /**
   * Opérateur de quotient par un scalaire. Chacune des composantes
   * de l'intensité est divisée upar un même scalaire.
   * Seules les intensités supérieures à 0 sont prises
   * en compte.
   */
  CEnergy operator/(const float& K) const
  {
    float result[COMPOSANTES];
    for (int i=0;i<COMPOSANTES;i++)
      result[i]=color[i]/K;
    CEnergy CEnergy(result);
    return CEnergy;
  }
  /**
   * Opérateur de produit par un scalaire. Chacune des composantes
   * de l'intensité est multipliée upar un même scalaire.
   * Seules les intensités supérieures à 0 sont prises
   * en compte.
   */
  CEnergy operator*(const float& K) const
  {
    float result[COMPOSANTES];
    for (int i=0;i<COMPOSANTES;i++)
      result[i]=color[i]*K;
    CEnergy CEnergy(result);
    return CEnergy;
  }

  /**
   * Opérateur de produit combinatoire d'intensités lumineuses.
   * Chacunes des composantes de chaque intensité sont
   * multipliées une à une.
   * Seules les intensités supérieures à 0 sont prises
   * en compte.
   */
  CEnergy operator*(const CEnergy& I) const
  {
    float result[COMPOSANTES];
    for (int i=0;i<COMPOSANTES;i++)
      result[i]=color[i]*I.color[i];
    CEnergy CEnergy(result);
    return CEnergy;
  }

  float& operator[](int i)
  {
    return color[i];
  }

  float operator[](int i) const
  {
    return color[i];
  }

  /**
   * Opérateur d'égalité.
   */
  bool operator==(const CEnergy& I) const
  {
    for (int i=0;i<COMPOSANTES;i++)
      if (color[i]!=I.color[i])
        return false;
    return true;
  }
  /**
   * Opérateur d'addition d'intensité lumineuses.
   * Chacunes des composantes de chaque intensité sont
   * additionnées une à une.
   * Cette fonction <B>modifie</B> l'opérande de gauche
   * par affectation.
   * Seules les intensités supérieures à 0 sont prises en compte.
   */
  CEnergy& operator+=(const CEnergy& I){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]= color[i] + I.color[i];
    return *this;
  }

  CEnergy& operator-=(const CEnergy& I){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]= color[i] - I.color[i];
    return *this;
  }
  /**
   * Opérateur de quotient par un scalaire. Chacune des composantes
   * de l'intensité est divisée upar un même scalaire.
   * Cette fonction <B>modifie</B> l'opérande de gauche par affectation.
   * Seules les intensités supérieures à 0 sont prises en
   * compte.
   */
  CEnergy& operator/=(const float& K){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=color[i]/K;
    return *this;
  }
  /**
   * Opérateur de produit par un scalaire. Chacune des composantes
   * de l'intensité est multipliée upar un même scalaire.
   * Cette fonction <B>modifie</B> l'opérande de gauche par affectation.
   * Seules les intensités supérieures à 0 sont prises en
   * compte.
   */
  CEnergy& operator*=(const float& K){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=color[i]*K;
    return *this;
  }
  /**
   * Opérateur de produit combinatoire d'intensités lumineuses.
   * Chacunes des composantes de chaque intensité sont
   * multipliées une à une.
   * Cette fonction <B>modifie</B> l'opérande de gauche par affectation.
   * Seules les intensités supérieures à 0 sont prises
   * en compte.
   */
  CEnergy& operator*=(const CEnergy& I){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=color[i]*I.color[i];
    return *this;
  }

  /**
   * Opérateur d'allocation.
   */
  CEnergy& operator=(const CEnergy& I) {
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=I.color[i];
    return *this;
  };

  const float* GetColors() const {
    return color;
  };

  void GetColor(float &r, float &g, float &b) const {
    r = color[RED];
    g = color[GREEN];
    b = color[BLUE];
  };

  int Energy2ColorRVB(){
    float red = color[RED];
    float green = color[GREEN];
    float blue= color[BLUE];
    return (
             ((int) ((red   <1.0?red   :1.0)*255) << 16) |
             ((int) ((green <1.0?green :1.0)*255) << 8)  |
             (int) ((blue  <1.0?blue  :1.0)*255)
           );
  }

	//-----------------------------------------------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------------------------------------------
	void randomize( float min, float max )
	{
		float range  = max - min;
		color[RED]   = (rand()/(float)RAND_MAX) * range + min;
		color[GREEN] = (rand()/(float)RAND_MAX) * range + min;
		color[BLUE]  = (rand()/(float)RAND_MAX) * range + min;
	}

	//-----------------------------------------------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------------------------------------------
	float max() const
	{
		if (color[RED]>color[GREEN]){
			if (color[RED]>color[BLUE])
				return(color[RED]);
		}else
			if (color[GREEN]>color[BLUE])
				return(color[GREEN]);
		return(color[BLUE]);
	}
};//class CEnergy

#endif

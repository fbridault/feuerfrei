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
 * Classe pour la repr�sentation d'une intensit� lumineuse.
 * Une intensit� est d�finie par un ensemble de valeurs
 * d'intensit�s pour certaines longueurs d'ondes particuli�res.
 *
 * @author	Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 */
class CEnergy
{
private:
  float color[COMPOSANTES];
  /**< Tableaux d'intensit�. Chaque longueur d'onde
   * a une intensit� lumineuse propre de type r�elle.
   * Une intensit� lumineuse est la combinaison de ces
   * longueur d'ondes. Le syst�me se base pour l'instant
   * sur le code RVB.*/

public:
  /**
   * Constructeur par d�faut. Cr�e une intensit�
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
   * Constructeur param�trique.
   * @param c pointeur vers un tableau de <CODE>COMPOSANTES</CODE>
   * �l�ments de type <CODE>float</CODE>
   */
  CEnergy (float r, float g, float b){
    color[RED] = r;
    color[GREEN] = g;
    color[BLUE] = b;
    color[ALPHA] = 1.0;
  }
  /**
   * Constructeur param�trique.
   * @param c pointeur vers un tableau de <CODE>COMPOSANTES</CODE>
   * �l�ments de type <CODE>float</CODE>
   */
  CEnergy (float c[COMPOSANTES]){
    for (int i=0; i<COMPOSANTES; i++)
      color[i]=c[i];
  }
  /**
   * Destructeur par d�faut.
   */
  ~CEnergy(){

  };

  /**
   * Op�rateur d'addition d'intensit� lumineuses.
   * Chacunes des composantes de chaque intensit� sont
   * additionn�es une � une.
   * Seules les intensit�s sup�rieures � 0
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
   * Op�rateur de quotient par un scalaire. Chacune des composantes
   * de l'intensit� est divis�e upar un m�me scalaire.
   * Seules les intensit�s sup�rieures � 0 sont prises
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
   * Op�rateur de produit par un scalaire. Chacune des composantes
   * de l'intensit� est multipli�e upar un m�me scalaire.
   * Seules les intensit�s sup�rieures � 0 sont prises
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
   * Op�rateur de produit combinatoire d'intensit�s lumineuses.
   * Chacunes des composantes de chaque intensit� sont
   * multipli�es une � une.
   * Seules les intensit�s sup�rieures � 0 sont prises
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
   * Op�rateur d'�galit�.
   */
  bool operator==(const CEnergy& I) const
  {
    for (int i=0;i<COMPOSANTES;i++)
      if (color[i]!=I.color[i])
        return false;
    return true;
  }
  /**
   * Op�rateur d'addition d'intensit� lumineuses.
   * Chacunes des composantes de chaque intensit� sont
   * additionn�es une � une.
   * Cette fonction <B>modifie</B> l'op�rande de gauche
   * par affectation.
   * Seules les intensit�s sup�rieures � 0 sont prises en compte.
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
   * Op�rateur de quotient par un scalaire. Chacune des composantes
   * de l'intensit� est divis�e upar un m�me scalaire.
   * Cette fonction <B>modifie</B> l'op�rande de gauche par affectation.
   * Seules les intensit�s sup�rieures � 0 sont prises en
   * compte.
   */
  CEnergy& operator/=(const float& K){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=color[i]/K;
    return *this;
  }
  /**
   * Op�rateur de produit par un scalaire. Chacune des composantes
   * de l'intensit� est multipli�e upar un m�me scalaire.
   * Cette fonction <B>modifie</B> l'op�rande de gauche par affectation.
   * Seules les intensit�s sup�rieures � 0 sont prises en
   * compte.
   */
  CEnergy& operator*=(const float& K){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=color[i]*K;
    return *this;
  }
  /**
   * Op�rateur de produit combinatoire d'intensit�s lumineuses.
   * Chacunes des composantes de chaque intensit� sont
   * multipli�es une � une.
   * Cette fonction <B>modifie</B> l'op�rande de gauche par affectation.
   * Seules les intensit�s sup�rieures � 0 sont prises
   * en compte.
   */
  CEnergy& operator*=(const CEnergy& I){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=color[i]*I.color[i];
    return *this;
  }

  /**
   * Op�rateur d'allocation.
   */
  CEnergy& operator=(const CEnergy& I) {
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=I.color[i];
    return *this;
  };

  const float* getColors() const {
    return color;
  };

  void getColor(float &r, float &g, float &b) const {
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

  void randomize( float min, float max )
  {
    float range  = max - min;
    color[RED]   = (rand()/(float)RAND_MAX) * range + min;
    color[GREEN] = (rand()/(float)RAND_MAX) * range + min;
    color[BLUE]  = (rand()/(float)RAND_MAX) * range + min;
  }

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

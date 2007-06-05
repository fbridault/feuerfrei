/* Intensity.h: interface for the Intensite class.*/
#ifndef INTENSITY_H
#define INTENSITY_H

class Intensity;

#define COMPOSANTES             4
#define RED                     0
#define GREEN                   1
#define BLUE                    2

/** 
 * Classe pour la représentation d'une intensité lumineuse. 
 * Une intensité est définie par un ensemble de valeurs 
 * d'intensités pour certaines longueurs d'ondes particulières.
 *
 * @author	Christophe Cassagnabère
 * @version	%I%, %G%
 * @since	1.0
 */
class Intensity
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
  Intensity (){
    for(int i=0;i<COMPOSANTES; i++)
      color[i]=0.0;
  }
  /**
   * Constructeur par recopie.
   */
  Intensity (const Intensity& I){
    for(int i=0; i<COMPOSANTES; i++)
      color[i]=I.color[i];
  }
  /**
   * Constructeur paramétrique. 
   * @param c pointeur vers un tableau de <CODE>COMPOSANTES</CODE>
   * éléments de type <CODE>float</CODE>
   */
  Intensity (float* c){
    for(int i=0; i<COMPOSANTES; i++) 
      color[i]=c[i];
  }
  /**
   * Destructeur par défaut.
   */
  ~Intensity(){
    
  };

  /**
   * Opérateur d'addition d'intensité lumineuses.
   * Chacunes des composantes de chaque intensité sont
   * additionnées une à une.
   * Seules les intensités supérieures à 0 
   * sont prises en compte.
   */
  Intensity operator+(const Intensity& I) const
  {
    float result[COMPOSANTES];
    for(int i=0;i<COMPOSANTES;i++)
      result[i]= (color[i]>0.0?color[i]:0.0) +
	(I.color[i]>0.0?I.color[i]:0.0);
    Intensity intensity(result);
    return intensity;
  }
  /**
   * Opérateur de quotient par un scalaire. Chacune des composantes 
   * de l'intensité est divisée upar un même scalaire. 
   * Seules les intensités supérieures à 0 sont prises
   * en compte.
   */
  Intensity operator/(const float& K) const
  {
    float result[COMPOSANTES];
    for (int i=0;i<COMPOSANTES;i++)
      result[i]=(color[i]>0.0?color[i]:0.0)/K;
    Intensity intensity(result);
    return intensity;
  }
  /**
   * Opérateur de produit par un scalaire. Chacune des composantes 
   * de l'intensité est multipliée upar un même scalaire. 
   * Seules les intensités supérieures à 0 sont prises
   * en compte.
   */
  Intensity operator*(const float& K) const
  {
    float result[COMPOSANTES];
    for (int i=0;i<COMPOSANTES;i++)
      result[i]=(color[i]>0.0?color[i]:0.0)*K;
    Intensity intensity(result);
    return intensity;
  }

  /**
   * Opérateur de produit combinatoire d'intensités lumineuses.
   * Chacunes des composantes de chaque intensité sont
   * multipliées une à une. 
   * Seules les intensités supérieures à 0 sont prises
   * en compte.
   */
  Intensity operator*(const Intensity& I) const
  {
    float result[COMPOSANTES];
    for (int i=0;i<COMPOSANTES;i++)
      result[i]=(color[i]>=0.0?color[i]:0.0)*
	(I.color[i]>=0.0?I.color[i]:0.0);
    Intensity intensity(result);
    return intensity;
  }	
  /**
   * Opérateur d'égalité.
   */
  bool operator==(const Intensity& I) const
  {
    for (int i=0;i<COMPOSANTES;i++)
      if(color[i]!=I.color[i])
	return false;
    return true;
  }
  /**
   * Opérateur d'addition d'intensité lumineuses.
   * Chacunes des composantes de chaque intensité sont
   * additionnées une à une.
   * Cette fonction <B>modifie</B> l'opérande de gauche
   * par affectation.
   * Seules les intensités supérieures à 0 
   *sont prises en compte.
   */
  Intensity& operator+=(const Intensity& I){
    for(int i=0;i<COMPOSANTES;i++)
      color[i]= (color[i]>0.0?color[i]:0.0) +
	(I.color[i]>0.0?I.color[i]:0.0);
    return *this;
  }
  /**
   * Opérateur de quotient par un scalaire. Chacune des composantes 
   * de l'intensité est divisée upar un même scalaire.
   * Cette fonction <B>modifie</B> l'opérande de gauche par affectation.
   * Seules les intensités supérieures à 0 sont prises en
   * compte.
   */
  Intensity& operator/=(const float& K){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=(color[i]>0.0?color[i]:0.0)/K;
    return *this;
  }
  /**
   * Opérateur de produit par un scalaire. Chacune des composantes 
   * de l'intensité est multipliée upar un même scalaire.
   * Cette fonction <B>modifie</B> l'opérande de gauche par affectation.
   * Seules les intensités supérieures à 0 sont prises en
   * compte.
   */
  Intensity& operator*=(const float& K){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=(color[i]>0.0?color[i]:0.0)*K;
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
  Intensity& operator*=(const Intensity& I){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=(color[i]>=0.0?color[i]:0.0)*
	(I.color[i]>=0.0?I.color[i]:0.0);
    return *this;
  }
  /**
   * Opérateur d'allocation.
   */
  Intensity& operator=(const Intensity& I) {
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=I.color[i];
    return *this;
  };

  /**
   * Lecture d'une composante de l'intensité lumineuse.
   * @param waveLength indice de la longueur d'onde recherchée.
   * @return Constante de type <CODE>float</CODE> représentant 
   * la valeur d'intensité lumineuse pour cette longueur
   * d'onde.
   */
  const float& getColor (const int& waveLength) const
  {
    return color[waveLength];
  }
  int intensity2ColorRVB(){	
    float red = getColor(RED);
    float green = getColor(GREEN);
    float blue= getColor(BLUE);		
    return (
	    ((int) ((red   <1.0?red   :1.0)*255) << 16) |
	    ((int) ((green <1.0?green :1.0)*255) << 8)  |
	     (int) ((blue  <1.0?blue  :1.0)*255)
	    ); 
  }
};//class Intensity

#endif 

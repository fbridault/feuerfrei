/* Intensity.h: interface for the Intensite class.*/
#ifndef INTENSITY_H
#define INTENSITY_H

class Intensity;

#define COMPOSANTES             4
#define RED                     0
#define GREEN                   1
#define BLUE                    2

/** 
 * Classe pour la repr�sentation d'une intensit� lumineuse. 
 * Une intensit� est d�finie par un ensemble de valeurs 
 * d'intensit�s pour certaines longueurs d'ondes particuli�res.
 *
 * @author	Christophe Cassagnab�re
 * @version	%I%, %G%
 * @since	1.0
 */
class Intensity
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
   * Constructeur param�trique. 
   * @param c pointeur vers un tableau de <CODE>COMPOSANTES</CODE>
   * �l�ments de type <CODE>float</CODE>
   */
  Intensity (float* c){
    for(int i=0; i<COMPOSANTES; i++) 
      color[i]=c[i];
  }
  /**
   * Destructeur par d�faut.
   */
  ~Intensity(){
    
  };

  /**
   * Op�rateur d'addition d'intensit� lumineuses.
   * Chacunes des composantes de chaque intensit� sont
   * additionn�es une � une.
   * Seules les intensit�s sup�rieures � 0 
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
   * Op�rateur de quotient par un scalaire. Chacune des composantes 
   * de l'intensit� est divis�e upar un m�me scalaire. 
   * Seules les intensit�s sup�rieures � 0 sont prises
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
   * Op�rateur de produit par un scalaire. Chacune des composantes 
   * de l'intensit� est multipli�e upar un m�me scalaire. 
   * Seules les intensit�s sup�rieures � 0 sont prises
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
   * Op�rateur de produit combinatoire d'intensit�s lumineuses.
   * Chacunes des composantes de chaque intensit� sont
   * multipli�es une � une. 
   * Seules les intensit�s sup�rieures � 0 sont prises
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
   * Op�rateur d'�galit�.
   */
  bool operator==(const Intensity& I) const
  {
    for (int i=0;i<COMPOSANTES;i++)
      if(color[i]!=I.color[i])
	return false;
    return true;
  }
  /**
   * Op�rateur d'addition d'intensit� lumineuses.
   * Chacunes des composantes de chaque intensit� sont
   * additionn�es une � une.
   * Cette fonction <B>modifie</B> l'op�rande de gauche
   * par affectation.
   * Seules les intensit�s sup�rieures � 0 
   *sont prises en compte.
   */
  Intensity& operator+=(const Intensity& I){
    for(int i=0;i<COMPOSANTES;i++)
      color[i]= (color[i]>0.0?color[i]:0.0) +
	(I.color[i]>0.0?I.color[i]:0.0);
    return *this;
  }
  /**
   * Op�rateur de quotient par un scalaire. Chacune des composantes 
   * de l'intensit� est divis�e upar un m�me scalaire.
   * Cette fonction <B>modifie</B> l'op�rande de gauche par affectation.
   * Seules les intensit�s sup�rieures � 0 sont prises en
   * compte.
   */
  Intensity& operator/=(const float& K){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=(color[i]>0.0?color[i]:0.0)/K;
    return *this;
  }
  /**
   * Op�rateur de produit par un scalaire. Chacune des composantes 
   * de l'intensit� est multipli�e upar un m�me scalaire.
   * Cette fonction <B>modifie</B> l'op�rande de gauche par affectation.
   * Seules les intensit�s sup�rieures � 0 sont prises en
   * compte.
   */
  Intensity& operator*=(const float& K){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=(color[i]>0.0?color[i]:0.0)*K;
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
  Intensity& operator*=(const Intensity& I){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=(color[i]>=0.0?color[i]:0.0)*
	(I.color[i]>=0.0?I.color[i]:0.0);
    return *this;
  }
  /**
   * Op�rateur d'allocation.
   */
  Intensity& operator=(const Intensity& I) {
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=I.color[i];
    return *this;
  };

  /**
   * Lecture d'une composante de l'intensit� lumineuse.
   * @param waveLength indice de la longueur d'onde recherch�e.
   * @return Constante de type <CODE>float</CODE> repr�sentant 
   * la valeur d'intensit� lumineuse pour cette longueur
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

/* Intensity.h: interface for the Intensite class.*/
#ifndef INTENSITY_H
#define INTENSITY_H

class Intensity;

#define COMPOSANTES             4
#define RED                     0
#define GREEN                   1
#define BLUE                    2

/** 
 * Classe pour la repr&eacute;sentation d'une intensit&eacute; lumineuse. 
 * Une intensit&eacute; est d&eacute;finie par un ensemble de valeurs 
 * d'intensit&eacute;s pour certaines longueurs d'ondes particuli&egrave;res.
 *
 * @author	Christophe Cassagnab&egrave;re
 * @version	%I%, %G%
 * @since	1.0
 */
class Intensity
{
private:
  double color[COMPOSANTES];
  /**< Tableaux d'intensit&eacute;. Chaque longueur d'onde
   * a une intensit&eacute; lumineuse propre de type r&eacute;elle.
   * Une intensit&eacute; lumineuse est la combinaison de ces 
   * longueur d'ondes. Le syst&egrave;me se base pour l'instant
   * sur le code RVB.*/  

public:
  /**
   * Constructeur par d&eacute;faut. Cr&eacute;e une intensit&eacute;
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
   * Constructeur param&eacute;trique. 
   * @param c pointeur vers un tableau de <CODE>COMPOSANTES</CODE>
   * &eacute;l&eacute;ments de type <CODE>double</CODE>
   */
  Intensity (double* c){
    for(int i=0; i<COMPOSANTES; i++) 
      color[i]=c[i];
  }
  /**
   * Destructeur par d&eacute;faut.
   */
  ~Intensity(){
    
  };

  /**
   * Op&eacute;rateur d'addition d'intensit&eacute; lumineuses.
   * Chacunes des composantes de chaque intensit&eacute; sont
   * additionn&eacute;es une &agrave; une.
   * Seules les intensit&eacute;s sup&eacute;rieures &agrave; 0 
   * sont prises en compte.
   */
  Intensity operator+(const Intensity& I) const
  {
    double result[COMPOSANTES];
    for(int i=0;i<COMPOSANTES;i++)
      result[i]= (color[i]>0.0?color[i]:0.0) +
	(I.color[i]>0.0?I.color[i]:0.0);
    Intensity intensity(result);
    return intensity;
  }
  /**
   * Op&eacute;rateur de quotient par un scalaire. Chacune des composantes 
   * de l'intensit&eacute; est divis&eacute;e upar un m&ecric;me scalaire. 
   * Seules les intensit&eacute;s sup&eacute;rieures &agrave; 0 sont prises
   * en compte.
   */
  Intensity operator/(const double& K) const
  {
    double result[COMPOSANTES];
    for (int i=0;i<COMPOSANTES;i++)
      result[i]=(color[i]>0.0?color[i]:0.0)/K;
    Intensity intensity(result);
    return intensity;
  }
  /**
   * Op&eacute;rateur de produit par un scalaire. Chacune des composantes 
   * de l'intensit&eacute; est multipli&eacute;e upar un m&ecric;me scalaire. 
   * Seules les intensit&eacute;s sup&eacute;rieures &agrave; 0 sont prises
   * en compte.
   */
  Intensity operator*(const double& K) const
  {
    double result[COMPOSANTES];
    for (int i=0;i<COMPOSANTES;i++)
      result[i]=(color[i]>0.0?color[i]:0.0)*K;
    Intensity intensity(result);
    return intensity;
  }

  /**
   * Op&eacute;rateur de produit combinatoire d'intensit&eacute;s lumineuses.
   * Chacunes des composantes de chaque intensit&eacute; sont
   * multipli&eacute;es une &agrave; une. 
   * Seules les intensit&eacute;s sup&eacute;rieures &agrave; 0 sont prises
   * en compte.
   */
  Intensity operator*(const Intensity& I) const
  {
    double result[COMPOSANTES];
    for (int i=0;i<COMPOSANTES;i++)
      result[i]=(color[i]>=0.0?color[i]:0.0)*
	(I.color[i]>=0.0?I.color[i]:0.0);
    Intensity intensity(result);
    return intensity;
  }	
  /**
   * Op&eacute;rateur d'&eacute;galit&eacute;.
   */
  bool operator==(const Intensity& I) const
  {
    for (int i=0;i<COMPOSANTES;i++)
      if(color[i]!=I.color[i])
	return false;
    return true;
  }
  /**
   * Op&eacute;rateur d'addition d'intensit&eacute; lumineuses.
   * Chacunes des composantes de chaque intensit&eacute; sont
   * additionn&eacute;es une &agrave; une.
   * Cette fonction <B>modifie</B> l'op&eacute;rande de gauche
   * par affectation.
   * Seules les intensit&eacute;s sup&eacute;rieures &agrave; 0 
   *sont prises en compte.
   */
  Intensity& operator+=(const Intensity& I){
    for(int i=0;i<COMPOSANTES;i++)
      color[i]= (color[i]>0.0?color[i]:0.0) +
	(I.color[i]>0.0?I.color[i]:0.0);
    return *this;
  }
  /**
   * Op&eacute;rateur de quotient par un scalaire. Chacune des composantes 
   * de l'intensit&eacute; est divis&eacute;e upar un m&ecric;me scalaire.
   * Cette fonction <B>modifie</B> l'op&eacute;rande de gauche par affectation.
   * Seules les intensit&eacute;s sup&eacute;rieures &agrave; 0 sont prises en
   * compte.
   */
  Intensity& operator/=(const double& K){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=(color[i]>0.0?color[i]:0.0)/K;
    return *this;
  }
  /**
   * Op&eacute;rateur de produit par un scalaire. Chacune des composantes 
   * de l'intensit&eacute; est multipli&eacute;e upar un m&ecric;me scalaire.
   * Cette fonction <B>modifie</B> l'op&eacute;rande de gauche par affectation.
   * Seules les intensit&eacute;s sup&eacute;rieures &agrave; 0 sont prises en
   * compte.
   */
  Intensity& operator*=(const double& K){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=(color[i]>0.0?color[i]:0.0)*K;
    return *this;
  }
  /**
   * Op&eacute;rateur de produit combinatoire d'intensit&eacute;s lumineuses. 
   * Chacunes des composantes de chaque intensit&eacute; sont
   * multipli&eacute;es une &agrave; une.
   * Cette fonction <B>modifie</B> l'op&eacute;rande de gauche par affectation.
   * Seules les intensit&eacute;s sup&eacute;rieures &agrave; 0 sont prises
   * en compte.
   */
  Intensity& operator*=(const Intensity& I){
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=(color[i]>=0.0?color[i]:0.0)*
	(I.color[i]>=0.0?I.color[i]:0.0);
    return *this;
  }
  /**
   * Op&eacute;rateur d'allocation.
   */
  Intensity& operator=(const Intensity& I) {
    for (int i=0;i<COMPOSANTES;i++)
      color[i]=I.color[i];
    return *this;
  };

  /**
   * Lecture d'une composante de l'intensit&eacute; lumineuse.
   * @param waveLength indice de la longueur d'onde recherch&eacute;e.
   * @return Constante de type <CODE>double</CODE> repr&eacute;sentant 
   * la valeur d'intensit&eacute; lumineuse pour cette longueur
   * d'onde.
   */
  const double& getColor (const int& waveLength) const
  {
    return color[waveLength];
  }
  int intensity2ColorRVB(){	
    double red = getColor(RED);
    double green = getColor(GREEN);
    double blue= getColor(BLUE);		
    return (
	    ((int) ((red   <1.0?red   :1.0)*255) << 16) |
	    ((int) ((green <1.0?green :1.0)*255) << 8)  |
	     (int) ((blue  <1.0?blue  :1.0)*255)
	    ); 
  }
};//class Intensity

#endif 

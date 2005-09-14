/* Vector.hpp: interface des classes CVector & CPoint.*/
#ifndef VECTOR_H
#define VECTOR_H

class CPoint;
class CVector;

/* Parametres globaux.*/
#include "header.h"
#include "mathFn.hpp"

using namespace std;

/** 
 * Classe de base repr&eacute;sentant un point en trois dimensions. Les coordonn&eacute;es
 * de chaque point et de chaque vecteur ont pour base un rep&egrave;re orthonorm&eacute; unique. 
 * L'int&eacute;gralit&eacute; de la sc&egrave;ne est cr&eacute;e de mani&egrave;re à se baser sur ce syt&egrave;me de coordonn&eacute;es 
 * unique.
 *
 * @author	Christophe Cassagnab&egrave;re modifi&eacute;e par Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 */
class CPoint  
{
protected:
  GLfloat X;/**< Coordonn&eacute;e spatiale en <CODE>X</CODE> &agrave; partir de l'origine du rep&egrave;re global.*/
  GLfloat Y;/**< Coordonn&eacute;e spatiale en <CODE>Y</CODE> &agrave; partir de l'origine du rep&egrave;re global.*/
  GLfloat Z;/**< Coordonn&eacute;e spatiale en <CODE>Z</CODE> &agrave; partir de l'origine du rep&egrave;re global.*/
public:
  /**
   * Constructeur par d&eacute;faut. Ce constructeur place le point en coordonn&eacute;es <CODE>(0, 0, 0)</CODE> par d&eacute;faut.
   */
  CPoint(){X=0.0; Y=0.0; Z=0.0;};
  /**
   * Constructeur par recopie.
   */
  CPoint(const CPoint& P){X=P.X; Y=P.Y; Z=P.Z;};
  /**
   * Constructeur param&eacute;trique.
   * @param Xp	coordonn&eacute;e en <CODE>X</CODE> du point &agrave; cr&eacute;er.
   * @param Yp	coordonn&eacute;e en <CODE>Y</CODE> du point &agrave; cr&eacute;er.
   * @param Zp	coordonn&eacute;e en <CODE>Z</CODE> du point &agrave; cr&eacute;er.
   */
  CPoint(const GLfloat& Xp, const GLfloat& Yp, const GLfloat& Zp){X=Xp; Y=Yp; Z=Zp;};
  /**
   * Destructeur par d&eacute;faut.
   */
  virtual ~CPoint(){};

  /**
   * Op&eacute;rateur d'allocation. Cet op&eacute;rateur doit être surcharg&eacute; dans toute classe fille de CPoint.
   */
  virtual CPoint& operator= (const CPoint& P){X=P.X; Y=P.Y; Z=P.Z; return *this;};
  /**
   * Op&eacute;rateur d'&eacute;galit&eacute;. Une marge correspondant &egrave; plus ou moins <CODE>epsilon</CODE> est tol&eacute;r&eacute;e 
   * afin de lisser les impr&eacute;cisions de calcul d'intersection. Cet op&eacute;rateur doit être surcharg&eacute; dans 
   * toute classe fille de CPoint.
   */
  virtual bool operator== (const CPoint& P) const
  {
    return((X <= P.X+EPSILON  &&  X >= P.X-EPSILON) &&
	   (Y <= P.Y+EPSILON  &&  Y >= P.Y-EPSILON) &&
	   (Z <= P.Z+EPSILON  &&  Z >= P.Z-EPSILON));
  };
  /**
   * Op&eacute;rateur de soustraction. Retourne la diff&eacute;rence entre deux points.  Cet op&eacute;rateur doit être surcharg&eacute; 
   * dans toute classe fille de CPoint.
   * @param P	point a soustraire au point courant.
   * @return	El&eacute;ment de type Cpoint.
   */
  virtual CPoint operator- (const CPoint& P) const
  {
    return CPoint(X-P.X,Y-P.Y,Z-P.Z);
  };
  /**
   * Op&eacute;rateur de distance. Retourne la distance entre deux points.
   * @param P	point distant du point courant.
   * @return	Constante de type <CODE>double</CODE> repr&eacute;sentant la distance entre les deux points.
   */
  GLfloat distance (const CPoint& P) const
  {
    return(MathFn::Sqrt((X-P.X)*(X-P.X)+
			(Y-P.Y)*(Y-P.Y)+
			(Z-P.Z)*(Z-P.Z)));
  };
  /** 
   * Obtenir un point obtenu en multipliant le point courant
   * par un r&eacute;el.(Ajout de Michel Leblond).
   * @param K	facteur multiplicateur.
   * @return	El&eacute;ment de type Cpoint.
   */
  CPoint operator*(const GLfloat &K)
  {
    return CPoint(X*K,Y*K,Z*K);
  };

  /**
   * Lecture de l'attribut <CODE>X</CODE>.
   * @return	Constante de type <CODE>double</CODE> repr&eacute;sentant <CODE>X</CODE>.
   */
  const GLfloat& getX () const
  {return X;};
  /**
0   * Lecture de l'attribut <CODE>Y</CODE>.
   * @return	Constante de type <CODE>double</CODE> repr&eacute;sentant <CODE>Y</CODE>.
   */
  const GLfloat& getY () const 
  {return Y;};
  /**
   * Lecture de l'attribut <CODE>Z</CODE>.
   * @return	Constante de type <CODE>double</CODE> repr&eacute;sentant <CODE>Z</CODE>.
   */
  const GLfloat& getZ () const
  {return Z;};

  /**
   * Modification de l'attribut <CODE>X</CODE>.
   * @param nX	nouvelle valeur pour l'attribut <CODE>X</CODE>.
   */
  void setX (const GLfloat& nX){X=nX;};
  /**
   * Modification de l'attribut <CODE>Y</CODE>.
   * @param nY	nouvelle valeur pour l'attribut <CODE>Y</CODE>.
   */
  void setY (const GLfloat& nY){Y=nY;};
  /**
   * Modification de l'attribut <CODE>Z</CODE>.
   * @param nZ	nouvelle valeur pour l'attribut <CODE>Z</CODE>.
   */
  void setZ (const GLfloat& nZ){Z=nZ;};
  
  /**
   * Modification des attributs <CODE>X</CODE>, <CODE>Y</CODE> et <CODE>Z</CODE>.
   * @param nX	nouvelle valeur pour l'attribut <CODE>X</CODE>.
   * @param nY	nouvelle valeur pour l'attribut <CODE>Y</CODE>.
   * @param nZ	nouvelle valeur pour l'attribut <CODE>Z</CODE>.
   */
  void set (const GLfloat& nX,const GLfloat& nY,const GLfloat& nZ){X=nX;Y=nY;Z=nZ;};
  
  /**
   * Modification de l'attribut <CODE>X</CODE>.
   * @param nX	nouvelle valeur pour l'attribut <CODE>X</CODE>.
   */
  void addX (const GLfloat& nX){X+=nX;};
  /**
   * Modification de l'attribut <CODE>Y</CODE>.
   * @param nY	nouvelle valeur pour l'attribut <CODE>Y</CODE>.
   */
  void addY (const GLfloat& nY){Y+=nY;};
  /**
   * Modification de l'attribut <CODE>Z</CODE>.
   * @param nZ	nouvelle valeur pour l'attribut <CODE>Z</CODE>.
   */
  void addZ (const GLfloat& nZ){Z+=nZ;};

  /** Afficher les coordonnées d'un point */
  void afficher(){
    cout <<"("<<X<<", "<<Y<<", "<<Z<<")";
  }//afficher

  /** Aditionner deux points */
  virtual CPoint operator+(CPoint P){
    CPoint resultat(X+P.X,Y+P.Y,Z+P.Z);
    return resultat;
  }//operator+
	
	/** Diviser toutes les composantes par un scalaire */
  virtual CPoint operator/(GLfloat div){
    CPoint resultat(X/div,Y/div,Z/div);
    return resultat;
  }
  /** Affecter trois coordonnées à un point */
/*  void set(double xx, double yy, double zz){
    X=xx;
    Y=yy;
    Z=zz;
  }//set
*/

  /** Donne la distance au carré entre deux points de l'espace.
   * @param pt1 position du premier point de contrôle
   * @param pt2 position du second point de contrôle 
   * @return la distance au carré
   */
  double squaredDistanceFrom( const CPoint* const pt )
  {
    return( (X - pt->X) * (X - pt->X)
	    + (Y - pt->Y) * (Y - pt->Y)
	    + (Z - pt->Z) * (Z - pt->Z) ) ;
  }

  static CPoint pointBetween( const CPoint* const pt1, const CPoint* const pt2 )
  {
    return CPoint( (pt2->X + pt1->X)/2.0, (pt2->Y + pt1->Y)/2.0, (pt2->Z + pt1->Z)/2.0);
  }
	
	void resetToNull(){X=0.0; Y=0.0; Z=0.0;};
};//CPoint

/** 
 * Classe de base repr&eacute;sentant un vecteur en trois dimensions. Cette classe hérite
 * de la classe CPoint et surcharge bon nombre d'opérateurs applicables sur les vecteurs. 
 * Les vecteurs ne sont pas normalisés à leur création.
 *
 * @see		CPoint
 * @author	Christophe Cassagnab&egrave;re
 * @version	%I%, %G%
 * @since	1.0
 */
class CVector : public CPoint
{
public:
  /**
   * Constructeur par d&eacute;faut. Ce constructeur donne au vecteur les coordonn&eacute;es nulles par d&eacute;faut.
   */
  CVector():CPoint(){};
  /**
   * Constructeur par recopie.
   */
  CVector(const CVector& V){X=V.X; Y=V.Y; Z=V.Z;};
  /**
   * Constructeur par recopie d'un argument de type CPoint.
   */	
  CVector(const CPoint& P){X=P.getX(); Y=P.getY(); Z=P.getZ();};
  /**
   * Constructeur param&eacute;trique.
   * @param Xp	coordonn&eacute;e en <CODE>X</CODE> du vecteur &agrave; cr&eacute;er.
   * @param Yp	coordonn&eacute;e en <CODE>Y</CODE> du vecteur &agrave; cr&eacute;er.
   * @param Zp	coordonn&eacute;e en <CODE>Z</CODE>du vecteur &agrave; cr&eacute;er.
   */
  CVector(const GLfloat& Xp, const GLfloat& Yp, const GLfloat& Zp):CPoint(Xp, Yp, Zp){};
  /**
   * Constructeur param&eacute;trique.
   * @param PA	point de d&eacute;part du vecteur &agrave; cr&eacute;er.
   * @param PB	point de d'arriv&eacute;e du vecteur &agrave; cr&eacute;er.
   */
  CVector(const CPoint& PA, const CPoint& PB){
    X=PB.getX() - PA.getX();
    Y=PB.getY() - PA.getY();
    Z=PB.getZ() - PA.getZ();
  };
  /**
   * Destructeur par d&eacute;faut.
   */
  virtual ~CVector(){};

  /**
   * Op&eacute;rateur de produit scalaire. On note que si <CODE>U</CODE> et <CODE>V</CODE> sont deux vecteurs, alors
   * <B><CODE>U.V = ||U|| x ||V|| x cos(U,V)</CODE></B>. O&ugrave; <B><CODE>cos(U,V)</CODE></B> est le 
   * cosinus de l'angle entre les deux vecteurs <CODE>U</CODE> et <CODE>V</CODE>.
   * @param V second membre du produit scalaire. 
   * @return El&eacute;ment de type double, r&eacute;sultat de l'op&eacute;ration.
   */
  GLfloat operator*(const CVector& V) const
  {return (X*V.X + Y*V.Y + Z*V.Z);};
  /**
   * Op&eacute;rateur de produit par un scalaire.
   * @param K facteur multiplicateur pour chaque coordonn&eacute;e.
   * @return El&eacute;ment de type CVector correpondant au vecteur d'entr&eacute;e dont 
   * les coordonn&eacute;e ont &eacute;t&eacute; multipli&eacute;es par <CODE>K</CODE>.
   */
  virtual CVector operator*(const GLfloat& K) const
  {return CVector(X*K,Y*K,Z*K);};

  /**
   * Op&eacute;rateur de produit vectoriel.
   * @param V second membre du produit vectoriel.
   * @return El&eacute;ment de type CVector correpondant &agrave; un vecteur perpendiculaire au
   * plan d&eacute;finit par <CODE>V</CODE> et le vecteur courant.
   */
  CVector operator^(const CVector& V) const
  {
    return CVector(Y*V.Z - Z*V.Y,
		   Z*V.X - X*V.Z,
		   X*V.Y - Y*V.X);
  };

  /**
   * Op&eacute;rateur d'allocation.
   */
  virtual CVector& operator= (const CVector& V){X=V.X; Y=V.Y; Z=V.Z; return *this;};
  /**
   * Op&eacute;rateur d'&eacute;galit&eacute;. Cet op&eacute;rateur fonctionne de ma m&ecirc;me mani&egrave;re que l'op&eacute;rateur 
   * &eacute;quivalent de la classe CPoint.
   * @see CPoint
   */
  virtual bool operator== (const CVector& V) const
  {
    return((X <= V.X+EPSILON  &&  X >= V.X-EPSILON) &&
	   (Y <= V.Y+EPSILON  &&  Y >= V.Y-EPSILON) &&
	   (Z <= V.Z+EPSILON  &&  Z >= V.Z-EPSILON));
  };
  /**
   * Op&eacute;rateur de soustraction. Les coordonn&eacute;es du vecteur <CODE>V</CODE> sont soustraites une &agrave; une 
   * aux coordonn&eacute;es du vecteur courant. Relation de Chasles. 
   */
  virtual CVector operator-(const CVector& V) const
  {return CVector(X-V.X, Y-V.Y, Z-V.Z);};
  /**
   * Op&eacute;rateur d'addition. Les coordonn&eacute;es du vecteur <CODE>V</CODE> sont ajout&eacute;es une &agrave; une 
   * aux coordonn&eacute;es du vecteur courant. Relation de Chasles.
   */
  CVector operator+(const CVector& V) const
  {return CVector(X+V.X, Y+V.Y, Z+V.Z);};

  /**
   * Op&eacute;rateur de transposition d'un point. Les coordonn&eacute;es du vecteur courant 
   * sont ajout&eacute;es une &agrave; une  aux coordonn&eacute;es du point <CODE>P</CODE>.
   * @param P point devant &ecirc;tre translat&eacute;.
   * @return El&eacute;ment de type Cpoint correponsdant au tranlat&eacute; du point <CODE>P</CODE>.
   */
  CPoint operator+(const CPoint& P) const
  {return CPoint(X+P.getX(), Y+P.getY(), Z+P.getZ());};
  /**
   * Op&eacute;rateur de position de base planaire. Cet op&eacute;rateur calcule la composante 
   * &agrave; l'origine d'un plan dont le vecteur courant est la normale et 
   * passant par le point <CODE>P</CODE>.
   * @param P point par lequel passe le plan.
   * @return Composante &agrave; l'origine du plan d&eacute;finit.
   */
  GLfloat operator*(const CPoint& P) const
  {return(X*P.getX() + Y*P.getY() + Z*P.getZ());};
	
  /**
   * Calcul de la norme d'un vecteur. 
   * @return Norme du vecteur courant. R&eacute;sultat diff&eacute;rent de 1 si le vecteur 
   * n'est pas normalis&eacute;.
   */
  GLfloat length() const
  {return (MathFn::Sqrt(X*X + Y*Y + Z*Z));};
  /**
   * Normalisation d'un vecteur. Cette fonction <B>modifie</B> le vecteur courant.
   * @return Vecteur courant normalis&eacute;.
   */
  CVector& normalize(){return ((*this)=(*this)*(1.0/this->length()));};

  bool colinearWith(const CVector* const v)
  {
    /* Si les 2 vecteurs sont colinéaires alors l'aire du trapèze décrite par les 2 vecteurs est nulle */
    /* Aire du trapèze = norme du produit vectoriel des 2 vecteurs */
    CVector vres;
    
    vres = (*this) ^ (*v);
    
    return( vres.length() == 0);
  }
  
};

#endif

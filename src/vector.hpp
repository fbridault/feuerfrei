// /* Vector.hpp: interface des classes Vector & Point.*/
#ifndef VECTOR_H
#define VECTOR_H

class Point;
class Vector;

/* Parametres globaux */
#define EPSILON                 1.0e-8

#include <GL/gl.h>
#include <math.h>
#include <iostream>

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
class Point  
{
public:
  GLfloat x;/**< Coordonn&eacute;e spatiale en <CODE>x</CODE> &agrave; partir de l'origine du rep&egrave;re global.*/
  GLfloat y;/**< Coordonn&eacute;e spatiale en <CODE>y</CODE> &agrave; partir de l'origine du rep&egrave;re global.*/
  GLfloat z;/**< Coordonn&eacute;e spatiale en <CODE>z</CODE> &agrave; partir de l'origine du rep&egrave;re global.*/
public:
  /**
   * Constructeur par d&eacute;faut. Ce constructeur place le point en coordonn&eacute;es <CODE>(0, 0, 0)</CODE> par d&eacute;faut.
   */
  Point(){x=0.0; y=0.0; z=0.0;};
  /**
   * Constructeur par recopie.
   */
  Point(const Point& P){x=P.x; y=P.y; z=P.z;};
  /**
   * Constructeur param&eacute;trique.
   * @param xp	coordonn&eacute;e en <CODE>x</CODE> du point &agrave; cr&eacute;er.
   * @param yp	coordonn&eacute;e en <CODE>y</CODE> du point &agrave; cr&eacute;er.
   * @param zp	coordonn&eacute;e en <CODE>z</CODE> du point &agrave; cr&eacute;er.
   */
  Point(const GLfloat& xp, const GLfloat& yp, const GLfloat& zp){x=xp; y=yp; z=zp;};
  /**
   * Destructeur par d&eacute;faut.
   */
  virtual ~Point(){};

  /**
   * Op&eacute;rateur d'allocation. Cet op&eacute;rateur doit être surcharg&eacute; dans toute classe fille de Point.
   */
  virtual Point& operator= (const Point& P){x=P.x; y=P.y; z=P.z; return *this;};
  /**
   * Op&eacute;rateur d'&eacute;galit&eacute;. Une marge correspondant &egrave; plus ou moins <CODE>epsilon</CODE> est tol&eacute;r&eacute;e 
   * afin de lisser les impr&eacute;cisions de calcul d'intersection. Cet op&eacute;rateur doit être surcharg&eacute; dans 
   * toute classe fille de Point.
   */
  virtual bool operator== (const Point& P) const
  {
    return((x <= P.x+EPSILON  &&  x >= P.x-EPSILON) &&
	   (y <= P.y+EPSILON  &&  y >= P.y-EPSILON) &&
	   (z <= P.z+EPSILON  &&  z >= P.z-EPSILON));
  };
  /**
   * Op&eacute;rateur d'in&eacute;galit&eacute;. Une marge correspondant &egrave; plus ou moins <CODE>epsilon</CODE> est tol&eacute;r&eacute;e 
   * afin de lisser les impr&eacute;cisions de calcul d'intersection. Cet op&eacute;rateur doit être surcharg&eacute; dans 
   * toute classe fille de Point.
   */
  virtual bool operator!= (const Point& P) const
  {
    return(! (x <= P.x+EPSILON  &&  x >= P.x-EPSILON) &&
	     (y <= P.y+EPSILON  &&  y >= P.y-EPSILON) &&
	     (z <= P.z+EPSILON  &&  z >= P.z-EPSILON));
  };
  /**
   * Op&eacute;rateur de soustraction. Retourne la diff&eacute;rence entre deux points.  Cet op&eacute;rateur doit être surcharg&eacute; 
   * dans toute classe fille de Point.
   * @param P	point a soustraire au point courant.
   * @return	El&eacute;ment de type Cpoint.
   */
  virtual Point operator- (const Point& P) const
  {
    return Point(x-P.x,y-P.y,z-P.z);
  };
  /**
   * Op&eacute;rateur de distance. Retourne la distance entre deux points.
   * @param P	point distant du point courant.
   * @return	Constante de type <CODE>float</CODE> repr&eacute;sentant la distance entre les deux points.
   */
  GLfloat distance (const Point& P) const
  {
    return(sqrt((x-P.x)*(x-P.x)+ (y-P.y)*(y-P.y)+ (z-P.z)*(z-P.z)));
  };
  /** 
   * Obtenir un point obtenu en multipliant le point courant
   * par un r&eacute;el.(Ajout de Michel Leblond).
   * @param K	facteur multiplicateur.
   * @return	El&eacute;ment de type Cpoint.
   */
  Point operator*(const GLfloat &K) const
  {
    return Point(x*K,y*K,z*K);
  };
  void operator*=(const GLfloat &K)
  {
    x*=K; y*=K; z*=K;
  };
  /** 
   * Obtenir un point obtenu en multipliant le point courant
   * par un r&eacute;el.
   * @param K	facteur multiplicateur.
   * @return	El&eacute;ment de type Cpoint.
   */
  Point operator*(const Point &op) const
  {
    return Point(x*op.x, y*op.y, z*op.z);
  };
  /** 
   * Obtenir un point obtenu en multipliant le point courant
   * par un r&eacute;el.(Ajout de Michel Leblond).
   * @param K	facteur multiplicateur.
   * @return	El&eacute;ment de type Cpoint.
   */
  void operator*=(const Point &op)
  {
    x*=op.x; y*=op.y; z*=op.z;
  };
  /**
   * Modification des attributs <CODE>X</CODE>, <CODE>Y</CODE> et <CODE>Z</CODE>.
   * @param nX	nouvelle valeur pour l'attribut <CODE>X</CODE>.
   * @param nY	nouvelle valeur pour l'attribut <CODE>Y</CODE>.
   * @param nZ	nouvelle valeur pour l'attribut <CODE>Z</CODE>.
   */
  void set (const GLfloat& nX,const GLfloat& nY,const GLfloat& nZ){x=nX;y=nY;z=nZ;};
  
  /** Afficher les coordonnées d'un point */
  friend ostream& operator << (ostream& os,const Point& pt)
  {
    os << "(" << pt.x << ", " << pt.y << ", " << pt.z << ")";
    return os ;
  }
  /** Aditionner deux points */
  virtual Point operator+(const Point& P) const{    
    return Point (x+P.x,y+P.y,z+P.z);
  }//operator+
  
  Point operator+(const GLfloat &K) const
  {
    return Point(x+K,y+K,z+K);
  };
  /** Aditionner deux points */
  virtual void operator+=(const Point& P){
    x += P.x;
    y += P.y;
    z += P.z;
  }//operator+
  
  /** Diviser toutes les composantes par un scalaire.
   * @param div Scalaire.
   */
  virtual Point operator/(GLfloat div) const{   
    return Point(x/div,y/div,z/div);
  }
  
  /** Diviser toutes les composantes par un scalaire.
   * @param div Scalaire.
   */
  virtual void operator/=(GLfloat div){
    x/=div; y/=div; z/=div;
  }
  
  /** Diviser deux points composante par composante.
   * @param div Scalaire.
   */
  virtual Point operator/(Point& div) const{
    return Point(x/div.x,y/div.y,z/div.z);
  }
  
  /** Diviser deux points composante par composante.
   * @param div Scalaire.
   */
  virtual void operator/=(Point& div){
    x/=div.x; y/=div.y; z/=div.z;
  }

  /** Donne la distance au carr� entre deux points de l'espace.
   * @param pt Position du point.
   * @return La distance au carr� entre le point courant et celui pass� en param�tre.
   */
  float squaredDistanceFrom( const Point& pt ) const
  {
    return( (x - pt.x) * (x - pt.x)
	    + (y - pt.y) * (y - pt.y)
	    + (z - pt.z) * (z - pt.z) ) ;
  }

  static Point pointBetween( const Point* const pt1, const Point* const pt2 )
  {
    return Point( (pt2->x + pt1->x)/2.0, (pt2->y + pt1->y)/2.0, (pt2->z + pt1->z)/2.0);
  }
	
  void resetToNull(){x=0.0; y=0.0; z=0.0;};

  float max()
  { 
    if(x>y){
      if(x>z)
	return(x);
    }else
      if(y>z)
	return(y);
    return(z);
  }
};//Point

/** 
 * Classe de base repr&eacute;sentant un vecteur en trois dimensions. Cette classe hérite
 * de la classe Point et surcharge bon nombre d'opérateurs applicables sur les vecteurs. 
 * Les vecteurs ne sont pas normalisés à leur création.
 *
 * @see		Point
 * @author	Christophe Cassagnab&egrave;re
 * @version	%I%, %G%
 * @since	1.0
 */
class Vector : public Point
{
public:
  /**
   * Constructeur par d&eacute;faut. Ce constructeur donne au vecteur les coordonn&eacute;es nulles par d&eacute;faut.
   */
  Vector():Point(){};
  /**
   * Constructeur par recopie.
   */
  Vector(const Vector& V) : Point(V.x, V.y, V.z) {};
  /**
   * Constructeur par recopie d'un argument de type Point.
   */	
  Vector(const Point& P) : Point(P) {};
  /**
   * Constructeur param&eacute;trique.
   * @param xp	coordonn&eacute;e en <CODE>x</CODE> du vecteur &agrave; cr&eacute;er.
   * @param yp	coordonn&eacute;e en <CODE>y</CODE> du vecteur &agrave; cr&eacute;er.
   * @param zp	coordonn&eacute;e en <CODE>z</CODE>du vecteur &agrave; cr&eacute;er.
   */
  Vector(const GLfloat& xp, const GLfloat& yp, const GLfloat& zp):Point(xp, yp, zp){};
  /**
   * Constructeur param&eacute;trique.
   * @param PA	point de d&eacute;part du vecteur &agrave; cr&eacute;er.
   * @param PB	point de d'arriv&eacute;e du vecteur &agrave; cr&eacute;er.
   */
  Vector(const Point& PA, const Point& PB){
    x=PB.x - PA.x;
    y=PB.y - PA.y;
    z=PB.z - PA.z;
  };
  /**
   * Destructeur par d&eacute;faut.
   */
  virtual ~Vector(){};

  /**
   * Op&eacute;rateur de produit scalaire. On note que si <CODE>U</CODE> et <CODE>V</CODE> sont deux vecteurs, alors
   * <B><CODE>U.V = ||U|| x ||V|| x cos(U,V)</CODE></B>. O&ugrave; <B><CODE>cos(U,V)</CODE></B> est le 
   * cosinus de l'angle entre les deux vecteurs <CODE>U</CODE> et <CODE>V</CODE>.
   * @param V second membre du produit scalaire. 
   * @return El&eacute;ment de type float, r&eacute;sultat de l'op&eacute;ration.
   */
  GLfloat operator*(const Vector& V) const
  {return (x*V.x + y*V.y + z*V.z);};
  /**
   * Op&eacute;rateur de produit par un scalaire.
   * @param K facteur multiplicateur pour chaque coordonn&eacute;e.
   * @return El&eacute;ment de type Vector correpondant au vecteur d'entr&eacute;e dont 
   * les coordonn&eacute;e ont &eacute;t&eacute; multipli&eacute;es par <CODE>K</CODE>.
   */
  virtual Vector operator*(const GLfloat& K) const
  {return Vector(x*K,y*K,z*K);};

  /**
   * Op&eacute;rateur de produit vectoriel.
   * @param V second membre du produit vectoriel.
   * @return El&eacute;ment de type Vector correpondant &agrave; un vecteur perpendiculaire au
   * plan d&eacute;finit par <CODE>V</CODE> et le vecteur courant.
   */
  Vector operator^(const Vector& V) const
  {
    return Vector(y*V.z - z*V.y,
		   z*V.x - x*V.z,
		   x*V.y - y*V.x);
  };

  /**
   * Op&eacute;rateur d'allocation.
   */
  virtual Vector& operator= (const Vector& V){x=V.x; y=V.y; z=V.z; return *this;};
  /**
   * Op&eacute;rateur d'&eacute;galit&eacute;. Cet op&eacute;rateur fonctionne de ma m&ecirc;me mani&egrave;re que l'op&eacute;rateur 
   * &eacute;quivalent de la classe Point.
   * @see Point
   */
  virtual bool operator== (const Vector& V) const
  {
    return((x <= V.x+EPSILON  &&  x >= V.x-EPSILON) &&
	   (y <= V.y+EPSILON  &&  y >= V.y-EPSILON) &&
	   (z <= V.z+EPSILON  &&  z >= V.z-EPSILON));
  };
  /**
   * Op&eacute;rateur de soustraction. Les coordonn&eacute;es du vecteur <CODE>V</CODE> sont soustraites une &agrave; une 
   * aux coordonn&eacute;es du vecteur courant. Relation de Chasles. 
   */
  virtual Vector operator-(const Vector& V) const
  {return Vector(x-V.x, y-V.y, z-V.z);};
  /**
   * Op&eacute;rateur d'addition. Les coordonn&eacute;es du vecteur <CODE>V</CODE> sont ajout&eacute;es une &agrave; une 
   * aux coordonn&eacute;es du vecteur courant. Relation de Chasles.
   */
  Vector operator+(const Vector& V) const
  {return Vector(x+V.x, y+V.y, z+V.z);};

  /**
   * Op&eacute;rateur de transposition d'un point. Les coordonn&eacute;es du vecteur courant 
   * sont ajout&eacute;es une &agrave; une  aux coordonn&eacute;es du point <CODE>P</CODE>.
   * @param P point devant &ecirc;tre translat&eacute;.
   * @return El&eacute;ment de type Cpoint correponsdant au tranlat&eacute; du point <CODE>P</CODE>.
   */
  Point operator+(const Point& P) const
  {return Point(x+P.x, y+P.y, z+P.z);};
  /**
   * Op&eacute;rateur de position de base planaire. Cet op&eacute;rateur calcule la composante 
   * &agrave; l'origine d'un plan dont le vecteur courant est la normale et 
   * passant par le point <CODE>P</CODE>.
   * @param P point par lequel passe le plan.
   * @return Composante &agrave; l'origine du plan d&eacute;finit.
   */
  GLfloat operator*(const Point& P) const
  {return(x*P.x + y*P.y + z*P.z);};

  Vector scaleBy(const Vector& V) const
  {return Vector(x*V.x, y*V.y, z*V.z); };
	
  /**
   * Calcul de la norme d'un vecteur. 
   * @return Norme du vecteur courant. R&eacute;sultat diff&eacute;rent de 1 si le vecteur 
   * n'est pas normalis&eacute;.
   */
  GLfloat length() const
  {return (sqrt(x*x + y*y + z*z));};
  /**
   * Normalisation d'un vecteur. Cette fonction <B>modifie</B> le vecteur courant.
   * @return Vecteur courant normalis&eacute;.
   */
  Vector& normalize() {return ((*this)=(*this)*(1.0/this->length()));};

  bool colinearWith(const Vector* const v) const
  {
    /* Si les 2 vecteurs sont colinéaires alors l'aire du trapèze décrite par les 2 vecteurs est nulle */
    /* Aire du trapèze = norme du produit vectoriel des 2 vecteurs */
    Vector vres;
    
    vres = (*this) ^ (*v);
    
    return( vres.length() == 0);
  }
  
};

#endif

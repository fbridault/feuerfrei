// /* CVector.hpp: interface des classes CVector & CPoint.*/
#ifndef VECTOR_H
#define VECTOR_H

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
class CPoint
{
public:
	GLfloat x;/**< Coordonn&eacute;e spatiale en <CODE>x</CODE> &agrave; partir de l'origine du rep&egrave;re global.*/
	GLfloat y;/**< Coordonn&eacute;e spatiale en <CODE>y</CODE> &agrave; partir de l'origine du rep&egrave;re global.*/
	GLfloat z;/**< Coordonn&eacute;e spatiale en <CODE>z</CODE> &agrave; partir de l'origine du rep&egrave;re global.*/
public:
	/**
	 * Constructeur par d&eacute;faut. Ce constructeur place le point en coordonn&eacute;es <CODE>(0, 0, 0)</CODE> par d&eacute;faut.
	 */
	CPoint(){
		x=0.0;
		y=0.0;
		z=0.0;
	};
	/**
	 * Constructeur par recopie.
	 */
	CPoint(const CPoint& P){
		x=P.x;
		y=P.y;
		z=P.z;
	};
	/**
	 * Constructeur param&eacute;trique.
	 * @param xp	coordonn&eacute;e en <CODE>x</CODE> du point &agrave; cr&eacute;er.
	 * @param yp	coordonn&eacute;e en <CODE>y</CODE> du point &agrave; cr&eacute;er.
	 * @param zp	coordonn&eacute;e en <CODE>z</CODE> du point &agrave; cr&eacute;er.
	 */
	CPoint(const GLfloat& xp, const GLfloat& yp, const GLfloat& zp){
		x=xp;
		y=yp;
		z=zp;
	};
	/**
	 * Destructeur par d&eacute;faut.
	 */
	virtual ~CPoint(){};

	/**
	 * Op&eacute;rateur d'allocation. Cet op&eacute;rateur doit être surcharg&eacute; dans toute classe fille de CPoint.
	 */
	virtual CPoint& operator= (const CPoint& P){
		x=P.x;
		y=P.y;
		z=P.z;
		return *this;
	};
	/**
	 * Op&eacute;rateur d'&eacute;galit&eacute;. Une marge correspondant &egrave; plus ou moins <CODE>epsilon</CODE> est tol&eacute;r&eacute;e
	 * afin de lisser les impr&eacute;cisions de calcul d'intersection. Cet op&eacute;rateur doit être surcharg&eacute; dans
	 * toute classe fille de CPoint.
	 */
	virtual bool operator== (const CPoint& P) const
	{
		return((x <= P.x+EPSILON  &&  x >= P.x-EPSILON) &&
		       (y <= P.y+EPSILON  &&  y >= P.y-EPSILON) &&
		       (z <= P.z+EPSILON  &&  z >= P.z-EPSILON));
	};
	/**
	 * Op&eacute;rateur d'in&eacute;galit&eacute;. Une marge correspondant &egrave; plus ou moins <CODE>epsilon</CODE> est tol&eacute;r&eacute;e
	 * afin de lisser les impr&eacute;cisions de calcul d'intersection. Cet op&eacute;rateur doit être surcharg&eacute; dans
	 * toute classe fille de CPoint.
	 */
	virtual bool operator!= (const CPoint& P) const
	{
		return(! (x <= P.x+EPSILON  &&  x >= P.x-EPSILON) &&
		       (y <= P.y+EPSILON  &&  y >= P.y-EPSILON) &&
		       (z <= P.z+EPSILON  &&  z >= P.z-EPSILON));
	};
	/**
	 * Op&eacute;rateur de soustraction. Retourne la diff&eacute;rence entre deux points.  Cet op&eacute;rateur doit être surcharg&eacute;
	 * dans toute classe fille de CPoint.
	 * @param P	point a soustraire au point courant.
	 * @return	El&eacute;ment de type Cpoint.
	 */
	virtual CPoint operator- (const CPoint& P) const
	{
		return CPoint(x-P.x,y-P.y,z-P.z);
	};
	/**
	 * Op&eacute;rateur de distance. Retourne la distance entre deux points.
	 * @param P	point distant du point courant.
	 * @return	Constante de type <CODE>float</CODE> repr&eacute;sentant la distance entre les deux points.
	 */
	GLfloat distance (const CPoint& P) const
	{
		return(sqrt((x-P.x)*(x-P.x)+ (y-P.y)*(y-P.y)+ (z-P.z)*(z-P.z)));
	};
	/**
	 * Obtenir un point obtenu en multipliant le point courant
	 * par un r&eacute;el.(Ajout de Michel Leblond).
	 * @param K	facteur multiplicateur.
	 * @return	El&eacute;ment de type Cpoint.
	 */
	CPoint operator*(const GLfloat &K) const
	{
		return CPoint(x*K,y*K,z*K);
	};
	void operator*=(const GLfloat &K)
	{
		x*=K;
		y*=K;
		z*=K;
	};
	/**
	 * Obtenir un point obtenu en multipliant le point courant
	 * par un r&eacute;el.
	 * @param K	facteur multiplicateur.
	 * @return	El&eacute;ment de type Cpoint.
	 */
	CPoint operator*(const CPoint &op) const
	{
		return CPoint(x*op.x, y*op.y, z*op.z);
	};
	/**
	 * Obtenir un point obtenu en multipliant le point courant
	 * par un r&eacute;el.(Ajout de Michel Leblond).
	 * @param K	facteur multiplicateur.
	 * @return	El&eacute;ment de type Cpoint.
	 */
	void operator*=(const CPoint &op)
	{
		x*=op.x;
		y*=op.y;
		z*=op.z;
	};
	/**
	 * Modification des attributs <CODE>X</CODE>, <CODE>Y</CODE> et <CODE>Z</CODE>.
	 * @param nX	nouvelle valeur pour l'attribut <CODE>X</CODE>.
	 * @param nY	nouvelle valeur pour l'attribut <CODE>Y</CODE>.
	 * @param nZ	nouvelle valeur pour l'attribut <CODE>Z</CODE>.
	 */
	void set (const GLfloat& nX,const GLfloat& nY,const GLfloat& nZ){
		x=nX;
		y=nY;
		z=nZ;
	};

	/** Afficher les coordonnées d'un point */
	friend ostream& operator << (ostream& os,const CPoint& pt)
	{
		os << "(" << pt.x << ", " << pt.y << ", " << pt.z << ")";
		return os ;
	}
	/** Aditionner deux points */
	virtual CPoint operator+(const CPoint& P) const{
		return CPoint (x+P.x,y+P.y,z+P.z);
	}//operator+

	CPoint operator+(const GLfloat &K) const
	{
		return CPoint(x+K,y+K,z+K);
	};
	/** Aditionner deux points */
	virtual void operator+=(const CPoint& P){
		x += P.x;
		y += P.y;
		z += P.z;
	}//operator+

	/** Diviser toutes les composantes par un scalaire.
	 * @param div Scalaire.
	 */
	virtual CPoint operator/(GLfloat div) const{
		return CPoint(x/div,y/div,z/div);
	}

	/** Diviser toutes les composantes par un scalaire.
	 * @param div Scalaire.
	 */
	virtual void operator/=(GLfloat div){
		x/=div;
		y/=div;
		z/=div;
	}

	/** Diviser deux points composante par composante.
	 * @param div Scalaire.
	 */
	virtual CPoint operator/(CPoint& div) const{
		return CPoint(x/div.x,y/div.y,z/div.z);
	}

	/** Diviser deux points composante par composante.
	 * @param div Scalaire.
	 */
	virtual void operator/=(CPoint& div){
		x/=div.x;
		y/=div.y;
		z/=div.z;
	}

	/** Donne la distance au carr� entre deux points de l'espace.
	 * @param pt Position du point.
	 * @return La distance au carr� entre le point courant et celui pass� en param�tre.
	 */
	float squaredDistanceFrom( const CPoint& pt ) const
	{
		return( (x - pt.x) * (x - pt.x)
		        + (y - pt.y) * (y - pt.y)
		        + (z - pt.z) * (z - pt.z) ) ;
	}

	static CPoint pointBetween( const CPoint* const pt1, const CPoint* const pt2 )
	{
		return CPoint( (pt2->x + pt1->x)/2.0, (pt2->y + pt1->y)/2.0, (pt2->z + pt1->z)/2.0);
	}

	void resetToNull(){
		x=0.0;
		y=0.0;
		z=0.0;
	};

	float max()
	{
		if (x>y){
			if (x>z)
				return(x);
		}else
			if (y>z)
				return(y);
		return(z);
	}
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
	CVector(const CVector& V) : CPoint(V.x, V.y, V.z) {};
	/**
	 * Constructeur par recopie d'un argument de type CPoint.
	 */
	CVector(const CPoint& P) : CPoint(P) {};
	/**
	 * Constructeur param&eacute;trique.
	 * @param xp	coordonn&eacute;e en <CODE>x</CODE> du vecteur &agrave; cr&eacute;er.
	 * @param yp	coordonn&eacute;e en <CODE>y</CODE> du vecteur &agrave; cr&eacute;er.
	 * @param zp	coordonn&eacute;e en <CODE>z</CODE>du vecteur &agrave; cr&eacute;er.
	 */
	CVector(const GLfloat& xp, const GLfloat& yp, const GLfloat& zp):CPoint(xp, yp, zp){};
	/**
	 * Constructeur param&eacute;trique.
	 * @param PA	point de d&eacute;part du vecteur &agrave; cr&eacute;er.
	 * @param PB	point de d'arriv&eacute;e du vecteur &agrave; cr&eacute;er.
	 */
	CVector(const CPoint& PA, const CPoint& PB){
		x=PB.x - PA.x;
		y=PB.y - PA.y;
		z=PB.z - PA.z;
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
	 * @return El&eacute;ment de type float, r&eacute;sultat de l'op&eacute;ration.
	 */
	GLfloat operator*(const CVector& V) const
	{
		return (x*V.x + y*V.y + z*V.z);
	};
	/**
	 * Op&eacute;rateur de produit par un scalaire.
	 * @param K facteur multiplicateur pour chaque coordonn&eacute;e.
	 * @return El&eacute;ment de type CVector correpondant au vecteur d'entr&eacute;e dont
	 * les coordonn&eacute;e ont &eacute;t&eacute; multipli&eacute;es par <CODE>K</CODE>.
	 */
	virtual CVector operator*(const GLfloat& K) const
	{
		return CVector(x*K,y*K,z*K);
	};

	/**
	 * Op&eacute;rateur de produit vectoriel.
	 * @param V second membre du produit vectoriel.
	 * @return El&eacute;ment de type CVector correpondant &agrave; un vecteur perpendiculaire au
	 * plan d&eacute;finit par <CODE>V</CODE> et le vecteur courant.
	 */
	CVector operator^(const CVector& V) const
	{
		return CVector(y*V.z - z*V.y,
		               z*V.x - x*V.z,
		               x*V.y - y*V.x);
	};

	/**
	 * Op&eacute;rateur d'allocation.
	 */
	virtual CVector& operator= (const CVector& V){
		x=V.x;
		y=V.y;
		z=V.z;
		return *this;
	};
	/**
	 * Op&eacute;rateur d'&eacute;galit&eacute;. Cet op&eacute;rateur fonctionne de ma m&ecirc;me mani&egrave;re que l'op&eacute;rateur
	 * &eacute;quivalent de la classe CPoint.
	 * @see CPoint
	 */
	virtual bool operator== (const CVector& V) const
	{
		return((x <= V.x+EPSILON  &&  x >= V.x-EPSILON) &&
		       (y <= V.y+EPSILON  &&  y >= V.y-EPSILON) &&
		       (z <= V.z+EPSILON  &&  z >= V.z-EPSILON));
	};
	/**
	 * Op&eacute;rateur de soustraction. Les coordonn&eacute;es du vecteur <CODE>V</CODE> sont soustraites une &agrave; une
	 * aux coordonn&eacute;es du vecteur courant. Relation de Chasles.
	 */
	virtual CVector operator-(const CVector& V) const
	{
		return CVector(x-V.x, y-V.y, z-V.z);
	};
	/**
	 * Op&eacute;rateur d'addition. Les coordonn&eacute;es du vecteur <CODE>V</CODE> sont ajout&eacute;es une &agrave; une
	 * aux coordonn&eacute;es du vecteur courant. Relation de Chasles.
	 */
	CVector operator+(const CVector& V) const
	{
		return CVector(x+V.x, y+V.y, z+V.z);
	};

	/**
	 * Op&eacute;rateur de transposition d'un point. Les coordonn&eacute;es du vecteur courant
	 * sont ajout&eacute;es une &agrave; une  aux coordonn&eacute;es du point <CODE>P</CODE>.
	 * @param P point devant &ecirc;tre translat&eacute;.
	 * @return El&eacute;ment de type Cpoint correponsdant au tranlat&eacute; du point <CODE>P</CODE>.
	 */
	CPoint operator+(const CPoint& P) const
	{
		return CPoint(x+P.x, y+P.y, z+P.z);
	};
	/**
	 * Op&eacute;rateur de position de base planaire. Cet op&eacute;rateur calcule la composante
	 * &agrave; l'origine d'un plan dont le vecteur courant est la normale et
	 * passant par le point <CODE>P</CODE>.
	 * @param P point par lequel passe le plan.
	 * @return Composante &agrave; l'origine du plan d&eacute;finit.
	 */
	GLfloat operator*(const CPoint& P) const
	{
		return(x*P.x + y*P.y + z*P.z);
	};

	CVector scaleBy(const CVector& V) const
	{
		return CVector(x*V.x, y*V.y, z*V.z);
	};

	/**
	 * Calcul de la norme d'un vecteur.
	 * @return Norme du vecteur courant. R&eacute;sultat diff&eacute;rent de 1 si le vecteur
	 * n'est pas normalis&eacute;.
	 */
	GLfloat length() const
	{
		return (sqrt(x*x + y*y + z*z));
	};
	/**
	 * Normalisation d'un vecteur. Cette fonction <B>modifie</B> le vecteur courant.
	 * @return Vecteur courant normalis&eacute;.
	 */
	CVector& normalize() {
		return ((*this)=(*this)*(1.0/this->length()));
	};

	bool colinearWith(const CVector* const v) const
	{
		/* Si les 2 vecteurs sont colinéaires alors l'aire du trapèze décrite par les 2 vecteurs est nulle */
		/* Aire du trapèze = norme du produit vectoriel des 2 vecteurs */
		CVector vres;

		vres = (*this) ^ (*v);

		return( vres.length() == 0);
	}

};

#endif

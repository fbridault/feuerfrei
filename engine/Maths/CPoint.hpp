#ifndef CPOINT_H
#define CPOINT_H

#define EPSILON                 1.0e-8

#include <math.h>
#include <iostream>

#include "../Common.hpp"

/**
 * Classe de base repr&eacute;sentant un point en trois dimensions. Les coordonn&eacute;es
 * de chaque point et de chaque vecteur ont pour base un rep&egrave;re orthonorm&eacute; unique.
 * L'int&eacute;gralit&eacute; de la sc&egrave;ne est cr&eacute;e de mani&egrave;re Ã  se baser sur ce syt&egrave;me de coordonn&eacute;es
 * unique.
 *
 * @author	Flavien Bridault
 */
class CPoint
{
public:
	GLfloat x;/**< Coordonn&eacute;e spatiale en <CODE>x</CODE>*/
	GLfloat y;/**< Coordonn&eacute;e spatiale en <CODE>y</CODE>*/
	GLfloat z;/**< Coordonn&eacute;e spatiale en <CODE>z</CODE>*/
public:
	/**
	 * Constructeur par d&eacute;faut. Ce constructeur place le point en coordonn&eacute;es <CODE>(0, 0, 0)</CODE> par d&eacute;faut.
	 */
	CPoint()
	{
		x=0.0;
		y=0.0;
		z=0.0;
	};
	/**
	 * Constructeur par recopie.
	 */
	CPoint(const CPoint& P)
	{
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
	CPoint(const GLfloat& xp, const GLfloat& yp, const GLfloat& zp)
	{
		x=xp;
		y=yp;
		z=zp;
	};
	/**
	 * Destructeur par d&eacute;faut.
	 */
	virtual ~CPoint(){};

	/**
	 * Op&eacute;rateur d'allocation. Cet op&eacute;rateur doit Ãªtre surcharg&eacute; dans toute classe fille de CPoint.
	 */
	virtual CPoint& operator= (const CPoint& P)
	{
		x=P.x;
		y=P.y;
		z=P.z;
		return rThis;
	};
	/**
	 * Op&eacute;rateur d'&eacute;galit&eacute;. Une marge correspondant &egrave; plus ou moins <CODE>epsilon</CODE> est tol&eacute;r&eacute;e
	 * afin de lisser les impr&eacute;cisions de calcul d'intersection. Cet op&eacute;rateur doit Ãªtre surcharg&eacute; dans
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
	 * afin de lisser les impr&eacute;cisions de calcul d'intersection. Cet op&eacute;rateur doit Ãªtre surcharg&eacute; dans
	 * toute classe fille de CPoint.
	 */
	virtual bool operator!= (const CPoint& P) const
	{
		return(! (x <= P.x+EPSILON  &&  x >= P.x-EPSILON) &&
		       (y <= P.y+EPSILON  &&  y >= P.y-EPSILON) &&
		       (z <= P.z+EPSILON  &&  z >= P.z-EPSILON));
	};
	/**
	 * Op&eacute;rateur de soustraction. Retourne la diff&eacute;rence entre deux points.  Cet op&eacute;rateur doit Ãªtre surcharg&eacute;
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
	void set (const GLfloat& nX,const GLfloat& nY,const GLfloat& nZ)
	{
		x=nX;
		y=nY;
		z=nZ;
	};

	/** Afficher les coordonnÃ©es d'un point */
	friend ostream& operator << (ostream& os,const CPoint& pt)
	{
		os << "(" << pt.x << ", " << pt.y << ", " << pt.z << ")";
		return os ;
	}
	/** Aditionner deux points */
	virtual CPoint operator+(const CPoint& P) const
	{
		return CPoint (x+P.x,y+P.y,z+P.z);
	}//operator+

	CPoint operator+(const GLfloat &K) const
	{
		return CPoint(x+K,y+K,z+K);
	};
	/** Aditionner deux points */
	virtual void operator+=(const CPoint& P)
	{
		x += P.x;
		y += P.y;
		z += P.z;
	}//operator+

	/** Diviser toutes les composantes par un scalaire.
	 * @param div Scalaire.
	 */
	virtual CPoint operator/(GLfloat div) const
	{
		return CPoint(x/div,y/div,z/div);
	}

	/** Diviser toutes les composantes par un scalaire.
	 * @param div Scalaire.
	 */
	virtual void operator/=(GLfloat div)
	{
		x/=div;
		y/=div;
		z/=div;
	}

	/** Diviser deux points composante par composante.
	 * @param div Scalaire.
	 */
	virtual CPoint operator/(CPoint& div) const
	{
		return CPoint(x/div.x,y/div.y,z/div.z);
	}

	/** Diviser deux points composante par composante.
	 * @param div Scalaire.
	 */
	virtual void operator/=(CPoint& div)
	{
		x/=div.x;
		y/=div.y;
		z/=div.z;
	}

	/** Donne la distance au carré entre deux points de l'espace.
	 * @param pt Position du point.
	 * @return La distance au carré entre le point courant et celui passé en paramètre.
	 */
	float squaredDistanceFrom( const CPoint& pt ) const
	{
		return( (x - pt.x) * (x - pt.x)
		        + (y - pt.y) * (y - pt.y)
		        + (z - pt.z) * (z - pt.z) ) ;
	}

	static CPoint pointBetween(CPoint const& a_rPoint1, CPoint const& a_rPoint2 )
	{
		return CPoint( (a_rPoint2.x + a_rPoint1.x)/2.0, (a_rPoint2.y +a_rPoint1.y)/2.0, (a_rPoint2.z + a_rPoint1.z)/2.0);
	}

	void resetToNull()
	{
		x=0.0;
		y=0.0;
		z=0.0;
	};

	float max()
	{
		if (x>y)
		{
			if (x>z)
				return(x);
		}
		else
			if (y>z)
				return(y);
		return(z);
	}

	void randomize( float min, float max )
	{
		float range = max - min;
		x = (rand()/(float)RAND_MAX) * range + min;
		y = (rand()/(float)RAND_MAX) * range + min;
		z = (rand()/(float)RAND_MAX) * range + min;
	}

	/** Returns the triangle area
	* La formule est simple, base * hauteur / 2
	* Pour appliquer, je definis V1 et V2 = (P1,P2) et (P1,P3)
	* Puis je calcule la hauteur depend de la projection de V2
	* Sur V1 (produit scalaire).
	*/
	static float GetTriangleArea(const CPoint &P1,const CPoint &P2,const CPoint &P3);

	// creates a random point on the triangle P1, P2, P3 => this
	void setRandomPointInTriangle(const CPoint &P1, const CPoint &P2, const CPoint &P3);

	// Returns the minimum value of x,y,z
	float minCoord() const
	{
		return ((x<y)?((x<z)?x:z):(y<z)?y:z);
	};

	// Returns the maximum value of x,y,z
	float maxCoord() const
	{
		return ((x>y)?((x>z)?x:z):(y>z)?y:z);
	};

	/** "this" coords should be < v coords.
	* If not, the methods reverses the corresponding values
	*/
	void sortCoordsMinMax(CPoint &vMin, CPoint &vMax) const
	{
		float aux;
		if (x<0)
		{
			aux=vMin.x;
			vMin.x=vMax.x;
			vMax.x=aux;
		}

		if (y<0)
		{
			aux=vMin.y;
			vMin.y=vMax.y;
			vMax.y=aux;
		}

		if (z<0)
		{
			aux=vMin.z;
			vMin.z=vMax.z;
			vMax.z=aux;
		}
	}


	/** Opérateur de multiplication par une matrice 4x4, généralement utilisée
	*  pour transformer un point dans le repère de l'oeil (multiplication par
	*  la MODELVIEW)
	*/
	CPoint operator*(GLfloat mat[16]) const
	{
		CPoint res;

		res.x = mat[0] * x + mat[4] * y + mat[8]  * z + mat[12];
		res.y = mat[1] * x + mat[5] * y + mat[9]  * z + mat[13];
		res.z = mat[2] * x + mat[6] * y + mat[10] * z + mat[14];

		return res;
	}

	// ================================================================
	// creates a point on a sphere, given theta/phi/r
	//
	void setSphereVertex(float th, float ph, const CPoint &C, float r)
	{
		x=C.x+r*sin(th)*cos(ph);
		y=C.y+r*cos(th);
		z=C.z+r*sin(th)*sin(ph);
	}

};//CPoint

#endif

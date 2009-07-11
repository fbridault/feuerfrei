#ifndef VECTOR_H
#define VECTOR_H

#include "CPoint.hpp"

/**
 * Classe de base repr&eacute;sentant un vecteur en trois dimensions. Cette classe hÃ©rite
 * de la classe CPoint et surcharge bon nombre d'opÃ©rateurs applicables sur les vecteurs.
 * Les vecteurs ne sont pas normalisÃ©s Ã  leur crÃ©ation.
 *
 * @see		CPoint
 * @author	Flavien Bridault
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
	CVector(const CPoint& PA, const CPoint& PB)
	{
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
	virtual CVector& operator= (const CVector& V)
	{
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

	/** Calcul de la norme d'un vecteur.
	 * @return Norme du vecteur courant. R&eacute;sultat diff&eacute;rent de 1 si le vecteur
	 * n'est pas normalis&eacute;.
	 */
	GLfloat norm() const
	{
		return (sqrt(x*x + y*y + z*z));
	};


	/** Calcul de la norme au carré d'un vecteur.
	 * @return Norme du vecteur courant. R&eacute;sultat diff&eacute;rent de 1 si le vecteur
	 * n'est pas normalis&eacute;.
	 */
	GLfloat sqrNorm() const
	{
		return (x*x + y*y + z*z);
	};

	/**
	 * Normalisation d'un vecteur. Cette fonction <B>modifie</B> le vecteur courant.
	 * @return Vecteur courant normalis&eacute;.
	 */
	CVector& normalize()
	{
		return ((*this)=(*this)*(1.0/this->norm()));
	};

	bool colinearWith(const CVector* const v) const
	{
		/* Si les 2 vecteurs sont colinÃ©aires alors l'aire du trapÃ¨ze dÃ©crite par les 2 vecteurs est nulle */
		/* Aire du trapÃ¨ze = norme du produit vectoriel des 2 vecteurs */
		CVector vres;

		vres = (*this) ^ (*v);

		return( vres.norm() == 0);
	}

	CVector GetMinCoord() const
	{
		float xp=fabs(x);
		float yp=fabs(y);
		float zp=fabs(z);

		if (xp<yp)
			if (xp<zp)
				return CVector(1,0,0);
			else
				return CVector(0,0,1);
		else
			if (yp<zp)
				return CVector(0,1,0);
			else
				return CVector(0,0,1);
	}

	/** Opérateur de multiplication par une matrice 4x4, généralement utilisée
	 *  pour transformer un vecteur dans le repère de l'oeil (multiplication par
	 *  la MODELVIEW). On ne prend donc pas en compte la translation.
	 */
	CVector operator*(GLfloat mat[16]) const
	{
		CVector res;

		res.x = mat[0] * x + mat[4] * y + mat[8]  * z;
		res.y = mat[1] * x + mat[5] * y + mat[9]  * z;
		res.z = mat[2] * x + mat[6] * y + mat[10] * z;

		return res;
	}

};

#endif

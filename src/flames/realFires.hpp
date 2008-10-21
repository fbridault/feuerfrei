#if !defined(FIRESOURCES_H)
#define FIRESOURCES_H

class Candle;
class Firmalampe;

#include "abstractFires.hpp"
#include "cloneFlames.hpp"

class CPointFlame;
class CLineFlame;
class FireSource;

/** La classe Candle permet la d�finition d'une bougie.
 *
 * @author	Flavien Bridault
 */
class Candle : public FireSource
{
public:
	/** Constructeur d'une bougie.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param scene CPointeur sur la sc�ne.
	 * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
	 * @param program pointeur sur le program charg� de la construction des shadow volumes.
	 * @param rayon Rayon de la flamme.
	 * @param wickFileName nom du fichier contenant la m�che. Si NULL, alors wick doit �tre fourni.
	 * @param wick Optionnel, objet repr�sentant la m�che. Si NULL, un cylindre simple est utilis�.
	 */
	Candle(const FlameConfig& flameConfig, Field3D * s, CScene* const scene, uint index,
	       const CShader& a_rShader, float rayon, const char *wickFileName, CWick *wick=NULL);
	/** Destructeur */
	virtual ~Candle(){};

	void setLightPosition (const CPoint& pos);

	/** Active la source de lumi�re. */
	void switchOnMulti ();
	/** Eteint la source */
	void switchOffMulti ();

private:
	/** Position de la lumi�re ponctuelle OpenGL dans l'espace. */
	GLfloat m_lightPositions[8][4];
	uint m_nbLights;
};


/** La classe Firmalampe permet la d�finition d'une firmalampe.
 *
 * @author	Flavien Bridault
 */
class Firmalampe : public FireSource
{
public:
	/** Constructeur d'une torche.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param scene CPointeur sur la sc�ne.
	 * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
	 * @param program pointeur sur le program charg� de la construction des shadow volumes.
	 * @param wickFileName nom du fichier contenant la m�che
	 */
	Firmalampe(const FlameConfig& flameConfig, Field3D * s, CScene *scene, uint index,
	           const CShader& a_rShader, const char *wickFileName);
	/** Destructeur */
	virtual ~Firmalampe(){};
};

/** La classe Torche permet la d�finition d'une flamme de type torche.
 * Le fichier OBJ repr�sentant le luminaire contient des m�ches qui doivent avoir un nom
 * en CWick*. Celle-ci ne sont pas affich�es � l'�cran. Les objets composant le luminaire
 * doivent s'appeler Torch.*
 *
 * @author	Flavien Bridault
 */
class Torch : public DetachableFireSource
{
public:
	/** Constructeur d'une torche.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param scene CPointeur sur la sc�ne.
	 * @param torchName nom du fichier contenant le luminaire.
	 * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
	 * @param program pointeur sur le program charg� de la construction des shadow volumes.
	 */
	Torch(const FlameConfig& flameConfig, Field3D * s, CScene *scene, const char *torchName, uint index,
	      const CShader& a_rShader);
	/** Destructeur */
	virtual ~Torch(){};

	/** Dessine la m�che de la flamme. Les m�ches des IRealFlame sont d�finies en (0,0,0), une translation
	 * est donc effectu�e pour tenir compte du placement du feu dans le monde.
	 */
	virtual void drawWick(bool displayBoxes) const {};

};

/** La classe CampFire permet la d�finition d'un feu de camp.
 * Le fichier OBJ repr�sentant le luminaire contient des m�ches qui doivent avoir un nom
 * en CWick*. A la diff�rence de la classe Torch, les m�ches sont affich�es. En revanche,
 * il n'existe pas de luminaire.
 *
 * @author	Flavien Bridault
 */
class CampFire : public DetachableFireSource
{
public:
	/** Constructeur d'une torche.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param scene CPointeur sur la sc�ne.
	 * @param fireName nom du fichier contenant le luminaire.
	 * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
	 * @param program pointeur sur le program charg� de la construction des shadow volumes.
	 */
	CampFire(const FlameConfig& flameConfig, Field3D * s, CScene *scene, const char *fireName, uint index,
	         const CShader& a_rShader);
	/** Destructeur */
	virtual ~CampFire(){};
};

/** La classe CandleStick permet la d�finition d'un chandelier. Elle est compos�e de flammes
 * clones.
 *
 * @author	Flavien Bridault
 */
class CandleStick : public FireSource
{
public:
	/** Constructeur d'un chandelier.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param scene CPointeur sur la sc�ne.
	 * @param filename Nom du fichier contenant le luminaire.
	 * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
	 * @param program pointeur sur le program charg� de la construction des shadow volumes.
	 * @param rayon Rayon de la flamme.
	 */
	CandleStick(const FlameConfig& flameConfig, Field3D * s, CScene *scene, const char *filename, uint index,
	            const CShader& a_rShader, float rayon);
	/** Destructeur */
	virtual ~CandleStick();

	virtual void build();

	virtual void drawWick(bool displayBoxes) const
	{
		CPoint pt(getPosition());
		CPoint scale(m_solver->getScale());
		glPushMatrix();
		glTranslatef (pt.x, pt.y, pt.z);
		glScalef (scale.x, scale.y, scale.z);
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->drawWick(displayBoxes);
		for (uint i = 0; i < m_nbCloneFlames; i++)
			m_cloneFlames[i]->drawWick(displayBoxes);
		glPopMatrix();
	}

	virtual void drawFlame(bool display, bool displayParticle, u_char boundingVolume=0) const
	{
		switch (boundingVolume)
		{
			case BOUNDING_SPHERE :
				drawBoundingSphere();
				break;
			case IMPOSTOR :
				drawImpostor();
				break;
			default :
				if (m_visibility)
				{
					CPoint pt(m_solver->getPosition());
					CPoint scale(m_solver->getScale());
					glPushMatrix();
					glTranslatef (pt.x, pt.y, pt.z);
					glScalef (scale.x, scale.y, scale.z);
					for (uint i = 0; i < m_nbFlames; i++)
						m_flames[i]->drawFlame(display, displayParticle);
					for (uint i = 0; i < m_nbCloneFlames; i++)
						m_cloneFlames[i]->drawFlame(display, displayParticle);
					glPopMatrix();
				}
		}
	}

	virtual void toggleSmoothShading (bool state);
//   virtual void setSamplingTolerance(u_char value){
//     FireSource::setSamplingTolerance(value);
//     for (uint i = 0; i < m_nbCloneFlames; i++)
//       m_cloneFlames[i]->setSamplingTolerance(value);
//   };

private:
	/** Nombre de flammes clones */
	uint m_nbCloneFlames;
	/** Tableau contenant les flammes clones */
	CloneCPointFlame **m_cloneFlames;
};


#endif

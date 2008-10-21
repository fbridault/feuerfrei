#if !defined(FIRESOURCES_H)
#define FIRESOURCES_H

class Candle;
class Firmalampe;

#include "abstractFires.hpp"
#include "cloneFlames.hpp"

class CPointFlame;
class CLineFlame;
class FireSource;

/** La classe Candle permet la définition d'une bougie.
 *
 * @author	Flavien Bridault
 */
class Candle : public FireSource
{
public:
	/** Constructeur d'une bougie.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param scene CPointeur sur la scène.
	 * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
	 * @param program pointeur sur le program chargé de la construction des shadow volumes.
	 * @param rayon Rayon de la flamme.
	 * @param wickFileName nom du fichier contenant la mèche. Si NULL, alors wick doit être fourni.
	 * @param wick Optionnel, objet représentant la mèche. Si NULL, un cylindre simple est utilisé.
	 */
	Candle(const FlameConfig& flameConfig, Field3D * s, CScene* const scene, uint index,
	       const CShader& a_rShader, float rayon, const char *wickFileName, CWick *wick=NULL);
	/** Destructeur */
	virtual ~Candle(){};

	void setLightPosition (const CPoint& pos);

	/** Active la source de lumière. */
	void switchOnMulti ();
	/** Eteint la source */
	void switchOffMulti ();

private:
	/** Position de la lumière ponctuelle OpenGL dans l'espace. */
	GLfloat m_lightPositions[8][4];
	uint m_nbLights;
};


/** La classe Firmalampe permet la définition d'une firmalampe.
 *
 * @author	Flavien Bridault
 */
class Firmalampe : public FireSource
{
public:
	/** Constructeur d'une torche.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param scene CPointeur sur la scène.
	 * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
	 * @param program pointeur sur le program chargé de la construction des shadow volumes.
	 * @param wickFileName nom du fichier contenant la mèche
	 */
	Firmalampe(const FlameConfig& flameConfig, Field3D * s, CScene *scene, uint index,
	           const CShader& a_rShader, const char *wickFileName);
	/** Destructeur */
	virtual ~Firmalampe(){};
};

/** La classe Torche permet la définition d'une flamme de type torche.
 * Le fichier OBJ représentant le luminaire contient des mèches qui doivent avoir un nom
 * en CWick*. Celle-ci ne sont pas affichées à l'écran. Les objets composant le luminaire
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
	 * @param scene CPointeur sur la scène.
	 * @param torchName nom du fichier contenant le luminaire.
	 * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
	 * @param program pointeur sur le program chargé de la construction des shadow volumes.
	 */
	Torch(const FlameConfig& flameConfig, Field3D * s, CScene *scene, const char *torchName, uint index,
	      const CShader& a_rShader);
	/** Destructeur */
	virtual ~Torch(){};

	/** Dessine la mèche de la flamme. Les mèches des IRealFlame sont définies en (0,0,0), une translation
	 * est donc effectuée pour tenir compte du placement du feu dans le monde.
	 */
	virtual void drawWick(bool displayBoxes) const {};

};

/** La classe CampFire permet la définition d'un feu de camp.
 * Le fichier OBJ représentant le luminaire contient des mèches qui doivent avoir un nom
 * en CWick*. A la différence de la classe Torch, les mèches sont affichées. En revanche,
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
	 * @param scene CPointeur sur la scène.
	 * @param fireName nom du fichier contenant le luminaire.
	 * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
	 * @param program pointeur sur le program chargé de la construction des shadow volumes.
	 */
	CampFire(const FlameConfig& flameConfig, Field3D * s, CScene *scene, const char *fireName, uint index,
	         const CShader& a_rShader);
	/** Destructeur */
	virtual ~CampFire(){};
};

/** La classe CandleStick permet la définition d'un chandelier. Elle est composée de flammes
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
	 * @param scene CPointeur sur la scène.
	 * @param filename Nom du fichier contenant le luminaire.
	 * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
	 * @param program pointeur sur le program chargé de la construction des shadow volumes.
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

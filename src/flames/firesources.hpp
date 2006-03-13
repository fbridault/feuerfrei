#if !defined(FIRESOURCES_H)
#define FIRESOURCES_H

class Candle;
class Firmalampe;

#include "fire.hpp"

class PointFlame;
class LineFlame;
class FireSource;

/** La classe Candle permet la définition d'une bougie.
 *
 * @author	Flavien Bridault
 */
class Candle : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la scène
   * @param innerForce force intérieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL)
   * @param shader pointeur sur le shader chargé de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes périphériques
   */
  Candle(Solver * s, Point& posRel, Scene *scene, double innerForce,  const char *filename, 
	 int index, CgSVShader * shader, double rayon, int nbSkeletons);
  virtual ~Candle(){};
};


/** La classe Firmalampe permet la définition d'une firmalampe.
 *
 * @author	Flavien Bridault
 */
class Firmalampe : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la scène
   * @param innerForce force intérieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL)
   * @param shader pointeur sur le shader chargé de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes périphériques
   * @param wickFileName nom du fichier contenant la mèche
   */
  Firmalampe(Solver * s, Point& posRel, Scene *scene, double innerForce,  const char *filename, 
	     int index, CgSVShader * shader, int nbSkeletons, const char *wickFileName);
  virtual ~Firmalampe(){};
};

/** La classe Firmalampe permet la définition d'une firmalampe.
 *
 * @author	Flavien Bridault
 */
class Torch : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la scène
   * @param innerForce force intérieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL)
   * @param shader pointeur sur le shader chargé de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes périphériques
   * @param wickFileName nom du fichier contenant la torche
   */
  Torch(Solver * s, Point& posRel, Scene *scene, double innerForce,  const char *filename, 
	int index, CgSVShader * shader, int nbSkeletons, const char *torchName);
  virtual ~Torch(){};
};

#endif

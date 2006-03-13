#if !defined(FIRESOURCES_H)
#define FIRESOURCES_H

class Candle;
class Firmalampe;

#include "fire.hpp"

class PointFlame;
class LineFlame;
class FireSource;

/** La classe Candle permet la d�finition d'une bougie.
 *
 * @author	Flavien Bridault
 */
class Candle : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la sc�ne
   * @param innerForce force int�rieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL)
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes p�riph�riques
   */
  Candle(Solver * s, Point& posRel, Scene *scene, double innerForce,  const char *filename, 
	 int index, CgSVShader * shader, double rayon, int nbSkeletons);
  virtual ~Candle(){};
};


/** La classe Firmalampe permet la d�finition d'une firmalampe.
 *
 * @author	Flavien Bridault
 */
class Firmalampe : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la sc�ne
   * @param innerForce force int�rieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL)
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes p�riph�riques
   * @param wickFileName nom du fichier contenant la m�che
   */
  Firmalampe(Solver * s, Point& posRel, Scene *scene, double innerForce,  const char *filename, 
	     int index, CgSVShader * shader, int nbSkeletons, const char *wickFileName);
  virtual ~Firmalampe(){};
};

/** La classe Firmalampe permet la d�finition d'une firmalampe.
 *
 * @author	Flavien Bridault
 */
class Torch : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la sc�ne
   * @param innerForce force int�rieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL)
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes p�riph�riques
   * @param wickFileName nom du fichier contenant la torche
   */
  Torch(Solver * s, Point& posRel, Scene *scene, double innerForce,  const char *filename, 
	int index, CgSVShader * shader, int nbSkeletons, const char *torchName);
  virtual ~Torch(){};
};

#endif

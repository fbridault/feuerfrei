#if !defined(SKELETON_H)
#define SKELETON_H

class Skeleton;
class Particle;

#include "header.h"

/* Particule dans l'espace, utilisée pour construire les squelettes
 * et la surface NURBS. Chaque particule est simplement un point, et
 * possède une durée de vie.
 *
 * @author	Flavien Bridault
 */
class Particle : public CPoint
{
public:
  /** Durée de vie de la particule */
  int lifespan;
  /**
   * Constructeur par d&eacute;faut. Ce constructeur donne à la particule des coordonn&eacute;es nulles par d&eacute;faut.
   */
  Particle():CPoint(){lifespan=0;};
  
  /** Constructeur par recopie */
  Particle(const Particle& P):CPoint(P)
  {
    lifespan=P.lifespan;
  };
  
  /** Décrémente la durée de vie de la particule */
  void decreaseLife(){lifespan--;};
  /** Test pour déterminer si la particule est encore vivante */
  bool isDead(){return (lifespan < 1);};
  /** Naissance d'une particule, on affecte sa durée de vie à la valeur passée en paramètre
   * @param l durée de vie en terme d'itérations
   */
  void birth(int l){lifespan=l;};
  
  virtual Particle& operator= (const Particle& P){X=P.X; Y=P.Y; Z=P.Z; lifespan=P.lifespan; return *this;};
  virtual Particle& operator= (const CPoint& P){X=P.getX(); Y=P.getY(); Z=P.getZ(); return *this;};
	
private:
};

class Solver;

/** La classe Skeleton est une file de particules gérée avec un vecteur, mais elle 
 * transgresse néanmoins la règle en permettant notamment un accès direct à tous les membres de la file
 * sans les supprimer.
 * L'entrée et la sortie d'éléments dans la file se gèrent à l'aide des fonctions entree() et sortie().
 * Si l'entrée est systématiquement en queue de file, la sortie peut s'effectuer au-delà de la tête de file.
 * Il est également utile de préciser que la queue de la file est mobile. Son indice prend donc 
 * des valeurs comprises dans [0;NB_PARTICULES]. La tête de la file est donc immobile et fixée à 0.
 * L'élément en queue de file est généralement le plus près de l'origine du squelette, et l'élément
 * en tête est donc le plus éloigné de l'origine. Chaque particule possède une durée de vie qui est
 * décrementée dès qu'un nouvel élément est inséré.
 *
 * @author	Flavien Bridault
 */
class Skeleton
{
public:
  /** Constructeur de squelette
   * @param s pointeur sur le solveur de fluides
   * @param position position de la flamme dans l'espace
   * @param pt position de l'origine du squelette
   */
  Skeleton(Solver* const s, const CPoint position, const CPoint pt);
  
  /** Constructeur de squelette
   * @param s pointeur sur le solveur de fluides
   * @param position position de la flamme dans l'espace
   * @param pt position de l'origine du squelette
   * @param pls durée de vie initiale d'une particule
   */
  Skeleton(Solver* const s, const CPoint position, const CPoint pt, int pls);
  virtual ~Skeleton();
  
  /** Donne l'élément en tête de file.
   * @return particule en tête de file
   */
  Particle *getLastElt() const{
    return &file[queue];
  };

  /** Donne l'élément à l'indice passé en paramètre.
   * @param i indice
   * @return particule à l'indice i
   */
  Particle *getElt(int i) const{
    return &file[i];
  };

  /** Donne l'origine du squelette.
   * @return position de l'origine du squelette
   */
  CPoint *getOrigine(){
    return &origine;
  };

  /** Donne la taille du squelette.
   * @return nombre de particules contenues dans la file du squelette
   */
  int getSize() const{
    return queue+1;
  };

  void draw ();

protected:
  /** Echange deux particules dans la file .
   * @param i indice de la première particule
   * @param j indice de la seconde particule
   */
  void swap(int i, int j);

  /** Insère une particule en queue de file.
   * @param pt position de la particule
   */
  void entree(const CPoint* const pt);

  /** Supprime la particule à une position donnée.
   * @param n indice
   */
  void sortie(int n);

  /** Déplace l'origine du squelette dans le champ de vélocité.
   * @param u vecteur de vélocité en u
   * @param v vecteur de vélocité en v
   * @param w vecteur de vélocité en w
   */
  virtual int move_origine()=0;

  /** Affectation de la position d'une particule dans l'espace 
   * @param i indice de la particule dans la file
   * @param pt nouvelle position de la particule
   */
  void setEltFile(int i, const CPoint* const pt);

  virtual void draw_origine ();

  virtual void draw_particle (Particle * const particle);

  /** Origine actuelle du squelette. */
  CPoint origine;
  /** Origine initiale du squelette. */
  CPoint origine_save;

  /** Pointeur sur le solveur de fluides. */
  Solver *solveur;
  /** Position de la flamme à laquelle appartient le squelette */
  CPoint flamePos;

private:
  /** File de particules. */
  Particle *file;
  
  /** Indice de la tête de la file. */
  int queue;
  
  /** Durée de vie initiale d'une particule */
  int particle_lifespan;
};

#endif

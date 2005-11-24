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
  
  virtual Particle& operator= (const Particle& P){x=P.x; y=P.y; z=P.z; lifespan=P.lifespan; return *this;};
  virtual Particle& operator= (const CPoint& P){x=P.x; y=P.y; z=P.z; return *this;};
	
private:
};

class Solver;

/** La classe Skeleton est une file de particules gérée avec un vecteur, mais elle 
 * transgresse néanmoins la règle en permettant notamment un accès direct à tous les membres de la file
 * sans les supprimer.
 * L'entrée et la removeParticle d'éléments dans la file se gèrent à l'aide des fonctions addParticle() et sortie().
 * Si l'entrée est systématiquement en queue de file, la removeParticle peut s'effectuer au-delà de la tête de file.
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
   * @param pls durée de vie initiale d'une particule
   */
  Skeleton(Solver* const s, const CPoint& position, const CPoint& rootMoveFactor, int pls);
  virtual ~Skeleton();
  
  /** Donne l'élément en tête de file.
   * @return particule en tête de file
   */
  Particle *getLastParticle() const{
    return &m_queue[m_headIndex];
  };

  /** Donne l'élément à l'indice passé en paramètre.
   * @param i indice
   * @return particule à l'indice i
   */
  Particle *getParticle(int i) const{
    return &m_queue[i];
  };

  /** Donne l'origine du squelette.
   * @return position de l'origine du squelette
   */
  CPoint *getRoot(){
    return &m_root;
  };

  /** Donne la taille du squelette.
   * @return nombre de particules contenues dans la file du squelette
   */
  int getSize() const{
    return m_headIndex+1;
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
  void addParticle(const CPoint* const pt);

  /** Supprime la particule à une position donnée.
   * @param n indice
   */
  void removeParticle(int n);

  /** Déplace l'origine du squelette dans le champ de vélocité.
   * @param u vecteur de vélocité en u
   * @param v vecteur de vélocité en v
   * @param w vecteur de vélocité en w
   */
  virtual int moveRoot()=0;

  /** Affectation de la position d'une particule dans l'espace 
   * @param i indice de la particule dans la file
   * @param pt nouvelle position de la particule
   */
  void updateParticle(int i, const CPoint* const pt);

  virtual void drawRoot ();

  virtual void drawParticle (Particle * const particle);

  /** Origine actuelle du squelette. */
  CPoint m_root;
  /** Origine initiale du squelette. */
  CPoint m_rootSave;

  /** Pointeur sur le solveur de fluides. */
  Solver *m_solver;

  /** Contient trois facteurs correctifs pour le déplacement de l'origine 
   * des squelettes. Selon le type de flamme, il est en effet nécessaire
   * que les origines se déplacent différemment
   */
  CPoint m_rootMoveFactor;
private:
  /** File de particules. */
  Particle *m_queue;
  
  /** Indice de la tête de la file. */
  int m_headIndex;
  
  /** Durée de vie initiale d'une particule */
  int m_particleLifespan;

};

#endif

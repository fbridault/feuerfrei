#if !defined(SKELETON_H)
#define SKELETON_H

class Skeleton;
class Particle;

#include "flames.hpp"

/** Particule dans l'espace, utilisée pour construire les squelettes
 * et la surface NURBS. Chaque particule est simplement un point, et
 * possède une durée de vie.
 *
 * @author	Flavien Bridault
 */
class Particle : public Point
{
public:
  /** Durée de vie de la particule */
  uint m_lifespan;
  /**
   * Constructeur par d&eacute;faut. Ce constructeur donne à la particule des coordonn&eacute;es nulles par d&eacute;faut.
   */
  Particle():Point(){m_lifespan=0;};
  
  /** Ce constructeur prend un point de l'espace comme base.
   * @param P point dans l'espace
   * @param lifespan durée de vie initiale
   */
  Particle(const Point& P, uint lifespan):Point(P)
  {
    m_lifespan=lifespan;
  };
  
  /** Constructeur par recopie 
   * @param P particule source
   */
  Particle(const Particle& P):Point(P)
  {
    m_lifespan=P.m_lifespan;
  };
  
  /** Décrémente la durée de vie de la particule. */
  void decreaseLife(){m_lifespan--;};
  
  /** Test pour déterminer si la particule est encore vivante.
   * @return vraie si la particule est morte
   */  
  bool isDead(){return (m_lifespan < 1);};
  
  /** Naissance d'une particule, on affecte sa durée de vie à la valeur passée en paramètre.
   * @param l durée de vie en terme d'itérations
   */
  void birth(uint l){m_lifespan=l;};
  
  /** Surchage de l'opérateur =
   * @param P particule source
   */
  virtual Particle& operator= (const Particle& P){x=P.x; y=P.y; z=P.z; m_lifespan=P.m_lifespan; return *this;};
  
  /** Surchage de l'opérateur =
   * @param P point dans l'espace
   */
  virtual Particle& operator= (const Point& P){x=P.x; y=P.y; z=P.z; return *this;};
};

class Field3D;

/** La classe FreeSkeleton est une file de particules gérée avec un vecteur, mais elle ne permet que 
 * la sortie d'éléments. Seule la sous-classe Skeleton permet d'ajouter des particules. Le nombre, la durée de vie
 * et la position des particules sont fixés dans le constructeur. Un FreeSkeleton est donc obligatoirement
 * éphémère.<br>
 * La sortie d'éléments dans la file se gère à l'aide de la fonctions removeParticle().
 * Il est également utile de préciser que la queue de la file est mobile. Son indice prend donc 
 * des valeurs comprises dans [0;size[. La tête de la file est donc immobile et fixée à 0.
 * L'élément en queue de file est généralement le plus près de l'origine du squelette, et l'élément
 * en tête est donc le plus éloigné de l'origine.
 *
 * @author	Flavien Bridault
 */
class FreeSkeleton
{
public:
  /** Constructeur de squelette libre.
   * @param size Nombre de particules maximum du squelette.
   * @param s Pointeur sur le solveur de fluides.
   */
  FreeSkeleton(uint size, Field3D* const s);
  
  /** Constructeur de squelette libre. Permet de construire un squelette à partir
   * d'un autre, en découpant celui en deux à la hauteur passée en paramètre. Le squelette
   * crée sera constitué de la partie supérieure, donc des particules comprises dans
   * l'intervalle [0;splitHeight].
   * @param src Pointeur sur le squelette source.
   * @param splitHeight Hauteur de la découpe.
   */
  FreeSkeleton(const FreeSkeleton * const src, uint splitHeight);
  /** Destructeur */
  virtual ~FreeSkeleton();
  
  /** Donne l'élément en tête de file.
   * @return particule en tête de file
   */
  Particle *getLastParticle() const{
    return &m_queue[m_headIndex];
  };
  
  /** Donne l'élément à l'indice passé en paramètre.
   * @param i Indice de la particule.
   * @return Particule à l'indice i.
   */
  Particle *getParticle(uint i) const{
    return &m_queue[i];
  };
  
  /** Donne l'élément situé au milieu.
   * @return Pointeur sur la particule du milieu.
   */
  Particle *getMiddleParticle() const{
    return &m_queue[m_headIndex/2];
  };
  
  /** Donne la taille du squelette, sans tenir compte éventuellement de l'origine.
   * @return nombre de particules contenues dans la file du squelette
   */
  virtual uint getSize() const{
    return m_headIndex+1;
  };
  
  /** Retourne true si le squelette ne contient plus de particules.
   * @return true sur le squelette est mort
   */
  virtual bool isDead() const{
    return (m_headIndex < 0);
  };
    
  /** Dessine le squelette à l'écran. */
  virtual void draw ();
  
  /** Déplacement des particules du squelette et suppression des particules mortes.
   * Aucune particule n'est générée dans cette classe.
   */
  virtual void move();
  
  /** Déplace une particule dans le champ de vélocité.
   * @param pos position de la particule
   */
  virtual bool moveParticle(Particle* const pos);

protected:
  /** Echange deux particules dans la file.
   * @param i indice de la première particule
   * @param j indice de la seconde particule
   */
  void swap(uint i, uint j);
    
  /** Supprime la particule à une position donnée.
   * @param n indice
   */
  virtual void removeParticle(uint n);
  
  /** Affectation de la position d'une particule dans l'espace.
   * @param i indice de la particule dans la file
   * @param pt nouvelle position de la particule
   */
  void updateParticle(uint i, const Point* const pt);
  
  /** Dessine les particules du squelettes 
   * @param particle Particule à dessiner.
   */
  virtual void drawParticle (Particle * const particle);
  
  /** Pointeur sur le solveur de fluides. */
  Field3D *m_solver;

  /** Indice de la tête de la file. */
  int m_headIndex;
  
  /** File de particules. */
  Particle *m_queue;  
};

/** La classe Skeleton est une file de particules gérée avec un vecteur, mais elle 
 * transgresse néanmoins la règle en permettant notamment un accès direct à tous les membres de la file
 * sans les supprimer.
 * L'entrée et la sortie d'éléments dans la file se gèrent à l'aide des fonctions addParticle() et removeParticle().
 * Si l'entrée est systématiquement en queue de file, la sortie peut s'effectuer au-delà de la tête de file.
 * Il est également utile de préciser que la queue de la file est mobile. Son indice prend donc 
 * des valeurs comprises dans [0;NB_PARTICLES_MAX[. La tête de la file est donc immobile et fixée à 0.
 * L'élément en queue de file est généralement le plus près de l'origine du squelette, et l'élément
 * en tête est donc le plus éloigné de l'origine. Chaque particule possède une durée de vie qui est
 * décrementée dès qu'un nouvel élément est inséré.
 *
 * @author	Flavien Bridault
 */
class Skeleton : public FreeSkeleton
{
public:
  /** Constructeur de squelette périphérique libre
   * @param s Pointeur sur le solveur de fluides.
   * @param position Position de la flamme dans l'espace.
   * @param rootMoveFactor Amplitude du déplacement autorisé pour l'origine du squelette. Varie
   * en fonction du type de flamme.
   * @param pls Durée de vie initiale d'une particule.
   */
  Skeleton(Field3D* const s, const Point& position, const Point& rootMoveFactor, uint *pls);
  /** Destructeur. */
  virtual ~Skeleton(){};
  
  void draw ();
  
  /** Déplacement des particules du squelette, génération d'une nouvelle particule et
   * suppression des particules mortes.
   */
  void move();
  virtual bool moveParticle(Particle* const pos);
  
  /** Déplacement de l'origine du squelette. */
  void moveRoot ();
  
  /** Retourne un pointeur sur l'origine du squelette.
   * @return Pointeur sur l'origine du squelette.
   */
  Point *getRoot(){
    return &m_root;
  };
  
protected:
  /** Insère une particule en queue de file.
   * @param pt position de la particule
   */
  void addParticle(const Point* const pt);
    
  /** Dessine l'origine du squelette. */
  virtual void drawRoot ();
  
  /** Origine actuelle du squelette. */
  Point m_root;
  
  /** Origine initiale du squelette. */
  Point m_rootSave;
  
private:
  /** Durée de vie initiale d'une particule */
  uint *m_particleLifespan;  
  
  /** Contient trois facteurs correctifs pour le déplacement de l'origine
   * des squelettes. Selon le type de flamme, il est en effet nécessaire
   * que les origines se déplacent différemment.
   */
  Point m_rootMoveFactor;
};

#endif

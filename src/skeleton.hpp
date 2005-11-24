#if !defined(SKELETON_H)
#define SKELETON_H

class Skeleton;
class Particle;

#include "header.h"

/* Particule dans l'espace, utilis�e pour construire les squelettes
 * et la surface NURBS. Chaque particule est simplement un point, et
 * poss�de une dur�e de vie.
 *
 * @author	Flavien Bridault
 */
class Particle : public CPoint
{
public:
  /** Dur�e de vie de la particule */
  int lifespan;
  /**
   * Constructeur par d&eacute;faut. Ce constructeur donne � la particule des coordonn&eacute;es nulles par d&eacute;faut.
   */
  Particle():CPoint(){lifespan=0;};
  
  /** Constructeur par recopie */
  Particle(const Particle& P):CPoint(P)
  {
    lifespan=P.lifespan;
  };
  
  /** D�cr�mente la dur�e de vie de la particule */
  void decreaseLife(){lifespan--;};
  /** Test pour d�terminer si la particule est encore vivante */
  bool isDead(){return (lifespan < 1);};
  /** Naissance d'une particule, on affecte sa dur�e de vie � la valeur pass�e en param�tre
   * @param l dur�e de vie en terme d'it�rations
   */
  void birth(int l){lifespan=l;};
  
  virtual Particle& operator= (const Particle& P){x=P.x; y=P.y; z=P.z; lifespan=P.lifespan; return *this;};
  virtual Particle& operator= (const CPoint& P){x=P.x; y=P.y; z=P.z; return *this;};
	
private:
};

class Solver;

/** La classe Skeleton est une file de particules g�r�e avec un vecteur, mais elle 
 * transgresse n�anmoins la r�gle en permettant notamment un acc�s direct � tous les membres de la file
 * sans les supprimer.
 * L'entr�e et la removeParticle d'�l�ments dans la file se g�rent � l'aide des fonctions addParticle() et sortie().
 * Si l'entr�e est syst�matiquement en queue de file, la removeParticle peut s'effectuer au-del� de la t�te de file.
 * Il est �galement utile de pr�ciser que la queue de la file est mobile. Son indice prend donc 
 * des valeurs comprises dans [0;NB_PARTICULES]. La t�te de la file est donc immobile et fix�e � 0.
 * L'�l�ment en queue de file est g�n�ralement le plus pr�s de l'origine du squelette, et l'�l�ment
 * en t�te est donc le plus �loign� de l'origine. Chaque particule poss�de une dur�e de vie qui est
 * d�crement�e d�s qu'un nouvel �l�ment est ins�r�.
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
   * @param pls dur�e de vie initiale d'une particule
   */
  Skeleton(Solver* const s, const CPoint& position, const CPoint& rootMoveFactor, int pls);
  virtual ~Skeleton();
  
  /** Donne l'�l�ment en t�te de file.
   * @return particule en t�te de file
   */
  Particle *getLastParticle() const{
    return &m_queue[m_headIndex];
  };

  /** Donne l'�l�ment � l'indice pass� en param�tre.
   * @param i indice
   * @return particule � l'indice i
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
   * @param i indice de la premi�re particule
   * @param j indice de la seconde particule
   */
  void swap(int i, int j);

  /** Ins�re une particule en queue de file.
   * @param pt position de la particule
   */
  void addParticle(const CPoint* const pt);

  /** Supprime la particule � une position donn�e.
   * @param n indice
   */
  void removeParticle(int n);

  /** D�place l'origine du squelette dans le champ de v�locit�.
   * @param u vecteur de v�locit� en u
   * @param v vecteur de v�locit� en v
   * @param w vecteur de v�locit� en w
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

  /** Contient trois facteurs correctifs pour le d�placement de l'origine 
   * des squelettes. Selon le type de flamme, il est en effet n�cessaire
   * que les origines se d�placent diff�remment
   */
  CPoint m_rootMoveFactor;
private:
  /** File de particules. */
  Particle *m_queue;
  
  /** Indice de la t�te de la file. */
  int m_headIndex;
  
  /** Dur�e de vie initiale d'une particule */
  int m_particleLifespan;

};

#endif

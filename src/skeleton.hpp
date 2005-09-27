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
  
  virtual Particle& operator= (const Particle& P){X=P.X; Y=P.Y; Z=P.Z; lifespan=P.lifespan; return *this;};
  virtual Particle& operator= (const CPoint& P){X=P.getX(); Y=P.getY(); Z=P.getZ(); return *this;};
	
private:
};

class Solver;

/** La classe Skeleton est une file de particules g�r�e avec un vecteur, mais elle 
 * transgresse n�anmoins la r�gle en permettant notamment un acc�s direct � tous les membres de la file
 * sans les supprimer.
 * L'entr�e et la sortie d'�l�ments dans la file se g�rent � l'aide des fonctions entree() et sortie().
 * Si l'entr�e est syst�matiquement en queue de file, la sortie peut s'effectuer au-del� de la t�te de file.
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
   */
  Skeleton(Solver* const s, const CPoint position, const CPoint pt);
  
  /** Constructeur de squelette
   * @param s pointeur sur le solveur de fluides
   * @param position position de la flamme dans l'espace
   * @param pt position de l'origine du squelette
   * @param pls dur�e de vie initiale d'une particule
   */
  Skeleton(Solver* const s, const CPoint position, const CPoint pt, int pls);
  virtual ~Skeleton();
  
  /** Donne l'�l�ment en t�te de file.
   * @return particule en t�te de file
   */
  Particle *getLastElt() const{
    return &file[queue];
  };

  /** Donne l'�l�ment � l'indice pass� en param�tre.
   * @param i indice
   * @return particule � l'indice i
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
   * @param i indice de la premi�re particule
   * @param j indice de la seconde particule
   */
  void swap(int i, int j);

  /** Ins�re une particule en queue de file.
   * @param pt position de la particule
   */
  void entree(const CPoint* const pt);

  /** Supprime la particule � une position donn�e.
   * @param n indice
   */
  void sortie(int n);

  /** D�place l'origine du squelette dans le champ de v�locit�.
   * @param u vecteur de v�locit� en u
   * @param v vecteur de v�locit� en v
   * @param w vecteur de v�locit� en w
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
  /** Position de la flamme � laquelle appartient le squelette */
  CPoint flamePos;

private:
  /** File de particules. */
  Particle *file;
  
  /** Indice de la t�te de la file. */
  int queue;
  
  /** Dur�e de vie initiale d'une particule */
  int particle_lifespan;
};

#endif

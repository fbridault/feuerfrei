#if !defined(SKELETON_H)
#define SKELETON_H

class Skeleton;
class Particle;

#include "flames.hpp"
#include "particle.hpp"

class Field3D;

/** La classe FreeSkeleton est une file de particules g�r�e avec un vecteur, mais elle ne permet que 
 * la sortie d'�l�ments. Seule la sous-classe Skeleton permet d'ajouter des particules. Le nombre, la dur�e de vie
 * et la position des particules sont fix�s dans le constructeur. Un FreeSkeleton est donc obligatoirement
 * �ph�m�re.<br>
 * La sortie d'�l�ments dans la file se g�re � l'aide de la fonctions removeParticle().
 * Il est �galement utile de pr�ciser que la queue de la file est mobile. Son indice prend donc 
 * des valeurs comprises dans [0;size[. La t�te de la file est donc immobile et fix�e � 0.
 * L'�l�ment en queue de file est g�n�ralement le plus pr�s de l'origine du squelette, et l'�l�ment
 * en t�te est donc le plus �loign� de l'origine.
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
  
  /** Constructeur de squelette libre. Permet de construire un squelette � partir
   * d'un autre, en d�coupant celui en deux � la hauteur pass�e en param�tre. Le squelette
   * cr�e sera constitu� de la partie sup�rieure, donc des particules comprises dans
   * l'intervalle [0;splitHeight].
   * @param src Pointeur sur le squelette source.
   * @param splitHeight Hauteur de la d�coupe.
   */
  FreeSkeleton(const FreeSkeleton * const src, uint splitHeight);
  /** Destructeur */
  virtual ~FreeSkeleton();
  
  /** Donne l'�l�ment en t�te de file.
   * @return particule en t�te de file
   */
  Particle *getLastParticle() const{
    return &m_queue[m_headIndex];
  };
  
  /** Donne l'�l�ment � l'indice pass� en param�tre.
   * @param i Indice de la particule.
   * @return Particule � l'indice i.
   */
  Particle *getInternalParticle(uint i) const{
    return &m_queue[i];
  };
  
  /** Donne l'�l�ment � l'indice pass� en param�tre.
   * @param i Indice de la particule.
   * @return Particule � l'indice i.
   */
  virtual Particle *getParticle(uint i) const{
    return &m_queue[i];
  };
  
  /** Donne l'�l�ment situ� au milieu.
   * @return Pointeur sur la particule du milieu.
   */
  Particle *getMiddleParticle() const{
    return &m_queue[m_headIndex/2];
  };
  
  /** Donne la taille du squelette, sans tenir compte �ventuellement de l'origine.
   * @return nombre de particules contenues dans la file du squelette
   */
  virtual uint getInternalSize() const{
    return m_headIndex+1;
  };
  
  /** Donne la taille du squelette, sans tenir compte �ventuellement de l'origine.
   * @return nombre de particules contenues dans la file du squelette
   */
  virtual uint getSize() const{
    return m_headIndex+1;
  };
  
  /** Donne l'�l�ment en t�te de file.
   * @return particule en t�te de file
   */
  float getSelfVelocity() const{
    return m_selfVelocity;
  };
  
  /** Retourne true si le squelette ne contient plus de particules.
   * @return true sur le squelette est mort
   */
  virtual bool isDead() const{
    return (m_headIndex < 0);
  };
    
  /** Dessine le squelette � l'�cran. */
  virtual void draw () const;
  
  /** D�placement des particules du squelette et suppression des particules mortes.
   * Aucune particule n'est g�n�r�e dans cette classe.
   */
  virtual void move();
  
  /** D�place une particule dans le champ de v�locit�.
   * @param pos position de la particule
   */
  virtual bool moveParticle(Particle* const particle);

protected:
  /** Echange deux particules dans la file.
   * @param i indice de la premi�re particule
   * @param j indice de la seconde particule
   */
  void swap(uint i, uint j);
    
  /** Supprime la particule � une position donn�e.
   * @param n indice
   */
  virtual void removeParticle(uint n);
  
  /** Affectation de la position d'une particule dans l'espace.
   * @param i indice de la particule dans la file
   * @param pt nouvelle position de la particule
   */
  void updateParticle(uint i, const Point* const pt)
  {
    m_queue[i] = *pt;
    m_queue[i].decreaseLife();
  }
  
  /** Dessine les particules du squelettes 
   * @param particle Particule � dessiner.
   */
  virtual void drawParticle (Particle * const particle) const;
  
  /** Pointeur sur le solveur de fluides. */
  Field3D *m_solver;

  /** Indice de la t�te de la file. */
  int m_headIndex;
  
  /** File de particules. */
  Particle *m_queue;
  /** Velocit� propre des particules du squelette, utilis�e par les FakeFields. */
  float m_selfVelocity;
};

#define FULL_SKELETON 0
#define HALF_SKELETON 1

/** La classe Skeleton est une file de particules g�r�e avec un vecteur, mais elle 
 * transgresse n�anmoins la r�gle en permettant notamment un acc�s direct � tous les membres de la file
 * sans les supprimer.
 * L'entr�e et la sortie d'�l�ments dans la file se g�rent � l'aide des fonctions addParticle() et removeParticle().
 * Si l'entr�e est syst�matiquement en queue de file, la sortie peut s'effectuer au-del� de la t�te de file.
 * Il est �galement utile de pr�ciser que la queue de la file est mobile. Son indice prend donc 
 * des valeurs comprises dans [0;NB_PARTICLES_MAX[. La t�te de la file est donc immobile et fix�e � 0.
 * L'�l�ment en queue de file est g�n�ralement le plus pr�s de l'origine du squelette, et l'�l�ment
 * en t�te est donc le plus �loign� de l'origine. Chaque particule poss�de une dur�e de vie qui est
 * d�crement�e d�s qu'un nouvel �l�ment est ins�r�.
 *
 * @author	Flavien Bridault
 */
class Skeleton : public FreeSkeleton
{
public:
  /** Constructeur de squelette p�riph�rique libre
   * @param s Pointeur sur le solveur de fluides.
   * @param position Position de la flamme dans l'espace.
   * @param rootMoveFactor Amplitude du d�placement autoris� pour l'origine du squelette. Varie
   * en fonction du type de flamme.
   * @param pls Dur�e de vie initiale d'une particule.
   */
  Skeleton(Field3D* const s, const Point& position, const Point& rootMoveFactor, uint pls);
  /** Destructeur. */
  virtual ~Skeleton(){};
  
  void draw () const;
  
  /** D�placement des particules du squelette, g�n�ration d'une nouvelle particule et
   * suppression des particules mortes.
   */
  void move();
  virtual bool moveParticle(Particle* const particle);
  
  virtual Particle *getParticle(uint i) const{
    return &m_queue[(m_lod) ? i*2 : i];
  };
  
  virtual uint getSize() const{
    return ( (m_lod) ? (m_headIndex >> 1)+1 : m_headIndex+1);
  };
  
  /** D�placement de l'origine du squelette. */
  void moveRoot ();
  
  /** Retourne un pointeur sur l'origine du squelette.
   * @return Pointeur sur l'origine du squelette.
   */
  Point *getRoot(){
    return &m_root;
  };
  
  /* Change la valeur du niveau de d�tail du squelette 
   * @param value valeur parmi {FULL_SKELETON d�taill�, HALF_SKELETON grossier (on consid�re une particule sur 2)}
   */
  void setLOD(u_char value){ m_lod = value; };
  
  void setLifeSpan(uint lifeSpan) { m_lifeSpan = lifeSpan; };
  
protected:
  /** Ins�re une particule en queue de file.
   * @param pt position de la particule
   */
  virtual void addParticle(const Point* const pt) = 0;
  
  /** Dessine l'origine du squelette. */
  virtual void drawRoot () const;
  
  /** Origine actuelle du squelette. */
  Point m_root;
  
  /** Origine initiale du squelette. */
  Point m_rootSave;
  uint m_lifeSpan;
private:  
  /** Contient trois facteurs correctifs pour le d�placement de l'origine
   * des squelettes. Selon le type de flamme, il est en effet n�cessaire
   * que les origines se d�placent diff�remment.
   */
  Point m_rootMoveFactor;
  /** Variable correspondant au niveau de d�tail : FULL_SKELETON d�taill�, HALF_SKELETON grossier (on consid�re une particule sur 2) */
  u_char m_lod;
};

#endif

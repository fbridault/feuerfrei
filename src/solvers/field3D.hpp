#ifndef FIELD3D_HPP
#define FIELD3D_HPP

#include "field.hpp"
#include "../flames/particle.hpp"

#include <list>

enum{
  LEFT_FACE = 1,
  RIGHT_FACE,
  BACK_FACE,
  FRONT_FACE,
};

#ifdef RTFLAMES_BUILD
class IFireSource;
#endif

class Field;

/** La classe Field3D est une abstraction d'un champ de vecteur en 3D. Cette classe abstraite fournit une interface pour la
 * d�finition des champs de vecteurs, mais elle ne contient aucune structure de donn�e repr�sentant les v�locit�s. Ceci est
 * laiss� � la charge des classes filles telles que RealField3D. La s�paration est n�cessaire car la classe FakeField3D simule
 * un champ de vecteur et ne stocke pas les v�locit�s par rapport � une grille. Par contre, cette classe propose tout de m�me
 * les m�thodes de dessin de la grille et du champ de vecteurs.
 *
 *
 * @author	Flavien Bridault.
 */
class Field3D : public Field
{
public:
  /** Constructeur par d�faut n�cessaire pour l'h�ritage multiple */
  Field3D ();

  /** Constructeur du champ.
   * @param position Position du solveur de la flamme.
   * @param n_x R�solution de la grille en x.
   * @param n_y R�solution de la grille en y.
   * @param n_z R�solution de la grille en z.
   * @param dim Dimension du solveur, utilis� pour l'affichage de la flamme.
   * @param scale Echelle utilis�e pour la taille du solveur.
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   */
  Field3D (const CPoint& position, uint n_x, uint n_y, uint n_z, float dim, const CPoint& scale, float timeStep, float buoyancy);
  /** Destructeur */
  virtual ~Field3D ();

  /** Lance une it�ration du solveur. */
  virtual void iterate () = 0;

  /** R�cup�ration de la valeur des trois composantes de la v�locit� dans la grille du solveur.
   * @param pos Position relative de la particule dans le solveur
   * @param selfVelocity V�locit� propre de la particule, utilis�e seulement pour les FakeFields.
   * @return Valeurs de v�locit�.
   */
  virtual CPoint getUVW (const CPoint& pos, float selfVelocity) const = 0;

  /** D�placement d'une particule dans le champ de v�locit�. Utilis�e par les squelettes pour mettre � jour la
   * position des particules.
   * @param particle Particule � d�placer.
   * @param selfVelocity V�locit� propre de la particule, utilis�e seulement pour les FakeFields.
   */
  virtual void moveParticle (Particle& particle, float selfVelocity) const = 0;

  /** Ajout d'une force externe pour la composante U.
   * @param pos Position relative de la particule dans le solveur
   * @param value Valeur de v�locit� � ajouter.
   */
  virtual void addUsrc (const CPoint& pos, float value) = 0;

  /** Ajout d'une force externe pour la composante V.
   * @param pos Position relative de la particule dans le solveur
   * @param value Valeur de v�locit� � ajouter.
   * @param selfVelocity Valeur de v�locit� propre.
   */
  virtual void addVsrc (const CPoint& pos, float value, float& selfVelocity) = 0;

  /** Ajout d'une force externe pour la composante W.
   * @param pos Position relative de la particule dans le solveur
   * @param value Valeur de v�locit� � ajouter.
   */
  virtual void addWsrc (const CPoint& pos, float value) = 0;

  /** Retourne la dimension sous forme d'un point.
   * @return Dimension.
   */
  CPoint getDim() const { return m_dim; };

  /** Retourne la dimension en X.
   * @return Dimension.
   */
  float getDimX () const { return m_dim.x; };
  /** Retourne la dimension en Y.
   * @return Dimension.
   */
  float getDimY () const { return m_dim.y; };
  /** Retourne la dimension en Z.
   * @return Dimension.
   */
  float getDimZ () const { return m_dim.z; };

  /** Retourne le nombre de voxels en X.
   * @return Nombre de voxels.
   */
  uint getXRes() const { return m_nbVoxelsX; };

  /** Retourne le nombre de voxels en Y.
   * @return Nombre de voxels.
   */
  uint getYRes() const { return m_nbVoxelsY; };

  /** Retourne le nombre de voxels en Z.
   * @return Nombre de voxels.
   */
  uint getZRes() const { return m_nbVoxelsZ; };

  /** Retourne le facteur d'�chelle.
   * @return Echelle.
   */
  CPoint const& getScale () const { return m_scale; };

  /** Divise la r�solution de la grille par 2 */
  virtual void divideRes () {};

  /** Multiplie la r�solution de la grille par 2 */
  virtual void multiplyRes () {};

  /** Diminue la r�solution de la grille de deux voxels */
  virtual void decreaseRes () {};

  /** Augmente la r�solution de la grille de deux voxels */
  virtual void increaseRes () {};

  virtual uint getNbMaxDiv () { return m_nbMaxDiv; };

  /** Donne le type de solveur courant */
  virtual bool isRealSolver () const { return true; };
  /** Change de type de solveur � la vol�e, seulement valable pour les bi-solveurs */
  virtual void switchToRealSolver () {};
  virtual void switchToFakeField () {};

  /** Fonction de dessin du champ de densit� */
  virtual void displayDensityField (void) {};

  /** Ajoute des forces externes sur une des six faces du solveur
   * @param face identifiant de la face parmi LEFT_FACE, RIGHT_FACE, FRONT_FACE, BACK_FACE
   */
  virtual void addForcesOnFace(unsigned char face, const CPoint& BLStrength, const CPoint& TLStrength,
			       const CPoint& TRStrength, const CPoint& BRStrength) = 0;

#ifdef RTFLAMES_BUILD
  /** Ajout d'une source de feu */
  void addFireSource(IFireSource* fireSource) { m_fireSources.push_back(fireSource); };
  list <IFireSource *> *getFireSourcesList() { return &m_fireSources; };
#endif

protected:
  /** Fonction de construction de la display list de la grille du solveur */
  virtual void buildDLGrid ();

  /** Fonction de construction de la display list du rep�re du solveur */
  virtual void buildDLBase ();

  /** Fonction de dessin de la v�locit� d'une cellule */
  void displayArrow (const CVector& direction);

  /** Nombre de voxels en X sur un c�t� du cube. */
  uint m_nbVoxelsX;
  /** Nombre de voxels en Y sur un c�t� du cube. */
  uint m_nbVoxelsY;
  /** Nombre de voxels en Z sur un c�t� du cube. */
  uint m_nbVoxelsZ;

  /** Dimensions du solveur */
  CPoint m_dim;
  /** Facteur d'�chelle du solveur */
  CPoint m_scale;

  float m_nbVoxelsXDivDimX,  m_nbVoxelsYDivDimY,  m_nbVoxelsZDivDimZ;
  uint m_nbMaxDiv;
#ifdef RTFLAMES_BUILD
  /** Liste des sources de feu contenus dans le champ de v�locit�.
   * Ceci est utilis� uniquement pour garder une trace des associations champs/feux,
   * utilis�e pour les threads.
   */
  list <IFireSource *> m_fireSources;
#endif
};

#endif

#ifndef FIELD3D_HPP
#define FIELD3D_HPP

#include "field.hpp"
#include "../flames/particle.hpp"

class Field;

/** La classe Field3D est une abstraction d'un champ de vecteur en 3D. Cette classe abstraite fournit une interface pour la
 * définition des champs de vecteurs, mais elle ne contient aucune structure de donnée représentant les vélocités. Ceci est
 * laissé à la charge des classes filles telles que RealField3D. La séparation est nécessaire car la classe FakeField3D simule
 * un champ de vecteur et ne stocke pas les vélocités par rapport à une grille. Par contre, cette classe propose tout de même
 * les méthodes de dessin de la grille et du champ de vecteurs.
 * 
 * 
 * @author	Flavien Bridault.
 */
class Field3D : public Field
{
public:
  /** Constructeur par défaut nécessaire pour l'héritage multiple */
  Field3D ();
  
  /** Constructeur du champ.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param n_z Résolution de la grille en z.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param scale Echelle utilisée pour la taille du solveur.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   */
  Field3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~Field3D ();
  
  /** Lance une itération du solveur. */
  virtual void iterate () = 0;
    
  /** Récupération de la valeur des trois composantes de la vélocité dans la grille du solveur.
   * @param pos Position relative de la particule dans le solveur
   * @return Valeurs de vélocité.
   */
  virtual Point getUVW (const Point& pos, double selfVelocity) const = 0;
  
  /** Ajout d'une force externe pour la composante U.
   * @param pos Position relative de la particule dans le solveur
   * @param value Valeur de vélocité à ajouter.
   */
  virtual void addUsrc (const Point& pos, double value) = 0;
  
  /** Ajout d'une force externe pour la composante V.
   * @param pos Position relative de la particule dans le solveur
   * @param value Valeur de vélocité à ajouter.
   * @param selfVelocity Valeur de vélocité propre.
   */
  virtual void addVsrc (const Point& pos, double value, double& selfVelocity) = 0;
  
  /** Ajout d'une force externe pour la composante W.
   * @param pos Position relative de la particule dans le solveur
   * @param value Valeur de vélocité à ajouter.
   */
  virtual void addWsrc (const Point& pos, double value) = 0;
  
  /** Affectation d'une force externe pour la composante V.
   * @param pos Position relative de la particule dans le solveur
   * @param value Valeur de vélocité à ajouter.
   */
  virtual void setVsrc (const Point& pos, double value) = 0;
    
  /** Retourne la dimension sous forme d'un point.
   * @return Dimension.
   */
  Point getDim() const { return m_dim; };
  
  /** Retourne la dimension en X.
   * @return Dimension.
   */
  double getDimX () const { return m_dim.x; };
  /** Retourne la dimension en Y.
   * @return Dimension.
   */
  double getDimY () const { return m_dim.y; };
  /** Retourne la dimension en Z.
   * @return Dimension.
   */
  double getDimZ () const { return m_dim.z; };
  
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

  /** Retourne le facteur d'échelle.
   * @return Echelle.
   */
  Point getScale () const { return m_scale; };
  
  /** Ajoute de façon ponctuelle des forces externes sur une des faces du solveur.
   * @param position Nouvelle position du solveur. Détermine l'intensité de la force.
   * @param move Si true, alors le solveur est en plus déplacé à la position passée en paramètre.
   */
  virtual void addExternalForces(const Point& position, bool move) = 0;
  
  /** Divise la résolution de la grille par 2 */
  virtual void divideRes () {};
  
  /** Multiplie la résolution de la grille par 2 */
  virtual void multiplyRes () {};
  
  /** Diminue la résolution de la grille de un voxel */
  virtual void decreaseRes () {};
  
  /** Augmente la résolution de la grille de un voxel */
  virtual void increaseRes () {};
  
  /** Donne le type de solveur courant */
  virtual bool isRealSolver () const { return true; };
  /** Change de type de solveur à la volée, seulement valable pour les bi-solveurs */
  virtual void switchToRealSolver () {};
  virtual void switchToFakeField () {};
  
  /** Fonction de dessin du champ de vélocité */
  void displayVelocityField (void) = 0;
  
  /** Fonction de dessin du champ de densité */
  void displayDensityField (void) {};
  
protected:  
  /** Fonction de construction de la display list de la grille du solveur */
  void buildDLGrid ();
  
  /** Fonction de construction de la display list du repère du solveur */
  void buildDLBase ();
  
  /** Fonction de dessin de la vélocité d'une cellule */
  void displayArrow (const Vector& direction);
  
  /** Nombre de voxels en X sur un côté du cube. */
  uint m_nbVoxelsX;
  /** Nombre de voxels en Y sur un côté du cube. */
  uint m_nbVoxelsY;
  /** Nombre de voxels en Z sur un côté du cube. */
  uint m_nbVoxelsZ;
  
  /** Dimensions du solveur */
  Point m_dim;
  /** Facteur d'échelle du solveur */
  Point m_scale;
  
  double m_nbVoxelsXDivDimX,  m_nbVoxelsYDivDimY,  m_nbVoxelsZDivDimZ;
};

#endif

#ifndef FIELD3D_HPP
#define FIELD3D_HPP

#include "field.hpp"
#include "../flames/particle.hpp"

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
  Field3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~Field3D ();
  
  /** Lance une it�ration du solveur. */
  virtual void iterate () = 0;
    
  /** R�cup�ration de la valeur des trois composantes de la v�locit� dans la grille du solveur.
   * @param pos Position relative de la particule dans le solveur
   * @return Valeurs de v�locit�.
   */
  virtual Point getUVW (const Point& pos, double selfVelocity) const = 0;
  
  /** Ajout d'une force externe pour la composante U.
   * @param pos Position relative de la particule dans le solveur
   * @param value Valeur de v�locit� � ajouter.
   */
  virtual void addUsrc (const Point& pos, double value) = 0;
  
  /** Ajout d'une force externe pour la composante V.
   * @param pos Position relative de la particule dans le solveur
   * @param value Valeur de v�locit� � ajouter.
   * @param selfVelocity Valeur de v�locit� propre.
   */
  virtual void addVsrc (const Point& pos, double value, double& selfVelocity) = 0;
  
  /** Ajout d'une force externe pour la composante W.
   * @param pos Position relative de la particule dans le solveur
   * @param value Valeur de v�locit� � ajouter.
   */
  virtual void addWsrc (const Point& pos, double value) = 0;
  
  /** Affectation d'une force externe pour la composante V.
   * @param pos Position relative de la particule dans le solveur
   * @param value Valeur de v�locit� � ajouter.
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

  /** Retourne le facteur d'�chelle.
   * @return Echelle.
   */
  Point getScale () const { return m_scale; };
  
  /** Ajoute de fa�on ponctuelle des forces externes sur une des faces du solveur.
   * @param position Nouvelle position du solveur. D�termine l'intensit� de la force.
   * @param move Si true, alors le solveur est en plus d�plac� � la position pass�e en param�tre.
   */
  virtual void addExternalForces(const Point& position, bool move) = 0;
  
  /** Divise la r�solution de la grille par 2 */
  virtual void divideRes () {};
  
  /** Multiplie la r�solution de la grille par 2 */
  virtual void multiplyRes () {};
  
  /** Diminue la r�solution de la grille de un voxel */
  virtual void decreaseRes () {};
  
  /** Augmente la r�solution de la grille de un voxel */
  virtual void increaseRes () {};
  
  /** Donne le type de solveur courant */
  virtual bool isRealSolver () const { return true; };
  /** Change de type de solveur � la vol�e, seulement valable pour les bi-solveurs */
  virtual void switchToRealSolver () {};
  virtual void switchToFakeField () {};
  
  /** Fonction de dessin du champ de v�locit� */
  void displayVelocityField (void) = 0;
  
  /** Fonction de dessin du champ de densit� */
  void displayDensityField (void) {};
  
protected:  
  /** Fonction de construction de la display list de la grille du solveur */
  void buildDLGrid ();
  
  /** Fonction de construction de la display list du rep�re du solveur */
  void buildDLBase ();
  
  /** Fonction de dessin de la v�locit� d'une cellule */
  void displayArrow (const Vector& direction);
  
  /** Nombre de voxels en X sur un c�t� du cube. */
  uint m_nbVoxelsX;
  /** Nombre de voxels en Y sur un c�t� du cube. */
  uint m_nbVoxelsY;
  /** Nombre de voxels en Z sur un c�t� du cube. */
  uint m_nbVoxelsZ;
  
  /** Dimensions du solveur */
  Point m_dim;
  /** Facteur d'�chelle du solveur */
  Point m_scale;
  
  double m_nbVoxelsXDivDimX,  m_nbVoxelsYDivDimY,  m_nbVoxelsZDivDimZ;
};

#endif

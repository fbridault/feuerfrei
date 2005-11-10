#ifndef WICK_H
#define WICK_H
/***************************************************************************
 *            Wick.hpp
 *
 *  Tue Mar 29 16:23:15 2005
 *  Copyright  2005  Flavien Bridault
 *  bridault@lil.univ-littoral.fr
 ****************************************************************************/
class Wick;

#include "material.hpp"
#include "object.hpp"

class CObject;
class Solveur;

/** Classe représentant une mèche de bougie */
class Wick : public CObject
{
private:
  /**<Liste des points qui vont servir � cr�er les squelettes guides */
  vector < CPoint * >m_leadPointsArray;
  
public:
  /** Constructeur de m�che	
   * @param filename nom du fichier de sc�ne o� est stock�e la m�che
   */
  Wick(const char *filename, int nb_lead_squelettes, CScene *scene);
  virtual ~Wick();
  	
  /**
   * Lecture du nombre de points des squelettes guides.
   */
  virtual int getLeadPointsArraySize () const
  {
    return m_leadPointsArray.size ();
  };
  /**
   * Lecture d'un point sp&eacute;cifique des squelettes guides.
   * @param index indice du point &agrave; obtenir.
   * @return Un pointeur vers le point recherch&eacute;.
   */
  virtual const CPoint *getLeadPoint(int index) const
  {
    return (m_leadPointsArray[index]);
  };
  virtual vector < CPoint * > *getLeadPointsArray ()
  {
    return ( &m_leadPointsArray );
  };
};

#endif

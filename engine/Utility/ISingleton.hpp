#ifndef SINGLETON_H
#define SINGLETON_H

#include <iostream>
#include <assert.h>

/** Classe implémentant le modèle du singleton.
 *  Pour être utilisée, la classe fille doit déclarer la classe ISingleton en tant qu'amie
 *  pour avoir que ISingleton seulement ait accès aux constructeurs et destructeurs.
 */
template <typename T>
class ITSingleton
{
protected:
  /** Constructeur */
  ITSingleton () { }
  /** Destructeur */
  ~ITSingleton () { std::cout << "Destroying singleton" << std::endl; }

public:
  static void Create ()
  {
    if (m_pSingleton == NULL)
      {
        m_pSingleton = new T;
      }
    else
      {
        std::cout << "ISingleton already created!" << std::endl;
        assert(false);
      }
  }
  static T &GetInstance ()
  {
    if (m_pSingleton == NULL)
      {
        std::cout << "ISingleton not created!" << std::endl;
        assert(false);
      }
    return *(static_cast<T*> (m_pSingleton));
  }

  static void Destroy ()
  {
    if ( m_pSingleton != NULL)
      {
        delete m_pSingleton;
        m_pSingleton = NULL;
      }
  }

private:
  // Instance unique
  static T *m_pSingleton;
};

template <typename T>
T *ITSingleton<T>::m_pSingleton = NULL;

#endif

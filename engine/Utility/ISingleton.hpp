#ifndef SINGLETON_H
#define SINGLETON_H

#include <iostream>

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
  // Interface publique
  static T *getInstance ()
  {
    if (m_singleton == NULL)
      {
        m_singleton = new T;
      }
    else
      {
        std::cout << "ISingleton already created!" << std::endl;
      }

    return (static_cast<T*> (m_singleton));
  }

  static void destroy ()
  {
    if ( m_singleton != NULL)
      {
        delete m_singleton;
        m_singleton = NULL;
      }
  }

private:
  // Instance unique
  static T *m_singleton;
};

template <typename T>
T *ITSingleton<T>::m_singleton = NULL;

#endif

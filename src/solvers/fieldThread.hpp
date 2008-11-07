#ifndef FIELDTHREAD_HPP
#define FIELDTHREAD_HPP

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_THREADS
    #error "This class requires thread support!"
#endif

#include <wx/thread.h>
#include <list>

using namespace std;

#include "globalField.hpp"
#include "../flames/abstractFires.hpp"

class FieldFiresThread;
class IFireSource;

class FieldThreadsScheduler : public wxThread
{
public:
  FieldThreadsScheduler();
  virtual ~FieldThreadsScheduler();

  void Init(const list <FieldThread *> &threads);
  /* CPoint d'entrée du Thread lorsque la méthode Run() est appelée */
  virtual ExitCode Entry();
  uint getNbSolved() const { return m_nbSolved; };
  void resetNbSolved() {
    m_remainingThreadsMutex.Lock();
    m_nbSolved=0;
    m_remainingThreadsMutex.Unlock();
  };

  void Stop(){ m_run = false; };
  void forceEnd() {
    m_remainingThreadsMutex.Lock();
    m_remainingThreads=0;
    m_remainingThreadsMutex.Unlock();
    m_schedulerSem.Post();
  };
  void signalWorkEnd() {
    m_remainingThreadsMutex.Lock();
    m_remainingThreads--;
    m_nbSolved++;
    m_remainingThreadsMutex.Unlock();
    m_schedulerSem.Post();
  };
  void unblock() { m_singleMutex.Unlock(); };
private:
  list <FieldThread *> m_threads;
  uint m_remainingThreads;
  uint m_nbThreads;
  wxMutex m_remainingThreadsMutex;
  wxMutex m_singleMutex;
  wxSemaphore m_schedulerSem;
  bool m_run;
  uint m_nbSolved;
};

/** Classe de base pour gérer le processus de calcul d'un champ de vélocité. */
class FieldThread: public wxThread
{
public:
  FieldThread(FieldThreadsScheduler* const scheduler);
  virtual ~FieldThread(){};

  void Stop(){ m_run = false; };
  void Lock(){ m_mutex.Lock(); };
  void Unlock(){ m_mutex.Unlock(); };
  void AskExecAuthorization(){ m_singleExecMutex.Lock(); };
  void GiveExecAuthorization(){ m_singleExecMutex.Unlock(); };
  /** CPoint d'entrée du Thread lorsque la méthode Run() est appelée */
  virtual ExitCode Entry() = 0;
  /** Retourne le solveur contenu dans le thread. */
  virtual Field3D *getSolver() const = 0;
  virtual void drawFlames(bool displayFlame, bool displayParticles, u_char displayFlamesBoundingVolumes) = 0;
protected:
  /** CPointeur sur l'ordonnanceur. */
  FieldThreadsScheduler *m_scheduler;
  bool m_run;
private:
  /** Mutex permettant principalement de verrouiller l'accès aux flammes. */
  wxMutex m_mutex;
  /** Mutex de synchronisation de manière à ce que tous les champs ne calculent
   * qu'une seule itération chacun sur un pas de temps.
   */
  wxMutex m_singleExecMutex;
};

class GlobalFieldThread: public FieldThread
{
public:
  GlobalFieldThread(GlobalField *globalField, FieldThreadsScheduler* const scheduler);
  virtual ~GlobalFieldThread(){};

  virtual ExitCode Entry();
  /** Implémentée mais elle n'est a priori jamais appelée, car cette méthode est seulement appelée *
   * dans le constructeur du solveur global, pour identifier les solveurs locaux. */
  Field3D *getSolver() const { cerr << "error GlobalFieldThread::getSolver()" << endl; return NULL; };
  void drawFlames(bool displayFlame, bool displayParticles, u_char displayFlamesBoundingVolumes) {} ;
private:
  GlobalField *m_field;
};

class FieldFiresThread: public FieldThread
{
public:
  FieldFiresThread(Field3D *field, FieldThreadsScheduler* const scheduler);
  virtual ~FieldFiresThread(){};

  virtual ExitCode Entry();
  Field3D *getSolver() const { return m_field; };
  void drawFlames(bool displayFlame, bool displayParticles, u_char displayFlamesBoundingVolumes){
    for (list < IFireSource* >::iterator flamesIterator = m_field->getFireSourcesList()->begin ();
	 flamesIterator != m_field->getFireSourcesList()->end (); flamesIterator++)
      (*flamesIterator)->drawFlame (displayFlame, displayParticles, displayFlamesBoundingVolumes);
  }
private:
  Field3D *m_field;
};

#endif

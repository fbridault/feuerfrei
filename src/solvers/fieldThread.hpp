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

#include "field3D.hpp"

class FieldFiresThread;
class FieldFlamesThread;
class FireSource;

class FieldThreadsScheduler : public wxThread
{
public:
  FieldThreadsScheduler();		
  virtual ~FieldThreadsScheduler();
  
  void Init(const list <FieldFiresThread *> &threads, const list <FieldFlamesThread *> &extraThreads);
  /* Point d'entrée du Thread lorsque la méthode Run() est appelée */
  virtual ExitCode Entry();
  
  void Stop(){ m_run = false; };
  void singleExecLock() { m_singleExecMutex.Lock(); };
  void forceUnlock() { m_singleExecCondition.Broadcast(); };
  void signalWorkEnd() {
    m_schedulerMutex.Lock();
    m_remainingThreads--;
    m_schedulerCondition.Signal();
    m_schedulerMutex.Unlock();
    m_singleExecCondition.Wait();
    m_singleExecMutex.Unlock();
  };
private:
  list <FieldFiresThread *> m_threads;
  list <FieldFlamesThread *> m_extraThreads;
  uint m_remainingThreads;
  uint m_nbThreads;
  wxMutex m_singleExecMutex, m_schedulerMutex;
  wxCondition m_singleExecCondition, m_schedulerCondition;
  bool m_run;
};

class FieldFiresAssociation
{
public:
  FieldFiresAssociation(Field3D* f) {field = f; };
  virtual ~FieldFiresAssociation() { fireSources.clear(); };
  
  /** Ajout d'une source de feu */
  void addFireSource(FireSource* fireSource) { fireSources.push_back(fireSource); };
  Field3D *field;
  list <FireSource *> fireSources;
};

class RealFlame;

class FieldFlamesAssociation
{
public:
  FieldFlamesAssociation(Field3D* f) {field = f; };
  virtual ~FieldFlamesAssociation() { delete field; flames.clear(); };
  
  /** Ajout d'une source de feu */
  void addFlameSource(RealFlame* flame) { flames.push_back(flame); };
  Field3D *field;
  list <RealFlame *> flames;
};


class FieldFiresThread: public wxThread
{
public:
  FieldFiresThread(FieldFiresAssociation *fieldAndFires, FieldThreadsScheduler* const scheduler);
  virtual ~FieldFiresThread();
  
  /** Ajout d'une source de feu à gérer par le thread. A appeler avant de lancer le thread
   * avec la méthode Run();
   */
  void Stop(){ m_run = false; };
  void Lock(){ m_mutex.Lock(); };
  void Unlock(){ m_mutex.Unlock(); };  
  /* Point d'entrée du Thread lorsque la méthode Run() est appelée */
  virtual ExitCode Entry();
private:
  FieldFiresAssociation *m_fieldAndFires;
  FieldThreadsScheduler *m_scheduler;
  wxMutex m_mutex;
  bool m_run;
};

class FieldFlamesThread: public wxThread
{
public:
  FieldFlamesThread(FieldFlamesAssociation *fieldAndFlames, FieldThreadsScheduler* const scheduler);
  virtual ~FieldFlamesThread();
  
  /** Ajout d'une source de feu à gérer par le thread. A appeler avant de lancer le thread
   * avec la méthode Run();
   */
  void Stop(){ m_run = false; };
  void Lock(){ m_mutex.Lock(); };
  void Unlock(){ m_mutex.Unlock(); };  
  /* Point d'entrée du Thread lorsque la méthode Run() est appelée */
  virtual ExitCode Entry();
private:
  FieldFlamesAssociation *m_fieldAndFlames;
  FieldThreadsScheduler *m_scheduler;
  wxMutex m_mutex;
  bool m_run;
};
#endif

#ifndef FIELDTHREAD_HPP
#define FIELDTHREAD_HPP

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_THREADS
    #error "This class requires thread support!"
#endif // wxUSE_THREADS

#include <wx/thread.h>
#include <list>

using namespace std;

class Field3D;
class FireSource;

class FieldFlamesAssociation
{
public:
  FieldFlamesAssociation(Field3D* f) {field = f; };
  virtual ~FieldFlamesAssociation() { fireSources.clear(); };
  
  /** Ajout d'une source de feu */
  void addFireSource(FireSource* fireSource) { fireSources.push_back(fireSource); };
  Field3D *field;
  list <FireSource *> fireSources;
};

class FieldThread: public wxThread
{
public:
  FieldThread(FieldFlamesAssociation *fieldAndFlames);
  virtual ~FieldThread();
  
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
  wxMutex m_mutex;
  bool m_run;
};

#endif

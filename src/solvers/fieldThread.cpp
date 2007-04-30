#include "fieldThread.hpp"

#include "solver3D.hpp"
#include "../flames/abstractFires.hpp"

FieldThread::FieldThread(FieldFlamesAssociation *fieldAndFlames) : wxThread(wxTHREAD_JOINABLE)
{ 
  m_fieldAndFlames = fieldAndFlames;
  m_run = true;
}

FieldThread::~FieldThread()
{
}

void *FieldThread::Entry()
{ 
  wxStopWatch swatch;
  long time;
  
  while(m_run){
    /* Permet de prendre en compte Pause() et Delete() */
    if(TestDestroy())
      break;
    swatch.Start();
    m_fieldAndFlames->field->cleanSources ();
    
    for (list < FireSource* >::iterator flamesIterator = m_fieldAndFlames->fireSources.begin ();
	 flamesIterator != m_fieldAndFlames->fireSources.end (); flamesIterator++)
      (*flamesIterator)->addForces ();
    
    m_fieldAndFlames->field->iterate ();
    
    /* Il faut protéger l'accès aux flammes */
    Lock();
    for (list < FireSource* >::iterator flamesIterator = m_fieldAndFlames->fireSources.begin ();
	 flamesIterator != m_fieldAndFlames->fireSources.end (); flamesIterator++)
      (*flamesIterator)->build();
    Unlock();
    time = swatch.Time();
    
    /** Si le calcul de l'itération passe en dessous des 50Hz, on dort le temps nécessaire */
    if(time < 20)
      Sleep(20 - time);
    
    //    Yield();
  }
}

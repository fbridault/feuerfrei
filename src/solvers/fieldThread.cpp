#include "fieldThread.hpp"

#include "solver3D.hpp"
#include "../flames/abstractFires.hpp"

FieldFiresThread::FieldFiresThread(FieldFiresAssociation *fieldAndFires) : wxThread(wxTHREAD_JOINABLE)
{ 
  m_fieldAndFires = fieldAndFires;
  m_run = true;
}

FieldFiresThread::~FieldFiresThread()
{
}

void *FieldFiresThread::Entry()
{ 
  wxStopWatch swatch;
  long time;
  
  while(m_run){
    /* Permet de prendre en compte Pause() et Delete() */
    if(TestDestroy())
      break;
    swatch.Start();
    
    /** Ajouter les forces externes des FDFs */
    for (list < FireSource* >::iterator flamesIterator = m_fieldAndFires->fireSources.begin ();
	 flamesIterator != m_fieldAndFires->fireSources.end (); flamesIterator++)
      (*flamesIterator)->addForces ();
    
    m_fieldAndFires->field->iterate ();
    
    /* Il faut protéger l'accès aux flammes lors de la construction */
    Lock();
    for (list < FireSource* >::iterator flamesIterator = m_fieldAndFires->fireSources.begin ();
	 flamesIterator != m_fieldAndFires->fireSources.end (); flamesIterator++)
      (*flamesIterator)->build();
    Unlock();

    /** Nettoyer les sources de forces externes */
    m_fieldAndFires->field->cleanSources ();
    Yield();
    time = swatch.Time();
    
    /** Si le calcul de l'itération passe en dessous des 50Hz, on dort le temps nécessaire */
    if(time < 20)
      Sleep(20 - time);
  }
}

FieldFlamesThread::FieldFlamesThread(FieldFlamesAssociation *fieldAndFlames) : wxThread(wxTHREAD_JOINABLE)
{ 
  m_fieldAndFlames = fieldAndFlames;
  m_run = true;
}

FieldFlamesThread::~FieldFlamesThread()
{
}

void *FieldFlamesThread::Entry()
{ 
  wxStopWatch swatch;
  long time;
  
  while(m_run){
    /* Permet de prendre en compte Pause() et Delete() */
    if(TestDestroy())
      break;
    swatch.Start();
    m_fieldAndFlames->field->cleanSources ();
    
    for (list < RealFlame* >::iterator flamesIterator = m_fieldAndFlames->flames.begin ();
	 flamesIterator != m_fieldAndFlames->flames.end (); flamesIterator++)
      (*flamesIterator)->addForces ();
    
    m_fieldAndFlames->field->iterate ();
    
    /* Il faut protéger l'accès aux flammes */
    Lock();
    for (list < RealFlame* >::iterator flamesIterator = m_fieldAndFlames->flames.begin ();
	 flamesIterator != m_fieldAndFlames->flames.end (); flamesIterator++)
      (*flamesIterator)->build();
    Unlock();
    time = swatch.Time();
    
    /** Si le calcul de l'itération passe en dessous des 50Hz, on dort le temps nécessaire */
    if(time < 20)
      Sleep(20 - time);
    
    //    Yield();
  }
}

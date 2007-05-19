#include "fieldThread.hpp"

#include "solver3D.hpp"
#include "../flames/abstractFires.hpp"

FieldThreadsScheduler::FieldThreadsScheduler()
  : wxThread(wxTHREAD_JOINABLE), m_singleExecMutex(wxMUTEX_DEFAULT), m_schedulerMutex(wxMUTEX_DEFAULT),
    m_singleExecCondition(m_singleExecMutex), m_schedulerCondition(m_schedulerMutex)
{
  m_run = true;
  /* On bloque pour éviter le Signal() dans les threads avant le Wait() */
  m_schedulerMutex.Lock();
}

FieldThreadsScheduler::~FieldThreadsScheduler()
{
}

void FieldThreadsScheduler::Init(const list <FieldFiresThread *> &threads, 
			    const list <FieldFlamesThread *> &extraThreads)
{
  m_threads = threads;
  m_extraThreads = extraThreads;
  m_nbThreads = m_threads.size()+m_extraThreads.size();
  m_remainingThreads = m_nbThreads;
}

void *FieldThreadsScheduler::Entry()
{ 
  wxStopWatch swatch;
  long time;
  
  swatch.Start();
  while(m_run){
    /* Permet de prendre en compte Pause() et Delete() */
    if(TestDestroy())
      break;

    /* Déverrouille m_schedulerMutex et se met en attente */
    m_schedulerCondition.Wait();
    m_schedulerMutex.Unlock();    

    m_schedulerMutex.Lock();
    /* On teste si tous les threads ont terminé */
    if(!m_remainingThreads){
      time = swatch.Time();
      /* Si le calcul de l'itération passe en dessous des 50Hz, on dort le temps nécessaire */
      if(time < 20)
	Sleep(20 - time);
      
      swatch.Start();
      m_remainingThreads = m_nbThreads;
      /* Verrouillage de la condition pour être sûr que tous les threads sont en attente */
      m_singleExecMutex.Lock();
      /* On réveille tous les threads en attente */
      m_singleExecCondition.Broadcast();
      /* Déverrouillage manuel (broadcast ne déverrouille pas le mutex associé) */
      m_singleExecMutex.Unlock();
    }
  }
}

FieldFiresThread::FieldFiresThread(FieldFiresAssociation *fieldAndFires, 
				   FieldThreadsScheduler* const scheduler) : wxThread(wxTHREAD_JOINABLE)
{ 
  m_fieldAndFires = fieldAndFires;
  m_scheduler = scheduler;
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
    m_scheduler->singleExecLock();
    
    /* Ajouter les forces externes des FDFs */
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

    /* Nettoyer les sources de forces externes */
    m_fieldAndFires->field->cleanSources ();
    
    /* Indique l'achèvement du travail au scheduler, le réveille et s'endort en attendant le signal */
    m_scheduler->signalWorkEnd();
  }
}

FieldFlamesThread::FieldFlamesThread(FieldFlamesAssociation *fieldAndFlames, 
				     FieldThreadsScheduler* const scheduler) : wxThread(wxTHREAD_JOINABLE)
{ 
  m_fieldAndFlames = fieldAndFlames;
  m_scheduler = scheduler;
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
    m_scheduler->singleExecLock();
    
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
    
    /* Nettoyer les sources de forces externes */
    m_fieldAndFlames->field->cleanSources ();
    
    /* Indique l'achèvement du travail au scheduler, le réveille et s'endort en attendant le signal */
    m_scheduler->signalWorkEnd();
  }
}

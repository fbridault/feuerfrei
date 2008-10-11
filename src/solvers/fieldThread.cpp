#include "fieldThread.hpp"

#include "solver3D.hpp"
#include "../flames/abstractFires.hpp"

FieldThreadsScheduler::FieldThreadsScheduler()
  : wxThread(wxTHREAD_JOINABLE)
{
  m_run = true;
}

FieldThreadsScheduler::~FieldThreadsScheduler()
{
}

void FieldThreadsScheduler::Init(const list <FieldThread *> &threads)
{
  m_threads = threads;
  m_nbThreads = m_threads.size();
  m_remainingThreads = m_nbThreads;
  m_nbSolved = 0;
}

void *FieldThreadsScheduler::Entry()
{
  wxStopWatch swatch;
  long time;

  swatch.Start();
  m_singleMutex.Lock();
  while(m_run){
    /* Permet de prendre en compte Pause() et Delete() */
    if(TestDestroy())
      break;
//     Yield();
    /* Endort le scheduler tant qu'aucun thread n'a terminé */
    m_schedulerSem.Wait();

    /* On teste si tous les threads ont terminé */
    m_remainingThreadsMutex.Lock();
    if(!m_remainingThreads){
      time = swatch.Time();
      /* Si le calcul de l'itération passe en dessous des 50Hz, on dort le temps nécessaire */
      if(time < 33)
	Sleep(33 - time);

      swatch.Start();
      m_remainingThreads = m_nbThreads;
      m_remainingThreadsMutex.Unlock();

      m_singleMutex.Lock();
      /* Relance tous les threads */
      for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
	   threadsIterator != m_threads.end (); threadsIterator++)
	(*threadsIterator)->GiveExecAuthorization();
    }else
      m_remainingThreadsMutex.Unlock();
  }
  return (void *)NULL;
}


FieldThread::FieldThread(FieldThreadsScheduler* const scheduler) : wxThread(wxTHREAD_JOINABLE)
{
  m_scheduler = scheduler;
  m_run = true;
}

GlobalFieldThread::GlobalFieldThread(GlobalField *globalField,
				     FieldThreadsScheduler* const scheduler) : FieldThread(scheduler)
{
  m_field = globalField;
}

void *GlobalFieldThread::Entry()
{
  wxStopWatch swatch;

  while(m_run){
    /* On verrouille le mutex pour ne permettre qu'une seule exécution à la fois */
    AskExecAuthorization();

    m_field->iterate ();

    /* Nettoyer les sources de forces externes */
    m_field->cleanSources ();

    /* Indique l'achèvement du travail au scheduler */
    m_scheduler->signalWorkEnd();

//     Yield();
    /* Permet de prendre en compte Pause() et Delete() */
    if(TestDestroy())
      break;
  }
  return (void *)NULL;
}

FieldFiresThread::FieldFiresThread(Field3D *field, FieldThreadsScheduler* const scheduler) : FieldThread(scheduler)
{
  m_field = field;
}

void *FieldFiresThread::Entry()
{
  wxStopWatch swatch;

  while(m_run){
    /* On verrouille le mutex pour ne permettre qu'une seule exécution à la fois */
    AskExecAuthorization();

    /* Ajouter les forces externes des FDFs */
    for (list < FireSource* >::iterator flamesIterator = m_field->getFireSourcesList()->begin ();
	 flamesIterator != m_field->getFireSourcesList()->end (); flamesIterator++)
      (*flamesIterator)->addForces ();

    m_field->iterate ();

    /* Il faut protéger l'accès aux flammes lors de la construction */
    Lock();
    for (list < FireSource* >::iterator flamesIterator = m_field->getFireSourcesList()->begin ();
	 flamesIterator != m_field->getFireSourcesList()->end (); flamesIterator++)
      (*flamesIterator)->build();
    Unlock();

    /* Nettoyer les sources de forces externes */
    m_field->cleanSources ();

    /* Indique l'achèvement du travail au scheduler */
    m_scheduler->signalWorkEnd();

//     Yield();
    /* Permet de prendre en compte Pause() et Delete() */
    if(TestDestroy())
      break;
  }
  return (void *)NULL;
}

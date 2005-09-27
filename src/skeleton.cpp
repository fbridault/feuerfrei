#include "skeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "graphicsFn.hpp"


Skeleton::Skeleton(Solver* const s, const CPoint position, const CPoint pt)
{
  solveur = s;
  flamePos = position;
  file = new Particle[NB_PARTICULES];
  queue = -1;
  
  origine = origine_save = pt;
  particle_lifespan = LIFE_SPAN_AT_BIRTH;
  entree(&origine);
}

Skeleton::Skeleton(Solver* const s, const CPoint position, const CPoint pt, int pls)
{
  solveur = s;
  flamePos = position;
  file = new Particle[NB_PARTICULES];
  queue = -1;
  
  origine = origine_save = pt;
  particle_lifespan = pls;
  entree(&origine);
}

Skeleton::~Skeleton()
{
  free(file);
}

void Skeleton::entree(const CPoint* const pt)
{
  if(queue >= NB_PARTICULES-1){
    puts("Erreur : trop de particules");
    return;
  }
  queue++;
  
  file[queue] = *pt;
  file[queue].birth(particle_lifespan);
}

void Skeleton::sortie(int n)
{
  int i;
  
  for( i=n; i<queue ; i++)
    file[i] = file[i+1];
  
  queue--;
  if(queue==-1){
    //puts("Erreur : file vide");
    entree(&origine);
    return;
  }
}

void Skeleton::setEltFile(int i, const CPoint* const pt)
{
  file[i] = *pt;
  file[i].decreaseLife();
}

void Skeleton::swap(int i, int j)
{
  Particle tmp(file[i]);
  
  file[i] = file[j];
  
  file[j] = tmp;
}

void Skeleton::draw ()
{
  glDisable (GL_LIGHTING);

  draw_origine();
  for (int i = 0; i < getSize (); i++)
    draw_particle( getElt (i) ) ;

  glEnable (GL_LIGHTING);
}

void Skeleton::draw_origine ()
{
  CPoint position (flamePos + origine);
    
  glColor4f (1.0, 0.0, 0.25, 0.8);
  glPushMatrix ();
  glTranslatef (position.getX (), position.getY (),
		position.getZ ());
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}

void Skeleton::draw_particle (Particle * const particle)
{
  CPoint position (flamePos + *particle);

  glColor4f (1.0, 1.0, 0.25, 0.8);
  glPushMatrix ();
  glTranslatef (position.getX (), position.getY (),
		position.getZ ());
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}

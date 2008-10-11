#include "globalField.hpp"

#include "HybridSolver3D.hpp"
#include "fakeField3D.hpp"
#include "LODField3D.hpp"
#include "../scene/scene.hpp"
#include "../interface/interface.hpp"
#include "fieldThread.hpp"

#define ARGS position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy
#define ARGS_SLV ARGS, vorticityConfinement
#define ARGS_GC ARGS_SLV, omegaDiff, omegaProj, epsilon

#ifdef MULTITHREADS
GlobalField::GlobalField(const list <FieldThread *> &threads, Scene* const scene, char type, uint n,
#else
GlobalField::GlobalField(const vector <Field3D *> &fields, Scene* const scene, char type, uint n,
#endif
			 float timeStep, float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon)
{
  CPoint max, min, width, position, scale(1.0f,1.0f,1.0f);
  uint n_x, n_y, n_z;
  float dim, buoyancy=0.0f;

  scene->computeBoundingBox(max,min);
  width = max - min;
  position = min;

  if(width.x > width.y)
    if(width.x > width.z){
      dim = width.x;
      n_x = n;
      n_y = (uint)(n*width.y/dim);
      n_z = (uint)(n*width.z/dim);
    }else{
      dim = width.z;
      n_z = n;
      n_x = (uint)(n*width.x/dim);
      n_y = (uint)(n*width.y/dim);
    }
  else
    if(width.y > width.z){
      dim = width.y;
      n_y = n;
      n_x = (uint)(n*width.x/dim);
      n_z = (uint)(n*width.z/dim);
    }else{
      dim = width.z;
      n_z = n;
      n_x = (uint)(n*width.x/dim);
      n_y = (uint)(n*width.y/dim);
    }

  switch(type){
  case GS_SOLVER :
    m_field = new GSSolver3D(ARGS_SLV);
    break;
  case GCSSOR_SOLVER :
    m_field = new GCSSORSolver3D(ARGS_GC);
    break;
  case HYBRID_SOLVER :
    m_field = new HybridSolver3D(ARGS_GC);
    break;
  case LOD_HYBRID_SOLVER :
    m_field = new LODSolver3D(ARGS_GC);
    break;
  case SIMPLE_FIELD :
    m_field = new RealField3D(ARGS);
    break;
  case FAKE_FIELD :
    m_field = new FakeField3D(position, dim, scale, timeStep, buoyancy);
    break;
  case LOD_FIELD :
    m_field = new LODField3D(ARGS_GC);
    break;
  default :
    cerr << "Unknown global solver type : " << (int)type << endl;
  }
#ifdef MULTITHREADS
  /* A cet instant, la liste des processus ne contient pas encore le thread du solveur global, */
  /* on peut donc tous les ajouter à la liste des solveurs sous influence. */
  for (list < FieldThread* >::const_iterator threadIterator = threads.begin ();
       threadIterator != threads.end (); threadIterator++)
    m_localFields.push_back( (*threadIterator)->getSolver() );
#else
  for (vector < Field3D* >::const_iterator solversIterator = fields.begin ();
       solversIterator != fields.end (); solversIterator++)
    m_localFields.push_back( (*solversIterator) );
#endif
}

void GlobalField::shareForces()
{
  CPoint pt,ldim;
  CPoint strength[8];
  float dump=0.0f;

  for (vector < Field3D* >::const_iterator solversIterator = m_localFields.begin ();
       solversIterator != m_localFields.end (); solversIterator++)
    {
      /* Localisation des solveurs */
      /* On cherche dans quelle cellule se trouve chacune des six faces du solveur local */
      pt = (*solversIterator)->getPosition() - m_field->getPosition();
      ldim = (*solversIterator)->getDim();
      ldim.x *= (*solversIterator)->getScale().x;
      ldim.y *= (*solversIterator)->getScale().y;
      ldim.z *= (*solversIterator)->getScale().z;

      /* Coordonnée de la cellule du coin (0,0,0) dans le solveur global */
      strength[0] = m_field->getUVW(pt,dump);
      strength[1] = m_field->getUVW(pt+CPoint(0.0f,ldim.y,0.0f),dump);
      strength[2] = m_field->getUVW(pt+CPoint(0.0f,ldim.y,ldim.z),dump);
      strength[3] = m_field->getUVW(pt+CPoint(0.0f,0.0f,ldim.z),dump);
      strength[4] = m_field->getUVW(pt+CPoint(ldim.x,ldim.y,0.0f),dump);
      strength[5] = m_field->getUVW(pt+CPoint(ldim.x,0.0f,0.0f),dump);
      strength[6] = m_field->getUVW(pt+CPoint(ldim.x,ldim.y,ldim.z),dump);
      strength[7] = m_field->getUVW(pt+CPoint(ldim.x,0.0f,ldim.z),dump);

      (*solversIterator)->addForcesOnFace(LEFT_FACE,strength[0], strength[1], strength[2], strength[3]);
      (*solversIterator)->addForcesOnFace(BACK_FACE,strength[0], strength[1], strength[4], strength[5]);
      (*solversIterator)->addForcesOnFace(FRONT_FACE,strength[3], strength[2], strength[6], strength[7]);
      (*solversIterator)->addForcesOnFace(RIGHT_FACE,strength[7], strength[6], strength[4], strength[5]);
    }
}

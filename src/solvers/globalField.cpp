#include "globalField.hpp"

#include "HybridSolver3D.hpp"
#include "fakeField3D.hpp"
#include "LODField3D.hpp"
#include "../scene/scene.hpp"
#include "../interface/interface.hpp"

GlobalField::GlobalField(const vector < Field3D* > &localFields, Scene* const scene, char type, uint n,
			 double timeStep, double omegaDiff, double omegaProj, double epsilon)
{
  Point max, min, width, position, scale(1,1,1);
  uint n_x, n_y, n_z;
  double dim, buoyancy=0.0;
  
  m_localFields = localFields; 
  
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
      m_field = new GSSolver3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy);
      break;
    case GCSSOR_SOLVER :
      m_field = new GCSSORSolver3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy,omegaDiff, omegaProj, epsilon);
      break;
    case HYBRID_SOLVER :
      m_field = new HybridSolver3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, omegaDiff, omegaProj, epsilon);
      break;
    case LOD_HYBRID_SOLVER :
      m_field = new LODHybridSolver3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, omegaDiff, omegaProj, epsilon);
      break;
    case SIMPLE_FIELD :
      m_field = new RealField3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy);
      break;
    case FAKE_FIELD :
      m_field = new FakeField3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy);
      break;
    case LOD_FIELD :
      m_field = new LODField3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, omegaDiff, omegaProj, epsilon);
      break;
  }
}

void GlobalField::shareForces()
{
  Point pt,ldim;
  Point strength[8];
  double dump=0;

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
      strength[1] = m_field->getUVW(pt+Point(0,ldim.y,0),dump);
      strength[2] = m_field->getUVW(pt+Point(0,ldim.y,ldim.z),dump);
      strength[3] = m_field->getUVW(pt+Point(0,0,ldim.z),dump);
      strength[4] = m_field->getUVW(pt+Point(ldim.x,ldim.y,0),dump);
      strength[5] = m_field->getUVW(pt+Point(ldim.x,0,0),dump);
      strength[6] = m_field->getUVW(pt+Point(ldim.x,ldim.y,ldim.z),dump);
      strength[7] = m_field->getUVW(pt+Point(ldim.x,0,ldim.z),dump);
      
      (*solversIterator)->addForcesOnFace(LEFT_FACE,strength[0], strength[1], strength[2], strength[3]);
      (*solversIterator)->addForcesOnFace(BACK_FACE,strength[0], strength[1], strength[4], strength[5]);
      (*solversIterator)->addForcesOnFace(FRONT_FACE,strength[3], strength[2], strength[6], strength[7]);
      (*solversIterator)->addForcesOnFace(RIGHT_FACE,strength[7], strength[6], strength[4], strength[5]);
    }
}

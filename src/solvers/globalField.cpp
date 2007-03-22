#include "globalField.hpp"

#include "HybridSolver3D.hpp"
#include "fakeField3D.hpp"
#include "LODField3D.hpp"
#include "../scene/scene.hpp"
#include "../interface/interface.hpp"

GlobalField::GlobalField(Field3D **const localFields, uint nbLocalFields, Scene* const scene, char type, uint n,
			 double timeStep, double buoyancy, double omegaDiff, double omegaProj, double epsilon)
{
  Point max, min, width, position, scale(1,1,1);
  uint n_x, n_y, n_z;
  double dim;
  
  m_localFields = localFields; 
  m_nbLocalFields = nbLocalFields;
  
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

void GlobalField::addExternalForces(const Point& position, bool move)
{
}

#include "luminary.hpp"

#define LAMP_NAME "Lamp"

#include "../interface/interface.hpp"

#include "../solvers/GSSolver3D.hpp"
#include "../solvers/GCSSORSolver3D.hpp"
#include "../solvers/GCSSORSolver3D-SSE.hpp"
#include "../solvers/HybridSolver3D.hpp"
#include "../solvers/fakeField3D.hpp"
#include "../solvers/LODField3D.hpp"
#include "realFires.hpp"
#include "../scene/scene.hpp"

Luminary::Luminary (const LuminaryConfig& config, vector <Field3D *> &fields, vector <FireSource *> &fireSources,
		    Scene* const scene, const GLSLProgram * const program, const char *filename, uint index)
{
  char mtlName[255];
  Field3D *field;
  FireSource *fireSource;
  
  m_scale = config.fields[0].scale;
  /* On importe tous les objets contenus dans le fichier OBJ dont le nom commence par LAMP_NAME */
  if(scene->getMTLFileNameFromOBJ(filename, mtlName))
    scene->importMTL(mtlName);
  
  m_hasLuminary = scene->importOBJ(filename, &m_luminary, NULL, LAMP_NAME);
  
  m_position = config.position;
  
  if(m_hasLuminary)
    for (list < Object* >::iterator luminaryIterator = m_luminary.begin ();
	 luminaryIterator  != m_luminary.end (); luminaryIterator++)
      (*luminaryIterator)->buildVBO();
  
  if(config.fires[0].type != CANDLESSET)
    {
      field = initField( config.fields[0], config.position );
      fireSource = initFire( config.fires[0], filename, field, scene, index, program );
      field->addFireSource( fireSource );
      
      m_fields.push_back( field );
      fields.push_back( field );
      m_fireSources.push_back( fireSource );
      fireSources.push_back( fireSource );
    }
  else
    {
      list<Object *> objList;
      int i=0;
  
      scene->importOBJ(filename, &objList, NULL, WICK_NAME_PREFIX);
    
      for (list < Object *>::iterator objListIterator = objList.begin ();
	   objListIterator != objList.end (); objListIterator++, i++)
	{
	  Point scale(.15,.15,.15);
	  /* Construction du VBO */
	  (*objListIterator)->buildVBO();
	  (*objListIterator)->buildBoundingSpheres();
      
	  scale *= m_scale;
	  /* On décale pour que le centre du solveur soit calé avec la mèche ( et non pas le coin inférieur gauche) */
	  Point pt((*objListIterator)->getPosition());
	  pt = (pt* m_scale + m_position)  - Point(scale.x/2.0,0.0,scale.z/2.0);
	  //     pt.y += s->getDimY()/4.0 * s->getScale().y;
      
	  field = new FakeField3D(pt, 10, 10, 10, 1.0, scale, .4, .1);
	  fireSource = new Candle (config.fires[0], field, scene, i, program, .125, *objListIterator);
	  
	  fireSource->setInnerForce(config.fires[0].innerForce);
	  fireSource->setFDF(config.fires[0].fdf);
	  fireSource->setPerturbateMode(config.fires[0].flickering);
	  
	  field->addFireSource( fireSource );
	  
	  m_fields.push_back( field );
	  fields.push_back( field );
	  m_fireSources.push_back( fireSource );
	  fireSources.push_back( fireSource );
	}
    }
}

Luminary::~Luminary ()
{
  /* On efface le luminaire, il n'appartient pas à la scène */
  for (list < Object* >::iterator luminaryIterator = m_luminary.begin ();
       luminaryIterator  != m_luminary.end (); luminaryIterator++)
    delete (*luminaryIterator);
  m_luminary.clear();
}

#define ARGS position, fieldConfig.resx, fieldConfig.resy, fieldConfig.resz,\
              fieldConfig.dim, fieldConfig.scale, fieldConfig.timeStep, fieldConfig.buoyancy
#define ARGS_SLV ARGS, fieldConfig.vorticityConfinement
#define ARGS_GC ARGS_SLV, fieldConfig.omegaDiff, fieldConfig.omegaProj, fieldConfig.epsilon

Field3D* Luminary::initField(const SolverConfig& fieldConfig, const Point& position)
{
  switch(fieldConfig.type){
  case GS_SOLVER :
    return (new GSSolver3D (ARGS_SLV));
  case GCSSOR_SOLVER :
    return (new GCSSORSolver3D(ARGS_GC));
  case GCSSOR_SOLVER_SSE :
    return (new GCSSORSolver3D_SSE(ARGS_GC));
  case HYBRID_SOLVER :
    return (new HybridSolver3D(ARGS_GC));
  case LOD_HYBRID_SOLVER :
    return (new LODSolver3D(ARGS_GC));
  case SIMPLE_FIELD :
    return (new RealField3D(ARGS));
  case FAKE_FIELD :
    return (new FakeField3D(ARGS));
  case LOD_FIELD :
    return (new LODField3D(ARGS_GC));
  case LOD_HYBRID_FIELD :
    return (new LODHybridField(ARGS_GC));
  default :
    cerr << "Unknown solver type : " << (int)fieldConfig.type << endl;
    ::wxExit();
    return NULL;
  }
}


FireSource* Luminary::initFire(const FlameConfig& flameConfig, const char *fileName, Field3D* field, 
			       Scene* const scene, uint i, const GLSLProgram* const SVProgram)
{
  FireSource *fire;
  switch(flameConfig.type){
    case CANDLE :
    case CANDLESSET :
      fire = new Candle (flameConfig, field, scene, i, SVProgram, 1/ 8.0);
      break;
    case FIRMALAMPE :
      fire = new Firmalampe(flameConfig, field, scene, i, SVProgram, fileName);
      break;
    case TORCH :
      fire = new Torch(flameConfig, field, scene, fileName, i, SVProgram);
      break;
    case CAMPFIRE :
      fire = new CampFire(flameConfig, field, scene, fileName, i, SVProgram);
      break;
    case CANDLESTICK :
      fire = new CandleStick (flameConfig, field, scene, "scenes/bougie.obj", i, SVProgram, 1/ 8.0);
      break;
    default :
      cerr << "Unknown flame type : " << (int)flameConfig.type << endl;
      ::wxExit();
      return NULL;
  } 
  fire->setInnerForce(flameConfig.innerForce);
  fire->setFDF(flameConfig.fdf);
  fire->setPerturbateMode(flameConfig.flickering);
  
  return fire;
}

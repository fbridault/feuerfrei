#include "luminary.hpp"

#define LAMP_NAME "Lamp"

#include <engine/Scene/CScene.hpp>
#include <engine/Utility/UObjImporter.hpp>

#include "../interface/interface.hpp"

#include "../solvers/GSSolver3D.hpp"
#include "../solvers/GCSSORSolver3D.hpp"
#include "../solvers/GCSSORSolver3D-SSE.hpp"
#include "../solvers/HybridSolver3D.hpp"
#include "../solvers/fakeField3D.hpp"
#include "../solvers/LODField3D.hpp"
#include "realFires.hpp"
#include "wick.hpp"

Luminary::Luminary (const LuminaryConfig& a_rConfig,
										vector <Field3D *> &a_vpFields,
										vector <IFireSource *> &a_vpFireSources,
										CScene& a_rScene,
										CharCPtrC a_szFilename,
										const CShader& a_rShadowMapShader,
										const CRenderTarget& a_rShadowRenderTarget)
{
	string mtlName;
	Field3D *pField;
	IFireSource *pFireSource;
	CPoint& rScale = a_rConfig.fields[0].scale;

	/* On importe tous les objets contenus dans le fichier OBJ dont le nom commence par LAMP_NAME */
	if (UObjImporter::getMTLFileNameFromOBJ(string(a_szFilename), mtlName))
		UObjImporter::importMTL(a_rScene, mtlName);

	m_hasLuminary = UObjImporter::import(a_rScene, string(a_szFilename), m_luminary, LAMP_NAME);

	m_position = a_rConfig.position;
	if (a_rConfig.fires[0].type != CANDLESSET)
	{
		pField = initField( a_rConfig.fields[0], m_position );
		assert(pField != NULL);
		pFireSource = initFire(a_rConfig.fires[0], a_szFilename, *pField, a_rScene, a_rShadowMapShader, a_rShadowRenderTarget);
	}
	else
	{
		vector<CWick *> objList;
		int i=0;

		UObjImporter::import(a_rScene, string(a_szFilename), objList, WICK_NAME_PREFIX);

		for (vector < CWick *>::iterator objListIterator = objList.begin ();
		     objListIterator != objList.end (); objListIterator++, i++)
		{
			pField = initField( a_rConfig.fields[0], m_position);
			assert(pField != NULL);
			pFireSource = new Candle (	a_rConfig.fires[0], *pField, a_rScene, .125f, NULL,
																	a_rShadowMapShader, a_rShadowRenderTarget, *objListIterator);

			pFireSource->setInnerForce(a_rConfig.fires[0].innerForce);
			pFireSource->setFDF(a_rConfig.fires[0].fdf);
			pFireSource->setPerturbateMode(a_rConfig.fires[0].flickering);
		}
	}

	pField->addFireSource( pFireSource );
	pField->setPosition( m_position - pFireSource->GetPosition() * rScale );

	/* Add field and fire in vectors */
	m_fields.push_back( pField );
	a_vpFields.push_back( pField );
	m_fireSources.push_back( pFireSource );
	a_vpFireSources.push_back( pFireSource );
	a_rScene.addSource( pFireSource );

	if (m_hasLuminary)
		for (vector < CObject* >::iterator luminaryIterator = m_luminary.begin ();
		     luminaryIterator  != m_luminary.end (); luminaryIterator++)
		{
			a_rScene.addObject(*luminaryIterator);
			(*luminaryIterator)->SetPosition(m_position);
			(*luminaryIterator)->SetScale(rScale);
		}
}

Luminary::~Luminary ()
{
	/* On efface pas le luminaire, il appartient à la scène */
}

#define ARGS position, fieldConfig.resx, fieldConfig.resy, fieldConfig.resz,\
	fieldConfig.dim, fieldConfig.scale, fieldConfig.timeStep, fieldConfig.buoyancy
#define ARGS_SLV ARGS, fieldConfig.vorticityConfinement
#define ARGS_GC ARGS_SLV, fieldConfig.omegaDiff, fieldConfig.omegaProj, fieldConfig.epsilon

Field3D* Luminary::initField(const SolverConfig& fieldConfig, const CPoint& position)
{
	switch (fieldConfig.type)
	{
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
			return (new FakeField3D(position, fieldConfig.dim, fieldConfig.scale, fieldConfig.timeStep, fieldConfig.buoyancy));
		case LOD_FIELD :
			return (new LODField3D(ARGS_GC));
		case LOD_HYBRID_FIELD :
			return (new LODSmoothField(ARGS_GC));
		default :
			cerr << "Unknown solver type : " << (int)fieldConfig.type << endl;
			::wxExit();
			return NULL;
	}
}


IFireSource* Luminary::initFire(	const FlameConfig& a_rFlameConfig,
															CharCPtrC a_szFilename,
															Field3D& a_rField,
															CScene& a_rScene,
															const CShader& a_rShadowMapShader,
															const CRenderTarget& a_rShadowRenderTarget)
{
	IFireSource *fire;
	switch (a_rFlameConfig.type)
	{
		case CANDLE :
			fire = new Candle (a_rFlameConfig,a_rField, a_rScene, .125f, a_szFilename, a_rShadowMapShader, a_rShadowRenderTarget);
			break;
		case FIRMALAMPE :
			fire = new Firmalampe(a_rFlameConfig, a_rField, a_rScene, a_szFilename, a_rShadowMapShader, a_rShadowRenderTarget);
			break;
		case TORCH :
			fire = new CTorch(a_rFlameConfig, a_rField, a_rScene, a_szFilename, ("textures/torch6.png"),
												a_rShadowMapShader, a_rShadowRenderTarget, 0.03f, 0.04f);
			break;
		case CAMPFIRE :
			fire = new CCampFire(a_rFlameConfig, a_rField, a_rScene, a_szFilename, ("textures/torch4.png"),
														a_rShadowMapShader, a_rShadowRenderTarget, 0.05f, 0.02f);
			break;
		case CANDLESTICK :
			fire = new CandleStick (a_rFlameConfig, a_rField, a_rScene, "scenes/bougie.obj", .125f, a_rShadowMapShader, a_rShadowRenderTarget);
			break;
		default :
			cerr << "Unknown flame type : " << (int)a_rFlameConfig.type << endl;
			::wxExit();
			return NULL;
	}
	fire->setInnerForce(a_rFlameConfig.innerForce);
	fire->setFDF(a_rFlameConfig.fdf);
	fire->setPerturbateMode(a_rFlameConfig.flickering);

	return fire;
}

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

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CLuminary::CLuminary (	const LuminaryConfig& a_rConfig,
						vector <Field3D *> &a_vpFields,
						vector <IFireSource *> &a_vpFireSources,
						CSpatialGraph &a_rGraph,
						CScene& a_rScene,
						CharCPtrC a_szFilename,
						const CShader& a_rShadowMapShader,
						const CRenderTarget& a_rShadowRenderTarget)
	{
	string mtlName;
	Field3D *pField;
	IFireSource *pFireSource;

	/* Import all objects in OBJ file whose name begins with LAMP_NAME */
	if (UObjImporter::GetMTLFileNameFromOBJ(string(a_szFilename), mtlName))
		UObjImporter::importMTL(a_rScene, mtlName);

	// Create a transform node for the luminary
	m_pTransform = new CTransform(a_rConfig.position, a_rConfig.fields[0].scale);
	a_rGraph.AddTransform(m_pTransform);

	if (a_rConfig.fires[0].type != CANDLESSET)
	{
		// Create a transform node for the field
		CTransform* pFieldTransform = new CTransform();

		// Parent luminary transform to field transform
		m_pTransform->AddChild(pFieldTransform);

		pField = CreateField( a_rConfig.fields[0], *pFieldTransform );
		assert(pField != NULL);

		CPoint pt(.0f,.0f,.0f);
		//pFieldTransform->SetPosition( (pt-pFireTransform->GetLocalPosition()) * a_rConfig.fields[0].scale );

		CTransform* pFireTransform = new CTransform();
		// Parent fire transform to field transform
		pFieldTransform->AddChild(pFireTransform);

		pFireSource = CreateFire(	a_rConfig.fires[0], a_szFilename, *pField, *pFireTransform,
									a_rScene, a_rShadowMapShader, a_rShadowRenderTarget);
		pField->addFireSource( pFireSource );

	}
	else
	{
		// TODO
		assert(false);
		// Create a transform node for the luminary
		/*CTransform* m_pTransform = new CTransform();
		assert(m_pTransform != NULL);
		a_rGraph.AddTransform(m_pTransform);
		UObjImporter::import<CWick>(a_rScene, string(a_szFilename), NULL, m_pTransform, WICK_NAME_PREFIX);

		ForEachIter(itObject, CTransform::CObjectsList, rTransform.GetObjects())
		{
			// Create a transform node for the field
			CTransform* pFieldTransform = new CTransform();
			a_rGraph.addTransform(pFieldTransform);

			// Parent luminary transform to field transform
			m_pTransform->AddChild(pFieldTransform);

			pField = initField( a_rConfig.fields[0], *pFieldTransform);
			assert(pField != NULL);
			pFireSource = new Candle (	a_rConfig.fires[0], *pField, a_rGraph, a_rScene, .125f, NULL,
										a_rShadowMapShader, a_rShadowRenderTarget, *itWick);

			pFireSource->setInnerForce(a_rConfig.fires[0].innerForce);
			pFireSource->setFDF(a_rConfig.fires[0].fdf);
			pFireSource->setPerturbateMode(a_rConfig.fires[0].flickering);

			pField->addFireSource( pFireSource );

			CTransform const& rFireTransform = pFireSource->GetTransform();
			CPoint pt(.0f,.0f,.0f);
			pFieldTransform->SetPosition(  (pt-rFireTransform.GetPosition()) * a_rConfig.fields[0].scale );
		}*/
	}

	m_hasLuminary = UObjImporter::import<CObject>(a_rScene, string(a_szFilename), NULL, m_pTransform, LAMP_NAME);

	/* Add field and fire in vectors */
	m_vpFields.push_back( pField );
	a_vpFields.push_back( pField );
	m_vpFireSources.push_back( pFireSource );
	a_vpFireSources.push_back( pFireSource );
	a_rScene.addSource( &pFireSource->GrabLight() );
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CLuminary::~CLuminary ()
{
}

#define ARGS a_rTransform, fieldConfig.resx, fieldConfig.resy, fieldConfig.resz,\
	fieldConfig.dim, fieldConfig.timeStep, fieldConfig.buoyancy
#define ARGS_SLV ARGS, fieldConfig.vorticityConfinement
#define ARGS_GC ARGS_SLV, fieldConfig.omegaDiff, fieldConfig.omegaProj, fieldConfig.epsilon

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
Field3D* CLuminary::CreateField(const SolverConfig& fieldConfig, CTransform& a_rTransform)
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
			return (new FakeField3D(a_rTransform, fieldConfig.dim, fieldConfig.timeStep, fieldConfig.buoyancy));
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

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
IFireSource* CLuminary::CreateFire(	const FlameConfig& a_rFlameConfig,
									CharCPtrC a_szFilename,
									Field3D& a_rField,
									CTransform &a_rTransform,
									CScene& a_rScene,
									const CShader& a_rShadowMapShader,
									const CRenderTarget& a_rShadowRenderTarget)
{
	IFireSource *pFire;
	switch (a_rFlameConfig.type)
	{
		case CANDLE :
			pFire = new Candle (	a_rFlameConfig, a_rField, a_rTransform, a_rScene, .125f, a_szFilename,
								a_rShadowMapShader, a_rShadowRenderTarget);
			break;
		case FIRMALAMPE :
			pFire = new Firmalampe(	a_rFlameConfig, a_rField, a_rTransform, a_rScene, a_szFilename,
									a_rShadowMapShader, a_rShadowRenderTarget);
			break;
		case TORCH :
			pFire = new CTorch(	a_rFlameConfig, a_rField, a_rTransform, a_rScene, a_szFilename,
								("textures/torch6.png"), a_rShadowMapShader, a_rShadowRenderTarget, 0.03f, 0.04f);
			break;
		case CAMPFIRE :
			pFire = new CCampFire(	a_rFlameConfig, a_rField, a_rTransform, a_rScene, a_szFilename,
									("textures/torch4.png"), a_rShadowMapShader, a_rShadowRenderTarget, 0.05f, 0.02f);
			break;
		case CANDLESTICK :
			pFire = new CandleStick (a_rFlameConfig, a_rField, a_rTransform, a_rScene, "scenes/bougie.obj",
									.125f, a_rShadowMapShader, a_rShadowRenderTarget);
			break;
		default :
			cerr << "Unknown flame type : " << (int)a_rFlameConfig.type << endl;
			assert(false);
			return NULL;
	}
	pFire->setInnerForce(a_rFlameConfig.innerForce);
	pFire->setFDF(a_rFlameConfig.fdf);
	pFire->setPerturbateMode(a_rFlameConfig.flickering);

	return pFire;
}

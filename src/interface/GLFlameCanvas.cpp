#include "GLFlameCanvas.hpp"

#include "flamesFrame.hpp"
#include <iostream>

#include <engine/Shading/CRenderer.hpp>
#include <engine/Utility/UObjImporter.hpp>

BEGIN_EVENT_TABLE(GLFlameCanvas, wxGLCanvas)
	EVT_SIZE(GLFlameCanvas::OnSize)
	EVT_PAINT(GLFlameCanvas::OnPaint)
	EVT_IDLE(GLFlameCanvas::OnIdle)
	EVT_MOTION(GLFlameCanvas::OnMouseMotion)
	EVT_MIDDLE_DOWN(GLFlameCanvas::OnMouseClick)
	EVT_MIDDLE_UP(GLFlameCanvas::OnMouseClick)
	EVT_RIGHT_DOWN(GLFlameCanvas::OnMouseClick)
	EVT_RIGHT_UP(GLFlameCanvas::OnMouseClick)
	EVT_MOUSEWHEEL(GLFlameCanvas::OnMouseWheel)
	EVT_CHAR(GLFlameCanvas::OnKeyPressed)
END_EVENT_TABLE();

GLFlameCanvas::GLFlameCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList,
                             long style, const wxString& name, const wxPalette& palette) :
	wxGLCanvas(parent, id, pos, size, style, name, attribList, palette),
	m_oSceneRenderList(NRenderType::eNormal),
	m_oFlamesRenderList(NRenderType::eFx)
{
	m_bInit = false;
	m_bRun = false;
	m_pixels = new u_char[size.GetWidth()*size.GetHeight()*3];
	m_framesCount = 0;
	/* Pour éviter de faire un calcul pour ajouter des 0... */
	/* Un jour je ferais mieux, promis... */
	m_globalFramesCount = 1000000;
	m_intensities = NULL;
	m_currentConfig = NULL;
	m_globalField = NULL;
	//m_framesCountForSwitch = 0;

	m_nurbsTest = 0;

	srand(clock());

	// TODO : To be moved soon in engine !!!
	CGlowState::Create();
}

GLFlameCanvas::~GLFlameCanvas()
{
	/* On ne détruit pas les threads, la méthode FlamesFrame::OnClose() s'en occupe */
	DestroyScene();
	delete [] m_pixels;
	if ( m_intensities ) delete [] m_intensities;
	delete m_gammaEngine;
}

void GLFlameCanvas::InitUISettings(void)
{
	/* Pour l'affichage */
	m_bRun = true;
	m_saveImages = false;
	m_glowOnly = false;
 	m_displayBase = m_displayVelocity = m_displayGrid = m_displayWickBoxes = m_fullscreen = false;
	m_drawShadowVolumes = false;
	m_gammaCorrection = false;
	m_displayFlamesBoundingVolumes = 0;
}

void GLFlameCanvas::InitGL()
{
	m_width = m_currentConfig->width;
	m_height = m_currentConfig->height;

	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);

	glEnable (GL_DEPTH_TEST);
	glDisable (GL_BLEND);

	glShadeModel (GL_SMOOTH);
	glEnable (GL_LINE_SMOOTH);

	glEnable (GL_CULL_FACE);
	glCullFace (GL_BACK);

	glEnable (GL_AUTO_NORMAL);
	glEnable (GL_NORMALIZE);

	glPolygonMode(GL_FRONT,GL_FILL);

	glDisable (GL_LIGHTING);

	CShader::SetShadersDirectory("src/shaders/");
	m_gammaEngine = new CGammaFX (m_width, m_height);
	setGammaCorrection( m_currentConfig->gammaCorrection );

}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void GLFlameCanvas::InitLuminaries(void)
{
	assert(m_pScene != NULL);

	CShader::SetShadersDirectory("engine/Shading/Shaders/");
	m_shadowMapRenderTarget = new CRenderTarget("depth shadow 2D linear", 512, 512, SHADOW_MAP_TEX_UNIT);
	m_genShadowCubeMapShader = new CShader ("shadowMapVP.glsl", "shadowMapFP.glsl", "SHADOW_MAP_CUBE");

	assert(m_genShadowCubeMapShader != NULL);
	assert(m_shadowMapRenderTarget != NULL);

	for (uint i=0; i < m_currentConfig->nbLuminaries; i++)
		m_luminaries.push_back( new CLuminary(	m_currentConfig->luminaries[i],
												m_fields,
												m_fires,
												m_oSceneGraph,
												*m_pScene,
												m_currentConfig->luminaries[i].fileName.fn_str(),
												*m_genShadowCubeMapShader,
												*m_shadowMapRenderTarget));

	prevNbFlames = m_fires.size();
	prevNbFields = m_fields.size();

	if ( m_intensities ) delete [] m_intensities;
	m_intensities = new float[m_fires.size()];
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
#ifdef MULTITHREADS
void GLFlameCanvas::InitThreads()
{
	/* Création d'un thread par champ de vélocité. */
	for (vector <Field3D *>::iterator fieldsIterator = m_fields.begin ();
	        fieldsIterator != m_fields.end (); fieldsIterator++)
		m_threads.push_back(new FieldFiresThread(*fieldsIterator, m_scheduler));

	if (m_currentConfig->useGlobalField)
	{
		m_globalField = new GlobalField(m_threads, m_oSceneGraph, m_currentConfig->globalField.type,
		                                m_currentConfig->globalField.resx, m_currentConfig->globalField.timeStep,
		                                m_currentConfig->globalField.vorticityConfinement, m_currentConfig->globalField.omegaDiff,
		                                m_currentConfig->globalField.omegaProj, m_currentConfig->globalField.epsilon);
		/* Ajout du thread du solveur global le cas échéant */
		m_threads.push_back(new GlobalFieldThread(m_globalField, m_scheduler));
	}
	m_scheduler->Init(m_threads);

	if ( m_scheduler->Create() != wxTHREAD_NO_ERROR )
	{
		wxLogError(_("Can't create scheduler thread!"));
	}

	for (list <FieldThread *>::iterator threadsIterator = m_threads.begin ();
	        threadsIterator != m_threads.end (); threadsIterator++)
		/* Création proprement dite du thread, il ne reste plus qu'à le lancer avec Run() */
		if ( (*threadsIterator)->Create() != wxTHREAD_NO_ERROR )
		{
			wxLogError(_("Can't create thread!"));
		}

	/* Lancement de l'ordonnanceur */
	m_scheduler->Run();
	/* Lancement des threads */
	for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
	        threadsIterator != m_threads.end (); threadsIterator++)
		(*threadsIterator)->Run();
}
#endif

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void GLFlameCanvas::InitScene()
{
	uint glowScales[2] = { 1, 4 };

	CScene::Create();
	m_pScene = &CScene::GetInstance();

	string fileName(m_currentConfig->sceneName.fn_str());

	cout << "Chargement de la scène " << fileName << endl;
	UObjImporter::import<CObject>(*m_pScene, fileName, &m_oSceneGraph);

	InitLuminaries();

#ifdef MULTITHREADS
	/** Il faut initialiser l'ordonnanceur avant que d'éventuels threads de CandleSets ne soit instanciés */
	m_scheduler = new FieldThreadsScheduler();
#else
	if (m_currentConfig->useGlobalField)
		m_globalField = new GlobalField(m_fields, m_oSceneGraph, m_currentConfig->globalField.type,
		                                m_currentConfig->globalField.resx, m_currentConfig->globalField.timeStep,
		                                m_currentConfig->globalField.vorticityConfinement, m_currentConfig->globalField.omegaDiff,
		                                m_currentConfig->globalField.omegaProj, m_currentConfig->globalField.epsilon);
#endif
	char macro[25];
	sprintf(macro,"NBSOURCES %d\n",(int)m_fires.size());

	m_pForwardRenderer = new CForwardRenderer(*m_pScene);

	m_pScene->postInit(false);

	CCamera::Create();
	m_pCamera = &CCamera::GetInstance();
	m_pCamera->init (	m_currentConfig->camera.position,
						m_currentConfig->camera.up,
						m_currentConfig->camera.view,
						m_width, m_height, m_currentConfig->clipping, *m_pScene);

	CShader::SetShadersDirectory("src/shaders/");
	m_glowEngine  = new GlowEngine (m_width, m_height, glowScales);
	m_depthPeelingEngine = new DepthPeelingEngine(m_width, m_height, DEPTH_PEELING_LAYERS_MAX);

#ifdef MULTITHREADS
	InitThreads();
#endif
	m_swatch = new wxStopWatch();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void GLFlameCanvas::Init (FlameAppConfig *config)
{
	m_currentConfig = config;

	CRenderFlameState::Create();

	InitUISettings();
	SetCurrent();
	InitGL();

	InitScene();

	m_bInit = true;

	cout << "Initialization over" << endl;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
#ifdef MULTITHREADS
void GLFlameCanvas::PauseThreads()
{
	m_scheduler->Pause();
	for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
	        threadsIterator != m_threads.end (); threadsIterator++)
	{
		(*threadsIterator)->Pause();
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void GLFlameCanvas::ResumeThreads()
{
	m_scheduler->Resume();
	for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
	        threadsIterator != m_threads.end (); threadsIterator++)
	{
		(*threadsIterator)->Resume();
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void GLFlameCanvas::DeleteThreads()
{
	m_bInit = false;
	/* On sort les threads de leur pause si nécessaire, car sinon il serait impossible de les arrêter ! */
	if ( !IsRunning() ) setRunningState(true);

	m_scheduler->Stop();

	for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
	        threadsIterator != m_threads.end (); threadsIterator++)
		(*threadsIterator)->Stop();

	m_scheduler->forceEnd();
	m_scheduler->unblock();
	m_scheduler->Wait();

	for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
	        threadsIterator != m_threads.end (); threadsIterator++)
		(*threadsIterator)->Wait();

	delete m_scheduler;
	/* Tous les threads sont terminés, on peut tout supprimer sans risque */
	for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
	        threadsIterator != m_threads.end (); threadsIterator++)
		delete (*threadsIterator);
	m_threads.clear();
}
#endif

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void GLFlameCanvas::Restart (void)
{
	m_bRun = false;
	m_bInit = false;
	Disable();
#ifdef MULTITHREADS
	DeleteThreads();
#endif
	DestroyScene();

	m_width = m_currentConfig->width;
	m_height = m_currentConfig->height;
	glViewport (0, 0, m_width, m_height);

	InitScene();

	setNbDepthPeelingLayers(m_currentConfig->nbDepthPeelingLayers);

	m_swatch->Start();

	m_bRun = true;
	m_bInit = true;
	cout << "Initialization over" << endl;
	Enable();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void GLFlameCanvas::ReloadFieldsAndFires (void)
{
	Disable();
#ifdef MULTITHREADS
	DeleteThreads();
#endif

	delete m_pForwardRenderer;
	for (vector < IFireSource* >::iterator firesIterator = m_fires.begin ();
	        firesIterator != m_fires.end (); firesIterator++)
		delete (*firesIterator);
	m_fires.clear();

	for (vector < Field3D* >::iterator fieldsIterator = m_fields.begin ();
	        fieldsIterator != m_fields.end (); fieldsIterator++)
		delete (*fieldsIterator);
	m_fields.clear();

	for (vector < CLuminary* >::iterator luminariesIterator = m_luminaries.begin ();
	        luminariesIterator != m_luminaries.end (); luminariesIterator++)
		delete (*luminariesIterator);
	m_luminaries.clear();

	if (m_globalField)
	{
		delete m_globalField;
		m_globalField = NULL;
	}
	InitLuminaries();

	/** Il faut initialiser l'ordonnanceur avant que d'éventuels threads de CandleSets soit instanciés */
#ifdef MULTITHREADS
	m_scheduler = new FieldThreadsScheduler();
#else
	if (m_currentConfig->useGlobalField)
		m_globalField = new GlobalField(m_fields, m_oSceneGraph, m_currentConfig->globalField.type,
		                                m_currentConfig->globalField.resx, m_currentConfig->globalField.timeStep,
		                                m_currentConfig->globalField.vorticityConfinement, m_currentConfig->globalField.omegaDiff,
		                                m_currentConfig->globalField.omegaProj, m_currentConfig->globalField.epsilon);
#endif

	char macro[25];
	sprintf(macro,"NBSOURCES %d\n",(int)m_fires.size());

	m_pForwardRenderer = new CForwardRenderer(*m_pScene);

#ifdef MULTITHREADS
	InitThreads();
#endif
	m_swatch->Start();
	m_bRun = true;
	m_bInit = true;
	cout << "Initialization over" << endl;
	Enable();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void GLFlameCanvas::RegeneratePhotometricSolids(uint flameIndex, wxString IESFileName)
{
	assert(false);
//	m_fires[flameIndex]->useNewIESFile(IESFileName.ToAscii());
//	m_photoSolid->deleteTexture();
//	m_photoSolid->generateTexture();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void GLFlameCanvas::DestroyScene(void)
{
	delete m_depthPeelingEngine;
	delete m_glowEngine;
	m_pCamera->Destroy();
	m_pScene->Destroy();
	delete m_pForwardRenderer;

	// Fire sources are deleted by the scene
	m_fires.clear();

	if (m_globalField)
	{
		delete m_globalField;
		m_globalField = NULL;
	}
	for (vector < Field3D* >::iterator fieldsIterator = m_fields.begin ();
	        fieldsIterator != m_fields.end (); fieldsIterator++)
		delete (*fieldsIterator);
	m_fields.clear();

	for (vector < CLuminary* >::iterator luminariesIterator = m_luminaries.begin ();
	        luminariesIterator != m_luminaries.end (); luminariesIterator++)
		delete (*luminariesIterator);
	m_luminaries.clear();

	// Clear the graphs
	m_oSceneGraph.Clear();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void GLFlameCanvas::OnIdle(wxIdleEvent& event)
{
#ifndef MULTITHREADS
	if(m_bRun)
	{
		for (vector < IFireSource* >::iterator firesIterator = m_fires.begin ();
				firesIterator != m_fires.end (); firesIterator++)
			(*firesIterator)->addForces ();

		for (vector < Field3D* >::iterator fieldsIterator = m_fields.begin ();
				fieldsIterator != m_fields.end (); fieldsIterator++)
			(*fieldsIterator)->iterate ();

		for (vector < IFireSource* >::iterator firesIterator = m_fires.begin ();
				firesIterator != m_fires.end (); firesIterator++)
			(*firesIterator)->build();

		for (vector < Field3D* >::iterator fieldsIterator = m_fields.begin ();
				fieldsIterator != m_fields.end (); fieldsIterator++)
			(*fieldsIterator)->cleanSources ();
	}
#endif
	/* Force à redessiner */
	this->Refresh();
	//  thisApp->Yield();
	//event.RequestMore();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void GLFlameCanvas::OnMouseMotion(wxMouseEvent& event)
{
	m_pCamera->OnMouseMotion(event.GetX(), event.GetY());
}

void GLFlameCanvas::OnMouseClick(wxMouseEvent& event)
{
	NMouseButton nButton;
	NMouseButtonState nButtonState;

	switch (event.GetButton())
	{
		case wxMOUSE_BTN_LEFT :
			nButton = NMouseButton::eLeft;
			break;
		case wxMOUSE_BTN_MIDDLE :
			nButton = NMouseButton::eMiddle;
			break;
		case wxMOUSE_BTN_RIGHT :
			nButton = NMouseButton::eRight;
			break;
	}
	if (event.ButtonDown())
	{
		nButtonState = NMouseButtonState::eDown;
	}
	else
	{
		nButtonState = NMouseButtonState::eUp;
	}
	m_pCamera->OnMouseClick(nButton, nButtonState, event.GetX(), event.GetY());
}

void GLFlameCanvas::OnMouseWheel(wxMouseEvent& event)
{
	m_pCamera->moveOnFrontOrBehind(-event.GetWheelRotation()/1000.0f);
}

void GLFlameCanvas::OnKeyPressed(wxKeyEvent& event)
{
	float step=0.1f;
	switch (event.GetKeyCode())
	{
		case WXK_LEFT:
			m_pCamera->moveOnSides(step);
			break;
		case WXK_RIGHT:
			m_pCamera->moveOnSides(-step);
			break;
		case WXK_UP:
			m_pCamera->moveOnFrontOrBehind(-step);
			break;
		case WXK_DOWN:
			m_pCamera->moveOnFrontOrBehind(step);
			break;
		case WXK_HOME:
			m_pCamera->moveUpOrDown(-step);
			break;
		case WXK_END:
			m_pCamera->moveUpOrDown(step);
			break;
		case 'f':
		case 'F':
			m_fullscreen = !m_fullscreen;
			((FlamesFrame *)GetParent())->ShowFullScreen(m_fullscreen);
			break;
		case 'l':
			for (vector < Field3D* >::iterator fieldsIterator = m_fields.begin ();
			        fieldsIterator != m_fields.end (); fieldsIterator++)
				(*fieldsIterator)->decreaseRes ();
			break;
		case 'L':
			for (vector < Field3D* >::iterator fieldsIterator = m_fields.begin ();
			        fieldsIterator != m_fields.end (); fieldsIterator++)
				(*fieldsIterator)->increaseRes ();
			break;
		case WXK_SPACE :
			setRunningState(!m_bRun);
			//m_nurbsTest = 1;
			break;
		case 'R' :
			/* Fonctionnalité permettant de remettre au maximum le niveau de précision, */
			/* ceci évidemment à des fins de tests et comparaisons */
#ifdef MULTITHREADS
			for (list < FieldThread* >::iterator threadIterator = m_threads.begin ();
			        threadIterator != m_threads.end (); threadIterator++)
				(*threadIterator)->Lock();
#endif
			/* On remet à la résolution max */
			for (vector < Field3D* >::iterator fieldsIterator = m_fields.begin ();
			        fieldsIterator != m_fields.end (); fieldsIterator++)
			{
				(*fieldsIterator)->switchToRealSolver();
				for (uint i=0; i < (*fieldsIterator)->getNbMaxDiv(); i++)
					(*fieldsIterator)->increaseRes ();
			}

			for (vector < IFireSource* >::iterator firesIterator = m_fires.begin ();
			        firesIterator != m_fires.end (); firesIterator++)
				(*firesIterator)->setLOD(1);

#ifdef MULTITHREADS
			for (list < FieldThread* >::iterator threadIterator = m_threads.begin ();
			        threadIterator != m_threads.end (); threadIterator++)
				(*threadIterator)->Unlock();
#endif
			break;
	}
	event.Skip();
}

void GLFlameCanvas::OnPaint (wxPaintEvent& event)
{
	if (!m_bInit)
		return;

	wxPaintDC dc(this);

	if (!GetContext())
		return;

	SetCurrent();

	if(m_oSceneGraph.IsUpdateVisibilityNeeded())
	{
		m_oSceneGraph.ComputeVisibility();
		// Accumulate graph nodes first (performs update only if needed)
		m_oFlamesRenderList.Accumulate(m_oSceneGraph);
		m_oSceneRenderList.Accumulate(m_oSceneGraph);
	}

	if (m_bRun)
	{
		m_bVisibility = false;

		// Get display flame state
		CRenderFlameState const &rRenderState = CRenderFlameState::GetInstance();
		bool const bDisplayFlame = rRenderState.GetDisplay();

		if (bDisplayFlame)
		{
			for (vector < IFireSource* >::iterator firesIterator = m_fires.begin ();
			        firesIterator != m_fires.end (); firesIterator++)
				if ((*firesIterator)->isVisible())
				{
					m_bVisibility = true;
					break;
				}
		}
	}

	// Get display particles state
	CRenderFlameState const &rRenderState = CRenderFlameState::GetInstance();
	bool const bDisplayParticles = rRenderState.GetDisplayParticle();

	if (!m_nurbsTest)
		if (m_bVisibility || bDisplayParticles)
		{
			if (m_currentConfig->glowEnabled )
			{
				/********** RENDU DES FLAMMES AVEC LE GLOW  *******************************/
				if (m_currentConfig->depthPeelingEnabled)
				{
					/* On décortique dans les calques */
					m_depthPeelingEngine->makePeels(m_oFlamesRenderList);

					m_glowEngine->activate();

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					/* On affiche la superposition des calques que l'on vient de décortiquer */
					m_depthPeelingEngine->render(m_oFlamesRenderList);
				}
				else
				{
					m_glowEngine->activate();

					/* Dessin de la scène dans le depth buffer sans les textures pour avoir les occlusions sur les flammes */
					glClear(GL_DEPTH_BUFFER_BIT);
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);
					CDrawState &rDrawState = CDrawState::GetInstance();
					rDrawState.SetShadingFilter(NShadingFilter::eAll);
					rDrawState.SetShadingType(NShadingType::eAmbient);
					m_oSceneRenderList.Render();
					glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
					glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

					glClear(GL_COLOR_BUFFER_BIT);
					glEnable(GL_BLEND);
					glBlendFunc (GL_SRC_ALPHA, GL_ONE);
					drawFlames();
					glDisable(GL_BLEND);
				}
				CRenderFlameState &rRenderState = CRenderFlameState::GetInstance();
				rRenderState.SetDisplayBoundingVolume(NDisplayBoundingVolume::eImpostor);
				m_glowEngine->Blur(m_oFlamesRenderList);
				rRenderState.SetDisplayBoundingVolume(NDisplayBoundingVolume::eNone);

				m_glowEngine->deactivate();
			}
			else
				if (m_currentConfig->depthPeelingEnabled)
					/* On effectue l'épluchage avant d'activer le gamma car tous les deux utilisent un FBO */
					m_depthPeelingEngine->makePeels(m_oFlamesRenderList);
		}
	if (m_gammaCorrection)
		m_gammaEngine->enableGamma();

	drawScene();

	/********************* DESSINS DES FLAMMES SANS GLOW **********************************/
	if ((m_bVisibility || bDisplayParticles) && !m_currentConfig->glowEnabled )
	{
		if (m_currentConfig->depthPeelingEnabled)
			m_depthPeelingEngine->render(m_oFlamesRenderList);
		else
			if (!m_nurbsTest)
				drawFlames();
			else
				if (m_nurbsTest==1)
				{
					m_flamesDisplayList=glGenLists(1);
					glNewList(m_flamesDisplayList,GL_COMPILE);
					drawFlames();
					glEndList();
					m_nurbsTest = 2;
				}
				else
					glCallList(m_flamesDisplayList);
	}

	if ((m_bVisibility || bDisplayParticles) && m_currentConfig->glowEnabled )
	{
		CRenderFlameState &rRenderState = CRenderFlameState::GetInstance();
		rRenderState.SetDisplayBoundingVolume(NDisplayBoundingVolume::eImpostor);
		m_glowEngine->DrawBlur(m_oFlamesRenderList,m_glowOnly);
		rRenderState.SetDisplayBoundingVolume(NDisplayBoundingVolume::eNone);
	}
	if (m_gammaCorrection)
		m_gammaEngine->disableGamma();

	/* Permet d'attendre que toutes les commandes OpenGL soient bien effectuées avant de faire le rendu */
	/* La différence n'est pas forcément visible en pratique, cela supprime surtout quelques petits bugs d'affichage */
	/* potentiels, par contre le framerate en prend un sacré coup !! */
	/* Dans l'idéal, je pense donc qu'il serait utile de placer du code CPU ici ! */
	//glFinish();
	SwapBuffers ();

	//event.Skip();

	/******************** CALCUL DU FRAMERATE *************************************/
	m_framesCount++;
	m_globalFramesCount++;

	m_t = m_swatch->Time();
	if (m_t >= 2000)
	{
#ifdef MULTITHREADS
		((FlamesFrame *)GetParent())->SetFPS( m_framesCount / (m_t/1000), m_scheduler->getNbSolved() / (m_t/1000), m_width, m_height );
		m_scheduler->resetNbSolved();
#else
		((FlamesFrame *)GetParent())->SetFPS( m_framesCount / (m_t/1000), m_framesCount / (m_t/1000), m_width, m_height );
#endif
		m_swatch->Start();
		m_framesCount = 0;
	}

	if (m_saveImages)
	{
		wxString filename;
		wxString zeros;

		glReadPixels (0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, m_pixels);

		filename << _("captures/capture") << m_globalFramesCount << _(".png");
		/* Création d'une image, le dernier paramètre précise que wxImage ne doit pas détruire */
		/* le tableau de données dans son destructeur */
		wxImage image(m_width,m_height,m_pixels,true),image2;
		image2 = image.Mirror(false);
		if (!image2.SaveFile(filename,wxBITMAP_TYPE_PNG))
			cerr << "Image saving error !!" << endl;
	}
#ifdef MULTITHREADS
	m_scheduler->unblock();
#endif
}

void GLFlameCanvas::drawScene()
{
	bool additiveBlending = false; /** Active le blending qu'à partir de la seconde passe de dessin */

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/******************* AFFICHAGE DE LA SCENE *******************************/

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset (2, 2);

	//memcpy(m_modelViewMatrix,g_modelViewMatrix,16*sizeof(float));
	//Maths::InvertMatrix(m_modelViewMatrix);

	for (vector < IFireSource* >::iterator firesIterator = m_fires.begin ();
	        firesIterator != m_fires.end (); firesIterator++)
	{
		IFireSource* pFireSource = *firesIterator;
		ILight const& rLight = pFireSource->GetLight();

		if ( !rLight.IsEnabled() ) continue;

		//if (m_displayShadows)
		/** Construction de la shadowMap */
		//m_pScene->getSource(i)->castShadows(*m_pCamera, *m_pScene, &m_modelViewMatrix[0][0]);

		/** Activation du blending additif à partir de la seconde passe */
		if (additiveBlending)
		{
			glEnable (GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);
			glDepthMask(GL_FALSE);
			glDepthFunc(GL_LEQUAL);
		}

		m_pForwardRenderer->drawDirect(rLight, m_oSceneRenderList);

		if (additiveBlending)
		{
			glDisable (GL_BLEND);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
		}
		else
			additiveBlending = true;
	}
	glDisable(GL_POLYGON_OFFSET_FILL);

	/** Passe ambiante, les couleurs des matériaux ne sont récupérées qu'à ce stade */
	/** Multiplication de la couleur ambiante : par une constante pour atténuer + par la couleur source */
	glEnable (GL_BLEND);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_ZERO,GL_SRC_COLOR);

	m_pForwardRenderer->drawBrdf(m_oSceneRenderList);

	glDisable (GL_BLEND);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_ALWAYS);

	/************ Affichage des outils d'aide à la visu (grille, etc...) *********/
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (m_currentConfig->useGlobalField)
	{
		CPoint const& rPosition = m_globalField->getPosition ();

		glPushMatrix ();
		glTranslatef (rPosition.x, rPosition.y, rPosition.z);
		if (m_displayBase)
			m_globalField->displayBase();
		if (m_displayGrid)
			m_globalField->displayGrid();
		if (m_displayVelocity)
			m_globalField->displayVelocityField();
		glPopMatrix ();
	}
	for (vector < Field3D* >::iterator fieldsIterator = m_fields.begin ();
	        fieldsIterator != m_fields.end (); fieldsIterator++)
	{
		CTransform const& rTransform = (*fieldsIterator)->GetTransform();
		CPoint const& rPosition =  rTransform.GetWorldPosition ();
		CPoint const& rScale = rTransform.GetScale ();

		glPushMatrix ();
		glTranslatef (rPosition.x, rPosition.y, rPosition.z);
		glScalef (rScale.x, rScale.y, rScale.z);
		if (m_displayBase)
			(*fieldsIterator)->displayBase();
		if (m_displayGrid)
			(*fieldsIterator)->displayGrid();
		if (m_displayVelocity)
			(*fieldsIterator)->displayVelocityField();
		glPopMatrix ();
	}
	glDisable (GL_BLEND);

	glDepthFunc(GL_LESS);
}

void GLFlameCanvas::OnSize(wxSizeEvent& event)
{
	GLint w,h;
	// this is also necessary to update the context on some platforms
	wxGLCanvas::OnSize(event);

	// set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
	GetSize(&w, &h);
	m_width = (uint)w;
	m_height = (uint)h;
#ifndef __WXMOTIF__
	if (GetContext())
#endif
	{
		SetCurrent();
		glViewport(0, 0, w, h);
		/* Il faut indiquer à tous les moteurs de rendu post-process la nouvelle taille */
		m_glowEngine->setSize(m_width,m_height);
		m_depthPeelingEngine->setSize(m_width,m_height);
		m_gammaEngine->setSize(m_width,m_height);
		/* Réallocation du tableau de pixels utilisé pour sauvegarder les images */
		delete [] m_pixels;
		m_pixels = new u_char[m_width*m_height*3];
	}
	Update();
}

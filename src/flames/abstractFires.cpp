#include "abstractFires.hpp"

#include <engine/Scene/CScene.hpp>
#include <engine/Physics/CEnergy.hpp>
#include "glowengine.hpp"


/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FIRESOURCE ****************************************/
/**********************************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
IFireSource::IFireSource(	const FlameConfig& a_rFlameConfig,
							Field3D& a_rField,
							uint a_uiNbFlames,
							CharCPtrC a_szTexname,
							const CShader& a_rGenShadowCubeMapShader,
							const CRenderTarget& a_rShadowRenderTarget) :
	CFlameLight(CPoint(0.0f,0.0f,0.0f),
				CEnergy(1.0,1.0,1.0),
				512,
				a_rGenShadowCubeMapShader,
				a_rShadowRenderTarget,
				string(a_rFlameConfig.IESFileName.fn_str())),
	m_rField(a_rField),
	m_oTexture(a_szTexname, GL_REPEAT, GL_REPEAT)
{
	m_nbFlames=a_uiNbFlames;
	/* Si le tableau n'est pas initialisé par le constructeur d'une sous-classe, on le fait ici */
	if (m_nbFlames) m_flames = new IRealFlame* [m_nbFlames];

	m_intensityCoef = 20.f;
	m_visibility = true;
	m_dist=0;
	buildBoundingSphere();
	m_flickSave=-1;
	m_fluidsLODSave=15;
	m_nurbsLODSave=-1;

	computeGlowWeights(0,3.0f);
	computeGlowWeights(1,10.0f);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
IFireSource::~IFireSource()
{
	assert(m_flames != NULL);
	for (uint i = 0; i < m_nbFlames; i++)
	{
		assert(m_flames[i] != NULL);
		delete m_flames[i];
	}
	delete[]m_flames;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IFireSource::Render() const
{
#ifdef COUNT_NURBS_POLYGONS
	g_count=0;
#endif
	// Get current flame rendering style
	CRenderFlameState const& rRenderState = CRenderFlameState::GetInstance();
	NDisplayBoundingVolume const& nBoundingVolume = rRenderState.GetDisplayBoundingVolume();

	CGlowState& rGlowState = CGlowState::GetInstance();
	if(rGlowState.IsEnabled() == true)
	{
		CShaderState& rShaderState = CShaderState::GetInstance();
		uint const uiIndex = rGlowState.GetPassNumber();

		GLfloat const fDivide = getGlowDivide(uiIndex);
		GLfloat const* pfWeights = getGlowWeights(uiIndex);
		rShaderState.SetUniform1f("divide", fDivide);
		rShaderState.SetUniform1fv("weights", pfWeights, FILTER_SIZE);
	}

	switch (nBoundingVolume)
	{
		case NDisplayBoundingVolume::eSphere :
			drawBoundingSphere();
			break;
		case NDisplayBoundingVolume::eImpostor :
			drawImpostor();
			break;
		case NDisplayBoundingVolume::eNone :
			if (m_visibility)
			{
				bool const bDisplay = rRenderState.GetDisplay();
				bool const bDisplayParticle = rRenderState.GetDisplayParticle();
				ForEachUInt (i, m_nbFlames)
				{
					m_flames[i]->drawFlame(bDisplay, bDisplayParticle);
				}
			}
			break;
		default :
			assert(false);
			break;
	}
#ifdef COUNT_NURBS_POLYGONS
	cerr << g_count << endl;
#endif
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IFireSource::build()
{
	CPoint averagePos;
	CVector averageVec;

	if (!m_visibility) return;

	for (uint i = 0; i < m_nbFlames; i++)
	{
		m_flames[i]->build();
		averagePos += m_flames[i]->getCenter ();
		averageVec += m_flames[i]->getMainDirection ();
	}

	CTransform const& rFieldTransform = m_rField.GetTransform();

	averagePos *= rFieldTransform.GetScale();
	m_oCenter = averagePos/m_nbFlames;
	averagePos = m_oCenter + rFieldTransform.GetLocalPosition();

	CTransform& rLightTransform = GrabTransform();
	rLightTransform.SetPosition(averagePos);

	m_oDirection = averageVec/m_nbFlames;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IFireSource::computeIntensityPositionAndDirection()
{
	//  float r,y;
	CVector o = getMainDirection();
	CTransform const& rTransform = m_rField.GetTransform();
	CPoint const& rScale = rTransform.GetScale();

	// l'intensité est calculée à partir du rapport de la longueur de la flamme (o)
	// et de la taille en y de la grille fois un coeff correcteur
	float fIntensity = o.norm()*(rScale.y)*m_intensityCoef;

	//  m_intensity = log(m_intensity)/6.0+1;
//   m_intensity = sin(m_intensity * PI/2.0);
	/* Fonction de smoothing pour éviter d'avoir trop de fluctuation */
	fIntensity = sqrt(fIntensity);

	SetIntensity(fIntensity);
	// l'axe de rotation est dans le plan x0z perpendiculaire aux coordonnées
	// de o projeté perpendiculairement dans ce plan
//   m_axeRotation.set(-o.z,0.0,o.x);

//   // l'angle de rotation theta est la coordonnée sphérique correspondante
//   y=o.y;
//   r = (float)o.length();
//   if(r - fabs(y) < EPSILON)
//     m_orientationSPtheta = 0.0;
//   else
//     m_orientationSPtheta=acos(y / r)*180.0/M_PI;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IFireSource::buildBoundingSphere ()
{
	CPoint p;
	float t;
	CTransform const& rTransform = m_rField.GetTransform();
	CPoint const& rScale = rTransform.GetScale();

	p = (rScale * m_rField.getDim());
	t = p.max();

	m_boundingSphere.SetRadius(sqrtf(3.0f)/2.0f*t);
	/* Augmentation de 10% du rayon pour prévenir l'apparition des flammes */
//   m_boundingSphere.radius *= 1.1;
	m_boundingSphere.SetCentre(getFieldPosition() + p/2.0f);
	//  m_boundingSphere.radius = ((getMainDirection()-getCenter()).scaleBy(m_rField.getScale())).length()+.1;
	//  m_boundingSphere.centre = getCenterSP();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IFireSource::drawImpostor() const
{
	if (m_visibility)
	{
//       CPoint position(getPosition());
//       CPoint scale(m_rField.getScale());
//       glPushMatrix();
//       glTranslatef (position.x, position.y, position.z);
//       glScalef (scale.x, scale.y, scale.z);
//       CUGraphicsFn::SolidBox(CPoint(),m_rField.getDim());
//       glPopMatrix();
		GLfloat modelview[16];

		CTransform const& rTransform = m_rField.GetTransform();
		CPoint const& rPos = rTransform.GetLocalPosition();
		CPoint const& rScale = rTransform.GetScale();
		float size=rScale.x*1.5f, halfSize=rScale.x*.5f;
		CPoint a,b,c,d,zero;
		CVector right,up,offset;

		glGetFloatv (GL_MODELVIEW_MATRIX, modelview);

		offset = CVector(modelview[2], modelview[6], modelview[10])*m_rField.getDim().z*rScale.z/2.0f;

		right.x = modelview[0];
		right.y = modelview[4];
		right.z = modelview[8];

		up.x = modelview[1];
		up.y = modelview[5];
		up.z = modelview[9];

		a = rPos - right * (size * 0.5f);
		b = rPos + right * size * 0.5f;
		c = rPos + right * size * 0.5f + up * size;
		d = rPos - right * size * 0.5f + up * size;

		glPushMatrix();
		glColor3f(1.0f,1.0f,1.0f);
		glTranslatef(offset.x,offset.y,offset.z);
		glBegin(GL_QUADS);
		glVertex3f(a.x+halfSize, a.y, a.z+halfSize);
		glVertex3f(b.x+halfSize, b.y, b.z+halfSize);
		glVertex3f(c.x+halfSize, c.y, c.z+halfSize);
		glVertex3f(d.x+halfSize, d.y, d.z+halfSize);
		glEnd();
		glPopMatrix();
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IFireSource::ComputeVisibility(const CCamera &view)
{
	bool vis_save=m_visibility;
	int fluidsLOD, nurbsLOD;
	float glow1LOD, glow2LOD;
	float coverage;

	buildBoundingSphere();

	m_dist=m_boundingSphere.visibleDistance(view);
	m_visibility = (m_dist);
	if (m_visibility)
	{
		if (!vis_save)
		{
			// TODO : Could a counter to re-enable index notification
			//cerr << "solver " << m_light - GL_LIGHT0 << " launched" << endl;
			cout << "solver launched" << endl;
			m_rField.setRunningState(true);
		}

		coverage = m_boundingSphere.GetPixelCoverage(view);

		/* Fonction obtenue par régression linéaire avec les données
		 * y = [.60 .25 .05 .025 .015 .01 .001] et x = [15 13 11 9 7 5 3]
		 */
		fluidsLOD = (int)nearbyint(2.0870203f*log(coverage*2399.4418f));

		if (coverage > .75f) nurbsLOD = 5;
		else if (coverage > .2f) nurbsLOD = 4;
		else if (coverage > .1f) nurbsLOD = 3;
		else if (coverage > .003f) nurbsLOD = 2;
		else if (coverage > .0015f) nurbsLOD = 1;
		else nurbsLOD = 0;

		glow1LOD = 0.3134922f*log(coverage*11115.586f);
		glow2LOD = 1.5543804f*log(coverage*840.01981f);

		/* Fonction obtenue par régression linéaire avec les données
		 */
		//     nurbsLOD = (int)nearbyint(1.116488*log(coverage*68.271493));
		//cout << "coverage " << coverage << " " << fluidsLOD << " " << nurbsLOD << " " << glow1LOD << " " << glow2LOD << endl;

		computeGlowWeights(0,glow1LOD);
		computeGlowWeights(1,glow2LOD);

		// Changement de niveau pour les fluides
		if (fluidsLOD < m_fluidsLODSave)
		{
			do
			{
				/* On passe en FakeField */
				if ( fluidsLOD == 5 )
				{
					m_rField.switchToFakeField();
				}
				m_fluidsLODSave-=1;
			}
			while (fluidsLOD < m_fluidsLODSave);
		}
		else
			if (fluidsLOD > m_fluidsLODSave)
			{
				do
				{
					/* On repasse en solveur */
					if ( fluidsLOD == 6 )
					{
						m_rField.switchToRealSolver();
					}
					m_fluidsLODSave+=1;
				}
				while (fluidsLOD > m_fluidsLODSave);
			}

		// Changement de niveau pour les NURBS
		if (nurbsLOD != m_nurbsLODSave)
		{
			setLOD(nurbsLOD);
			m_nurbsLODSave=nurbsLOD;
		}
	}
	else
		if (vis_save)
		{
			// TODO : Could a counter to re-enable index notification
			//cerr << "solver " << m_light - GL_LIGHT0 << " launched" << endl;
			cout << "solver stopped" << endl;
			m_rField.setRunningState(false);
		}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IFireSource::computeGlowWeights(uint index, float sigma)
{
	int offset;
	m_glowDivide[index] = 0.0f;

	offset = FILTER_SIZE/2;
	for (int x=-offset ; x<=offset ; x++)
	{
		m_glowWeights[index][x+offset] = expf(-(x*x)/(sigma*sigma));
		m_glowDivide[index] += m_glowWeights[index][x+offset];
		//    cerr << x << " " << x+offset << " " << m_glowWeights[index][x+offset] << endl;
	}
	//     cerr << m_divide[index] << endl;
	m_glowDivide[index] = 1.f/m_glowDivide[index];
}


bool IFireSource::operator<(const IFireSource& other) const
{
	return (m_dist < other.m_dist);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IFireSource::setLOD(u_char value)
{
	switch (value)
	{
		case 5:
			for (uint i = 0; i < m_nbFlames; i++)
			{
				m_flames[i]->setSamplingTolerance(4);
				m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
				m_flames[i]->setFlatFlame(false);
			}
			break;
		case 4:
			for (uint i = 0; i < m_nbFlames; i++)
			{
				m_flames[i]->setSamplingTolerance(3);
				m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
				m_flames[i]->setFlatFlame(false);
			}
			break;
		case 3:
			for (uint i = 0; i < m_nbFlames; i++)
			{
				m_flames[i]->setSamplingTolerance(2);
				m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
				m_flames[i]->setFlatFlame(false);
			}
			break;
		case 2:
			for (uint i = 0; i < m_nbFlames; i++)
			{
				m_flames[i]->setSamplingTolerance(2);
				m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
				m_flames[i]->setFlatFlame(false);
			}
			break;
		case 1:
			for (uint i = 0; i < m_nbFlames; i++)
			{
				m_flames[i]->setSamplingTolerance(1);
				m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
				m_flames[i]->setFlatFlame(true);
			}
			break;
		case 0:
			for (uint i = 0; i < m_nbFlames; i++)
			{
				m_flames[i]->setSamplingTolerance(1);
				m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
				m_flames[i]->setFlatFlame(true);
			}
			break;
		default:
			cerr << "Bad NURBS LOD parameter" << endl;
	}
}


/**********************************************************************************************************************/
/******************************** IMPLEMENTATION DE LA CLASSE DETACHABLEFIRESOURCE ************************************/
/**********************************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
IDetachableFireSource::IDetachableFireSource(const FlameConfig& a_rFlameConfig,
																						Field3D& a_rField,
																						uint a_uiNbFlames,
																						CharCPtrC a_szTexname,
																						const CShader& a_rGenShadowCubeMapShader,
																						const CRenderTarget& a_rShadowRenderTarget) :
		IFireSource (a_rFlameConfig, a_rField, a_uiNbFlames, a_szTexname, a_rGenShadowCubeMapShader, a_rShadowRenderTarget)
{
	computeGlowWeights(0,1.8f);
	computeGlowWeights(1,2.2f);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
IDetachableFireSource::~IDetachableFireSource()
{
	for (list < CDetachedFlame* >::iterator flamesIterator = m_detachedFlamesList.begin ();
	        flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
		delete (*flamesIterator);
	m_detachedFlamesList.clear ();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IDetachableFireSource::drawFlame(bool display, bool displayParticle, u_char boundingVolume) const
{
#ifdef COUNT_NURBS_POLYGONS
	g_count=0;
#endif
	switch (boundingVolume)
	{
		case 1 :
			drawBoundingSphere();
			break;
		case 2 :
			drawImpostor();
			break;
		default :
			if (m_visibility)
			{
				CTransform const& rTransform = m_rField.GetTransform();
				rTransform.Push();

				for (uint i = 0; i < m_nbFlames; i++)
					m_flames[i]->drawFlame(display, displayParticle);
				for (list < CDetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
				        flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
					(*flamesIterator)->drawFlame(display, displayParticle);

				rTransform.Pop();
			}
			break;
	}
#ifdef COUNT_NURBS_POLYGONS
	cerr << g_count << endl;
#endif
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IDetachableFireSource::build()
{
	CPoint averagePos, tmp;
	CVector averageVec;
	CDetachedFlame* flame;

	if (!m_visibility) return;

	for (uint i = 0; i < m_nbFlames; i++)
	{
		m_flames[i]->breakCheck();
		m_flames[i]->build();
		averagePos += m_flames[i]->getCenter ();
		averageVec += m_flames[i]->getMainDirection ();
	}
	CTransform const& rFieldTransform = m_rField.GetTransform();
	averagePos *= rFieldTransform.GetScale();
	m_oCenter = averagePos/m_nbFlames;
	averagePos = m_oCenter + rFieldTransform.GetLocalPosition();

	CTransform &rTransform = GrabTransform();
	rTransform.SetPosition(averagePos);

	m_oDirection = averageVec/m_nbFlames;

	/* Destruction des flammes détachées en fin de vie */
	list < CDetachedFlame* >::iterator flamesIterator = m_detachedFlamesList.begin ();
	while ( flamesIterator != m_detachedFlamesList.end())
	{
		if (!(*flamesIterator)->build())
		{
			flame = *flamesIterator;
			flamesIterator = m_detachedFlamesList.erase(flamesIterator);
			delete flame;
		}
		else
			flamesIterator++;
	}

	CPoint ptMax(-FLT_MAX, -FLT_MAX, -FLT_MAX), ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
	CPoint pt;
	CPoint p;
	float t,k;
	p = (rFieldTransform.GetScale() * m_rField.getDim())/2.0f;
	t = p.max();
	k = t*t;

	/* Calcul de la bounding sphere pour les flammes détachées */
//   if( m_detachedFlamesList.size () )
//     for (list < CDetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
// 	 flamesIterator != m_detachedFlamesList.end();  flamesIterator++){
//       pt = (*flamesIterator)->getTop();
//       if(pt.x > ptMax.x)
// 	ptMax.x = pt.x;
//       if(pt.y > ptMax.y)
// 	ptMax.y = pt.y;
//       if(pt.z > ptMax.z)
// 	ptMax.z = pt.z;
//       pt = (*flamesIterator)->getBottom();
//       if(pt.x < ptMin.x)
//       ptMin.x = pt.x;
//       if(pt.y < ptMin.y)
// 	ptMin.y = pt.y;
//       if(pt.z < ptMin.z)
// 	ptMin.z = pt.z;
//       ptMin *= m_rField.getScale();
//       ptMax *= m_rField.getScale();
//       m_boundingSphere.radius = (sqrt(k+k) + ptMax.distance(ptMin));
//       m_boundingSphere.centre = m_rField.getPosition() + (p + (ptMax + ptMin)/2.0f)/2.0f;
//     }else{
	m_boundingSphere.SetRadius(sqrt(k+k));
	m_boundingSphere.SetCentre(rFieldTransform.GetLocalPosition() + p);
//   }
	/* Calcul de la bounding box pour affichage */
	buildBoundingBox ();
}

// void IDetachableFireSource::drawImpostor() const
// {
//   if(m_visibility)
//     {
//       GLfloat modelview[16];

//       CPoint pos(m_rField.getPosition());
//       float size=m_rField.getScale().x*1.1;
//       CPoint a,b,c,d,zero;
//       CVector right,up,offset;//(0.0f,0.0f,0.5f);

//       glGetFloatv (GL_MODELVIEW_MATRIX, modelview);
//       offset = CVector(modelview[2], modelview[6], modelview[10])*.5f*size;

//       right.x = modelview[0];
//       right.y = modelview[4];
//       right.z = modelview[8];

//       up.x = modelview[1];
//       up.y = modelview[5];
//       up.z = modelview[9];

//       a = pos - right * (size * 0.5f);
//       b = pos + right * size * 0.5f;
//       c = pos + right * size * 0.5f + up * size;
//       d = pos - right * size * 0.5f + up * size;

//       glPushMatrix();
//       glColor3f(1.0f,1.0f,1.0f);
//       glTranslatef(offset.x,offset.y,offset.z);
//       glBegin(GL_QUADS);
//       glVertex3f(a.x+0.5f, a.y, a.z+0.5f);
//       glVertex3f(b.x+0.5f, b.y, b.z+0.5f);
//       glVertex3f(c.x+0.5f, c.y, c.z+0.5f);
//       glVertex3f(d.x+0.5f, d.y, d.z+0.5f);
//       glEnd();
//       glPopMatrix();
//     }
// }

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IDetachableFireSource::setSmoothShading (bool state)
{
	IFireSource::setSmoothShading(state);
	for (list < CDetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
	        flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
		(*flamesIterator)->setSmoothShading(state);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IDetachableFireSource::computeIntensityPositionAndDirection()
{
	CVector o = getMainDirection();
	float fIntensity;

	CTransform const& rFieldTransform = m_rField.GetTransform();

	// l'intensité est calculée à partir du rapport de la longueur de la flamme (o)
	// et de la taille en y de la grille fois un coeff correcteur
	fIntensity=o.norm()*(rFieldTransform.GetScale().y)*m_intensityCoef;

	/* Fonction de smoothing pour éviter d'avoir trop de fluctuation */
	fIntensity = sqrt(fIntensity)*2.0f;

	SetIntensity(fIntensity);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IDetachableFireSource::ComputeVisibility(const CCamera &view)
{
	bool vis_save=m_visibility;
	int nurbsLOD, fluidsLOD;
	float glow1LOD, glow2LOD;
	float coverage;

	buildBoundingSphere();

	m_dist=m_boundingSphere.visibleDistance(view);
	m_visibility = (m_dist);

	if (m_visibility)
	{
		if (!vis_save)
		{
			// TODO : Could a counter to re-enable index notification
			//cerr << "solver " << m_light - GL_LIGHT0 << " launched" << endl;
			cerr << "solver launched" << endl;
			m_rField.setRunningState(true);
		}

		coverage = m_boundingSphere.GetPixelCoverage(view);

		/* Fonction obtenue par régression linéaire avec les données
		 * y = [.60 .25 .05 .025 .015 .01 .001] et x = [15 13 11 9 7 5 3]
		 */
		fluidsLOD = (int)nearbyint(2.0870203*log(coverage*2399.4418));

		if (fluidsLOD < 5) fluidsLOD=5;
		else if (fluidsLOD > 15) fluidsLOD=15;

		if (coverage > .9f) nurbsLOD = 5;
		else if (coverage > .5f) nurbsLOD = 4;
		else if (coverage > .2f) nurbsLOD = 3;
		else if (coverage > .05f) nurbsLOD = 2;
		else if (coverage > .01f) nurbsLOD = 1;
		else nurbsLOD = 0;

		glow1LOD = 0.3134922*log(coverage*11115.586);
		glow2LOD = 1.5543804*log(coverage*840.01981);
		//     nurbsLOD = (int)nearbyint(1.0731832*log(coverage*54.470523));
		//cout << "coverage " << coverage << " " << fluidsLOD << " " << nurbsLOD << endl;

		computeGlowWeights(0,glow1LOD);
		computeGlowWeights(1,glow2LOD);

		if (fluidsLOD < m_fluidsLODSave)
		{
			int diff = (m_fluidsLODSave - fluidsLOD)/2;
			while (diff > 0 )
			{
				/* On passe en FakeField */
				if ( fluidsLOD == 5 && m_fluidsLODSave==7)
				{
					if (getPerturbateMode() != FLICKERING_NOISE)
					{
						m_flickSave = getPerturbateMode();
						setPerturbateMode(FLICKERING_NOISE);
					}
					m_rField.switchToFakeField();
				}
				else
					m_rField.decreaseRes();
				diff--;
				m_fluidsLODSave-=2;
			}
		}
		else
			if (fluidsLOD > m_fluidsLODSave)
			{
				int diff = (fluidsLOD - (m_fluidsLODSave-1))/2;

				while (diff > 0 )
				{
					/* On passe en FakeField */
					if ( fluidsLOD == 6 && (m_fluidsLODSave-1)==4)
					{
						if (m_flickSave > -1)
						{
							setPerturbateMode(m_flickSave);
							m_flickSave = -1;
						}
						m_rField.switchToRealSolver();
					}
					else
						m_rField.increaseRes();
					diff--;
					m_fluidsLODSave+=2;
				}
			}

		// Changement de niveau pour les NURBS
		if (nurbsLOD != m_nurbsLODSave)
		{
			setLOD(nurbsLOD);
			m_nurbsLODSave=nurbsLOD;
		}
	}
	else
		if (vis_save)
		{
			// TODO : Could a counter to re-enable index notification
			//cerr << "solver " << m_light - GL_LIGHT0 << " launched" << endl;
			cerr << "solver stopped" << endl;
			m_rField.setRunningState(false);
		}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IDetachableFireSource::buildBoundingBox ()
{
	CPoint ptMax(-FLT_MAX, -FLT_MAX, -FLT_MAX), ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
	CPoint pt;

	if ( m_detachedFlamesList.size () )
		for (list < CDetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
		        flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
		{
			pt = (*flamesIterator)->getTop();
			if (pt.x > ptMax.x)
				ptMax.x = pt.x;
			if (pt.y > ptMax.y)
				ptMax.y = pt.y;
			if (pt.z > ptMax.z)
				ptMax.z = pt.z;
			pt = (*flamesIterator)->getBottom();
			if (pt.x < ptMin.x)
				ptMin.x = pt.x;
			if (pt.y < ptMin.y)
				ptMin.y = pt.y;
			if (pt.z < ptMin.z)
				ptMin.z = pt.z;
		}
	pt = m_rField.getDim();
	if (pt.x > ptMax.x)
		ptMax.x = pt.x;
	if (pt.y > ptMax.y)
		ptMax.y = pt.y;
	if (pt.z > ptMax.z)
		ptMax.z = pt.z;
	if (0.0f < ptMin.x)
		ptMin.x = 0.0f;
	if (0.0f < ptMin.y)
		ptMin.y = 0.0f;
	if (0.0f < ptMin.z)
		ptMin.z = 0.0f;

	CTransform const& rFieldTransform = m_rField.GetTransform();
	m_BBmin = ptMin * rFieldTransform.GetScale();
	m_BBmax = ptMax * rFieldTransform.GetScale();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void IDetachableFireSource::setLOD(u_char value)
{
	u_char tolerance=4;
	switch (value)
	{
		case 5:
			for (uint i = 0; i < m_nbFlames; i++)
			{
				m_flames[i]->setSamplingTolerance(4);
				m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
				m_flames[i]->setFlatFlame(false);
				tolerance = 4;
			}
			break;
		case 4:
			for (uint i = 0; i < m_nbFlames; i++)
			{
				m_flames[i]->setSamplingTolerance(3);
				m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
				m_flames[i]->setFlatFlame(false);
				tolerance = 3;
			}
			break;
		case 3:
			for (uint i = 0; i < m_nbFlames; i++)
			{
				m_flames[i]->setSamplingTolerance(2);
				m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
				m_flames[i]->setFlatFlame(false);
			}
			tolerance = 2;
			break;
		case 2:
			for (uint i = 0; i < m_nbFlames; i++)
			{
				m_flames[i]->setSamplingTolerance(1);
				m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
				m_flames[i]->setFlatFlame(false);
			}
			tolerance = 1;
			break;
		case 1:
		case 0:
			for (uint i = 0; i < m_nbFlames; i++)
			{
				m_flames[i]->setSamplingTolerance(1);
				m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
				m_flames[i]->setFlatFlame(true);
			}
			tolerance = 1;
			break;
		default:
			cerr << "Bad NURBS LOD parameter" << endl;
	}
	for (list < CDetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
			flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
		(*flamesIterator)->setSamplingTolerance(tolerance);
}

#include "abstractFlames.hpp"

#include <wx/intl.h>

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE NURBSFLAME ****************************************/
/**********************************************************************************************************************/

NurbsFlame::NurbsFlame(FlameConfig* flameConfig, uint nbSkeletons, ushort nbFixedPoints, Texture* const tex)
{
  m_nbSkeletons = nbSkeletons;
  m_nbFixedPoints = nbFixedPoints;
  
  m_uorder = 4;
  m_vorder = 4;  
  
  /* Allocation des tableaux à la taille maximale pour les NURBS, */
  /* ceci afin d'éviter des réallocations trop nombreuses */
  m_ctrlPoints =  new GLfloat[(NB_PARTICLES_MAX + m_nbFixedPoints) * (m_nbSkeletons + m_uorder - 1) * 3];
  m_texPoints =  new GLfloat[(NB_PARTICLES_MAX + m_nbFixedPoints) * (m_nbSkeletons + m_uorder - 1) * 2];
  m_uknots = new GLfloat[m_uorder + m_nbSkeletons + m_uorder - 1];
  m_vknots = new GLfloat[m_vorder + NB_PARTICLES_MAX + m_nbFixedPoints];
  m_texTmp = new GLfloat[(NB_PARTICLES_MAX + m_nbFixedPoints)];
  
  /* Initialisation des pointeurs de sauvegarde des tableaux */
  m_ctrlPointsSave = m_ctrlPoints;
  m_texPointsSave = m_texPoints;
  m_texTmpSave = m_texTmp;
  
  m_nurbs = gluNewNurbsRenderer();
  gluNurbsProperty(m_nurbs, GLU_SAMPLING_TOLERANCE, flameConfig->samplingTolerance);
  gluNurbsProperty(m_nurbs, GLU_DISPLAY_MODE, GLU_FILL);
  gluNurbsProperty(m_nurbs, GLU_NURBS_MODE,GLU_NURBS_TESSELLATOR);
  /* Important : ne fait pas la facettisation si la NURBS n'est pas dans le volume de vision */
  gluNurbsProperty(m_nurbs, GLU_CULLING, GL_TRUE);
  gluNurbsCallback(m_nurbs, GLU_NURBS_ERROR, (void(*)())nurbsError);
  
  gluNurbsCallback(m_nurbs, GLU_NURBS_BEGIN_DATA, (void(*)())NurbsBegin);
  gluNurbsCallback(m_nurbs, GLU_NURBS_VERTEX, (void(*)())NurbsVertex);
  gluNurbsCallback(m_nurbs, GLU_NURBS_NORMAL, (void(*)())NurbsNormal);
  gluNurbsCallback(m_nurbs, GLU_NURBS_TEXTURE_COORD, (void(*)())NurbsTexCoord);
  gluNurbsCallback(m_nurbs, GLU_NURBS_END_DATA, (void(*)())NurbsEnd);
  
  m_toggle=false;
  gluNurbsCallbackData(m_nurbs,(GLvoid *)&m_toggle);
  
  m_position=flameConfig->position;
  
  m_flameConfig = flameConfig;

  m_tex = tex;
}

NurbsFlame::NurbsFlame(NurbsFlame* const source, uint nbSkeletons, ushort nbFixedPoints, Texture* const tex)
{  
  m_nbSkeletons = nbSkeletons;
  m_nbFixedPoints = nbFixedPoints;
  
  m_uorder = 4;
  m_vorder = 4;
  
  /* Allocation des tableaux à la taille maximale pour les NURBS, */
  /* ceci afin d'éviter des réallocations trop nombreuses */
  m_ctrlPoints =  new GLfloat[(NB_PARTICLES_MAX + m_nbFixedPoints) * (m_nbSkeletons + m_uorder - 1) * 3];
  m_texPoints =  new GLfloat[(NB_PARTICLES_MAX + m_nbFixedPoints) * (m_nbSkeletons + m_uorder - 1) * 2];
  m_uknots = new GLfloat[m_uorder + m_nbSkeletons + m_uorder - 1];
  m_vknots = new GLfloat[m_vorder + NB_PARTICLES_MAX + m_nbFixedPoints];
  m_texTmp = new GLfloat[(NB_PARTICLES_MAX + m_nbFixedPoints)];
  
  /* Initialisation des pointeurs de sauvegarde des tableaux */
  m_ctrlPointsSave = m_ctrlPoints;
  m_texPointsSave = m_texPoints;
  m_texTmpSave = m_texTmp;
  
  m_nurbs = gluNewNurbsRenderer();
  gluNurbsProperty(m_nurbs, GLU_SAMPLING_TOLERANCE, source->m_flameConfig->samplingTolerance);
  gluNurbsProperty(m_nurbs, GLU_DISPLAY_MODE, GLU_FILL);
  gluNurbsProperty(m_nurbs, GLU_NURBS_MODE,GLU_NURBS_TESSELLATOR);
  /* Important : ne fait pas la facettisation si la NURBS n'est pas dans le volume de vision */
  gluNurbsProperty(m_nurbs, GLU_CULLING, GL_TRUE);
  gluNurbsCallback(m_nurbs, GLU_NURBS_ERROR, (void(*)())nurbsError);
  
  gluNurbsCallback(m_nurbs, GLU_NURBS_BEGIN_DATA, (void(*)())NurbsBegin);
  gluNurbsCallback(m_nurbs, GLU_NURBS_VERTEX, (void(*)())NurbsVertex);
  gluNurbsCallback(m_nurbs, GLU_NURBS_NORMAL, (void(*)())NurbsNormal);
  gluNurbsCallback(m_nurbs, GLU_NURBS_TEXTURE_COORD, (void(*)())NurbsTexCoord);
  gluNurbsCallback(m_nurbs, GLU_NURBS_END_DATA, (void(*)())NurbsEnd);
  
  m_toggle=false;
  gluNurbsCallbackData(m_nurbs,(GLvoid *)&m_toggle);
  
  m_position=source->m_position;

  m_tex = tex;
}

NurbsFlame::~NurbsFlame()
{
  gluDeleteNurbsRenderer(m_nurbs);
  
  delete[]m_ctrlPoints;
  delete[]m_uknots;
  delete[]m_vknots;
  delete[]m_texPoints;
  delete[]m_texTmp;
}

void NurbsFlame::toggleSmoothShading()
{
  m_toggle = !m_toggle;
}

void NurbsFlame::drawNurbs ()
{
  gluBeginSurface (m_nurbs);
  gluNurbsSurface (m_nurbs, m_uknotsCount, m_uknots, m_vknotsCount, m_vknots, (m_maxParticles + m_nbFixedPoints) * 2,
		   2, m_texPoints, m_uorder, m_vorder, GL_MAP2_TEXTURE_COORD_2);
  gluNurbsSurface (m_nurbs, m_uknotsCount, m_uknots, m_vknotsCount, m_vknots, (m_maxParticles + m_nbFixedPoints) * 3,
		   3, m_ctrlPoints, m_uorder, m_vorder, GL_MAP2_VERTEX_3);
  gluEndSurface (m_nurbs);
}

void NurbsFlame::drawLineFlame ()
{
  if (m_toggle)
    {
      glColor4f (1.0, 1.0, 1.0, 1.0);      
      drawNurbs ();
    }
  else
    {
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable (GL_TEXTURE_2D);
      glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      m_tex->bind();
      
      drawNurbs ();
      
      glDisable (GL_TEXTURE_2D);
    }
}

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FIXEDFLAME ****************************************/
/**********************************************************************************************************************/

FixedFlame::FixedFlame(FlameConfig* flameConfig, uint nbSkeletons, ushort nbFixedPoints, Texture* const tex) :
  NurbsFlame(flameConfig, nbSkeletons, nbFixedPoints, tex),
  m_halo(_("textures/halo.png"), GL_CLAMP, GL_CLAMP)
{
}

FixedFlame::~FixedFlame()
{
}

void FixedFlame::drawHalo (double angle)
{
  double h = getTop()->distance(*getBottom());
  Point *top = getTop();
  Point *bottom = getBottom();
  
  glActiveTextureARB(GL_TEXTURE0_ARB);
  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  m_halo.bind ();
      
  glPushMatrix ();
  
  glRotatef (angle , 0.0, 1.0, 0.0);    

  /* On effectue une interpolation du mouvement en x et en z */
  double x1, x2, x3, x4;
  if(angle > 0)
    if(angle < 90){
      x1 = ((bottom->x - h) * (90 - angle) - (bottom->z + h) * angle)/180;
      x2 = ((bottom->x + h) * (90 - angle) - (bottom->z - h) * angle)/180;
      x3 = ((top->x + h) * (90 - angle) - (top->z - h) * angle)/180;
      x4 = ((top->x - h) * (90 - angle) - (top->z + h) * angle)/180;
    }else{
      x1 = ((bottom->x + h) * (90 - angle) - (bottom->z + h) * (180-angle))/180;
      x2 = ((bottom->x - h) * (90 - angle)  - (bottom->z - h) * (180-angle))/180;
      x3 = ((top->x - h) * (90 - angle) - (top->z - h) * (180-angle))/180;
      x4 = ((top->x + h) * (90 - angle) - (top->z + h) * (180-angle))/180;
    }
  else
    if(angle > -90){
      x1 = ((bottom->x - h) * (90 + angle) + (bottom->z - h) * -angle)/180;
      x2 = ((bottom->x + h) * (90 + angle) + (bottom->z + h) * -angle)/180;
      x3 = ((top->x + h) * (90 + angle) + (top->z + h) * -angle)/180;
      x4 = ((top->x - h) * (90 + angle) + (top->z - h) * -angle)/180;
	  
    }else{
      x1 = ((bottom->x + h) * (90 + angle) + (bottom->z - h) * (180+angle))/180;
      x2 = ((bottom->x - h) * (90 + angle) + (bottom->z + h) * (180+angle))/180;
      x3 = ((top->x - h) * (90 + angle) + (top->z + h) * (180+angle))/180;
      x4 = ((top->x + h) * (90 + angle) + (top->z - h) * (180+angle))/180;
    }
  
  glColor4f(0.0,0.0,0.0,0.0);
  glBegin(GL_QUADS);
      
  glTexCoord2f(0.0,0.0);
  glVertex3f(x1,bottom->y,0.0);
      
  glTexCoord2f(1.0,0.0);
  glVertex3f(x2,bottom->y,0.0);
      
  glTexCoord2f(1.0,1.0);
  glVertex3f(x3,top->y+h/3,0.0);
      
  glTexCoord2f(0.0,1.0);
  glVertex3f(x4,top->y+h/3,0.0);
      
  glEnd();

  glPopMatrix();
}

void FixedFlame::drawPointFlame ()
{
  if (m_toggle)
    {
      glColor4f (1.0, 1.0, 1.0, 1.0);      
      drawNurbs ();
    }
  else
    {
      double angle, angle2, angle3, angle4;
  
      /* Déplacement de la texture de maniÃ¨re Ã  ce qu'elle reste "en face" de l'observateur */
      GLdouble m[4][4];
  
      glGetDoublev (GL_MODELVIEW_MATRIX, &m[0][0]);
  
      /* Position de la bougie = translation dans la matrice courante */
      Vector bougiepos(m[3][0], m[3][1], m[3][2]);
  
      /* Position de l'axe de regard de bougie dans le repère du monde = axe initial * Matrice de rotation */
      /* Attention, ne pas prendre la translation en plus !!!! */
      Vector worldLookAt(m[2][0], m[2][1], m[2][2]);
      Vector worldLookX(m[0][0], m[0][1], m[0][2]);
      Vector direction(-bougiepos.x, 0.0, -bougiepos.z);
  
      direction.normalize ();
      /* Apparemment, pas besoin de le normaliser, on laisse pour le moment */
      worldLookAt.normalize ();
      worldLookX.normalize ();
  
      angle = -acos (direction * worldLookAt);
      angle2 = acos (direction * worldLookX);
      
      /****************************************************************************************/
      /* Génération du halo */
      angle4 = (angle2 < PI / 2.0) ? -angle : angle;
      angle3 = angle4 * 180 / (double) (PI);
  
      glEnable (GL_TEXTURE_2D);
      drawHalo(angle3);
      /****************************************************************************************/
      /* Affichage de la flamme */
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      m_tex->bind();
      
      glMatrixMode (GL_TEXTURE);
      glPushMatrix ();
      glLoadIdentity ();
      
      glTranslatef (0.0, angle4 / (double) (PI), 0.0);
  
      drawNurbs();
  
      glPopMatrix();
  
      glMatrixMode (GL_MODELVIEW);
  
      glDisable (GL_TEXTURE_2D);
    }
}

/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE REALFLAME ****************************************/
/**********************************************************************************************************************/

RealFlame::RealFlame(FlameConfig* flameConfig, uint nbSkeletons, ushort nbFixedPoints, Texture* const tex, Field3D *s) :
  FixedFlame (flameConfig, nbSkeletons, nbFixedPoints, tex)
{  
  m_distances = new double[NB_PARTICLES_MAX - 1 + m_nbFixedPoints];
  m_maxDistancesIndexes = new int[NB_PARTICLES_MAX - 1 + m_nbFixedPoints];
  
  m_periSkeletons = new PeriSkeleton* [m_nbSkeletons];
  for (uint i = 0; i < m_nbSkeletons; i++)
    m_periSkeletons[i]=NULL;
  
  m_solver = s;
  m_innerForce = flameConfig->innerForce;
  m_perturbateCount=0;
  
  locateInField();
}

bool RealFlame::build ()
{
  uint i, j, l;
  double vinc, vtmp, vtex;
  m_maxParticles = 0;
  vtex = -0.5;
  
  for (i = 0; i < m_nbLeadSkeletons; i++)
    m_leadSkeletons[i]->move ();
  
  /* Déplacement et détermination du maximum */
  for (i = 0; i < m_nbSkeletons; i++)
    {
      m_periSkeletons[i]->move ();
      if (m_periSkeletons[i]->getSize () > m_maxParticles)
	m_maxParticles = m_periSkeletons[i]->getSize ();
    }
  
  m_size = m_maxParticles + m_nbFixedPoints;
  
  vinc = 1.0 / (double)(m_size-1);
  vtmp = 0;
  for (i = 0; i < m_size; i++){
    m_texTmp[i] = vtmp;
    vtmp += vinc;
  }
  
  /* Direction des u */
  for (i = 0; i < m_nbSkeletons; i++)
    {
      vtex += .5;
      /* Problème pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
      /* La solution retenue va ajouter des points de contrôles là où les points de contrôles sont le plus éloignés */
      if (m_periSkeletons[i]->getSize () < m_maxParticles)
	{
	  // Nombre de points de contrôle supplémentaires
	  uint nb_pts_supp = m_maxParticles - m_periSkeletons[i]->getSize ();
	  Point pt;
	  /* On calcule les distances entre les particules successives */
	  /* On prend également en compte l'origine du squelette ET les extrémités du guide */
	  /* On laisse les distances au carré pour des raisons évidentes de coût de calcul */	  
	  m_distances[0] = 
	    m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0)->squaredDistanceFrom (m_periSkeletons[i]->getParticle (0));
	  
	  for (j = 0; j < m_periSkeletons[i]->getSize () - 1; j++)
	    m_distances[j + 1] = 
	      m_periSkeletons[i]->getParticle (j)->squaredDistanceFrom(m_periSkeletons[i]->getParticle (j + 1));
	  
	  m_distances[m_periSkeletons[i]->getSize ()] = 
	    m_periSkeletons[i]->getLastParticle ()->squaredDistanceFrom (m_periSkeletons[i]->getRoot ());
	  m_distances[m_periSkeletons[i]->getSize () + 1] =
	    m_periSkeletons[i]->getRoot()->squaredDistanceFrom(m_periSkeletons[i]->getLeadSkeleton ()->getRoot ());
	  
	  /* On cherche les indices des distances max */
	  /* On n'effectue pas un tri complet car on a seulement besoin de connaître les premiers */
	  for (l = 0; l < nb_pts_supp; l++)
	    {
	      double dist_max = FLT_MIN;
	      
	      for (j = 0; j < m_periSkeletons[i]->getSize () + 2; j++)
		{
		  if (m_distances[j] > dist_max)
		    {
		      m_maxDistancesIndexes[l] = j;
		      dist_max = m_distances[j];
		    }
		}
	      /* Il n'y a plus de place */
	      if (dist_max == FLT_MIN)
		m_maxDistancesIndexes[l] = -1;
	      else
		/* On met à la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
		m_distances[m_maxDistancesIndexes[l]] = 0;
	    }
	  /* Les particules les plus écartées sont maintenant connues, on peut passer à l'affichage */
	  /* Remplissage des points de contrôle */
	  setCtrlPoint (m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0), vtex);
	  
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == 0)
	      {
		pt = Point::pointBetween(m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0), m_periSkeletons[i]->getParticle (0));
		setCtrlPoint (&pt, vtex);
	      }
	  
	  for (j = 0; j < m_periSkeletons[i]->getSize () - 1; j++)
	    {
	      /* On regarde s'il ne faut pas ajouter un point */
	      for (l = 0; l < nb_pts_supp; l++)
		{
		  if (m_maxDistancesIndexes[l] == (int)j + 1)
		    {
		      /* On peut référencer j+1 puisque normalement, m_maxDistancesIndexes[l] != j si j == m_periSkeletons[i]->getSize()-1 */
		      pt = Point::pointBetween(m_periSkeletons[i]->getParticle (j), m_periSkeletons[i]->getParticle (j + 1));
		      setCtrlPoint (&pt, vtex);
		    }
		}
	      setCtrlPoint (m_periSkeletons[i]->getParticle (j), vtex);
	    }
	  
	  setCtrlPoint (m_periSkeletons[i]->getLastParticle (), vtex);
	  
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == (int)m_periSkeletons[i]->getSize ())
	      {
		pt = Point::pointBetween(m_periSkeletons[i]->getRoot (), m_periSkeletons[i]-> getLastParticle ());
		setCtrlPoint (&pt, vtex);
	      }
	  
	  setCtrlPoint (m_periSkeletons[i]->getRoot (), vtex);
	  
	  bool prec = false;
	  
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == (int)m_periSkeletons[i]->getSize () + 1)
	      {
		pt = Point::pointBetween(m_periSkeletons[i]->getRoot (),
					 m_periSkeletons[i]->getLeadSkeleton()->getRoot ());
		setCtrlPoint (&pt, vtex);
		prec = true;
	      }
	  
	  /* Points supplémentaires au cas où il n'y a "plus de place" ailleurs entre les particules */
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == -1)
	      {
		if (!prec)
		  {
		    pt = *m_periSkeletons[i]-> getRoot ();
		  }
		pt = Point::pointBetween (&pt, m_periSkeletons[i]->getLeadSkeleton()->getRoot ());
		setCtrlPoint (&pt, vtex);
		prec = true;
	      }
	  setCtrlPoint (m_periSkeletons[i]->getLeadSkeleton ()->getRoot (), vtex);
	}
      else
	{
	  /* Cas sans problème */
	  /* Remplissage des points de contrôle */
	  setCtrlPoint (m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0), vtex);
	  for (j = 0; j < m_periSkeletons[i]->getSize (); j++)
	    {
	      setCtrlPoint (m_periSkeletons[i]->getParticle (j), vtex);
	    }
	  setCtrlPoint (m_periSkeletons[i]->getRoot (), vtex);
	  setCtrlPoint (m_periSkeletons[i]->getLeadSkeleton ()->getRoot (), vtex);
	}
      m_texTmp = m_texTmpSave;
    }
  
  /* On recopie les m_uorder squelettes pour fermer la NURBS */
  GLfloat *startCtrlPoints = m_ctrlPointsSave;
  for (i = 0; i < ((m_uorder-1)*m_size)*3; i++)
    *m_ctrlPoints++ = *startCtrlPoints++;
  m_ctrlPoints = m_ctrlPointsSave;
  
  GLfloat *startTexPoints = m_texPointsSave;
  for (i = 0; i < ((m_uorder-1)*m_size)*2; i++)
    *m_texPoints++ = *startTexPoints++;  
  m_texPoints = m_texPointsSave;
  
  /* Affichage en NURBS */
  m_uknotsCount = m_nbSkeletons + m_uorder + m_uorder - 1;
  m_vknotsCount = m_maxParticles + m_vorder + m_nbFixedPoints;
  
  for (i = 0; i < m_uknotsCount; i++)
    m_uknots[i] = i;
  
  for (j = 0; j < m_vknotsCount; j++)
    m_vknots[j] = (j < m_vorder) ? 0 : (j - m_vorder + 1);
  
  for (j = m_vknotsCount - m_vorder; j < m_vknotsCount; j++)
    m_vknots[j] = m_vknots[m_vknotsCount - m_vorder];
  
  m_vknots[m_vorder] += 0.9;
  
  if (m_vknots[m_vorder] > m_vknots[m_vorder + 1])
    for (j = m_vorder + 1; j < m_vknotsCount; j++)
      m_vknots[j] += 1;

  return true;
}

RealFlame::~RealFlame()
{
  for (uint i = 0; i < m_nbSkeletons; i++)
    delete m_periSkeletons[i];
  delete[]m_periSkeletons;
  
  for (uint i = 0; i < m_nbLeadSkeletons; i++)
    delete m_leadSkeletons[i];
  delete[]m_leadSkeletons;
  
  delete[]m_distances;
  delete[]m_maxDistancesIndexes;
}

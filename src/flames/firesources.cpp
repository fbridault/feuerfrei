#include "firesources.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"

#include <vector>
#include <string>

#define WICK_NAME_PREFIX "Wick"
#define TORCH_NAME "Torch"

Candle::Candle (FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index, 
		CgSVShader * shader, double rayon):
  FireSource (flameConfig, s, 1, scene, filename, index, shader)
	       //   cgCandleVertexShader (_("bougieShader.cg"),_("vertCandle"),context),
	       //   cgCandleFragmentShader (_("bougieShader.cg"),_("fragCandle"),context)
{
  m_flames[0] = new PointFlame(flameConfig, s, rayon);
}

Firmalampe::Firmalampe(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index,
		       CgSVShader * shader, const char *wickFileName):
  FireSource (flameConfig, s, 1, scene, filename, index, shader)
{
  m_flames[0] = new LineFlame( flameConfig, scene, _("textures/firmalampe.png"), s, wickFileName);
}

Torch::Torch(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *torchName, uint index,
	     CgSVShader * shader):
  FireSource (flameConfig, s, 0, scene, torchName, index, shader, TORCH_NAME)
{
  vector<string> objList;
  int i=0;
  
  scene->getObjectsNameFromOBJ(torchName, objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new BasicFlame* [m_nbFlames];
  
  for (vector < string >::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame( flameConfig, scene, _("textures/torch5.png"), s, torchName, (*objListIterator).c_str());
    }
}

CampFire::CampFire(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *fireName, uint index, 
		   CgSVShader * shader):
  FireSource (flameConfig, s, 0, scene, fireName, index, shader, TORCH_NAME),
  m_halo(_("textures/halo.png"), GL_CLAMP, GL_CLAMP)
{
  vector<string> objList;
  int i=0;
  
  scene->getObjectsNameFromOBJ(fireName, objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new BasicFlame* [m_nbFlames];
  
  for (vector < string >::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame(flameConfig, scene, _("textures/torch4.png"), s, fireName, (*objListIterator).c_str());
    }
}


void CampFire::drawFlame(bool displayParticle)
{
  Point pt(m_solver->getPosition());

  glPushMatrix();
  glTranslatef (pt.x, pt.y, pt.z);

  glScalef (m_solver->getDimX(), m_solver->getDimY(), m_solver->getDimZ());

//       double angle, angle2, angle3, angle4;
      
//       /* Déplacement de la texture de maniÃ¨re Ã  ce qu'elle reste "en face" de l'observateur */
//       GLdouble m[4][4];

//       glGetDoublev (GL_MODELVIEW_MATRIX, &m[0][0]);

//       /* Position de la bougie = translation dans la matrice courante */
//       Vector bougiepos(m[3][0], m[3][1], m[3][2]);

//       /* Position de l'axe de regard de bougie dans le repère du monde = axe initial * Matrice de rotation */
//       /* Attention, ne pas prendre la translation en plus !!!! */
//       Vector worldLookAt(m[2][0], m[2][1], m[2][2]);
//       Vector worldLookX(m[0][0], m[0][1], m[0][2]);
//       Vector direction(-bougiepos.x, 0.0, -bougiepos.z);

//       direction.normalize ();
//       /* Apparemment, pas besoin de le normaliser, on laisse pour le moment */
//       worldLookAt.normalize ();
//       worldLookX.normalize ();
      
//       angle = -acos (direction * worldLookAt);
//       angle2 = acos (direction * worldLookX);
            
//       glActiveTextureARB(GL_TEXTURE0_ARB);
//       glEnable (GL_TEXTURE_2D);
//       /****************************************************************************************/
//       /* Génération du halo */
//       angle4 = (angle2 < PI / 2.0) ? -angle : angle;
//       angle3 = angle4 * 180 / (double) (PI);
      
//       glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//       glBindTexture (GL_TEXTURE_2D, m_halo.getTexture ());
      
//       glPushMatrix ();
      

//       double h = 0;
//       Point top;
//       Point bottom;
//       for (uint i = 0; i < m_nbFlames; i++){
// 	h += m_flames[i]->getTop()->distance(*m_flames[i]->getBottom());
// 	top += *m_flames[i]->getTop();
// 	bottom += *m_flames[i]->getBottom();
//       }
      
//       h /= m_nbFlames;
//       top = top / m_nbFlames;
//       bottom = bottom / m_nbFlames;
      
//       glRotatef (angle3 , 0.0, 1.0, 0.0);    

//       /* On effectue une interpolation du mouvement en x et en z */
//       double x1, x2, x3, x4;
//       if(angle3 > 0)
// 	if(angle3 < 90){
// 	  x1 = ((bottom.x - h) * (90 - angle3) - (bottom.z + h) * angle3)/180;
// 	  x2 = ((bottom.x + h) * (90 - angle3) - (bottom.z - h) * angle3)/180;
// 	  x3 = ((top.x + h) * (90 - angle3) - (top.z - h) * angle3)/180;
// 	  x4 = ((top.x - h) * (90 - angle3) - (top.z + h) * angle3)/180;
// 	}else{
// 	  x1 = ((bottom.x + h) * (90 - angle3) - (bottom.z + h) * (180-angle3))/180;
// 	  x2 = ((bottom.x - h) * (90 - angle3)  - (bottom.z - h) * (180-angle3))/180;
// 	  x3 = ((top.x - h) * (90 - angle3) - (top.z - h) * (180-angle3))/180;
// 	  x4 = ((top.x + h) * (90 - angle3) - (top.z + h) * (180-angle3))/180;
// 	}
//       else
// 	if(angle3 > -90){
// 	  x1 = ((bottom.x - h) * (90 + angle3) + (bottom.z - h) * -angle3)/180;
// 	  x2 = ((bottom.x + h) * (90 + angle3) + (bottom.z + h) * -angle3)/180;
// 	  x3 = ((top.x + h) * (90 + angle3) + (top.z + h) * -angle3)/180;
// 	  x4 = ((top.x - h) * (90 + angle3) + (top.z - h) * -angle3)/180;
	  
// 	}else{
// 	  x1 = ((bottom.x + h) * (90 + angle3) + (bottom.z - h) * (180+angle3))/180;
// 	  x2 = ((bottom.x - h) * (90 + angle3) + (bottom.z + h) * (180+angle3))/180;
// 	  x3 = ((top.x - h) * (90 + angle3) + (top.z + h) * (180+angle3))/180;
// 	  x4 = ((top.x + h) * (90 + angle3) + (top.z - h) * (180+angle3))/180;
// 	}
      
// //       glColor3f(1,1,1);
//       glBegin(GL_QUADS);
//       glTexCoord2f(0.0,0.0);
//       glVertex3f(x1,bottom.y,0.0);
      
//       glTexCoord2f(1.0,0.0);
//       glVertex3f(x2,bottom.y,0.0);
      
//       glTexCoord2f(1.0,1.0);
//       glVertex3f(x3,top.y+h/2,0.0);
      
//       glTexCoord2f(0.0,1.0);
//       glVertex3f(x4,top.y+h/2,0.0);
      
//       glEnd();

//       glPopMatrix();
//       glDisable (GL_TEXTURE_2D);
      

  for (uint i = 0; i < m_nbFlames; i++){
    m_flames[i]->drawFlame(displayParticle);
//     ((LineFlame *)m_flames[i])->generateAndDrawSparks();
  }
  glPopMatrix();
}
  

CandleStick::CandleStick (FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index, 
			  CgSVShader * shader, double rayon):
  FireSource (flameConfig, s, 1, scene, filename, index, shader)
{
  m_flames[0] = new PointFlame(flameConfig, s, rayon);

  m_nbCloneFlames = 20;
  m_cloneFlames = new ClonePointFlame* [m_nbCloneFlames];
  
  m_cloneFlames[0]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(.5,0,0));
  m_cloneFlames[1]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-.5,0,0));
  m_cloneFlames[2]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.1,0,0.5));
  m_cloneFlames[3]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0,0,-0.5));
  m_cloneFlames[4]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(.5,0,0.5));
  m_cloneFlames[5]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-.5,0,0.4));
  m_cloneFlames[6]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.5,0,-0.5));
  m_cloneFlames[7]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-0.3,0,-0.5));
  m_cloneFlames[8]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1.1,0,0));
  m_cloneFlames[9]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-1,0,0));
  m_cloneFlames[10] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.1,0,1.1));
  m_cloneFlames[11] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-0.14,0,-1));
  m_cloneFlames[12]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1,0,1));
  m_cloneFlames[13]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-.9,0,1.3));
  m_cloneFlames[14] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1,0,-1));
  m_cloneFlames[15] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-1,0,-1));
  m_cloneFlames[16]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1.33,0,0));
  m_cloneFlames[17]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-1.5,0,0));
  m_cloneFlames[18] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0,0,1.4));
  m_cloneFlames[19] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0,0,-1.5));
}

CandleStick::~CandleStick()
{
  for (uint i = 0; i < m_nbCloneFlames; i++)
    delete m_cloneFlames[i];
  delete[]m_cloneFlames;
}

void CandleStick::build()
{
  Point averagePos, tmp;
  
  for (uint i = 0; i < m_nbFlames; i++){
    averagePos +=  m_flames[i]->getCenter ();
    m_flames[i]->build();
  }
  
  for (uint i = 0; i < m_nbCloneFlames; i++){
    averagePos += m_cloneFlames[i]->getCenter ();
    m_cloneFlames[i]->build();
  }
  
  averagePos = averagePos/(m_nbFlames+m_nbCloneFlames);
  averagePos += getPosition();
  setLightPosition(averagePos);
}

void CandleStick::toggleSmoothShading(void)
{
  FireSource::toggleSmoothShading();
  for (uint i = 0; i < m_nbCloneFlames; i++)
    m_cloneFlames[i]->toggleSmoothShading();
}

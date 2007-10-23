#ifndef GLFLAMECANVAS_H
#define GLFLAMECANVAS_H

class GLFlameCanvas;

#include "../common.hpp"
#include "interface.hpp"

#include <wx/glcanvas.h>

#include "../shaders/glsl.hpp"
#include "../scene/gammaEngine.hpp"

#include "../scene/camera.hpp"
#include "../scene/scene.hpp"
#include "../scene/graphicsFn.hpp"

#include "../flames/abstractFires.hpp"
#include "../flames/glowengine.hpp"
#include "../flames/DPengine.hpp"

#include "../solvers/fieldThread.hpp"
#include "../solvers/globalField.hpp"
#include "../flames/solidePhoto.hpp"

class PhotometricSolidsRenderer;

class GLFlameCanvas : public wxGLCanvas
{
public:
  GLFlameCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList = 0,  
		long style=0, const wxString& name=_("GLCanvas"), const wxPalette& palette = wxNullPalette);
  
  ~GLFlameCanvas();
  
  void OnIdle(wxIdleEvent& event);
  void OnPaint(wxPaintEvent& event);
  void drawScene(void);
  void drawFlames(void);
  void drawFlamesBoundingBoxes(void);
  
  /** Défini l'action à effectuer lorsque la souris se déplace */
  void OnMouseMotion(wxMouseEvent& event);
  /** Défini l'action à effectuer lorsqu'un bouton de la souris est enfoncé */
  void OnMouseClick(wxMouseEvent& event);
  void OnMouseWheel(wxMouseEvent& event);
  void OnKeyPressed(wxKeyEvent& event);
  void OnSize(wxSizeEvent& event);
  
  /** Initialisations relatives à l'environnement OpenGL */
  void InitGL();
  /** Initialisations des luminaires, qui eux-mêmes créent les champs de vélocité et les flammes. */
  void InitLuminaries(void);
  /** Initialisations relatives à la scène */
  void InitScene();
#ifdef MULTITHREADS
  /** Initialisations relatives aux solveurs */
  void InitThreads(void);
#endif
  /** Initialisations relatives aux paramètres de visualisation */
  void InitUISettings(void);
  void Restart (void);
  void ReloadFieldsAndFires (void);
  void DestroyScene(void);
  /** Initialisation globale du contrôle */
  void Init(FlameAppConfig *config);
  
  bool IsRunning(void) { return m_run; };
  /** Lance/arrête l'animation */
  void setRunningState(bool run) { 
    m_run=run;
    if(!m_run) PauseThreads();
    if(m_run) ResumeThreads();
  };
  
  /** Active/Désactive le glow seul */
  void ToggleGlowOnlyDisplay(void) { m_glowOnly=!m_glowOnly; };
  void ToggleGridDisplay(void) { m_displayGrid=!m_displayGrid; };
  void ToggleBaseDisplay(void) { m_displayBase=!m_displayBase; };
  void ToggleVelocityDisplay(void) { m_displayVelocity=!m_displayVelocity; };
  void ToggleParticlesDisplay(void) { m_displayParticles=!m_displayParticles; };
  void ToggleWickBoxesDisplay(void) { m_displayWickBoxes=!m_displayWickBoxes; };
  void ToggleFlamesDisplay(void) { m_displayFlame=!m_displayFlame; };
  void ToggleShadowVolumesDisplay(void) { m_drawShadowVolumes=!m_drawShadowVolumes; };
  void setSmoothShading(bool state) {
    for (vector < FireSource* >::iterator firesIterator = m_fires.begin ();
	 firesIterator != m_fires.end (); firesIterator++)
      (*firesIterator)->setSmoothShading (state);
  };
  void ToggleSaveImages(void) { m_saveImages = !m_saveImages; };
  void moveLuminary(int selected, Point& pt){ 
    /* On ne peut déplacer que les solveurs locaux */
    m_luminaries[selected]->move(pt);
    for (vector < FireSource* >::iterator firesIterator = m_fires.begin ();
	 firesIterator != m_fires.end (); firesIterator++)
      (*firesIterator)->computeVisibility(*m_camera,true);
  };
  void addPermanentExternalForcesToField(int selectedField, Point &pt){ 
    if(selectedField >= 0) m_fields[selectedField]->addPermanentExternalForces(pt);
    else m_globalField->addPermanentExternalForces(pt);
  };
  void setFieldBuoyancy(int index, float value){ 
    if(index >= 0) m_fields[index]->setBuoyancy(value);
    else m_globalField->setBuoyancy(value);
  };
  void setFlameForces(int index, float value){ m_fires[index]->setInnerForce(value); };
  void setFlameIntensity(int index, float value){ m_fires[index]->setIntensityCoef(value); };
  void setFlameLOD(int index, u_char value){ m_fires[index]->setLOD(value); };
  
  void setLuminaryBuoyancy(int index, float value){ 
    if(index >= 0) m_luminaries[index]->setBuoyancy(value);
    else m_globalField->setBuoyancy(value);
  };
  void setLuminaryVorticity(int index, float value){ 
    if(index >= 0) m_luminaries[index]->setVorticity(value);
    else m_globalField->setVorticity(value);
  };
  void setLuminaryForces(int index, float value){ m_luminaries[index]->setInnerForce(value); };
  void setLuminaryLOD(int index, u_char value){ m_luminaries[index]->setLOD(value); };
  void setLuminaryFDF(int index, int value) { m_luminaries[index]->setFDF(value); };
  void setLuminaryPerturbateMode(int index, char value) { m_luminaries[index]->setPerturbateMode(value); };
  void setLuminaryLeadLifeSpan(int index, uint value) {m_luminaries[index]->setLeadLifeSpan(value); };  
  void setLuminaryPeriLifeSpan(int index, uint value) { m_luminaries[index]->setPeriLifeSpan(value); };
  
  void setNbDepthPeelingLayers(uint value){ m_depthPeelingEngine->setNbLayers(value); };
  void RegeneratePhotometricSolids(uint flameIndex, wxString IESFileName);
  
  /** Change l'affichage des sphères englobantes. */
  void setBoundingSphereMode(bool mode) { m_scene->setBoundingSphereMode(mode); };
  void setBoundingVolumesDisplay(u_char display) { m_displayFlamesBoundingVolumes = display; };
  void setGammaCorrection(float gamma) { m_gammaEngine->SetGamma(gamma); };
  void setGammaCorrectionState(bool state) { m_gammaCorrection=state; };
  void computeGlowWeights(uint index, float sigma) { m_glowEngine->computeWeights(index, sigma); };

#ifdef MULTITHREADS
  void DeleteThreads();
#endif
private:
  void WriteFPS ();
  void DrawVelocity (void);
  void PauseThreads();
  void ResumeThreads();
  
//   void cast_shadows_float_multiple();
  void castShadows();
  
  /** Configuration de l'application */
  FlameAppConfig *m_currentConfig;
  /********* Variables relatives au contrôle de l'affichage **************/
  /* true si la simulation est en cours, 0 sinon */
  bool m_run, m_saveImages;
  bool m_displayVelocity, m_displayBase, m_displayGrid, m_displayFlame, m_displayParticles, m_displayWickBoxes;
  bool m_drawShadowVolumes, m_glowOnly, m_gammaCorrection, m_fullscreen;
  u_char m_displayFlamesBoundingVolumes;
  /** true si l'application est correctement initialisée, false sinon */
  bool m_init;

  /********* Variables relatives à la fenêtre d'affichage ****************/
  uint m_width, m_height;
  uint prevNbFields, prevNbFlames;
  
  Camera *m_camera;
  /* Pour le compte des frames */
  uint m_framesCount, m_globalFramesCount;

  int m_t;
  
  /* Tableau de pixels pour la sauvegarde des images */
  u_char *m_pixels;

  /********* Variables relatives aux solides photométriques **************/
  PhotometricSolidsRenderer *m_photoSolid;
  PixelLightingRenderer *m_pixelLighting;

  /********* Variables relatives au glow *********************************/
  GlowEngine *m_glowEngine;
  
  DepthPeelingEngine *m_depthPeelingEngine;
  
  vector <Luminary *> m_luminaries;
  /** Liste de threads. */
  list <FieldThread *> m_threads;
  /** Ordonnanceur des threads */
  FieldThreadsScheduler *m_scheduler;
  vector <Field3D *> m_fields;
  GlobalField *m_globalField;
    
  /********* Variables relatives à la simulation *************************/
  vector <FireSource *> m_fires;
  Scene *m_scene;
  GLSLProgram *m_SVProgram;
  GLSLVertexShader *m_SVShader;
  GammaEngine *m_gammaEngine;
  wxStopWatch *m_swatch;
  
  float *m_intensities;
  bool m_visibility;

  const static int m_nbIterFlickering = 20;
  DECLARE_EVENT_TABLE()
};

inline void GLFlameCanvas::drawFlames(void)
{
  /* Dessin de la flamme */
#ifdef MULTITHREADS
  for (list < FieldThread* >::iterator threadIterator = m_threads.begin ();
       threadIterator != m_threads.end (); threadIterator++)
    {
      (*threadIterator)->Lock();
      (*threadIterator)->drawFlames(m_displayFlame, m_displayParticles, m_displayFlamesBoundingVolumes);
      (*threadIterator)->Unlock();
    }
#else
  for (vector < FireSource* >::iterator firesIterator = m_fires.begin ();
       firesIterator != m_fires.end (); firesIterator++)
    (*firesIterator)->drawFlame(m_displayFlame, m_displayParticles, m_displayFlamesBoundingVolumes);
#endif
}

inline void GLFlameCanvas::drawFlamesBoundingBoxes(void)
{
  for (vector < FireSource* >::iterator firesIterator = m_fires.begin ();
       firesIterator != m_fires.end (); firesIterator++)
    (*firesIterator)->drawImpostor ();
}

#endif

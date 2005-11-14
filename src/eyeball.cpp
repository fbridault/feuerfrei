#include "eyeball.hpp"

#include "trackball.h"

void
normalisationd (GLdouble * x, GLdouble * y, GLdouble * z)
{
  GLdouble norm;

  norm = sqrt ((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
  (*x) /= norm;
  (*y) /= norm;
  (*z) /= norm;
}

Eyeball::Eyeball (int w, int h, double clipping)
{
  W = w;
  H = h;

  clipping_value = clipping;

  depl = FALSE;
  depl_rapide = TRUE;
  minx = miny = minz = -1.0;
  maxx = maxy = maxz = 1.0;

  trackball (curquat, 0.0, 0.0, 0.0, 0.0);

  /* ouverture de la pyramide de vision */
  ouverture = 60.0;

  /* calcul du vecteur UP en fonction du pt de vue et de la dir de visee */
  centerx = (GLdouble) (minx > 0 ? (maxx - minx) : (maxx + minx)) / 2.0;
  centery = (GLdouble) (miny > 0 ? (maxy - miny) : (maxy + miny)) / 2.0;
  centerz = (GLdouble) (minz > 0 ? (maxz - minz) : (maxz + minz)) / 2.0;

  taillex = (minx > 0 ? (maxx - minx) : (maxx + minx));
  zoomstep = (taillex != 0.0 ? taillex : 1.0) / 50.0;

  /* vecteur pointant vers le haut */
  upx = 0.0;
  upy = 1.0;
  upz = 0.0;

  /* point de vue */
  eyex = centerx;
  eyey = centery;
  eyez = 8.0 * (centerz != 0.0 ? centerz : 0.25);

  /* initialisation et normalisation du vecteur de visee */
  vectviseex = centerx - eyex;
  vectviseey = centery - eyey;
  vectviseez = centerz - eyez;
  normalisationd (&vectviseex, &vectviseey, &vectviseez);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (ouverture, 1.0, 0.1, clipping_value);

  /* initialisation du deplacement trackball */
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  gluLookAt (eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
  glPushMatrix ();
  glLoadIdentity ();
}

void
Eyeball::recalcModelView (void)
{
  GLdouble m[4][4];

  glPopMatrix ();
  glPushMatrix ();
  build_rotmatrix (m, curquat);
  glTranslatef (centerx, centery, centerz);
  /*glTranslatef(eyex, eyey, eyez); */
  glMultMatrixd (&m[0][0]);
  glTranslatef (-centerx, -centery, -centerz);
  /*glTranslatef(-eyex, -eyey, -eyez); */
}

void Eyeball::OnMouseClick (wxMouseEvent& event)
{
  bouton_active = event.GetButton();
  beginx = event.GetX();
  beginy = event.GetY();

  if (depl_rapide)
    if (event.ButtonDown())
      depl = TRUE;
    else
      depl = FALSE;
}

void Eyeball::OnMouseMotion (wxMouseEvent& event)
{
  if (!depl)
    return;
  if (bouton_active == wxMOUSE_BTN_RIGHT)
    {
      trackball (lastquat,
		 (2.0 * beginx - W) / W,
		 (H - 2.0 * beginy) / H,
		 (COEFFICIENT_ROTATION_QUATERNION * 2.0 * event.GetX() - W) / W,
		 (H - COEFFICIENT_ROTATION_QUATERNION * 2.0 * event.GetY()) / H);
      add_quats (lastquat, curquat, curquat);
    }
  else if (bouton_active == wxMOUSE_BTN_MIDDLE)
    {
      eyez += (event.GetY() - beginy) * zoomstep;
      if (eyez < centerz)
	eyez = centerz + (double) 1.0;
      //if(eyez > 5.0 * taillex) eyez = 5.0 * taillex;
      recalculer_matrice_initiale ();
    }
  beginx = event.GetX();
  beginy = event.GetY();
}

void
Eyeball::recalculer_matrice_initiale (void)
{
  glPopMatrix ();
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (ouverture, 1.0, 0.1, clipping_value);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  gluLookAt (eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
  glPushMatrix ();
}

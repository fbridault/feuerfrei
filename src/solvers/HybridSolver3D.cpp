#include "HybridSolver3D.hpp"
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

/* Le constructeur de GSSolver3D n'a pas de paramètre, il n'est donc pas appelé explicitement */
HybridSolver3D::HybridSolver3D (CTransform& a_rTransform, uint n_x, uint n_y, uint n_z, float dim,
				float timeStep, float buoyancy, float vorticityConfinement, float omegaDiff,
				float omegaProj, float epsilon) :
  Solver3D (a_rTransform, n_x, n_y, n_z, dim, timeStep, buoyancy, vorticityConfinement),
  GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
}
/* Le constructeur de GSSolver3D n'a pas de paramètre, il n'est donc pas appelé explicitement */
HybridSolver3D::HybridSolver3D (float omegaDiff, float omegaProj, float epsilon) :
  GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
}

HybridSolver3D::~HybridSolver3D ()
{
}

/* Pas de diffusion */
void HybridSolver3D::diffuse (unsigned char b, float *const x, float *const x0, float a)
{
  GSSolver3D::diffuse (b, x, x0, a);
}

void HybridSolver3D::project (float *const p, float *const div)
{
  GCSSORSolver3D::project(p,div);
}
